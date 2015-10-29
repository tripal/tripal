<?php
/**
 * Implements hook_field_storage_info().
 */
function tripal_fields_field_storage_info() {
  return array(
    'field_chado_storage' => array(
      'label' => t('Chado storage'),
      'description' => t('Stores fields in the local Chado database.'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_storage_query().
 *
 * @param $query
 */
function tripal_fields_field_storage_query($query) {
  // TODO: figure out what this function does.
}
/**
 * Implements hook_field_storage_write().
 */
function tripal_fields_field_storage_write($entity_type, $entity, $op, $fields) {

  // Convert the fields into a key/value list of fields and their values.
  $field_vals = tripal_fields_field_storage_unnest_fields($fields, $entity_type, $entity);

  $data_table = NULL;
  $type_field = NULL;
  $record_id = NULL;
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

        // Recursively write fields for the base table.
        tripal_fields_field_storage_write_recursive($entity_type, $entity,
          $op, $field_vals, $data_table, $type_field);

        // Now get the new chado_entity that was just added.
        $details = db_select('chado_entity', 'ce')
          ->fields('ce')
          ->condition('entity_id', $entity->id)
          ->execute()
          ->fetchObject();
        $data_table = $details->data_table;
        $type_field = $details->field;
        $record_id = $details->record_id;

      }
      break;

    case FIELD_STORAGE_UPDATE :

      // Get the base table and record id for the fields of this entity.
      $details = db_select('chado_entity', 'ce')
        ->fields('ce')
        ->condition('entity_id', $entity->id)
        ->execute()
        ->fetchObject();
      $data_table = $details->data_table;
      $type_field = $details->field;
      $record_id = $details->record_id;

      tripal_fields_field_storage_write_recursive($entity_type, $entity,
        $op, $field_vals, $data_table, $type_field, $record_id);
      if (!$details) {
        // TODO: what to do if record is missing!
      }
      break;
  }

  // Recursively write fields for any referring tables that have
  // values in the $field_vals array.  There should be a key for each
  // FK table in the $field_vals array.
//   $schema = chado_get_schema($data_table);
//   $rtables = $schema['referring_tables'];
//   foreach ($rtables as $rtable) {
//     if (array_key_exists($rtable, $field_vals)) {
//       tripal_fields_field_storage_write_recursive($entity_type, $entity,
//           $op, $field_vals, $rtable, NULL, NULL, 1);
//     }
//   }
}

/**
 * Implements hook_field_storage_write_recursive().
 */
function tripal_fields_field_storage_write_recursive($entity_type, $entity,
   $op, $field_vals, $tablename, $type_field = NULL,
   $record_id = NULL, $depth = 0) {

  // Intialize the values array.
  $values = array();

  // Get the schema for this table so that we can identify the primary key
  // and foreign keys.
  $schema = chado_get_schema($tablename);
  $pkey_field = $schema['primary key'][0];
  $fkey_fields = $schema['foreign keys'];
  $fkey_fields_list = array();

  // STEP 1: Recurse on the FK fields.
  // Loop through the foreign keys so that we can recurse on those first.
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

      // Get the value of the FK field as provided by the user.
      $fk_val = NULL;
      $fk_vals = array();
      $fk_field_name = $tablename . '__' . $local_id;
      if (array_key_exists($fk_field_name, $field_vals)) {
        $fk_val = $field_vals[$fk_field_name][0]['value'];
        $fk_vals = $field_vals[$fk_field_name][0];
        unset($fk_vals['value']);
      }

      // Don't recurse if the value of the FK field is set to NULL.  The
      // Tripal Chado API value for NULL is '__NULL__'.
      if ($fk_val == "__NULL__") {
        $values[$local_id] = $fk_val;
        continue;
      }

      // Don't recuse if there are no values to select/insert a foreign key.
      if (count(array_keys($fk_vals)) == 0) {
        $values[$local_id] = $fk_val;
        continue;
      }

      // Recurse on the FK field.  Pass in the ID for the FK field if one
      // exists in the $field_vals;
      $fk_val = tripal_fields_field_storage_write_recursive($entity_type,
        $entity, $op, $fk_vals, $fk_table, NULL, $fk_val, $depth + 1);
      if (isset($fk_val) and $fk_val != '' and $fk_val != 0) {
        $values[$local_id] = $fk_val;
      }
    }
  }

  // STEP 2: Loop through the non FK fields.
  // Loop through the fields passed to the function and find any that
  // are for this table.  Then add their values to the $values array.
  foreach ($field_vals as $field_name => $items) {

    if (preg_match('/^' . $tablename . '__(.*)/', $field_name, $matches)) {
      $chado_field = $matches[1];

      // Skip the PKey field. We won't ever insert a primary key and if
      // one is provided in the fields then we use it for matching on an
      // update.  We don't add it to the $values array in either case.
      if ($chado_field == $pkey_field) {
        continue;
      }

      // Skip FK fields as those should already have been dealt with the
      // recursive code above.
      if (in_array($chado_field, $fkey_fields_list)) {
        continue;
      }

      // Add the value of the field to the $values arr for later insert/update.
      $values[$chado_field] = $items[0]['value'];
    }
  }

  // STEP 3: Insert/Update the record.
  // If there are no values then return.
  if (count($values) == 0) {
    return $record_id;
  }
  // If we don't have an incoming record ID then this is an insert.
  if ($record_id == NULL) {
    // STEP 3a: Before inserting, we want to make sure the record does not
    // already exist.  Using the unique constraint check for a matching record.
    $options = array('is_duplicate' => TRUE);
    $is_duplicate = chado_select_record($tablename, array('*'), $values, $options);
    if($is_duplicate) {
      $record = chado_select_record($tablename, array('*'), $values);
      return $record[0]->$pkey_field;
    }

    // STEP 3b: Insert the reocrd
    // Insert the values array as a new record in the table.
    $record = chado_insert_record($tablename, $values);
    if ($record === FALSE) {
      throw new Exception('Could not insert Chado record into table: "' . $tablename . '".');
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
          drupal_set_message('Unable to insert new Chado entity.', 'error');
        }
      }
  }
  // We have an incoming record_id so this is an update.
  else {
    $match[$pkey_field] = $record_id;
    if (!chado_update_record($tablename, $match, $values)) {
      drupal_set_message("Could not update Chado record in table: $tablename.", 'error');
    }
  }

  return $record_id;
}

/**
 * Implements hook_field_storage_load().
 *
 * Responsible for loading the fields from the Chado database and adding
 * their values to the entity.
 */
function tripal_fields_field_storage_load($entity_type, $entities, $age, $fields, $options) {
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

/**
 * Iterates through all of the fields reformats to a key/value array.
 *
 * @param $fields
 */
function tripal_fields_field_storage_unnest_fields($fields, $entity_type, $entity) {
  $new_fields = array();

  // Iterate through all of the fields.
  foreach ($fields as $field_id => $ids) {

    // Get the field name and all of it's items.
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];
    $items = field_get_items($entity_type, $entity, $field_name);

    // Iterate through the field's items. Fields with cardinality ($delta) > 1
    // are multi-valued.
    foreach ($items as $delta => $item) {
      foreach ($item as $item_name => $value) {
        if ($item_name == 'value') {
          $new_fields[$field_name][$delta]['value'] = $value;
          continue;
        }
        $matches = array();
        if (preg_match('/^(.*?)__.*?$/', $item_name, $matches)) {
          $table_name = $matches[1];
          $new_fields[$field_name][$delta][$item_name][0]['value'] = $value;
        }
      }
    }
  }
  return $new_fields;
}