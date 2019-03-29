<?php

/**
 * Implements hook_field_storage_info().
 */
function tripal_ws_field_storage_info() {
  return [
    'field_tripal_ws_storage' => [
      'label' => t('Tripal Web Services'),
      'description' => t('Retrieves fields data from a remote site using Tripal web services.'),
      'settings' => [],
    ],
  ];
}

/**
 * Implements hook_field_storage_load().
 *
 * Responsible for loading the fields from the Chado database and adding
 * their values to the entity.
 */
function tripal_ws_field_storage_load($entity_type, $entities, $age,
                                      $fields, $options) {

  $load_current = $age == FIELD_LOAD_CURRENT;
  global $language;
  $langcode = $language->language;

  foreach ($entities as $id => $entity) {

    // Iterate through the entity's fields so we can get the column names
    // that need to be selected from each of the tables represented.
    foreach ($fields as $field_id => $ids) {

      // By the time this hook runs, the relevant field definitions have been
      // populated and cached in FieldInfo, so calling field_info_field_by_id()
      // on each field individually is more efficient than loading all fields in
      // memory upfront with field_info_field_by_ids().
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $field_type = $field['type'];
      $field_module = $field['module'];

      // Get the instance for this field.  If no instance exists then skip
      // loading of this field. This can happen when a field is deleted from
      // a bundle using the user UI form.
      // TODO: how to deal with deleted fields?
      $instance = field_info_instance($entity_type, $field_name, $entity->bundle);

      if (!$instance) {
        continue;
      }

      // Set an empty value by default, and let the hook function update it.
      $entity->{$field_name}['und'][0]['value'] = '';
      tripal_load_include_field_class($field_type);
      if (class_exists($field_type) && method_exists($field_type, 'load')) {
        $tfield = new $field_type($field, $instance);
        $tfield->load($entity);
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}

/**
 * Implements hook_field_storage_query().
 */
function tripal_ws_field_storage_query($query) {

}

/**
 * Implements hook_field_storage_write().
 */
function tripal_ws_field_storage_write($entity_type, $entity, $op, $fields) {

}

