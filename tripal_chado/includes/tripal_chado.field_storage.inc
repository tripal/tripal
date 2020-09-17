<?php

/**
 * Implements hook_field_storage_info().
 */
function tripal_chado_field_storage_info() {
  return [
    'field_chado_storage' => [
      'label' => t('Chado'),
      'description' => t('Stores fields in the local Chado database.'),
      'settings' => [
        'tripal_storage_api' => TRUE,
      ],
      // The logo_url key is supported by Tripal. It's not a Drupal key. It's
      // used for adding a logo or picture for the data store to help make it
      // more easily recognized on the  field_ui_field_overview_form. Ideally
      // the URL should point to a relative path on the local Drupal site.
      'logo_url' => url(drupal_get_path('module', 'tripal') . '/theme/images/250px-ChadoLogo.png'),
    ],
  ];
}

/**
 * Implements hook_field_storage_write().
 */
function tripal_chado_field_storage_write($entity_type, $entity, $op, $fields) {

  // Get the bundle and the term for this entity.
  $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
  $term = entity_load('TripalTerm', ['id' => $entity->term_id]);
  $term = reset($term);

  // Convert the Tripal term entity into the appropriate record in Chado.
  $dbxref = chado_get_dbxref([
    'accession' => $term->accession,
    'db_id' => ['name' => $term->vocab->vocabulary],
  ]);
  $cvterm = chado_get_cvterm(['dbxref_id' => $dbxref->dbxref_id]);

  // Get the base table, type field and record_id from the entity.
  $base_table = $entity->chado_table;
  $type_field = $entity->chado_column;
  $record = $entity->chado_record;
  $record_id = $entity->chado_record_id;
  $linker = property_exists($entity, 'chado_linker') ? $entity->chado_linker : '';
  $type_id = property_exists($entity, 'chado_type_id') ? $entity->chado_type_id : '';
  $type_value = property_exists($entity, 'chado_type_value') ? $entity->chado_type_value : '';
  $base_type_id = property_exists($entity, 'chado_base_type_id') ? $entity->chado_base_type_id : '';
  $base_schema = chado_get_schema($base_table);
  $base_pkey = $base_schema['primary key'][0];

  // Convert the fields into a key/value list of fields and their values.
  list($field_vals, $field_items) = tripal_chado_field_storage_write_merge_fields($fields, $entity_type, $entity);

  // Start out with the base table fields.
  $values = $field_vals[$base_table];

  // First, set the pkey for the record if this is an update.
  if ($record_id) {
    $values[$base_pkey] = $record_id;
  }
  // For content types that use a type_id in the base table then we need
  // to add in the type_id value.
  elseif ($type_field and !$linker) {
    $values[$type_field] = $cvterm->cvterm_id;
  }
  // If we have a linker table and there is a base_type_id then the base
  // table requires a type_id and we need to add it in.
  elseif ($linker and $base_type_id) {
    // If a base table has a 'type_id' field then we must have the user
    // specify the base table type as well.
    $base_type_column = FALSE;
    $schema = chado_get_schema($base_table);
    foreach ($schema['foreign keys'] as $fk_id => $details) {
      if ($details['table'] == 'cvterm') {
        foreach ($details['columns'] as $fk_left => $fk_right) {
          if ($fk_left == 'type_id') {
            $base_type_column = 'type_id';
          }
        }
      }
    }
    if ($base_type_column) {
      $values[$base_type_column] = $base_type_id;
    }
  }

  // Insert the record into the base table and get the record ID.
  $base_record_id = tripal_chado_field_storage_write_table($base_table, $values, $base_table);
  if (!$base_record_id) {
    throw new Exception('Unable to write fields to Chado: ' . print_r($field_items, TRUE));
  }

  // For content types that use a linker table we need to add a record into
  // the linker table.
  if ($type_field and $linker) {
    // If this is for a property table then there will be a value.
    if ($type_value) {
      $field_vals[$linker][] = [
        $base_pkey => $base_record_id,
        'type_id' => $type_id,
        'value' => $type_value,
      ];
    }
    // If there is no value then this is a cvterm table
    else {
      $args = [':uniquename' => 'null'];
      $sql = "
        SELECT *
        FROM {pub}
        WHERE uniquename = :uniquename
      ";
      $pub = chado_query($sql, $args)->fetchObject();
      if (!$pub) {
        throw new Exception('Unable to save record because the NULL publication is missing from the pub table. Please add a null record where the uniquename value is "null".');
      }
      $field_vals[$linker][] = [
        $base_pkey => $base_record_id,
        'cvterm_id' => $cvterm->cvterm_id,
        'pub_id' => $pub->pub_id,
      ];
    }
  }

  // If this is an insert then add the chado_entity record.
  if ($op == FIELD_STORAGE_INSERT) {
    // Add the record to the proper chado entity table
    $chado_entity_table = chado_get_bundle_entity_table($bundle);
    $record_id = db_insert($chado_entity_table)
      ->fields([
        'entity_id' => $entity->id,
        'record_id' => $base_record_id,
      ])
      ->execute();
    if (!$record_id) {
      throw new Exception('Unable to insert new Chado entity.');
    }
  }

  // Now that we have handled the base table, we need to handle linking tables.
  foreach ($field_vals as $table_name => $details) {
    // Skip the base table as we've already dealt with it.
    if ($table_name == $base_table) {
      continue;
    }
    foreach ($details as $delta => $values) {
      $record_id = tripal_chado_field_storage_write_table($table_name, $values, $base_table, $base_pkey, $base_record_id);
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
function tripal_chado_field_storage_write_table($table_name, $values, $base_table, $base_pkey = NULL, $base_record_id = NULL) {
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

    // Remove the pkey field if it is present (it is empty).
    if (array_key_exists($pkey, $values)) {
      unset($values[$pkey]);
    }

    // Before inserting, we want to make sure the record does not
    // already exist.  Using the unique constraint check for a matching record.
    $options = ['is_duplicate' => TRUE];
    $is_duplicate = chado_select_record($table_name, ['*'], $values, $options);
    if ($is_duplicate) {
      $record = chado_select_record($table_name, ['*'], $values);
      return $record[0]->$pkey;
    }

    // If this table_name is a linker table then we want to be sure to add in
    // the value for the $base_record_id it it isn't set.  This would only
    // occur on an insert.
    if (array_key_exists($base_table, $fkeys) and $base_table != $table_name) {
      foreach ($fkeys[$base_table]['columns'] as $lkey => $rkey) {
        if ($rkey == $base_pkey and !array_key_exists($lkey, $values) or empty($values[$lkey])) {
          $values[$lkey] = $base_record_id;
        }
      }
    }

    // Insert the values array as a new record in the table but remove the
    // pkey as it should be set.
    $new_vals = $values;
    unset($new_vals[$pkey]);
    $record = chado_insert_record($table_name, $new_vals);
    if ($record === FALSE) {
      throw new Exception('Could not insert Chado record into table: "' . $table_name . '": ' . print_r($new_vals, TRUE));
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
 * Implements hook_field_storage_pre_load().
 *
 * Adds a 'chado_record' object containing the base record for the
 * entity.
 */
function tripal_chado_field_storage_pre_load($entity_type, $entities, $age,
                                             $fields, $options) {


  // Itereate through the entities and add in the Chado record.
  foreach ($entities as $id => $entity) {

    // Only deal with Tripal Entities.
    if ($entity_type != 'TripalEntity') {
      continue;
    }

    $record_id = NULL;
    if (property_exists($entity, 'chado_table')) {
      // Get the base table and record id for the fields of this entity.
      $base_table = $entity->chado_table;
      $type_field = $entity->chado_column;
      $record_id = $entity->chado_record_id;
    }
    else {
      $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
      $base_table = $bundle->data_table;
      $type_field = $bundle->type_column;

      // Get the record id for the fields of this entity.
      $chado_entity_table = chado_get_bundle_entity_table($bundle);
      $details = db_select($chado_entity_table, 'ce')
        ->fields('ce')
        ->condition('entity_id', $entity->id)
        ->execute()
        ->fetchObject();
      if ($details) {
        $record_id = isset($details->record_id) ? $details->record_id : '';
      }
      $entity->chado_table = $base_table;
      $entity->chado_record_id = $record_id;
      $entity->chado_column = $type_field;
    }
    if ($record_id) {
      // Get this table's schema.
      $schema = chado_get_schema($base_table);
      $pkey_field = $schema['primary key'][0];

      // Get the base record if one exists
      $columns = ['*'];
      $match = [$pkey_field => $record_id];
      $record = chado_generate_var($base_table, $match);
      $entity->chado_record = $record;
    }
  }
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
    $base_table = $entity->chado_table;
    $type_field = $entity->chado_column;
    $record_id = $entity->chado_record_id;
    $record = $entity->chado_record;
    $schema = chado_get_schema($base_table);

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
            foreach ($schema['foreign keys'] as $table => $fk_details) {
              foreach ($fk_details['columns'] as $lfkey => $rfkey) {
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
          $tfield->load($entity, ['record' => $record]);
        }
        // For text fields that were not handled by a TripalField class we
        // want to automatically expand those fields.
        else {
          if ($schema['fields'][$field_column]['type'] == 'text') {
            $record = chado_expand_var($record, 'field', "$field_table.$field_column");
            $entity->{$field_name}['und'][0]['value'] = $record->$field_column;
            // Text fields that have a text_processing == 1 setting need a
            // special 'format' element too:
            if ([
              key_exists('text_processing', $instance['settings']) and
              $instance['settings']['text_processing'] == 1,
            ]) {
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
          $tfield->load($entity, ['record' => $record]);
        }
      }

    } // end: foreach ($fields as $field_id => $ids) {
  } // end: foreach ($entities as $id => $entity) {
}

/**
 * Merges the values of all fields into a single array keyed by table name.
 */
function tripal_chado_field_storage_write_merge_fields($fields, $entity_type, $entity) {
  $all_fields = [];
  $base_fields = [];
  $field_items = [];

  // Iterate through all of the fields and organize them into a
  // new fields array keyed by the table name
  foreach ($fields as $field_id => $ids) {

    // Get the field name and information about it.
    $field = field_info_field_by_id($field_id);
    $field_name = $field['field_name'];
    $instance = field_info_instance('TripalEntity', $field['field_name'], $entity->bundle);

    // Some fields don't add data to
    // Chado so they don't have a table, but they are still attached to the
    // entity. Just skip these.
    if (!array_key_exists('chado_table', $instance['settings'])) {
      continue;
    }
    $chado_table = $instance['settings']['chado_table'];
    $chado_column = $instance['settings']['chado_column'];
    $base_table = $instance['settings']['base_table'];

    // We want to iterate through the field's items. We will also store the
    // field values in the $field_items array to be returned by this function.
    $items = field_get_items($entity_type, $entity, $field_name);
    $temp = [];
    $field_items[$field_name] = $items;

    // If there are no items for the field and it's a base table field then
    // we need to set it to NULL so that the value can be removed.
    if (empty($items)) {
      if (empty($items) and ($chado_table == $base_table)) {
        $base_fields[$chado_table][$chado_column] = '__NULL__';
      }
    }
    else {
      if (is_array($items)) {
        // Note: fields with cardinality ($delta) > 1 are multi-valued.
        foreach ($items as $delta => $item) {

          // A field may have multiple items. The field can use items
          // indexed with "chado-" to represent values that should map directly
          // to chado tables and fields.
          $value_set = FALSE;
          foreach ($item as $item_name => $value) {
            $matches = [];
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
    }
  }

  $all_fields = array_merge($base_fields, $all_fields);
  return [$all_fields, $field_items];
}

/**
 * Implements hook_field_storage_query().
 */
function tripal_chado_field_storage_query($query) {

  // Initialize the result array.
  $result = [
    'TripalEntity' => [],
  ];

  // There must always be an entity filter that includes the bundle. Otherwise
  // it would be too overwhelming to search every table of every content type
  // for a matching field.
  if (!array_key_exists('bundle', $query->entityConditions)) {
    return $result;
  }
  $bundle = tripal_load_bundle_entity(['name' => $query->entityConditions['bundle']]);
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
  $cquery->fields('base', [$pkey]);

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
        else {
          if ($query->entityConditions['bundle']['value'] != $bundle_name) {
            continue;
          }
        }
      }

      // Allow the field to update the query object.
      $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
      if (tripal_load_include_field_class($field_type)) {
        $field_obj = new $field_type($field, $instance);
        $field_obj->query($cquery, $condition);
      }
      // If there is not a ChadoField class for this field then add the
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
          $base_field = chado_get_semweb_column($chado_table, $condition['column']);

          $matches = [];
          $key = $condition['value'];
          $op = array_key_exists('operator', $condition) ? $condition['operator'] : '=';
          if (preg_match('/^(.+);(.+)$/', $key, $matches)) {
            $key = $matches[1];
            $op = $matches[2];
          }

          switch ($op) {
            case 'eq':
              $op = '=';
              break;
            case 'gt':
              $op = '>';
              break;
            case 'gte':
              $op = '>=';
              break;
            case 'lt':
              $op = '<';
              break;
            case 'lte':
              $op = '<=';
              break;
            case '<>':
            case 'ne':
              $op = '!=';
              break;
            case 'LIKE':
            case 'contains':
              $op = 'LIKE';
              if (!preg_match('/\%/', $key)) {
                $key = '%' . $key . '%';
              }
              break;
            case 'NOT LIKE':
              $op = 'NOT LIKE';
              if (!preg_match('/\%/', $key)) {
                $key = '%' . $key . '%';
              }
              break;
            case 'starts':
              $op = 'LIKE';
              $key = $key . '%';
              break;
            default:
              $op = '=';
          }
          $cquery->condition('base.' . $base_field, $key, $op);
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
  $chado_entity_table = chado_get_bundle_entity_table($bundle);
  $cquery->join($chado_entity_table, 'CE', "CE.record_id = base.$pkey");
  $cquery->join('tripal_entity', 'TE', "CE.entity_id = TE.id");
  $cquery->fields('CE', ['entity_id']);
  $cquery->fields('TE', ['bundle']);
  if (array_key_exists('start', $query->range)) {
    $cquery->range($query->range['start'], $query->range['length']);
  }
  // Make sure we only get records of the correct entity type
  $cquery->condition('TE.bundle', $query->entityConditions['bundle']['value']);

  // Iterate through all the property conditions. These will be filters
  // on the tripal_entity table.
  foreach ($query->propertyConditions as $index => $condition) {
    $cquery->condition('TE.' . $condition['column'], $condition['value']);
  }

  // If there is an entity_id filter then apply that.
  if (array_key_exists('entity_id', $query->entityConditions)) {
    $value = $query->entityConditions['entity_id']['value'];
    $op = '';
    if (array_key_exists('op', $query->entityConditions['entity_id'])) {
      $op = $query->entityConditions['entity_id']['op'];
    }
    // Handle a single entity_id
    if (!is_array($value)) {
      switch ($op) {
        case 'CONTAINS':
          $op = 'LIKE';
          $value = '%' . $value . '%';
          break;
        case 'STARTS_WITH':
          $op = 'LIKE';
          $value = $value . '%';
          break;
        case 'BETWEEN':
          // This case doesn't make sense for a single value. So, just set it
          // equal to the end.
          $op = '=';
        default:
          $op = $op ? $op : '=';
      }
    }
    // Handle multiple entitie IDs.
    else {
      switch ($op) {
        case 'CONTAINS':
          $op = 'IN';
          break;
        case 'STARTS_WITH':
          $op = 'IN';
          break;
        case 'BETWEEN':
          $op = 'BETWEEN';
        default:
          $op = 'IN';
      }
    }
    if ($op == 'BETWEEN') {
      $cquery->condition('TE.id', $value[0], '>');
      $cquery->condition('TE.id', $value[1], '<');
    }
    else {
      $cquery->condition('TE.id', $value, $op);
    }
  }

  // Now set any ordering.
  foreach ($query->order as $index => $sort) {
    // Add in property ordering.
    if ($sort['type'] == 'property') {
      // TODO: support ordering by bundle properties.
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
          else {
            if ($query->entityConditions['bundle']['value'] != $bundle_name) {
              continue;
            }
          }
        }

        // See if there is a ChadoField class for this instance. If not then do
        // our best to order the field.
        $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
        if (tripal_load_include_field_class($field_type)) {
          $field_obj = new $field_type($field, $instance);
          $field_obj->queryOrder($cquery, [
            'column' => $column,
            'direction' => $direction,
          ]);
        }
        // There is no class so do our best to order the data by this field
        else {
          $base_table = $instance['settings']['base_table'];
          $chado_table = $instance['settings']['chado_table'];
          $table_column = chado_get_semweb_column($chado_table, $column);
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


  // Only include records that are deleted.  Tripal doesn't keep track of
  // records that are deleted that need purging separately so we can do nothing
  // with this.
  if (property_exists($query, 'deleted') and $query->deleted) {
    // There won't ever be field data marked as deleted so just created a
    // condition that always evaluates to false.
    $cquery->where('1=0');
  }

  // Please keep these dpm's they help with debugging
  // dpm($cquery->__toString());
  // dpm($cquery->getArguments());

  $records = $cquery->execute();

  // If this is not a count query then return the results.
  $result = [];
  while ($record = $records->fetchObject()) {
    $ids = [$record->entity_id, 0, $record->bundle];
    $result['TripalEntity'][$record->entity_id] = entity_create_stub_entity('TripalEntity', $ids);
  }
  return $result;
}


function tripal_chado_field_storage_bundle_mapping_form_get_defaults($form, $form_state) {

  // The term for the content type should already be set in the form state.
  $term = $form_state['values']['term'];

  // Get the form default values.
  $default = [
    'table' => '',
    'has_all' => '',
    'use_linker' => '',
    'use_prop' => '',
    'use_cvterm' => '',
    'cv_id' => '',
    'type_column' => '',
    'base_selected_term' => '',
    'prop_selected_term' => '',
    'prop_term_value' => '',
    'stage' => 'table-select',
  ];

  // Get the stage.
  if (array_key_exists('chado-stage', $form_state)) {
    $default['stage'] = $form_state['chado-stage'];
  }
  if (array_key_exists('base_chado_table', $form_state['values'])
    and $form_state['values']['base_chado_table']) {
    $default['table'] = $form_state['values']['base_chado_table'];
  }
  else {
    $mapped_table = chado_get_cvterm_mapping(['cvterm_id' => $term->cvterm_id]);
    if ($mapped_table) {
      $default['table'] = $mapped_table->chado_table;
    }
  }
  if (array_key_exists('chado_table_has_all', $form_state['values'])
    and $form_state['values']['chado_table_has_all']) {
    $default['has_all'] = $form_state['values']['chado_table_has_all'];
  }
  if (array_key_exists('chado_type_use_linker', $form_state['values'])
    and $form_state['values']['chado_type_use_linker']) {
    $default['use_linker'] = $form_state['values']['chado_type_use_linker'];
  }
  if (array_key_exists('chado_type_use_prop', $form_state['values'])
    and $form_state['values']['chado_type_use_prop']) {
    $default['use_prop'] = $form_state['values']['chado_type_use_prop'];
  }
  if (array_key_exists('type_column', $form_state['values'])
    and $form_state['values']['type_column']) {
    $default['type_column'] = $form_state['values']['type_column'];
  }
  if (array_key_exists('prop_term_value', $form_state['values'])
    and $form_state['values']['prop_term_value']) {
    $default['prop_term_value'] = $form_state['values']['prop_term_value'];
  }
  if (array_key_exists('term_name1', $form_state['values'])) {
    $default['base_selected_term'] = tripal_get_term_lookup_form_result($form, $form_state, '', 1);
  }
  if (array_key_exists('term_name2', $form_state['values'])) {
    $default['prop_selected_term'] = tripal_get_term_lookup_form_result($form, $form_state, '', 2);
  }
  if (array_key_exists('chado_type_use_cv', $form_state['values'])
    and $form_state['values']['chado_type_use_cv']) {
    $default['use_cvterm'] = $form_state['values']['chado_type_use_cv'];
  }
  if (array_key_exists('chado_type_cv_id', $form_state['values'])
    and $form_state['values']['chado_type_cv_id']) {
    $default['cv_id'] = $form_state['values']['chado_type_cv_id'];
  }
  return $default;
}

/**
 * Implements hook_field_storage_bundle_mapping_form().
 *
 * This is a Tripal added hook to the field storage API.
 */
function tripal_chado_field_storage_bundle_mapping_form($form, &$form_state, $term, &$submit_disabled) {

  $selected_term_id = (is_object($term)) ? $term->cvterm_id : NULL;
  $submit_disabled = TRUE;

  // Get the form defaults values from the form state.
  $default = tripal_chado_field_storage_bundle_mapping_form_get_defaults($form, $form_state);

  // Initialize the form.
  $form = [];

  $form['selected_cvterm_id'] = [
    '#type' => 'value',
    '#value' => $selected_term_id,
  ];

  tripal_chado_field_storage_bundle_mapping_form_summary($form, $form_state, $default);

  if ($default['stage'] == 'table-select') {
    tripal_chado_field_storage_bundle_mapping_form_table_select($form, $form_state, $term, $default);
  }
  else {
    if ($default['stage'] == 'has-all-select') {
      tripal_chado_field_storage_bundle_mapping_form_has_all_select($form, $form_state, $term, $default);
    }
    else {
      if ($default['stage'] == 'type-select') {
        tripal_chado_field_storage_bundle_mapping_form_type_select($form, $form_state, $term, $default);
      }
      else {
        if ($default['stage'] == 'prop-select') {
          tripal_chado_field_storage_bundle_mapping_form_prop_select($form, $form_state, $term, $default);
        }
        else {
          if ($default['stage'] == 'prop-settings') {
            tripal_chado_field_storage_bundle_mapping_form_prop_settings($form, $form_state, $term, $default);
          }
          else {
            if ($default['stage'] == 'linker-select') {
              tripal_chado_field_storage_bundle_mapping_form_linker_select($form, $form_state, $term, $default);
            }
            else {
              if ($default['stage'] == 'linker-settings') {
                tripal_chado_field_storage_bundle_mapping_form_linker_settings($form, $form_state, $term, $default);
              }
              else {
                if ($default['stage'] == 'complete') {
                  $submit_disabled = FALSE;
                }
              }
            }
          }
        }
      }
    }
  }

  $form['chado-settings-reset'] = [
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#name' => 'chado-settings-reset',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];

  return $form;
}

/**
 * Adds form elements for the summary of the selection.
 */
function tripal_chado_field_storage_bundle_mapping_form_summary(&$form, &$form_state, $default) {
  $rows = [];
  $headers = [];

  // If we're on the first step then just return, there is no summary.
  if ($default['stage'] == 'table-select') {
    return;
  }

  // Make sure the form values are set.
  $form['base_chado_table'] = [
    '#type' => 'value',
    '#value' => $default['table'],
  ];
  $form['chado_table_has_all'] = [
    '#type' => 'value',
    '#value' => $default['has_all'],
  ];
  $form['type_column'] = [
    '#type' => 'value',
    '#value' => $default['type_column'],
  ];
  $form['chado_type_use_prop'] = [
    '#type' => 'value',
    '#value' => $default['use_prop'],
  ];
  $form['prop_term'] = [
    '#type' => 'value',
    '#value' => $default['prop_selected_term'],
  ];
  $form['base_term'] = [
    '#type' => 'value',
    '#value' => $default['base_selected_term'],
  ];
  $form['prop_term_value'] = [
    '#type' => 'value',
    '#value' => $default['prop_term_value'],
  ];
  $form['chado_type_use_linker'] = [
    '#type' => 'value',
    '#value' => $default['use_linker'],
  ];
  $form['chado_type_use_cv'] = [
    '#type' => 'value',
    '#value' => $default['use_cvterm'],
  ];
  $form['chado_type_cv_id'] = [
    '#type' => 'value',
    '#value' => $default['cv_id'],
  ];

  //
  // Add the base table row.
  //
  $rows[] = [
    [
      'data' => 'Chado Base Table',
      'header' => TRUE,
      'width' => '20%',
    ],
    $default['table'],
  ];
  //
  // Add the row indicating if all of the records are of the type.
  //
  if ($default['has_all'] == 'Yes' or $default['has_all'] == 'No') {

    $message = 'Yes. All records in the ' . $default['table'] . ' table are of this type.';
    if ($default['has_all'] == 'No') {
      $message = 'No. All records in the ' . $default['table'] . ' table are not of this type.';
    }
    $rows[] = [
      [
        'data' => 'All are of this type?',
        'header' => TRUE,
        'width' => '20%',
      ],
      $message,
    ];
  }
  //
  // Add the row indicating if the type_column is used.
  //
  if ($default['type_column']) {
    $message = $default['type_column'];
    if ($default['type_column'] == 'none') {
      $message = 'No type column is needed.';
    }
    $rows[] = [
      [
        'data' => 'Type Column',
        'header' => TRUE,
        'width' => '20%',
      ],
      $message,
    ];
  }
  //
  // Add the row indicating that the record can be distinguished with a prop.
  //
  if ($default['use_prop']) {
    $message = 'Yes. The field is distinguishable using a property record.';
    if ($default['use_prop'] == 'No') {
      $message = 'No. The field is not distinguishable using a property record.';
    }
    $rows[] = [
      [
        'data' => 'Use a property?',
        'header' => TRUE,
        'width' => '20%',
      ],
      $message,
    ];
  }
  //
  //  Add the row for the base type for a property mapping
  //
  if ($default['use_prop'] == 'Yes' and $default['base_selected_term']) {
    $term = $default['base_selected_term'][0];
    $rows[] = [
      [
        'data' => 'Base Type',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->name . ' (' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . ')',
    ];
  }
  //
  //  Add the row for the type for a property mapping
  //
  if ($default['prop_selected_term']) {
    $term = $default['prop_selected_term'][0];
    $rows[] = [
      [
        'data' => 'Property Type',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->name . ' (' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . ')',
    ];
  }
  //
  //  Add the row for the value for a property mapping
  //
  if ($default['prop_term_value']) {
    $rows[] = [
      [
        'data' => 'Property Value',
        'header' => TRUE,
        'width' => '20%',
      ],
      $default['prop_term_value'],
    ];
  }
  //
  //  Add the row for the linker table selection
  //
  if ($default['use_linker']) {
    $message = 'Yes. The field is distinguishable using a cvterm linker table. The Content Type will be used to find records in the linker table';
    if ($default['use_linker'] == 'No') {
      $message = 'No. The field is not distinguishable using a cvterm linker table.';
    }
    $rows[] = [
      [
        'data' => 'Use a Chado cvterm linker?',
        'header' => TRUE,
        'width' => '20%',
      ],
      $message,
    ];
  }

  //
  //  Add the row for the base type for a property mapping
  //
  if ($default['use_linker'] == 'Yes' and $default['base_selected_term']) {
    $term = $default['base_selected_term'][0];
    $rows[] = [
      [
        'data' => 'Base Type',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->name . ' (' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . ')',
    ];
  }

  //
  // Add the row for cvterm options
  //
  if ($default['use_cvterm']) {
    $message = 'No. All records belong to a single controlled vocabulary.';
    if ($default['use_cvterm'] == 'parent') {
      $message = 'Yes. Records should include all child records of the specified term.';
    }
    $rows[] = [
      [
        'data' => 'Use a Parent Chado cvterm?',
        'header' => TRUE,
        'width' => '20%',
      ],
      $message,
    ];

    // Add the cv if needed
    if ($default['cv_id'] > 0) {
      $cv_name = chado_query('SELECT name FROM chado.cv WHERE cv_id=:id',
        [':id' => $default['cv_id']])->fetchField();
      $rows[] = [
        [
          'data' => 'Restrict to Vocabulary',
          'header' => TRUE,
          'width' => '20%',
        ],
        $cv_name,
      ];
    }
  }

  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  $form['chado_settings_summary'] = [
    '#type' => 'markup',
    '#markup' => theme_table($table),
    '#weight' => -100,
  ];

  if ($default['stage'] == 'complete') {
    $form['im_ready'] = [
      '#type' => 'markup',
      '#markup' => '<p>Your content type is ready for creation! Please click the "Create Content Type" button below.</p>',
    ];
  }
  if ($default['stage'] == 'stop') {
    $form['im_ready'] = [
      '#type' => 'markup',
      '#markup' => '<p>Unfortunately, it is not possible to create this conent type. Please check your selections above. If you need help creating a content type, you may reach out to the Tripal Community for assistance.</p>',
    ];
  }
}

/**
 * Adds form elements for Chado table selection step
 */
function tripal_chado_field_storage_bundle_mapping_form_table_select(&$form, &$form_state, $term, $default) {

  $base_tables = chado_get_base_tables();
  $options = [0 => '-- Select table --'];
  foreach ($base_tables AS $tablename) {
    $options[$tablename] = $tablename;
  }
  $form['base_chado_table'] = [
    '#type' => 'select',
    '#title' => 'Chado table',
    '#options' => $options,
    '#description' => 'Select the Chado table into which the primary records for this content type will be stored.',
    '#default_value' => $default['table'],
    '#ajax' => [
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-add-type-form",
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];
  $form['table-select-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-table-select-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}


/**
 * Adds form elements for step 1: Chado type field selection.
 */
function tripal_chado_field_storage_bundle_mapping_form_has_all_select(&$form, &$form_state, $term, $default) {

  // Get the schema for this table.
  $schema = chado_get_schema($default['table']);

  // If the selected base table is 'cvterm' then handle it specially.
  if ($default['table'] == 'cvterm') {
    tripal_chado_field_storage_bundle_mapping_form_add_cvterm($form,
      $form_state, $term, $default);
    if ($default['use_cvterm'] == 'cv' and $default['cv_id']) {
      $submit_disabled = FALSE;
    }
    if ($default['use_cvterm'] == 'parent') {
      $submit_disabled = FALSE;
    }
    return;
  }

  $term_name = $term->name;

  // Form elements to determine if all records in base table are of this type.
  $form['chado_table_has_all'] = [
    '#type' => 'radios',
    '#options' => [
      'Yes' => 'Yes',
      'No' => 'No',
    ],
    '#title' => 'Are all records in the "' . $default['table'] .
      '" table of type "' . $term_name . '"?',
    '#description' => 'Select "No" if the "' .
      $default['table'] . '" table houses more than just data of type "' .
      $term_name . '".',
    '#default_value' => $default['has_all'],
  ];

  $form['chado-has-all-select-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-has-all-select-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}

/*
  // If all the records in the table are of this type then we're done.
  if ($default['has_all'] == 'Yes') {
    $submit_disabled = FALSE;
    return $form;
  }

  // If this table has a field that maps to a cvterm record then we want
  // to ask if the user wants to use that field.
  tripal_chado_field_storage_bundle_mapping_form_add_type($form,
      $form_state, $term, $default);

  // If the type_column is set then we're done!  We know the deafult table
  // and column to map this data type.
  if (!empty($default['type_column']) and $default['type_column'] != 'none') {
    $submit_disabled = FALSE;
    return $form;
  }

  // Let's set the names of the linker and prop table for use below.
  $linker_table = $default['table'] . '_cvterm';
  $prop_table = $default['table']. 'prop';
  $linker_exists = chado_table_exists($linker_table);
  $prop_exists = chado_table_exists($prop_table);

  // Ask the user if they want to use a linker table if it exists.
  if ($prop_exists) {

    tripal_chado_field_storage_bundle_mapping_form_add_prop($form,
        $form_state, $term, $prop_table, $default);

    // If the user wants to use the property table then we're done!
    if ($default['use_prop'] == 'Yes') {
      $submit_disabled = FALSE;
      return $form;
    }

    // Ask if the user wants to use the linker table if it exists.
    if ($linker_exists) {
      tripal_chado_field_storage_bundle_mapping_form_add_linker($form,
          $form_state, $term, $linker_table, $default);

      // If the user wants to use the linker table then we're done!
      if ($default['use_linker'] == 'Yes') {
        $submit_disabled = FALSE;
        return $form;
      }
    }

  }
  // A prop table doesn't exist then ask the user if they want
  // to use the linker table to associate records.
  else if ($linker_exists) {
    tripal_chado_field_storage_bundle_mapping_form_add_prop($form,
        $form_state, $term, $prop_table, $default);

    // If the user wants to use the linker table then we're done!
    if ($default['use_linker'] == 'Yes') {
      $submit_disabled = FALSE;
      return $form;
    }
  }

  $form['notice'] = array(
    '#type' => 'item',
    '#markup' => '<font color="red">Unfortunately, the options selected above do not allow for mapping of this content type to records in Chado.</font>'
  );
  return $form;
}
 */

/**
 * Adds form elements for step 1: Chado type field selection.
 */
function tripal_chado_field_storage_bundle_mapping_form_type_select(&$form, &$form_state, $term, $default) {

  $term_name = $term->name;
  $column_options = tripal_chado_field_storage_bundle_mapping_form_get_type_fks_options($default['table']);

  $default_column = !empty($default['type_column']) ? $default['type_column'] : 'type_id';

  $form['type_column'] = [
    '#type' => 'select',
    '#title' => 'Type Column',
    '#options' => $column_options,
    '#description' => 'Please select the column in the "' .
      $default['table'] . '" table that will identify a record as being of type "' .
      $term_name . '". If you select "--None--" then you will be asked
      if the type can be identified using a property or linker table.
      Only fields that have a foreign key to the cvterm table will be listed.',
    '#default_value' => $default_column,
  ];
  $form['chado-type-select-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-type-select-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_prop_select(&$form, &$form_state, $term, $default) {

  $prop_table = $default['table'] . 'prop';

  $form['chado_type_use_prop'] = [
    '#type' => 'radios',
    '#title' => 'Do you want to use the "' . $prop_table . '" table
      to distinguish between content types?',
    '#options' => [
      'Yes' => 'Yes',
      'No' => 'No',
    ],
    '#description' => t('Sometimes records can be distinguished
      using a value in property table, especially if there is no column
      in the specified Chado table to identify the
      record type. In these cases the property table can be used.'),
    '#default_value' => $default['use_prop'],
  ];
  $form['chado-prop-select-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-prop-select-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}

function tripal_chado_field_storage_bundle_mapping_form_prop_settings(&$form, &$form_state, $term, $default) {

  $prop_term_value = $default['prop_term_value'];

  $base_type_column = tripal_chado_field_storage_bundle_mapping_form_get_base_type_column($default['table']);

  $validate = [
    'tripal_chado_field_storage_bundle_mapping_form_validate',
    'tripal_admin_add_type_form_validate',
  ];

  if ($base_type_column) {
    $description = t('The ' . $default['table'] . ' table of Chado requires
      that each record have a type.  You have indicated that this
      content type is not distinguished by a column in the base tabe but
      instead by a property.  However, the ' . $default['table'] . ' table requires
      that a value be given for the ' . $base_type_column . '. Please indicate
      what this type should be.  For example, suppose this content type is a SNP (SO:0000694) and
      you store SNPs in Chado in the feature table as "genetic_marker" (SO:0001645), however
      the record is defined as a SNP through a corresponding record in the featurprop
      table that has a type of "type" (rdfs:type) and a value of "SNP".
      In this case the content type should be SNP (SO:0001645) and here the
      "genetic_marker" (SO:0001645) term should be provided. The "type" term
      should be used in the Property Type field below and "SNP" as the Property Value.');
    tripal_get_term_lookup_form($form, $form_state, '',
      'Base Type', $description, FALSE, '', 1,
      'tripal_admin_add_type_form_ajax_callback', 'tripal-add-type-form',
      $validate, -2);
  }

  // We need a term lookup form for the property type.
  $description = t('The content type "' . $term->name . '" must be
        associated with a property type.  The property type must be the
        name of a term in a controlled vocabulary and the controlled
        vocabulary should already be loaded into Tripal. Please select
        the property type that will be used to identify records of this
        type');
  tripal_get_term_lookup_form($form, $form_state, '',
    'Property Type', $description, FALSE, '', 2,
    'tripal_admin_add_type_form_ajax_callback', 'tripal-add-type-form',
    $validate, -8);

  $form['prop_term_value'] = [
    '#type' => 'textfield',
    '#title' => 'Property Value',
    '#description' => t('All properties have a type and a value.  Above you
        indicated the type of property. Now you must specify the value
        that this properly must have in order to be considered of
        type "' . $term->name . '".'),
    '#default_value' => $prop_term_value,
  ];
  $form['chado-prop-settings-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-prop-settings-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_linker_select(&$form, &$form_state, $term, $default) {
  $linker_table = $default['table'] . '_cvterm';
  $form['chado_type_use_linker'] = [
    '#type' => 'radios',
    '#title' => 'Do you want to use the "' . $linker_table . '" table
      to distinguish between content types?',
    '#options' => [
      'Yes' => 'Yes',
      'No' => 'No',
    ],
    '#description' => t('Sometimes records can be distinguished
      using a linker table, especially if there is no column
      in the specified Chado table to identify the
      record type. In these cases the linker table can be used.'),
    '#default_value' => $default['use_linker'],
  ];
  $form['chado-linker-select-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-linker-select-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_linker_settings(&$form, &$form_state, $term, $default) {

  $base_type_column = tripal_chado_field_storage_bundle_mapping_form_get_base_type_column($default['table']);

  $validate = [
    'tripal_chado_field_storage_bundle_mapping_form_validate',
    'tripal_admin_add_type_form_validate',
  ];

  $description = t('The ' . $default['table'] . ' table of Chado requires
    that each record have a type.  You have indicated that this
    content type is not distinguished by a column in the base tabe but
    instead by a cvterm linker table.  However, the ' . $default['table'] . ' table requires
    that a value be given for the ' . $base_type_column . '. Please indicate
    what this type should be.');
  tripal_get_term_lookup_form($form, $form_state, '',
    'Base Type', $description, FALSE, '', 1,
    'tripal_admin_add_type_form_ajax_callback', 'tripal-add-type-form',
    $validate, -10);

  $form['chado-linker-settings-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-linker-settings-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_get_type_fks_options($table) {
  // Get the list of columns in the default table.
  $schema = chado_get_schema($table);
  $column_options = ['none' => '--None--'];
  $cvt_fkeys = (isset($schema['foreign keys']['cvterm'])) ? array_keys($schema['foreign keys']['cvterm']['columns']) : [];
  foreach ($schema['fields'] as $column_name => $column_details) {
    if (in_array($column_name, $cvt_fkeys)) {
      $column_options[$column_name] = $column_name;
    }
  }
  return $column_options;
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_get_base_type_column($table) {

  // If a base table has a 'type_id' field then we must have the user
  // specify the base table type as well.
  $base_type_column = FALSE;
  $schema = chado_get_schema($table);
  foreach ($schema['foreign keys'] as $fk_id => $details) {
    if ($details['table'] == 'cvterm') {
      foreach ($details['columns'] as $fk_left => $fk_right) {
        if ($fk_left == 'type_id') {
          $base_type_column = 'type_id';
        }
      }
    }
  }
  return $base_type_column;
}

/**
 * Provides the option form needed for a cvterm-based content type.
 */
function tripal_chado_field_storage_bundle_mapping_form_add_cvterm(&$form,
                                                                   &$form_state, $term, &$default) {

  $form['chado_type_use_cv'] = [
    '#type' => 'radios',
    '#title' => 'How do you want to distinguish the "' . $term->name . '" records
      within the cvterm table?',
    '#options' => [
      'cv' => 'All records that belong to a single controlled vocabulary?',
      //  SPF: Commenting out until we can write the code to deal with
      //  using a parent term to indicate child elements that belong to the
      //  content type.
      // 'parent' => 'All child records of the specified term?'
    ],
    '#description' => t('Records in the cvterm table are often not used for content
        types. The records in the cvterm table are meant to be vocabulary terms.
        However, there are times when records in the cvterm table may be
        used for a content type. One example is for trait "pages" that
        use phenotype values stored in the phenotype table. The vocabulary that
        contains the "trait" terms should then be provided.'),
    '#default_value' => $default['use_cvterm'],
  ];
  if ($default['use_cvterm'] == 'cv') {
    $cvs = chado_get_cv_select_options();
    $form['chado_type_cv_id'] = [
      '#type' => 'select',
      '#options' => $cvs,
      '#title' => t('Select a controlled vocabulary'),
      '#default_value' => $default['cv_id'],
    ];
  }

  $form['chado-cvterm-settings-continue'] = [
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#name' => 'chado-cvterm-settings-continue',
    '#validate' => [
      'tripal_chado_field_storage_bundle_mapping_form_validate',
      'tripal_admin_add_type_form_validate',
    ],
  ];

}

/**
 * Implements hook_field_stoage_bundle_mapping_form_validate().
 */
function tripal_chado_field_storage_bundle_mapping_form_validate($form, &$form_state) {

  // Don't do validation on an ajax callback.
  if (array_key_exists('#ajax', $form_state['triggering_element'])) {
    return;
  }

  // Get the form values.
  $default = tripal_chado_field_storage_bundle_mapping_form_get_defaults($form, $form_state);
  $clicked_button = $form_state['clicked_button']['#name'];

  // Check if prop  and linker tables exist. We'll use these values later.
  $prop_table = $default['table'] . 'prop';
  $prop_exists = chado_table_exists($prop_table);
  $linker_table = $default['table'] . '_cvterm';
  $linker_exists = chado_table_exists($linker_table);

  //
  // Valiate the table select stage
  //
  if ($clicked_button == 'chado-table-select-continue') {
    // Make sure a default table is selected
    if (!$default['table']) {
      form_set_error('base_chado_table', 'Please select a default table.');
      return;
    }
    $form_state['chado-stage'] = 'has-all-select';
  }

  //
  // Valiate the has all select stage
  //
  if ($clicked_button == 'chado-has-all-select-continue') {
    if ($default['has_all'] == 'Yes') {
      $form_state['chado-stage'] = 'complete';
    }
    else {
      $column_options = tripal_chado_field_storage_bundle_mapping_form_get_type_fks_options($default['table']);
      // If this table has fields that link to the cvterm table then
      // let the user select the type column. The column_options is empty if
      // it equals 1 because it has the --none-- element.
      if (count($column_options) > 1) {
        $form_state['chado-stage'] = 'type-select';
      }
      // Otherwise, see if there are other options....
      else {
        // If there is no type_id or the type_id column should not be
        // used then we need to let the user select if they want to
        // use the prop table or a cvterm linking table. We'll offer the
        // prop table first.
        if ($prop_exists) {
          $form_state['chado-stage'] = 'prop-select';
        }
        else {
          if ($linker_exists) {
            $form_state['chado-stage'] = 'linker-select';
          }
          else {
            $form_state['chado-stage'] = 'stop';
          }
        }
      }

    }
  }

  //
  // Valiate the type select stage
  //
  if ($clicked_button == 'chado-type-select-continue') {
    if ($default['type_column'] != 'none') {
      $form_state['chado-stage'] = 'complete';
    }
    else {
      // If there is no type_id or the type_id column should not be
      // used then we need to let the user select if they want to
      // use the prop table or a cvterm linking table. We'll offer the
      // prop table first.
      if ($prop_exists) {
        $form_state['chado-stage'] = 'prop-select';
      }
      else {
        if ($linker_exists) {
          $form_state['chado-stage'] = 'linker-select';
        }
        else {
          $form_state['chado-stage'] = 'stop';
        }
      }
    }
  }

  //
  // Valiate the property select stage
  //
  if ($clicked_button == 'chado-prop-select-continue') {
    if ($default['use_prop'] == 'Yes') {
      $form_state['chado-stage'] = 'prop-settings';
    }
    else {
      if ($default['use_prop'] == 'No') {
        if ($linker_exists) {
          $form_state['chado-stage'] = 'linker-select';
        }
        else {
          $form_state['chado-stage'] = 'stop';
        }
      }
      else {
        form_set_error('chado_type_use_prop', 'Please select a value.');
      }
    }

  }

  //
  // Validate the property settings stage
  //
  if ($clicked_button == 'chado-prop-settings-continue') {
    // If a base type was needed then make sure we have a value selected.
    $valid = TRUE;
    if (array_key_exists('term_name1', $form_state['values'])) {
      $base_selected = $default['base_selected_term'];
      if (count($base_selected) == 0) {
        form_set_error('term_match1][term_name1', 'Please select a base table type vocabulary term.');
        $valid = FALSE;
      }
      if (count($base_selected) > 1) {
        form_set_error('term_match1][term_name1', 'Please select only one base table type vocabulary term.');
        $valid = FALSE;
      }
    }
    // Make sure we only have one property type selected
    $prop_selected = $default['prop_selected_term'];
    if (count($prop_selected) == 0) {
      form_set_error('term_match2][term_name2', 'Please select a prpoerty type vocabulary term.');
      $valid = FALSE;
    }
    if (count($prop_selected) > 1) {
      form_set_error('term_match2][term_name2', 'Please select only one property type vocabulary term.');
      $valid = FALSE;
    }
    if (!$default['prop_term_value']) {
      form_set_error('prop_term_value', 'Please provide a property value.');
      $valid = FALSE;
    }
    if (!$valid) {
      return;
    }
    // If we're here then all validations passed and we are good to go!
    $form_state['chado-stage'] = 'complete';
  }

  //
  // Validate the linker settings stage
  //
  if ($clicked_button == 'chado-linker-select-continue') {
    if ($default['use_linker'] == 'Yes') {
      // If our base table has a type_id then we need need to make the
      // user provide a value for it, otherwise we're done.
      $base_type_column = tripal_chado_field_storage_bundle_mapping_form_get_base_type_column($default['table']);
      if ($base_type_column) {
        $form_state['chado-stage'] = 'linker-settings';
      }
      else {
        $form_state['chado-stage'] = 'complete';
      }
    }
    else {
      if ($default['use_linker'] == 'No') {
        $form_state['chado-stage'] = 'stop';
      }
      else {
        form_set_error('chado_type_use_linker', 'Please select a value.');
      }
    }
  }

  if ($clicked_button == 'chado-linker-settings-continue') {
    $valid = TRUE;
    if (array_key_exists('term_name1', $form_state['values'])) {
      $base_selected = $default['base_selected_term'];
      if (count($base_selected) == 0) {
        form_set_error('term_match1][term_name1', 'Please select a base table type vocabulary term.');
        $valid = FALSE;
      }
      if (count($base_selected) > 1) {
        form_set_error('term_match1][term_name1', 'Please select only one base table type vocabulary term.');
        $valid = FALSE;
      }
    }
    if (!$valid) {
      return;
    }

    // If we're here then all validations passed and we are good to go!
    $form_state['chado-stage'] = 'complete';
  }

  if ($clicked_button == 'chado-cvterm-settings-continue') {
    // If the user chose to differentiate type based on cv_id and supplied a cv_id
    // then we're done!
    if ($form_state['values']['chado_type_use_cv'] == 'cv') {
      if (isset($form_state['values']['chado_type_cv_id']) AND ($form_state['values']['chado_type_cv_id'] > 0)) {
        $form_state['chado-stage'] = 'complete';
      }
      else {
        if (isset($form['chado_type_cv_id'])) {
          form_set_error('chado_type_cv_id', 'Please select a controlled vocabulary which contains records for this content type.');
        }
      }
    }

    // If the user chose to diffentiate type based on the current term,
    // then we're done!
    if ($form_state['values']['chado_type_use_cv'] == 'parent') {
      $form_state['chado-stage'] = 'complete';
    }
  }

  //
  // Check for a reset.
  //
  if ($clicked_button == 'chado-settings-reset') {
    $form_state['chado-stage'] = 'table-select';
    $valid = FALSE;
  }

}

/**
 * Implements hook_field_stoage_bundle_mapping_form_submit().
 *
 * Here we do no action on submit other than set the storage_args
 * variable that is passed by reference.
 */
function tripal_chado_field_storage_bundle_mapping_form_submit($form,
                                                               &$form_state, $term, &$storage_args) {

  $default = tripal_chado_field_storage_bundle_mapping_form_get_defaults($form, $form_state);

  // If we have a type_column then we know this type uses a column in the
  // base table, so we can set the storage args and return.
  if ($default['type_column'] and $default['type_column'] != 'none') {
    $storage_args['data_table'] = $default['table'];
    $storage_args['type_column'] = $default['type_column'];
    return;
  }

  // If the user indicated that all the records in a table are of this type.
  if ($default['has_all'] == 'Yes') {
    $storage_args['data_table'] = $default['table'];
    $storage_args['type_id'] = $form_state['values']['selected_cvterm_id'];
    $storage_args['type_linker_table'] = '';
    $storage_args['type_column'] = '';
    $storage_args['type_value'] = '';
  }

  // If the user indicated they wanted to use the prop table then we'll
  // set the storage args and return.
  else {
    if ($default['use_prop'] == 'Yes') {
      $storage_args['data_table'] = $default['table'];
      $storage_args['type_linker_table'] = $default['table'] . 'prop';
      $storage_args['type_column'] = 'type_id';
      $storage_args['type_id'] = $form_state['values']['prop_term'][0]->cvterm_id;
      $storage_args['type_value'] = $default['prop_term_value'];
      if (is_array($form_state['values']['base_term'])) {
        $storage_args['base_type_id'] = $form_state['values']['base_term'][0]->cvterm_id;
      }
    }

    // If the user indicated they wanted to use the linker tables then we'll
    // set the storage args and return.
    else {
      if ($default['use_linker'] == 'Yes') {
        $storage_args['data_table'] = $default['table'];
        $storage_args['type_id'] = $form_state['values']['selected_cvterm_id'];
        $storage_args['type_linker_table'] = $default['table'] . '_cvterm';
        $storage_args['type_column'] = 'cvterm_id';
        if (is_array($form_state['values']['base_term'])) {
          $storage_args['base_type_id'] = $form_state['values']['base_term'][0]->cvterm_id;
        }
      }

      // If the user indicated they want to use a cv_id to control which cvterm
      // records become pages then we'll set the stroage args and return.
      else {
        if (($default['use_cvterm'] == 'cv') AND ($default['cv_id'])) {
          $storage_args['data_table'] = 'cvterm';
          $storage_args['type_id'] = $form_state['values']['selected_cvterm_id'];
          $storage_args['type_value'] = $default['cv_id'];
        }

        //  If the user indicated they want to use a cvterm_id to control which cvterm
        // records become pages then we'll set the stroage args and return.
        elseif ($default['use_cvterm'] == 'cvterm') {
          // @todo @lacey implement this case
          drupal_set_message('This case currently is not supported.', 'warning');
        }
      }
    }
  }

}
