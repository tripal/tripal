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
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the field defaults.
    $md5checksum = '';
    if (count($items) > 0 and array_key_exists('chado-feature__md5checksum', $items[0])) {
      $md5checksum = $items[0]['chado-feature__md5checksum'];
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => $md5checksum,
    );
    $widget['chado-feature__md5checksum'] = array(
      '#type' => 'value',
      '#value' => $md5checksum,
    );
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field = $this->field;
    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the residues so we can calculate teh length.
    $residues = $form_state['values']['feature__residues'][$langcode][0]['chado-feature__residues'];

    if ($residues) {
      // Remove spaces and new lines from the residues string.
      $residues = preg_replace('/\s/', '', $residues);
      $form_state['values']['feature__residues'][$langcode][0]['chado-feature__residues'] = $residues;
      $form_state['values'][$field_name][$langcode][$delta]['chado-feature__md5checksum'] = md5($residues);
    }
    else {
      // Otherwise, remove the md5 value
      $form_state['values'][$field_name][$langcode][$delta]['chado-feature__md5checksum'] = '__NULL__';
    }
  }
}
