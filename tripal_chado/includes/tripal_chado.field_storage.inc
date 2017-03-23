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

  // Convert the Tripal term entity into the appropriate record in Chado.
  $dbxref = tripal_get_dbxref(array('accession' => $term->accession, 'db_id' => array('name' => $term->vocab->vocabulary)));
  $cvterm = tripal_get_cvterm(array('dbxref_id' => $dbxref->dbxref_id));

  // Get the base table, type field and record_id from the entity.
  $base_table = $entity->chado_table;
  $type_field = $entity->chado_column;
  $record     = $entity->chado_record;
  $record_id  = $entity->chado_record_id;
  $base_schema = chado_get_schema($base_table);
  $base_pkey = $base_schema['primary key'][0];

  // Convert the fields into a key/value list of fields and their values.
  $field_vals = tripal_chado_field_storage_write_merge_fields($fields, $entity_type, $entity);

  // First, write the record for the base table.  If we have a record id then
  // this is an update and we need to set the primary key.  If not, then this
  // is an insert and we need to set the type_id if the table supports it.
  $values = $field_vals[$base_table];
  if ($record_id) {
    $values[$base_pkey] = $record_id;
  }
  elseif ($type_field) {
    $values[$type_field] = $cvterm->cvterm_id;
  }
  $base_record_id = tripal_chado_field_storage_write_table($base_table, $values);

  // If this is an insert then add the chado_entity record.
  if ($op == FIELD_STORAGE_INSERT) {
    // Add the record to the proper chado entity table
    $chado_entity_table = tripal_chado_get_bundle_entity_table($bundle);
    $record = array(
      'entity_id' => $entity->id,
      'record_id' => $base_record_id,
    );
    $success = drupal_write_record($chado_entity_table, $record);
    if (!$success) {
      drupal_set_message('Unable to insert new Chado entity.', 'error');
    }
  }

  // Now that we have handled the base table, we need to handle linking tables.
  foreach ($field_vals as $table_name => $details) {
    // Skip the base table as we've already dealt with it.
    if ($table_name == $base_table) {
      continue;
    }
    foreach ($details as $delta => $values) {
      $record_id = tripal_chado_field_storage_write_table($table_name, $values);
    }
  }
}

/**
 * Write (inserts/updates/deletes) values for a Chado table.
 *
 * The $values array is of the same format used by chado_insert_record() and
 * chado_update_record().  However, both of those methods will use any nested
 * arrays (i.e. representing foreign keys) to select an appropriate record ID
 * that can be substituted as the value.  Here, the nested arrays are
 * either inserted or updated as well, but the choice is determined if the
 * primary key value is present.  If present an update occurs, if not present
 * then an insert occurs.
 *
 * This function is recursive and nested arrays from the lowest point of the
 * "tree" are dealt with first.
 *
 * @param $table_name
 *   The name of the table on which the insertion/update is performed.
 * @param $values
 *   The values array for the insertion.
 *
 * @throws Exception
 *
 * @return
 *   The unique record ID.
 */
