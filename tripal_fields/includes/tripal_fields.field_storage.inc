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
 * Implements hook_field_storage_query().
 *
 * @param $query
 */
function tripal_entities_field_storage_query($query) {
  // TODO: figure out what this function does.
}
/**
 * Implements hook_field_storage_write().
 */
function tripal_entities_field_storage_write($entity_type, $entity, $op, $fields) {

  // Convert the fields into a key/value list of fields and their values.
  $field_vals = tripal_entities_field_storage_unnest_fields($fields, $entity_type, $entity);

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
          $op, $field_vals, $data_table, $type_field);
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
        $op, $field_vals, $tablename, $type_field, $record_id);
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
   $op, $field_vals, $tablename, $type_field = NULL, $record_id = NULL, $depth = 0) {

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

      // Get the value of the FK field as provided by the user.
      $fk_val = NULL;
      $fk_field_name = $tablename . '__' . $local_id;
      $fk_val = array_key_exists($fk_field_name, $field_vals) ? $field_vals[$fk_field_name] : NULL;

      // Don't recurse if the value of the FK field is set to NULL.  The
      // Tripal Chado API value for NULL is '__NULL__'.
      if ($fk_val == "__NULL__") {
        $values[$local_id] = $fk_val;
        continue;
      }
      // Recurse on the FK field.  Pass in the ID for the FK field if one
      // exists in the $field_vals;
      $fk_val = tripal_entities_field_storage_write_recursive($entity_type,
        $entity, $op, $field_vals, $fk_table, NULL, $fk_val, $depth + 1);
      if (isset($fk_val) and $fk_val != '' and $fk_val != 0) {
        $values[$local_id] = $fk_val;
      }
    }
  }

  // STEP 2: Loop through the incoming fields.
  // Loop through the fields passed to the function and find any that
  // are for this table.  Then add their values to the $values array.
  foreach ($field_vals as $field_name => $field_val) {
    // If the field value is empty then continue.
    if (!isset($field_val) or $field_val === '' or $field_val === NULL) {
      continue;
    }
    if (preg_match('/^' . $tablename . '__(.*)/', $field_name, $matches)) {
      $chado_field = $matches[1];

      // Skip the Pkey field. We won't ever insert a primary key and if
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
      $values[$chado_field] = $field_vals[$field_name];
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

/**
 * Iterates through all of the fields reformats to a key/value array.
 *
 * @param $fields
 */
function tripal_entities_field_storage_unnest_fields($fields, $entity_type, $entity) {
  $new_fields = array();
  foreach ($fields as $field_id => $ids) {
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];
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
        // If the $value is an array then this field has nested fields. We
        // want to add those to our $new_fields list.
        if (is_array($item['value'])) {
          foreach ($item['value'] as $children) {
            foreach ($children as $child_field_name => $child_value) {
              if (preg_match('/^.*?__.*?$/', $child_field_name)) {
                $new_fields[$child_field_name] = $child_value;
              }
            }
          }
        }
        else {
          $new_fields[$field_name] = $item['value'];
        }
      }
    }
  }
  return $new_fields;
}