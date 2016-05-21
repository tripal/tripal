<?php

/**
 * Retrieves an entity that matches the given table and record id.
 *
 * @param $table
 *   The name of the Chado table.
 * @param $record_id
 *   The record's primary key in the table specified by $table.
 *
 * @return
 *   A chado_entity object.
 */
function tripal_load_chado_entity($table, $record_id) {
   $entity_id = db_select('chado_entity', 'ce')
     ->fields('ce', array('entity_id'))
     ->condition('ce.record_id', $record_id)
     ->condition('ce.data_table', $table)
     ->execute()
     ->fetchField();
   if ($entity_id) {
     $entity = entity_load('TripalEntity', array($entity_id));
     return reset($entity);
   }
   return NULL;
}

/**
 * Retrieves the entity ID using a record ID.
 *
 * @param unknown $data_table
 * @param unknown $record_id
 */
function tripal_get_chado_entity_id($data_table, $record_id) {
   return db_select('chado_entity', 'ce')
     ->fields('ce', array('entity_id'))
     ->condition('data_table', $data_table)
     ->condition('record_id', $record_id)
     ->execute()
     ->fetchField();
}

/**
 * Publishes content in Chado as a new TripalEntity entity.
 *
 * @param $values
 *   A key/value associative array that supports the following keys:
 *   - bundle_name:  The name of the the TripalBundle (e.g. bio_data-12345).
 * @param $job_id
 *   (Optional) The numeric job ID as provided by the Tripal jobs system. There
 *   is no need to specify this argument if this function is being called
 *   outside of the jobs systems.
 *
 * @return boolean
 *   TRUE if all of the records of the given bundle type were published, and
 *   FALSE if a failure occured.
 */
function tripal_chado_publish_records($values, $job_id = NULL) {
  $bundle_name = $values['bundle_name'];
  $sync_node = array_key_exists('sync_node', $values) ? $values['sync_node'] : '';

  if (!array_key_exists('bundle_name', $values) or !$values['bundle_name']) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not publish record: @error", array('@error' => 'The bundle name was not provided'));
    return FALSE;
  }

  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
  $bundle_id = $bundle->id;
  $table = tripal_get_bundle_variable('chado_table', $bundle_id);
  $column = tripal_get_bundle_variable('chado_column', $bundle_id);
  $cvterm_id = tripal_get_bundle_variable('chado_cvterm_id', $bundle_id);

  // Get the table information
  $table_schema = chado_get_schema($table);
  $pkey_field = $table_schema['primary key'][0];

  $select = "SELECT $pkey_field as record_id ";
  $from = "FROM {" . $table . "} T
      LEFT JOIN public.chado_entity CE on CE.record_id = T.$pkey_field
    AND CE.data_table = '$table'
  ";

  if ($sync_node && db_table_exists('chado_' . $table)) {
    $select = "SELECT T.$pkey_field as record_id, CT.nid ";
    $from .= "INNER JOIN public.chado_$table CT ON CT.$pkey_field = T.$pkey_field";
  }
  $where = " WHERE CE.record_id IS NULL ";
  if ($table != 'analysis' and $table != 'organism') {
    $where .= "AND $column = $cvterm_id";
  }

  $sql = $select . $from . $where;
  $records = chado_query($sql);
  $num_published = 0;
  $transaction  = db_transaction();
  try {
    while($record = $records->fetchObject()) {

      // First save the tripal_entity record.
      $record_id = $record->record_id;
      $ec = entity_get_controller('TripalEntity');
      $entity = $ec->create(array(
        'bundle' => $bundle_name,
        'term_id' => $bundle->term_id,
      ));
      if (!$entity->save()) {
        throw new Exception('Could not create entity.');
      }

      // Next save the chado_entity record.
      $entity_record = array(
        'entity_id' => $entity->id,
        'record_id' => $record_id,
        'data_table' => $table,
        'type_table' => $table,
        'field' => $column,
      );

      // For the Tv2 to Tv3 migration we want to add the nid to the
      // entity so we can associate the node with the entity.
      if (property_exists($record, 'nid')) {
        $entity_record['nid'] = $record->nid;
      }
      $success = drupal_write_record('chado_entity', $entity_record);

      $entity = entity_load('TripalEntity', array($entity->id));
      $entity = reset($entity);
      $title_format = tripal_get_title_format($bundle);
      $title = tripal_replace_entity_tokens($title_format, $entity, $bundle);
      $ec->setTitle($entity, $title);
      $num_published++;
    }
  }
  catch (Exception $e) {
    $transaction->rollback();
    $error = $e->getMessage();
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not publish record: @error", array('@error' => $error));
    drupal_set_message('Failed publishing record. See recent logs for more details.', 'error');
    return FALSE;
  }
  drupal_set_message("Succesfully published $num_published " . $bundle->label . " record(s).");
  return TRUE;
}

