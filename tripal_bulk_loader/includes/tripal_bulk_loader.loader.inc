<?php

/**
 * @file
 * Handles the actual loading of data.
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Add Loader Job Form
 *
 * This form is meant to be included on the node page to allow users to submit/re-submit
 * loading jobs
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_add_loader_job_form($form, $form_state, $node) {
  $form = array();

  // --notify--
  if ($node->job_status == 'Loading...') {
    drupal_set_message(t("The Loading Summary only updates at the end of each constant set.
      Although records may have already been inserted, they won't be available until the
      current constant set is full loaded and no errors are encountered.", array()), 'warning');
  }

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );

  $form['file'] = array(
    '#type' => 'hidden',
    '#value' => $node->file
  );

  $form['job_id'] = array(
    '#type' => 'hidden',
    '#value' => $node->job_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => ($node->job_id) ? 'Re-Submit Job' : 'Submit Job',
  );

  $form['submit-cancel'] = array(
    '#type' => ($node->job_id)? 'submit' : 'hidden',
    '#value' => 'Cancel Job',
  );

  if ($node->keep_track_inserted) {
    $form['submit-revert'] = array(
      '#type' => ($node->job_id) ? 'submit' : 'hidden',
      '#value' => 'Revert',
    );
  }

  return $form;
}

/**
 * Add Loader Job Form (Submit)
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_add_loader_job_form_submit($form, $form_state) {
  global $user;

  if (preg_match('/Submit Job/', $form_state['values']['op'])) {
    //Submit Tripal Job
    $job_args[1] = $form_state['values']['nid'];
    if (is_readable($form_state['values']['file'])) {
      $fname = basename($form_state['values']['file']);
      $job_id = tripal_add_job("Bulk Loading Job: $fname", 'tripal_bulk_loader', 'tripal_bulk_loader_load_data', $job_args, $user->uid);

      // add job_id to bulk_loader node
      $success = db_update('tripal_bulk_loader')->fields(array(
        'job_id' => $job_id,
        'job_status' => 'Submitted to Queue'
      ))->condition('nid',$form_state['values']['nid'])->execute();
    }
    else {
      drupal_set_message(t("Can not open %file. Job not scheduled.", array('%file' => $form_state['values']['file'])));
    }
  }
  elseif (preg_match('/Re-Submit Job/', $form_state['values']['op'])) {
    tripal_rerun_job($form_state['values']['job_id']);
    $success = db_update('tripal_bulk_loader')->fields(array(
        'job_status' => 'Submitted to Queue'
      ))->condition('nid',$form_state['values']['nid'])->execute();
  }
  elseif (preg_match('/Cancel Job/', $form_state['values']['op'])) {
    $success = db_update('tripal_bulk_loader')->fields(array(
        'job_status' => 'Job Cancelled'
      ))->condition('nid',$form_state['values']['nid'])->execute();
    tripal_cancel_job($form_state['values']['job_id']);
  }
  elseif (preg_match('/Revert/', $form_state['values']['op'])) {

    // Remove the records from the database that were already inserted
    $resource = db_query('SELECT * FROM {tripal_bulk_loader_inserted} WHERE nid=:nid ORDER BY tripal_bulk_loader_inserted_id DESC', array(':nid' => $form_state['values']['nid']));
    while ($r = $resource->fetchObject()) {
      $ids = preg_split('/,/', $r->ids_inserted);
      db_query('DELETE FROM {'.$r->table_inserted_into.'} WHERE '.$r->table_primary_key.' IN ('.$r->ids_inserted.')')->execute();
      $result = db_query('SELECT true as present FROM {'.$r->table_inserted_into.'} WHERE '.$r->table_primary_key.' IN ('.$r->ids_inserted.')')->fetchObject();
      if (!$result->present) {
        drupal_set_message(t('Successfully Removed data Inserted into the %tableto table.', array('%tableto' => $r->table_inserted_into)));
        db_query('DELETE FROM {tripal_bulk_loader_inserted} WHERE tripal_bulk_loader_inserted_id=:id', array(':id' => $r->tripal_bulk_loader_inserted_id))->execute();
      }
      else {
        drupal_set_message(t('Unable to remove data Inserted into the %tableto table!', array('%tableto' => $r->table_inserted_into)), 'error');
      }
    }

    // reset status
    $success = db_update('tripal_bulk_loader')->fields(array(
        'job_status' => 'Reverted -Data Deleted'
      ))->condition('nid',$form_state['values']['nid'])->execute();
  }

}

/**
 * Tripal Bulk Loader
 *
 * This is the function that's run by tripal_launch_jobs to bulk load chado data.
 *
 * @param $nid
 *   The Node ID of the bulk loading job node to be loaded. All other needed data is expected to be
 *   in the node (ie: template ID and file)
 *
 * Note: Instead of returning a value this function updates the tripal_bulk_loader.status.
 *   Errors are thrown through watchdog and can be viewed at admin/reports/dblog.
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_load_data($nid, $job_id) {

  // ensure no timeout
  drupal_set_time_limit(0);

  // set the status of the job (in the node not the tripal jobs)
  db_update('tripal_bulk_loader')
    ->fields(array('job_status' => 'Loading...'))
    ->condition('nid', $nid)
    ->execute();

  $node = node_load($nid);
  print "Template: " . $node->template->name . " (" . $node->template_id . ")\n";

  $total_lines = trim(`wc --lines < $node->file`);
  print "File: " . $node->file . " (" . $total_lines . " lines)\n";

  //print "\nClearing all prepared statements from previous runs of this loader...\n";
  //tripal_core_chado_clear_prepared('_'.$node->nid.'_');

  // Prep Work ==================================================================================
  print "\nPreparing to load...\n";
  $loaded_without_errors = TRUE;

  // Generate default values array
  $default_data = array();
  $field2column = array();
  $record2priority = array();
  $tables = array();
  $template_array = $node->template->template_array;

  // first build the record2priority array
  foreach ($template_array as $priority => $record_array) {
    $record2priority[$record_array['record_id']] = $priority;
  }

  //
  foreach ($template_array as $priority => $record_array) {
    if (!is_array($record_array)) {
      continue;
    }

    // Add tables being inserted into to a list to be treated differently
    // this is used to acquire locks on these tables
    if (preg_match('/insert/', $record_array['mode'])) {
      $tables[$record_array['table']] = $record_array['table'];
    }

    // iterate through each of the fiels for the current record and
    // set the default_data array
    foreach ($record_array['fields'] as $field_index => $field_array) {

      $default_data[$priority]['table'] = $record_array['table'];
      $default_data[$priority]['mode'] = ($record_array['mode']) ? $record_array['mode'] : 'insert';
      $default_data[$priority]['select_if_duplicate'] = ($record_array['select_if_duplicate']) ? $record_array['select_if_duplicate'] : 0;
      $default_data[$priority]['update_if_duplicate'] = ($record_array['update_if_duplicate']) ? $record_array['update_if_duplicate'] : 0;
      $default_data[$priority]['disabled'] = ($record_array['disable']) ? $record_array['disable'] : 0;
      $default_data[$priority]['optional'] = ($record_array['optional']) ? $record_array['optional'] : 0;
      $default_data[$priority]['select_optional'] = ($record_array['select_optional']) ? $record_array['select_optional'] : 0;
      $default_data[$priority]['record_id'] = $record_array['record_id'];
      $default_data[$priority]['required'][$field_array['field']] = $field_array['required'];

      $one = $default_data[$priority];
      if (isset($field_array['regex'])) {
        $default_data[$priority]['regex_transform'][$field_array['field']] = $field_array['regex'];
      }

      $two = $default_data[$priority];

      if (preg_match('/table field/', $field_array['type'])) {
        $default_data[$priority]['values_array'][$field_array['field']] = '';
        $default_data[$priority]['need_further_processing'] = TRUE;
        $field2column[$priority][$field_array['field']] = $field_array['spreadsheet column'];

      }
      elseif (preg_match('/constant/', $field_array['type'])) {
        $default_data[$priority]['values_array'][$field_array['field']] = $field_array['constant value'];

      }
      elseif (preg_match('/foreign key/', $field_array['type'])) {
        $default_data[$priority]['values_array'][$field_array['field']] = array();
        $default_data[$priority]['need_further_processing'] = TRUE;
        $default_data[$priority]['values_array'][$field_array['field']]['foreign record']['record'] = $field_array['foreign key'];

        // Add in the FK / Referral table
        $fk_priority = $record2priority[$field_array['foreign key']];
        $fk_table = $template_array[$fk_priority]['table'];
        $default_data[$priority]['values_array'][$field_array['field']]['foreign record']['table'] = $fk_table;

        // Add in the FK / Referral field
        // for backwards compatibility we need to get the FK relationship to find
        // out what field we're joining on.  For templates created using a
        // previous version it was assumed that the FK field was always the field to join
        if (!array_key_exists('foreign field', $field_array)) {
          $tbl_description = chado_get_schema($record_array['table']);
          foreach ($tbl_description['foreign keys'] as $key_table => $key_array) {
            if ($key_table == $fk_table) {
              foreach ($key_array['columns'] as $left_field => $right_field) {
                if ($left_field == $field_array['field']) {
                  $field_array['foreign field'] = $right_field;
                }
              }
            }
          }
        }
        $default_data[$priority]['values_array'][$field_array['field']]['foreign record']['field'] = $field_array['foreign field'];
      }
      else {
        print 'WARNING: Unsupported type: ' . $field_array['type'] . ' for ' . $table . '.' . $field_array['field'] . "!\n";
      }
      $three = $default_data[$priority];

    } // end of foreach field
  } //end of foreach record

  ///////////////////////////////////////////////
  // For each set of constants
  ///////////////////////////////////////////////
  print "Loading...\n";
  $original_default_data = $default_data;
  $group_index = 0;
  $total_num_groups = sizeof($node->constants);
  // If there are no constant sets and no exposed fields
  // then create an empty constant set so loader runs
  if ($total_num_groups == 0 && empty($node->exposed_fields)) {
    $node->constants = array(
      0 => array()
    );
    $total_num_groups = 1;
  }
  foreach ($node->constants as $group_id => $set) {
    // revert default data array for next set of constants
    $default_data = $original_default_data;
    $group_index++;

    // Add constants
    if (!empty($set)) {
      print "Constants:\n";
      foreach ($set as $priority => $record) {
        foreach ($record as $field_id => $field) {

          print "\t- " . $field['chado_table'] . '.' . $field['chado_field'] . ' = ' . $field['value'] . "\n";

          if ($default_data[$priority]['table'] == $field['chado_table']) {
            if (isset($default_data[$priority]['values_array'][$field['chado_field']])) {
              if (isset($field2column[$priority][$field['chado_field']])) {
                $field2column[$priority][$field['chado_field']] = $field['value'];
              }
              else {
                $default_data[$priority]['values_array'][$field['chado_field']] = $field['value'];
              }
            }
            else {
              print "ERROR: Template has changed after constants were assigned!\n";
              tripal_bulk_loader_throw_error('Template has changed after constants were assigned', array(), TRIPAL_NOTICE);
              exit(1);
            }
          }
          else {
            print "ERROR: Template has changed after constants were assigned!\n";
            tripal_bulk_loader_throw_error('Template has changed after constants were assigned', array(), TRIPAL_NOTICE);
            exit(1);
          }
        }
      }
    }

    // Open File
    print "\tPreparing to load the current constant set...\n";
    print "\t\tOpen File...\n";
    try {
      $file = new SplFileObject($node->file, 'r');
    }
    catch (Exception $e) {
      tripal_bulk_loader_throw_error('Could not open file %file',
        array('%file' => $node->file), TRIPAL_ERROR);
      return;
    }

    // Set defaults
    $header = '';
    if (preg_match('/(t|true|1)/', $node->file_has_header)) {
      $file->next();
      $header = $file->current();
    }
    $num_records = 0;
    $num_lines = 0;
    $num_errors = 0;
    $interval = intval($total_lines * 0.0001);
    if ($interval == 0) {
      $interval = 1;
    }

    // Start Transaction
    $savepoint = '';
    switch (variable_get('tripal_bulk_loader_transactions', 'row')) {
      case "none":
        break;
      case "all":
        print "\t\tStart Transaction...\n";
        $TRANSACTION = db_transaction();
        $transactions = TRUE;
        break;
      case "row":
        print "\t\tStart Transaction...\n";
        $TRANSACTION = db_transaction();
        $transactions = TRUE;
        $new_transaction_per_row = TRUE;
        break;
    }

    // Disable triggers
    $triggers_disabled = FALSE;
    if ($transactions AND variable_get('tripal_bulk_loader_disable_triggers', TRUE)) {
      print "\t\tDefer Constraints...\n";
      $triggers_disabled = TRUE;
      chado_query("SET CONSTRAINTS ALL DEFERRED");
    }

    // Acquire Locks
    if ($transactions) {
      print "\t\tAcquiring Table Locks...\n";
      $lockmode = variable_get('tripal_bulk_loader_lock', 'ROW EXCLUSIVE');
      foreach ($tables as $table) {
        print "\t\t\t$lockmode for $table\n";
        chado_query("LOCK TABLE ".$table." IN ".$lockmode." MODE");
      }
    }

    print "\tLoading the current constant set...\n";
    tripal_bulk_loader_progress_bar(0, $total_lines);
    while (!$file->eof()) {
      $file->next();
      $raw_line = $file->current();
      $raw_line = trim($raw_line);
      if (empty($raw_line)) {
        continue;
      } // skips blank lines
      $line = explode("\t", $raw_line);
      $num_lines++;

      // update the job status every 1% of lines processed for the current group
      if ($node->job_id and $num_lines % $interval == 0) {

        // percentage of lines processed for the current group
        $group_progress = round(($num_lines / $total_lines) * 100);
        tripal_bulk_loader_progress_bar($num_lines, $total_lines);

        // percentage of lines processed for all groups
        // <previous group index> * 100 + <current group progress>
        // --------------------------------------------------------
        //               <total number of groups>
        // For example, if you were in the third group of 3 constant sets
        // and had a group percentage of 50% then the job progress would be
        // (2*100 + 50%) / 3 = 250%/3 = 83%
        $job_progress = round(((($group_index - 1) * 100) + $group_progress) / $total_num_groups);
        tripal_set_job_progress($node->job_id, $job_progress);
      }

      $data = $default_data;

      // iterate through each record and process the line
      $data_keys = array_keys($data);
      foreach ($data_keys as $priority) {
        $options = array(
          'field2column' => $field2column,
          'record2priority' => $record2priority,
          'line' => $line,
          'line_num' => $num_lines,
          'group_index' => $group_index,
          'node' => $node,
          'nid' => $node->nid,
        );


        // execute all records that are not disabled
        $no_errors = FALSE;
        if (array_key_exists($priority, $data) and
            array_key_exists('disabled', $data[$priority]) and
            $data[$priority]['disabled'] == 0) {
          $no_errors = process_data_array_for_line($priority, $data, $default_data, $options);
        }
        else {
          // set status to true for skipped records
          $no_errors = TRUE;
        }

        tripal_bulk_loader_progress_file_track_job($job_id, $no_errors);
        $failed = FALSE;
        if ($no_errors == FALSE) {
          // Encountered an error
          if ($transactions) {
            $TRANSACTION->rollback();
          }
          tripal_bulk_loader_finish_loading($node->nid, FALSE);
          break;
        }
      } // end of foreach table in default data array

      tripal_bulk_loader_progress_file_track_job($job_id, FALSE, TRUE);

      if ($failed) {
        $TRANSACTION->rollback();
        tripal_bulk_loader_finish_loading($node->nid, FALSE);
        break;
      }
      else {
        // Row inserted successfully
        if ($transactions && $new_transaction_per_row) {
          // commit current transaction and start a new one
          unset($TRANSACTION);
          $TRANSACTION = db_transaction();
        }
      }
    } //end of foreach line of file

    // END Transaction
    if ($transactions) {
      unset($TRANSACTION);
    }

    if ($failed) {
      $loaded_without_errors = FALSE;
      break;
    }
    tripal_bulk_loader_progress_bar($total_lines, $total_lines);
    tripal_bulk_loader_progress_file_track_job($job_id, FALSE, FALSE, TRUE);
  } //end of foreach constant set

  tripal_bulk_loader_finish_loading($node->nid, $loaded_without_errors);

}

/**
 * Process the data array for a given line
 *
 * @param $addt
 *   Requires: field2column', 'record2priority', 'line', 'line_num',
 *   'group_index', 'node', 'nid'
 *
 * @ingroup tripal_bulk_loader
 */
