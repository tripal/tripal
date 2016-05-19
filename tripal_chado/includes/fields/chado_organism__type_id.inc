<?php

class chado_organism__type_id extends TripalField {

  /**
   * @see TripalField::info()
   */
  public function field_info() {
    return array(
      'label' => t('Infraspecific Type'),
      'description' => t('A field for specifying an infraspecific type.'),
      'default_widget' => 'chado_organism__type_id_widget',
      'default_formatter' => 'chado_organism__type_id_formatter',
      'settings' => array(
      ),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::attach_info()
   */
  public function attach_info($entity_type, $bundle, $settings) {

    $field_info = array();

    $table_name = $settings['data_table'];
    $type_table = $settings['type_table'];
    $type_field = $settings['field'];
    $cv_id      = $settings['cv_id'];
    $cvterm_id  = $settings['cvterm_id'];

    $schema = chado_get_schema($table_name);

    // If this is the organism table and Chado v1.3 then attach.
    if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
      $field_info = array(
        'field_name' => 'organism__type_id',
        'field_type' => 'chado_organism__type_id',
        'widget_type' => 'chado_organism__type_id_widget',
        'description' => 'Select an Infraspecific Type.',
        'label' => 'Infrapsecific Type',
        'is_required' => 0,
        'storage' => 'field_chado_storage',
        'widget_settings' => array(
          'display_label' => 1
        ),
        'field_settings' => array(
          'chado_table' => 'organism',
          'chado_column' => 'type_id',
          'semantic_web' => array(
            'name' => 'infraspecificTaxon',
            'accession' => '0000046',
            'ns' => 'TAXRANK',
            'nsurl' => 'https://github.com/phenoscape/taxrank',
          ),
        ),
      );
    }
    return $field_info;
  }

  /**
   * @see TripalField::widget_info()
   */
  public function widget_info() {
    return array(
      'label' => t('Infraspecific Type'),
      'field types' => array('chado_organism__type_id')
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  public function formatter_info() {
    return array(
      'label' => t('Infraspecific Type'),
      'field types' => array('chado_organism__type_id'),
      'settings' => array(
      ),
    );
  }

  /**
   * @see TripalField::widget()
   */
  public function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    $type_id = 0;
    if (count($items) > 0 and array_key_exists('organism__type_id', $items[0])) {
      $type_id = $items[0]['organism__type_id'];
    }

    $form['value'] = array(
      '#type' => 'value',
      '#value' =>  array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $cv = tripal_get_default_cv($field_table, $field_column);
    $options = array();
    if ($cv) {
      $options = tripal_get_cvterm_select_options($cv->cv_id);
    }
    $widget['organism__type_id'] = array(
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $type_id,
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
      '#element_validate' => array('chado_organism__type_id_widget_validate'),
    );
  }

  /**
   * @see TripalField::load()
   */
  public function load($field, $entity, $details) {

    $record = $details['record'];
    $settings = $field['settings'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'organism__type_id' => '',
    );

    if ($record->type_id) {
      $entity->{$field_name}['und'][0]['value'] = $record->type_id->name;
      $entity->{$field_name}['und'][0]['organism__type_id'] = $record->type_id->cvterm_id;
    }
  }

}


/**
 * Callback function for validating the chado_organism__type_id_widget.
 */
function chado_organism__type_id_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];
  $field = $form_state['field'][$field_name]['und']['field'];
  $settings = $field['settings'];
  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];


}