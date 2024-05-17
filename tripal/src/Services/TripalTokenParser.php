<?php
namespace Drupal\tripal\Services;

use \Drupal\tripal\Entity\TripalEntity;
use \Drupal\tripal\Entity\TripalEntityType;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use Symfony\Component\Validator\Constraints\IsNull;

class TripalTokenParser {

  /**
   * The content type object.
   * @var \Drupal\tripal\Entity\TripalEntityType $bundle
   */
  protected $bundle = NULL;

  /**
   *
   * @var \Drupal\tripal\Entity\TripalEntity $entity
   */
  protected $entity = NULL;

  /**
   * An array of field instances.
   *
   * @var array $fields
   */
  protected $fields = [];

  /**
   * An array of field values indexed first by field name then by property key.
   *
   * @var array $values.
   */
  protected $values = [];

  /**
   * Returns bundle object given to the parser.
   *
   * @return \Drupal\tripal\Entity\TripalEntityType
   */
  public function getBunde() {
    return $this->bundle;
  }

  /**
   * Returns the array of values given to the parser.
   * @return array
   */
  public function getValues() {
    return $this->values;
  }

  /**
   * Returns the entity given to the parser.
   *
   * @return \Drupal\tripal\Entity\TripalEntity
   */
  public function getEntity() {
    return $this->entity;
  }

  /**
   * Returns the names of the fields that have been added.
   *
   * @return array
   */
  public function getFieldNames() {
    return array_keys($this->fields);
  }

  /**
   *
   * @param TripalEntity $entity
   */
  public function setEntity(TripalEntity $entity) {
    if ($entity->getType() != $this->bundle->getId()) {
      throw new \Exception(t('TripalTokenParser: The entity provided is not of the same type as the bundle'));
    }

    $this->entity = $entity;
  }


  /**
   * Initializes the token parser service
   *
   * The content type or bundle Id.
   * @param string \Drupal\tripal\Entity\TripalEntityType $bundle
   */
  public function initParser(TripalEntityType $bundle, TripalEntity $entity = NULL) {
    $this->bundle = $bundle;
    if ($entity) {
      $this->setEntity($entity);
    }

    // Get the field manager, field definitions for the bundle type, and
    // the field type manager.
    /** @var \Drupal\Core\Entity\EntityFieldManager $field_manager **/
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $bundle->getID());
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    // Iterate over the field definitions for the bundle and create a field instance.
    /** @var \Drupal\Core\Field\BaseFieldDefinition $field_definition **/
    $field_definition = NULL;
    foreach ($field_defs as $field_name => $field_definition) {
      if (!empty($field_definition->getTargetBundle())) {
        $configuration = [
          'field_definition' => $field_definition,
          'name' => $field_name,
          'parent' => NULL,
        ];
        $field = $field_type_manager->createInstance($field_definition->getType(), $configuration);
        $this->fields[$field_name] = $field;
      }
    }
  }

  /**
   * Adds the field values that should be used for replacement.
   *
   * @param string $field_name
   *   The name of the field that the value belongs to
   * @param StoragePropertyValue $value
   *   The property values
   */
  public function addFieldValue($field_name, string $key, $value){
    // Special case handling, fields set to one of the HTML formats
    // will be stored wrapped in paragraph tags <p></p>.
    // These are disruptive to the title, so remove them.
    // Any other HTML markup is allowed, e.g. <em> <strong>
    $value = preg_replace('/^<p>(.*)<\/p>[\r\n]*$/', '$1', $value);
    $this->values[$field_name][$key] = $value;
  }

  /**
   * Replaces the tokens with field values within the provided strings.
   *
   * @param array $tokenized_strings
   *   An array of strings with field names as tokens.  Field name should be
   *   surrounded by square brackets.
   *
   * @return array
   *   An array with all of the strings from the input $tokenized_strings array
   *   but with field tokens replaced with appropriate values.
   */
  public function replaceTokens(array $tokenized_strings) {

    $replaced = $tokenized_strings;
    foreach ($tokenized_strings as $index => $tokenized_string) {
      $value = NULL;

      // Get the tokens in the string.
      $tokens = [];
      $matches = [];
      if (preg_match_all('/\[.*?\]/', $tokenized_string, $matches)) {
        $tokens = $matches[0];
      }
      foreach ($tokens as $token) {
        $token = preg_replace('/\[/', '', $token);
        $token = preg_replace('/\]/', '', $token);

        // Look for values for bundle or entity related tokens.
        if ($token === 'TripalBundle__bundle_id') {
          $value = $this->bundle->getID();
          $replaced[$index] = trim(preg_replace("/\[$token\]/", $value,  $replaced[$index]));
        }
        elseif ($token == 'TripalEntityType__label') {
          $value = $this->bundle->getLabel();
          $replaced[$index] = trim(preg_replace("/\[$token\]/", $value,  $replaced[$index]));
        }
        elseif ($token === 'TripalEntity__entity_id' and !is_null($this->entity)) {
          $value = $this->entity->getID();
          $replaced[$index] = trim(preg_replace("/\[$token\]/", $value,  $replaced[$index]));
        }
        // Look for values for field related tokens
        elseif (in_array($token, array_keys($this->fields))) {
          $field = $this->fields[$token];
          $key = $field->mainPropertyName();
          if (array_key_exists($token, $this->values)) {
            $value = $this->values[$token][$key];
            if (!is_null($value)) {
              $replaced[$index] = trim(preg_replace("/\[$token\]/", $value,  $replaced[$index]));
            }
          }
        }
      }
    }

    return $replaced;
  }
}
