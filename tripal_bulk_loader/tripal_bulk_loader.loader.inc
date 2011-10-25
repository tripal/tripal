<?php

/**
 * Add Loader Job Form
 *
 * This form is meant to be included on the node page to allow users to submit/re-submit 
 * loading jobs
 */
function tripal_bulk_loader_add_loader_job_form ($form_state, $node) {
  $form = array();
  
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
  
  $form['submit-revert'] = array(
    '#type' => ($node->job_id) ? 'submit' : 'hidden',
    '#value' => 'Revert',
  );  
  
  return $form;
}

/**
 * Add Loader Job Form (Submit)
 */
function tripal_bulk_loader_add_loader_job_form_submit ($form, $form_state) {
  global $user;
  
  if (preg_match('/Submit Job/', $form_state['values']['op'])) {
    //Submit Tripal Job
		$job_args[1] = $form_state['values']['nid'];
		if (is_readable($form_state['values']['file'])) {
			$fname = basename($form_state['values']['file']);
			$job_id = tripal_add_job("Bulk Loading Job: $fname",'tripal_bulk_loader', 'tripal_bulk_loader_load_data', $job_args, $user->uid);
			
			// add job_id to bulk_loader node
      $success = db_query("UPDATE {tripal_bulk_loader} SET job_id=%d WHERE nid=%d", $job_id, $form_state['values']['nid']);
      
      // change status
      db_query("UPDATE tripal_bulk_loader SET job_status='%s' WHERE nid=%d",'Submitted to Queue', $form_state['values']['nid']);
		} else {
			drupal_set_message("Can not open ".$form_state['values']['file'].". Job not scheduled.");
		}    
  } elseif (preg_match('/Re-Submit Job/', $form_state['values']['op'])) {
    tripal_jobs_rerun($form_state['values']['job_id']);
    db_query("UPDATE tripal_bulk_loader SET job_status='%s' WHERE nid=%d",'Submitted to Queue', $form_state['values']['nid']);
  } elseif (preg_match('/Cancel Job/', $form_state['values']['op'])) {
    db_query("UPDATE tripal_bulk_loader SET job_status='%s' WHERE nid=%d",'Job Cancelled', $form_state['values']['nid']);
    tripal_jobs_cancel($form_state['values']['job_id']);
  } elseif (preg_match('/Revert/', $form_state['values']['op'])) {
  
    // Remove the records from the database that were already inserted
    $resource = db_query('SELECT * FROM {tripal_bulk_loader_inserted} WHERE nid=%d ORDER BY tripal_bulk_loader_inserted_id DESC', $form_state['values']['nid']);
    while ($r = db_fetch_object($resource)) {
      $ids = preg_split('/,/',$r->ids_inserted);
      db_query('DELETE FROM %s WHERE %s IN (%s)',$r->table_inserted_into, $r->table_primary_key, $r->ids_inserted);
      $result = db_fetch_object(db_query('SELECT true as present FROM %s WHERE %s IN (%s)', $r->table_inserted_into, $r->table_primary_key, $r->ids_inserted));
      if (!$result->present) {
        drupal_set_message('Successfully Removed data Inserted into the '.$r->table_inserted_into.' table.');
        db_query('DELETE FROM {tripal_bulk_loader_inserted} WHERE tripal_bulk_loader_inserted_id=%d',$r->tripal_bulk_loader_inserted_id);
      } else {
        drupal_set_message('Unable to remove data Inserted into the '.$r->table_inserted_into.' table!', 'error');
      }
    }
    
    // reset status
    db_query("UPDATE tripal_bulk_loader SET job_status='%s' WHERE nid=%d",'Reverted -Data Deleted', $form_state['values']['nid']);
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
function tripal_bulk_loader_load_data($nid) {
  
  // ensure no timeout
  set_time_limit(0);
  
  // set the status of the job (in the node not the tripal jobs)
  db_query("UPDATE tripal_bulk_loader SET job_status='%s' WHERE nid=%d",'Loading...', $nid);
  
  print "Memory Usage (Start): ".number_format((memory_get_usage() * 0.000000953674316), 5, '.', ',') . " Mb\n";
  
  $node = node_load($nid);
  print "Template: ".$node->template->name." (".$node->template_id.")\n";
  print "File: ".$node->file."\n";
  
  print "Memory Usage (After Node Load): ".number_format((memory_get_usage() * 0.000000953674316), 5, '.', ',') . " Mb\n";
  
  // Prep Work ==================================================================================
  $loaded_without_errors = TRUE;
  
  // Generate default values array
  $default_data = array();
  $field2column = array();
  $record2priority = array();
  foreach ($node->template->template_array as $priority => $record_array) {
    if (!is_array($record_array)) { continue; }
    
    //watchdog('T_bulk_loader','1)'.$record_array['record_id']." => \n<pre>".print_r($record_array,TRUE).'</pre>', array(), WATCHDOG_NOTICE);
    
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
        
      } elseif (preg_match('/constant/', $field_array['type'])) {
        $default_data[$priority]['values_array'][$field_array['field']] = $field_array['constant value'];
        
      } elseif (preg_match('/foreign key/', $field_array['type'])) {
        $default_data[$priority]['values_array'][$field_array['field']] = array();
        $default_data[$priority]['values_array'][$field_array['field']]['foreign record'] = $field_array['foreign key'];
        $default_data[$priority]['need_further_processing'] = TRUE;
        
      } else {
        print 'WARNING: Unsupported type: '. $field_array['type'] . ' for ' . $table . '.' . $field_array['field']."!\n";
      }   
      
      $three = $default_data[$priority];
      //watchdog('T_bulk_loader','A)'.$field_index.':<pre>Field Array =>'.print_r($field_array,TRUE)."Initial => \n".print_r($one, TRUE)."\nAfter Regex =>".print_r($two, TRUE)."Final =>\n".print_r($three,TRUE).'</pre>', array(), WATCHDOG_NOTICE);
      
    } // end of foreach field
    //watchdog('T_bulk_loader','2)'.$record_array['record_id'].':<pre>'.print_r($default_data[$priority], TRUE).'</pre>', array(), WATCHDOG_NOTICE);
  } //end of foreach record
  
  print "Memory Usage (end of prep work): ".number_format((memory_get_usage() * 0.000000953674316), 5, '.', ',') . " Mb\n";
  
  //print "Default Data:".print_r($default_data,TRUE)."\n";
  //watchdog('T_bulk_loader','Default Data:<pre>'.print_r($default_data, TRUE).'</pre>', array(), WATCHDOG_NOTICE);
  
  //print "\nDefault Values Array: ".print_r($default_data, TRUE)."\n";
  //print "\nField to Column Mapping: ".print_r($field2column, TRUE)."\n";
  
  // Parse File adding records as we go ========================================================
  $file_handle = fopen($node->file, 'r');
  if (preg_match('/(t|true|1)/', $node->file_has_header)) { fgets($file_handle, 4096); }
  $num_records = 0;
  $num_lines = 0;
  $num_errors = 0;
  while (!feof($file_handle)) {

    // Clear variables
    // Was added to fix memory leak
    unset($line);                     unset($raw_line);
    unset($data);                     unset($data_keys);
    unset($priority);                 unset($sql);
    unset($result);                   
    
    $raw_line = fgets($file_handle, 4096);
    $raw_line = trim($raw_line);
    if (empty($raw_line)) { continue; } // skips blank lines
    $line = explode("\t", $raw_line);
    $num_lines++;

    $data = $default_data;

    $data_keys = array_keys($data); 
    foreach ($data_keys as $priority) {
      $status = process_data_array_for_line($priority, $data, $default_data, $field2column, $record2priority, $line, $nid);
      if (!$status ) { $loaded_without_errors = FALSE; }
    } // end of foreach table in default data array

    // determine memory increase
    $line_mem_increase = memory_get_usage() - $memory;
    if ($num_lines != 1) { 
      $increased_mem = $increased_mem + $line_mem_increase;
    }
    $memory = memory_get_usage();
    
  } //end of foreach line of file
  
  // check that data was inserted and update job_status
  $sql = 'SELECT count(*) as num_tables FROM {tripal_bulk_loader_inserted} WHERE nid=%d GROUP BY nid';
  $result = db_fetch_object(db_query($sql, $nid));
  if ($result->num_tables > 0) {
    $node->job_status = 'Data Inserted';
    drupal_write_record('node',$node,'nid');
  }
  
  // set the status of the job (in the node not the tripal jobs)
  if ($loaded_without_errors) { $status = 'Loading Completed Successfully'; } else { $status = 'Errors Encountered'; }
  db_query("UPDATE tripal_bulk_loader SET job_status='%s' WHERE nid=%d",$status, $nid);
  
  $avg_line_increase = ( $increased_mem / $num_lines) * 0.0078125;
  print "Average Increase in Memory per Line: ".number_format($avg_line_increase, 5, '.', ',') . " Kb\n";
  print "Peak Memory Usage: ".number_format((memory_get_peak_usage() * 0.000000953674316), 5, '.', ',') . " Mb\n";
  print "End Memory Usage: ".number_format((memory_get_usage() * 0.000000953674316), 5, '.', ',') . " Mb\n";

}

