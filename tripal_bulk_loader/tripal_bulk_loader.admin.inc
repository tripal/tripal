<?php

/**
 * @file
 * Handles Create/Edit/Delete Template Admin Forms
 */
 
/**
 * Provides a description page and quick links for the bulk loader
 */
function tripal_bulk_loader_admin_template () {
	$output = '';
	
	$output .= '<br><h3>Quick Links:</h3>';
  $output .= l('Create a new bulk loader template', 'admin/tripal/tripal_bulk_loader_template/create')."<br>"; 
  $output .= l('Edit a bulk loader template', 'admin/tripal/tripal_bulk_loader_template/edit')."<br>"; 
  $output .= l('Delete a bulk loader template', 'admin/tripal/tripal_bulk_loader_template/delete')."<br>"; 
  $output .= l('Export a bulk loader template', 'admin/tripal/tripal_bulk_loader_template/export')."<br>";
  $output .= l('Import a bulk loader template', 'admin/tripal/tripal_bulk_loader_template/import')."<br>";
  $output .= '<br>';
  
  $output .= '<h3>Module Description:</h3>';
  $output .= '<p>This module provides the ability to create loading templates for any tab-delimited '
    .'data file allowing it to be loaded into chado. The Loading Templates are a direct mapping '
    .'between the columns in your file and the columns in chado tables. As such to use this tool '
    .'you need to be very familar with the chado schema -See '
    .l('Chado -Getting Started', 'http://gmod.org/wiki/Chado_-_Getting_Started')
    .'. The ability to add constants and specify foreign key contraints is also provided '
    .'in order for the loader to fill chado columns which may be required but are not specified '
    .'in your input file.</p>';
  $output .= '<br>';
  
  $output .= '<h3>Setup Instructions</h3>';
  $output .= '<p>After intallation of the bulk loader module, the following tasks should be performed:</p>';
  $output .= '<ol>';
  $output .= '<li><b>Install Theme:</b> In order for Bulk Loading pages to be displayed correctly, '
    .'the contents of the Tripal Bulk Loader theme directory ([drupal root]/sites/all/modules/tripal/tripal_bulk_loader/theme) '
    .'should be moved to the base directory of the Tripal theme ([drupal root]/sites/all/themes/tripal). '
    .'Finally the drupal cache should be cleared for the new theme to take effect -navigate to admin/settings/performance '
    .'and click the Clear Cached Data button.</li>';
  $output .= '</ol>';
  
  return $output;
}


//////////////////////////////////////////////////////////////////////////////////////
// Modify Template
//////////////////////////////////////////////////////////////////////////////////////


/**
 * The main form reached at admin/tripal/tripal_bulk_loader/create and /edit
 */
function tripal_bulk_loader_modify_template_base_form ($form_state = NULL, $mode){
	$form = array();

 	// get template id from path and rebuild form
 	if ($_GET['template_id']) {
    if (preg_match('/^\d+$/', $_GET['template_id'])) {
      $form_state['storage']['template_id'] = $_GET['template_id'];
    }
    
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $result = db_fetch_object(db_query($sql, $form_state['storage']['template_id']));
    $form_state['storage']['template'] = unserialize($result->template_array);
    $form_state['storage']['template_name'] = $result->name;
  
    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template'] as $priority => $record_array) {
      if (!is_array($record_array)) { continue; }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }
  }

  $form['mode'] = array(
    '#type' => 'hidden',
    '#value' => $mode,
  );

  if ($form_state['storage']['template_id']) {    
    $form['template_name'] = array(
        '#type' => 'item',
        '#title' => 'Template',
        '#value' => $form_state['storage']['template_name'],
        '#weight' => 1,
    );
  } else {
    if (preg_match('/create/', $mode)) {
     $form['new_template_name'] = array(
        '#type' => 'textfield',
        '#title' => 'Template Name',
        '#weight' => 1,
      );
    } elseif (preg_match('/edit/', $mode)) {
      $sql = "SELECT * FROM {tripal_bulk_loader_template}";
      $resource = db_query($sql);
      $templates = array();
      $templates[''] = 'Select a Template';
      while ($r = db_fetch_object($resource)) {
        $templates[$r->template_id] = $r->name;
      }
      
      $form['template_id'] = array(
        '#title'         => t('Template'),
        '#description'   => t('Please select the template you would like to edit.'),
        '#type'          => 'select',
        '#options'       => $templates,
        '#default_value' => $form_state['storage']['template_id'],
        '#weight'        => 0,
        '#required'      => TRUE,
        '#weight' => 1,
      );	
    }
  }

  $form['records'] = array(
		'#type' => ($form_state['storage']['template_id'])? 'fieldset' : 'hidden',
		'#title' => t('Current Records'), 
		'#weight' => 2,
  );
  
  $form['records']['description'] = array(
    '#type' => 'item',
    '#value' => 'Records will be inserted into the chado database in the order listed below. To '
      .'change this order: <ul><li>Drag the rows into the correct order OR</li><li>Enter '
      .'the numbers 1 and up in the Order textboxes to indicate the correct order.</li></ul>',
  );
  
  $form['records']['records-data'] = array(
    '#tree' => TRUE,
  );

  $form['records']['no_records'] = array(
    '#type' => 'hidden',
    '#value' => TRUE,
  );

  $form['records']['submit-new_record'] = array(
    '#type' => 'submit',
    '#value' => 'New Record/Field',
  );

  $form['records']['submit-reorder'] = array(
    '#type' => 'submit',
    '#value' => 'Save Order',
  );
	
	$form['fields'] = array(
		'#type' => ($form_state['storage']['template_id'])? 'fieldset' : 'hidden',
		'#title' => t('Current Fields'),
		'#weight' => 3,
	);

  $form['fields']['fields-data'] = array(
    '#tree' => TRUE,
  );
  
	if ($form_state['storage']['template']) {	
    
    // List Current Fields -------------------------------------------------------------
    $i=1;
    foreach ($form_state['storage']['template'] as $priority => $table_array) {
      if (!is_array($table_array)) { continue; }
        
        $form['records']['no_records']['#value'] = FALSE;
        
        $form['records']['records-data'][$priority] = array(
          'title' => array(
            '#type' => 'markup',
            '#value' => $table_array['record_id'],
          ),
          'chado_table' => array(
            '#type' => 'markup',
            '#value' => $table_array['table'],
          ),
          'mode' => array(
            '#type' => 'item',
            '#value' => ($table_array['mode']) ? $table_array['mode'] : 'insert_unique',
          ),
          'new_priority' => array(
            '#type' => 'select',
            '#options' => range(1, sizeof($form_state['storage']['template'])),
            '#default_value' => $priority,
          ),
          'old_priority' => array(
            '#type' => 'hidden',
            '#value' => $priority,
          ),
          'id'  => array(
            '#type' => 'hidden',
            '#value' => $priority,
          ),
          'submit-edit_record' => array(
            '#type' => 'submit',
            '#name' => (string)$priority,
            '#value' => 'Edit Record',
          ),          
          'submit-add_field' => array(
            '#type' => 'submit',
            '#name' => (string)$priority,
            '#value' => 'Add Field',
          ),
          'submit-duplicate_record' => array(
            '#type' => 'submit',
            '#name' => (string)$priority,
            '#value' => 'Duplicate Record'
          ),
        );

        foreach ($table_array['fields'] as $field_index => $field) {
        
          $form['fields']['fields-data'][$i] = array(
            'record_id' => array(
              '#type' => 'item',
              '#value' => $table_array['record_id'],
            ),
            'priority_hidden' => array(
              '#type' => 'hidden',
              '#value' => $priority,
            ),
            'field_name' => array(
              '#type' => 'item',
              '#value' => $field['title'],
            ),
            'chado_table_name' => array(
              '#type' => 'item',
              '#value' => $table_array['table'],
            ),		
            'chado_table_hidden' => array(
              '#type' => 'hidden',
              '#value' => $table_array['table'],
            ),
            'chado_field_name' => array(
              '#type' => 'item',
              '#value' => $field['field'],
            ),
            'sheet_name' => array(
              '#type' => 'item',
              '#value' => $field['spreadsheet sheet'],
            ),
            'column_num' => array(
              '#type' => 'item',
              '#value' => $field['spreadsheet column'],
            ),
            'constant_value' => array(
              '#type' => 'item',
              '#value' => $field['constant value'],
            ),          
            'field_index' => array(
              '#type' => 'hidden',
              '#value' => $field_index
            ),
            'foreign_record_id' => array(
              '#type' => 'item',
              '#value' => $field['foreign key'],
            ),
            'edit_submit' => array(
              '#type' => 'submit',
              '#name' => (string)$i,
              '#value' => "Edit Field",
            ),
            'delete_submit' => array(
              '#type' => 'submit',
              '#name' => (string)$i,
              '#value' => "Delete Field",
            ),
          );
          
          $i++;
        }
    }
    $form['fields']['total_fields'] = array(
      '#type' => 'item',
      '#value' => $i,
    );

	}

  if ($form['records']['no_records']['#value']) {
    $form['records']['description'] = array(
      '#type' => 'item',
      '#value' => 'There are currently no records.',
    );    
    unset($form['records']['submit-reorder']);
  
    $form['fields']['description'] = array(
      '#type' => 'item',
      '#value' => 'There are currently no fields.',
    );
    
  }
    
  $mode_title = (preg_match('/create/',$mode)) ? 'Create Template' : 'Edit Template';
	$value = ($form_state['storage']['template_id'])? 'Save Template' : $mode_title;
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => $value,
		'#weight' => 4,
	);
	
	return $form;
}

