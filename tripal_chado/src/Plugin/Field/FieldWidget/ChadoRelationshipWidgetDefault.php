<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;
use Drupal\tripal_chado\Controller\ChadoCVTermAutocompleteController;
use Drupal\tripal_chado\Controller\ChadoGenericAutocompleteController;


/**
 * Plugin implementation of default Chado relationship widget.
 *
 * @FieldWidget(
 *   id = "chado_relationship_widget_default",
 *   label = @Translation("Chado Relationship Widget"),
 *   description = @Translation("The default relationship widget."),
 *   field_types = {
 *     "chado_relationship_type_default"
 *   }
 * )
 */
class ChadoRelationshipWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_name = $field_definition->get('field_name');
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $base_column = $storage_settings['base_column'] ?? '';
    $subject_column = $storage_settings['subject_column'] ?? '';
    $object_column = $storage_settings['object_column'] ?? '';
    // During manual field addition there may be no base table
    // selected yet, in which case bypass this form
    if (!$base_table || !$base_column) {
      return $element;
    }

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
#dpm($item_vals, "CPWF3 item_vals");//@@@
    $record_id = $item_vals['record_id'] ?? 0;
    $type_id = $item_vals['type_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $subject_id = $item_vals['subject_id'] ?? 0;
    $subject_name = $item_vals['subject_name'] ?? '';
    $object_id = $item_vals['object_id'] ?? 0;
    $object_name = $item_vals['object_name'] ?? '';
    if ($subject_id) {
      $subject_name .= ' (' . $subject_id . ')';
    }
    if ($object_id) {
      $object_name .= ' (' . $object_id . ')';
    }

    $reverse_default = 0;
    $related_default = $object_name;
    $related_id = $object_id;
    if ($record_id and $object_id and ($record_id == $object_id)) {
      $reverse_default = 1;
      $related_default = $subject_name;
      $related_id = $subject_id;
    }

    $element['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoRelationshipWidgetDefault';
    $element['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $element['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $element['type_id'] = [
      '#type' => 'value',
      '#default_value' => $type_id,
    ];

    // pass the foreign key names through the form for massageFormValues()
    $element['subject_column'] = [
      '#type' => 'value',
      '#default_value' => $subject_column,
    ];
    $element['object_column'] = [
      '#type' => 'value',
      '#default_value' => $object_column,
    ];
    // pass the field machine name through the form for massageFormValues()
    $element['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];

    // CV term autocomplete. This controller includes synonyms
    $term_autocomplete_default = '';
    if ($type_id) {
      $cv_autocomplete = new ChadoCVTermAutocompleteController();
      $term_autocomplete_default = $cv_autocomplete->formatCVterm($type_id);
    }
    $element['term'] = [
      '#type' => 'textfield',
      '#required' => FALSE,
#      '#description' => $this->t('Enter a vocabulary term name. A set of matching'
#          . ' candidates will be provided to choose from. You may find the multiple matching terms'
#          . ' from different vocabularies. The full accession for each term is provided'
#          . ' to help choose. Only the top 10 best matches are shown at a time.'),
      '#default_value' => $term_autocomplete_default,
      '#disabled' => FALSE,
      '#autocomplete_route_name' => 'tripal.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 10],
      '#element_validate' => [[static::class, 'validateAutocomplete']],
    ];

    // Related record
    $element['related_record'] = [
      '#type' => 'textfield',
      '#required' => FALSE,
#      '#description' => $this->t('Select the record that is related to the current record.'),
      '#default_value' => $related_default,
      '#disabled' => FALSE,
      '#autocomplete_route_name' => 'tripal_chado.generic_autocomplete',
      '#autocomplete_route_parameters' => [
        'base_table' => $base_table,
        'column_name' => $base_column,
        'type_column' => 'x',
        'property_table' => $base_table,
        'match_limit' => 10,
        'type_id' => 0,
      ],
      '#element_validate' => [[static::class, 'validateRelatedRecord']],
    ];

    $element['reverse'] = [
      '#type' => 'checkbox',
      '#title' => t('Reverse'),
      '#default_value' => $reverse_default,
    ];

    // To reduce clutter, only display these on the first row
    if ($delta == 0) {
      $element['term']['#title'] = t('Controlled Vocabulary Term');
      $element['related_record']['#title'] = $this->t('Related @table record', ['@table' => $base_table]);
      $element['reverse']['#description'] = $this->t('if this is the subject of the relationship', ['@table' => $base_table]);
    }

    // We also need these two to have a specific combined wrapper in addition to the fieldset.
    $element['term']['#prefix'] = '<div class="chado-relationship-field-wrapper form-item">' . ($element['term']['#prefix'] ?? '');
    $element['direction']['#suffix'] = ($element['direction']['#suffix'] ?? '') . '</div>';

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveRelatedInitialValues($delta, $field_name, $linker_id, $type_id, $related_id, $reverse_default, $form_state);
    // Additional initial values specific to this field

    return $element;
  }

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
   * @param int $type_id
   *   The cvterm_id of the relationship type
   * @param int $related_record_id
   *   The ID of related record
   * @param int $reverse
   *   The checkbox to reverse subject:object orientation
   * @param FormStateInterface &$form_state
   *   The current form state.
   */
  protected function saveRelatedInitialValues(int $delta, string $field_name, int $linker_id, int $type_id, int $related_record_id, int $reverse, FormStateInterface &$form_state) {
    $storage = $form_state->getStorage();
    // We want the initial values, so never update them once saved.
    if (!($storage['initial_values'][$field_name][$delta] ?? FALSE)) {
      $storage['initial_values'][$field_name][$delta] = [
        'linker_id' => $linker_id,
        'type_id' => $type_id,
        'related_record_id' => $related_record_id,
        'reverse' => $reverse,
      ];
      $form_state->setStorage($storage);
    }
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $linker_key = 'linker_id';
    $first_delta = array_key_first($values);
dpm($values, "CPW01A Widget massage values in");//@@@
    $field_name = $values[$first_delta]['field_name'];





    // Convert the widget fields into an updated $values array
    // with the items expected by the field type
    $this->preMassageFormValues($values);
dpm($values, "CPW01B Widget massage values after first processing");//@@@




    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is removed from the autocomplete field.
    $retained_records = [];
    foreach ($values as $delta => $value) {
      if ($value[$linker_key]) {
dpm("CPW02A save retained record at delta $delta");//@@@
        $retained_records[$delta] = $value[$linker_key];
      }
      if ((($value['subject_id'] == 0) and ($value['object_id'] == 0)) or ($value['type_id'] == 0)) {
        if ($value['record_id']) {
          // If there is a record_id, but no related record, this
          // means we need to pass in this record to chado storage
          // to have the linker record be deleted there. To do
          // this, we need to have the correct primitive type for
          // this field, so change from empty string to zero.
dpm("CPW02B set to zero at delta $delta");//@@@
          $values[$delta]['subject_id'] = 0;
          $values[$delta]['object_id'] = 0;
          $values[$delta]['type_id'] = 0;
        }
        else {
          // If there is no record_id, then it is the empty
          // field at the end of the list, and can be ignored.
dpm("CPW02C remove at delta $delta");//@@@
          unset($values[$delta]);
        }
      }
    }
dpm($retained_records, "CPW03A retained records");//@@@
dpm($values, "CPW03B values");//@@@



    // If there were any values in the initial values that are not
    // present in the current form state, then an existing record
    // was deleted by clicking the "Remove" button. Similarly to
    // the code above, we need to include these in the values array
    // so that chado storage is informed to delete the linking record.
    $next_delta = $values ? array_key_last($values) + 1 : 0;
    $storage_values = $form_state->getStorage();
    $initial_values = $storage_values['initial_values'][$field_name];
dpm($initial_values, "CPW04 initial_values. next_delta=$next_delta");//@@@
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id'
      $linker_id = $initial_value['linker_id'] ?? 0;
      if ($linker_id) {
        $initial_reverse = $initial_value['reverse'];
        if (!in_array($linker_id, $retained_records)) {
          // This item was removed from the form. Add back a value
          // so that chado storage knows to remove the chado record.
          $values[$next_delta]['linker_id'] = $linker_id;
          $this->markForDeletion($values, $next_delta, $initial_reverse, $initial_value['related_record_id']);
dpm($values[$next_delta], "CPW05 adding back value at $next_delta");//@@@
          $next_delta++;
        }
        else {
          // The item is still in the form.
          // Handle the case where the related record is changed, which happens
          // either by selecting a different record, or by changing the state
          // of the reverse toggle. If either of these changed, then the
          // original record needs to be removed, and then a new record created.
          $delta = array_search($linker_id, $retained_records);
          $check_key = ($initial_reverse == 1) ? 'subject_id' : 'object_id';
          if ($values[$delta][$check_key] != $initial_value['related_record_id']) {
            // Move the new info from the form to the end and remove linker_id so it will be inserted as new
            $values[$next_delta] = $values[$delta];
            $values[$next_delta]['linker_id'] = 0;
dpm("CPW06 Mark edited delta=$delta for deletion and add $next_delta for insertion");//@@@
            $next_delta++;
            // mark the current record for deletion in chado storage
            $this->markForDeletion($values, $delta, $initial_reverse, $initial_value['related_record_id']);
          }
        }
      }
    }



    // Reset the weights
    $i = 0;
    foreach ($values as $delta => $value) {
      $values[$delta]['_weight'] = $i;
      $i++;
    }



dpm($values, "CPW09 Widget massage values out");//@@@
    return $values;
  }

  /**
   * Configures a single record so that it will be deleted by chado storage.
   * Because both subject_id and object_id are set as "store_link", we need to
   * have the original related record ID present in the correct one for deletion.
   *
   * @param array &$values
   *   The values array presented by massageFormValues
   * @param int $delta
   *   The numeric index of the item.
   * @param int $reverse
   *   The checkbox to reverse subject:object orientation
   * @param int $related_record_id
   *   The non-host record, either from subject_id or object_id depending on direction
   * @return void
   */
  protected function markForDeletion(array &$values, $delta, $reverse, $related_record_id): void {
    if ($reverse == 1) {
      $values[$delta]['subject_id'] = $related_record_id;
      $values[$delta]['object_id'] = 0;
    }
    else {
      $values[$delta]['subject_id'] = 0;
      $values[$delta]['object_id'] = $related_record_id;
    }
    // This field is configured as delete_if_empty, and will be the one that triggers chado deletion
    $values[$delta]['type_id'] = 0;
  }

  /**
   * Convert the values from the form fields into an updated
   * array containing the items expected by the field type.
   *
   * @param array &$values
   *   The values array passed to massageFormValues
   * @return void
   */
  protected function preMassageFormValues(array &$values): void {
    foreach ($values as $delta => $value) {
      $new_value = $value;
      $new_value['subject_id'] = 0;
      $new_value['object_id'] = 0;
      $new_value['type_id'] = 0;

      // Use autocomplete to replace term with cvterm_id value
      $cvterm_id = 0;
      if ($value['term']) {
        $cv_autocomplete = new ChadoCVTermAutocompleteController();
        $cvterm_id = $cv_autocomplete->getCVtermId($value['term']);

        // Use the autocomplete to convert the related record to its ID value
        $record_id = $value['record_id'];
        $related_record_id = 0;
        if ($value['related_record']) {
          // If you add a term but no record, it will just be ignored, thus
          // we don't add the term until here when we know there is a record.
          $new_value['type_id'] = $cvterm_id;
          $generic_autocomplete = new ChadoGenericAutocompleteController();
          $related_record_id = $generic_autocomplete->getPkeyId($value['related_record']);

          // We need to know the orientation to put the correct
          // values in the subject and object columns
          if ($value['reverse'] == 1) {
            $new_value['subject_id'] = $related_record_id;
            $new_value['object_id'] = $record_id;
          }
          else {
            $new_value['subject_id'] = $record_id;
            $new_value['object_id'] = $related_record_id;
          }

        }
      }
      // Remove form items that are not needed for the field
      unset($new_value['term']);
      unset($new_value['related_record']);
      unset($new_value['reverse']);

      $values[$delta] = $new_value;
    }
  }

  /**
   * Form element validation handler for the CV term field
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateAutocomplete($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];
    if ($element_value != '') {
      $cv_autocomplete = new ChadoCVTermAutocompleteController();
      $cvterm_id = $cv_autocomplete->getCVtermId($element_value);
      if (!$cvterm_id) {
        $form_state->setErrorByName(implode('][', $element_parents),
            t('The Controlled Vocabulary Term "@term" is not a valid term', ['@term' => $element_value]));
      }
      // We permit entering a term without a related record, it will just be ignored
    }
  }

  /**
   * Form element validation handler for the related record field
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateRelatedRecord($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    // element_parents e.g. 0 => "project_relationship", 1 => 0, 2 => "related_record"
    $element_value = $element['#value'];
    if ($element_value != '') {
      $generic_autocomplete = new ChadoGenericAutocompleteController();
      $related_record_id = $generic_autocomplete->getPkeyId($element_value);
      if (!$related_record_id) {
        $form_state->setErrorByName(implode('][', $element_parents),
            t('The specified record does not include a numeric record ID in parentheses'));
      }

      // The related record cannot be the same as the current record
      $values = $form_state->getValues();
      $record_id = $values[$element_parents[0]][$element_parents[1]]['record_id'] ?? 0;
      if ($record_id and ($record_id == $related_record_id)) {
        $form_state->setErrorByName(implode('][', $element_parents),
            t('The specified record cannot be the same as this entity'));
      }

      // We will not permit having a related record without also specifying a term.
      // The term has its own validation, we just need to check that one was entered.
      $term = $values[$element_parents[0]][$element_parents[1]]['term'] ?? 'missing';
      if (!$term) {
        $form_state->setErrorByName(implode('][', [$element_parents[0], $element_parents[1], 'term']),
            t('A record has been entered, but a term has not been specified'));
      }
    }
  }
}
