<?php

class content_type extends TripalField {

  /**
   * @see TripalField::setFieldName()
   */
  protected function setFieldName() {
    $this->field_name = 'content_type';
  }

  /**
   * @see TripalField::setCanAttach()
   */
  protected function setCanAttach() {
    // We always attach to TriplEntity entities.
    if ($this->entity_type == 'TripalEntity') {
      $this->can_attach = TRUE;
    }
  }
  /**
   * @see TripalField::fieldInfo()
   */
  static function fieldInfo() {
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
   * @see TripalField::createFieldInfo()
   */
  public function createInfo() {
    if (!$this->can_attach) {
      return;
    }

    return array(
      'field_name' => $this->field_name,
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
   * @see TripalField::createInstanceInfo()
   */
  public function createInstanceInfo() {

    if (!$this->can_attach) {
      return;
    }

    return array(
      'field_name' => $this->field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $this->bundle->name,
      'label' => 'Resource Type',
      'description' => '',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
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
   * @see TripalField::widgetInfo()
   */
  static function widgetInfo() {
    return array(
      'tripal_content_type_widget' => array(
        'label' => t('Record Type'),
        'field types' => array('content_type')
      ),
    );
  }
  /**
   * @see TripalField::formatterInfo()
   */
  static function formatterInfo() {
    return array(
      'tripal_content_type_formatter' => array(
        'label' => t('Record Type'),
        'field types' => array('content_type')
      ),
    );
  }

  /**
   * @see TripalField::load()
   */
  static function load($field, $entity, $details = array()) {

    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
    $entity->{$field['field_name']}['und'][0]['value'] = $bundle->label;
  }

  /**
   * @see TripalField::formatterView()
   */
  static function formatterView(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
      );
    }
  }
  /**
   * @see TripalField::widgetForm()
   */
  static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $widget = $element;
    switch ($instance['widget']['type']) {
      case 'tripal_content_type_widget':
        // There is no widget for this type.
        break;
    }

  }

}