/**
 * Submit for tripal_bulk_loader_modify_template_base_form
 */
function tripal_bulk_loader_modify_template_base_form_submit($form, &$form_state){

  $form_state['rebuild'] = TRUE;
  if ($form_state['storage']['template_id']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $result = db_fetch_object(db_query($sql, $form_state['storage']['template_id']));
    $form_state['storage']['template'] = unserialize($result->template_array);
  }
  
  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  switch ($op) {
    // Initialize after template is chosen ----------------------------------------
    case 'Edit Template':
      $form_state['storage']['template_id'] = $form_state['values']['template_id'];
      
      $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
      $result = db_fetch_object(db_query($sql, $form_state['storage']['template_id']));
      $form_state['storage']['template'] = unserialize($result->template_array);
      $form_state['storage']['template_name'] = $result->name;
    
      $form_state['storage']['record2priority'] = array();
      foreach ($form_state['storage']['template'] as $priority => $record_array) {
        if (!is_array($record_array)) { continue; }
        $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
      }
    break;
    
    case 'Create Template':
      $record = array(
        'name' => $form_state['values']['new_template_name'],
        'template_array' => array(),
      );
      drupal_write_record('tripal_bulk_loader_template', $record);
      $form_state['storage']['template_id'] = $record['template_id'];
      $form_state['storage']['template_name'] = $record['name'];
      $form_state['storage']['template'] = array();
    break;
    
    // Save Reordered Records -----------------------------------------------------
    case 'Save Order':
      $new_template = $form_state['storage']['template'];
      // unset old elements
       $form_state['storage']['record2priority'] = array();
      foreach ($new_template as $priority => $record_array) { 
        if (preg_match('/\d+/', $priority)) { unset($new_template[$priority]); }
      }
      //set elements in new order
      foreach ($form_state['values']['records-data'] as $item) {
        $new_template[$item['new_priority']] = $form_state['storage']['template'][$item['old_priority']];
        $record_name = $new_template[$item['new_priority']]['record_id'];
        $form_state['storage']['record2priority'][$record_name] = $item['new_priority'];
      }
      ksort($new_template);
      $form_state['storage']['template'] = $new_template;
    break;
    
    case 'New Record/Field':
      $query = array(
        'template_id'=> $form_state['storage']['template_id'], 
        'record_id' => 'NEW',
      );
      drupal_goto('admin/tripal/tripal_bulk_loader_template/add_field', $query);    
    break;

    case 'Edit Record':
      $query = array(
        'template_id'=> $form_state['storage']['template_id'], 
        'record_id' => $form_state['clicked_button']['#name'],
      );
      drupal_goto('admin/tripal/tripal_bulk_loader_template/edit_record', $query);
    break;
    
    case 'Add Field':
      $query = array(
        'template_id'=> $form_state['storage']['template_id'], 
        'record_id' => $form_state['clicked_button']['#name'],
      );
      drupal_goto('admin/tripal/tripal_bulk_loader_template/add_field', $query);
    break;

    case 'Duplicate Record':
      // original record (one to be duplicated)
      $orig_priority = $form_state['clicked_button']['#name'];
      $record = $form_state['storage']['template'][ $orig_priority ];
      
      // new record
      $new_priority = sizeof($form_state['storage']['template']) + 1;
      $record['record_id'] = $record['record_id'] . '_' . date('YzHi');
      $form_state['storage']['template'][ $new_priority ] = $record;
    break;
    
    case 'Edit Field':
      $field_data_index = $form_state['clicked_button']['#name'];
      $query = array(
        'template_id'=> $form_state['storage']['template_id'], 
        'record_id' => $form_state['values']['fields-data'][$field_data_index]['priority_hidden'],
        'field_index' => $form_state['values']['fields-data'][$field_data_index]['field_index'],
      );
      drupal_goto('admin/tripal/tripal_bulk_loader_template/edit_field', $query);    
    break;
    
    case 'Delete Field':
      $field_data = $form_state['values']['fields-data'][$form_state['clicked_button']['#name']];
      $priority = $field_data['priority_hidden'];
      $field_key = $field_data['field_index'];
      unset($form_state['storage']['template'][$priority]['fields'][$field_key]);
      if (!$form_state['storage']['template'][$priority]['fields']) {
        unset($form_state['storage']['record2priority'][$form_state['storage']['template'][$priority]['record_id']]);
        unset($form_state['storage']['template'][$priority]);
      } 
      drupal_set_message('Deleted Field from Template.');
    break;
  } //end of switch
    
  // Save Template
  $record = array(
    'template_id' => $form_state['storage']['template_id'],
    'template_array' => serialize($form_state['storage']['template'])
  );
  drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
  drupal_set_message('Template Saved.');
	
}

//////////////////////////////////////////////////////////////////////////////////////
// Delete Template
//////////////////////////////////////////////////////////////////////////////////////

/**
 * Delete Template Form
 * This form allows admin to delete already existing templates
 */
function tripal_bulk_loader_delete_template_base_form () {
	$form = array();

	$sql = "SELECT * FROM {tripal_bulk_loader_template}";
	$resource = db_query($sql);
	$templates = array();
	$templates[''] = 'Select a Template';
	while ($r = db_fetch_object($resource)) {
		$templates[$r->template_id] = $r->name;
	}
  $form['template_name'] = array(
      '#title'         => t('Template'),
      '#description'   => t('Please select the template you would like to delete.'),
      '#type'          => 'select',
      '#options'       => $templates,
      '#weight'        => 0,
      '#required'      => TRUE,
	);	
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Delete Template',
	);
	
	return $form;
}

/**
 * Delete Template Form Submit
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage that were submitted
 */
function tripal_bulk_loader_delete_template_base_form_submit ($form, &$form_state) {
	$sql = "DELETE FROM {tripal_bulk_loader_template} WHERE template_id=%d";
	db_query($sql, $form_state['values']['template_name']);
}

//////////////////////////////////////////////////////////////////////////////////////
// Import/Export Template
//////////////////////////////////////////////////////////////////////////////////////

/**
 * Import/Export Template Form
 *
 * On export, simply selects the serialized array from the db for a given template 
 * and presents it to the user. On import, a serialized template array and a name is 
 * supplied and a template record is created.
 *
 * @todo Make array presented to the user more readable. (ie: unserialize and print to the screen)
 *
 * @param $form_state
 *   The values and storage for the form
 * @param $mode
 *   Either 'import' or 'export' to indicate which function is being performed
 * @return
 *   A form array to be rendered by drupal_get_form
 */
function tripal_bulk_loader_import_export_template_form ($form_state = NULL, $mode) {
  $form = array();

  $form['mode'] = array(
    '#type' => 'hidden',
    '#value' => $mode,
  );
  
  if (preg_match('/import/', $mode)) {
   $form['new_template_name'] = array(
      '#type' => 'textfield',
      '#title' => 'Template Name',
      '#weight' => 1,
    );
  } elseif (preg_match('/export/', $mode)) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template}";
    $resource = db_query($sql);
    $templates = array();
    $templates[''] = 'Select a Template';
    while ($r = db_fetch_object($resource)) {
      $templates[$r->template_id] = $r->name;
    }
    
    $form['template_id'] = array(
      '#title'         => t('Template'),
      '#description'   => t('Please select the template you would like to edit.'),
      '#type'          => 'select',
      '#options'       => $templates,
      '#default_value' => $form_state['storage']['template_id'],
      '#weight'        => 0,
      '#required'      => TRUE,
      '#weight' => 1,
    );	
  }

  $form['template_array'] = array(
    '#type' => 'textarea',
    '#title' => 'Template Array',
    '#default_value' => $form_state['storage']['template_array'],
    '#weight' => 2,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#weight' => 10,
  );
  
  return $form;
}

