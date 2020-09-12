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
  return [
    'tripal_no_storage' => [
      'label' => t('Tripal'),
      'description' => t('The NULL storage is a placeholder for field values
          that are not stored in any storage backend (e.g. entity types).'),
      'settings' => [
        'tripal_storage_api' => TRUE,
      ],
    ],
  ];
}

/**
 * Implements hook_field_storage_load().
 *
 * Responsible for loading the fields and adding their values to the entity.
 */
function tripal_field_storage_load($entity_type, $entities, $age,
                                   $fields, $options) {

  $load_current = $age == FIELD_LOAD_CURRENT;
  global $language;
  $langcode = $language->language;

  foreach ($entities as $id => $entity) {

    // Iterate through the entity's fields so we can get the column names
    // that need to be selected from each of the tables represented.
    $tables = [];
    foreach ($fields as $field_id => $ids) {

      // By the time this hook runs, the relevant field definitions have been
      // populated and cached in FieldInfo, so calling field_info_field_by_id()
      // on each field individually is more efficient than loading all fields in
      // memory upfront with field_info_field_by_ids().
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $field_type = $field['type'];
      $field_module = $field['module'];

      // Get the instance for this field
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

  $filter = [];
  $entity_ids = [];

  // Create the initial query.
  $select = db_select('tripal_entity', 'TE');
  $select->join('tripal_bundle', 'TB', 'TE.bundle = TB.name');
  $select->fields('TE', ['id']);
  $select->fields('TB', ['name']);

  // Apply any entity condition filters.
  if ($query->entityConditions) {
    if (array_key_exists('bundle', $query->entityConditions)) {
      $select->condition('TB.name', $query->entityConditions['bundle']['value']);
    }
  }

  if (property_exists($query, 'relationshipConditions') && $query->relationshipConditions) {
    foreach ($query->relationshipConditions as $table_alias => $reldetails) {
      $field = $reldetails['field'];
      $value = $reldetails['value'];
      $op = $reldetails['op'];
      $relationship = $query->relationships[$table_alias];
      $table = $relationship['table'];
      $select->join($table, $table_alias, 'TE.id = ' . $table_alias . '.' . $relationship['field']);
      $select->condition($table_alias . '.' . $field, $value, $op);
    }
  }

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

  // Add a range of records to retrieve
  if ($query->range) {
    $select->range($query->range['start'], $query->range['length']);
  }

  // Only include records that are deleted.  Tripal doesn't keep track of
  // records that are deleted that need purging separately so we can do nothing
  // with this.
  if (property_exists($query, 'deleted') and $query->deleted) {
    // There won't ever be field data marked as deleted so just created a
    // condition that always evaluates to false.
    $select->where('1=0');
  }

  //   dpm($query);
  //   dpm($select->__toString());
  //   dpm($select->getArguments());

  // Perform the query and return the results.
  $entities = $select->execute();

  $result = [
    'TripalEntity' => [],
  ];
  while ($entity = $entities->fetchObject()) {
    $ids = [$entity->id, '0', $entity->name];
    $result['TripalEntity'][$entity->id] = entity_create_stub_entity('TripalEntity', $ids);
  }
  return $result;
}
