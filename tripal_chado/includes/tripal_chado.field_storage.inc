<?php

/**
 * Implements hook_field_storage_info().
 */
function tripal_chado_field_storage_info() {
  return array(
    'field_chado_storage' => array(
      'label' => t('Chado'),
      'description' => t('Stores fields in the local Chado database.'),
      'settings' => array(),
      // The logo_url key is supported by Tripal. It's not a Drupal key. It's
      // used for adding a logo or picture for the data store to help make it
      // more easily recognized on the  field_ui_field_overview_form. Ideally
      // the URL should point to a relative path on the local Drupal site.
      'logo_url' => url(drupal_get_path('module', 'tripal') . '/theme/images/250px-ChadoLogo.png'),
    ),
  );
}

/**
 * Implements hook_field_storage_write().
 */
function tripal_chado_field_storage_write($entity_type, $entity, $op, $fields) {

  // Get the bundle and the term for this entity.
  $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
  $term = entity_load('TripalTerm', array('id' => $entity->term_id));
  $term = reset($term);

  // Get the base table, type field and record_id from the entity.
  $base_table = $entity->chado_table;
  $type_field = $entity->chado_column;
  $record     = $entity->chado_record;
  $record_id  = $entity->chado_record_id;

  // Convert the fields into a key/value list of fields and their values.
  $field_vals = tripal_chado_field_storage_merge_fields($fields, $entity_type, $entity);
dpm($field_vals);
return;
  // Write the record for the base table.
  $record_id = tripal_chado_field_storage_write_table(array(
    'entity' => $entity,
    'term' => $term,
    'op' => $op,
    'field_vals' => $field_vals,
    'base_table' => $base_table,
    'tablename' => $base_table,
    'type_field' => $type_field,
    'record_id' => $record_id,
  ));

  // If this is an insert then add the chado_entity record.
  if ($op == FIELD_STORAGE_INSERT) {
    // Add a record to the chado_entity table so that the data for the
    // fields can be pulled from Chado when loaded the next time.
    $record = array(
      'entity_id' => $entity->id,
      'record_id' => $record_id,
      'data_table' => $base_table,
      'type_table' => $base_table,
      'field' => $type_field,
    );
    $success = drupal_write_record('chado_entity', $record);
    if (!$success) {
      drupal_set_message('Unable to insert new Chado entity.', 'error');
    }
  }

  // Now that we have handled the base table, we need to handle linking tables.
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
        if (array_key_exists($field_name, $field_vals)) {
          foreach ($field_vals[$field_name] as $delta => $fvals) {
            tripal_chado_field_storage_write_table(array(
              'entity' => $entity,
              'term' => $term,
              'op' => $op,
              'field_vals' => $fvals,
              'base_table' => $base_table,
              'tablename' => $field_table
            ));
          }
        }
      }
    }
  }
}

/**
 *
 */
function tripal_chado_field_storage_write_table($params) {
  $entity = $params['entity'];
  $term = $params['term'];
  $op = $params['op'];
  $field_vals = $params['field_vals'];
  $base_table = $params['base_table'];
  $tablename = $params['tablename'];
  $type_field = array_key_exists('type_field', $params) ? $params['type_field'] : NULL;
  $record_id = array_key_exists('record_id', $params) ? $params['record_id'] : NULL;
  $depth = array_key_exists('depth', $params) ? $params['depth'] : 0;

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
        $values[$local_id] = $term->details['cvterm']->cvterm_id;
        continue;
      }

      // If this is a linking table, do not recurse on the fields that
      // link back to the base table.
      if ($tablename != $base_table && $details['table'] == $base_table) {
        $fkey_base_linker = $local_id;
        continue;
      }

      // Get the value of the FK field as provided by the user.
      $fk_val = NULL;
      $fk_vals = array();
      $fk_field_name = $tablename . '__' . $local_id;
      if (array_key_exists($fk_field_name, $field_vals)) {
        $fk_val = $field_vals[$fk_field_name][0][$tablename . '__' . $local_id];
        $fk_vals = $field_vals[$fk_field_name][0];
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

      // Recurse on the FK field.
      $nparams = $params;
      $nparams['field_vals'] = $fk_vals;
      $nparams['tablename'] = $fk_table;
      $nparams['type_field'] = NULL;
      $nparams['record_id'] = $fk_val;
      $nparams['depth'] = $depth + 1;
      $fk_val = tripal_chado_field_storage_write_table($nparams);
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
      if (!$items[0][$tablename . '__' . $chado_field]) {
        continue;
      }

      // Add the value of the field to the $values arr for later insert/update.
      $values[$chado_field] = $items[0][$tablename . '__' . $chado_field];
    }
  }

  // STEP 3: Insert/Update the record.
  // If there are no values then return.
  if (count($values) == 0) {
    return $record_id;
  }

  dpm($values);
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
    // TODO: what if the unique constraint matches another record?  That is
    // not being tested for here.
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
    $record = chado_generate_var($base_table, $match);
    $entity->chado_record = $record;

    // For now, expand all 'text' fields.
    // TODO: we want to be a bit smarter and allow the user to configure this
    // for now we'll expand.
    foreach ($schema['fields'] as $field_name => $details) {
      if ($schema['fields'][$field_name]['type'] == 'text') {
        $record = chado_expand_var($record, 'field', $base_table . '.' . $field_name);
      }
    }

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
          // If the field column is an object then it's a FK to another table.
          // and because $record object is created by the chado_generate_var()
          // function we must go one more level deeper to get the value
          if (is_object($record->$field_column)) {
            $entity->{$field_name}['und'][0][$field_table . '__' . $field_column] = $record->$field_column->$field_column;
          }
          else {
            // For non FK fields we'll make the field value be the same
            // as the column value.
            $entity->{$field_name}['und'][0]['value'] = $record->$field_column;
            $entity->{$field_name}['und'][0][$field_table . '__' . $field_column] = $record->$field_column;
          }
        }

        // Allow the creating module to alter the value if desired.  The
        // module should do this if the field has any other form elements
        // that need populationg besides the value which was set above.
        module_load_include('inc', $field_module, 'includes/fields/' . $field_type);
        if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
          $field_obj = new $field_type();
          $field_obj->load($field, $entity, array('record' => $record));
        }

        $load_function = $field_type . '_load';
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
        $load_function = $field_type . '_load';
        module_load_include('inc', $field_module, 'includes/fields/' . $field_type);

        if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
          $field_obj = new $field_type();
          $field_obj->load($field, $entity, array('record' => $record));
        }

        if (function_exists($load_function)) {
          $load_function($field, $entity, $base_table, $record);
        }
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}
/**
 * Merges the values of all fields into a single array keyed by table name.
 */