/**
 * Import/Export Template Form Submit
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage that were submitted
 */
function tripal_bulk_loader_import_export_template_form_submit ($form, &$form_state) {
  switch ($form_state['values']['mode']) {
    case 'export':
      $record = db_fetch_object(db_query("SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d", $form_state['values']['template_id']));
      $form_state['storage']['template_array'] = $record->template_array;
      $form_state['storage']['template_id'] = $form_state['values']['template_id'];
    break;
    case 'import':
      $record = array(
        'name' => $form_state['values']['new_template_name'],
        'template_array' => $form_state['values']['template_array'],
      );
      drupal_write_record('tripal_bulk_loader_template', $record);
      if ($record->template_id) {
        drupal_set_message('Successfully imported Tripal Bulk Loader Template.');
      }
    break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Edit Record Form
//////////////////////////////////////////////////////////////////////////////////////

/**
 * Edit Record Form
 *
 * This form is meant to be called from a bulk loader form. The following should be set 
 * in the query section of the path:
 *  - template_id=\d+: the template which the edited record is part of
 *  - record_id=\d+: the priority or key in the template array of the record to be edited
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 */
function tripal_bulk_loader_edit_template_record_form (&$form_state = NULL) {
 	$form['#cache'] = TRUE; // Make sure the form is cached.
  
 	// get template id from path
 	$template_id = ($_GET['template_id'] !== NULL) ? $_GET['template_id'] : $form_state['values']['template_id'];
 	
 	// if there is no template supplied don't return rest of form
 	if (!$template_id) {
 	  return $form;
 	}

  // Pre-process values/defaults ---------------------------

 	// If this is the first load of the form (no form state) we need to initialize some variables
 	if (!$form_state['storage']['template']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $template = db_fetch_object(db_query($sql, $template_id));
    $form_state['storage']['template_array'] = unserialize($template->template_array);
    $form_state['storage']['template'] = $template;
    
    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template_array'] as $priority => $record_array) {
      if (!is_array($record_array)) { continue; }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }
    
    $form_state['storage']['referring URL'] = $_SERVER["HTTP_REFERER"];
  } else {
    $template = $form_state['storage']['template'];
  }

 	// get the record_id from the path
 	if ($_GET['record_id'] !== NULL) {
   	$form_state['values']['field_group'] = $_GET['record_id'];
   	$form_state['storage']['original_priority'] = $_GET['record_id'];
  }
  
  
  // Tables and default table
  $tables = tripal_core_get_chado_tables();
  if ($form_state['values']['chado_table']) {
    $table = $form_state['values']['chado_table'];
  } else {
    $table = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['table'];  
  }

  //dpm($form_state, 'form state');
  
 	// Form Proper ------------------------------------------- 	
  $form['template_name'] = array(
    '#type' => 'item',
    '#title' => 'Template',
    '#value' => $template->name,
  );

  $form['template_id'] = array(
    '#type' => 'hidden',
    '#value' => $template_id,
  );
  
  $form['edit_record'] = array(
    '#type' => 'fieldset',
  );

  // check template array for records then add one more
  if (!$form_state['storage']['record2priority']) { 
    $groups = array();
  } else {
    $groups = array_flip($form_state['storage']['record2priority']);
  }
  $priority_default = $form_state['values']['field_group'];
  $form['edit_record']['field_group']  = array(
    '#type' => 'select',
    '#title' => 'Record',
    '#description' => 'By Changing the record here, you can move all the fields from the current record into the selected record.',
    '#options' => $groups,
    '#default_value' => $priority_default,
    '#required' => TRUE,
  );
  
  $form['edit_record']['record_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Unique Record Name',
    '#default_value' => $groups[$priority_default],
  );

  $form['edit_record']['chado_table'] = array(
    '#type' => 'select',
    '#title' => t('Chado Table'),
    '#description' => 'This changes the chado table for all fields in this record.',
    '#options' => $tables,
    '#default_value' => $table,
  );
  
  $form['edit_record']['mode'] = array(
    '#type' => 'radios',
    '#title' => 'Action to take when Loading Record',
    '#options' => array(
      'select' => 'SELECT: Don\'t insert this record: it\'s used to define a foreign key in another record',
      'insert' => 'INSERT: Insert the record',
      'optional' => 'OPTIONAL: Record will only be inserted if all required data is filled in',
      'insert_once' => 'INSERT ONCE: Record will be inserted once for the entire spreadsheet',
      'insert_unique' => 'INSERT UNIQUE: Only insert record if there isn\'t a record with the same values',
    ),
    '#default_value' => 'insert_unique'
  );
  
  $form['edit_record']['submit-edit_record'] = array(
      '#type' => 'submit',
      '#value' => 'Edit Record'
  );

  $form['edit_record']['submit-cancel'] = array(
      '#type' => 'submit',
      '#value' => 'Cancel'
  );
  
  return $form;
}


/**
 * Edit Record Form Submit
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   Contains the values and storage for the form
 */
function tripal_bulk_loader_edit_template_record_form_submit ($form, &$form_state) {
  //dpm($form_state, 'form state -start submit');
  
  if (!$form_state['ahah_submission']) {
    if ($form_state['values']['op'] ==  'Edit Record') {

      $template = $form_state['storage']['template_array'];
      
      // Edit Record
      $record = $template[ $form_state['storage']['original_priority'] ];
      $record['record_id'] = $form_state['values']['record_name'];
      $record['mode'] = $form_state['values']['mode'];
      $record['table'] = $form_state['values']['chado_table'];
      
      if ($form_state['storage']['original_priority'] != $form_state['values']['field_group']) {
        $record['fields'] = array_merge($record['fields'], $template[ $form_state['values']['field_group'] ]['fields']);
        $template[ $form_state['values']['field_group'] ] = $record;
        unset($template[ $form_state['storage']['original_priority'] ]);
      } else {
        $template[ $form_state['storage']['original_priority'] ] = $record;
      }

      // Save Template
      $form_state['storage']['template']->template_array = serialize($template);
      $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));
  
      if ($success) {
        drupal_set_message('Successfully Updated Template Record');
        drupal_set_message('Template Saved.'); 
        
        $path = explode('?',$form_state['storage']['referring URL']);
        parse_str($path[1], $query);
        $query['template_id'] = $form_state['storage']['template']->template_id;
        drupal_goto($path[0], $query);
      } else {
        drupal_set_message('Unable to Save Template!', 'error');
        watchdog('T_bulk_loader',
          'Unable to save bulk loader template: %template',
          array('%template' => print_r($form_state['storage']['template'], TRUE)),
          WATCHDOG_ERROR
        );
      }
    } elseif ($form_state['values']['op'] ==  'Cancel') {
        $path = explode('?',$form_state['storage']['referring URL']);
        parse_str($path[1], $query);
        $query['template_id'] = $form_state['storage']['template']->template_id;
        //dpm('Redirecting to: '.$path[0].'?'.print_r($query,TRUE).' where the referring URL:'.$form_state['storage']['referring URL']);
        drupal_goto($path[0], $query);    
    }
  }  
  
}

//////////////////////////////////////////////////////////////////////////////////////
// Add/Edit Field Forms
//////////////////////////////////////////////////////////////////////////////////////