function tripal_chado_field_storage_write_table($table_name, $values) {
  $schema = chado_get_schema($table_name);
  $fkeys = $schema['foreign keys'];
  $pkey = $schema['primary key'][0];


  // Fields with a cardinality greater than 1 will often submit an
  // empty form.  We want to remove these empty submissions.  We can detect
  // them if all of the fields are empty.
  $num_empty = 0;
  foreach ($values as $column => $value) {
    if (!$value) {
      $num_empty++;
    }
  }
  if ($num_empty == count(array_keys($values))) {
    return '';
  }

  // If the primary key column has a value but all other values are empty then
  // this is a delete.
  if (array_key_exists($pkey, $values) and $values[$pkey]) {
    $num_vals = 0;
    foreach ($values as $value) {
      if ($value) {
        $num_vals++;
      }
    }
    if ($num_vals == 1) {
      $new_vals[$pkey] = $values[$pkey];
      if (!chado_delete_record($table_name, $new_vals)) {
        throw new Exception('Could not delete record from table: "' . $table_name . '".');
      }
      return '';
    }
  }

  // If the primary key column does not have a value then this is an insert.
  if (!array_key_exists($pkey, $values) or !$values[$pkey] or !isset($values[$pkey])) {
    // Before inserting, we want to make sure the record does not
    // already exist.  Using the unique constraint check for a matching record.
    $options = array('is_duplicate' => TRUE);
    $is_duplicate = chado_select_record($table_name, array('*'), $values, $options);
    if($is_duplicate) {
      $record = chado_select_record($table_name, array('*'), $values);
      return $record[0]->$pkey;
    }

    // Insert the values array as a new record in the table but remove the
    // pkey as it should be set.
    $new_vals = $values;
    unset($new_vals[$pkey]);
    $record = chado_insert_record($table_name, $new_vals);
    if ($record === FALSE) {
      throw new Exception('Could not insert Chado record into table: "' . $table_name . '".');
    }
    return $record[$pkey];
  }

  // If we've made it to this point then this is an update.
  // TODO: what if the unique constraint matches another record?  That is
  // not being tested for here.
  $match[$pkey] = $values[$pkey];
  if (!chado_update_record($table_name, $match, $values)) {
    drupal_set_message("Could not update Chado record in table: $table_name.", 'error');
  }
  return $values[$pkey];
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

    if (property_exists($entity, 'chado_table')) {
      // Get the base table and record id for the fields of this entity.
      $base_table = $entity->chado_table;
      $type_field = $entity->chado_column;
      $record_id  = $entity->chado_record_id;
    }
    else {
      $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
      $base_table = $bundle->data_table;
      $type_field = $bundle->type_column;

      // Get the record id for the fields of this entity.
      $chado_entity_table = tripal_chado_get_bundle_entity_table($bundle);
      $details = db_select($chado_entity_table, 'ce')
        ->fields('ce')
        ->condition('entity_id', $entity->id)
        ->execute()
        ->fetchObject();
      if (!$details) {
        // TODO: what to do if record is missing!
      }
      $record_id  = isset($details->record_id) ? $details->record_id : '';
    }

    // Get this table's schema.
    $schema = chado_get_schema($base_table);
    $pkey_field = $schema['primary key'][0];

    // Get the base record if one exists
    $columns = array('*');
    $match = array($pkey_field => $record_id);
    $record = chado_generate_var($base_table, $match);
    $entity->chado_record = $record;

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

      // Get the instance for this field.  If no instance exists then skip
      // loading of this field. This can happen when a field is deleted from
      // a bundle using the user UI form.
      // TODO: how to deal with deleted fields?
      $instance = field_info_instance($entity_type, $field_name, $entity->bundle);

      if (!$instance) {
        continue;
      }

      // Skip fields that don't map to a Chado table.
      if (!array_key_exists('settings', $instance) or
          !array_key_exists('chado_table', $instance['settings'])) {
        continue;
      }

      // Get the Chado table and column for this field.
      $field_table = $instance['settings']['chado_table'];
      $field_column = $instance['settings']['chado_column'];

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
            $fkey_column = $field_column;
            foreach($schema['foreign keys'] as $table => $fk_details) {
              foreach($fk_details['columns'] as $lfkey => $rfkey) {
                if ($lfkey == $field_column) {
                  $fkey_column = $rfkey;
                }
              }
            }
            $entity->{$field_name}['und'][0]['chado-' . $field_table . '__' . $field_column] = $record->$field_column->$fkey_column;
          }
          else {
            // For non FK fields we'll make the field value be the same
            // as the column value.
            $entity->{$field_name}['und'][0]['value'] = $record->$field_column;
            $entity->{$field_name}['und'][0]['chado-' . $field_table . '__' . $field_column] = $record->$field_column;
          }
        }

        // Allow the creating module to alter the value if desired.  The
        // module should do this if the field has any other form elements
        // that need populationg besides the value which was set above.
        tripal_load_include_field_class($field_type);
        if (class_exists($field_type) and is_subclass_of($field_type, 'TripalField')) {
          $tfield = new $field_type($field, $instance);
          $tfield->load($entity, array('record' => $record));
        }
        // For text fields that were not handled by a TripalField class we
        // want to automatically expand those fields.
        else {
          if ($schema['fields'][$field_column]['type'] == 'text') {
            $record = chado_expand_var($record, 'field', "$field_table.$field_column");
            $entity->{$field_name}['und'][0]['value'] = $record->$field_column;
            // Text fields that have a text_processing == 1 setting need a
            // special 'format' element too:
            if (array(key_exists('text_processing', $instance['settings']) and
                $instance['settings']['text_processing'] == 1)) {
              // TODO: we need a way to write the format back to the
              // instance settings if the user changes it when using the form.
              $entity->{$field_name}['und'][0]['format'] = array_key_exists('format', $instance['settings']) ? $instance['settings']['format'] : 'full_html';
            }
          }
        }
      }

      // Type 2: fields for linked records.  These fields will have any number
      // of form elements that might need populating so we'll offload the
      // loading of these fields to the field itself.
      if ($field_table != $base_table) {

        // Set an empty value by default, and let the hook function update it.
        $entity->{$field_name}['und'][0]['value'] = '';
        tripal_load_include_field_class($field_type);
        if (class_exists($field_type) && method_exists($field_type, 'load')) {
          $tfield = new $field_type($field, $instance);
          $tfield->load($entity, array('record' => $record));
        }
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}
/**
 * Merges the values of all fields into a single array keyed by table name.
 */
