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
 * Implements hook_create_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_field_create_info($entity_type, $bundle, $term) {
  return array(
    'content_type' => array(
      'field_name' => 'content_type',
      'type' => 'content_type',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'tripal_no_storage'
      ),
      'settings' => array(
        'semantic_web' => 'rdf:type',
      ),
    ),
  );
}

/**
 * Implements hook_create_instance_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_field_create_instance_info($entity_type, $bundle, $term) {
  return array(
    'content_type' => array(
      'field_name' => 'content_type',
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => 'Resource Type',
      'description' => '',
      'required' => FALSE,
      'settings' => array(),
      'widget' => array(
        'type' => 'tripal_content_type_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'tripal_content_type_formatter',
          'settings' => array(),
        ),
      ),
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

/**
 *
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_field_edit_form is used for customizing the settings of
 * a field attached to an entity.
 *
 * This alter function disables some of the form widgets when the storage
 * backend indicates they are not appropriate.
 */
function tripal_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {

  // For entity fields added by Tripal Entities we don't want the
  // the end-user to change the cardinality and the required fields
  // such that record can't be saved in Chado.

  // TODO: this shouldn't be hardcoded here.  These settings
  // should be part of the field and handled by the tripal_entity module.
  if ($form['#instance']['entity_type'] == 'TripalEntity') {
    $form['field']['cardinality']['#access'] = FALSE;
    $form['instance']['required']['#access'] = FALSE;
  }

  // TODO: don't the the maximum length be larger than the field size.
}

/**
 *
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_field_overview_form_ is used for adding and reordering the
 * fields attached to a bundle.  It also includes edit and delete links and
 * links for editing field types and widgets.
 *
 * This alter function is used to add a new 'Supported By' column to
 * the table to let the user know where fields are storing their data.
 */
function tripal_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {
  // Add the 'Storage Location' to the table header.
  $form['fields']['#header'][] = 'Supported By * ';

  // Add the storage location as the final column for each field.
  $storage_info = module_invoke_all('field_storage_info');
  foreach (element_children($form['fields']) as $field_name) {
    $field = field_info_field($field_name);
    // For rows in the tables that aren't fields, just add an empty value
    // for the storage column.
    if (!$field) {
      $form['fields'][$field_name][] = array(
        '#markup' => '',
      );
      continue;
    }
    $storage_type = $field['storage']['type'];
    $storage_label = array_key_exists('label', $storage_info[$storage_type]) ? $storage_info[$storage_type]['label'] : '';
    if ($storage_type == 'field_sql_storage') {
      $storage_label = 'Drupal';
    }
    if (array_key_exists('logo_url', $storage_info[$storage_type])) {
      $logo_url = $storage_info[$storage_type]['logo_url'];
      $form['fields'][$field_name][] = array(
        '#markup' => '<img class="form-field-ui-field-overview-storage-logo" src="' . $logo_url . '">',
      );
    }
    else {
      $form['fields'][$field_name][] = array(
        '#markup' => $storage_label,
      );
    }
  }
  $form['note'] = array(
    '#markup' =>  '* Fields attached to this content type can use various
      storage backends. Please be sure when you add new fields that the
      storage backend is appropriate. For example, if you use Chado, and you
      want all biological content to be stored in Chado, be sure that the
      respective fields are "supported by" Chado.',
  );
}