/**
 * Add Field Form
 *
 * This form is meant to be called from a bulk loader form. Blank Defaults are in place but you
 * can use the following in the query of the path to set defaults for a given template:
 *  - template_id=\d+: the template to add the field to
 *  - record_id=\d+: the priority or key in the template array of the record to add the field to
 * 
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 */
function tripal_bulk_loader_add_template_field_form (&$form_state = NULL) {
  $form = array();
 	$form['#cache'] = TRUE; // Make sure the form is cached.
 
 	// get template id from path
 	$template_id = ($_GET['template_id']) ? $_GET['template_id'] : $form_state['values']['template_id'];
 	
 	// if there is no template supplied don't return rest of form
 	if (!$template_id) {
 	  return $form;
 	}

  // Pre-set Variables needed for form proper------------------------------------------

 	// If this is the first load of the form (no form state) we need to initialize some variables
 	if (!$form_state['storage']['template']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $template = db_fetch_object(db_query($sql, $template_id));
    $form_state['storage']['template_array'] = unserialize($template->template_array);
    $form_state['storage']['template'] = $template;
    
    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template_array'] as $priority => $record_array) {
      if (!is_array($record_array)) { continue; }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }
    
    $form_state['storage']['referring URL'] = $_SERVER["HTTP_REFERER"];
  } else {
    $template = $form_state['storage']['template'];
  }

  $field_type = ($form_state['values']['field_type'])? $form_state['values']['field_type'] : 'table field';

  // Tables and default table
  $tables = tripal_core_get_chado_tables();
  if ($form_state['values']) {
    if (!preg_match('/^'.current($tables).'$/', $form_state['values']['chado_table'])) {
      $table = $form_state['values']['chado_table'];
    } elseif ($form_state['values']['record_name']) {
      $record_name = $form_state['values']['record_name'];
      $priority = $form_state['storage']['record2priority'][$record_name];
      $table = $form_state['storage']['template_array'][$priority]['table'];
    } else {
      $priority = $form_state['values']['field_group'];
      $table = $form_state['storage']['template_array'][$priority]['table'];  
    }
  } 
  if (!$table) {
    $table = reset($tables);
  }

 	// get the record_id from the path
 	if ($_GET['record_id'] !== NULL) {
   	$form_state['values']['field_group'] = $_GET['record_id'];
    if (preg_match('/\d+/', $_GET['record_id'])) {
      $priority = $form_state['values']['field_group'];
      $table = $form_state['storage']['template_array'][$priority]['table'];  
    }
  }
  
  // Fields and foreign key mappings
  $chado_fields = array();
  $fk_options = array();
  $fk_options['NULL'] = 'None';
  $table_description = module_invoke_all('chado_'.$table.'_schema');
  //dpm($table_description, 'table description for |'.$table.'|');
  if ($field_type == 'foreign key') {
    $foreign_field2table = array();
    foreach ($table_description['foreign keys'] as $key_table => $key_array) {
      foreach ($key_array['columns'] as $left_field => $right_field) {
        $chado_fields[$left_field] = $left_field;
        $foreign_field2table[$left_field] = $key_table;
      }
    }
    reset($chado_fields);
    
    // set default field
    if (empty($chado_fields)) {
      $field = NULL;
    } elseif ($chado_fields[$form_state['values']['chado_field']]) {
      $field = $form_state['values']['chado_field'];
    } else {
      $field = current($chado_fields);
    }
    
    // Foreign key options
    $foreign_table = $foreign_field2table[$field];
    if ($foreign_table) {
      foreach ($form_state['storage']['record2priority'] as $record_name => $priority ) {
        if (preg_match('/^'.$foreign_table.'$/', $form_state['storage']['template_array'][$priority]['table'])) {
          $fk_options[$record_name] = $record_name;
        }
      }
    }
  } else {
    foreach($table_description['fields'] as $field_name => $field_array) {
        $chado_fields[$field_name] = $field_name;
    }
  }

  $variables = array(
    'form_state' => $form_state,
    'tables' => $tables,
    'default table' => $table,
    'fields' => $chado_fields,
    'default_field' => $field,
    'priority' => $priority,
    'record_name' => $record_name,
    'table description' => $table_description,
    'foreign key options' => $fk_options,    
    'foreign field=>table' => $foreign_field2table,
    'foreign table' => $foreign_table,
  );
  //dpm($variables, 'variables');
    
  // Start of Form Proper--------------------------------------------------------------
  //dpm($form_state, 'Form State');
  
  $form['template_name'] = array(
    '#type' => 'item',
    '#title' => 'Template',
    '#value' => $template->name,
  );

  $form['template_id'] = array(
    '#type' => 'hidden',
    '#value' => $template_id,
  );

  $form['add_fields'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="tripal_bulk_loader_template-add_field">',
    '#suffix' => '</div>',
  );

	$form['add_fields']['field_type'] = array(
		'#type' => 'radios',
		'#title' => t('Type of Field'),
		'#options' => array(
			'table field' => t('Spreadsheet: Fields which maps to a Spreadsheet Column'),
			'constant' => t('Constant: Field which remains Constant throughout the Spreadsheet'),
			'foreign key' => t('Foreign Key: Fields which map to a record in another table'),
		),
		'#required' => TRUE,
		'#default_value' => $field_type,
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/add_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-add_field',
      'effect' => 'fade'
    ),
	);

  // check template array for records then add one more
  if (!$form_state['storage']['record2priority']) { 
    $groups = array();
  } else {
    $groups = array_flip($form_state['storage']['record2priority']);
  }
  $groups['NONE'] = 'Select a Record';
  $groups['NEW'] = 'New Record';
  $form['add_fields']['field_group']  = array(
    '#type' => 'select',
    '#title' => 'Record',
    '#description' => 'This is used to group a set of fields together allowing '
      .'multiple records to be inserted into the same table per line of the spreadsheet',
    '#options' => $groups,
    '#default_value' => (preg_match('/\w+.*/',$form_state['values']['field_group'])) ? $form_state['values']['field_group'] : 'NONE',
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/add_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-add_field',
      'effect' => 'fade'
    ),
    '#required' => TRUE,
  );
  
  $form['add_fields']['record_name'] = array(
    '#type' => (preg_match('/NEW/', $form_state['values']['field_group'])) ? 'textfield' : 'hidden',
    '#title' => 'Unique Record Name',
    '#prefix' => '<div id="tripal_bulk_loader_template-add_record">',
    '#suffix' => '</div>',
    '#default_value' => $form_state['values']['record_name'],
  );
  
  $form['add_fields']['field_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Human-readable Title for Field'),
    '#default_value' => $form_state['values']['field_title'],
  );
  
  // Spreadsheet column
  $form['add_fields']['columns'] = array(
    '#type' => 'fieldset',
    '#title' => t('Spreadsheet Column'),
    '#collapsible' => TRUE,
    '#collapsed' => ($field_type == 'table field')? FALSE : TRUE,
  );

  $form['add_fields']['columns']['sheet_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Worksheet'),
    '#description' => t('Specify the name of the worksheet.'),
    '#size' => 5,
    '#default_value' => ($form_state['values']['sheet_name'])? $form_state['values']['sheet_name'] : 'Sheet1',    
  );
  
  $form['add_fields']['columns']['column_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Column'),
    '#description' => t('Specify the column in the spreadsheet that this field maps to where the first column is 1.'),
    '#size' => 5,
    '#default_value' => $form_state['values']['column_number'],
  );

  // Global Value
  $form['add_fields']['constant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Constant'),
    '#collapsible' => TRUE,
    '#collapsed' => ($field_type == 'constant')? FALSE : TRUE,
  );
  
  $form['add_fields']['constant']['constant_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Constant Value'),
    '#description' => t('Specify the value you wish this field to have regardless of spreadsheet data.'),
    '#default_value' => $form_state['values']['constant_value']
  );

  // Foreign Key
  $form['add_fields']['foreign_key'] = array(
    '#type' => 'fieldset',
    '#title' => 'Foreign Key',
    '#collapsible' => TRUE,
    '#collapsed' => ($field_type == 'foreign key')? FALSE : TRUE,
  );

  $form['add_fields']['foreign_key']['foreign_record'] = array(
    '#type' => 'select',
    '#title' => 'Record to refer to',
    '#descripion' => 'Select the record that this foreign key shouold refer to. The record needs to already exist and be of the correct table.',
    '#options' => $fk_options,
  );

  // Chado Field
  $form['add_fields']['chado'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chado Field/Column Details'),
    '#description' => t('Specify the Table/Field in chado that this field maps to.'),
  );
  
  $form['add_fields']['chado']['chado_table'] = array(
    '#type' => 'select',
    '#title' => t('Chado Table'),
    '#options' => $tables,
    '#default_value' => $table,
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/add_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-add_field',
      'effect' => 'fade'
      ),
  );
  
  $form['add_fields']['chado']['chado_field'] = array(
    '#type' => 'select',
    '#title' => t('Chado Field/Column'),
    '#options' => $chado_fields,
    '#default_value' => $form_state['values']['chado_field'],
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/add_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-add_field',
      'effect' => 'fade'
    ),
  );

  $form['add_fields']['additional'] = array(
    '#type' => 'fieldset',
    '#title' => 'Additional Options',
  );

  $form['add_fields']['additional']['required'] = array(
    '#type' => 'checkbox',
    '#title' => 'Make this file required',
  );
  
  $form['add_fields']['additional']['regex_transform'] = array(
    '#type' => 'fieldset',
    '#title' => 'Transform Spreadsheet Value Rules',
    '#collapsible' => TRUE, 
    '#collapsed' => (!$form_state['storage']['regex']['pattern']) ? TRUE : FALSE,
  );
  
  $form['add_fields']['additional']['regex_transform']['regex_description'] = array(
    '#type' => 'item',
    '#value' => 'A transformation rule allows you to transform the original value '
      .'(usually from a user submitted spreadsheet) into the form you would like it stored '
      .'in the chado database. Each rule consists of a match pattern (a php regular expression '
      .'which determines which replacement patterns are applied and captures regions of the '
      .'original value) and a replacement pattern (a string which may contain capture references '
      .'that describes what the new value should be). Each rule is applied to the result of the '
      .'previous rule.'
  );

  $form['add_fields']['additional']['regex_transform']['regex-data'] = array(
    '#tree' => TRUE,
  );
  if (!$form_state['storage']['regex']['pattern']) { $form_state['storage']['regex']['pattern'] = array(); }
  foreach ($form_state['storage']['regex']['pattern'] as $index => $pattern) {
    $data_element = array(
      'pattern' => array(
        '#type' => 'item',
        '#value' => $pattern,
      ),
      'replace' => array(
        '#type' => 'item',
        '#value' => $form_state['storage']['regex']['replace'][$index],
      ),
      'old_index' => array(
        '#type' => 'hidden',
        '#value' => $index,
      ),
      'new_index' => array(
        '#type' => 'select',
        '#options' => range(0,sizeof($form_state['storage']['regex']['pattern'])-1),
        '#default_value' => $index,
      ),
      'id' => array(
        '#type' => 'hidden',
        '#value' => $index,
      ),
      'submit-delete' => array(
        '#type' => 'submit',
        '#value' => 'Delete Transformation',
        '#name' => $index,
      ),
    );
    $form['add_fields']['additional']['regex_transform']['regex-data'][$index] = $data_element;
  }

  $form['add_fields']['additional']['regex_transform']['submit-reorder_regex'] = array(
    '#type' => ($form_state['storage']['regex']['pattern']) ? 'submit' : 'hidden',
    '#value' => 'Save Transformation Rule Order'
  );
  
  $form['add_fields']['additional']['regex_transform']['new_regex'] = array(
    '#type' => 'fieldset',
    '#title' => 'Add a new Transformation Rule',
  );
  
  $form['add_fields']['additional']['regex_transform']['new_regex']['pattern'] = array(
    '#type' => 'textfield',
    '#title' => 'Match Pattern',
    '#description' => 'You can use standard php regular expressions in this field to specify a '
      .'pattern. Only if this pattern matches the value in the spreadsheet does the replacement '
      .'pattern get applied to the value. To capture a section of your value for use in the '
      .'replacement patten surround with round brackets. For example, <i>GI:(\d+)</i> will match '
      .' NCBI gi numbers and will capture the numerical digits for use in the replacement pattern. '
      .' To match and capture any value use <i>.*</i>',
  );
  
  $form['add_fields']['additional']['regex_transform']['new_regex']['replace'] = array(
    '#type' => 'textfield',
    '#title' => 'Replacement Pattern',
    '#description' => 'This pattern should contain the text you want to replace the match pattern '
    .'mentioned above. It can include references of the form \n where n is the number of the '
    .'capture in the match pattern. For example, \1 will be replaced with the text matched in your '
    .'first set of round brackets.',
  ); 

  if ($field_type == 'table field') {
    $tab = '&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp';
    $form['add_fields']['additional']['regex_transform']['new_regex']['replace']['#description'] .= '<p>'
      .'The following references are also available for spreadsheet fields: <b><#column:<i>number</i>#></b>. '
      .'This allows you to substitute other spreadsheet values into the current field. For example, '
      .'if you had the following line:<br>'
      .$tab.'SNP'.$tab.'15-Jan-2011'.$tab.'1'.$tab.'54'.$tab.'Contig34355'
      .'<br> and your current field is for column #1 and you\'re inserting into the chado field '
      .'feature.uniquename then you might want to add in the data to ensure your uniquename is '
      .'unique. The Match Pattern is (.*) to select all the first column and the Replacement '
      .'Pattern could be \1_<#column:2#> which would insert SNP_15-Jan-2011 into the database.</p>';
  }
  
  $form['add_fields']['additional']['regex_transform']['new_regex']['submit-add_transform'] = array(
    '#type' => 'submit',
    '#value' => 'Add Transformation',
  );
  
  $form['add_fields']['additional']['regex_transform']['test_regex'] = array(
    '#type' => 'fieldset',
    '#title' => 'Test Transformation Rules',
  );
  
  $form['add_fields']['additional']['regex_transform']['test_regex']['test_string'] = array(
    '#type' => 'textfield',
    '#title' => 'Test Value',
    '#description' => 'This should be a value that you expect the above transformation rules '
      .'to be applied to.',
    '#default_value' => $form_state['storage']['test_regex_test'],
  );
  
  $form['add_fields']['additional']['regex_transform']['test_regex']['test_result'] = array(
    '#type' => 'textfield',
    '#title' => 'Test Result',
    '#description' => 'This is the value that would be saved to the database after the above transformation '
      .'riles were applied to the Test Value.',
    '#default_value' => $form_state['storage']['test_regex_result'],
  );
  
  $form['add_fields']['additional']['regex_transform']['test_regex']['submit-test'] = array(
    '#type' => 'submit',
    '#value' => 'Test Transformation Rules'
  );
  
  $form['add_fields']['submit-add_field'] = array(
      '#type' => 'submit',
      '#value' => 'Add Field'
  );

  $form['add_fields']['submit-cancel'] = array(
      '#type' => 'submit',
      '#value' => 'Cancel'
  );
  
  return $form;
}

