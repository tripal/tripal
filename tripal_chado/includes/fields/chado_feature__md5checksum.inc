<?php

class chado_feature__md5checksum  extends TripalField {

  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Sequence MD5 checksum'),
      'description' => t('A field for generating MD5 checksum for a sequence.'),
      'default_widget' => 'chado_feature__md5checksum_widget',
      'default_formatter' => 'chado_feature__md5checksum_formatter',
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
  protected function can_attach($entity_type, $bundle, $details) {
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    if ($table_name == 'feature') {
      return TRUE;
    }
    return FALSE;
  }
  /**
   * @see TripalField::create_info()
   */
  function create_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '__md5checksum',
      'type' => 'chado_feature__md5checksum',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'semantic_web' => 'data:2190',
      ),
    );
  }
  /**
   * @see TripalField::create_instance_info()
   */
  function create_instance_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '__md5checksum',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence Checksum',
      'description' => 'The MD5 checksum for the sequence. The checksum here
        will always be unique for the raw unformatted sequence. To verify that the
        sequence has not been corrupted, download the raw sequence and use an MD5 tool
        to calculate the value. If the value calculated is identical the one shown
        here, then the downloaded sequence is uncorrupted.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_feature__md5checksum_widget',
        'settings' => array(
          'display_label' => 1,
          'md5_fieldname' => 'feature__md5checksum',
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_feature__md5checksum_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('MD5 Checksum'),
      'field types' => array('chado_feature__md5checksum'),
    );
  }

  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('MD5 Checksum'),
      'field types' => array('chado_feature__md5checksum'),
      'settings' => array(
      ),
    );
  }

  /**
   * @see TripalField::formatter_view()
   */
  function formatter_view(&$element, $entity_type, $entity, $field,
      $instance, $langcode, $items, $display) {
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
   * @see TripalField::widget_form()
   */
  public function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the field defaults.
    $md5checksum = '';
    if (count($items) > 0 and array_key_exists('feature__md5checksum', $items[0])) {
      $md5checksum = $items[0]['feature__md5checksum'];
    }
    if (array_key_exists('values', $form_state)) {
      $md5checksum = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'feature__md5checksum');
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['feature__md5checksum'] = array(
      '#type' => 'value',
      '#value' => $md5checksum,
      '#element_validate' => array('chado_feature__md5checksum_widget_validate'),
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
  $residues = tripal_chado_get_field_form_values('feature__residues', $form_state, 0, 'feature__residues');
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, md5($residues), 0, 'feature__md5checksum');
  }
  else {
    // Otherwise, remove the md5 value
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__', 0, $field_table . '__md5checksum');
  }
}