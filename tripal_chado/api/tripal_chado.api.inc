<?php

/**
 * @file
 *
 * This file contains miscellaneous API functions specific to working with
 * records in Chado that do not have a home in any other sub category of 
 * API functions.
 */

/**
 * @defgroup tripal_chado_api Chado
 *
 * @ingroup tripal_api
 * The Tripal Chado API is a set of functions for interacting with data 
 * inside of a Chado relational database. Entities (or pages) in Drupal
 * that are provided by Tripal can supply data from any supported database 
 * back-end, and Chado is the default. This API contains a variety of sub
 * categories (or groups) where functions are organized.  Any extension module
 * that desires to work with data in Chado will find these functions useful.
 */

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
 *
 * @ingroup tripal_chado_api
 */
function chado_publish_records($values, $job_id = NULL) {

  // We want the job object in order to report progress.
  if (is_object($job_id)) {
    $job = $job_id;
  }
  if (is_numeric($job_id)) {
    $job = new TripalJob();
    $job->load($job_id);
  }
  $report_progress = FALSE;
  if (is_object($job)) {
    $report_progress = TRUE;
  }

  // Make sure we have the required options: bundle_name.
  if (!array_key_exists('bundle_name', $values) or !$values['bundle_name']) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "Could not publish record: @error",
      array('@error' => 'The bundle name must be provided'));
    return FALSE;
  }

  // Get the incoming arguments from the $values array.
  $bundle_name = $values['bundle_name'];
  $filters = array_key_exists('filters', $values) ? $values['filters'] : array();
  $sync_node = array_key_exists('sync_node', $values) ? $values['sync_node'] : '';

  // Load the bundle entity so we can get information about which Chado
  // table/field this entity belongs to.
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
  if (!$bundle) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
        "Unknown bundle. Could not publish record: @error",
        array('@error' => 'The bundle name must be provided'));
    return FALSE;
  }
  $chado_entity_table = chado_get_bundle_entity_table($bundle);


  // Get the mapping of the bio data type to the Chado table.
  $chado_bundle = db_select('chado_bundle', 'cb')
    ->fields('cb')
    ->condition('bundle_id', $bundle->id)
    ->execute()
    ->fetchObject();
  if(!$chado_bundle) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
        "Cannot find mapping of bundle to Chado tables. Could not publish record.");
    return FALSE;
  }

  $table = $chado_bundle->data_table;
  $type_column = $chado_bundle->type_column;
  $type_linker_table = $chado_bundle->type_linker_table;
  $cvterm_id  = $chado_bundle->type_id;
  $type_value = $chado_bundle->type_value;

  // Get the table information for the Chado table.
  $table_schema = chado_get_schema($table);
  $pkey_field = $table_schema['primary key'][0];

  // Construct the SQL for identifying which records should be published.
  $args = array();
  $select = "SELECT T.$pkey_field as record_id ";
  $from = "
    FROM {" . $table . "} T
      LEFT JOIN [" . $chado_entity_table . "] CE on CE.record_id = T.$pkey_field
  ";

  // For migration of Tripal v2 nodes to entities we want to include the
  // coresponding chado linker table.
  if ($sync_node && db_table_exists('chado_' . $table)) {
    $select = "SELECT T.$pkey_field as record_id, CT.nid ";
    $from .= " INNER JOIN [chado_" . $table . "] CT ON CT.$pkey_field = T.$pkey_field";
  }
  $where = " WHERE CE.record_id IS NULL ";

  // Handle records that are mapped to property tables.
  if ($type_linker_table and $type_column and $type_value) {
    $propschema = chado_get_schema($type_linker_table);
    $fkeys = $propschema['foreign keys'][$table]['columns'];
    foreach ($fkeys as $leftkey => $rightkey) {
      if ($rightkey == $pkey_field) {
        $from .= " INNER JOIN {" . $type_linker_table . "} LT ON T.$pkey_field = LT.$leftkey ";
      }
    }
    $where .= "AND LT.$type_column = :cvterm_id and LT.value = :prop_value";
    $args[':cvterm_id'] = $cvterm_id;
    $args[':prop_value'] = $type_value;
  }

  // Handle records that are mapped to cvterm linking tables.
  if ($type_linker_table and $type_column and !$type_value) {
    $cvtschema = chado_get_schema($type_linker_table);
    $fkeys = $cvtschema['foreign keys'][$table]['columns'];
    foreach ($fkeys as $leftkey => $rightkey) {
      if ($rightkey == $pkey_field) {
        $from .= " INNER JOIN {" . $type_linker_table . "} LT ON T.$pkey_field = LT.$leftkey ";
      }
    }
    $where .= "AND LT.$type_column = :cvterm_id";
    $args[':cvterm_id'] = $cvterm_id;
  }

  // Handle records that are mapped via a type_id column in the base table.
  if (!$type_linker_table and $type_column) {
    $where .= "AND T.$type_column = :cvterm_id";
    $args[':cvterm_id'] = $cvterm_id;
  }

  // Handle the case where records are in the cvterm table and mapped via a single
  // vocab.  Here we use the type_value for the cv_id.
  if ($table == 'cvterm' and $type_value) {
    $where .= "AND T.cv_id = :cv_id";
    $args[':cv_id'] = $type_value;
  }

  // Handle the case where records are in the cvterm table but we want to
  // use all of the child terms.
  if ($table == 'cvterm' and !$type_value) {
    $where .= "AND T.cvterm_id IN (
       SELECT CVTP.subject_id
       FROM {cvtermpath} CVTP
       WHERE CVTP.object_id = :cvterm_id)
     ";
    $args[':cvterm_id'] = $cvterm_id;
  }

  // Now add in any additional filters
  $fields = field_info_field_map();
  foreach ($fields as $field_name => $details) {
    if (array_key_exists('TripalEntity', $details['bundles']) and
        in_array($bundle_name, $details['bundles']['TripalEntity']) and
        in_array($field_name, array_keys($filters))){
      $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
      $chado_table = $instance['settings']['chado_table'];
      $chado_column = $instance['settings']['chado_column'];
      if ($chado_table == $table) {
        $where .= " AND T.$chado_column = :$field_name";
        $args[":$field_name"] = $filters[$field_name];
      }
    }
  }
  // First get the count
  $sql = "SELECT count(*) as num_records " . $from . $where;
  $result = chado_query($sql, $args);
  $count = $result->fetchField();

  // calculate the interval for updates
  $interval = intval($count / 50);
  if ($interval < 1) {
    $interval = 1;
  }

  // Perform the query.
  $sql = $select . $from . $where;
  $records = chado_query($sql, $args);
  $transaction = db_transaction();

  print "\nNOTE: publishing records is performed using a database transaction. \n" .
      "If the load fails or is terminated prematurely then the entire set of \n" .
      "is rolled back with no changes to the database\n\n";

  $i = 0;
  printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, 0, number_format(memory_get_usage()));
  try {
    while($record = $records->fetchObject()) {

      // update the job status every interval
      if ($i % $interval == 0) {
        $complete = ($i / $count) * 33.33333333;
        // Currently don't support setting job progress within a transaction.
        // if ($report_progress) { $job->setProgress(intval($complete * 3)); }
        printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 3, number_format(memory_get_usage()));
      }

      // First save the tripal_entity record.
      $record_id = $record->record_id;
      $ec = entity_get_controller('TripalEntity');
      $entity = $ec->create(array(
        'bundle' => $bundle_name,
        'term_id' => $bundle->term_id,
        // Add in the Chado details for when the hook_entity_create()
        // is called and our tripal_chado_entity_create() implementation
        // can deal with it.
        'chado_record' => chado_generate_var($table, array($pkey_field => $record_id)),
        'chado_record_id' => $record_id,
        'publish' => TRUE,
      ));
      $entity = $entity->save();
      if (!$entity) {
        throw new Exception('Could not create entity.');
      }

      // Next save the chado entity record.
      $entity_record = array(
        'entity_id' => $entity->id,
        'record_id' => $record_id,
      );

      // For the Tv2 to Tv3 migration we want to add the nid to the
      // entity so we can associate the node with the entity.
      if (property_exists($record, 'nid')) {
        $entity_record['nid'] = $record->nid;
      }
      $result = db_insert($chado_entity_table)
        ->fields($entity_record)
        ->execute();
      if(!$result){
        throw new Exception('Could not create mapping of entity to Chado record.');
      }

      $i++;
    }
  }
  catch (Exception $e) {
    $transaction->rollback();
    $error = $e->getMessage();
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not publish record: @error", array('@error' => $error));
    drupal_set_message('Failed publishing record. See recent logs for more details.', 'error');
    return FALSE;
  }
  drupal_set_message("Succesfully published $i " . $bundle->label . " record(s).");
  return TRUE;
}

/**
 * Returns an array of tokens based on Tripal Entity Fields.
 *
 * @param $base_table
 *    The name of a base table in Chado.
 * @return
 *    An array of tokens where the key is the machine_name of the token.
 * 
 * @ingroup tripal_chado_api
 */
function chado_get_tokens($base_table) {

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

        $sub_tokens = chado_get_tokens($table);
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
 * 
 *  @ingroup tripal_chado_api
 */
function chado_replace_tokens($string, $record) {
  // Get the list of tokens
  $tokens = chado_get_tokens($record->tablename);

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
