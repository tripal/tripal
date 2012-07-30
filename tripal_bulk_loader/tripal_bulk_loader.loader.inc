<?php

/**
 * @file
 * @todo Add file header description
 */

/**
 * Add Loader Job Form
 *
 * This form is meant to be included on the node page to allow users to submit/re-submit
 * loading jobs
 */
function tripal_bulk_loader_add_loader_job_form($form_state, $node) {
  $form = array();

  // --notify--
  if ($node->job_status == 'Loading...') {
    $progress = tripal_bulk_loader_progess_file_get_progress($node->job_id);
    drupal_set_message(t("The Loading Summary only updates at the end of each constant set.
      %num records have already been inserted; however, they won't be available until the
      current constant set is full loaded and no errors are encountered.", array('%num' => $progress->num_records)), 'warning');
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
      $success = db_query("UPDATE {tripal_bulk_loader} SET job_id=%d WHERE nid=%d", $job_id, $form_state['values']['nid']);

      // change status
      db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Submitted to Queue', $form_state['values']['nid']);
    }
    else {
      drupal_set_message(t("Can not open %file. Job not scheduled.", array('%file' => $form_state['values']['file'])));
    }
  }
  elseif (preg_match('/Re-Submit Job/', $form_state['values']['op'])) {
    tripal_jobs_rerun($form_state['values']['job_id']);
    db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Submitted to Queue', $form_state['values']['nid']);
  }
  elseif (preg_match('/Cancel Job/', $form_state['values']['op'])) {
    db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Job Cancelled', $form_state['values']['nid']);
    tripal_jobs_cancel($form_state['values']['job_id']);
  }
  elseif (preg_match('/Revert/', $form_state['values']['op'])) {

    // Remove the records from the database that were already inserted
    $resource = db_query('SELECT * FROM {tripal_bulk_loader_inserted} WHERE nid=%d ORDER BY tripal_bulk_loader_inserted_id DESC', $form_state['values']['nid']);
    while ($r = db_fetch_object($resource)) {
      $ids = preg_split('/,/', $r->ids_inserted);
      db_query('DELETE FROM %s WHERE %s IN (%s)', $r->table_inserted_into, $r->table_primary_key, $r->ids_inserted);
      $result = db_fetch_object(db_query('SELECT true as present FROM %s WHERE %s IN (%s)', $r->table_inserted_into, $r->table_primary_key, $r->ids_inserted));
      if (!$result->present) {
        drupal_set_message(t('Successfully Removed data Inserted into the %tableto table.', array('%tableto' => $r->table_inserted_into)));
        db_query('DELETE FROM {tripal_bulk_loader_inserted} WHERE tripal_bulk_loader_inserted_id=%d', $r->tripal_bulk_loader_inserted_id);
      }
      else {
        drupal_set_message(t('Unable to remove data Inserted into the %tableto table!', array('%tableto' => $r->table_inserted_into)), 'error');
      }
    }

    // reset status
    db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Reverted -Data Deleted', $form_state['values']['nid']);
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
 */
function tripal_bulk_loader_load_data($nid, $job_id) {

  // ensure no timeout
  set_time_limit(0);

  // set the status of the job (in the node not the tripal jobs)
  db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Loading...', $nid);


  $node = node_load($nid);
  print "Template: " . $node->template->name . " (" . $node->template_id . ")\n";

  $total_lines = trim(`wc --lines < $node->file`);
  print "File: " . $node->file . " (" . $total_lines . " lines)\n";

  // Prep Work ==================================================================================
  $loaded_without_errors = TRUE;

  // Generate default values array
  $default_data = array();
  $field2column = array();
  $record2priority = array();
  $tables = array();

  foreach ($node->template->template_array as $priority => $record_array) {
    if (!is_array($record_array)) {
      continue;
    }

    // Add tables being inserted into to a list to be treated differently
    // this is used to acquire locks on these tables
    if (preg_match('/insert/', $record_array['mode'])) {
      $tables[$record_array['table']] = $record_array['table'];
    }

    foreach ($record_array['fields'] as $field_index => $field_array) {

      $default_data[$priority]['table'] = $record_array['table'];
      $default_data[$priority]['mode'] = ($record_array['mode']) ? $record_array['mode'] : 'insert_unique';
      $default_data[$priority]['record_id'] = $record_array['record_id'];
      $record2priority[$record_array['record_id']] = $priority;
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
        $default_data[$priority]['values_array'][$field_array['field']]['foreign record'] = $field_array['foreign key'];
        $default_data[$priority]['need_further_processing'] = TRUE;

      }
      else {
        print 'WARNING: Unsupported type: ' . $field_array['type'] . ' for ' . $table . '.' . $field_array['field'] . "!\n";
      }

      $three = $default_data[$priority];
      //watchdog('T_bulk_loader','A)'.$field_index.':<pre>Field Array =>'.print_r($field_array,TRUE)."Initial => \n".print_r($one, TRUE)."\nAfter Regex =>".print_r($two, TRUE)."Final =>\n".print_r($three,TRUE).'</pre>', array(), WATCHDOG_NOTICE);

    } // end of foreach field
    //watchdog('T_bulk_loader','2)'.$record_array['record_id'].':<pre>'.print_r($default_data[$priority], TRUE).'</pre>', array(), WATCHDOG_NOTICE);
  } //end of foreach record

  ///////////////////////////////////////////////
  // For each set of constants
  ///////////////////////////////////////////////
  $original_default_data = $default_data;
  $group_index = 0;
  $total_num_groups = sizeof($node->constants);
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
              watchdog('T_bulk_loader', 'Template has changed after constants were assigned', array(), WATCHDOG_NOTICE);
              exit(1);
            }
          }
          else {
            print "ERROR: Template has changed after constants were assigned!\n";
            watchdog('T_bulk_loader', 'Template has changed after constants were assigned', array(), WATCHDOG_NOTICE);
            exit(1);
          }
        }
      }
    }

    //print "Default Data:".print_r($default_data,TRUE)."\n";
    //watchdog('T_bulk_loader','Default Data:<pre>'.print_r($default_data, TRUE).'</pre>', array(), WATCHDOG_NOTICE);

    //print "\nDefault Values Array: ".print_r($default_data, TRUE)."\n";
    //print "\nField to Column Mapping: ".print_r($field2column, TRUE)."\n";

    // Parse File adding records as we go ========================================================

    // Open File
    $file_handle = fopen($node->file, 'r');

    // Set defaults
    if (preg_match('/(t|true|1)/', $node->file_has_header)) {
      fgets($file_handle, 4096);
    }
    $num_records = 0;
    $num_lines = 0;
    $num_errors = 0;
    $interval = intval($total_lines * 0.10);
    if ($interval == 0) {
      $interval = 1;
    }

    // Start Transaction
    switch (variable_get('tripal_bulk_loader_transactions', 'row')) {
      case "none":
        break;
      case "all":
        tripal_db_start_transaction();
        $transactions = TRUE;
        $savepoint = "";
        break;
      case "row":
        tripal_db_start_transaction();
        $transactions = TRUE;
        $savepoint = "last_row_complete";
        break;
    }

    // Disable triggers
    $triggers_disabled = FALSE;
    if ($transactions AND variable_get('tripal_bulk_loader_disable_triggers', TRUE)) {
      $triggers_disabled = TRUE;
      chado_query("SET CONSTRAINTS ALL DEFERRED");
    }

    // Acquire Locks
    $lockmode = variable_get('tripal_bulk_loader_lock', 'ROW EXCLUSIVE');
    foreach ($tables as $table) {
      chado_query("LOCK TABLE %s IN %s MODE", $table, $lockmode);
    }

    tripal_bulk_loader_progress_bar(0, $total_lines);
    while (!feof($file_handle)) {

      // Clear variables
      // Was added to fix memory leak
      unset($line);                     unset($raw_line);
      unset($data);                     unset($data_keys);
      unset($priority);                 unset($sql);
      unset($result);

      $raw_line = fgets($file_handle, 4096);
      $raw_line = trim($raw_line);
      if (empty($raw_line)) {
        continue;
      } // skips blank lines
      $line = explode("\t", $raw_line);
      $num_lines++;

      // update the job status every 10% of lines processed for the current group
      if ($node->job_id and $num_lines % $interval == 0) {
        // percentage of lines processed for the current group
        $group_progress = round(($num_lines/$total_lines)*100);
        tripal_bulk_loader_progress_bar($num_lines, $total_lines);
        // percentage of lines processed for all groups
        // <previous group index> * 100 + <current group progress>
        // --------------------------------------------------------
        //               <total number of groups>
        // For example, if you were in the third group of 3 constant sets
        // and had a group percentage of 50% then the job progress would be
        // (2*100 + 50%) / 3 = 250%/3 = 83%
        $job_progress = round(((($group_index-1)*100)+$group_progress)/$total_num_groups);
        tripal_job_set_progress($node->job_id, $job_progress);
      }

      $data = $default_data;

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
        $status = process_data_array_for_line($priority, $data, $default_data, $options);
        tripal_bulk_loader_progress_file_track_job($job_id, $status);
        if (!$status ) {
          // Encountered an error
          if ($transactions) {
            tripal_db_rollback_transaction($savepoint);
          }
          $failed = TRUE;
          break;
        }
      } // end of foreach table in default data array

      tripal_bulk_loader_progress_file_track_job($job_id, FALSE, TRUE);

      if ($failed) {
        break;
      }
      else {
        // Row inserted successfully
        // Set savepoint if supplied
        if ($savepoint) {
          if ($num_lines == 1) {
            tripal_db_set_savepoint_transaction($savepoint);
          }
          else {
            // Tell it to remove the previous savepoint of the same name
            tripal_db_set_savepoint_transaction($savepoint, TRUE);
          }
        }
      }

    } //end of foreach line of file

    // END Transaction
    if ($transactions) {
      // end the transaction
      tripal_db_commit_transaction();
    }

    if ($failed) {
      $loaded_without_errors = FALSE;
      break;
    }

    tripal_bulk_loader_progress_bar($total_lines, $total_lines);
    tripal_bulk_loader_progress_file_track_job($job_id, FALSE, FALSE, TRUE);
  } //end of foreach constant set

  // set the status of the job (in the node not the tripal jobs)
  if ($loaded_without_errors) {
    $status = 'Loading Completed Successfully';
  }
  else {
    $status = 'Errors Encountered';
  }
  db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", $status, $nid);

}

