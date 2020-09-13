<?php

class content_type extends TripalField {

  // The default label for this field.
  public static $default_label = 'Content Type';

  // The default description for this field.
  public static $default_description = 'The Tripal content type.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = [];

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'tripal_no_storage';


  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
    $entity->{$this->field['field_name']}['und'][0]['value'] = $bundle->label;
  }

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

    foreach ($items as $delta => $item) {
      $element[$delta] = [
        '#type' => 'markup',
        '#markup' => $item['value'],
      ];
    }
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
  }
}