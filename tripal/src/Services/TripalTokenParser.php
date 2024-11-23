<?php
namespace Drupal\tripal\Services;

use \Drupal\tripal\Entity\TripalEntity;
use \Drupal\tripal\Entity\TripalEntityType;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;


/**
 * The TripalTokenParser class replaces tokens in strings with a
 * corresponding value. Tokens are marked by square brackets, and
 * the square brackets may be 1 or 2 levels deep. This allows
 * having a prefix or suffix that is only included when the token
 * has a defined value.
 *
 * A simple token string example:
 *   "The title is [Title]."
 * An example of a token that may or may not be defined "Issue":
 *   "[Journal]. [Volume][([Issue])]"
 * If defined the replaced string could be "Science. 123(45)"
 * but if Issue is not defined then the parentheses are not
 * included "Science. 123"
 *
 * In some cases the token may have several possible values,
 * and this can be designated by a pipe "|" delimited list.
 * For example:
 * "[ [Journal Name|Series Name|Journal Abbreviation].]"
 * The first of these that has a defined value will be used
 * for the substitution, or if none are defined, the token
 * is removed.
 *
 * Any prefix or suffix in a 2-level token is checked against
 * the token value, and if already present, is not added a
 * second time. For example, for a token "[[Title].]"
 * and a title value of "Cool Science.", then the replaced
 * value will not include both periods, only one is retained.
 */
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
  public static function deprecatedgetEntityTitle(TripalEntityType $bundle, array $entity_values) {

    // Initialize the Tripal token parser service.
    /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
    $token_parser = \Drupal::service('tripal.token_parser');
    $token_parser->initParser($bundle);
    $token_parser->clearValues();

    // Iterate through each field to add its property values to the token parser.
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
    $replaced = $token_parser->replaceEntityTokens([$title]);

    // Since this is a single entity, we return the only title.
    // Replace tokens returns an array to handle recursive situations.
    return $replaced[0];
  }

  /**
   * Returns bundle object given to the parser.
   *
   * @return \Drupal\tripal\Entity\TripalEntityType
   */
  public function deprecatedgetBundle() {
    return $this->bundle;
  }

  /**
   * Returns the array of values given to the parser.
   * @return array
   */
  public function deprecatedgetValues() {
    return $this->field_values;
  }

  /**
   * Empties the values saved for each token.
   *
   * This should be done between replacing tokens for different entities.
   */
  public function deprecatedclearValues() {
    $this->field_values = [];
  }

  /**
   * Returns the entity given to the parser.
   *
   * @return \Drupal\tripal\Entity\TripalEntity
   */
  public function deprecatedgetEntity() {
    return $this->entity;
  }

  /**
   * Returns the names of the fields that have been added.
   *
   * @return array
   */
  public function deprecatedgetFieldNames() {
    return array_keys($this->fields);
  }

  /**
   *
   * @param TripalEntity $entity
   */
  public function deprecatedsetEntity(TripalEntity $entity) {
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
  public function deprecatedinitParser(TripalEntityType $bundle, TripalEntity $entity = NULL) {
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
   * Replaces tokens within multiple tokenized strings with corresponding values.
   *
   * @param array $tokenized_strings
   *   An array of strings with tokens contained within one or two levels
   *   of square brackets. Multiple candidate tokens are separated by pipe
   *   character. For example [ [Journal Name|Journal Abbreviation].]
   *   In this latter case, parsing left to right, the first token that
   *   has a defined value will be used for replacement.
   * @param array $token_values
   *   Values to use for token replacement. Key is token name without
   *   square brackets, value is replacement value for the token.
   * @param bool $strict
   *   If FALSE (default), tokens with no defined value are removed.
   *   If TRUE, these tokens are left in the returned string
   *
   * @return array
   *   An array with all of the strings from the input $tokenized_strings array
   *   but with tokens replaced with appropriate values, or removed if no token
   *   value is availiable.
   */
  public function replaceTokensArray(array $tokenized_strings, array $token_values, bool $strict = FALSE) {

    $replaced_strings = [];
    foreach ($tokenized_strings as $index => $tokenized_string) {
      $replaced_strings[$index] = $this->replaceTokens($tokenized_string, $token_values);
    }
    return $replaced_strings;
  }

  /**
   * Replaces tokens within a single tokenized string with corresponding values.
   *
   * @param string $tokenized_string
   *   A string with tokens contained within one or two levels
   *   of square brackets. Multiple candidate tokens are separated by pipe
   *   character. For example [ [Journal Name|Journal Abbreviation].]
   *   In this latter case, parsing left to right, the first token that
   *   has a defined value will be used for replacement.
   * @param array $token_values
   *   Values to use for token replacement. Key is token name without
   *   square brackets, value is replacement value for the token.
   * @param bool $strict
   *   If FALSE (default), tokens with no defined value are removed.
   *   If TRUE, these tokens are left in the returned string
   *
   * @return string
   *   The passed $tokenized_string with tokens replaced with
   *   corresponding values, or the token is removed if no token
   *   replacement value is defined.
   */
  public function replaceTokens(string $tokenized_string, array $token_values, bool $strict = FALSE) {

    // Match double tokens e.g. [([issue])] or [ [title|name].]
    if (preg_match_all('/\[[^\[\]]*\[[^\]]+\][^\]]*\]/', $tokenized_string, $matches)) {
      foreach ($matches[0] as $token_string) {
        // separate into prefix, key, suffix
        preg_match('/\[([^\[\]]*)\[([^\]]+)\]([^\]]*)\]/', $token_string, $submatches);
        $prefix = $submatches[1];
        $key = $this->firstMatchedToken($submatches[2], $token_values);
        $suffix = $submatches[3];
        // $key could be an empty string here, in which case there is no value
        $value = $token_values[$key] ?? '';
        // If prefix or suffix are already part of the ends of the value string,
        // then omit them, e.g. title already ends in a period and token is "{ {Title}.}"
        $full_value = '';
        if (strlen($value)) {
          if (strlen($prefix) and substr($value, 0, strlen($prefix)) == $prefix) {
            $prefix = '';
          }
          if (strlen($suffix) and substr($value, -strlen($suffix)) == $suffix) {
            $suffix = '';
          }
          $full_value = $prefix . $value . $suffix;
        }
        if (strlen($full_value) or !$strict) {
          $tokenized_string = str_replace($token_string, $full_value, $tokenized_string);
        }
      }
    }

    // Match any remaining single tokens, e.g. [organism_genus]
    if (preg_match_all('/\[[^\[\]]+\]/', $tokenized_string, $matches)) {
      foreach ($matches[0] as $token_string) {
        $key = substr($token_string, 1, strlen($token_string)-2);
        $key = $this->firstMatchedToken($key, $token_values);
        $value = $token_values[$key] ?? '';
        if (strlen($value) or !$strict) {
          $tokenized_string = str_replace($token_string, $value, $tokenized_string);
        }
      }
    }
    return $tokenized_string;
  }

  /**
   * Determine the first token in a '|' delimited string of tokens
   * that has defined value in the $values array.
   *
   * @param string $token_string
   *   One or more tokens delimited by "|"
   * @param array $values
   *   Associative array of key value pairs where keys correspond to the tokens.
   *
   * @return string
   *   The first matching token that is present as a key in $values
   *   and is not null. Returns an empty string if none of the
   *   tokens have defined values.
   */
  protected function firstMatchedToken(string $token_string, array $values): string {
    $tokens = explode('|', $token_string);
    foreach ($tokens as $token) {
      if (array_key_exists($token, $values) and !is_null($values[$token])) {
        return $token;
      }
    }
    return '';
  }

}
