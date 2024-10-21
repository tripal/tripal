<?php
namespace Drupal\tripal\Services;

use \Drupal\tripal\Entity\TripalEntity;
use \Drupal\tripal\Entity\TripalEntityType;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use Symfony\Component\Validator\Constraints\IsNull;

class TripalTokenParser {

  /**
   * The content type object.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType $bundle
   */
  protected $bundle = NULL;

  /**
   * A specific piece of content containing values.
   *
   * @var \Drupal\tripal\Entity\TripalEntity $entity
   */
  protected $entity = NULL;

  /**
   * An array of field instances for the bundle in $bundle.
   *
   * @var array $fields
   */
  protected $fields = [];

  /**
   * An array of field values indexed first by field name then by property key.
   *
   * @var array $field_values.
   */
  protected $field_values = [];

  /**
   * An array of token values where the key is the token name and the value is
   * it's value. The value must not be an object or array.
   *
   * @var array $token_values.
   */
  protected $token_values = [];

  /**
   * Uses this tokenparser to get the title of an entity based on its
   * bundle title format and the fields values in the entity.
   *
   * @param TripalEntityType $bundle
   *  The bundle for the entity whose title we want to generate.
   * @param array $entity_values
   *  The field values for the entity whom we want to generate the title for.
   *  This is a nested array with the first keys being field names. Within each
   *  array for a given field the keys are delta and the values are an array of
   *  the property names => values for that field delta.
   *
   * @return string
   *  The title format string with all the tokens replaced.
   */
  public static function getEntityTitle(TripalEntityType $bundle, array $entity_values) {

    // Initialize the Tripal token parser service.
    /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
    $token_parser = \Drupal::service('tripal.token_parser');
    $token_parser->initParser($bundle);
    $token_parser->clearValues();

    // Iterate through each field to add it's property values to the token parser.
    foreach ($entity_values as $field_name => $field_values) {
      // We currently only support single value fields so check for that here.
      if (sizeof($field_values) == 1) {
        // Grab the first and only delta for this field.
        $item = $field_values[0];
        // Iterate through the properties and add each to the token parser.
        foreach ($item as $property_name => $property_value) {
          $token_parser->addFieldValue(
            $field_name,
            $property_name,
            $property_value
          );
        }
      }
    }

    // Now that the token parser is set up, we can get the title by replacing
    // the tokens in the title format.
    $title = $bundle->getTitleFormat();
    $replaced = $token_parser->replaceTokens([$title]);

    // Since this is a single entity, we return the only title.
    // Replace tokens returns an array to handle recursive situations.
    return $replaced[0];
  }

  /**
   * Returns bundle object given to the parser.
   *
   * @return \Drupal\tripal\Entity\TripalEntityType
   */
  public function getBundle() {
    return $this->bundle;
  }

  /**
   * Returns the array of values given to the parser.
   * @return array
   */
  public function getValues() {
    return $this->field_values;
  }

  /**
   * Empties the values saved for each token.
   *
   * This should be done between replacing tokens for different entities.
   */
  public function clearValues() {
    $this->field_values = [];
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

    // Ensure there is no bleed through of values from previous substitutions.
    $this->clearValues();
  }

  /**
   * Given an entity, prepare the token => value mapping to support token replacement.
   *
   * Note: Replaces initParser when you have both bundle/entity (i.e. entity crud)
   *
   * @param TripalEntityType $bundle
   * @param TripalEntity $entity
   * @return void
   */
  public function processEntityValues(TripalEntityType $bundle, TripalEntity $entity) {
    $this->bundle = $bundle;
    if ($entity) {
      $this->setEntity($entity);
    }

    // Clear the values since we only parse for one entity at a time.
    $this->clearValues();


    // For each field attached to this entity...
    $field_defs = $entity->getFieldDefinitions();
    foreach ($field_defs as $field_name => $field_def) {
      $this->fields[$field_name] = $field_def;

      // Retrieve the items array.
      /** @var \Drupal\Core\Field\FieldItemList $items **/
      $items = $entity->get($field_name);
      // Token replacement only supports single-value fields...
      // therefore, only if the items array has a single value...
      if (count($items) === 1) {
        /** @var \Drupal\Core\Field\Plugin\Field\FieldType\EntityReferenceItem  $item **/
        $item = $items[0];
        // Now add the value for each property to the parser.
        /** @var \Drupal\Core\TypedData\TypedDataInterface $prop **/
        $props = $item->getProperties();
        if (is_array($props)) {
          foreach ($props as $prop) {
            $this->addFieldValue($field_name, $prop->getName(), $prop->getValue());
          }
        }
      }
    }

    // Ensure there is no bleed through of values from previous substitutions.
    $this->clearValues();
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

    // For field-based tokens we replace the token with the value of
    // the main property. Thus only set the token value if this is the
    // main property.
    if (array_key_exists($field_name, $this->fields) && method_exists($this->fields[$field_name], 'mainPropertyName')) {
      $field = $this->fields[$field_name];
      $main_prop_key = $field->mainPropertyName();
      if ($main_prop_key == $key) {
        $this->token_values[$field_name] = $value;
      }
    }
    // If we can't determine the main property but we do have a value then set
    // this directly as the token. This could cause unexpected bahaviour if a
    // multi-property field does not implement mainPropertyName()...
    // BUT this should only happen in a non-Tripal field.
    elseif (!empty($value)) {
      $this->token_values[$field_name] = $value;
    }
    else {
      $this->token_values[$field_name] = NULL;
    }

    // That said, we always set the field_values regardless of whether it is the
    // main property.
    // @todo in the future we should support property key specific tokens
    $this->field_values[$field_name][$key] = $value;
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
        elseif (in_array($token, array_keys($this->token_values))) {
          $value = $this->token_values[$token];
          if (!is_null($value)) {
            $replaced[$index] = trim(preg_replace("/\[$token\]/", $value,  $replaced[$index]));
          }
          else {
            // A token value may be missing either because there is no value, or
            // because there is a typo in the token. In any case, remove the token.
            // @todo add validation when editing title tokens to prevent the latter case.
            $replaced[$index] = trim(preg_replace("/\[$token\]/", '',  $replaced[$index]));
          }
        }
      }
    }

    return $replaced;
  }
}