/**
 * Returns an array of tokens based on Tripal Entity Fields.
 *
 * @param $base_table
 *    The name of a base table in Chado.
 * @return
 *    An array of tokens where the key is the machine_name of the token.
 */
function tripal_get_chado_tokens($base_table) {

  $tokens = array();
  $table_descrip = chado_get_schema($base_table);
  foreach ($table_descrip['fields'] as $field_name => $field_details) {

    $token = '[' . $base_table . '.' . $field_name . ']';
    $location = implode(' > ',array($base_table, $field_name));

    $tokens[$token] = array(
      'name' => ucwords(str_replace('_',' ',$base_table)) . ': ' . ucwords(str_replace('_',' ',$field_name)),
      'table' => $base_table,
      'field' => $field_name,
      'token' => $token,
      'description' => array_key_exists('description', $field_details) ? $field_details['description'] : '',
      'location' => $location
    );

    if (!array_key_exists('description', $field_details) or preg_match('/TODO/',$field_details['description'])) {
      $tokens[$token]['description'] = 'The '.$field_name.' field of the '.$base_table.' table.';
    }
  }

  // RECURSION:
  // Follow the foreign key relationships recursively
  if (array_key_exists('foreign keys', $table_descrip)) {
    foreach ($table_descrip['foreign keys'] as $table => $details) {
      foreach ($details['columns'] as $left_field => $right_field) {

        $sub_token_prefix = $base_table . '.' . $left_field;
        $sub_location_prefix = implode(' > ',array($base_table, $left_field));

        $sub_tokens = tripal_get_chado_tokens($table);
        if (is_array($sub_tokens)) {
          $tokens = array_merge($tokens, $sub_tokens);
        }
      }
    }
  }

  return $tokens;
}

/**
 * Replace all Chado Tokens in a given string.
 *
 * NOTE: If there is no value for a token then the token is removed.
 *
 * @param string $string
 *   The string containing tokens.
 * @param $record
 *   A Chado record as generated by chado_generate_var()
 *
 * @return
 *   The string will all tokens replaced with values.
 */
function tripal_replace_chado_tokens($string, $record) {
  // Get the list of tokens
  $tokens = tripal_get_chado_tokens($record->tablename);

  // Determine which tokens were used in the format string
  if (preg_match_all('/\[[^]]+\]/', $string, $used_tokens)) {
    // Get the value for each token used
    foreach ($used_tokens[0] as $token) {
      $token_info = $tokens[$token];
      if (!empty($token_info)) {
        $table = $token_info['table'];
        $var = $record;
        $value = '';

        // Iterate through each portion of the location string. An example string
        // might be:  stock > type_id > name.
        $location = explode('>', $token_info['location']);
        foreach ($location as $index) {
          $index = trim($index);

          // if $var is an object then it is the $node object or a table
          // that has been expanded.
          if (is_object($var)) {
            // check to see if the index is a member of the object. If so,
            // then reset the $var to this value.
            if (property_exists($var, $index)) {
              $value = $var->$index;
            }
          }
          // if the $var is an array then there are multiple instances of the same
          // table in a FK relationship (e.g. relationship tables)
          elseif (is_array($var)) {
            $value = $var[$index];
          }
          else {
            tripal_report_error('tripal_chado', TRIPAL_WARNING,
              'Tokens: Unable to determine the value of %token. Things went awry when trying ' .
              'to access \'%index\' for the following: \'%var\'.',
              array('%token' => $token, '%index' => $index, '%var' => print_r($var,TRUE))
            );
          }
        }
        $string = str_replace($token, $value, $string);
      }
    }
  }
  return $string;
}

/**
 *
 */
function tripal_associate_chado_semweb_term($chado_table, $chado_column, $term) {

  // Check to see if the record has already been added to the chado_semweb table
  $query = db_select('chado_semweb', 'CS');
  $query->fields('CS', array('chado_semweb_id'));
  $query->condition('chado_column', $chado_column);
  $query->condition('cvterm_id', $term->cvterm_id);
  if ($chado_table) {
    $query->condition('chado_table', $chado_table);
  }
  $id = $query->execute()->fetchField();

  $values = array(
    'chado_table' => $chado_table,
    'chado_column' => $chado_column,
    'cvterm_id' => $term->cvterm_id,
  );
  // If we have an ID then this is an update, otherwise it's an insert.
  if ($id) {
    $values['chado_semweb_id'] = $id;
    $success = drupal_write_record('chado_semweb', $values, 'chado_semweb_id');
  }
  else {
    $success = drupal_write_record('chado_semweb', $values);
  }
  if (!$success) {
    FALSE;
  }

  return TRUE;
}

