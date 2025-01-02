<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalField\TripalWidgetBase;



/**
 * Defines the Chado field widget base class.
 *
 * For linking multi-cardinality fields, this class includes the following
 * helper methods to provide support for the "remove" button added by Drupal.
 * To enable this support, the following need to be implemented in your widget:
 *  1. You have the following elements in formElement():
 *     - a hidden value element with the key of 'record_id' that contains the
 *       base table primary key. This is used to determine if an element should
 *       be removed due to being set to empty.
 *     - a hidden value element with the key of 'field_name' and the value is
 *       the name of the field (see code example below).
 *       @code
 *         $field_name = $items->getFieldDefinition()->get('field_name');
 *         $elements['field_name'] = [
 *           '#type' => 'value',
 *           '#default_value' => $field_name,
 *         ];
 *       @endcode
 *  2. You call saveInitialValues() at the bottom of your formElement() and
 *     pass in information about the linking record.
 *  3. You call massageLinkingFormValues() or massagePropertyFormValues() in
 *     your massageFormValues() and indicate the requested elements. See the doc
 *     block headers for these methods for specifics.
 */
abstract class ChadoWidgetBase extends TripalWidgetBase {

  /**
   * Saves some values from the initial form state when an entity
   * is first edited for multi-cardinality linking fields.
   * These values are needed to support the "Remove" button.
   *
   * @param int $delta
   *   The numeric index of the item.
   * @param string $field_name
   *   The machine name of the field used for linking the info we're saving in
   *   form_state with the values submitted by the form.
   * @param int $linker_id
   *   The primary key value of the record in the linking table.
   * @param FormStateInterface &$form_state
   *   The current form state.
   */
  protected function saveInitialValues(int $delta, string $field_name, int $linker_id, FormStateInterface &$form_state) {
    $storage = $form_state->getStorage();
    // We want the initial values, so never update them once saved.
    if (!($storage['initial_values'][$field_name][$delta] ?? FALSE)) {
      $storage['initial_values'][$field_name][$delta] = [
        'linker_id' => $linker_id,
      ];
      $form_state->setStorage($storage);
    }
  }

  /**
   * Generic select form element generator. For a small number of values
   * this creates a select, for many values this creates an autocomplete.
   *
   * @param Drupal\pgsql\Driver\Database\pgsql\Select $query
   *   A prepared database query
   * @param string $pkey_column
   *   The name of the primary key column
   * @param int|null $default_id
   *   The pkey_id value of the default, if one exists
   * @param array $autocomplete_parameters
   *   All the values needed for the autocomplete
   * @param ?int $limit
   *   The maximum number of records for a select. If more, then
   *   use autocomplete. Use zero if autocomplete always wanted.
   *
   * @return array
   *   The appropriate form element
   */
  protected function genericSelectElement($query, string $pkey_column, ?int $default_id,
      array $autocomplete_parameters, ?int $limit = NULL): array {

    // The limit parameter is optional for this function. If not specified,
    // then use the global settings value. If that is not set, default to 50.
    if (is_null($limit)) {
      $limit = \Drupal::config('tripal.settings')->get('tripal_entity_type.widget_select_limit');
      if (is_null($limit) or (trim($limit) === '')) {
        $limit = 50;
      }
    }

    $element = [];

    // Get a count of the number of possible values
    $count = $query->countQuery()->execute()->fetchField();

    // For a large number of options, use an autocomplete
    if ($count > $limit) {

      // Look up the default value if one was specified
      $default_value = '';
      if ($default_id) {
        $query->condition($pkey_column, $default_id, '=');
        $result = $query->execute()->fetchObject();
        if ($result) {
          // Strip HTML tags if present, e.g. in Pub title
          $default_value = strip_tags($result->value ?? '');
          if (property_exists($record, 'type') and $record->type) {
            $default_value .= ' [' . $record->type . ']';
          }
          // Append the chado pkey id value
          $default_value .= ' (' . $default_id . ')';
        }
      }
      $element = [
        '#type' => 'textfield',
        '#default_value' => $default_value,
        '#autocomplete_route_name' => 'tripal_chado.generic_autocomplete',
        '#autocomplete_route_parameters' => $autocomplete_parameters,
      ];
    }

    // For a small number of options, use a select
    else {
      $results = $query->execute();
      $select_options = [];
      while ($record = $results->fetchObject()) {
        // Strip HTML tags if present, e.g. in Pub title
        $value = strip_tags($record->value ?? '');
        if (property_exists($record, 'type') and $record->type) {
          $value .= ' [' . $record->type . ']';
        }
        $select_options[$record->pkey_id] = $value;
      }
      natcasesort($select_options);
      $element = [
        '#type' => 'select',
        '#options' => $select_options,
        '#default_value' => $default_id,
        '#empty_option' => $this->t('- Select -'),
      ];
    }
    $element['#element_validate'] = [[static::class, 'validateAutocomplete']];
    return $element;
  }