/**
 * Add Field Submit
 * 
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage for the form
 */
function tripal_bulk_loader_add_template_field_form_submit ($form, &$form_state) {
  
  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  
  if (!$form_state['ahah_submission']) {
    if ($op ==  'Add Field') {

      $template = $form_state['storage']['template_array'];

       // If new record
      if (preg_match('/NEW/',$form_state['values']['field_group'])) {
        $record_name = $form_state['values']['record_name'];
        $priority = sizeof($form_state['storage']['template_array']) + 1;
        $record2priority[$record_name] = $priority;
        $template[$priority]['table'] = $form_state['values']['chado_table'];
        $template[$priority]['record_id'] = $record_name;
        
      } else {
        $priority = $form_state['values']['field_group'];
        $record_name = $record2priority[$priority];
      }
      
      // Add field to template array
      if ($form_state['values']['field_type'] == 'table field') {
        $field = array(
          'type' => 'table field',
          'title' => $form_state['values']['field_title'],
          'field' => $form_state['values']['chado_field'],
          'required' => $form_state['values']['required'],
          //'allowed values' => empty by default,
          'spreadsheet sheet' => $form_state['values']['sheet_name'],
          'spreadsheet column' => $form_state['values']['column_number'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.     
        );
      } elseif ($form_state['values']['field_type'] == 'constant') {
        $field = array(
          'type' => 'constant',
          'title' => $form_state['values']['field_title'],
          'field' => $form_state['values']['chado_field'],
          'required' => $form_state['values']['required'],
          //'allowed values' => empty by default,
          'constant value' => $form_state['values']['constant_value'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.     
        );      
      } elseif ($form_state['values']['field_type'] == 'foreign key') {
        $field = array(
          'type' => 'foreign key',
          'title' => $form_state['values']['field_title'],
          'field' => $form_state['values']['chado_field'],
          'foreign key' => $form_state['values']['foreign_record'],
          'required' => $form_state['values']['required'],
        );        
      }

      // Deal with any additional options
      if ($form_state['storage']['regex']) {
        $field['regex'] = $form_state['storage']['regex'];
      }

      // Save Template
      $template[$priority]['fields'][] = $field;
      $form_state['storage']['template']->template_array = serialize($template);
      $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));
  
      if ($success) {
        drupal_set_message('Successfully Added Field to Template');
        drupal_set_message('Template Saved.'); 
        
        $path = explode('?',$form_state['storage']['referring URL']);
        parse_str($path[1], $query);
        $query['template_id'] = $form_state['storage']['template']->template_id;
        drupal_goto($path[0], $query);
      } else {
        drupal_set_message('Unable to Save Template!', 'error');
        watchdog('T_bulk_loader',
          'Unable to save bulk loader template: %template',
          array('%template' => print_r($form_state['storage']['template'], TRUE)),
          WATCHDOG_ERROR
        );
      }
    } elseif ($op ==  'Cancel') {
        $path = explode('?',$form_state['storage']['referring URL']);
        parse_str($path[1], $query);
        $query['template_id'] = $form_state['storage']['template']->template_id;
        drupal_goto($path[0], $query);    
    } elseif ($op == 'Add Transformation') {

      // Add transformation rule to original field
      $form_state['storage']['regex']['pattern'][] = '/' . $form_state['values']['pattern'] . '/';
      $form_state['storage']['regex']['replace'][] = $form_state['values']['replace'];      
      drupal_set_message('Successfully Added Transformation Rule');
      
    } elseif ($op == 'Save Transformation Rule Order') {
      
      // Generate new regex array
      $new_regex = array();
      $old_regex = $form_state['storage']['regex'];
      foreach ($form_state['values']['regex-data'] as $key => $element) {
        $new_regex['pattern'][ $element['new_index'] ] = $old_regex['pattern'][ $element['old_index'] ];
        $new_regex['replace'][ $element['new_index'] ] = $old_regex['replace'][ $element['old_index'] ];
      }
      
      // sort new regex arrays
      asort($new_regex['pattern']);
      asort($new_regex['replace']);

      $form_state['storage']['regex'] = $new_regex;
    } elseif ($op == 'Delete Transformation') {
    
      // Unset regex rule
      $index = $form_state['clicked_button']['#name'];
      unset($form_state['storage']['regex']['pattern'][$index]);
      unset($form_state['storage']['regex']['replace'][$index]);
      
    } elseif ($op == 'Test Transformation Rules') {
    
      $patterns = $form_state['storage']['regex']['pattern'];
      $replaces = $form_state['storage']['regex']['replace'];
      $test_string = $form_state['values']['test_string'];
      $form_state['storage']['test_regex_result'] = preg_replace($patterns, $replaces, $test_string);
      $form_state['storage']['test_regex_test'] = $test_string;
    }
  }  

}

