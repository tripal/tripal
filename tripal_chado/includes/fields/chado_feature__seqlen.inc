<?php

class chado_feature__seqlen extends TripalField {
  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Sequence length'),
      'description' => t('A field for calculating the length of a sequence.'),
      'default_widget' => 'chado_feature__seqlen_widget',
      'default_formatter' => 'chado_feature__seqlen_formatter',
      'settings' => array(),
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
      'field_name' => 'feature__seqlen',
      'type' => 'chado_feature__seqlen',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'seqlen',
        'semantic_web' => 'data:1249',
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
      'field_name' => 'feature__seqlen',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Raw Sequence Length',
      'description' => 'The number of residues in the raw sequence.  This length
        is only for the assigned raw sequence and does not represent the length of any
        sequences derived from alignments. If this value is zero but aligned sequences
        are present then this record has no official assigned sequence.',
      'required' => FALSE,
      'settings' => array(),
      'widget' => array(
        'type' => 'chado_feature__seqlen_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_feature__seqlen_formatter',
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
      'label' => t('Sequence Length'),
      'field types' => array('chado_feature__seqlen'),
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('Residues Length'),
      'field types' => array('chado_feature__seqlen'),
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
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
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

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['feature__seqlen'] =  array(
      '#type' => 'value',
      '#value' => 0,
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
      '#element_validate' => array('chado_feature__seqlen_widget_validate'),
    );
  }

}

/**
 * Callback function for validating the chado_feature__seqlen_widget.
 */
function chado_feature__seqlen_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // Get the residues so we can calculate teh length.
  $residues = tripal_chado_get_field_form_values('feature__residues', $form_state, 0, 'feature__residues');
  // Remove any white spaces.
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, strlen($residues), 0, 'feature__seqlen');
  }
  else {
    // Otherwise, remove the seqlen value
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__', 0, 'feature_seqlen');
  }
}