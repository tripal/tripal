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
  
  // get a default values array to be passed into tripal_core_chado_insert
  // and get a mapping between table.field and spreadsheet column
  $default_values_array = array();
  $field2column = array();
  foreach ($node->template->template_array as $table => $table_array) {
    if (is_array($table_array)) {
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
    }
  }
  
  //print "\nDefault Values Array: ".print_r($default_values_array, TRUE)."\n";
  //print "\nField to Column Mapping: ".print_r($field2column, TRUE)."\n";
  
  $file_handle = fopen($node->file, 'r');
  if (preg_match('/(t|true|1)/', $node->file_has_header)) { fgets($file_handle, 4096); }
  while (!feof($file_handle)) {
    $line = array();
    $raw_line = fgets($file_handle, 4096);
    $line = preg_split("/\t/", $raw_line);
    
    $values = $default_values_array;
    foreach ($values as $table => $table_array) {
      foreach ($table_array as $field => $value) {
        $column = $field2column[$table][$field] - 1;
        if ($line[$column]) {
        	$values[$table][$field] = $line[$column];
				}
      }
			$has_record = tripal_core_chado_select($table, array_keys($values[$table]), $values[$table], array('has_record' => TRUE));
			if ($has_record) {
				$values_string = array();
				foreach ($values[$table] as $k => $v) { 
					if (strlen($v) > 20) { $v = substr($v,0,20) . '...'; }
					$values_string[] = $k.' => '.$v; 
				}
				print "\tWARNING: Record already exists in $table where ".implode(', ',$values_string).".\n";
			} else {
	      $success = tripal_core_chado_insert($table, $values[$table]);
  	    if (!$success) {
    	  	print "ERROR: Unable to insert the following record into $table: ".print_r($values[$table], TRUE)."\n";
      	}
      }
    } //end of tables in $values

  } //end of file
  
}