  /**
   * Assists the massageFormValues() function for linking fields, that
   * is, double-hop fields where an intermediate linking table is used.
   * This includes properly handling deletion of the record in the
   * linking table in chado.
   *
   * @param string $fkey
   *   The foreign key column name in the linking table.
   *   Needed because it is not guaranteed to be in $values array,
   *   e.g. for dbxref.
   * @param array $values
   *   The submitted form values produced by the widget.
   *   - If the widget does not manage multiple values itself, the array holds
   *     the values generated by the multiple copies of the $element generated
   *     by the formElement() method, keyed by delta.
   *   - If the widget manages multiple values, the array holds the values
   *     of the form element generated by the formElement() method.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state.
   * @param string $linker_key
   *   The key used for the linking table primary key.
   *   For most fields this is "linker_id" and can be omitted, but
   *   see the synonym field for an exception.
   *
   * @return array
   *   An array of field values, keyed by delta.
   */
  protected function massageLinkingFormValues(string $fkey, array $values,
      FormStateInterface $form_state, string $linker_key = 'linker_id') {
    if (!$values) {
      return $values;
    }

    // In some cases the foreign key is not the same name as in the
    // base table, e.g. manufacturer_id as a fkey for contact_id.
    // n.b. this has no effect for the property field.
    $first_delta = array_key_first($values);
    $fkey = $values[$first_delta]['linker_fkey_column'] ?? $fkey;

    // The machine name for the field. Sometimes there are multiple
    // copies of one field, e.g. properties, so this distinguishes them.
    $field_name = $values[$first_delta]['field_name'];

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is changed to "- Select -"
    $retained_records = [];
    foreach ($values as $val_key => $value) {
      if ($value[$linker_key]) {
        $retained_records[$val_key] = $value[$linker_key];
      }
      if ($value[$fkey] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no linked record id, this
          // means we need to pass in this record to chado storage
          // to have the linker record be deleted there. To do
          // this, we need to have the correct primitive type for
          // this field, so change from empty string to zero.
          $values[$val_key][$fkey] = 0;
        }
        else {
          // If there is no record_id, then it is the empty
          // field at the end of the list, and can be ignored.
          unset($values[$val_key]);
        }
      }
    }

