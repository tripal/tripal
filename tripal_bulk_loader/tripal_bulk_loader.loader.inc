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
      
		} else {
			drupal_set_message("Can not open ".$form_state['values']['file'].". Job not scheduled.");
		}    
  } elseif (preg_match('/Re-Submit Job/', $form_state['values']['op'])) {
    tripal_jobs_rerun($form_state['values']['job_id']);
  } elseif (preg_match('/Cancel Job/', $form_state['values']['op'])) {
    tripal_jobs_cancel($form_state['values']['job_id']);
  }

}

/**
 * Tripal Bulk Loader
 *
 * This is the function that's run by tripal_launch_jobs to bulk load chado data.
 * @param $nid
 *   The Node ID of the bulk loading job node to be loaded. All other needed data is expected to be 
 *   in the node (ie: template ID and file)
 *
 * Note: Instead of returning a value this function updates the tripal_bulk_loader.status and
 *   Enters errors into tripal_bulk_loader_errors if they are encountered.
 */
function tripal_bulk_loader_load_data($nid) {
  
  $node = node_load($nid);
  print "Template: ".$node->template->name." (".$node->template_id.")\n";
  print "File: ".$node->file."\n";
  
  // Prep Work ==================================================================================
  // Generate default values array
  $default_data = array();
  $field2column = array();
  $record2priority = array();
  foreach ($node->template->template_array as $priority => $record_array) {
    if (!is_array($record_array)) { continue; }
    
    foreach ($record_array['fields'] as $field_index => $field_array) {
      $default_data[$priority]['table'] = $record_array['table'];
      $default_data[$priority]['mode'] = ($record_array['mode']) ? $record_array['mode'] : 'insert';
      $default_data[$priority]['record_id'] = $record_array['record_id'];
      $record2priority[$record_array['record_id']] = $priority;
      $default_data[$priority]['required'][$field_array['field']] = $field_array['required'];
      
      if (isset($field_array['regex'])) {
        $default_data[$priority]['regex_transform'][$field_array['field']] = $field_array['regex'];
      }
      
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
    } // end of foreach field
  } //end of foreach record

  //print "\nDefault Values Array: ".print_r($default_data, TRUE)."\n";
  //print "\nField to Column Mapping: ".print_r($field2column, TRUE)."\n";
  
  // Parse File adding records as we go ========================================================
  $file_handle = fopen($node->file, 'r');
  if (preg_match('/(t|true|1)/', $node->file_has_header)) { fgets($file_handle, 4096); }
  $num_records = 0;
  $num_lines = 0;
  $num_errors = 0;
  while (!feof($file_handle)) {
    $line = array();
    $raw_line = fgets($file_handle, 4096);
    $raw_line = trim($raw_line);
    if (preg_match('/^\s*$/', $raw_line)) { continue; } // skips blank lines
    $line = preg_split("/\t/", $raw_line);
    $num_lines++;

    $data = $default_data;
    
    foreach ($data as $priority => $table_data) {
      $table = $table_data['table'];
      $values = $table_data['values_array'];
      if ($table_data['need_further_processing']) {
        $values = tripal_bulk_loader_add_spreadsheetdata_to_values ($values, $line, $field2column[$priority]);
        $values = tripal_bulk_loader_add_foreignkey_to_values($values, $data, $record2priority);
      }
      $values = tripal_bulk_loader_regex_tranform_values($values, $table_data, $line);

      if (!$values) {
        $msg = $table_data['record_id'].' ('.$table_data['mode'].') Aborted due to error in previous record.';
        watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
        print "ERROR: ".$msg."\n";
        $data[$priority]['error'] = TRUE;
      }
      
      if (preg_match('/optional/', $table_array['mode'])) {
        // Check all required fields are set
        $table_desc = module_invoke_all('chado_'.$table.'_schema');
        $fields = $table_desc['fields'];
        foreach($fields as $field => $def){
          // a field is considered missing if it cannot be null and there is no default
          // value for it or it is of type 'serial'
          if($def['not null'] == 1 and !array_key_exists($field,$insert_values) and !isset($def['default']) and strcmp($def['type'],serial)!=0){
             $msg = $table_data['record_id'].' ('.$table_data['mode'].') Missing Database Required Value: '.$table.'.'.$field;
             watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
            print "ERROR: ".$msg."\n";
            $data[$priority]['error'] = TRUE;
          }
        }
      } //end of if optional record
      
      // Check required fields are present
      foreach ($table_data['required'] as $field => $required) {
        if ($required) {
          if (!isset($values[$field])) {
            $msg = $table_data['record_id'].' ('.$table_data['mode'].') Missing Template Required Value: '.$table.'.'.$field;
            watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
            print "ERROR: ".$msg."\n";
            $data[$priority]['error'] = TRUE;            
          }
        }
      }
      
      if ($data[$priority]['error']) {
        continue;
      }
      
      // add new values array into the data array
      $data[$priority]['values_array'] = $values;
       
      // first check if it already exists
      $exists = tripal_core_chado_select($table, array_keys($values), $values, array('has_record'=>TRUE));
      //print "Number of Records:".$exists.".\n";
      if ($exists === 1) {
        if (!preg_match('/select/',$table_data['mode'])) {
          $msg = $table_data['record_id'].' ('.$table_data['mode'].') Record already exists in '.$table.' where values:'.print_r($values,TRUE);
          watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
          print "ERROR: ".$msg."\n";
          $data[$priority]['error'] = TRUE;
        }
      } elseif ($exists > 1) { 
          $msg = $table_data['record_id'].' ('.$table_data['mode'].') 2+ Records exists in '.$table.' where values:'.print_r($values,TRUE);
          watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
          print "ERROR: ".$msg."\n";   
          $data[$priority]['error'] = TRUE;
      } else {
        // if it doesn't exist already then insert it
        if (!preg_match('/select/',$table_data['mode'])) {
          $success = tripal_core_chado_insert($table, $values);
          if (!$success) {
            $msg = $table_data['record_id'].' ('.$table_data['mode'].') Unable to insert record into '.$table.' where values:'.print_r($values,TRUE);
            watchdog('T_bulk_loader', $msg, array(), WATCHDOG_ERROR); 
            print "ERROR: ".$msg."\n";  
            $data[$priority]['error'] = TRUE;
          } else {
            //print "Inserted ".$table_data['record_id']." into ".$table."\n";
          }
        } else {
          $msg = $table_data['record_id'].' ('.$table_data['mode'].') No Matching record in '.$table.' where values:'.print_r($values,TRUE);
          watchdog('T_bulk_loader', $msg, array(), WATCHDOG_WARNING); 
          print "ERROR: ".$msg."\n";
          $data[$priority]['error'] = TRUE;        
        }
      }// end of if/not record exists
    } // end of foreach table in default data array

  } //end of foreach line of file
  
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
    if (preg_match('/\w+/',$line[$column])) {
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
      // only if there was no error found when the foreign record was processed
      if (isset($data[$foreign_priority]['error']) and $data[$foreign_priority]['error']) {
        return FALSE;
      } else {
        $values[$field] = $foreign_values;
      }
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

  foreach ($table_data['regex_transform'] as $field => $regex_array) {
    if (!is_array($regex_array)) { continue; }
    
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