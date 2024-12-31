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
   * Controller autocomplete method to use for any chado table
   * where the returned value is from a single column.
   *
   * For simplicity, this autocomplete assumes that the primary key
   * of the base table is the base table name + '_id', and that the
   * type columnn is named 'type_id'.
   *
   * To support columns without a unique constraint, the returned
   * autocomplete value includes the primary key numeric value in
   * parentheses at the end.
   *
   * @param Request request
   *
   * @param string $base_table
   *   Chado base table name
   *
   * @param string $column_name
   *   Name of chado base column to be returned
   *
   * @param string $property_table
   *   Property table name, use same name as base table if not needed
   *
   * @param int $count
   *   Desired number of matching names to suggest.
   *   Default to 5 items.
   *   Must be declared in autocomplete route parameter e.g. ['count' => 5].
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
  public function handleAutocomplete(Request $request,
     string $base_table, string $column_name, string $property_table,
     int $count = 5, int $type_id = 0) {

    // Array to hold matching records.
    $response = [];

    if ($request->query->get('q')) {
      // Get typed in string input.
      $string = trim($request->query->get('q'));

      // Proceed to autocomplete when string is at least a character
      // long and result count is set to a value greater than 0.
      if (strlen($string) > 0 && $count > 0) {

        $connection = \Drupal::service('tripal_chado.database');

        // Sanitization, placeholders cannot be used for column and table names
        $base_table = $connection->escapeTable($base_table);
        $column_name = $connection->escapeTable($column_name);
        $property_table = $connection->escapeTable($property_table);

        // Assumptions for simplicity
        $pkey_id = $base_table . '_id';
        $type_column = 'type_id';

        // Transform string into a search pattern with wildcards
        $keyword = '%' . strtolower($string) . '%';

        $args = [];
        $sql = 'SELECT BT.' . $pkey_id .' AS pkey, BT.' . $column_name . ' AS value'
             . ' FROM {1:' . $base_table . '} BT';

        // If specified, restrict to type provided by type_id in the route parameter.
        if ($type_id > 0) {
          if ($property_table and ($property_table != $base_table)) {
            $sql .= ' LEFT JOIN {1:' . $property_table . '} PT ON BT.' . $pkey_id . ' = PT.' . $pkey_id
                 . ' WHERE PT.' . $type_column . ' = :type_id AND';
          }
          else {
            $sql .= ' WHERE BT.' . $type_column . ' = :type_id AND';
          }
          $args[':type_id'] = $type_id;
        }
        else {
          $sql .= ' WHERE';
        }

        $sql .= ' LOWER(BT.' . $column_name .') LIKE :keyword'
             . ' ORDER BY BT.' . $column_name . ' ASC LIMIT :count';
        $args[':keyword'] = $keyword;
        $args[':count'] = $count;

        // Perform the database query
        $results = $connection->query($sql, $args);

        // Compose the response
        if ($results) {
          foreach($results as $record) {
            // Strip HTML tags if present, e.g. in Pub title
            $value = strip_tags($record->value);
            // Append the chado pkey id value
            $value .= ' (' . $record->pkey . ')';
            $response[] = [
              'value' => $value, // Value returned and value displayed by textfield.
              'label' => $value, // Value shown in the list of options.
            ];
          }
        }
      }
    }

    return new JsonResponse($response);
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
