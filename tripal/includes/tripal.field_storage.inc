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
      // that need populationg besides the value which was set above.
      module_load_include('inc', $field_module, 'includes/fields/' . $field_type);
      if (class_exists($field_type)) {
        $field_type::load($field, $entity);
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}

/**
 * Implements hook_field_storage_query().
 *
 * Used by EntityFieldQuery to find the entities having certain field values.
 *
 * We do not support use of the EntityFieldQuery API for Tripal based fields
 * because EFQ doesn't support when multiple storage backends are used. Instead
 * use the TripalFieldQuery class and implement the hook_storage_tquery()
 * function.
 */
function tripal_field_storage_query($query) {

}

/**
 * Implements hook_field_storage_tquery().
 *
 * Used by TripalFieldQuery to find the entities having certain field values.
 *
 * @param $conditions
 */
function tripal_field_storage_tquery($conditions) {

  $filter = array();
  $entity_ids = array();
  foreach ($conditions as $index => $condition) {

    $field = $condition['field'];
    $field_type = $field['type'];
    $field_module = $field['module'];
    $settings = $field['settings'];
    $operator = $condition['operator'];

    // This module only supports one field, so we can just perform the filter.
    // using the appropriate operator.
    $query = db_select('tripal_entity', 'TE');
    $query->join('chado_entity', 'CE', 'TE.id = CE.entity_id');
    $query->fields('CE', array('entity_id'));
    $query->condition('TE.term_id', $condition['value'], $operator);
    $results = $query->execute();
    $filter_ids = array();
    while ($entity_id = $results->fetchField()) {
      $filter_ids[] = $entity_id;
    }

    // Take the intersection of IDs in this filter with those in the
    // final $entity_ids;
    if (count($entity_ids) == 0) {
      $entity_ids = $filter_ids;
    }
    else {
      $entity_ids = array_intersect($entity_ids, $filter_ids);
    }
  }
  return $entity_ids;
}