/**
 * Edit Field Form
 *
 * This form is meant to be called from a bulk loader form. The following should be set 
 * in the query section of the path:
 *  - template_id=\d+: the template which the edited field is part of
 *  - record_id=\d+: the priority or key in the template array of the record the field 
 *      is currently part of
 *  - field_index=\d+: the key of the field in the fields array of the previously
 *      specified record
 * 
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 */
function tripal_bulk_loader_edit_template_field_form (&$form_state = NULL) {
  $form = array();
 	$form['#cache'] = TRUE; // Make sure the form is cached.
 
 	// get template id from path
 	$template_id = ($_GET['template_id']) ? $_GET['template_id'] : $form_state['values']['template_id'];
 	
 	// if there is no template supplied don't return rest of form
 	if (!$template_id) {
 	  return $form;
 	}

  // Pre-set Variables needed for form proper------------------------------------------

 	// If this is the first load of the form (no form state) we need to initialize some variables
 	if (!$form_state['storage']['template']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $template = db_fetch_object(db_query($sql, $template_id));
    $form_state['storage']['template_array'] = unserialize($template->template_array);
    $form_state['storage']['template'] = $template;
    
    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template_array'] as $priority => $record_array) {
      if (!is_array($record_array)) { continue; }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }
    
    $form_state['storage']['referring URL'] = $_SERVER["HTTP_REFERER"];
  } else {
    $template = $form_state['storage']['template'];
  }
  
  // get the field from the path
  if (!($_GET['record_id']===NULL || $_GET['field_index']===NULL)) {
    $priority = $_GET['record_id'];
    $field_index = $_GET['field_index'];
    $template_field = $form_state['storage']['template_array'][$priority]['fields'][$field_index];
    $form_state['storage']['original_field'] = $template_field;
    $form_state['storage']['original_field']['priority'] = $priority;
    $form_state['storage']['original_field']['field_index'] = $field_index;
  }
  
  $field_type = ($form_state['values']['field_type'])? $form_state['values']['field_type'] : $template_field['type'];

  // Tables and default table
  $tables = tripal_core_get_chado_tables();
  if ($form_state['values']) {
    $table = $form_state['values']['chado_table'];
  } else {
    $table = $form_state['storage']['template_array'][$priority]['table'];  
  }
  
  
  // Fields and foreign key mappings
  $chado_fields = array();
  $fk_options = array();
  $fk_options['NULL'] = 'None';
  $table_description = module_invoke_all('chado_'.$table.'_schema');
  if ($field_type == 'foreign key') {
    $foreign_field2table = array();
    foreach ($table_description['foreign keys'] as $key_table => $key_array) {
      foreach ($key_array['columns'] as $left_field => $right_field) {
        $chado_fields[$left_field] = $left_field;
        $foreign_field2table[$left_field] = $key_table;
      }
    }
    reset($chado_fields);
    
    // set default field
    if (empty($chado_fields)) {
      $field = NULL;
    } elseif ($chado_fields[$form_state['values']['chado_field']]) {
      $field = $form_state['values']['chado_field'];
    } elseif ($template_field['field']) {
      $field = $template_field['field'];
    } else {
      $field = current($chado_fields);
    }
    //dpm($field, 'field');
    
    // Foreign key options
    $foreign_table = $foreign_field2table[$field];
    if ($foreign_table) {
      foreach ($form_state['storage']['record2priority'] as $record_nameT => $priorityT ) {
        if (preg_match('/^'.$foreign_table.'$/', $form_state['storage']['template_array'][$priorityT]['table'])) {
          $fk_options[$record_nameT] = $record_nameT;
        }
      }
    }
  } else {
    foreach($table_description['fields'] as $field_name => $field_array) {
        $chado_fields[$field_name] = $field_name;
    }
  }
    
