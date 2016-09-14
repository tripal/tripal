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
      'type' => 'chado_feature__md5checksum',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'md5checksum'),
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
      'label' => 'Sequence Checksum',
      'description' => 'The MD5 checksum for the sequence. The checksum here
        will always be unique for the raw unformatted sequence. To verify that the
        sequence has not been corrupted, download the raw sequence and use an MD5 tool
        to calculate the value. If the value calculated is identical the one shown
        here, then the downloaded sequence is uncorrupted.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'chado_feature__md5checksum_widget',
        'settings' => array(
          'display_label' => 1,
          'md5_fieldname' => 'feature__md5checksum',
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'chado_feature__md5checksum_formatter',
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
      'chado_feature__md5checksum_widget' => array(
        'label' => t('MD5 Checksum'),
        'field types' => array('chado_feature__md5checksum'),
      ),
    );
  }

  /**
   * @see TripalField::formatterInfo()
   */
  static function formatterInfo() {
    return array(
        'chado_feature__md5checksum_formatter' => array(
        'label' => t('MD5 Checksum'),
        'field types' => array('chado_feature__md5checksum'),
        'settings' => array(
        ),
      ),
    );
  }

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