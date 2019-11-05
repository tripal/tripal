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
 * @param $job
 *   The jobs management object for the job if this function is run as a job.
 *   This argument is added by Tripal during a job run and is not needed if
 *   this function is run directly.
 *
 * @return boolean
 *   TRUE if all of the records of the given bundle type were published, and
 *   FALSE if a failure occured.
 *
 * @ingroup tripal_chado_api
 */
function chado_publish_records($values, $job = NULL) {

  // Used for adding runtime to the progress report.
  $started_at = microtime(TRUE);

  // We want the job object in order to report progress with the job.
  if (is_numeric($job)) {
    $job_id = $job;
    $job = new TripalJob();
    $job->load($job_id);
  }
  $report_progress = TRUE;
  if (!is_object($job)) {
    $report_progress = FALSE;
  }

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $message_type = 'publish_records';
  $message_opts = [
    'watchdog' == FALSE,
    'job' => $job,
    'print' => TRUE,
  ];

  // Start an array for caching objects to save performance.
  $cache = [];

  // Make sure we have the required options: bundle_name.
  if (!array_key_exists('bundle_name', $values) or !$values['bundle_name']) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "Could not publish record: @error",
      ['@error' => 'The bundle name must be provided'], $message_opts);
    return FALSE;
  }

  // Get the incoming arguments from the $values array.
  $bundle_name = $values['bundle_name'];
  $filters = array_key_exists('filters', $values) ? $values['filters'] : [];
  $sync_node = array_key_exists('sync_node', $values) ? $values['sync_node'] : '';

  // We want to break the number of records to publish into chunks in order to ensure
  // transactions do not run for too long (performance issue). The number of records
  // to be processed per chunk is set here:
  $chunk_size = 500;

  // Load the bundle entity so we can get information about which Chado
  // table/field this entity belongs to.
  $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
  $cache['bundle'] = $bundle;
  if (!$bundle) {
    tripal_report_error($message_type, TRIPAL_ERROR,
      "Unknown bundle. Could not publish record: @error",
      ['@error' => 'The bundle name must be provided'], $message_opts);
    return FALSE;
  }
  $chado_entity_table = chado_get_bundle_entity_table($bundle);

  // Get the mapping of the bio data type to the Chado table.
  $chado_bundle = db_select('chado_bundle', 'cb')
    ->fields('cb')
    ->condition('bundle_id', $bundle->id)
    ->execute()
    ->fetchObject();
  if (!$chado_bundle) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "Cannot find mapping of bundle to Chado tables. Could not publish record.");
    return FALSE;
  }

  // Load the term for use in setting the alias for each entity created.
  $term = entity_load('TripalTerm', ['id' => $bundle->term_id]);
  $cache['term'] = $term;

  $table = $chado_bundle->data_table;
  $type_column = $chado_bundle->type_column;
  $type_linker_table = $chado_bundle->type_linker_table;
  $cvterm_id = $chado_bundle->type_id;
  $type_value = $chado_bundle->type_value;

  // Get the table information for the Chado table.
  $table_schema = chado_get_schema($table);
  $pkey_field = $table_schema['primary key'][0];

  // Construct the SQL for identifying which records should be published.
  $args = [];
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
      in_array($field_name, array_keys($filters))) {
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
  // @performance optimize, estimate or remove this. It's only used for reporting progress on the command-line.
  $sql = "SELECT count(*) as num_records " . $from . $where;
  $result = chado_query($sql, $args);
  $count = $result->fetchField();

  tripal_report_error($message_type, TRIPAL_INFO,
    "There are !count records to publish.",
    ['!count' => $count], $message_opts);

  if ($report_progress) {
    $job->setTotalItems($count);
    $job->setItemsHandled(0);
    $job->setInterval(1);
  }

  // Perform the query in chunks.
  $sql = $select . $from . $where . ' LIMIT ' . $chunk_size;
  $more_records_to_publish = TRUE;
  $num_actually_published = 0; // the full number of records published.
  $i = 0; //reset on each chunk.
  while ($more_records_to_publish) {

    $records = chado_query($sql, $args);

    // There is no need to cache transactions since Drupal handles nested
    // transactions "by performing no transactional operations (as far as the
    // database sees) within the inner nesting layers". Effectively, Drupal
    // ensures nested trasactions work the same as passing a transaction
    // through to the deepest level and not starting a new transaction if we
    // are already in one.
    $transaction = db_transaction();
    try {
      // Keep track of how many we've published so far
      // before clearing our chunk,
      $num_actually_published = $num_actually_published + $i;
      $i = 0;
      while ($record = $records->fetchObject()) {

        // First save the tripal_entity record.
        // @performace This is likely a bottleneck. Too bad we can't create
        // multiple entities at once... sort of like the copy method.
        $record_id = $record->record_id;
        $ec = entity_get_controller('TripalEntity');

        $entity = $ec->create([
          'bundle' => $bundle_name,
          'term_id' => $bundle->term_id,
          // Add in the Chado details for when the hook_entity_create()
          // is called and our tripal_chado_entity_create() implementation
          // can deal with it.
          'chado_record' => chado_generate_var($table, [$pkey_field => $record_id], ['include_fk' => 0]),
          'chado_record_id' => $record_id,
          'publish' => TRUE,
          'bundle_object' => $bundle,
        ]);

        $entity = $entity->save($cache);
        if (!$entity) {
          throw new Exception('Could not create entity.');
        }

        // Next save the chado entity record.
        $entity_record = [
          'entity_id' => $entity->id,
          'record_id' => $record_id,
        ];

        // For the Tv2 to Tv3 migration we want to add the nid to the
        // entity so we can associate the node with the entity.
        if (property_exists($record, 'nid')) {
          $entity_record['nid'] = $record->nid;
        }
        $result = db_insert($chado_entity_table)
          ->fields($entity_record)
          ->execute();
        if (!$result) {
          throw new Exception('Could not create mapping of entity to Chado record.');
        }

        $i++;
        if ($report_progress) {
          $job->setItemsHandled($i);
        }
      }
    } catch (Exception $e) {
      $transaction->rollback();
      $error = $e->getMessage();
      tripal_report_error($message_type, TRIPAL_ERROR, "Could not publish record: @error", ['@error' => $error]);
      drupal_set_message('Failed publishing record. See recent logs for more details.', 'error');
      return FALSE;
    }

    // If we get through the loop and haven't completed 100 records, then
    // we're done!
    if ($i < $chunk_size) {
      $more_records_to_publish = FALSE;
    }

    // Commit our current chunk.
    unset($transaction);
  }

  // Add the last number published to our total.
  $num_actually_published = $num_actually_published + $i;

  tripal_report_error($message_type, TRIPAL_INFO,
    "Successfully published !count !type record(s).",
    ['!count' => $num_actually_published, '!type' => $bundle->label], $message_opts);

  return TRUE;
}

