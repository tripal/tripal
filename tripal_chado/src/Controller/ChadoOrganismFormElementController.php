<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\pgsql\Driver\Database\pgsql\Select;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;
use Drupal\Core\Form\FormStateInterface;

/**
 * Controller, Chado Organism Autocomplete.
 */
class ChadoOrganismFormElementController extends ChadoGenericAutocompleteController {

  /**
   * Controller method, autocomplete organism name.
   *
   * @param Symfony\Component\HttpFoundation\Request $request
   *   The request object.
   * @param int $match_limit
   *   Desired number of matching organism names to suggest.
   *   Default to 5 items.
   *   Must be declared in autocomplete route parameter.
   *
   * @return JsonObject
   *   Matching organism rows in an array where organism name
   *   is both the value to the array keys label and value.
   */
  public function handleAutocomplete(Request $request, int $match_limit = 5) {

    // Array to hold matching records.
    $response = [];

    // The string to autocomplete from the form input.
    $string = trim($request->query->get('q'));

    $options = [
      'match_limit' => $match_limit,
      'match_operator' => $this->match_operator,
    ];

    $query = $this->getQuery($string, $options);
    if ($query) {
      // Perform the database query.
      $results = $query->execute();

      if ($results) {
        while ($record = $results->fetchObject()) {
          // Strip HTML tags if present, but this is unlikely for organism.
          $value = strip_tags($record->organism ?? '');
          $value .= ' (' . $record->pkey . ')';
          $response[] = [
            'value' => $value,
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
   *     match_operator - Either 'CONTAINS' (default) or 'STARTS_WITH'.
   *     match_limit - Desired number of autocomplete matching names to suggest.
   *
   * @return ?\Drupal\pgsql\Driver\Database\pgsql\Select
   *   A database query object
   */
  public static function getQuery(string $string, array $options): ?Select {

    // Set defaults.
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
   * Fetch the pkey organism_id number, given an autocomplete value.
   *
   * The value includes a numeric ID in parentheses at the end of the string.
   *
   * @param string $value
   *   A value from an autocomplete with the ID in parentheses at the end,
   *   e.g. "Tripalus bogusii (ignored) (123)".
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

  /**
   * Returns a form element array, either a select or an autocomplete.
   *
   * @param array $element
   *   The form element array to be populated.
   * @param mixed $default
   *   The default value, either an integer pkey ID or a string.
   * @param array $options
   *   The following keys are used:
   *   select_limit - The maximum number of options to show in a select list.
   *                If the number of possible options exceeds this limit,
   *               an autocomplete text field is returned instead.
   *              A value of zero means always use an autocomplete.
   *   match_operator - Either 'CONTAINS' (default) or 'STARTS_WITH'.
   *   match_limit - Desired number of autocomplete matching names to suggest.
   *   size - The size of the textfield for autocomplete.
   *   placeholder - Placeholder text for the autocomplete textfield.
   *
   * @return array|null
   *   A form element array, either a select or an autocomplete.
   */
  public static function getFormElement(array $element, mixed $default, array $options): ?array {

    $element = [];

    // Construct a query
    // A single wildcard indicates that all records are to be returned.
    $string = '%';
    // Add one to select limit so we know if it is exceeded.
    $count_options = $options;
    $count_options['match_limit'] = $options['select_limit'] + 1;
    $query = self::getQuery($string, $count_options);

    // Get a count of the number of possible values, unless forcing always
    // autocomplete.
    $count = 1;
    if ($options['select_limit'] > 0) {
      $count = $query->countQuery()->execute()->fetchField();
    }

    // For a large number of options, or if limit is zero, use an autocomplete.
    if ($count > $options['select_limit']) {
      $element = self::getAutocompleteElement($element, $default, $options);
    }
    else {
      $element = self::getSelectElement($element, $default, $options);
    }
    return $element;
  }

  /**
   * The select form element.
   *
   * @param array $element
   *   The form element array to be populated.
   * @param mixed $default
   *   The default value, either an integer pkey ID or a string.
   * @param array $options
   *   select_limit - The maximum number of options to show in a select list.
   *   match_operator - Either 'CONTAINS' (default) or 'STARTS_WITH'.
   *   match_limit - Desired number of matching names to suggest.
   *
   * @return array|null
   *   A form element array, either a select element.
   */
  public static function getSelectElement(array $element, mixed $default, array $options): ?array {
    $select_options = self::getSelectOptions($options);
    natcasesort($select_options);
    if (gettype($default) == 'integer') {
      $default_id = $default;
    }
    elseif (gettype($default) == 'string') {
      $default_id = self::getPkeyId($default);
    }
    $element = [
      '#type' => 'select',
      '#options' => $select_options,
      '#default_value' => $default_id,
      '#empty_option' => t('- Select -'),
    ];
    $element['#element_validate'] = [[static::class, 'validateAutocomplete']];
    return $element;
  }

  /**
   * The autocomplete form element.
   *
   * @param array $element
   *   The form element array to be populated.
   * @param mixed $default
   *   The default value, either an integer pkey ID or a string.
   * @param array $options
   *   select_limit - The maximum number of options to show in a select list.
   *   match_operator - Either 'CONTAINS' (default) or 'STARTS_WITH'.
   *   match_limit - Desired number of autocomplete matching names to suggest.
   *   size - The size of the textfield for autocomplete.
   *   placeholder - Placeholder text for the autocomplete textfield.
   *
   * @return array|null
   *   A form element array, an autocomplete.
   */
  public static function getAutocompleteElement(array $element, mixed $default, array $options): ?array {
    if (gettype($default) == 'string') {
      $default_value = $default;
    }
    elseif (gettype($default) == 'integer') {
      $default_id = $default;
      $default_value = '';
      if ($default_id) {
        // We can reuse the existing query since only one change is needed.
        $query->condition('organism_id', $default_id, '=');
        $result = $query->execute()->fetchObject();
        if ($result) {
          // Strip HTML tags if present, e.g. in Pub title.
          $default_value = strip_tags($result->organism ?? '');
          // Append the chado pkey id value.
          $default_value .= ' (' . $default_id . ')';
        }
      }
    }
    $element = [
      '#type' => 'textfield',
      '#default_value' => $default_value,
      '#autocomplete_route_name' => 'tripal_chado.organism_autocomplete_element',
      '#autocomplete_route_parameters' => ['match_limit' => $options['match_limit']],
      '#size' => $options['size'],
    ];
    unset($options['size']);
    $element['#autocomplete_route_parameters'] = $options;
    return $element;
  }

  /**
   * The select options for a select element.
   *
   * @param array $options
   *   select_limit - The maximum number of options to show in a select list.
   *   match_limit - Desired number of matching names to suggest.
   */
  public static function getSelectOptions(array $options): ?array {
    // Construct a query
    // A single wildcard indicates that all records are to be returned.
    $string = '%';
    // Add one to select limit so we know if it is exceeded.
    $count_options = $options;
    $count_options['match_limit'] = $options['select_limit'] + 1;
    $query = self::getQuery($string, $count_options);
    $results = $query->execute();
    $select_options = [];
    while ($record = $results->fetchObject()) {
      // Strip HTML tags if present, but this is not likely for organism.
      $organism = strip_tags($record->abbreviation ?: $record->organism ?? '');
      $select_options[$record->pkey] = $organism;
    }
    return $select_options;
  }

  /**
   * Form element validation handler for an autocomplete field.
   *
   * @param array $element
   *   The form element being validated.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   */
  public static function validateAutocomplete($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];

    // The value must either be an integer, or a string with an integer
    // value in parentheses at the end.
    $valid = TRUE;
    if ($element_value) {
      $valid = FALSE;
      if (preg_match('/^\d+$/', $element_value)) {
        $valid = TRUE;
      }
      elseif (preg_match('/\(\d+\)$/', $element_value)) {
        $valid = TRUE;
      }
    }
    if (!$valid) {
      $form_state->setErrorByName(implode('][', $element_parents),
          'The specified record must include its chado record number in parentheses at the end');
    }
  }

}
