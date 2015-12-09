<?php

/**
 * Implements hook_field_storage_info().
 */
function tripal_chado_field_storage_info() {
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
function tripal_chado_field_storage_query($query) {
  // TODO: figure out what this function does.
}
/**
 * Implements hook_field_storage_write().
 */
function tripal_chado_field_storage_write($entity_type, $entity, $op, $fields) {

  // Get the base table, type field and record_id from the entity.
  $base_table = $entity->chado_table;
  $type_field = $entity->chado_field;
  $record_id  = property_exists($entity, 'chado_record_id') ? $entity->chado_record_id : NULL;

  // Convert the fields into a key/value list of fields and their values.
  $field_vals = tripal_chado_field_storage_unnest_fields($fields, $entity_type, $entity);

  // Recursively write fields for the base table.
  $record_id = tripal_chado_field_storage_write_recursive($entity_type, $entity,
      $op, $field_vals, $base_table, $base_table, $type_field, $record_id);

  // If this is an insert then add the chado_entity record.
  if ($op == FIELD_STORAGE_INSERT) {
    // Add a record to the chado_entity table so that the data for the
    // fields can be pulled from Chado when loaded the next time.
    $record = array(
      'entity_id' => $entity->id,
      'record_id' => $record_id,
      'data_table' => $entity->chado_table,
      'type_table' => $entity->chado_table,
      'field' => $entity->chado_field,
    );
    $success = drupal_write_record('chado_entity', $record);
    if (!$success) {
      drupal_set_message('Unable to insert new Chado entity.', 'error');
    }
  }

  // Now that we have handled the base table, we need to handle fields that
  // are not part of the base table.
  foreach ($fields as $field_id) {
    // Get the field using the id.
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];

    // If the field has a chado_table setting then we can try to write.
    if (array_key_exists('settings', $field) and array_key_exists('chado_table', $field['settings'])) {

      // Skip fields that use the base table, as we've already handled those.
      if ($field['settings']['chado_table'] != $base_table){
        $field_table = $field['settings']['chado_table'];

        // Iterate through each record.
        foreach ($field_vals[$field_name] as $delta => $fvals) {
          tripal_chado_field_storage_write_recursive($entity_type, $entity,
              $op, $fvals, $base_table, $field_table);
        }
      }
    }
  }
}

/**
 *
 * @param $entity_type
 * @param $entity
 * @param $op
 * @param $field_vals
 * @param $tablename
 * @param $type_field
 * @param $record_id
 * @param $depth
 * @throws Exception
 * @return
 *   The record_id of the table if a matching record exists.
 */
function tripal_chado_field_storage_write_recursive($entity_type, $entity,
   $op, $field_vals, $base_table, $tablename, $type_field = NULL,
   $record_id = NULL, $depth = 0) {

  // Intialize the values array.
  $values = array();

  // Get the schema for this table so that we can identify the primary key
  // and foreign keys.
  $schema = chado_get_schema($tablename);
  $pkey_field = $schema['primary key'][0];
  $fkey_fields = $schema['foreign keys'];
  $fkey_fields_list = array();
  $fkey_base_linker = NULL;

  // STEP 1: Recurse on the FK fields.
  // Loop through the foreign keys so that we can recurse on those first.
  foreach ($fkey_fields as $fk_table => $details) {
    foreach ($details['columns'] as $local_id => $remote_id) {

      // If this is the base table then do not recurse on the type_id.
      if ($tablename == $base_table && $local_id == $type_field) {
        $values[$local_id] = $entity->cvterm_id;
        continue;
      }

      // If this is not the base table then do not recurse on the FK field
      // that links this table to the base table.
      if ($tablename != $base_table && $details['table'] == $base_table) {
        $fkey_base_linker = $local_id;
        continue;
      }

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

      // Don't recuse if there are no fkvals.
      if (count(array_keys($fk_vals)) == 0) {
        continue;
      }

      // Keep track of the FK fields so that in STEP 2 we don't have to
      // loop through the $fk_fields again.
      $fkey_fields_list[] = $local_id;

      // Recurse on the FK field.  Pass in the ID for the FK field if one
      // exists in the $field_vals;
      $fk_val = tripal_chado_field_storage_write_recursive($entity_type,
        $entity, $op, $fk_vals, $base_table, $fk_table, NULL, $fk_val, $depth + 1);
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

      // If the value is empty then exclude this field
      if (!$items[0]['value']) {
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
function tripal_chado_field_storage_load($entity_type, $entities, $age,
    $fields, $options) {

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

    // Get some values needed for loading the values from Chado.
    $base_table = $details->data_table;
    $type_field = $details->field;
    $record_id = $details->record_id;

    // Get this table's schema.
    $schema = chado_get_schema($base_table);
    $pkey_field = $schema['primary key'][0];

    // Get the base record if one exists
    $columns = array('*');
    $match = array($pkey_field => $record_id);
    $record = chado_select_record($base_table, $columns, $match);
    $record = $record[0];

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

      // Skip fields that don't map to a Chado table (e.g. kvproperty_adder).
      if (!array_key_exists('settings', $field) or !array_key_exists('chado_table', $field['settings'])) {
        continue;
      }

      // Get the Chado table and column for this field.
      $field_table = $field['settings']['chado_table'];
      $field_column = $field['settings']['chado_column'];

      // There are only two types of fields: 1) fields that represent a single
      // column of the base table, or 2) fields that represent a linked record
      // in a many-to-one relationship with the base table.

      // Type 1: fields from base tables.
      if ($field_table == $base_table) {
        // Set an empty value by default, and if there is a record, then update.
        $entity->{$field_name}['und'][0]['value'] = '';
        if ($record and property_exists($record, $field_column)) {
          $entity->{$field_name}['und'][0]['value'] = $record->$field_column;
        }
        // Allow the creating module to alter the value if desired.  The
        // module should do this if the field has any other form elements
        // that need populationg besides the default value.
        $load_function = $field_module . '_' . $field_type . '_field_load';
        module_load_include('inc', $field_module, 'includes/fields/' . $field_type);
        if (function_exists($load_function)) {
          $load_function($field, $entity, $base_table, $record);
        }
      }

      // Type 2: fields for linked records.  These fields will have any number
      // of form elements that might need populating so we'll offload the
      // loading of these fields to the field itself.
      if ($field_table != $base_table) {
        // Set an empty value by default, and let the hook function update it.
        $entity->{$field_name}['und'][0]['value'] = '';
        $load_function = $field_module . '_' . $field_type . '_field_load';
        module_load_include('inc', $field_module, 'includes/fields/' . $field_type);
        if (function_exists($load_function)) {
          $load_function($field, $entity, $base_table, $record);
        }
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}
/**
 * Iterates through all of the fields reformats to a key/value array.
 *
 * @param $fields
 */
function tripal_chado_field_storage_unnest_fields($fields, $entity_type, $entity) {
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