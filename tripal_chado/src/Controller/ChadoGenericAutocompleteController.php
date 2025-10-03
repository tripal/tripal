<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\pgsql\Driver\Database\pgsql\Select;
use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Controller, Chado Generic Autocomplete.
 */
class ChadoGenericAutocompleteController extends ControllerBase {

  /**
   * Contains default values for options needed for this controller's methods.
   *
   * These options are set when ::getDefaultOptions() is called.
   *
   * @var array
   *   Populated by its getter, this array will contain the default values for
   *   the options used in the various form methods.
   *   Keys match those defined for the ::getFormElement() $options paramater.
   */
  protected static array $default_options = [];

  /**
   * Retrieves the default options used by the form element methods.
   *
   * @return array
   *   The default values for the options used in the various form methods.
   *   Keys match those defined for the ::getFormElement() $options paramater.
   */
  public static function getDefaultOptions(): array {

    // We can use static in-memory caching to save grabbing from settings
    // multiple times in a single page load.
    if (!empty(static::$default_options)) {
      return static::$default_options;
    }

    // If the options have not been set yet, then we need to grab them from
    // the tripal config settings.
    $settings = \Drupal::config('tripal.settings');
    $default_options = [
      'select_limit' => $settings->get('tripal_entity_type.widget_global_select_limit') ?? 50,
      'match_limit' => $settings->get('tripal_entity_type.match_limit') ?? 10,
      'match_operator' => $settings->get('tripal_entity_type.match_operator') ?? 'CONTAINS',
      'size' => $settings->get('tripal_entity_type.size'),
      'placeholder' => $settings->get('tripal_entity_type.placeholder'),
    ];
    static::$default_options = $default_options;

    return $default_options;
  }

  /**
   * Indicates if the primary key should be added to the response.
   *
   * Used by derived classes.
   *
   * @var bool
   *   If TRUE then the primary key is added to the end of the response string
   *   within curved brackets. If FALSE then it is not.
   */
  protected bool $include_pkey = TRUE;

  /**
   * Controls whether a leading wildcard character is included.
   *
   * Used by derived classes.
   *
   * @var string
   *   Must be one of the following:
   *   - CONTAINS: a leading wildcard character is added to the query allowing
   *     an internal string match.
   *   - STARTS_WITH: no leading wildcard character is added to the query,
   *     requiring the beginning of the string to match.
   */
  protected string $match_operator = 'CONTAINS';

  /**
   * Generic Chado autocomplete method.
   *
   * NOTE: This is suitable to use for any chado table where the returned value
   * is from a single column.
   *
   * For simplicity, this autocomplete assumes that the primary key
   * of the base table is the base table name + '_id', and that the
   * type columnn is named 'type_id'.
   *
   * To support columns without a unique constraint, the returned
   * autocomplete value includes the primary key numeric value in
   * parentheses at the end, e.g. "Impressive Publication (42)".
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   Represents the current HTTP request.
   * @param string $base_table
   *   Chado base table name.
   * @param string $column_name
   *   Name of chado base column in the specified table to be returned.
   * @param string $type_column
   *   If the base table has a type column, the column name. This is
   *   usually "type_id". Use a single character placeholder if absent.
   * @param string $property_table
   *   Property table name, use same name as base table if not needed.
   * @param int $match_limit
   *   Desired number of matching names to suggest. See ::getDefaultOptions()
   *   for the default value. If set to zero, then autocomplete is disabled.
   *   Define in autocomplete route parameter e.g. ['match_limit' => 15].
   * @param int $type_id
   *   Restricts the results returned to a specific type. This must be a value
   *   present in the $type_column and also present in the chado cvterm table.
   *   Set to 0 in order to not restrict to a specific type.
   *   Define in autocomplete route parameter e.g. ['type_id' => 0].
   *
   * @return Symfony\Component\HttpFoundation\JsonResponse
   *   Matching table results in an array where the value of the $column_name
   *   column is both the key and the value. If $type_id is provided, the type
   *   name is added to the end of the key/value surrounded by square brackets.
   *   If the include_pkey property is TRUE then the primary key value is
   *   appended to the end of both the key and value surrounded by curved
   *   brackets, e.g. 'ftbA-1 [gene] (42)'.
   */
  public function handleGenericAutocomplete(
    Request $request,
    string $base_table,
    string $column_name,
    string $type_column,
    string $property_table,
    int $match_limit = 10,
    int $type_id = 0,
  ) {

    // Array to hold matching records.
    $response = [];

    // The string to autocomplete from the form input.
    $string = trim($request->query->get('q'));

    // Generate a database query.
    $options = [
      'base_table' => $base_table,
      'column_name' => $column_name,
      'type_column' => $type_column,
      'property_table' => $property_table,
      'match_limit' => $match_limit,
      'type_id' => $type_id,
      'match_operator' => $this->match_operator,
    ];
    $query = self::getQuery($string, $options);

    if ($query) {
      // Perform the database query.
      $results = $query->execute();

      // Compose the response.
      if ($results) {
        while ($record = $results->fetchObject()) {
          // Strip HTML tags if present, e.g. in Pub title.
          $value = strip_tags($record->value);
          // Append the type when available.
          if (property_exists($record, 'type') and $record->type) {
            $value .= ' [' . $record->type . ']';
          }
          // Append the chado pkey id value.
          if ($this->include_pkey) {
            $value .= ' (' . $record->pkey . ')';
          }
          $response[] = [
            // Value returned and value displayed by textfield.
            'value' => $value,
            // Value shown in the list of options.
            'label' => $value,
          ];
        }
      }
    }

    return new JsonResponse($response);
  }