/**
 * 
 *
 */
function process_data_array_for_line ($priority, &$data, &$default_data, $field2column, $record2priority, $line, $nid) {
  $table_data = $data[$priority];

  $no_errors = TRUE;

  $table = $table_data['table'];
  $values = $table_data['values_array'];
  
  //watchdog('T_bulk_loader','Original:<pre>'.print_r($table_data, TRUE).'</pre>', array(), WATCHDOG_NOTICE);
  
  //print 'default values:'.print_r($values,TRUE)."\n";
  if ($table_data['need_further_processing']) {
    $values = tripal_bulk_loader_add_spreadsheetdata_to_values ($values, $line, $field2column[$priority]);
    if (!$values) {
      watchdog('T_bulk_loader','Spreadsheet Added:'.print_r($values, TRUE), array(), WATCHDOG_NOTICE);
    }
    
    $values = tripal_bulk_loader_add_foreignkey_to_values($values, $data, $record2priority);
    if (!$values) {
      watchdog('T_bulk_loader','FK Added:<pre>'.print_r($values, TRUE).print_r($data[$priority],TRUE).'</pre>', array(), WATCHDOG_NOTICE);
    }
  }
  $values = tripal_bulk_loader_regex_tranform_values($values, $table_data, $line);
  if (!$values) {
    watchdog('T_bulk_loader','Regex:<pre>'.print_r($values, TRUE).print_r($table_data, TRUE).'</pre>'.'</pre>', array(), WATCHDOG_NOTICE);
  }

  if (!$values) {
    $msg = $table_data['record_id'].' ('.$table_data['mode'].') Aborted due to error in previous record. Values of current record:'.print_r($table_data['values_array'],TRUE);
    watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
    print "ERROR: ".$msg."\n";
    $data[$priority]['error'] = TRUE;
    $no_errors = FALSE;
  }
  
  $table_desc = module_invoke_all('chado_'.$table.'_schema');
  if (preg_match('/optional/', $table_array['mode'])) {
    // Check all db required fields are set
    $fields = $table_desc['fields'];
    foreach($fields as $field => $def){
      // a field is considered missing if it cannot be null and there is no default
      // value for it or it is of type 'serial'
      if($def['not null'] == 1 and !array_key_exists($field,$insert_values) and !isset($def['default']) and strcmp($def['type'],serial)!=0){
         $msg = $table_data['record_id'].' ('.$table_data['mode'].') Missing Database Required Value: '.$table.'.'.$field;
         watchdog('T_bulk_loader', $msg, array(), WATCHDOG_NOTICE); 
         $data[$priority]['error'] = TRUE;
      }
    }
  } //end of if optional record
  
  // Check required fields are present
  foreach ($table_data['required'] as $field => $required) {
    if ($required) {
      if (!isset($values[$field])) {
        $msg = $table_data['record_id'].' ('.$table_data['mode'].') Missing Template Required Value: '.$table.'.'.$field;
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
    $header = $values['feature_id']['uniquename'] .' '. $table_data['record_id'];
  } else {
    $header = $values['uniquename'] .' '. $table_data['record_id'];
  }
  
  // if insert unique then check to ensure unique
  if (preg_match('/insert_unique/',$table_data['mode'])) {
    $unique = tripal_core_chado_select($table, array_keys($table_desc['fields']), $values, array('has_record'=>TRUE));
    //print 'Unique?'.print_r(array('table' => $table, 'columns' => array_keys($table_desc['fields']), 'values' => $values),TRUE).' returns '.$unique."\n";
    if ($unique > 0) {
      //$default_data[$priority]['inserted'] = TRUE;
      //watchdog('T_bulk_loader', $header.': Not unique ('.$unique.'):'.print_r($values,'values')."\n".print_r($data,TRUE),array(),WATCHDOG_NOTICE);;
      return $no_errors;
    }
  }
  
    
  if (!preg_match('/select/',$table_data['mode'])) {
    //watchdog('T_bulk_loader',$header.': Inserting:'.print_r($values, TRUE), array(), WATCHDOG_NOTICE);
    $record = tripal_core_chado_insert($table, $values);
    if (!$record) {
      $msg = $table_data['record_id'].' ('.$table_data['mode'].') Unable to insert record into '.$table.' where values:'.print_r($values,TRUE);
      watchdog('T_bulk_loader', $msg, array(), WATCHDOG_ERROR); 
      print "ERROR: ".$msg."\n";  
      $data[$priority]['error'] = TRUE;
      $no_errors = FALSE;
    } else {
      //add changes back to values array
      $data[$priority]['values_array'] = $record;
      $values = $record;
      
      // if mode=insert_once then ensure we only insert it once
      if (preg_match('/insert_once/',$table_data['mode'])) {
        $default_data[$priority]['inserted'] = TRUE;
      }
      
      // add to tripal_bulk_loader_inserted
      $insert_record = db_fetch_object(db_query(
        "SELECT * FROM {tripal_bulk_loader_inserted} WHERE table_inserted_into='%s' AND nid=%d",
        $table,
        $nid
      ));
      if ($insert_record) {
        $insert_record->ids_inserted .= ',' . $values[ $table_desc['primary key'][0] ];
        drupal_write_record('tripal_bulk_loader_inserted', $insert_record, 'tripal_bulk_loader_inserted_id');
        //print 'Update: '.print_r($insert_record,TRUE)."\n";
        return $no_errors;
      } else {
        $insert_record = array(
          'nid' => $nid,
          'table_inserted_into' => $table,
          'table_primary_key' => $table_desc['primary key'][0],
          'ids_inserted' => $values[ $table_desc['primary key'][0] ],
        );
        //print 'New: '.print_r($insert_record,TRUE)."\n";
        $success = drupal_write_record('tripal_bulk_loader_inserted', $insert_record);
        return $no_errors;
      }//end of if insert record
      
    } //end of if insert was successful
  } else {
    $exists = tripal_core_chado_select($table, array_keys($table_desc['fields']), $values, array('has_record'=>TRUE));
    if (!$exists) {
      // No record on select
      $msg = $table_data['record_id'].' ('.$table_data['mode'].') No Matching record in '.$table.' where values:'.print_r($values,TRUE);
      watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
      $data[$priority]['error'] = TRUE;  
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
function tripal_bulk_loader_add_spreadsheetdata_to_values ($values, $line, $field2column) {

  foreach ($values as $field => $value) {
    if (is_array($value)) { continue; }
    
    $column = $field2column[$field] - 1;
    if ($column < 0) { continue; }
    
    if (preg_match('/\S+/',$line[$column])) {
      $values[$field] = $line[$column];
    } else {
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
function tripal_bulk_loader_regex_tranform_values ($values, $table_data, $line) {

  if (empty($table_data['regex_transform']) OR !is_array($table_data['regex_transform'])) { return $values; }
  
  //watchdog('T_bulk_loader','Regex Transformation:<pre>'.print_r($table_data['regex_transform'], TRUE).'</pre>', array(), WATCHDOG_NOTICE);
  
  foreach ($table_data['regex_transform'] as $field => $regex_array) {
    if (!is_array($regex_array['replace'])) { continue; }
    
    //print 'Match:'.print_r($regex_array['pattern'],TRUE)."\n";
    //print 'Replace:'.print_r($regex_array['replace'],TRUE)."\n";
    //print 'Was:'.$values[$field]."\n";
    
    // Check for <#column:\d+#> notation
    // if present replace with that column in the current line
    foreach ($regex_array['replace'] as $key => $replace) {
      if (preg_match_all('/<#column:(\d+)#>/', $replace, $matches)) {
        foreach ($matches[1] as $k => $column_num) {
          $replace = preg_replace('/'.$matches[0][$k].'/', $line[$column_num-1], $replace);
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
function tripal_bulk_loader_flatten_array ($values) {
  $flattened_values = array();
  
  foreach ($values as $k => $v) {
    if (is_array($v)) {
      $vstr = array();
      foreach ($v as $vk => $vv) {
        if (strlen($vv) > 20) {
          $vstr[] = $vk .'=>'. substr($vv, 0, 20) . '...';
        } else {
          $vstr[] = $vk .'=>'. $vv;
        }
      }
      $v = '{'. implode(',',$vstr) .'}';
    } elseif (strlen($v) > 20) {
      $v = substr($v, 0, 20) . '...';
    }
    $flattened_values[] = $k .'=>'. $v;
  }
  
  return implode(', ',$flattened_values);
}