//  dpm(array( 'tables' => $tables, 'default table' => $table, 'record name' => $record_name, 'priority' => $priority,
//    'fields' => $chad_fields, 'default field' => $field, 'foreign field=>table' => $foreign_field2table, 
//    'table desc' => $table_description, 'foreign record options' => $fk_options, 'foreign table' => $foreign_table
//    ), 'Variables');
  
  // Start of Form Proper--------------------------------------------------------------
  
  $form['template_name'] = array(
    '#type' => 'item',
    '#title' => 'Template',
    '#value' => $template->name,
  );

  $form['template_id'] = array(
    '#type' => 'hidden',
    '#value' => $template_id,
  );

  $form['edit_fields'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="tripal_bulk_loader_template-edit_field">',
    '#suffix' => '</div>',
  );

	$form['edit_fields']['field_type'] = array(
		'#type' => 'radios',
		'#title' => t('Type of Field'),
		'#options' => array(
			'table field' => t('Spreadsheet: Fields which maps to a Spreadsheet Column'),
			'constant' => t('Constant: Field which remains Constant throughout the Spreadsheet'),
			'foreign key' => t('Foreign Key: Fields which map to a record in another table'),
		),
		'#required' => TRUE,
		'#default_value' => $field_type,
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/edit_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-edit_field',
      'effect' => 'fade'
    ),
	);

  // check template array for records then edit one more
  if (!$form_state['storage']['record2priority']) { 
    $groups = array();
  } else {
    $groups = array_flip($form_state['storage']['record2priority']);
  }
  $groups['NONE'] = 'Select a Record';
  $groups['NEW'] = 'New Record';
  $form['edit_fields']['field_group']  = array(
    '#type' => 'select',
    '#title' => 'Record',
    '#description' => 'This is used to group a set of fields together allowing '
      .'multiple records to be inserted into the same table per line of the spreadsheet',
    '#options' => $groups,
    '#default_value' => (preg_match('/(\d+|\w+)/',$form_state['values']['field_group'])) ? $form_state['values']['field_group'] : $priority,
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/edit_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-edit_field',
      'effect' => 'fade'
    ),
    '#required' => TRUE,
  );
  
  $form['edit_fields']['record_name'] = array(
    '#type' => (preg_match('/NEW/', $form_state['values']['field_group'])) ? 'textfield' : 'hidden',
    '#title' => 'Unique Record Name',
    '#prefix' => '<div id="tripal_bulk_loader_template-edit_record">',
    '#suffix' => '</div>',
    '#default_value' => $form_state['values']['record_name'],
  );
  
  $form['edit_fields']['field_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Human-readable Title for Field'),
    '#default_value' => ($form_state['values']['field_title']) ? $form_state['values']['field_title'] : $template_field['title'],
  );
  
  // Spreadsheet column
  $form['edit_fields']['columns'] = array(
    '#type' => 'fieldset',
    '#title' => t('Spreadsheet Column'),
    '#collapsible' => TRUE,
    '#collapsed' => ($field_type == 'table field')? FALSE : TRUE,
  );

  $form['edit_fields']['columns']['sheet_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Worksheet'),
    '#description' => t('Specify the name of the worksheet.'),
    '#size' => 5,
    '#default_value' => ($form_state['values']['sheet_name'])? $form_state['values']['sheet_name'] : $template_field['spreadsheet sheet'],    
  );
  
  $form['edit_fields']['columns']['column_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Column'),
    '#description' => t('Specify the column in the spreadsheet that this field maps to where the first column is 1.'),
    '#size' => 5,
    '#default_value' => ($form_state['values']['column_number']) ? $form_state['values']['column_number'] : $template_field['spreadsheet column'],
  );

  // Global Value
  $form['edit_fields']['constant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Constant'),
    '#collapsible' => TRUE,
    '#collapsed' => ($field_type == 'constant')? FALSE : TRUE,
  );
  
  $form['edit_fields']['constant']['constant_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Constant Value'),
    '#description' => t('Specify the value you wish this field to have regardless of spreadsheet data.'),
    '#default_value' => ($form_state['values']['constant_value']) ? $form_state['values']['constant_value'] : $template_field['constant value'],
  );

  // Foreign Key
  $form['edit_fields']['foreign_key'] = array(
    '#type' => 'fieldset',
    '#title' => 'Foreign Key',
    '#collapsible' => TRUE,
    '#collapsed' => ($field_type == 'foreign key')? FALSE : TRUE,
  );

  $form['edit_fields']['foreign_key']['foreign_record'] = array(
    '#type' => 'select',
    '#title' => 'Record to refer to',
    '#descripion' => 'Select the record that this foreign key shouold refer to. The record needs to already exist and be of the correct table.',
    '#options' => $fk_options,
  );

  // Chado Field
  $form['edit_fields']['chado'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chado Field/Column Details'),
    '#description' => t('Specify the Table/Field in chado that this field maps to.'),
  );
  
  $form['edit_fields']['chado']['chado_table'] = array(
    '#type' => 'select',
    '#title' => t('Chado Table'),
    '#options' => $tables,
    '#default_value' => $table,
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/edit_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-edit_field',
      'effect' => 'fade'
      ),
  );
  
  $form['edit_fields']['chado']['chado_field'] = array(
    '#type' => 'select',
    '#title' => t('Chado Field/Column'),
    '#options' => $chado_fields,
    '#default_value' => ($form_state['values']['chado_field']) ? $form_state['values']['chado_field'] : $template_field['field'],
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/edit_field_ahah',
      'wrapper' => 'tripal_bulk_loader_template-edit_field',
      'effect' => 'fade'
    ),
  );

  $form['edit_fields']['additional'] = array(
    '#type' => 'fieldset',
    '#title' => 'Additional Options',
  );

  $form['edit_fields']['additional']['required'] = array(
    '#type' => 'checkbox',
    '#title' => 'Make this file required',
    '#default_value' => $template_field['required'],
  );
  
  $form['edit_fields']['additional']['regex_transform'] = array(
    '#type' => 'fieldset',
    '#title' => 'Transform Spreadsheet Value Rules',
    '#collapsible' => TRUE, 
    '#collapsed' => (!$template_field['regex']['pattern']) ? TRUE : FALSE,
  );
  
  $transformation_msg = '<p>A transformation rule allows you to transform the original value '
      .'(usually from a user submitted spreadsheet) into the form you would like it stored '
      .'in the chado database. Each rule consists of a match pattern (a php regular expression '
      .'which determines which replacement patterns are applied and captures regions of the '
      .'original value) and a replacement pattern (a string which may contain capture references '
      .'that describes what the new value should be). Each rule is applied to the result of the '
      .'previous rule.<p>'; 
  $form['edit_fields']['additional']['regex_transform']['regex_description'] = array(
    '#type' => 'item',
    '#value' => $transformation_msg,
  );

  $form['edit_fields']['additional']['regex_transform']['regex-data'] = array(
    '#tree' => TRUE,
  );
  foreach ($template_field['regex']['pattern'] as $index => $pattern) {
    $data_element = array(
      'pattern' => array(
        '#type' => 'item',
        '#value' => $pattern,
      ),
      'replace' => array(
        '#type' => 'item',
        '#value' => $template_field['regex']['replace'][$index],
      ),
      'old_index' => array(
        '#type' => 'hidden',
        '#value' => $index,
      ),
      'new_index' => array(
        '#type' => 'select',
        '#options' => range(0,sizeof($template_field['regex']['pattern'])-1),
        '#default_value' => $index,
      ),
      'id' => array(
        '#type' => 'hidden',
        '#value' => $index,
      ),
      'submit-delete' => array(
        '#type' => 'submit',
        '#value' => 'Delete Transformation',
        '#name' => $index,
      ),
    );
    $form['edit_fields']['additional']['regex_transform']['regex-data'][$index] = $data_element;
  }
  
  $form['edit_fields']['additional']['regex_transform']['submit-reorder_regex'] = array(
    '#type' => 'submit',
    '#value' => 'Save Transformation Rule Order'
  );
  
  $form['edit_fields']['additional']['regex_transform']['new_regex'] = array(
    '#type' => 'fieldset',
    '#title' => 'Add a new Transformation Rule',
  );
  
  $form['edit_fields']['additional']['regex_transform']['new_regex']['pattern'] = array(
    '#type' => 'textfield',
    '#title' => 'Match Pattern',
    '#description' => 'You can use standard <b>php regular expressions</b> in this field to specify a '
      .'pattern. Only if this pattern matches the value in the spreadsheet does the replacement '
      .'pattern get applied to the value. To capture a section of your value for use in the '
      .'replacement patten surround with round brackets. For example, <i>GI:(\d+)</i> will match '
      .' NCBI gi numbers and will capture the numerical digits for use in the replacement pattern. '
      .' To match and capture any value use <i>.*</i>',
  );
  
  $form['edit_fields']['additional']['regex_transform']['new_regex']['replace'] = array(
    '#type' => 'textfield',
    '#title' => 'Replacement Pattern',
    '#description' => '<p>This pattern should contain the text you want to replace the match pattern '
    .'mentioned above. It can include references of the form <b>\n</b> where n is the number of the '
    .'capture in the match pattern. For example, \1 will be replaced with the text matched in your '
    .'first set of round brackets.</p>',
  ); 
  
  if ($field_type == 'table field') {
    $tab = '&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp';
    $form['edit_fields']['additional']['regex_transform']['new_regex']['replace']['#description'] .= '<p>'
      .'The following references are also available for spreadsheet fields: <b><#column:<i>number</i>#></b>. '
      .'This allows you to substitute other spreadsheet values into the current field. For example, '
      .'if you had the following line:<br>'
      .$tab.'SNP'.$tab.'15-Jan-2011'.$tab.'1'.$tab.'54'.$tab.'Contig34355'
      .'<br> and your current field is for column #1 and you\'re inserting into the chado field '
      .'feature.uniquename then you might want to add in the data to ensure your uniquename is '
      .'unique. The Match Pattern is (.*) to select all the first column and the Replacement '
      .'Pattern could be \1_<#column:2#> which would insert SNP_15-Jan-2011 into the database.</p>';
  }
  
  $form['edit_fields']['additional']['regex_transform']['new_regex']['submit-add_transform'] = array(
    '#type' => 'submit',
    '#value' => 'Add Transformation',
  );
  
  $form['edit_fields']['additional']['regex_transform']['test_regex'] = array(
    '#type' => 'fieldset',
    '#title' => 'Test Transformation Rules',
  );
  
  $form['edit_fields']['additional']['regex_transform']['test_regex']['test_string'] = array(
    '#type' => 'textfield',
    '#title' => 'Test Value',
    '#description' => 'This should be a value that you expect the above transformation rules '
      .'to be applied to.',
    '#default_value' => $form_state['storage']['test_regex_test'],
  );
  
  $form['edit_fields']['additional']['regex_transform']['test_regex']['test_result'] = array(
    '#type' => 'textfield',
    '#title' => 'Test Result',
    '#description' => 'This is the value that would be saved to the database after the above transformation '
      .'riles were applied to the Test Value.',
    '#default_value' => $form_state['storage']['test_regex_result'],
  );
  
  $form['edit_fields']['additional']['regex_transform']['test_regex']['submit-test'] = array(
    '#type' => 'submit',
    '#value' => 'Test Transformation Rules'
  );
  
  $form['edit_fields']['submit-edit_field'] = array(
      '#type' => 'submit',
      '#value' => 'Edit Field'
  );

  $form['edit_fields']['submit-cancel'] = array(
      '#type' => 'submit',
      '#value' => 'Cancel'
  );
  
  return $form;
}

/**
 * Edit Field Form Submit
 * 
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage for the form
 */