    // If there were any values in the initial values that are not
    // present in the current form state, then an existing record
    // was deleted by clicking the "Remove" button. Similarly to
    // the code above, we need to include these in the values array
    // so that chado storage is informed to delete the linking record.
    $next_delta = $values ? array_key_last($values) + 1 : 0;
    $storage_values = $form_state->getStorage();
    $initial_values = $storage_values['initial_values'][$field_name];
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id', regardless of $linker_key value.
      $linker_id = $initial_value['linker_id'] ?? 0;
      if ($linker_id and !in_array($linker_id, $retained_records)) {
        // This item was removed from the form. Add back a value
        // so that chado storage knows to remove the chado record.
        $values[$next_delta][$linker_key] = $linker_id;
        $values[$next_delta][$fkey] = 0;
        $next_delta++;
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      $values[$val_key]['_weight'] = $i;
      $i++;
    }
    return $values;
  }

  /**
   * A helper for massageFormValues() where the generic autocomplete is used.
   *
   * The genericSelectElement() will return an integer in the $values array
   * when a select is used, but returns a string with an embedded id value
   * in parentheses when the autocomplete is used. In this latter case, we
   * need to return just the integer value.
   * Note that if you somehow pass a text string without an embedded value,
   * then no changes are made, however, this should be prevented by validation.
   *
   * @param string $pkey_id
   *   The name of the value to be massaged, e.g. "analysis_id"
   * @param array $values
   *   The submitted form values produced by the widget.
   * @return array
   *   The massaged values
   */
  protected function genericSelectMassageFormValues(string $pkey_id, array $values): array {
    foreach ($values as $key => $info) {
      if (array_key_exists($pkey_id, $info)) {
        // If already an integer, then do nothing
        if (!preg_match('/^\d+$/', $info[$pkey_id])) {
          if (preg_match('/\((\d+)\)$/', $info[$pkey_id], $matches)) {
            $values[$key][$pkey_id] = $matches[1];
          }
        }
      }
    }
    return $values;
  }

  /**
   * Assists the massageFormValues() function for property fields, that
   * is, single-hop fields where the linked table contains a value.
   * This includes properly handling deletion of the record in the
   * linked table in chado.
   *
   * @param string $val
   *   The name that the value is stored under, i.e. 'value'
   * @param array $values
   *   The submitted form values produced by the widget.
   *   - If the widget does not manage multiple values itself, the array holds
   *     the values generated by the multiple copies of the $element generated
   *     by the formElement() method, keyed by delta.
   *   - If the widget manages multiple values, the array holds the values
   *     of the form element generated by the formElement() method.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state.
   * @param string $rank_term
   *   If present, used to reset rank in values.
   * @param string $linker_key
   *   The key used for the linking table primary key.
   *   For property fields this is "prop_id" and can be omitted.
   *
   * @return array
   *   An array of field values, keyed by delta.
   */
  protected function massagePropertyFormValues(string $val, array $values,
      FormStateInterface $form_state, string $rank_term = NULL, string $linker_key = 'prop_id') {

    if (!$values) {
      return $values;
    }

    // The field name for the field. There are usually multiple
    // copies of a property field, so this distinguishes them.
    $first_delta = array_key_first($values);
    $field_name = $values[$first_delta]['field_name'];

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is changed to "- Select -"
    $retained_records = [];
    foreach ($values as $val_key => $value) {
      if ($value[$linker_key]) {
        $retained_records[$val_key] = $value[$linker_key];
      }
      if ($value[$val] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no value, this
          // means we need to pass in this record to chado storage
          // to have the linker record be deleted there. Here,
          // the empty string is the correct primitive type,
          // so nothing to change.
        }
        else {
          // If there is no record_id, then it is the empty
          // field at the end of the list, and can be ignored.
          unset($values[$val_key]);
        }
      }
    }

    // If there were any values in the initial values that are not
    // present in the current form state, then an existing record
    // was deleted by clicking the "Remove" button. Similarly to
    // the code above, we need to include these in the values array
    // so that chado storage is informed to delete the linking record.
    $next_delta = $values ? array_key_last($values) + 1 : 0;
    $storage_values = $form_state->getStorage();
    $initial_values = $storage_values['initial_values'][$field_name];
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id', regardless of $linker_key value.
      $linker_id = $initial_value['linker_id'];
      if ($linker_id and !in_array($linker_id, $retained_records)) {
        // This item was removed from the form. Add back a value
        // so that chado storage knows to remove the chado record.
        $values[$next_delta][$linker_key] = $linker_id;
        $values[$next_delta][$val] = '';
        $next_delta++;
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      if ($values[$val_key][$val]) {
        $values[$val_key]['_weight'] = $i;
        if ($rank_term) {
          $values[$val_key][$rank_term] = $i;
        }
        $i++;
      }
    }
    return $values;
  }

  /**
   * Form element validation handler for an autocomplete field
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
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
