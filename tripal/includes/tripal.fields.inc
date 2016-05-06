<?php

/**
 * Implements hook_field_info().
 */
function tripal_field_info() {
  $fields = array(
    'content_type' => array(
      'label' => t('Record Type'),
      'description' => t('The content type.'),
      'default_widget' => 'tripal_content_type_widget',
      'default_formatter' => 'tripal_content_type_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'tripal_no_storage',
        'module' => 'tripal',
        'active' => TRUE
      ),
    ),
  );
  return $fields;
}
/**
 * Implements hook_field_widget_info();
 */
function tripal_field_widget_info() {
  return array(
    'tripal_content_type_widget' => array(
      'label' => t('Record Type'),
      'field types' => array('content_type')
    ),
  );
}

/**
 * Implements hook_field_formatter_info().
 */
function tripal_field_formatter_info() {
  return array(
    'tripal_content_type_formatter' => array(
      'label' => t('Record Type'),
      'field types' => array('content_type')
    ),
  );
}
/**
 * Implements hook_field_widget_form().
 */
function tripal_field_widget_form(&$form, &$form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  switch ($instance['widget']['type']) {
    case 'tripal_content_type_widget':
      // There is no widget for this type.
      break;
  }
}
/**
 * Implements hook_field_formatter_view().
 */
function tripal_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $element = array();
  switch ($display['type']) {
    case 'tripal_content_type_formatter':
      module_load_include('inc', 'tripal', 'includes/fields/content_type');
      tripal_content_type_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
  }
  return $element;
}
/**
 * Implements hook_field_is_empty().
 */
function tripal_field_is_empty($item, $field) {

  // If there is no value field then the field is empty.
  if (!array_key_exists('value', $item)) {
    return TRUE;
  }

  // Iterate through all of the fields and if at least one has a value
  // the field is not empty.
  foreach ($item as $form_field_name => $value) {
    if (isset($value) and $value != NULL and $value != '') {
      return FALSE;
    }
  }

  // Otherwise, the field is empty.
  return TRUE;
}

/**
 * Simple provides a message indicating that the field cannot be deleted.
 */
function tripal_field_no_delete() {
  drupal_set_message('This field cannot be removed.', 'warning');
  return '';
}