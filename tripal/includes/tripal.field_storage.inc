<?php

/**
 * Implements hook_field_storage_info().
 *
 * The Tripal module does not provide a storage back-end.  But it does provide
 * a placeholder when no storage backend is needed but a field
 * is still desired.  The 'tripal_no_storage' backend is used for the
 * content_type field which adds a type field to every entity.
 */
function tripal_field_storage_info() {
  return array(
    'tripal_no_storage' => array(
      'label' => t('Tripal'),
      'description' => t('The NULL storage is a placeholder for field values
          that are not stored in any storage backend (e.g. entity types).'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_storage_load().
 *
 * Responsible for loading the fields from the Chado database and adding
 * their values to the entity.
 */
function tripal_field_storage_load($entity_type, $entities, $age,
    $fields, $options) {

  $load_current = $age == FIELD_LOAD_CURRENT;
  global $language;
  $langcode = $language->language;

  foreach ($entities as $id => $entity) {

    // Iterate through the entity's fields so we can get the column names
    // that need to be selected from each of the tables represented.
    $tables = array();
    foreach ($fields as $field_id => $ids) {

      // By the time this hook runs, the relevant field definitions have been
      // populated and cached in FieldInfo, so calling field_info_field_by_id()
      // on each field individually is more efficient than loading all fields in
      // memory upfront with field_info_field_by_ids().
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $field_type = $field['type'];
      $field_module = $field['module'];

      // Allow the creating module to alter the value if desired.  The
      // module should do this if the field has any other form elements
      // that need populationg besides the default value.
      $load_function = $field_module . '_' . $field_type . '_field_load';
      module_load_include('inc', $field_module, 'includes/fields/' . $field_type);
      if (function_exists($load_function)) {
        $load_function($field, $entity);
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}

/**
 * Implements hook_field_storage_query().
 *
 * Used by EntityFieldQuery to find the entities having certain entity
 * and field conditions and sort them in the given field order.
 *
 * NOTE: This function needs to exist or errors are triggered but so far it doesn't
 * appear to actually need to do anything...
 */
function tripal_field_storage_query($query) {

}