<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalField\TripalWidgetBase;



/**
 * Defines the Chado field widget base class.
 */
abstract class ChadoWidgetBase extends TripalWidgetBase {

  /**
   * Saves some values from the initial form state when an entity
   * is first edited for multi-cardinality linking fields.
   * These values are needed to support the "Remove" button.
   *
   * @param int $delta
   *   The numeric index of the item.
   * @param string $field_term
   *   The controlled vocabulary term for the field.
   * @param int $linker_id
   *   The primary key value of the record in the linking table.
   * @param FormStateInterface &$form_state
   *   The current form state.
   */
  protected function saveInitialValues(int $delta, string $field_term, int $linker_id, FormStateInterface &$form_state) {
    $storage = $form_state->getStorage();
    // We want the initial values, so never update them once saved.
    if (!($storage['initial_values'][$field_term][$delta] ?? FALSE)) {
      $storage['initial_values'][$field_term][$delta] = [
        'linker_id' => $linker_id,
      ];
      $form_state->setStorage($storage);
    }
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
    $fkey = $values[0]['linker_fkey_column'] ?? $fkey;

    // The CV term used for the field. Sometimes there are multiple
    // copies of one field, e.g. properties, so this distinguishes them.
    $field_term = $values[0]['field_term'];

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is changed to "-- Select --"
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
    $initial_values = $storage_values['initial_values'][$field_term];
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id', regardless of $linker_key value.
      $linker_id = $initial_value['linker_id'];
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

    // The CV term used for the field. There are usually multiple
    // copies of a property field, so this distinguishes them.
    $field_term = $values[0]['field_term'];

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is changed to "-- Select --"
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
    $initial_values = $storage_values['initial_values'][$field_term];
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
}
