<?php

class taxrank__infraspecific_taxon_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Infraspecific Taxon';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('taxrank__infraspecific_taxon');

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
    );
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }
}