  /**
   * Returns a database query ready to execute.
   *
   * This allows the same query to be used for both autocomplete and select.
   *
   * @param string $string
   *   The string to be autocompleted, used to limit the query.
   *   The string "%" has the special meaning of return all records.
   * @param array $options
   *   The following keys are used:
   *   - base_table (string): Chado base table name (required; e.g. pub).
   *   - column_name (string): Name of chado column to be returned (e.g. title).
   *   - pkey_id (string): The primary key of the base table, defaults to
   *     base_table + "_id" (e.g. pub_id)
   *   - type_column (string): If the base table has a type column, holds the
   *     column name. This is usually "type_id". Use a single character
   *     placeholder if absent (e.g. "-").
   *   - property_table (string): the name of the property table name associated
   *     with this base table. If the type is stored in the base table then use
   *     same name as base table here. (e.g. pubprop)
   *   - property_type_column (string): type column in property table,
   *     defaults to "type_id". This is only used if the property_table is not
   *     the same as the base table.
   *   - type_id (int): Used to restrict records to those of a specific type.
   *     Set to 0 in order to not restrict to a specific type.
   *     Define in autocomplete route parameter e.g. ['type_id' => 0].
   *   - match_operator (string): Either 'CONTAINS' (default) or 'STARTS_WITH'.
   *   - match_limit (int): Desired number of autocomplete matching names to
   *     suggest, default 10. If zero, there is no limit.
   *
   * @return ?\Drupal\pgsql\Driver\Database\pgsql\Select
   *   A database query object
   */
  public static function getQuery(string $string, array $options): ?Select {

    // Set defaults.
    $options['pkey_id'] ??= $options['base_table'] . '_id';
    $options['type_column'] ??= '.';
    $options['property_table'] ??= $options['base_table'];
    $options['property_type_column'] ??= 'type_id';
    $options['type_id'] ??= 0;
    $options['match_operator'] ??= 'CONTAINS';
    $options['match_limit'] ??= 10;

    $query = NULL;
    // Generate a query only if $string is at least a character
    // long and result count is set to a value greater than 0.
    if (strlen($string) > 0 && $options['match_limit'] > 0) {

      $connection = \Drupal::service('tripal_chado.database');

      // Sanitization, placeholders cannot be used for column and table names.
      $base_table = $connection->escapeTable($options['base_table']);
      $pkey_id = $connection->escapeTable($options['pkey_id']);
      $column_name = $connection->escapeTable($options['column_name']);
      $type_column = $connection->escapeTable($options['type_column']);
      $property_table = $connection->escapeTable($options['property_table']);
      $property_type_column = $connection->escapeTable($options['property_type_column']);

      // Transform string into a search pattern with wildcards.
      // Only STARTS_WITH and CONTAINS are supported.
      $condition_value = $string . '%';
      if ($options['match_operator'] == 'CONTAINS') {
        $condition_value = '%' . $condition_value;
      }
      $query = $connection->select('1:' . $base_table, 'BT');
      $query->addField('BT', $options['pkey_id'], 'pkey');
      $query->addField('BT', $column_name, 'value');
      // A single "%" wildcard is used to indicate that we should return
      // all records. This is used for a form select element.
      if ($string != '%') {
        $query->condition('BT.' . $column_name, $condition_value, 'ILIKE');
      }
      $query->orderBy('BT.' . $column_name, 'ASC');
      if ($options['match_limit']) {
        $query->range(0, $options['match_limit']);
      }

      if ($options['type_id'] > 0) {
        // We are limiting to records of a specific type.
        if ($property_table and ($property_table != $base_table)) {
          $query->leftJoin('1:' . $property_table, 'PT', '"BT".' . $options['pkey_id'] . ' = "PT".' . $options['pkey_id']);
          $query->condition('PT.' . $options['property_type_column'], $options['type_id'], '=');
        }
        else {
          $query->condition('BT.' . $type_column, $options['type_id'], '=');
        }
      }
      else {
        // There was no limiting by type, so lookup the actual type
        // if it is present in the base table.
        if (strlen($options['type_column']) > 1) {
          $query->addField('CVT', 'name', 'type');
          $query->leftJoin('1:cvterm', 'CVT', '"BT".' . $type_column . ' = "CVT".cvterm_id');
        }
      }
    }
    return $query;
  }

  /**
   * Fetch the pkey id number from an autocomplete value.
   *
   * Assumes the primary key value is present at the end of the string in
   * curved brackets.
   *
   * @param string $value
   *   A value from an autocomplete with the ID in parentheses at the end,
   *   earlier parentheses are ignored, e.g. "Some (Big) Analysis (12)".
   *
   * @return int
   *   Primary key ID number of the record, or 0 if an unparsable $value was
   *   passed, which can happen if the user did not let the autocomplete
   *   supply a value, (e.g. 12).
   */
  public static function getPkeyId(string $value): int {
    $id = 0;

    $matches = [];
    if (preg_match('/\((\d+)\)/', $value, $matches)) {
      $id = $matches[array_key_last($matches)];
    }

    return $id;
  }

}