/**
 *
 *
         $options = array(
          'field2column' => $field2column,
          'record2priority' => $record2priority,
          'line' => $line,
          'line_num' => $num_lines,
          'group_index' => $group_index,
          'node' => $node,
          'nid' => $node->nid,
        );
 */
function process_data_array_for_line($priority, &$data, &$default_data, $addt) {
  $table_data = $data[$priority];
  $addt = (object) $addt;
  $no_errors = TRUE;

  $table = $table_data['table'];
  $values = $table_data['values_array'];

  //watchdog('T_bulk_loader','Original:<pre>'.print_r($table_data, TRUE).'</pre>', array(), WATCHDOG_NOTICE);

  //print 'default values:'.print_r($values,TRUE)."\n";
  if ($table_data['need_further_processing']) {
    $values = tripal_bulk_loader_add_spreadsheetdata_to_values($values, $addt->line, $addt->field2column[$priority]);
    if (!$values) {
      //watchdog('T_bulk_loader', 'Line ' . $addt->line_num . ' Data File Added:' . print_r($values, TRUE), array(), WATCHDOG_NOTICE);
    }

    $values = tripal_bulk_loader_add_foreignkey_to_values($values, $data, $addt->record2priority);
    if (!$values) {
      //watchdog('T_bulk_loader', 'Line ' . $addt->line_num . ' FK Added:<pre>' . print_r($values, TRUE) . print_r($data[$priority], TRUE) . '</pre>', array(), WATCHDOG_NOTICE);
    }
  }
  $values = tripal_bulk_loader_regex_tranform_values($values, $table_data, $addt->line);
  if (!$values) {
    //watchdog('T_bulk_loader', 'Line ' . $addt->line_num . ' Regex:<pre>' . print_r($values, TRUE) . print_r($table_data, TRUE) . '</pre>' . '</pre>', array(), WATCHDOG_NOTICE);
  }

  if (!$values) {
    $msg = 'Line ' . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') Aborted due to error in previous record. Values of current record:' . print_r($table_data['values_array'], TRUE);
    watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING);
    print "ERROR: " . $msg . "\n";
    $data[$priority]['error'] = TRUE;
    $no_errors = FALSE;
  }

  $table_desc = tripal_core_get_chado_table_schema($table);
  if (preg_match('/optional/', $table_array['mode'])) {
    // Check all db required fields are set
    $fields = $table_desc['fields'];
    foreach ($fields as $field => $def) {
      // a field is considered missing if it cannot be null and there is no default
      // value for it or it is of type 'serial'
      if ($def['not null'] == 1 and !array_key_exists($field, $insert_values) and !isset($def['default']) and strcmp($def['type'], serial)!=0) {
        $msg = 'Line ' . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') Missing Database Required Value: ' . $table . '.' . $field;
        watchdog('T_bulk_loader', $msg, array(), WATCHDOG_NOTICE);
        $data[$priority]['error'] = TRUE;
      }
    }
  } //end of if optional record

  // Check required fields are present
  foreach ($table_data['required'] as $field => $required) {
    if ($required) {
      if (!isset($values[$field])) {
        $msg = 'Line ' . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') Missing Template Required Value: ' . $table . '.' . $field;
        watchdog('T_bulk_loader', $msg, array(), WATCHDOG_NOTICE);
        $data[$priority]['error'] = TRUE;
      }
    }
  }

  // add new values array into the data array
  $data[$priority]['values_array'] = $values;

  // check if it is already inserted
  if ($table_data['inserted']) {
    //watchdog('T_bulk_loader','Already Inserted:'.print_r($values,TRUE),array(),WATCHDOG_NOTICE);
    return $no_errors;
  }

  // if there was an error already -> don't insert
  if ($data[$priority]['error']) {
    return $no_errors;
  }

  $header = '';
  if (isset($values['feature_id'])) {
    $header = $values['feature_id']['uniquename'] . ' ' . $table_data['record_id'];
  }
  else {
    $header = $values['uniquename'] . ' ' . $table_data['record_id'];
  }

  // if insert unique then check to ensure unique
  if (preg_match('/insert_unique/', $table_data['mode'])) {
    $unique = tripal_core_chado_select($table, array_keys($table_desc['fields']), $values, array('has_record' => TRUE));
    //print 'Unique?'.print_r(array('table' => $table, 'columns' => array_keys($table_desc['fields']), 'values' => $values),TRUE).' returns '.$unique."\n";
    if ($unique > 0) {
      //$default_data[$priority]['inserted'] = TRUE;
      //watchdog('T_bulk_loader', $header.': Not unique ('.$unique.'):'.print_r($values,'values')."\n".print_r($data,TRUE),array(),WATCHDOG_NOTICE);;
      return $no_errors;
    }
  }


  if (!preg_match('/select/', $table_data['mode'])) {
    // Use prepared statement?
    if (variable_get('tripal_bulk_loader_prepare', TRUE)) {
      $options = array('statement_name' => 'record_' . $priority);
      if (($addt->line_num > 1 && $addt->group_index == 1) OR $addt->group_index > 1) {
        $options['is_prepared'] = TRUE;
      }
    }
    else {
      $options = array();
    }

    // Skip tripal_core_chado_insert() built-in validation?
    if (variable_get('tripal_bulk_loader_skip_validation', FALSE)) {
      $options['skip_validation'] = TRUE;
    }

    $record = tripal_core_chado_insert($table, $values, $options);

    if (!$record) {
      $msg = 'Line ' . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') Unable to insert record into ' . $table . ' where values:' . print_r($values, TRUE);
      watchdog('T_bulk_loader', $msg, array(), WATCHDOG_ERROR);
      print "ERROR: " . $msg . "\n";
      $data[$priority]['error'] = TRUE;
      $no_errors = FALSE;
    }
    else {
      //add changes back to values array
      $data[$priority]['values_array'] = $record;
      $values = $record;

      // if mode=insert_once then ensure we only insert it once
      if (preg_match('/insert_once/', $table_data['mode'])) {
        $default_data[$priority]['inserted'] = TRUE;
      }

      // add to tripal_bulk_loader_inserted
      if ($addt->node->keep_track_inserted) {
        $insert_record = db_fetch_object(db_query(
          "SELECT * FROM {tripal_bulk_loader_inserted} WHERE table_inserted_into='%s' AND nid=%d",
          $table,
          $addt->nid
        ));
        if ($insert_record) {
          $insert_record->ids_inserted .= ',' . $values[ $table_desc['primary key'][0] ];
          drupal_write_record('tripal_bulk_loader_inserted', $insert_record, 'tripal_bulk_loader_inserted_id');
          //print 'Update: '.print_r($insert_record,TRUE)."\n";
          return $no_errors;
        }
        else {
          $insert_record = array(
            'nid' => $addt->nid,
            'table_inserted_into' => $table,
            'table_primary_key' => $table_desc['primary key'][0],
            'ids_inserted' => $values[ $table_desc['primary key'][0] ],
          );
          //print 'New: '.print_r($insert_record,TRUE)."\n";
          $success = drupal_write_record('tripal_bulk_loader_inserted', $insert_record);
          return $no_errors;
        }//end of if insert record
      }// end of if keeping track of records inserted

    } //end of if insert was successful
  }
  else {
    $exists = tripal_core_chado_select($table, array_keys($table_desc['fields']), $values, array('has_record' => TRUE));
    if (!$exists) {
      // No record on select
      $msg = 'Line ' . $addt->line_num . ' ' . $table_data['record_id'] . ' (' . $table_data['mode'] . ') No Matching record in ' . $table . ' where values:' . print_r($values, TRUE);
      watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING);
      $data[$priority]['error'] = TRUE;
      $no_errors = FALSE;
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
 */
function tripal_bulk_loader_add_foreignkey_to_values($values, $data, $record2priority) {

  foreach ($values as $field => $value) {
    if (is_array($value)) {
      $foreign_record = $value['foreign record'];
      $foreign_priority = $record2priority[$foreign_record];
      $foreign_values = $data[$foreign_priority]['values_array'];

      // add to current values array
      $values[$field] = $foreign_values;
    }
  }

  return $values;
}

/**
 * Uses a supplied regex to transform spreadsheet values
 *
 * @param $values
 *   The select/insert values array for the given table
 * @param $table_data
 *   The data array for the given table
 */
function tripal_bulk_loader_regex_tranform_values($values, $table_data, $line) {

  if (empty($table_data['regex_transform']) OR !is_array($table_data['regex_transform'])) {
    return $values;
  }

  //watchdog('T_bulk_loader','Regex Transformation:<pre>'.print_r($table_data['regex_transform'], TRUE).'</pre>', array(), WATCHDOG_NOTICE);

  foreach ($table_data['regex_transform'] as $field => $regex_array) {
    if (!is_array($regex_array['replace'])) {
      continue;
    }

    //print 'Match:'.print_r($regex_array['pattern'],TRUE)."\n";
    //print 'Replace:'.print_r($regex_array['replace'],TRUE)."\n";
    //print 'Was:'.$values[$field]."\n";

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
 */
function tripal_bulk_loader_progress_bar($current=0, $total=100, $size=50) {

  // First iteration
  if ($current == 0) {
    $new_bar = TRUE;
    fputs(STDOUT, "Progress:\n");
  }

  //Percentage round off for a more clean, consistent look
  $perc = round(($current/$total)*100, 2);
  // percent indicator must be four characters, if shorter, add some spaces
  for ($i = strlen($perc); $i <= 4; $i++) {
    $perc = ' ' . $perc;
  }

  $total_size = $size + $i + 3 + 2;
  // if it's not first go, remove the previous bar
  if (!$new_bar) {
    for ($place = $total_size; $place > 0; $place--) {
      // echo a backspace (hex:08) to remove the previous character
      echo "\x08";
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
  echo " $perc%";

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