function process_data_array_for_line($priority, &$data, &$default_data, $addt) {
//$time_start = microtime(true);

  $table_data = $data[$priority];
  $addt = (object) $addt;
  $no_errors = TRUE;

  $table = $table_data['table'];
  $values = $table_data['values_array'];

  // populate the values array with real value either from the input data file line
  // or from the foreign key / referral record
  if (array_key_exists('need_further_processing', $table_data) and $table_data['need_further_processing']) {
    if (array_key_exists($priority, $addt->field2column)) {
      $values = tripal_bulk_loader_add_spreadsheetdata_to_values($values, $addt->line, $addt->field2column[$priority]);
    }
    $values = tripal_bulk_loader_add_foreignkey_to_values($table_data, $values, $data, $addt->record2priority, $addt->nid, $priority, $default_data);
  }

  $values = tripal_bulk_loader_regex_tranform_values($values, $table_data, $addt->line);
  if (!$values) {
    //tripal_bulk_loader_throw_error('Line ' . $addt->line_num . ' Regex:<pre>' . print_r($values, TRUE) . print_r($table_data, TRUE) . '</pre>' . '</pre>', array(), TRIPAL_NOTICE);
  }

  // get the table description
  $table_desc = chado_get_schema($table);
  if (!$table_desc) {
    $msg = "Tripal does not know about the table named '%table'. If this is a custom table,
      please define it first";
    tripal_bulk_loader_throw_error($msg,  array('%table' => $table), TRIPAL_ERROR);
    $data[$priority]['error'] = TRUE;
    return;
  }

  // Check that template required fields are present. if a required field is
  // missing and this
  // is an optional record then just return. otherwise raise an error
  $skip_optional = 0;
  foreach ($table_data['required'] as $field => $required) {
    if ($required) {
      // check if the field has no value (or array is empty)
      if (!isset($values[$field]) or
          (is_array($values[$field]) and count($values[$field]) == 0)) {
        // check if the record is optional.  For backwards compatiblity we need to
        // check if the 'mode' is set to 'optional'
        if ($table_data['optional'] or preg_match('/optional/', $table_data['mode']) or
            $table_data['select_optional'])  {
          $skip_optional = 1;
          // set the values array to be empty since we all required fields are
          // optional and we can't do a select/insert so we don't want to keep
          // the values if this record is used in a later FK relationship.
          $values = array();
        }
        else {
          $msg = "Line " . $addt->line_num . ' "' . $table_data['record_id'] .
            '" (' . $table_data['mode'] . ') Missing template required value: ' . $table . '.' . $field;
          tripal_bulk_loader_throw_error($msg, array(), TRIPAL_WARNING);
          $data[$priority]['error'] = TRUE;
          $no_errors = FALSE;
        }
      }
    }
  }

  // for an insert, check that all database required fields are present in the values array
  // we check for 'optional' in the mode for backwards compatibility. The 'optional'
  // mode used to be a type of insert
  if (!$skip_optional and (preg_match('/insert/', $table_data['mode']) or
       preg_match('/optional/', $table_data['mode']))) {
    // Check all database table required fields are set
    $fields = $table_desc['fields'];
    foreach ($fields as $field => $def) {
      // a field is considered missing if it cannot be null and there is no default
      // value for it or it is not of type 'serial'
      if (array_key_exists('not null', $def) and $def['not null'] == 1 and   // field must have a value
          !array_key_exists($field, $values) and                             // there is not a value for it
          !array_key_exists('default', $def) and                             // there is no default for it
          strcmp($def['type'], 'serial') != 0) {                             // it is not a 'serial' type column
        $msg = "Line " . $addt->line_num . ' ' . $table_data['record_id'] .
               ' (' . $table_data['mode'] . ') Missing Database Required Value: ' . $table . '.' . $field;
        tripal_bulk_loader_throw_error($msg, array(), TRIPAL_ERROR);
        $data[$priority]['error'] = TRUE;
        $no_errors = FALSE;
      }
    }
  }

  // add updated values array into the data array
  $data[$priority]['values_array'] = $values;

  // if there was an error already -> don't insert
  if (array_key_exists('error', $data[$priority]) and $data[$priority]['error']) {
    tripal_bulk_loader_throw_error('Skipping processing of %table due to previous errors',array('%table'=>$table),TRIPAL_NOTICE);
    return $no_errors;
  }

  // skip optional fields
  if ($skip_optional) {
    tripal_bulk_loader_throw_error('Skipping an optional record (%record)',array('%record'=>$table_data['record_id']),TRIPAL_NOTICE);
    return $no_errors;
  }

  // check if it is already inserted
  if (array_key_exists('inserted', $table_data) and $table_data['inserted']) {
    tripal_bulk_loader_throw_error('Skipping %record since it is already inserted',array('%record'=>$table_data['record_id']),TRIPAL_NOTICE);
    return $no_errors;
  }

  // check if it is already selected, if so, just get the value stored in
  // the default_data array
  if (array_key_exists('selected', $table_data) and $table_data['selected']) {
    $data[$priority]['values_array'] = $default_data[$priority]['values_array'];
    tripal_bulk_loader_throw_error('%record was already selected thus we are just returning the values previously selected.',array('%record'=>$table_data['record_id']),TRIPAL_NOTICE);
    return $no_errors;
  }

  // make sure we have some value in the select_if_duplicate and update_if_duplicate options
  if (!array_key_exists('select_if_duplicate', $table_data)) {
    $table_data['select_if_duplicate'] = 0;
  }
  if (!array_key_exists('update_if_duplicate', $table_data)) {
    $table_data['update_if_duplicate'] = 0;
  }

  // if "select if duplicate" is enabled then check to ensure unique constraint is not violated.
  // If it is violated then simply return, the record already exists in the database.
  // We check for "insert_unique" for backwards compatibilty but that mode no longer exists
  $data[$priority]['is_duplicate'] = 0;
  if (preg_match('/insert_unique/', $table_data['mode']) or
     $table_data['select_if_duplicate'] == 1 or
     $table_data['update_if_duplicate'] == 1) {
    $options = array('is_duplicate' => TRUE, 'print_errors' => TRUE);
    $duplicate = chado_select_record($table, array_keys($table_desc['fields']), $values, $options);

    // if this is a duplicate then substitute the values in the table_data array so
    // that for future records that may depend on this one, they can get the values needed
    if ($duplicate and is_array($duplicate) and count($duplicate) == 1) {
      $dup_record = $duplicate[0];
      // save the duplicate record for later.  If this is an update_if_duplicate
      // then we'll need this record as the match
      $data[$priority]['is_duplicate'] = (array) $dup_record;

      // if all we have is one field then we will just use the value returned
      // rather than create an array of values. This way it will prevent
      // the tripal_core_chado_(select|insert|update) from recursing on
      // foreign keys and make the loader go faster.
      if (count((array) $dup_record) == 1) {
        foreach ($dup_record as $key => $value) {
          $data[$priority]['values_array'] = $value;
        }
      }
      // if we have multiple fields returned then we need to set the values
      // the new array.
      else {
        // convert object to array
        $new_values = array();
        foreach ($dup_record as $key => $value) {
          $new_values[$key] = $value;
        }
        $data[$priority]['values_array'] = $new_values;
      }
      // return if this is a select_if_duplicate
      if ($table_data['select_if_duplicate'] == 1) {
        tripal_bulk_loader_throw_error('Simply returning values for %record since it was already inserted',array('%record'=>$table_data['record_id']),TRIPAL_NOTICE);
        return $no_errors;
      }
    }
  }
  else {
    # TODO: what to do if there are more than one value returned when
    # checking for a duplicate?
  }

  if (!preg_match('/select/', $table_data['mode'])) {
    // Use prepared statement?
    if (variable_get('tripal_bulk_loader_prepare', TRUE)) {
      $options = array('statement_name' =>  'record_' . $addt->nid . '_' . $priority);
      if (($addt->line_num > 1 && $addt->group_index == 1) OR $addt->group_index > 1) {
        //$options['is_prepared'] = TRUE;
      }
    }
    else {
      $options = array();
    }
    // Skip chado_insert_record() built-in validation?
    if (variable_get('tripal_bulk_loader_skip_validation', FALSE)) {
      $options['skip_validation'] = TRUE;
    }

    if ($table_data['update_if_duplicate'] == 1) {
      if (array_key_exists('statement_name', $options)) {
        $options['statement_name'] = 'upd_' . $options['statement_name'];
      }
      // This should have been set on the first round of inserts for this record
      $match = $data[$priority]['is_duplicate'];
      // However, sometimes there is a pre-existing record before the loader starts
      // Thus check that this value is set and if not, then generate a match array
      // based on the unique keys for this record.
      if (empty($match)) {
        $match = array();
        // First check to see if we have fields for the primary key
        foreach ($table_desc['primary key'] as $k_field) {
          if (!empty($values[$k_field])) {
            $match[$k_field] = $values[$k_field];
          }
        }
        // Otherwise check the fields that are part of the unique key
        if (empty($match)) {
          foreach ($table_desc['unique keys'] as $u_keys) {
            foreach ($u_keys as $u_field) {
              if (!empty($values[$u_field])) {
                $match[$u_field] = $values[$u_field];
              }
            }
          }
        }
      }
      if (!empty($match)) {
        // Now we need to check if it already exists via a select
        $results = chado_select_record($table, array_keys($table_desc['fields']), $match, array('print_errors' => TRUE));
        // If not then insert
        if (empty($results)) {
          $options['statement_name'] = 'ins_'.$options['statement_name'];
          $options['print_errors'] = TRUE;
          $record = chado_insert_record($table, $values, $options);
        }
        else {
          $options['return_record'] = TRUE;
          $options['print_errors'] = TRUE;
          $record = chado_update_record($table, $match, $values, $options);
        }
      }
      else {
        $msg = "\nLine " . $addt->line_num . ' ' . $table_data['record_id'] . ' (' .
        $table_data['mode'] . ') Unable to update record since none of the unique key or primary key fields were available ' .
        ' where values:' . print_r($values, TRUE);

        tripal_bulk_loader_throw_error($msg, array(), TRIPAL_ERROR);
        $data[$priority]['error'] = TRUE;
        $no_errors = FALSE;
      }
    }
    else {
      $options['print_errors'] = TRUE;
      $record = chado_insert_record($table, $values, $options);
    }

    // if the insert was not successful
    if (!$record) {
      $msg = "\nLine " . $addt->line_num . ' ' . $table_data['record_id'] . ' (' .
        $table_data['mode'] . ') Unable to insert record into ' . $table .
        ' where values:' . print_r($values, TRUE);

      tripal_bulk_loader_throw_error($msg, array(), TRIPAL_ERROR);
      $data[$priority]['error'] = TRUE;
      $no_errors = FALSE;
    }
    // if the insert was succesful
    else {

      // if mode=insert_once then ensure we only insert it once
      if (preg_match('/insert_once/', $table_data['mode'])) {
        $default_data[$priority]['inserted'] = TRUE;
      }

      // add to tripal_bulk_loader_inserted
      if ($addt->node->keep_track_inserted) {
        $insert_record = db_query(
          "SELECT * FROM {tripal_bulk_loader_inserted} WHERE table_inserted_into=:table AND nid=:nid",
          array(
          ':table' => $table,
          'nid' => $addt->nid
        ))->fetchObject();
        if ($insert_record) {
          $insert_record->ids_inserted .= ',' . $record[$table_desc['primary key'][0] ];
          drupal_write_record('tripal_bulk_loader_inserted', $insert_record, 'tripal_bulk_loader_inserted_id');
          //print 'Update: '.print_r($insert_record,TRUE)."\n";
          //return $no_errors;
        }
        else {
          $insert_record = array(
            'nid' => $addt->nid,
            'table_inserted_into' => $table,
            'table_primary_key' => $table_desc['primary key'][0],
            'ids_inserted' => $record[ $table_desc['primary key'][0] ],
          );
          //print 'New: '.print_r($insert_record,TRUE)."\n";
          $success = drupal_write_record('tripal_bulk_loader_inserted', $insert_record);
          //return $no_errors;
        }//end of if insert record
      }// end of if keeping track of records inserted

      // substitute the values array for the primary key if it exists
      // and is a single field
      if (array_key_exists('primary key', $table_desc)) {
        if (count($table_desc['primary key']) == 1) {
          $pkey_field = $table_desc['primary key'][0];
          $data[$priority]['values_array'] = $record[$pkey_field];
        }
      }
      else {
        //add changes back to values array
        $data[$priority]['values_array'] = $record;
        $values = $record;
      }
    } //end of if insert was successful
  }
  // perform a select
  else {
    // get the matches for this select
    $matches = array();
    if (is_array($values) and count($values) > 0) {
      $matches = chado_select_record($table, array_keys($table_desc['fields']), $values, array('print_errors' => TRUE));
    }
    // if the record doesn't exist and it's not optional then generate an error
    if (count($matches) == 0) {
      // No record on select
      if ($table_data['select_optional'] != 1) {
        $msg = "\nLine " . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') No Matching record in ' . $table . ' where values:' . print_r($values, TRUE);
        tripal_bulk_loader_throw_error($msg, array(), TRIPAL_ERROR);
        $data[$priority]['error'] = TRUE;
        $no_errors = FALSE;
      }
      // there is no match and select optional is turned on, so we want to set
      // the values to empty for any records with an FK relationship on this one
      else {
        $data[$priority]['values_array'] = NULL;
      }
    }
    // if we have more than one record matching and this select isn't optional then fail
    if (count($matches) > 1) {
      if ($table_data['select_optional'] != 1) {
        $msg = "\nLine " . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') Too many matching records in ' . $table . ' where values:' . print_r($values, TRUE);
        tripal_bulk_loader_throw_error($msg, array(), TRIPAL_WARNING);
        $data[$priority]['error'] = TRUE;
        $no_errors = FALSE;
      }
      // there are too many matches and this is an optional select so set
      // the values to empty for any records with an FK relationship on this one
      else {
        $data[$priority]['values_array'] = NULL;
      }
    }
    // if mode=select_once then ensure we only select it once
    if (preg_match('/select_once/', $table_data['mode'])) {
      $default_data[$priority]['selected'] = TRUE;

      // save the pkey
      if (array_key_exists('primary key', $table_desc)) {
        $new_values = array();
        foreach ($matches[0] as $key => $value) {
          $new_values[$key] = $value;
        }
        $default_data[$priority]['values_default'] = $new_values;
      }
    }
  }

  return $no_errors;
}