function tripal_chado_field_storage_write_merge_fields($fields, $entity_type, $entity) {
  $all_fields = array();
  $base_fields = array();

  // Iterate through all of the fields and organize them into a
  // new fields array keyed by the table name
  foreach ($fields as $field_id => $ids) {

    // Get the field name and information about it.
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];
    $instance = field_info_instance('TripalEntity', $field['field_name'], $entity->bundle);
    // Some fields (e.g. chado_linker_cvterm_adder) don't add data to
    // Chado so they don't have a table, but they are still attached to the
    // entity. Just skip these.
    if (!array_key_exists('chado_table', $instance['settings'])) {
      continue;
    }
    $chado_table = $instance['settings']['chado_table'];
    $chado_column = $instance['settings']['chado_column'];
    $base_table = $instance['settings']['base_table'];

    // Iterate through the field's items. Fields with cardinality ($delta) > 1
    // are multi-valued.
    $items = field_get_items($entity_type, $entity, $field_name);
    $temp = array();
    foreach ($items as $delta => $item) {

      // A field may have multiple items. The field can use items
      // indexed with "chado-" to represent values that should map directly
      // to chado tables and fields.
      $value_set = FALSE;
      foreach ($item as $item_name => $value) {
        $matches = array();
        if (preg_match('/^chado-(.*?)__(.*?)$/', $item_name, $matches)) {
          $table_name = $matches[1];
          $column_name = $matches[2];
          // If this field belongs to the base table then we just add
          // those values in... there's no delta.
          if ($table_name == $base_table) {
            $base_fields[$table_name][$column_name] = $value;
          }
          else {
            $temp[$table_name][$delta][$column_name] = $value;
          }
          $value_set = TRUE;
        }
      }

      // If there is no value set for the field using the
      // chado-[table_name]__[field name] naming schema then check if a 'value'
      // item is present and if so use that for the table column value.
      if (!$value_set and array_key_exists('value', $items[$delta]) and
          !is_array($items[$delta]['value'])) {
        // If this field belongs to the base table then we just add
        // those values in... there's no delta.
        if ($base_table == $chado_table) {
          $base_fields[$chado_table][$chado_column] = $item['value'];
        }
        else {
          $temp[$chado_table][$delta][$chado_column] = $item['value'];
        }
      }
    }

    // Now merge the records for this field with the $new_fields array
    foreach ($temp as $table_name => $details) {
      foreach ($details as $delta => $list) {
        $all_fields[$table_name][] = $list;
      }
    }
  }

  $all_fields = array_merge($base_fields, $all_fields);
  return $all_fields;
}

/**
 * Implements hook_field_storage_query().
 */
