<?php

class local__source_data_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Data Source';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__source_data'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the field defaults.
    $sourcename = '';
    $sourceversion = '';
    $sourceuri = '';

    // Set defaults based on incoming item values.
    if (count($items) > 0 and array_key_exists('value', $items[0])) {
      $sourcename = $items[0]['chado-analysis__sourcename'];
      $sourceversion = $items[0]['chado-analysis__sourceversion'];
      $sourceuri = $items[0]['chado-analysis__sourceuri'];
    }

    // Set defaults based on incoming form state (happens on a failed form
    // submit).
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $sourcename = $form_state['values'][$field_name]['und'][$delta]['source_data']['sourcename'];
      $sourceversion = $form_state['values'][$field_name]['und'][$delta]['source_data']['sourceversion'];
      $sourceuri = $form_state['values'][$field_name]['und'][$delta]['source_data']['sourceuri'];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => $sourcename,
    ];
    $widget['chado-analysis__sourcename'] = [
      '#type' => 'value',
      '#value' => $sourcename,
    ];
    $widget['chado-analysis__sourceversion'] = [
      '#type' => 'value',
      '#value' => $sourceversion,
    ];
    $widget['chado-analysis__sourceuri'] = [
      '#type' => 'value',
      '#value' => $sourceuri,
    ];

    $widget['source_data'] = [
      '#type' => 'fieldset',
      '#title' => $this->instance['label'],
      '#description' => $this->instance['description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    ];
    $widget['source_data']['sourcename'] = [
      '#type' => 'textfield',
      '#title' => 'Data Source Name',
      '#description' => 'The name of the source where data was obtained for this analysis.',
      '#default_value' => $sourcename,
      '#required' => $element['#required'],
    ];
    $widget['source_data']['sourceversion'] = [
      '#type' => 'textfield',
      '#title' => 'Data Source Version',
      '#description' => 'The version number of the data source (if applicable).',
      '#default_value' => $sourceversion,
    ];
    $widget['source_data']['sourceuri'] = [
      '#type' => 'textfield',
      '#title' => 'Data Source URI',
      '#description' => 'The URI (e.g. web URL) where the source data can be retreived.',
      '#default_value' => $sourceuri,
    ];

  }

  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->instance['settings']['chado_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $source_name = $form_state['values'][$field_name]['und'][$delta]['source_data']['sourcename'];
    $sourceversion = $form_state['values'][$field_name]['und'][$delta]['source_data']['sourceversion'];
    $sourceuri = $form_state['values'][$field_name]['und'][$delta]['source_data']['sourceuri'];

    // Set the value so the form element won't be ignored.
    $form_state['values'][$field_name]['und'][$delta]['value'] = $source_name;

    if (!$sourceversion) {
      $sourceversion = '__NULL__';
    }
    if (!$sourceuri) {
      $sourceuri = '__NULL__';
    }

    // And set the values for our Chado insert/update
    $form_state['values'][$field_name]['und'][$delta]['chado-analysis__sourcename'] = $source_name;
    $form_state['values'][$field_name]['und'][$delta]['chado-analysis__sourceversion'] = $sourceversion;
    $form_state['values'][$field_name]['und'][$delta]['chado-analysis__sourceuri'] = $sourceuri;
  }
}
