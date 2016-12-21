<?php

class obi__organism_widget extends ChadoFieldWidget {

  // The default lable for this field.
  public static $default_label = 'Organism';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('obi__organism');


  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    $organism_id = 0;
    if (count($items) > 0 and array_key_exists('chado-' . $field_table . '__organism_id', $items[0])) {
      $organism_id = $items[0]['chado-' . $field_table . '__organism_id'];
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $options = tripal_get_organism_select_options(FALSE);
    $widget['chado-' . $field_table . '__organism_id'] = array(
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $organism_id,
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    );
  }
}
