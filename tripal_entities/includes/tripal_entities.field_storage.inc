<?php
/**
 * Implements hook_field_storage_info().
 */
function tripal_entities_field_storage_info() {
  return array(
    'field_chado_storage' => array(
      'label' => t('Chado storage'),
      'description' => t('Stores fields in the local Chado database.'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_storage_write().
 */
function tripal_entities_field_storage_write($entity_type, $entity, $op, $fields) {
  drupal_debug($entity_type);
  // Get the IDs for this entity.
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Find out which table should receive the insert.
  $tablename = 'feature';
  $type_field = 'type_id';
  $schema = chado_get_schema($tablename);
  $pkey_field = $schema['primary key'][0];

  // Construct the values array that will be used to insert into the table.
  $values = array();
  foreach ($fields as $field_id) {
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];

    $matches = array();
    if (preg_match('/^' . $tablename . '__(.*)/', $field_name, $matches)) {
      $chado_field = $matches[1];
      // Currently, we only support one language, but for for the sake of
      // thoroughness we'll iterate through all possible languages.
      $all_languages = field_available_languages($entity_type, $field);
      $field_languages = array_intersect($all_languages, array_keys((array) $entity->$field_name));
      foreach ($field_languages as $langcode) {
        $items = (array) $entity->{$field_name}[$langcode];
        // The number of items is related to the cardinatily of the field.
        foreach ($items as $delta => $item) {
          $values[$chado_field] = $item['value'];
        }
      }
    }
  }
  // Add in the type_id field.
  $values[$type_field] = $entity->cvterm_id;

  switch ($op) {
    case FIELD_STORAGE_INSERT:
      $record = chado_insert_record($tablename, $values);
      if ($record === FALSE) {
        drupal_set_message('Could not insert Chado record.', 'error');
      }
      $entity->record_id = $record[$pkey_field];
      break;
    case FIELD_STORAGE_UPDATE:
      $match[$pkey_field] = $entity->record_id;
      chado_update_record($tablename, $match, $values);
      break;
  }
}
/**
 * Implements hook_field_storage_load().
 *
 * Responsible for loading the fields from the Chado database and adding
 * their values to the entity.
 */
function tripal_entities_field_storage_load($entity_type, $entities, $age, $fields, $options) {
  $load_current = $age == FIELD_LOAD_CURRENT;
  global $language;
  $langcode = $language->language;

  foreach ($entities as $id => $entity) {
    // Find out which table should receive the insert.
    $tablename = 'feature';
    $type_field = 'type_id';
    $schema = chado_get_schema($tablename);
    $pkey_field = $schema['primary key'][0];
    $record_id = $entity->record_id;

    // Iterate through the field names to get the list of tables and fields
    // that should be queried.
    $columns = array();
    foreach ($fields as $field_id => $ids) {
      // By the time this hook runs, the relevant field definitions have been
      // populated and cached in FieldInfo, so calling field_info_field_by_id()
      // on each field individually is more efficient than loading all fields in
      // memory upfront with field_info_field_by_ids().
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];

      $matches = array();
      if (preg_match('/^(.*?)__(.*?)$/', $field_name, $matches)) {
        $table = $matches[1];
        $field = $matches[2];
        $columns[$table][] = $field;
      }
    }

    // Get the record
    $values = array($pkey_field => $entity->record_id);
    $record = chado_select_record($tablename, $columns[$tablename], $values);

    // Now set the field values
    foreach ($fields as $field_id => $ids) {
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $matches = array();
      if (preg_match('/^(.*?)__(.*?)$/', $field_name, $matches)) {
        $table = $matches[1];
        $field = $matches[2];
        $entity->{$field_name}['und'][] = array('value' => $record[0]->$field);
      }
    }
  }
}