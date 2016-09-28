<?php

class chado_feature__md5checksum  extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Sequence MD5 checksum';

  // The default description for this field.
  public static $default_description = 'A field for generating MD5 checksum for a sequence.';

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
      $content = key_exists('value', $item) ? $item['value'] : '';
      $element[$delta] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the field defaults.
    $md5checksum = '';
    if (count($items) > 0 and array_key_exists('feature__md5checksum', $items[0])) {
      $md5checksum = $items[0]['feature__md5checksum'];
    }
    if (array_key_exists('values', $form_state)) {
      //$md5checksum = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'feature__md5checksum');
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['feature__md5checksum'] = array(
      '#type' => 'value',
      '#value' => $md5checksum,
//      '#element_validate' => array('chado_feature__md5checksum_widget_validate'),
    );
  }

}

/**
 * Callback function for validating the chado_feature__md5checksum_widget.
 */
function chado_feature__md5checksum_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];
  $field = $form_state['field'][$field_name]['und']['field'];
  $settings = $field['settings'];
  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];

  // Calculate the md5 checksum for the sequence only if md5 box is checked and
  // the residues exist.
  //$residues = tripal_chado_get_field_form_values('feature__residues', $form_state, 0, 'feature__residues');
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, md5($residues), 0, 'feature__md5checksum');
  }
  else {
    // Otherwise, remove the md5 value
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__', 0, $field_table . '__md5checksum');
  }
}