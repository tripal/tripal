<?php

/**
 * Adds a field to an Entity bundle.
 *
 * @param $field_name
 *   The name of the field.
 * @param $field_info
 *   An associative array containing the field information.  The following
 *   key/value pairs are supported:
 *     'field_type' : a valid field type.  May be any of the Drupal default
 *       fields, one created by the tripal_chado module or another custom module.
 *     'widget_type' : a valid widget type. May be any of the Drupal default
 *       fields, one created by the tripal_chado module or another custom module.
 *     'field_settings' : an array of settings that are appropriate for the
 *       selected field type.
 *     'widget_settings' : an array of settings that are appropriate for the
 *       selected widget type.
 *     'description' :  a default description for this field.
 *     'label' : a label used as a header for this field.
 *     'is_required' : indicates if the field is required in the edit form.
 *     'cardinality' : indicates the number of values this field can support.
 *       the default is 1 (meaning only one value). Use a value of
 *       FIELD_CARDINALITY_UNLIMITED for unlimited number of values.
 * @param $entity_type_name
 *   The entity type name.
 * @param $bundle_name
 *   The bundle name.
 *
 */
function tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name) {

  $field = field_info_field($field_name);

  // If the field exists and is attached to this bundle then just return,
  // there is nothing left to do.
  if ($field and array_key_exists('bundles', $field) and
      array_key_exists($entity_type_name, $field['bundles']) and
      in_array($bundle_name, $field['bundles'][$entity_type_name])) {
    return;
  }

  // Allow other modules to alter the field information array.
  drupal_alter('chado_field', $field_info);

  $cardinality = 1;
  if (array_key_exists('cardinality', $field_info) and is_numeric($field_info['cardinality'])) {
    $cardinality = $field_info['cardinality'];
  }

  // If the field doesn't exist then create it.
  if (!$field) {
    $field = array(
      'field_name' => $field_name,
      'type' => $field_info['field_type'],
      'cardinality' => $cardinality,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage'
      ),
      'settings' => $field_info['field_settings'],
    );
    field_create_field($field);
  }

  // Attach the field to the bundle.
  $field_instance = array(
    'field_name' => $field_name,
    'label' => $field_info['label'],
    'description' => $field_info['description'],
    'widget' => array(
      'type' => $field_info['widget_type'],
      'settings' => $field_info['widget_settings'],
    ),
    'entity_type' => $entity_type_name,
    'required' => $field_info['is_required'],
    'settings' => $field_info['field_settings'],
    'bundle' => $bundle_name,
  );
  field_create_instance($field_instance);
}