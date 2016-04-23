<?php
/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_feature__residues_info() {
  return array(
    'label' => t('Residues'),
    'description' => t('A field for managing nucleotide and protein residues.'),
    'default_widget' => 'chado_feature__residues_widget',
    'default_formatter' => 'chado_feature__residues_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_attach_info().
 *
 * This is a hook provided by the tripal_Chado module. It allows the field
 * to specify which bundles it will attach to and to specify thee settings.
 *
 * @param $entity_type
 * @param $entity
 * @param $term
 *
 * @return
 *   A field array
 */
function chado_feature__residues_attach_info($entity_type, $bundle, $target) {
  $field_info = array();

  $table_name = $target['data_table'];
  $type_table = $target['type_table'];
  $type_field = $target['field'];
  $cv_id      = $target['cv_id'];
  $cvterm_id  = $target['cvterm_id'];


  // If  this is not the feature table then we don't want to attach.
  if ($table_name != 'feature') {
    return $field_info;
  }

  $field_info = array(
    'field_name' => 'feature__residues',
    'field_type' => 'chado_feature__residues',
    'widget_type' => 'chado_feature__residues_widget',
    'description' => 'An IUPAC compatible residues for this feature.',
    'label' => 'Residues',
    'is_required' => 0,
    'storage' => 'field_chado_storage',
    'widget_settings' => array(
      'display_label' => 1
    ),
    'field_settings' => array(
      'chado_table' => $table_name,
      'chado_column' => 'residues',
      'semantic_web' => array(
        'type' => '',
        'ns' => '',
        'nsurl' => '',
      ),
    ),
  );
  return $field_info;
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_feature__residues_widget_info() {
  return array(
    'label' => t('Residues'),
    'field types' => array('chado_feature__residues'),
  );
}
/**
 * Implements hook_formatter_info.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_info() for each field
 * to specify.
 *
 */
function chado_feature__residues_formatter_info() {
  return array(
    'label' => t('Residues'),
    'field types' => array('chado_feature__residues'),
  );
}
/**
 *
 * @param unknown $entity_type
 * @param unknown $entity
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $display
 */
function chado_feature__residues_formatter(&$element, $entity_type, $entity, $field,
  $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    $residues = key_exists('value', $item) ? $item['value'] : '';
    $content = '<pre class="residues-formatter">' . $residues . '</pre>';
    $element[$delta] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}

/**
 *
 * @param unknown $field_name
 * @param unknown $widget
 * @param unknown $form
 * @param unknown $form_state
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $delta
 * @param unknown $element
 */
function chado_feature__residues_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $widget['value'] = array(
    '#type' => 'textarea',
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#default_value' => count($items) > 0 ? $items[0]['value'] : '',
    '#delta' => $delta,
    '#element_validate' => array('chado_feature__residues_widget_validate'),
    '#cols' => 30,
  );
}
/**
 * Callback function for validating the chado_feature__residues_widget.
 */
function chado_feature__residues_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // Remove any white spaces.
  $residues = tripal_chado_get_field_form_values($field_name, $form_state);
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, $residues);
  }
}