<?php

/**
 * Implements hook_field_info().
 */
function tripal_fields_field_info() {
  $fields = array(
    'organism_id' => array(
      'label' => t('Organism'),
      'description' => t('A field for specifying an organism.'),
      'default_widget' => 'tripal_fields_organism_select_widget',
      'default_formatter' => 'tripal_fields_organism_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'dbxref_id' => array(
      'label' => t('Cross-reference'),
      'description' => t('This record can be cross-referenced with a record in another online database. This field is intended for the most prominent reference.  At a minimum, the database and accession must be provided.'),
      'default_widget' => 'tripal_fields_primary_dbxref_widget',
      'default_formatter' => 'tripal_fields_primary_dbxref_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'residues' => array(
      'label' => t('Residues'),
      'description' => t('A field for managing nucleotide and protein residues.'),
      'default_widget' => 'tripal_fields_residue_textarea_widget',
      'default_formatter' => 'tripal_fields_residues_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'md5checksum' => array(
      'label' => t('MD5 checksum'),
      'description' => t('A field for generating MD5 checksum for a sequence.'),
      'default_widget' => 'tripal_fields_md5checksum_checkbox_widget',
      'default_formatter' => 'tripal_fields_md5checksum_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'seqlen' => array(
      'label' => t('Sequence length'),
      'description' => t('A field for calculating the length of a sequence.'),
      'default_widget' => 'tripal_fields_seqlen_hidden_widget',
      'default_formatter' => 'tripal_fields_seqlen_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    // The field provides a widget for adding new properties
    // to an entity that is connected to a base table that has a prop table
    // in Chado.
    'kvproperty' => array(
      'label' => t('Add a Property Type'),
      'description' => t('This record may have any number of properties. Use this field to first add the type.'),
      'default_widget' => 'tripal_fields_kvproperty_widget',
      'default_formatter' => 'tripal_fields_kvproperty_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
  );
  return $fields;
}

/**
 * Implements hook_field_widget_info().
 */
function tripal_fields_field_widget_info() {
  return array(
    'tripal_fields_organism_select_widget' => array(
      'label' => t('Organism Select'),
      'field types' => array('organism_id')
    ),
    'tripal_fields_primary_dbxref_widget' => array(
      'label' => t('Cross-reference'),
      'field types' => array('dbxref_id'),
      'description' => t('This record can be cross-referenced with a record in another online database. This field is intended for the most prominent reference.  At a minimum, the database and accession must be provided.'),
    ),
    'tripal_fields_md5checksum_checkbox_widget' => array(
      'label' => t('MD5 Checksum Checkbox'),
      'field types' => array('md5checksum'),
    ),
    'tripal_fields_residues_textarea_widget' => array(
      'label' => t('Residues'),
      'field types' => array('residues'),
    ),
    'tripal_fields_seqlen_hidden_widget' => array(
      'label' => t('Sequence Length'),
      'field types' => array('seqlen'),
    ),
    'tripal_fields_kvproperty_widget' => array(
      'label' => t('Property'),
      'field types' => array('kvproperty'),
    ),
  );
}
/**
 * Implements hook_field_formatter_info().
 */
function tripal_fields_field_formatter_info() {
  return array(
    'tripal_fields_organism_formatter' => array(
      'label' => t('Organism'),
      'field types' => array('organism_id')
    ),
    'tripal_fields_primary_dbxref_formatter' => array(
      'label' => t('Cross-reference'),
      'field types' => array('dbxref_id')
    ),
    'tripal_fields_md5checksum_formatter' => array(
      'label' => t('MD5 checksum'),
      'field types' => array('md5checksum')
    ),
    'tripal_fields_residues_formatter' => array(
      'label' => t('Residues'),
      'field types' => array('residues')
    ),
    'tripal_fields_seqlen_formatter' => array(
      'label' => t('Sequence length'),
      'field types' => array('seqlen')
    ),
    'tripal_fields_kvproperty_formatter' => array(
      'label' => t('Property'),
      'field types' => array('kvproperty')
    ),
  );
}
/**
 * Implements hook_field_formatter_view().
 *
 * Two formatters are implemented.
 * - field_example_simple_text just outputs markup indicating the color that
 *   was entered and uses an inline style to set the text color to that value.
 * - field_example_color_background does the same but also changes the
 *   background color of div.region-content.
 *
 * @see field_example_field_formatter_info()
 */
function tripal_fields_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $element = array();
  switch ($display['type']) {

    case 'tripal_fields_organism_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/fields/organism_id');
      tripal_fields_organism_select_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_fields_primary_dbxref_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/fields/primary_dbxref');
      tripal_fields_primary_dbxref_formatter($element, $entity_type, $entity, $field,
        $instance, $langcode, $items, $display);
      break;
    case 'tripal_fields_md5checksum_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/fields/md5checksum');
      tripal_fields_md5checksum_checkbox_formatter($element, $entity_type, $entity, $field,
        $instance, $langcode, $items, $display);
      break;
    case 'tripal_fields_residues_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/fields/residues');
      tripal_fields_residues_textarea_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_fields_seqlen_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/fields/seqlen');
      tripal_fields_seqlen_hidden_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_fields_kvproperty_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/fields/kvproperty');
      tripal_fields_kvproperty_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
  }
  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_fields_field_widget_form(&$form, &$form_state, $field,
  $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  $widget['#delta'] = $delta;
  $field_name = $field['field_name'];

  switch ($instance['widget']['type']) {
    case 'tripal_fields_organism_select_widget':
      // Make sure the include files get parsed now and for the form submits.
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/organism_id');
      module_load_include('inc', 'tripal_fields', 'includes/fields/organism_id');
      // Update the widget with the new field.
      tripal_fields_organism_select_widget($field_name, $widget, $form,
          $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_primary_dbxref_widget':
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/primary_dbxref');
      module_load_include('inc', 'tripal_fields', 'includes/fields/primary_dbxref');
      tripal_fields_primary_dbxref_widget($field_name, $widget, $form,
        $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_md5checksum_checkbox_widget':
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/md5checksum');
      module_load_include('inc', 'tripal_fields', 'includes/fields/md5checksum');
      tripal_fields_md5checksum_checkbox_widget($field_name, $widget,
          $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_residues_textarea_widget':
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/residues');
      module_load_include('inc', 'tripal_fields', 'includes/fields/residues');
      tripal_fields_residues_textarea_widget($field_name, $widget,
          $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_seqlen_hidden_widget':
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/seqlen');
      module_load_include('inc', 'tripal_fields', 'includes/fields/seqlen');
      tripal_fields_seqlen_hidden_widget($field_name, $widget,
          $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_kvproperty_widget':
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/kvproperty');
      module_load_include('inc', 'tripal_fields', 'includes/fields/kvproperty');
      tripal_fields_kvproperty_widget($field_name, $widget,
          $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
  }
  return $element;
}


/**
 * Implements hook_field_is_empty().
 */
function tripal_fields_field_is_empty($item, $field) {
  if (empty($item['value']) && (string) $item['value'] !== '0') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Returns the values of the field from the $form_state.
 */
function tripal_fields_get_field_form_values($field_name, $form_state, $child = NULL) {
  $values = array();
  if (!array_key_exists('values', $form_state)) {
    return $values;
  }
  if (array_key_exists($field_name, $form_state['values'])) {
    foreach ($form_state['values'][$field_name] as $langcode => $items) {
      foreach ($items as $delta => $value) {
        if ($child and array_key_exists($child, $value['value'][0]) and $value['value'][0][$child]) {
          $values[] = $value['value'][0][$child];
        }
        else if (!$child and $value['value']) {
          $values[] = $value['value'];
        }
      }
    }
  }
  return $values;
}

/**
 * Returns the values of the field from the $form_state.
 */
function tripal_fields_set_field_form_values($field_name, &$form_state, $newvalue, $child = NULL) {
  $values = array();
  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    foreach ($items as $delta => $value) {
      if ($child and array_key_exists($child, $value['value'][0]) and $value['value'][0][$child]) {
        $form_state['values'][$field_name][$langcode][$delta]['value'][0][$child] = $newvalue;
      }
      else if (!$child) {
        $form_state['values'][$field_name][$langcode][$delta]['value'] = $newvalue;
      }
    }
  }
  return $values;
}

