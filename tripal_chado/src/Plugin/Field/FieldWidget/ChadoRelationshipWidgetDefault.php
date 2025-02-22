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
    $base_column = $storage_settings['base_column'] ?? NULL;
    // During manual field addition there may be no base table
    // selected yet, in which case bypass this form
    if (!$base_table || !$base_column) {
      return $element;
    }

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
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
    if ($record_id and $object_id and ($record_id == $object_id)) {
      $reverse_default = 1;
      $related_default = $subject_name;
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
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $element;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $new_values = [];
    foreach ($values as $delta => $value) {
      // Construct an updated $value array as expected by the field type
      $new_value = [];

      // Use autocomplete to replace term with cvterm_id value
      $cvterm_id = 0;
      if ($value['term']) {
        $cv_autocomplete = new ChadoCVTermAutocompleteController();
        $cvterm_id = $cv_autocomplete->getCVtermId($value['term']);

        // Use autocomplete to convert related record to its ID value
        $record_id = $value['record_id'];
        $related_record_id = 0;
        if ($value['related_record']) {
          $new_value = $value;
          $new_value['subject_id'] = 0;
          $new_value['object_id'] = 0;
          $new_value['type_id'] = $cvterm_id;
          $generic_autocomplete = new ChadoGenericAutocompleteController();
          $related_record_id = $generic_autocomplete->getPkeyId($value['related_record']);

          // We need to know the orientation to put the correct
          // values in the subject and object columns
          $reverse = $value['reverse'];

          if ($reverse == 1) {
            $new_value['subject_id'] = $related_record_id;
            $new_value['object_id'] = $record_id;
          }
          else {
            $new_value['subject_id'] = $record_id;
            $new_value['object_id'] = $related_record_id;
          }

          // Remove items that are no longer needed
          unset($new_value['term']);
          unset($new_value['related_record']);
          unset($new_value['direction']);
        }
      }

      $new_values[$delta] = $new_value;
    }

    return $new_values;
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