/**
 * Returns an array of tokens based on Tripal Entity Fields.
 *
 * @param $base_table
 *    The name of a base table in Chado.
 *
 * @return
 *    An array of tokens where the key is the machine_name of the token.
 *
 * @ingroup tripal_chado_api
 */
function chado_get_tokens($base_table) {

  $tokens = [];
  $table_descrip = chado_get_schema($base_table);
  foreach ($table_descrip['fields'] as $field_name => $field_details) {

    $token = '[' . $base_table . '.' . $field_name . ']';
    $location = implode(' > ', [$base_table, $field_name]);

    $tokens[$token] = [
      'name' => ucwords(str_replace('_', ' ', $base_table)) . ': ' . ucwords(str_replace('_', ' ', $field_name)),
      'table' => $base_table,
      'field' => $field_name,
      'token' => $token,
      'description' => array_key_exists('description', $field_details) ? $field_details['description'] : '',
      'location' => $location,
    ];

    if (!array_key_exists('description', $field_details) or preg_match('/TODO/', $field_details['description'])) {
      $tokens[$token]['description'] = 'The ' . $field_name . ' field of the ' . $base_table . ' table.';
    }
  }

  // RECURSION:
  // Follow the foreign key relationships recursively
  if (array_key_exists('foreign keys', $table_descrip)) {
    foreach ($table_descrip['foreign keys'] as $table => $details) {
      foreach ($details['columns'] as $left_field => $right_field) {

        $sub_token_prefix = $base_table . '.' . $left_field;
        $sub_location_prefix = implode(' > ', [$base_table, $left_field]);

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
 * @ingroup tripal_chado_api
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
              [
                '%token' => $token,
                '%index' => $index,
                '%var' => print_r($var, TRUE),
              ]
            );
          }
        }
        $string = str_replace($token, $value, $string);
      }
    }
  }
  return $string;
}
