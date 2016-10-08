<?php

class chado_feature__seqlen extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Sequence length';

  // The default description for this field.
  public static $default_description = 'A field for calculating the length of a sequence.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

   /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

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
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-feature__seqlen'] =  array(
      '#type' => 'value',
      '#value' => 0,
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    );
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($entity_type, $entity, $langcode, &$items, $form, &$form_state) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the residues so we can calculate teh length.
    $residues = $form_state['values']['feature__residues']['und'][0]['value']['residues'];
    // Remove any white spaces.
    if ($residues) {
      $residues = preg_replace('/\s/', '', $residues);
      $form_state['values']['feature__residues']['und'][0]['value']['chado-feature__residues'] = $residues;
      $items[0]['chado-feature__seqlen'] = strlen($residues);
    }
    else {
      $items[0]['chado-feature__seqlen'] = '__NULL__';
    }
  }
}