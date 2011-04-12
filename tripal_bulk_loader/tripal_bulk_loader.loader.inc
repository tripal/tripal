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
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit Job'
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
			tripal_add_job("Bulk Loading Job: $fname",'tripal_bulk_loader', 'tripal_bulk_loader_load_data', $job_args, $user->uid);
		} else {
			drupal_set_message("Can not open ".$form_state['values']['file'].". Job not scheduled.");
		}    
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
  // get base table and it's table_description array
  $base_table = $node->template->template_array['module'];
  $base_table_desc = module_invoke_all('chado_'.$base_table.'_schema');
  $related_tables = module_invoke_all('tripal_bulk_loader_'.$base_table.'_related_tables');
  //print "Base Table: ".print_r($base_table,TRUE)."\nTable DEscription: ".print_r($base_table_desc, TRUE)."\nTemplate array: ".print_r($node->template->template_array,TRUE)."\n";

  // get a default values array to be passed into tripal_core_chado_insert
  // and get a mapping between table.field and spreadsheet column
  // also determine relationship between each table and base table
  $default_values_array = array();
  $field2column = array();
  $relationships2base = array();
  $all_tables = array();
  foreach ($node->template->template_array as $table => $table_array) {
    if (is_array($table_array)) {
      $all_tables[$table] = $table;
    
      foreach ($table_array['field'] as $field_array) {
        if (preg_match('/table field/', $field_array['type'])) {
          $default_values_array[$table][$field_array['field']] = '';
          $field2column[$table][$field_array['field']] = $field_array['spreadsheet column'];
        } elseif (preg_match('/constant/', $field_array['type'])) {
          $default_values_array[$table][$field_array['field']] = $field_array['constant value'];
        } else {
          print 'WARNING: Unsupported type: '. $field_array['type'] . ' for ' . $table . '.' . $field_array['field']."!\n";
        }
      }
      
      // Determine what relation is between this table and the base table-----------------------
      // This is used later to link the various records
      //print "===\n";
      // Is there a foreign key to this table in the base table? 
      // ie: feature.type_id (base) for cvterm (current)
      //print "A) Is there fkey to ".$table." table in ".$base_table."?\n";
      if ($related_tables['Foreign Key Relations'][$table]) {
          //print "YES!\n";
          $relationships2base['foreign key'][$table] = $base_table_desc['foreign keys'][$table];
      }
      
      // Is there a foreign key in this table to the base table? 
      // ie: featureloc.feature_id (current) for feature (base)
      
      //print "B) Does ".$table." contain a fkey to ".$base_table."?\n";
      if ($related_tables['Direct Relations'][$table]) {
          $table_desc = module_invoke_all('chado_'.$table.'_schema');
          //print "YES!\n";
          $relationships2base['direct'][$table] = $table_desc['foreign keys'][$base_table];
      }
      
      // Is there a linking table which links this table with the base table
      // ie: analysisfeature (current) links analysis and feature (base)
      //print "C) Is ".$table." a linking table?\n";
      if ($related_tables['Indirect Relations'][$table]) {
        //print "YES!\n";
        $table_desc = module_invoke_all('chado_'.$table.'_schema');
        $relationships2base['indirect'][$table] = $table_desc['foreign keys'];
      }

    }
  }
  
  //print "\nDefault Values Array: ".print_r($default_values_array, TRUE)."\n";
  //print "\nField to Column Mapping: ".print_r($field2column, TRUE)."\n";
  //print "\nRelationships to Base Table: ".print_r($relationships2base, TRUE)."\n";
  
  // Parse File adding records as we go ========================================================
  $file_handle = fopen($node->file, 'r');
  if (preg_match('/(t|true|1)/', $node->file_has_header)) { fgets($file_handle, 4096); }
  $num_records = 0;
  $num_lines = 0;
  $num_errors = 0;
  while (!feof($file_handle)) {
    $line = array();
    $raw_line = fgets($file_handle, 4096);
    $line = preg_split("/\t/", $raw_line);
    $num_lines++;
    
    // Contains constants set above
    $tables = $all_tables;
    $values = $default_values_array;
    
    // Insert base record-----------------------------------------------------------------------
    foreach ($relationships2base['foreign key'] as $table => $desc) {
      // Ensure foreign key is present
      $values[$table] = tripal_bulk_loader_supplement_values_array ($values[$table], $line, $field2column[$table]);
      $success = tripal_bulk_loader_ensure_record ($table, $values[$table]);   
      if ($success) {
        $num_records++;
      } else {
        $num_errors++;
      }
      
      //Add to base values array
      foreach ($desc['columns'] as $foreign_key => $primary_key) {
        $values[$base_table][$foreign_key] = $values[$table];
      }
      
      unset($tables[$table]);
    }
    
    // base table
    $values[$base_table] = tripal_bulk_loader_supplement_values_array ($values[$base_table], $line, $field2column[$base_table]);
    $success = tripal_bulk_loader_ensure_record ($base_table, $values[$base_table]);     
    //print 'Base Table values array: '.print_r($values[$base_table], TRUE)."\n";
    if (!$success) {
      print "ERROR: Unable to insert base record where base table: ".$base_table.
        " and values array: ".tripal_bulk_loader_flatten_array($values[$base_table])."\n";
      $num_errors++;
    } else {
      $num_records++;
    }
    unset($tables[$base_table]);
    
    // Insert all tables with direct relationship to base ----------------------------------------
    foreach ($relationships2base['direct'] as $table => $desc) {
      //Add base to values array
      foreach ($desc['columns'] as $foreign_key => $primary_key) {
        $values[$table][$foreign_key] = $values[$base_table];
      }

      // Supplement and Add record
      $values[$table] = tripal_bulk_loader_supplement_values_array ($values[$table], $line, $field2column[$table]);
      $success = tripal_bulk_loader_ensure_record ($table, $values[$table]);
      if ($success) {
        $num_records++;
      } else {
        $num_errors++;
      }
      unset($tables[$table]);
    } 
    
    // Add in all other tables -----------------------------------------------------------------
    foreach ($tables as $table) {
      // Don't insert if its an indirect relationship linking table
      if (!$relationships2base['indirect'][$table]) {
        // Supplement and Add record
        $values[$table] = tripal_bulk_loader_supplement_values_array ($values[$table], $line, $field2column[$table]);
        $success = tripal_bulk_loader_ensure_record ($table, $values[$table]);    
        if ($success) {
          $num_records++;
        } else {
          $num_errors++;
        }
      }
    }
    
    // Add in indirect relationships -----------------------------------------------------------
    foreach ($relationships2base['indirect'] as $table => $desc) {
      // Add foreign keys to values array
      foreach ($desc as $subtable => $subdesc) {
        foreach ($subdesc['columns'] as $foreign_key => $primary_key) {
          $values[$table][$foreign_key] = $values[$subtable];
        }
      }
      
      // Supplement and Add record
      $values[$table] = tripal_bulk_loader_supplement_values_array ($values[$table], $line, $field2column[$table]);
      $success = tripal_bulk_loader_ensure_record ($table, $values[$table]);      
      if ($success) {
        $num_records++;
      } else {
        $num_errors++;
      }
    }
    
  } //end of file
  
  print "\nNumber of Records Inserted:".$num_records."\n";
  print "Number of Lines in File:".$num_lines."\n";
  print "Number of Errors: ".$num_errors."\n\n";
  
  if (!$num_errors) {
    db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'successfully loaded', $nid);
  } else {
    db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'load attempted', $nid);
  }
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
function tripal_bulk_loader_supplement_values_array ($values, $line, $field2column) {

  foreach ($values as $field => $value) {
    $column = $field2column[$field] - 1;
    if ($line[$column] OR (!$values[$field])) {
      $values[$field] = $line[$column];
    }
  }
  
  return $values;
}

/**
 * This function first ensures the record doesn't already exist and then inserts it
 *
 * @param $table
 *   The table the record should be present in
 * @param $values
 *   The values array used for selecting and/or inserting the record
 * @return
 *   TRUE or FALSE based on presence or absence of record
 */
function tripal_bulk_loader_ensure_record ($table, $values) {
  
  // get flattened values array for printing errors
  $flattened_values = tripal_bulk_loader_flatten_array($values);
  
  // check if record exists
  $has_record = tripal_core_chado_select($table, array_keys($values), $values, array('has_record' => TRUE));
  if ($has_record) {
    print "\tWARNING: Record already exists in $table where ".implode(',',$flattened_values).".\n";
    return true;
  } else {
    // if record doesn't exist then insert it
    $success = tripal_core_chado_insert($table, $values);
    if (!$success) {
      print "ERROR: Unable to insert the following record into $table: ".implode(',',$flattened_values)."\n";
      return false;
    } else {
      return true;
    }
  }
      
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
  
  return $flattened_values;
}