/**
 * This function adds the file data to the values array
 *
 * @param $values
 *   The default values array -contains all constants
 * @param $line
 *   An array of values for the current line
 * @param $field2column
 *   An array mapping values fields to line columns
 * @return
 *   Supplemented values array
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_add_spreadsheetdata_to_values($values, $line, $field2column) {
  foreach ($values as $field => $value) {
    if (is_array($value)) {
      continue;
    }

    $column = $field2column[$field] - 1;
    if ($column < 0) {
      continue;
    }

    if (preg_match('/\S+/', $line[$column])) {
      $values[$field] = $line[$column];
    }
    else {
      unset($values[$field]);
    }
  }

  return $values;
}

/**
 * Handles foreign keys in the values array.
 *
 * Specifically, if the value for a field is an array then it is assumed that the array contains
 * the name of the record whose values array should be substituted here. Thus the foreign
 * record is looked up and the values array is substituted in.
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_add_foreignkey_to_values($table_array, $values, $data, $record2priority, $nid, $priority, $default_data) {

  // iterate through each field in the $values arrray and
  // substitute any values for FK / referring fields
  foreach ($values as $field => $value) {
    // if the field value is an array then it is an FK
    if (is_array($value)) {

      // get the name and priority of the foreign record
      $foreign_record   = $value['foreign record']['record'];
      $foreign_priority = $record2priority[$foreign_record];
      $foreign_table    = $value['foreign record']['table'];
      $foreign_field    = $value['foreign record']['field'];

      // get the values of the foreign record and substitute those for the values
      $foreign_values   = $data[$foreign_priority]['values_array'];

      // check to see if we have any default values in the $default_data array
      // these were populated from select statements that only need to run once
      // so we can reuse the values from those previous selects.
      if (array_key_exists($foreign_priority, $default_data) and
          array_key_exists('values_default', $default_data[$foreign_priority]) and
          array_key_exists($foreign_field, $default_data[$foreign_priority]['values_default'])) {
         $values[$field] = $default_data[$foreign_priority]['values_default'][$foreign_field];
         continue;
      }

      // if the field in the Referral records is in a FK relationship with
      // this field then we can simply keep the value we have
      $tbl_description = chado_get_schema($table_array['table']);
      if ($tbl_description and
          array_key_exists('foreign keys', $tbl_description) and
          array_key_exists($foreign_table, $tbl_description['foreign keys']) and
          array_key_exists($field, $tbl_description['foreign keys'][$foreign_table]['columns']) and
          $foreign_field == $tbl_description['foreign keys'][$foreign_table]['columns'][$field]) {

        $values[$field] = $foreign_values;
      }
      // if the field in the Referral records is not in an FK relationship
      // with this field then we we have to get the requested value, we must
      // return only a single value
      else {
        // if the current value of the referral records is a non-array then this
        // is the primary key, we can use it to select the value we need.
        $fk_description = chado_get_schema($foreign_table);
        if (!is_array($foreign_values)) {
          // if we have a value then use it to get the field we need
          if ($foreign_values) {
            $fvalues = array($fk_description['primary key'][0] => $foreign_values);
            $columns = array($foreign_field);
            $options = array('statement_name' => 'pk_' . $foreign_table);
            $options['print_errors'] = TRUE;
            $record  = chado_select_record($foreign_table, $columns, $fvalues, $options);
            if ($record) {
              $values[$field] = $record[0]->$foreign_field;
            }
            else {
              unset($values[$field]);
            }
          }
          // if we don't have a value then there's nothing we can do so
          // set this value to nothing as well
          else {
            unset($values[$field]);
          }
        }
        // if the current value is an array and our field is not in it, then
        // we need to select a value for our field.
        else {
          $fvalues  = $foreign_values;
          $columns = array($foreign_field);
          $options = array('statement_name' => 'blk_' . $nid . $priority . $foreign_table);
          $options['print_errors'] = TRUE;
          $record  = chado_select_record($foreign_table, $columns, $fvalues, $options);
          if ($record) {
            $values[$field] = $record[0]->$foreign_field;
          }
          else {
            unset($values[$field]);
          }
        } // end else from: if (!is_array($foreign_values) ...
      } // end else from: if ($tbl_description ...
    } // end if(is_array($value)) ...
  } // end foreach ($values ...

  // return the updated field values
  return $values;
}

/**
 * Uses a supplied regex to transform spreadsheet values
 *
 * @param $values
 *   The select/insert values array for the given table
 * @param $table_data
 *   The data array for the given table
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_regex_tranform_values($values, $table_data, $line) {

  if (!array_key_exists('regex_transform', $table_data) or
      empty($table_data['regex_transform']) or
      !array_key_exists('regex_transform', $table_data) or
      !is_array($table_data['regex_transform'])) {
    return $values;
  }

  //tripal_core_report_error('T_bulk_loader', TRIPAL_NOTICE,'Regex Transformation:<pre>'.print_r($table_data['regex_transform'], TRUE).'</pre>', array());

  foreach ($table_data['regex_transform'] as $field => $regex_array) {
    if (!array_key_exists('replace', $regex_array) or
        !array_key_exists('pattern', $regex_array) or
        !is_array($regex_array['replace'])) {
      continue;
    }

    // Check for <#column:\d+#> notation
    // if present replace with that column in the current line
    foreach ($regex_array['replace'] as $key => $replace) {
      if (preg_match_all('/<#column:(\d+)#>/', $replace, $matches)) {
        foreach ($matches[1] as $k => $column_num) {
          $replace = preg_replace('/' . $matches[0][$k] .'/', $line[$column_num-1], $replace);
        }
        $regex_array['replace'][$key] = $replace;
      }
    }

    // do the full replacement
    $old_value = $values[$field];
    $new_value = preg_replace($regex_array['pattern'], $regex_array['replace'], $old_value);
    $values[$field] = $new_value;

    if ($values[$field] === '') {
      unset($values[$field]);
    }
    //print 'Now:'.$values[$field]."\n";
  }

  return $values;
}

/**
 * Flattens an array up to two levels
 * Used for printing of arrays without taking up much space
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_flatten_array($values) {
  $flattened_values = array();

  foreach ($values as $k => $v) {
    if (is_array($v)) {
      $vstr = array();
      foreach ($v as $vk => $vv) {
        if (drupal_strlen($vv) > 20) {
          $vstr[] = $vk . '=>' . drupal_substr($vv, 0, 20) . '...';
        }
        else {
          $vstr[] = $vk . '=>' . $vv;
        }
      }
      $v = '{' . implode(',', $vstr) . '}';
    }
    elseif (drupal_strlen($v) > 20) {
      $v = drupal_substr($v, 0, 20) . '...';
    }
    $flattened_values[] = $k . '=>' . $v;
  }

  return implode(', ', $flattened_values);
}

/**
 * Used to display loader progress to the user
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_progress_bar($current=0, $total=100, $size=50) {
  $new_bar = FALSE;
  $mem = memory_get_usage();

  // First iteration
  if ($current == 0) {
    $new_bar = TRUE;
    fputs(STDOUT, "Progress:\n");
  }

  // Percentage round off for a more clean, consistent look
  $percent = sprintf("%.02f", round(($current/$total) * 100, 2));
  // percent indicator must be four characters, if shorter, add some spaces
  for ($i = strlen($percent); $i <= 4; $i++) {
    $percent = ' ' . $percent;
  }


  $total_size = $size + $i + 3 + 2;
  $place = 0;
  // if it's not first go, remove the previous bar
  if (!$new_bar) {
    for ($place = $total_size; $place > 0; $place--) {
      // echo a backspace (hex:08) to remove the previous character
      //echo "\x08";
    }
  }

  // output the progess bar as it should be
  // Start with a border
  echo '[';
  for ($place = 0; $place <= $size; $place++) {
    // output "full" spaces if this portion is completed
    if ($place <= ($current / $total * $size)) {
      echo '|';
    }
    else {
      // Otherwise empty space
      echo '-';
    }
  }
  // End with a border
  echo ']';

  // end a bar with a percent indicator
  echo " $percent%. ($current of $total) Memory: $mem\r";

  // if it's the end, add a new line
  if ($current == $total) {
    echo "\n";
  }

}

/**
 * Keep track of progress in file rather then database
 *
 * This provides an alternative method to keep track of progress that doesn't require the
 * database. It was needed because you can't switch databases within a transaction...
 * Waiting until the end of a constant set is much too long to wait for any indication
 * that things are working.
 *
 * Each line represents a line processed in the loading file. Each period (.) represents
 * a successfully inserted record.
 *
 * @param $job_id
 *   The ID of the current tripal job
 * @param $record_added
 *   A boolean indicated whether a record was added successfully
 * @param $line_complete
 *   A boolean indicating whether the current line is finished
 * @param $close
 *   A boolean indicating that the file should be closed
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_progress_file_track_job($job_id, $record_added, $line_complete = FALSE, $close = FALSE) {
  // retrieve the file handle
  $file_handle = variable_get('tripal_bulk_loader_progress_file_handle', NULL);

  // open file for reading if not already
  if (!$file_handle) {
    $file_handle = fopen('/tmp/tripal_bulk_loader_progress-'. $job_id . '.out', 'w');
    variable_set('tripal_bulk_loader_progress_file_handle', $file_handle);
  }

  if ($record_added) {
    fwrite($file_handle, '.');
  }

  if ($line_complete) {
    fwrite($file_handle, "\n");
  }

  // close the file if finished
  if ($close) {
    fclose($file_handle);
    variable_set('tripal_bulk_loader_progress_file_handle', NULL);
  }
}

/**
 * Used to throw a hopefully human-readable error.
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_throw_error($msg, $args, $severity) {
  $options = array('print' => TRUE);
  if ($severity == TRIPAL_NOTICE OR $severity == TRIPAL_INFO OR $severity == TRIPAL_DEBUG) {
    unset($options['print']);
  }

  tripal_report_error(
    'tripal_bulk',
    $severity,
    $msg,
    $args,
    $options
  );
}

/**
 * Finishes the loading job by setting variables and exiting.
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_finish_loading($nid, $loaded_without_errors) {

  // set the status of the job (in the node not the tripal jobs)
  if ($loaded_without_errors) {
    $status = 'Loading Completed Successfully';
  }
  else {
    $status = 'Errors Encountered';
  }
  db_update('tripal_bulk_loader')
    ->fields(array('job_status' => $status))
    ->condition('nid', $nid)
    ->execute();

  if(!$loaded_without_errors) {
    drush_set_error('BULK_LOAD_FAILED', 'Execution aborted due to errors.');
    exit();
  }
}
