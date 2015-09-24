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

  switch ($op) {
    case FIELD_STORAGE_INSERT:
      // Use the cvterm_id to look up tables where this term is used
      $sel_values = array(
        'term_id' => array(
          'cvterm_id' => $entity->cvterm_id,
        ),
      );
      $term_usage = chado_generate_var('tripal_term_usage', $sel_values, array('return_array' => 1));

      // For each table that uses this term, insert the field recursively
      foreach ($term_usage as $usage) {
        $data_table = $usage->data_table;
        //$type_table = $usage->type_table;
        $type_field = $usage->field;
        tripal_entities_field_storage_write_recursive($entity_type, $entity,
          $op, $fields, $data_table, $type_field);
      }
      break;

    case FIELD_STORAGE_UPDATE :

      // Get the base table and record id for the fields of this entity.
      $details = db_select('chado_entity', 'ce')
        ->fields('ce')
        ->condition('entity_id', $entity->id)
        ->execute()
        ->fetchObject();
      $tablename = $details->data_table;
      $type_field = $details->field;
      $record_id = $details->record_id;

      tripal_entities_field_storage_write_recursive($entity_type, $entity,
        $op, $fields, $tablename, $type_field, $record_id);
      if (!$details) {
        // TODO: what to do if record is missing!
      }
      break;
  }
}

/**
 * Implements hook_field_storage_write_recursive().
 */
function tripal_entities_field_storage_write_recursive($entity_type, $entity,
    $op, $fields, $tablename, $type_field = NULL, $record_id = NULL, $depth = 0) {

  // Intialize the values array and $record_id;
  $values = array();

  // Get the schema for this table so that we can identify the primary key
  // and foreign keys.
  $schema = chado_get_schema($tablename);
  $pkey_field = $schema['primary key'][0];
  $fkey_fields = $schema['foreign keys'];
  $fkey_fields_list = array();

  // STEP 1: Recurse on the FK fields.
  // Loop through the foreign keys os that we can recurse on those first.
  foreach ($fkey_fields as $fk_table => $details) {
    foreach ($details['columns'] as $local_id => $remote_id) {
      // If this is the base table then do not recurse on the type_id.  The
      // type_id is set in the entity.  We are at the base table when
      // the depth is 0.
      if ($depth == 0 && $local_id == $type_field) {
        $values[$local_id] = $entity->cvterm_id;
        continue;
      }
      // Keep track of the FK fields so that in STEP 2 we don't have to
      // loop through the $fk_fields again.
      $fkey_fields_list[] = $local_id;

      // Recurse differently if this is an insert or an update.
      $fk_val = NULL;
      switch ($op) {
        case FIELD_STORAGE_INSERT:
          // On an insert we do not pass in a $record_id because
          // we don't have any.
          $fk_val = tripal_entities_field_storage_write_recursive($entity_type,
            $entity, $op, $fields, $fk_table, NULL, NULL, $depth + 1);
          break;
        case FIELD_STORAGE_UPDATE:
          // On an update we must get the value of the FK field. This should
          // be present as a field, so get it's value and pass it in as the
          // $record_id field.
          $fk_val = tripal_entities_get_field_value($tablename . '__' . $local_id, $entity, $entity_type);
          $fk_val = tripal_entities_field_storage_write_recursive($entity_type,
            $entity, $op, $fields, $fk_table, NULL, $fk_val, $depth + 1);
          break;
      }
      if ($fk_val) {
        $values[$local_id] = $fk_val;
      }
    }
  }

  // STEP 2: Loop through the incoming fields.
  // Loop through the fields passed to the function and find any that
  // are for this table.  Then add their values to the $values array.
  foreach ($fields as $field_id) {
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];

    if (preg_match('/^' . $tablename . '__(.*)/', $field_name, $matches)) {
      $chado_field = $matches[1];

      // Skip FK fields as those should already have been dealt with the
      // recursive code above.
      if (in_array($field_id, $fkey_fields_list)) {
        continue;
      }

      // Add the value of the field to the $values arr for later insert/update.
      $values[$chado_field] = tripal_entities_get_field_value($tablename . '__' . $chado_field, $entity, $entity_type);
    }
  }

  // STEP 3: Insert/Update the record.
  // If there are no values then return.
  $entity->storage = array();
  switch ($op) {
    case FIELD_STORAGE_INSERT:
      if (count($values) == 0) {
        return NULL;
      }
      // Inser the values array as a new record in the table.
      $record = chado_insert_record($tablename, $values);
      if ($record === FALSE) {
        drupal_set_message('Could not insert Chado record.', 'error');
      }
      $record_id = $record[$pkey_field];

      // Add a record to the chado_entity table so that the data for the
      // fields can be pulled from Chado when loaded the next time. Only do
      // this for the base table record.
      if ($depth == 0) {
        $record = array(
          'entity_id' => $entity->id,
          'record_id' => $record_id,
          'data_table' => $tablename,
          'type_table' => $tablename, // TODO: this must be fixed.
          'field' => $type_field,
        );
        $success = drupal_write_record('chado_entity', $record);
        if (!$success) {
          drupal_set_message('Unable to insert new data.', 'error');
        }
      }
      break;
    case FIELD_STORAGE_UPDATE:
      // If we don't have any values we are not going to update the
      // record because there is nothing to udpate, do just return the
      // record_id.
      if (count($values) == 0) {
        return $record_id;
      }
      $match[$pkey_field] = $record_id;

      chado_update_record($tablename, $match, $values);
      break;
  }
  return $record_id;
}

/**
 *
 */
function tripal_entities_get_field_value($field_name, $entity, $entity_type) {
  $value = NULL;
  $field = field_info_field($field_name);
  if (!$field) {
    return $value;
  }

  // Currently, we only support one language, but for for the sake of
  // thoroughness we'll iterate through all possible languages.
  $all_languages = field_available_languages($entity_type, $field);
  $field_languages = array_intersect($all_languages, array_keys((array) $entity->$field_name));
  foreach ($field_languages as $langcode) {
    $items = (array) $entity->{$field_name}[$langcode];
    // The number of items is related to the cardinatily of the field.
    // We should always only have one item because this is a field in a
    // table. But we loop anyway.
    foreach ($items as $delta => $item) {
      $value = $item['value'];
    }
  }
  return $value;
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

   // Get the base table and record id for the fields of this entity.
   $details = db_select('chado_entity', 'ce')
      ->fields('ce')
      ->condition('entity_id', $entity->id)
      ->execute()
      ->fetchObject();
    if (!$details) {
      // TODO: what to do if record is missing!
    }

    // Find out which table should receive the insert.
    $tablename = $details->data_table;
    $type_field = $details->field;
    $schema = chado_get_schema($tablename);
    $pkey_field = $schema['primary key'][0];
    $record_id = $details->record_id;

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
    $values = array($pkey_field => $record_id);
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