function tripal_chado_field_storage_merge_fields($fields, $entity_type, $entity) {
  $new_fields = array();

  // Iterate through all of the fields and organize them into a
  // new fields array keyed by the table name
  foreach ($fields as $field_id => $ids) {

    // Get the field name and information about it.
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];
    // Some fields (e.g. chado_linker_cvterm_adder) don't add data to
    // Chado so they don't have a table, but they are still attached to the
    // entity. Just skip these.
    if (!array_key_exists('chado_table', $field['settings'])) {
      continue;
    }
    $chado_table = $field['settings']['chado_table'];
    $chado_column = $field['settings']['chado_column'];

    // Iterate through the field's items. Fields with cardinality ($delta) > 1
    // are multi-valued.
    $items = field_get_items($entity_type, $entity, $field_name);
    foreach ($items as $delta => $item) {
      foreach ($item as $item_name => $value) {
        $matches = array();
        if (preg_match('/^(.*?)__(.*?)$/', $item_name, $matches)) {
          $table_name = $matches[1];
          $column_name = $matches[2];

          // Is this a nested FK field? If so break it down into a sub array.
          if (preg_match('/^(.*?)--(.*?)$/', $column_name, $matches)) {
            $parent_item_name = $matches[1];
            $sub_item_name = $matches[2];
            $sub_item = tripal_chado_field_storage_expand_field($sub_item_name, $value);
            // If we've already encountered this table and column then we've
            // already seen the numeric FK value or we've already added a
            // subcolumn. If the former we want to convert this to an array
            // so we can add the details.
            if (array_key_exists($parent_item_name, $new_fields[$table_name][$delta]) and
                !is_array($new_fields[$table_name][$delta][$parent_item_name])) {
              $new_fields[$table_name][$delta][$parent_item_name] = array();
            }
            $new_fields[$table_name][$delta][$parent_item_name] += $sub_item;
          }
          else {
            // If not seen this table and column then just add it. If we've
            // already seen it then it means it's a FK field and we've already
            // added subfields so do nothing.
            if (!array_key_exists($table_name, $new_fields) or
                !array_key_exists($delta, $new_fields[$table_name]) or
                !array_key_exists($column_name, $new_fields[$table_name][$delta])) {
              $new_fields[$table_name][$delta][$column_name] = $value;
            }
          }
        }
      }
      // If there is no value set for the field using the
      // [table_name]__[field name] naming schema then check if a 'value' item
      // is present and if so use that.
      if ((!array_key_exists($chado_table, $new_fields) or
           !array_key_exists($delta, $new_fields[$chado_table]) or
           !array_key_exists($chado_column, $new_fields[$chado_table][$delta])) and
          array_key_exists('value', $items[$delta]) and
          !is_array($items[$delta]['value'])) {
        $new_fields[$chado_table][$delta][$chado_column] = $items[$delta]['value'];
      }
    }
  }

  return $new_fields;
}

/**
 * Recurses through a field's item name breaking it into a nested array.
 *
 *
 */
function tripal_chado_field_storage_expand_field($item_name, $value) {

   $matches = array();
   if (preg_match('/^(.*?)--(.*?)$/', $item_name, $matches)) {
     $parent_item_name = $matches[1];
     $sub_item_name = $matches[2];
     $sub_item = tripal_chado_field_storage_expand_field($sub_item_name, $value);
     return array($parent_item_name => $sub_item);
   }
   else {
     return array($item_name => $value);
   }
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
function tripal_chado_field_storage_query($query) {
  $fieldConditions = $query->fieldConditions;
  foreach ($fieldConditions as $condition) {
    $field = $condition['field'];
    $field_name = $field['field_name'];
    $column = $condition['column'];
  }
}