function tripal_chado_field_storage_query($query) {
dpm($query);
  // Initialize the result array.
  $result = array(
    'TripalEntity' => array()
  );

  // There must always be an entity filter that includes the bundle. Otherwise
  // it would be too overwhelming to search every table of every content type
  // for a matching field.
  if (!array_key_exists('bundle', $query->entityConditions)) {
    return $result;
  }
  $bundle = tripal_load_bundle_entity(array('name' => $query->entityConditions['bundle']));
  if (!$bundle) {
    return;
  }
  $data_table = $bundle->data_table;
  $type_column = $bundle->type_column;
  $type_id = $bundle->type_id;
  $schema = chado_get_schema($data_table);
  $pkey = $schema['primary key'][0];


  // Initialize the Query object.
  $cquery = chado_db_select($data_table, 'base');
  $cquery->fields('base', array($pkey));

  // Iterate through all the conditions and add to the filters array
  // a chado_select_record compatible set of filters.
  foreach ($query->fieldConditions as $index => $condition) {
    $field = $condition['field'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];

    // Skip conditions that don't belong to this storage type.
    if ($field['storage']['type'] != 'field_chado_storage') {
      continue;
    }

    // The Chado settings for a field are part of the instance and each bundle
    // can have the same field but with different Chado mappings. Therefore,
    // we need to iterate through the bundles to get the field instances.
    foreach ($field['bundles']['TripalEntity'] as $bundle_name) {

      // If there is a bundle filter for the entity and if the field is not
      // associated with the bundle then skip it.
      if (array_key_exists('bundle', $query->entityConditions)) {
        if (strtolower($query->entityConditions['bundle']['operator']) == 'in' and
            !array_key_exists($bundle_name, $query->entityConditions['bundle']['value'])) {
          continue;
        }
        else if ($query->entityConditions['bundle']['value'] != $bundle_name) {
          continue;
        }
      }

      // Allow the field to update the query object.
      $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
      if (tripal_load_include_field_class($field_type)) {
        $field_obj = new $field_type($field, $instance);
        $field_obj->query($cquery, $condition);
      }
      // If there is no ChadoField class for this field then add the
      // condition as is.
      else {
        $alias = $field['field_name'];
        $chado_table = $instance['settings']['chado_table'];
        $base_table = $instance['settings']['base_table'];
        $bschema = chado_get_schema($base_table);
        $bpkey = $bschema['primary key'][0];

        if ($chado_table == $base_table) {
          // Get the base table column that is associated with the term
          // passed as $condition['column'].
          $base_field = tripal_get_chado_semweb_column($chado_table, $condition['column']);
          $cquery->condition('base.' . $base_field , $condition['value'], $condition['operator']);
        }
        if ($chado_table != $base_table) {
          // TODO: I don't think we'll get here because linker fields will
          // always have a custom field that should implement a query()
          // function. But just in case here's a note to handle it.
        }
      }
    }
  } // end foreach ($query->fieldConditions as $index => $condition) {

  // Now join with the chado entity table to get published records only.
  $chado_entity_table = tripal_chado_get_bundle_entity_table($bundle);
  $cquery->join($chado_entity_table, 'CE', "CE.record_id = base.$pkey");
  $cquery->join('tripal_entity', 'TE', "CE.entity_id = TE.id");
  $cquery->fields('CE', array('entity_id'));
  $cquery->fields('TE', array('bundle'));
  if (array_key_exists('start', $query->range)) {
    $cquery->range($query->range['start'], $query->range['length']);
  }
  // Make sure we only get records of the correct entity type
  $cquery->condition('TE.bundle', $query->entityConditions['bundle']['value']);

  // Now set any ordering.
  foreach ($query->order as $index => $sort) {
    // Add in property ordering.
    if ($order['type'] == 'property') {

    }
    // Add in filter ordering
    if ($sort['type'] == 'field') {

      $field = $sort['specifier']['field'];
      $field_type = $field['type'];
      $field_name = $field['field_name'];

      // Skip sorts that don't belong to this storage type.
      if ($field['storage']['type'] != 'field_chado_storage') {
        continue;
      }
      $column = $sort['specifier']['column'];
      $direction = $sort['direction'];

      // The Chado settings for a field are part of the instance and each bundle
      // can have the same field but with different Chado mappings. Therefore,
      // we need to iterate through the bundles to get the field instances.
      foreach ($field['bundles']['TripalEntity'] as $bundle_name) {

        // If there is a bundle filter for the entity and if the field is not
        // associated with the bundle then skip it.
        if (array_key_exists('bundle', $query->entityConditions)) {
          if (strtolower($query->entityConditions['bundle']['operator']) == 'in' and
              !array_key_exists($bundle_name, $query->entityConditions['bundle']['value'])) {
            continue;
          }
          else if ($query->entityConditions['bundle']['value'] != $bundle_name) {
            continue;
          }
        }

        // See if there is a ChadoField class for this instance. If not then do
        // our best to order the field.
        $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
        if (tripal_load_include_field_class($field_type)) {
          $field_obj = new $field_type($field, $instance);
          $field_obj->queryOrder($cquery, array('column' => $column, 'direction' => $direction));
        }
        // There is no class so do our best to order the data by this field
        else {
          $base_table = $instance['settings']['base_table'];
          $chado_table = $instance['settings']['chado_table'];
          $table_column = tripal_get_chado_semweb_column($chado_table, $column);
          if ($table_column) {
             if ($chado_table == $base_table) {
               $cquery->orderBy('base.' . $table_column, $direction);
             }
             else {
               // TODO: how do we handle a field that doesn't map to the base table.
               // We would expect that all of these would be custom field and
               // the ChadoField::queryOrder() function would be implemented.
             }
          }
          else {
            // TODO: handle when the name can't be matched to a table column.
          }
        }
      } // end foreach ($field['bundles']['TripalEntity'] as $bundle_name) {
    } // end if ($sort['type'] == 'field') {
  } // end foreach ($query->order as $index => $sort) {

  //print_r($cquery->__toString());
  //print_r($cquery->getArguments());

  $records = $cquery->execute();

  $result = array();
  while ($record = $records->fetchObject()) {
    $ids = array($record->entity_id, 0, $record->bundle);
    $result['TripalEntity'][$record->entity_id] = entity_create_stub_entity('TripalEntity', $ids);
  }
  return $result;
}
