<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Controller, Chado Generic Autocomplete.
 */
class ChadoGenericAutocompleteController extends ControllerBase {

  /**
   * Controls whether the primary key is included in the response in
   * parentheses. Used by derived classes.
   */
  protected bool $include_pkey = TRUE;

  /**
   * Controls whether a leading wildcard character is included.
   * Used by derived classes.
   */
  protected bool $leading_wildcard = TRUE;

  /**
   * Controller autocomplete method to use for any chado table
   * where the returned value is from a single column.
   *
   * For simplicity, this autocomplete assumes that the primary key
   * of the base table is the base table name + '_id', and that the
   * type columnn is named 'type_id'.
   *
   * To support columns without a unique constraint, the returned
   * autocomplete value includes the primary key numeric value in
   * parentheses at the end, e.g. "Impressive Publication (42)".
   *
   * @param Request request
   *
   * @param string $base_table
   *   Chado base table name
   *
   * @param string $column_name
   *   Name of chado base column to be returned
   *
   * @param string $type_column
   *   If the base table has a type column, the column name. This is
   *   usually "type_id". Use a single character placeholder if absent.
   *
   * @param string $property_table
   *   Property table name, use same name as base table if not needed
   *
   * @param int $count
   *   Desired number of matching names to suggest.
   *   Default to 10 items.
   *   If set to zero, then autocomplete is disabled.
   *   Must be declared in autocomplete route parameter e.g. ['count' => 15].
   *
   * @param int $type_id
   *   Publication type set in pub.type_id to restrict publications to specific type.
   *   Default to 0, return publications regardless of type.
   *   Must be declared in autocomplete route parameter e.g. ['type_id' => 0].
   *
   * @return Json Object
   *   Matching publication rows in an array where pub title
   *   is both the value to the array keys label and value.
   */
  public function handleGenericAutocomplete(Request $request,
     string $base_table, string $column_name, string $type_column, string $property_table,
     int $count = 10, int $type_id = 0) {

    // Array to hold matching records.
    $response = [];

    // The string to autocomplete from the form input
    $string = trim($request->query->get('q'));

    // Generate a database query
    $options = [
      'base_table' => $base_table,
      'column_name' => $column_name,
      'type_column' => $type_column,
      'property_table' => $property_table,
      'count' => $count,
      'type_id' => $type_id,
      'leading_wildcard' => $this->leading_wildcard,
    ];
    $query = self::getQuery($string, $options);

    if ($query) {
      // Perform the database query
      $results = $query->execute();

      // Compose the response
      if ($results) {
        while ($record = $results->fetchObject()) {
          // Strip HTML tags if present, e.g. in Pub title
          $value = strip_tags($record->value);
          // Append the type when available
          if (property_exists($record, 'type')) {
            $value .= ' [' . $record->type . ']';
          }
          // Append the chado pkey id value
          if ($this->include_pkey) {
            $value .= ' (' . $record->pkey . ')';
          }
          $response[] = [
            'value' => $value, // Value returned and value displayed by textfield.
            'label' => $value, // Value shown in the list of options.
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
   *
   * @param array $options
   *   The following keys are used:
   *     base_table - Chado base table name (required)
   *     column_name - Name of chado base column to be returned (required)
   *     pkey_id - The primary key of the base table, defaults to base_table . "_id"
   *     type_column - If the base table has a type column, holds the column name. This is
   *                   usually "type_id". Use a single character placeholder if absent.
   *     property_table - Property table name, use same name as base table if not needed
   *     property_type_column - type column in property table, defaults to "type_id"
   *     count - Desired number of matching names to suggest.
   *             Default to 10 items. If set to zero, then autocomplete is disabled.
   *     type_id - Used to restrict records to those of a specific type.
   *               Default to 0, return records regardless of type.
   *               Must be declared in autocomplete route parameter e.g. ['type_id' => 0].
   *     leading_wildcard - Indicates if the query should include a "%" wildcard at
   *                        the beginning, defaults to TRUE.
   *
   * @return ?\Drupal\pgsql\Driver\Database\pgsql\Select $query
   *   A database query object
   */
  public static function getQuery(string $string, array $options): ?\Drupal\pgsql\Driver\Database\pgsql\Select {

    // Set defaults
    $options['type_column'] ??= '.';
    $options['property_table'] ??= $options['base_table'];
    $options['property_type_column'] ??= 'type_id';
    $options['pkey_id'] ??= $options['base_table'] . '_id';
    $options['count'] ??= 10;
    $options['type_id'] ??= 0;
    $options['leading_wildcard'] ??= TRUE;

    $query = NULL;
    // Generate a query only if string is at least a character
    // long and result count is set to a value greater than 0.
    if (strlen($string) > 0 && $options['count'] > 0) {

      $connection = \Drupal::service('tripal_chado.database');

      // Sanitization, placeholders cannot be used for column and table names
      $base_table = $connection->escapeTable($options['base_table']);
      $pkey_id = $connection->escapeTable($options['pkey_id']);
      $column_name = $connection->escapeTable($options['column_name']);
      $type_column = $connection->escapeTable($options['type_column']);
      $property_table = $connection->escapeTable($options['property_table']);
      $property_type_column = $connection->escapeTable($options['property_type_column']);

      // Transform string into a search pattern with wildcards
      $condition_value = ($options['leading_wildcard']?'%':'') . $string . '%';
      $query = $connection->select('1:' . $base_table, 'BT');
      $query->addField('BT', $options['pkey_id'], 'pkey');
      $query->addField('BT', $column_name, 'value');
      // Single "%" wildcard is used to indicate return all records for a select
      if ($string != '%') {
        $query->condition('BT.' . $column_name, $condition_value, 'ILIKE');
      }
      $query->orderBy('BT.' . $column_name, 'ASC');
      if ($options['count']) {
        $query->range(0, $options['count']);
      }

      if ($options['type_id'] > 0) {
        if ($property_table and ($property_table != $base_table)) {
          $query->join('1:' . $property_table, 'PT', '"BT".' . $options['pkey_id'] . ' = "PT".' . $options['pkey_id']);
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
          $query->join('1:cvterm', 'CVT', '"BT".' . $type_column . ' = "CVT".cvterm_id');
        }
      }
    }
    return $query;
  }

  /**
   * Fetch the pkey id number, given an autocomplete value with numeric
   * id in parentheses at the end of the string.
   *
   * @param string $value
   *   Autocomplete value, e.g. "Some (Big) Analysis (12)"
   *
   * @return int
   *   Primary key id number of the record, or 0 if invalid $value was passed.
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
