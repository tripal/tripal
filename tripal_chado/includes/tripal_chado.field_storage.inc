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
  $dbxref = chado_get_dbxref(array('accession' => $term->accession, 'db_id' => array('name' => $term->vocab->vocabulary)));
  $cvterm = chado_get_cvterm(array('dbxref_id' => $dbxref->dbxref_id));

  // Get the base table, type field and record_id from the entity.
  $base_table = $entity->chado_table;
  $type_field = $entity->chado_column;
  $record     = $entity->chado_record;
  $record_id  = $entity->chado_record_id;
  $linker     = property_exists($entity, 'chado_linker') ? $entity->chado_linker : '';
  $base_schema = chado_get_schema($base_table);
  $base_pkey = $base_schema['primary key'][0];

  // Convert the fields into a key/value list of fields and their values.
  list($field_vals, $field_items) = tripal_chado_field_storage_write_merge_fields($fields, $entity_type, $entity);
  // dpm($field_vals);  
  
  // First, write the record for the base table.
  $values = $field_vals[$base_table];
  if ($record_id) {
    $values[$base_pkey] = $record_id;
  }
  elseif ($type_field and !$linker) {
    $values[$type_field] = $cvterm->cvterm_id;
  }
  $base_record_id = tripal_chado_field_storage_write_table($base_table, $values, $base_table);
  if (!$base_record_id) {
    throw new Exception('Unable to write fields to Chado: ' . print_r($field_items, TRUE));
  }

  // If this is an insert then add the chado_entity record.
  if ($op == FIELD_STORAGE_INSERT) {
    // Add the record to the proper chado entity table
    $chado_entity_table = chado_get_bundle_entity_table($bundle);
    $record_id = db_insert($chado_entity_table)
      ->fields(array(
        'entity_id' => $entity->id,
        'record_id' => $base_record_id
      ))
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

    // Before inserting, we want to make sure the record does not
    // already exist.  Using the unique constraint check for a matching record.
    $options = array('is_duplicate' => TRUE);
    $is_duplicate = chado_select_record($table_name, array('*'), $values, $options);
    if($is_duplicate) {
      $record = chado_select_record($table_name, array('*'), $values);
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
      $record_id  = $entity->chado_record_id;
    }
    else {
      $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
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
        $record_id  = isset($details->record_id) ? $details->record_id : '';
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
      $columns = array('*');
      $match = array($pkey_field => $record_id);
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
    $record_id  = $entity->chado_record_id;
    $record = $entity->chado_record;
    $schema = chado_get_schema($base_table);

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
  $field_items = array();

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

    // Iterate through the field's items. Fields with cardinality ($delta) > 1
    // are multi-valued.
    $items = field_get_items($entity_type, $entity, $field_name);
    $temp = array();
    $field_items[$field_name] = $items;
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
  return [$all_fields, $field_items];
}

/**
 * Implements hook_field_storage_query().
 */
function tripal_chado_field_storage_query($query) {

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

          $matches = array();
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
          $cquery->condition('base.' . $base_field , $key, $op);
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
  $cquery->fields('CE', array('entity_id'));
  $cquery->fields('TE', array('bundle'));
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
  if (isset($query->deleted)) {
    // There won't ever be field data marked as deleted so just created a
    // condition that always evaluates to false.
    $cquery->where('1=0');
  }

 //dpm($cquery->__toString());
 //dpm($cquery->getArguments());

  $records = $cquery->execute();

  // If the query is a count query then just return the  total count.
  if ($query->count) {
    return $records->rowCount();
  }

  // If this is not a count query then return the results.
  $result = array();
  while ($record = $records->fetchObject()) {
    $ids = array($record->entity_id, 0, $record->bundle);
    $result['TripalEntity'][$record->entity_id] = entity_create_stub_entity('TripalEntity', $ids);
  }
  return $result;
}


/**
 * Implements hook_field_storage_bundle_mapping_form().
 *
 * This is a Tripal added hook to the field storage API.
 */
function tripal_chado_field_storage_bundle_mapping_form($form, &$form_state,
    $term, &$submit_disabled) {

  $selected_term_id = (is_object($term)) ? $term->cvterm_id : NULL;

  // Initialize the form.
  $form = array();

  // Get the form default values.
  $default = array(
    'table' => '',
    'has_all' => 'No',
    'use_cvterm' => 'cv',
    'cv_id' => '',
    'use_linker' => 'Yes',
    'use_prop' => 'Yes',
    'type_column' => '',
    'prop_term_name' => '',
    'prop_term_value' => '',
  );
  if (array_key_exists('base_chado_table', $form_state['values'])
      and $form_state['values']['base_chado_table']) {
    $default['table'] = $form_state['values']['base_chado_table'];
  }
  else {
    $mapped_table = chado_get_cvterm_mapping(array('cvterm_id' => $selected_term_id));
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
  if (array_key_exists('type_column_ignore', $form_state['values'])
    and $form_state['values']['type_column_ignore']) {
      $default['type_column_ignore'] = $form_state['values']['type_column_ignore'];
  } 
  if (array_key_exists('prop_term_name', $form_state['values'])
      and $form_state['values']['prop_term_name']) {
    $default['prop_term_name'] = $form_state['values']['prop_term_name'];
  }
  if (array_key_exists('prop_term_value', $form_state['values'])
      and $form_state['values']['prop_term_value']) {
    $default['prop_term_value'] = $form_state['values']['prop_term_value'];
  }
  if (array_key_exists('chado_type_use_cv', $form_state['values'])
      and $form_state['values']['chado_type_use_cv']) {
    $default['use_cvterm'] = $form_state['values']['chado_type_use_cv'];
  }
  if (array_key_exists('chado_type_cv_id', $form_state['values'])
      and $form_state['values']['chado_type_cv_id']) {
    $default['cv_id'] = $form_state['values']['chado_type_cv_id'];
  }

  $form['selected_cvterm_id'] = array(
    '#type' => 'value',
    '#value' => $selected_term_id,
  );

  $base_tables = chado_get_base_tables();
  $options = array(0 => '-- Select table --');
  foreach ($base_tables AS $tablename) {
    $options[$tablename] = $tablename;
  }
  $form['base_chado_table'] = array(
    '#type' => 'select',
    '#title' => 'Chado table',
    '#options' => $options,
    '#description' => 'Select the Chado table into which the primary records for this content type will be stored.',
    '#default_value' => $default['table'],
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  $form_state['input']['base_chado_table'] = $default['table'];

  // Return now if the user hasn't provided a table.
  if (!$default['table']) {
    return $form;
  }

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
    return $form;
  }

  // Ask the user if all of the records in the default table are of one type.
  tripal_chado_field_storage_bundle_mapping_form_add_allrecs($form,
      $form_state, $term, $default);

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
  if (!empty($default['type_column'])) {
    $submit_disabled = FALSE;
    return $form;
  }
  
  // If the type column is not disabled and the user has not selected
  // a type then return.
  if (!$default['type_column_ignore'] and empty($default['type_column'])) {
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

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_add_type(&$form,
    &$form_state, $term, &$default){

  $term_name = $term->name;

  // Get the list of columns in the default table.
  $schema = chado_get_schema($default['table']);
  $column_options = array('none' => '--None--');
  $cvt_fkeys = (isset($schema['foreign keys']['cvterm'])) ? array_keys($schema['foreign keys']['cvterm']['columns']) : array();
  foreach ($schema['fields'] as $column_name => $column_details) {
    if (in_array($column_name, $cvt_fkeys)) {
      $column_options[$column_name] = $column_name;
    }
  }

  // If this table has no types then just return.
  if (count($column_options) == 1) {
    $default['type_column'] = '';
    return;
  }
  
  $default_column_ignore = !empty($default['type_column_ignore']) ? $default['type_column_ignore'] : 0;
  $default_column = !empty($default['type_column']) ? $default['type_column'] : 'type_id';
   
  $form['type_column'] = array(
    '#type' => 'select',
    '#title' => 'Type Column',
    '#options' => $column_options,
    '#description' => 'Please select the column in the "' .
      $default['table'] . '" table that will identify a record as being of type "' .
      $term_name . '". If you select "--None--" then you will be asked
      if you the type can be identified using a property or linker table.
      Only fields that have a foreign key to the cvterm table will be listed.',
    '#default_value' => $default_column,
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
    '#disabled' => $default_column_ignore,
  );
  $form['type_column_ignore'] = array(
    '#type' => 'checkbox',
    '#title' => 'The type column does not distinguish the type',
    '#default_value' => $default_column_ignore,
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  // Set the default value so we can short circuit the form.
  $default['type_column'] = $default_column;
  if ($default['type_column'] == 'none') {
    $default['type_column'] = '';
  }
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_add_allrecs(&$form,
    &$form_state, $term, $default) {

  $term_name = $term->name;

  // Form elements to determine if all records in base table are of this type.
  $form['chado_table_has_all'] = array(
    '#type' => 'radios',
    '#options' => array(
      'Yes' => 'Yes',
      'No' => 'No'
    ),
    '#title' => 'Are all records in the "' . $default['table'] .
      '" table of type "'. $term_name . '"?',
    '#description' => 'Select "No" if the "' .
      $default['table'] . '" table houses more than just data of type "' .
      $term_name . '".',
    '#default_value' => $default['has_all'],
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_add_linker(&$form,
    &$form_state, $term, $linker_table, $default){

  $form['chado_type_use_linker'] = array(
    '#type' => 'radios',
    '#title' => 'Do you want to use the "' . $linker_table . '" table
      to distinguish between content types?',
    '#options' => array(
      'Yes' => 'Yes',
      'No' => 'No'
    ),
    '#description' => t('Sometimes records can be distringuished
      using a linker table, especially if there is no column
      in the specified Chado table to identify the
      record type. In these cases the linker table can be used.'),
    '#default_value' => $default['use_linker'],
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
}
/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_add_prop(&$form,
    &$form_state, $term, $prop_table, $default){

  $form['chado_type_use_prop'] = array(
    '#type' => 'radios',
    '#title' => 'Do you want to use the "' . $prop_table . '" table
      to distinguish between content types?',
    '#options' => array(
      'Yes' => 'Yes',
      'No' => 'No'
    ),
    '#description' => t('Sometimes records can be distringuished
      using a value in property table, especially if there is no column
      in the specified Chado table to identify the
      record type. In these cases the property table can be used.'),
    '#default_value' => $default['use_prop'],
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  if ($default['use_prop'] == 'Yes') {
    $prop_term_name = $default['prop_term_name'];
    $prop_term_value = $default['prop_term_value'];

    $form['prop_term_name'] = array(
      '#title'       => t('Property Type'),
      '#type'        => 'textfield',
      '#description' => t('The content type "' . $term->name . '" must be
          associated with a property type.  The property type must be the
          name of a term in a controlled vocabulary and the controlled
          vocabulary should already be loaded into Tripal. Please select
          the property type that will be used to identify records of this
          type'),
      '#required'    => TRUE,
      '#default_value' => $prop_term_name,
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
    );
    $form['prop_term_select_button'] = array(
      '#type' => 'submit',
      '#value' => t('Lookup Term'),
      '#name' => 'select_prop_cvterm',
      '#ajax' => array(
        'callback' => "tripal_admin_add_type_form_ajax_callback",
        'wrapper' => "tripal-vocab-select-form",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    // If the term has been provided by the user then we want to search for
    // matching terms in the database and let them select among any matches.
    if ($prop_term_name) {
      $form['prop_terms_list'] = array(
        '#type' => 'fieldset',
        '#title' => t('Matching Terms'),
        '#description' => t('Please select the term the best matches the
          property type.')
      );
      $match = array(
        'name' => $prop_term_name,
      );
      $pterms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
      $pterms = chado_expand_var($pterms, 'field', 'cvterm.definition');
      $num_terms = 0;

      // Let the user select from any matching terms. Sometimes there may be
      // more than one that match.
      foreach ($pterms as $pterm) {
        // Save the user a click by setting the default value as 1 if there's
        // only one matching term.
        $default = FALSE;
        $attrs = array();
        if ($num_terms == 0 and count($pterms) == 1) {
          $default = TRUE;
          $attrs = array('checked' => 'checked');
        }
        $term_element_name = 'prop_term-' . $pterm->cvterm_id;
        $form['prop_terms_list'][$term_element_name] = array(
          '#type' => 'checkbox',
          '#title' =>  $pterm->name,
          '#default_value' => $default,
          '#attributes' => $attrs,
          '#description' => '<b>Vocabulary:</b> ' . $pterm->cv_id->name . ' (' . $pterm->dbxref_id->db_id->name . ') ' . $pterm->cv_id->definition .
            '<br><b>Term: </b> ' . $pterm->dbxref_id->db_id->name . ':' . $pterm->dbxref_id->accession . '.  ' .
            '<br><b>Definition:</b>  ' . $pterm->definition,
          '#ajax' => array(
            'callback' => "tripal_admin_add_type_form_ajax_callback",
            'wrapper' => "tripal-vocab-select-form",
            'effect' => 'fade',
            'method' => 'replace'
          ),
        );
        $num_terms++;
      }

      $form['prop_term_value'] = array(
        '#type' => 'textfield',
        '#title' => 'Property Value',
        '#description' => t('All properties have a type and a value.  Above you
            indicated the type of property. Now you must specify the value
            that this properly must have in order to be considered of
            type "' . $term->name . '".'),
        '#default_value' => $prop_term_value,
        '#required' => TRUE,
      );
    }
  }
}

/**
 *
 */
function tripal_chado_field_storage_bundle_mapping_form_add_cvterm(&$form,
    &$form_state, $term, &$default){

  $form['chado_type_use_cv'] = array(
    '#type' => 'radios',
    '#title' => 'How do you want to distinguish the "' . $term->name . '" records
      within the cvterm table?',
    '#options' => array(
      'cv' => 'All records that belong to a single controlled vocabulary?',
      'parent' => 'All child records of the specified term?'
    ),
    '#description' => t('Records in the cvterm table are often not used for content
        types. The records in the cvterm table are meant to be vocabulary terms.
        However, there are times when records in the cvterm table can be
        used for a content type. One example is for trait "pages" that
        use phenotype values stored in the phenotype table.  Because records in
        the cvterm table are vocabulary terms they do not have "types" so the
        only ways to distinguish them are 1) to use the controlled vocabulary to
        distinguish the records or 2) to use all children records of the
        selected term.  Please select the appropriate value for this case.'),
    '#default_value' => $default['use_cvterm'],
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  if ($default['use_cvterm'] == 'cv') {
    $cvs = chado_get_cv_select_options();
    $form['chado_type_cv_id'] = array(
      '#type' => select,
      '#options' => $cvs,
      '#title' => t('Select a controlled vocabulary'),
      '#default_value' => $default['cv_id'],
      '#ajax' => array(
        'callback' => "tripal_admin_add_type_form_ajax_callback",
        'wrapper' => "tripal-vocab-select-form",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
  }

}
/**
 * Implements hook_field_stoage_bundle_mapping_form_validate().
 */
function tripal_chado_field_storage_bundle_mapping_form_validate($form, &$form_state) {
  // Get the form values
  $default = array(
    'table' => '',
    'has_all' => 'No',
    'use_cvterm' => 'cv',
    'cv_id' => '',
    'use_linker' => 'Yes',
    'use_prop' => 'Yes',
    'type_column' => '',
    'prop_term_name' => '',
    'prop_term_value' => '',
  );
  if (array_key_exists('base_chado_table', $form_state['values'])
      and $form_state['values']['base_chado_table']) {
    $default['table'] = $form_state['values']['base_chado_table'];
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
  if (array_key_exists('prop_term_name', $form_state['values'])
      and $form_state['values']['prop_term_name']) {
    $default['prop_term_name'] = $form_state['values']['prop_term_name'];

    // Make sure we have only one property cvterm selected
    $num_selected = 0;
    foreach ($form_state['values'] as $key => $value) {
      $matches = array();
      if (preg_match("/^prop_term-(\d+)$/", $key, $matches) and
          $form_state['values']['prop_term-' . $matches[1]]) {
        $cvterm_id = $matches[1];
        $term = chado_generate_var('cvterm', array('cvterm_id' => $cvterm_id));
        $num_selected++;
      }
    }
    if ($num_selected == 0) {
      form_set_error('', 'Please select at least one property term.');
    }
    else if ($num_selected > 1) {
      form_set_error('prop_term-' . $cvterm_id, 'Please select only one property term from the list below.');
    }
    $form_state['prop_term'] = $term;
  }
  if (array_key_exists('prop_term_value', $form_state['values'])
      and $form_state['values']['prop_term_value']) {
    $default['prop_term_value'] = $form_state['values']['prop_term_value'];
  }
  if (array_key_exists('chado_type_use_cv', $form_state['values'])
      and $form_state['values']['chado_type_use_cv']) {
    $default['use_cvterm'] = $form_state['values']['chado_type_use_cv'];
  }
  if (array_key_exists('chado_type_cv_id', $form_state['values'])
      and $form_state['values']['chado_type_cv_id']) {
    $default['cv_id'] = $form_state['values']['chado_type_cv_id'];
  }

  // Make sure a default table is selected
  if (!$default['table']) {
    form_set_error('base_chado_table', 'Please select a default table.');
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

  // Get the form values
  $default = array(
    'table' => '',
    'has_all' => 'No',
    'use_cvterm' => '',
    'cv_id' => '',
    'use_linker' => 'No',
    'use_prop' => 'No',
    'type_column' => '',
    'prop_term_name' => '',
    'prop_term_value' => '',
  );
  if (array_key_exists('base_chado_table', $form_state['values'])
      and $form_state['values']['base_chado_table']) {
    $default['table'] = $form_state['values']['base_chado_table'];
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
  if (array_key_exists('prop_term_name', $form_state['values'])
      and $form_state['values']['prop_term_name']) {
    $default['prop_term_name'] = $form_state['values']['prop_term_name'];
  }
  if (array_key_exists('prop_term_value', $form_state['values'])
      and $form_state['values']['prop_term_value']) {
    $default['prop_term_value'] = $form_state['values']['prop_term_value'];
  }
  if (array_key_exists('chado_type_use_cv', $form_state['values'])
      and $form_state['values']['chado_type_use_cv']) {
    $default['use_cvterm'] = $form_state['values']['chado_type_use_cv'];
  }
  if (array_key_exists('chado_type_cv_id', $form_state['values'])
      and $form_state['values']['chado_type_cv_id']) {
    $default['cv_id'] = $form_state['values']['chado_type_cv_id'];
  }

  // If the user selected to use the cvterm table then we have to
  // handle it specially.
  if ($default['use_cvterm'] == 'cv' and !empty($default['cv_id'])) {
    $storage_args['data_table'] = $default['table'];
    $storage_args['type_id'] = $form_state['values']['selected_cvterm_id'];
    $storage_args['type_value'] = '';
    if ($default['cv_id']) {
      $storage_args['type_value'] = $default['cv_id'];
    }
    $storage_args['type_column'] = '';
    return;
  }

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
  else if ($default['use_prop'] == 'Yes') {
    $storage_args['data_table'] = $default['table'];
    $storage_args['type_linker_table'] = $default['table'] . 'prop';
    $storage_args['type_column'] = 'type_id';
    $storage_args['type_id'] = $form_state['prop_term']->cvterm_id;
    $storage_args['type_value'] = $default['prop_term_value'];
  }

  // If the user indicated they wanted to use the linker tables then we'll
  // set the storage args and return.
  else if ($default['use_linker'] == 'Yes') {
    $storage_args['data_table'] = $default['table'];
    $storage_args['type_id'] = $form_state['values']['selected_cvterm_id'];
    $storage_args['type_linker_table'] = $default['table'] . '_cvterm';
    $storage_args['type_column'] = 'cvterm_id';
  }
}

