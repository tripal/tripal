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

      // Get the instnace for this field
      $instance = field_info_instance($entity_type, $field_name, $entity->bundle); 

      // Allow the creating module to alter the value if desired.  The
      // module should do this if the field has any other form elements
      // that need populationg besides the value which was set above.
      tripal_load_include_field_class($field_type);
      if (class_exists($field_type)) {
        $tfield = new $field_type($field, $instance);
        $tfield->load($entity);
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}

/**
 * Implements hook_field_storage_query().
 */
function tripal_field_storage_query($query) {

  $filter = array();
  $entity_ids = array();

  // Create the initial query.
  $select = db_select('tripal_entity', 'TE');
  $select->join('tripal_bundle', 'TB', 'TE.bundle = TB.name');
  $select->fields('TE', array('id'));
  $select->fields('TB', array('name'));

  // Add in any filters to the query.
  foreach ($query->fieldConditions as $index => $condition) {
    $field = $condition['field'];
    // Skip conditions that don't belong to this storage type.
    if ($field['storage']['type'] != 'tripal_no_storage') {
      continue;
    }
    $value = $condition['value'];
    $operator = $condition['operator'] ? $condition['operator'] : '=';

    // Filtering on the content type is filtering on the label.
    if ($field['field_name'] == 'content_type') {
      $select->condition('TB.label', $value, $operator);
    }
  }

  // Add in any sorting to the query.
  foreach ($query->order as $index => $sort) {
    $field = $sort['specifier']['field'];
    // Skip sorts that don't belong to this storage type.
    if ($field['storage']['type'] != 'tripal_no_storage') {
      continue;
    }
    $direction = $sort['direction'];

    // Filtering on the content type is a filter using the label
    if ($field['field_name'] == 'content_type') {
      $select->orderBy('TB.label', $direction);
    }
  }

  // Perform the query and return the results.
  $entities = $select->execute();
  $result = array(
    'TripalEntity' => array(),
  );
  while ($entity = $entities->fetchObject()) {
    $ids = array($entity->id, '0', $entity->name);
    $result['TripalEntity'][$entity->id] = entity_create_stub_entity('TripalEntity', $ids);
  }
  return $result;
}