function tripal_bulk_loader_edit_template_field_form_submit ($form, &$form_state) {

  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  //dpm($op, 'Operation Submitted');
  
  //Clear Test
  $form_state['storage']['test_regex_result'] = NULL;
  $form_state['storage']['test_regex_test'] = NULL;
  
  if (!$form_state['ahah_submission']) {
    if ($op ==  'Edit Field') {

      // If new record
      if (preg_match('/NEW/',$form_state['values']['field_group'])) {
        // add new record
        $record_name = $form_state['values']['record_name'];
        $priority = sizeof($form_state['storage']['template_array']) + 1;
        $old_priority = $form_state['storage']['original_field']['priority'];
        $field_index = $form_state['storage']['original_field']['field_index'];
        $form_state['storage']['record2priority'][$record_name] = $priority;
        $form_state['storage']['template_array'][$priority]['table'] = $form_state['values']['chado_table'];
        $form_state['storage']['template_array'][$priority]['record_id'] = $record_name;
        
      } else {
        $priority = $form_state['values']['field_group'];
        $old_priority = $form_state['storage']['original_field']['priority'];
        $field_index = $form_state['storage']['original_field']['field_index'];
        $record_name = $form_state['storage']['record2priority'][$priority];
      }
      
      $field = $form_state['storage']['original_field'];
      if ($form_state['values']['field_type'] == 'table field') {
          $field['type'] = 'table field';
          $field['title'] = $form_state['values']['field_title'];
          $field['field'] = $form_state['values']['chado_field'];
          $field['required'] = $form_state['values']['required'];
          //$field['allowed values'] = empty by default;
          $field['spreadsheet sheet'] = $form_state['values']['sheet_name'];
          $field['spreadsheet column'] = $form_state['values']['column_number'];
          //$field['exposed'] = 'true|false';  If exposed, will give a select box first from allowed values if set, second from database if values not set.  
      } elseif ($form_state['values']['field_type'] == 'constant') {
          $field['type'] = 'constant';
          $field['title'] = $form_state['values']['field_title'];
          $field['field'] = $form_state['values']['chado_field'];
          $field['required'] = $form_state['values']['required'];
          //$field['allowed values'] = empty by default;
          $field['constant value'] = $form_state['values']['constant_value'];
          //$field['exposed'] = 'true|false';  If exposed, will give a select box first from allowed values if set, second from database if values not set.      
      } elseif ($form_state['values']['field_type'] == 'foreign key') {
          $field['type'] = 'foreign key';
          $field['title'] = $form_state['values']['field_title'];
          $field['field'] = $form_state['values']['chado_field'];
          $field['foreign key'] = $form_state['values']['foreign_record'];
          $field['required'] = $form_state['values']['required'];     
      }
      
      // Deal with any additional options
      
      // if the record has changed...
      $form_state['storage']['template_array'][$priority]['table'] = $form_state['values']['chado_table'];
      if ($old_priority != $priority) {
        $form_state['storage']['template_array'][$priority]['fields'][] = $field;
        unset($form_state['storage']['template_array'][$old_priority]['fields'][$field_index]);
        
        // if there are no fields left delete the old record
        if (!$form_state['storage']['template_array'][$old_priority]['fields']) {
          unset($form_state['storage']['template_array'][$old_priority]);
        }
      } else {
        $form_state['storage']['template_array'][$priority]['fields'][$field_index] = $field;
      }

      // Save Template
      $form_state['storage']['template']->template_array = serialize($form_state['storage']['template_array']);
      $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));
  
      if ($success) {
        drupal_set_message('Successfully Updated Field');
        drupal_set_message('Template Saved.'); 

        $path = explode('?',$form_state['storage']['referring URL']);
        parse_str($path[1], $query);
        $query['template_id'] = $form_state['storage']['template']->template_id;
        drupal_goto($path[0], $query);        
      } else {
        drupal_set_message('Unable to Save Template!', 'error');
        watchdog('T_bulk_loader',
          'Unable to save bulk loader template: %template',
          array('%template' => print_r($form_state['storage']['template'], TRUE)),
          WATCHDOG_ERROR
        );
      }
      
    } elseif ($op ==  'Cancel') {
    
        $path = explode('?',$form_state['storage']['referring URL']);
        parse_str($path[1], $query);
        $query['template_id'] = $form_state['storage']['template']->template_id;
        drupal_goto($path[0], $query);    

    } elseif ($op == 'Add Transformation') {

      // Add transformation rule to original field
      $form_state['storage']['original_field']['regex']['pattern'][] = '/' . $form_state['values']['pattern'] . '/';
      $form_state['storage']['original_field']['regex']['replace'][] = $form_state['values']['replace'];      

      // Add original field back into template
      $priority = $form_state['storage']['original_field']['priority'];
      $field_index = $form_state['storage']['original_field']['field_index'];
      $form_state['storage']['template_array'][$priority]['fields'][$field_index] = $form_state['storage']['original_field'];
      
      // Save Template
      $form_state['storage']['template']->template_array = serialize($form_state['storage']['template_array']);
      $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));
  
      if ($success) {
        drupal_set_message('Successfully Added Transformation Rule');
        drupal_set_message('Template Saved.'); 
      } else {
        drupal_set_message('Unable to Save Template!', 'error');
        watchdog('T_bulk_loader',
          'Unable to save bulk loader template: %template',
          array('%template' => print_r($form_state['storage']['template'], TRUE)),
          WATCHDOG_ERROR
        );
      }
    } elseif ($op == 'Save Transformation Rule Order') {
      
      // Generate new regex array
      $new_regex = array();
      $old_regex = $form_state['storage']['original_field']['regex'];
      foreach ($form_state['values']['regex-data'] as $key => $element) {
        $new_regex['pattern'][ $element['new_index'] ] = $old_regex['pattern'][ $element['old_index'] ];
        $new_regex['replace'][ $element['new_index'] ] = $old_regex['replace'][ $element['old_index'] ];
      }
      
      // sort new regex arrays
      asort($new_regex['pattern']);
      asort($new_regex['replace']);
      
      // Add back to original field
      $form_state['storage']['original_field']['regex'] = $new_regex;
      $priority = $form_state['storage']['original_field']['priority'];
      $field_index = $form_state['storage']['original_field']['field_index'];
      $form_state['storage']['template_array'][$priority]['fields'][$field_index] = $form_state['storage']['original_field'];
      
      // Save Template
      $form_state['storage']['template']->template_array = serialize($form_state['storage']['template_array']);
      $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));
  
      if ($success) {
        drupal_set_message('Successfully Reordered Transformation Rules');
        drupal_set_message('Template Saved.'); 
      } else {
        drupal_set_message('Unable to Save Template!', 'error');
        watchdog('T_bulk_loader',
          'Unable to save bulk loader template: %template',
          array('%template' => print_r($form_state['storage']['template'], TRUE)),
          WATCHDOG_ERROR
        );
      }
    } elseif ($op == 'Delete Transformation') {
    
      // Unset regex rule
      $index = $form_state['clicked_button']['#name'];
      unset($form_state['storage']['original_field']['regex']['pattern'][$index]);
      unset($form_state['storage']['original_field']['regex']['replace'][$index]);
      
      $priority = $form_state['storage']['original_field']['priority'];
      $field_index = $form_state['storage']['original_field']['field_index'];
      $form_state['storage']['template_array'][$priority]['fields'][$field_index] = $form_state['storage']['original_field'];
      
      // Save Template
      $form_state['storage']['template']->template_array = serialize($form_state['storage']['template_array']);
      $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));
  
      if ($success) {
        drupal_set_message('Successfully Reordered Transformation Rules');
        drupal_set_message('Template Saved.'); 
      } else {
        drupal_set_message('Unable to Save Template!', 'error');
        watchdog('T_bulk_loader',
          'Unable to save bulk loader template: %template',
          array('%template' => print_r($form_state['storage']['template'], TRUE)),
          WATCHDOG_ERROR
        );
      }
      
    } elseif ($op == 'Test Transformation Rules') {
    
      $patterns = $form_state['storage']['original_field']['regex']['pattern'];
      $replaces = $form_state['storage']['original_field']['regex']['replace'];
      $test_string = $form_state['values']['test_string'];
      $form_state['storage']['test_regex_result'] = preg_replace($patterns, $replaces, $test_string);
      $form_state['storage']['test_regex_test'] = $test_string;
    }
  } 
  
}

//////////////////////////////////////////////////////////////////////////////////////
// AHAH Callbacks
//////////////////////////////////////////////////////////////////////////////////////

/**
 * AHAH Function: Replace $form['add_fields'] in tripal_bulk_loader_add_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_add_field_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form['add_fields'];
  // Remove the wrapper so we don't double it up.
  //unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
  
}

/**
 * AHAH Function: Replace $form['edit_fields'] in tripal_bulk_loader_edit_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_edit_field_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form['edit_fields'];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
  
}