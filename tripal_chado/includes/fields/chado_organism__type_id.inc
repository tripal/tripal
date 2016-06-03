<?php

class chado_organism__type_id extends TripalField {

  /**
   * @see TripalField::info()
   */
  public static function fieldInfo() {
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
   * @see TripalField::can_attach()
   */
  protected function setCanAttach(){
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $schema = chado_get_schema($table_name);

    // If this is the organism table and Chado v1.3 then attach.
    if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
      $this->can_attach = TRUE;
      return;
    }
    $this->can_attach = FALSE;
  }
  /**
   * @see TripalField::setFieldName()
   */
  protected function setFieldName() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $this->field_name = 'organism__type_id';
  }
  /**
   * @see TripalField::create_info()
   */
  function createInfo() {
    if (!$this->can_attach) {
      return;
    }

    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_organism__type_id',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'organism',
        'chado_column' => 'type_id',
        'semantic_web' => 'local:infraspecific_type',
      ),
    );
  }
  /**
   * @see TripalField::createInstanceInfo()
   */
  function createInstanceInfo() {
    if (!$this->can_attach) {
      return;
    }

    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    return array(
      'field_name' => $this->field_name,
      'entity_type' => $this->entity_type,
      'bundle' => $this->bundle->name,
      'label' => 'Infrapsecific Type',
      'description' => 'The Infrapsecific Type.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'chado_organism__type_id_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_organism__type_id_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::widgetInfo()
   */
  public static function widgetInfo() {
    return array(
      'chado_organism__type_id_widget' => array(
        'label' => t('Infraspecific Type'),
        'field types' => array('chado_organism__type_id'),
      ),
    );
  }
  /**
   * @see TripalField::formatterInfo()
   */
  public static function formatterInfo() {
    return array(
      'chado_organism__type_id_formatter' => array(
        'label' => t('Infraspecific Type'),
        'field types' => array('chado_organism__type_id'),
        'settings' => array(
        ),
      ),
    );
  }

  /**
   * @see TripalField::widget()
   */
  public static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
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
  static function load($field, $entity, $details = array()) {

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