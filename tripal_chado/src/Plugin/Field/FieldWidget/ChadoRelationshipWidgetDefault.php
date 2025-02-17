<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;
use Drupal\tripal_chado\Controller\ChadoCVTermAutocompleteController;
use Drupal\tripal_chado\Controller\ChadoGenericAutocompleteController;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

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

//    $chado = \Drupal::service('tripal_chado.database');
//dpm($element, "CP001 passed element");//@@@
#    $element = [];

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_name = $field_definition->get('field_name');
dpm($field_name, "CP20 field_name=");//@@@ OK
#    $field_settings = $field_definition->getSettings();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
dpm($storage_settings, "CP20B widget formElement storage_settings=");//@@@ OK base_table and base_column
    // During manual field addition there may be no base table selected yet, so bypass this form
    if (!$base_table) {
      return $element;
    }
    $base_column = $storage_settings['base_column'];
dpm($base_column ?? 'undef', "CP20C base_column=");//@@@ OK

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
dpm($item_vals, "CP21 widget item_vals=");  //@@@ NOT OK? empty array
    $record_id = $item_vals['record_id'] ?? 0;
#if(!$record_id){ dpm($items[$delta], "CP21R no record_id items[$delta]=");}//@@@
    $linker_id = $item_vals['linker_id'] ?? 0;

    $element['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $element['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];

    // CV term autocomplete
    $term_autocomplete_default = ''; //@todo
    $element['term'] = [
      '#type' => 'textfield',
      '#title' => 'Controlled Vocabulary Term',
      '#required' => FALSE,
      '#description' => $this->t('Enter a vocabulary term name. A set of matching'
          . ' candidates will be provided to choose from. You may find the multiple matching terms'
          . ' from different vocabularies. The full accession for each term is provided'
          . ' to help choose. Only the top 10 best matches are shown at a time.'),
      '#default_value' => $term_autocomplete_default,
      '#disabled' => FALSE,
      '#autocomplete_route_name' => 'tripal.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 10],
      '#element_validate' => [[static::class, 'validateAutocomplete']],
    ];

    // Related record
    $related_record_default = 'xx'; //@todo
dpm($base_table, "CP22a base_table=");//@@@
dpm($base_column, "CP22b base_column=");//@@@
    $element['related_record'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Related @table record', ['@table' => $base_table]),
      '#required' => FALSE,
      '#description' => $this->t('Select the record that is related to the current record.'),
      '#default_value' => $related_record_default,
      '#disabled' => FALSE,
      '#autocomplete_route_name' => 'tripal_chado.generic_autocomplete',
//@@@{base_table}/{column_name}/{type_column}/{property_table}/{match_limit}/{type_id}
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

    $direction_default = 1; //@todo
    $element['direction'] = [
      '#type' => 'radios',
      '#title' => $this->t('Orientation of the relationship'),
      '#options' => [
        '1' => $this->t('Related @table record is the subject of the relationship', ['@table' => $base_table]),
        '-1' => $this->t('Related @table record is the object of the relationship', ['@table' => $base_table]),
      ],
      '#default_value' => $direction_default,
    ];

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
dpm($value, "CP25 massageFormValues delta=$delta value=");//@@@
      // Look up cvterm_id from term name returned from autocomplete
      $cv_autocomplete = new ChadoCVTermAutocompleteController();
      $cvterm_id = $cv_autocomplete->getCVtermId($value['term']);
dpm($cvterm_id, "CP26 cvterm_id=");//@@@
      $record_id = $value['record_id'];
      $generic_autocomplete = new ChadoGenericAutocompleteController();
      $related_record_id = $generic_autocomplete->getPkeyId($value['related_record']);
dpm($record_id, "CP27 record_id=");//@@@
dpm($related_record_id, "CP28 related_record_id=");//@@@
      // We need to put the correct values in the subject and object columns
      $direction = $value['direction'];
dpm($direction, "CP29 direction=");//@@@
      // Construct a $value as expected by the field type
      $new_value = $value;

      $new_value['type_id'] = $cvterm_id;
      unset($new_value['term']);

      if ($direction == 1) {
        $new_value['subject_id'] = $record_id;
        $new_value['object_id'] = $related_record_id;
      }
      else {
        $new_value['subject_id'] = $related_record_id;
        $new_value['object_id'] = $record_id;
      }
      unset($new_value['related_record']);
      unset($new_value['direction']);
dpm($new_value, "CP30 new_value=");//@@@
      $new_values[$delta] = $new_value;
#$x = debug_backtrace(); for ($i = 0; $i<count($x); $i++) { //@@@
#$caller = $x[$i]['function'];dpm("CP31 caller $i = $caller"); }//@@@
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

      // The related record cannot be the same as the current record @todo
#      $values = $form_state->getValues();
#dpm($values, "valrr values");//@@@
#      $entity_id = $values['path'][$element_parents[1]]['pid'] ?? 0;
#dpm($parent_id, "CP311 parent_id"); dpm($related_record_id, "CP312 related_record_id");//@@@
#      if ($parent_id && $parent_id == $related_record_id) {
#        $form_state->setErrorByName(implode('][', $element_parents),
#            t('The specified record cannot be the same as this entity'));
#      }

      // We will not permit having a related record without also specifying a term.
      // The term has its own validation, we just need to check for not empty.
      $term = $values[$element_parents[0]][$element_parents[1]]['term'] ?? 'missing';
      if (!$term) {
        $form_state->setErrorByName(implode('][', [$element_parents[0], $element_parents[1], 'term']),
            t('A record has been entered, but a term has not been specified'));
      }
    }
  }

  /**
   * Ajax callback to update the db_id for the accession autocomplete.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
# Not likely to be needed, using cvterm autocomplete instead
#  public function widgetAjaxCallback($form, &$form_state) {
#dpm("CP92 widgetAjaxCallback");//@@@
#    // Extract the field's machine name and delta from the triggering element,
#    // e.g. "field_study_dbxref[0][dbxref][db_id]".
#    $triggering_element = $form_state->getTriggeringElement()['#name'];
#    preg_match('/^([^\[]+)\[(\d+)\]/', $triggering_element, $matches);
#    $machine_name = $matches[1];
#    $delta = $matches[2];
#
#    $response = new AjaxResponse();
#    $response->addCommand(new ReplaceCommand('#edit-' . $machine_name . '-accession-' . $delta,
#        $form[$machine_name]['widget'][$delta]['dbxref_accession']));
#    return $response;
#  }
}
