<?php

namespace Drupal\tripal_chado\Controller;

@trigger_error('The ' . __NAMESPACE__ . '\ChadoOrganismAutocompleteController is deprecated in tripal:4.0.0 and is removed from tripal:4.1.0. Instead, use \Drupal\tripal_chado\Controller\ChadoOrganismFormElement. All the methods in ChadoOrganismAutocompleteController now exist in ChadoOrganismFormElement class with the same functionality. See https://github.com/tripal/tripal/pull/2293', E_USER_DEPRECATED);

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Controller, Chado Organism Autocomplete.
 *
 * @deprecated in tripal:4.0.0 and is removed from tripal:4.1.0.
 * All the methods in ChadoOrganismAutocompleteController now exist in
 * ChadoOrganismFormElement class with the same functionality.
 * Use
 * \Drupal\tripal_chado\Controller\ChadoOrganismFormElement instead.
 *
 * @see https://github.com/tripal/tripal/pull/2293
 */
class ChadoOrganismAutocompleteController extends ChadoGenericAutocompleteController {
  /**
   * Controller method, autocomplete organism name.
   *
   * @param Request request
   *
   * @param int $match_limit
   *   Desired number of matching organism names to suggest.
   *   Default to 5 items.
   *   Must be declared in autocomplete route parameter i.e. ['match_limit' => 5].
   *
   * @return Json Object
   *   Matching organism rows in an array where organism name
   *   is both the value to the array keys label and value.
   */
  public function handleAutocomplete(Request $request, int $match_limit = 5) {

    // Array to hold matching records.
    $response = [];

    // The string to autocomplete from the form input
    $string = trim($request->query->get('q'));

    $options = [
      'match_limit' => $match_limit,
      'match_operator' => $this->match_operator,
    ];

    $query = $this->getQuery($string, $options);
    if ($query) {
      // Perform the database query
      $results = $query->execute();

      if ($results) {
        while ($record = $results->fetchObject()) {
          // Strip HTML tags if present, but this is unlikely for organism.
          $value = strip_tags($record->organism ?? '');
          // Because of https://github.com/GMOD/Chado/issues/139 we want to have the
          // pkey value included in the autocomplete result because it is possible to
          // create two organisms with the exact same name.
          $value .= ' (' . $record->pkey . ')';
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
   *     match_operator - Either 'CONTAINS' (default) or 'STARTS_WITH'.
   *     match_limit - Desired number of autocomplete matching names to suggest, default 10.
   *                   If zero, there is no limit.
   *
   * @return ?\Drupal\pgsql\Driver\Database\pgsql\Select $query
   *   A database query object
   */
  public static function getQuery(string $string, array $options): ?\Drupal\pgsql\Driver\Database\pgsql\Select {

    // Set defaults
    $options['match_operator'] ??= 'CONTAINS';
    $options['match_limit'] ??= 10;

    // Generate a query only if $string is at least a character
    // long and result count is set to a value greater than 0.
    $query = NULL;
    if (strlen($string) > 0 && $options['match_limit'] > 0) {

      $connection = \Drupal::service('tripal_chado.database');

      // Transform string into a search pattern with wildcards.
      // Only STARTS_WITH and CONTAINS are supported.
      $condition_value = $string . '%';
      if ($options['match_operator'] == 'CONTAINS') {
        $condition_value = '%' . $condition_value;
      }
      $query = $connection->select('1:organism', 'BT');
      $query->leftJoin('1:cvterm', 'T', '"BT".type_id = "T".cvterm_id');
      $query->addField('BT', 'organism_id', 'pkey');
      $query->addField('BT', 'abbreviation', 'abbreviation');
      $query->addField('BT', 'common_name', 'common_name');
      $query->addExpression("CONCAT_WS(' ', genus, species, name, infraspecific_name)", 'organism');

      // A single "%" wildcard is used to indicate that we should return
      // all records. This is used for a form select element.
      if ($string != '%') {
        $query->where("CONCAT_WS(' ', genus, species, name, infraspecific_name) ILIKE :value",
            [':value' => $condition_value]);
      }
      $query->orderBy('organism', 'ASC');
      if ($options['match_limit'] > 0) {
        $query->range(0, $options['match_limit']);
      }
    }
    return $query;
  }

  /**
   * Fetch the pkey organism_id number, given an autocomplete value with
   * a numeric ID in parentheses at the end of the string.
   *
   * @param string $value
   *   A value from an autocomplete with the ID in parentheses at the end,
   *   e.g. "Tripalus bogusii (ignored) (123)"
   *
   * @return int
   *   Primary key ID number of the record, or 0 if an unparsable $value was
   *   passed, which can happen if the user did not let the autocomplete
   *   supply a value.
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
