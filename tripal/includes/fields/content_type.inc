<?php

class content_type extends TripalField {
  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Record Type'),
      'description' => t('The content type.'),
      'default_widget' => 'tripal_content_type_widget',
      'default_formatter' => 'tripal_content_type_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'tripal_no_storage',
        'module' => 'tripal',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'tripal_content_type_widget' => array(
        'label' => t('Record Type'),
        'field types' => array('content_type')
      ),
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'tripal_content_type_formatter' => array(
        'label' => t('Record Type'),
        'field types' => array('content_type')
      ),
    );
  }
  /**
   * @see TripalField::can_attach()
   */
  protected function can_attach($entity_type, $bundle, $details) {
    // We always attach to TriplEntity entities.
    if ($entity_type == 'TripalEntity') {
      return TRUE;
    }
  }

  /**
   * @see TripalField::create_info()
   */
  public function create_info($entity_type, $bundle, $details) {

    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    return array(
      'field_name' => 'content_type',
      'type' => 'content_type',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'tripal_no_storage'
      ),
      'settings' => array(
        'semantic_web' => 'rdfs:type',
      ),
    );
  }
  /**
   * @see TripalField::create_instance_info()
   */
  public function create_instance_info($entity_type, $bundle, $details) {

    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    return array(
      'field_name' => 'content_type',
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => 'Resource Type',
      'description' => '',
      'required' => FALSE,
      'settings' => array(),
      'widget' => array(
        'type' => 'tripal_content_type_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'tripal_content_type_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::formatter_view()
   */
  function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
      );
    }
  }
  /**
   * @see TripalField::widget_form()
   */
  function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $widget = $element;
    switch ($instance['widget']['type']) {
      case 'tripal_content_type_widget':
        // There is no widget for this type.
        break;
    }

  }
  /**
   * @see TripalField::load()
   */
  function load($field, $entity) {

    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
    $field_name = $field['field_name'];
    $entity->{$field_name}['und'][0]['value'] = $bundle->label;
  }
}