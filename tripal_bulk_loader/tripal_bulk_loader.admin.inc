<?php

/**
 * @file
 * Handles Create/Edit/Delete Template Admin Forms
 */
 
/**
 * tripal_bulk_loader_admin_template
 */
function tripal_bulk_loader_admin_template () {
	$add_url = url("admin/tripal/tripal_bulk_loader_template/add");
   $output = "<a href=\"$add_url\">Create a new bulk loader template</a><br>"; 
   $del_url = url("admin/tripal/tripal_bulk_loader_template/delete");
   $output .= "<a href=\"$del_url\">Delete a bulk loader template</a>";
   return $output;
}

//////////////////////////////////////////////////////////////////////////////////////
// Create Template
//////////////////////////////////////////////////////////////////////////////////////

/**
 * tripal_bulk_loader_admin_template_form
 */
function tripal_bulk_loader_create_template_base_form (&$form_state = NULL) {

   // Basic Details--------------------------------------------------------------
   $form['bulk_loader'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 1: Basic Template Details'),
   );
   
	$modules = module_invoke_all('tripal_bulk_loader_supported_modules');
	$modules[''] = 'Select A Module';
	if ($form_state['storage']['base_table']) {
	  $base_table = $form_state['storage']['base_table'];
	} else {
	  $base_table = '';
	}
  $form['bulk_loader']['chado_module'] = array(
      '#title'         => t('Chado Module'),
      '#description'   => t('Please select the module for which you would like to create an importer'),
      '#type'          => 'select',
      '#options'       => $modules,
      '#default_value' => $base_table,
      '#weight'        => 0,
      '#required'      => TRUE
	);

  $form['bulk_loader']['template_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Template Name'),
    '#default_value' => $form_state['storage']['template_name'],
  );

  if ($base_table) {
    
    // Add Fields---------------------------------------------------------------------
		$form = array_merge($form, tripal_bulk_loader_add_template_field_form($form_state));

    // List Current Fields------------------------------------------------------------
    $form['current_fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 3: Confirm Fields')
    );
    
    if ($form_state['storage']['template']) {
			$form['current_fields']['items'] = array(
				'#type' => 'item',
				'#value' => theme('tripal_bulk_loader_template', $form_state['storage']['template_id']),
			);
    } else {
    	$form['current_fields']['items'] = array(
    		'#type' => 'item',
    		'#value' => t('There are currently no fields added to this template.'),
    	);
    }
    
    // Submit-------------------------------------------------------------------------
    $form['submit-save'] = array(
        '#type' => 'submit',
        '#value' => 'Save Template'
    );
  } else {
    $form['submit-next_step'] = array(
        '#type' => 'submit',
        '#value' => 'Next Step'
    );
  } //end of if base details are set
	
	return $form;
}

/**
* tripal_bulk_loader_admin_template_form_validate
*/
function tripal_bulk_loader_create_template_base_form_validate($form, &$form_state){
  if(!$form_state['ahah_submission']) {
    if ($form_state['values']['op'] ==  'Add Field') {
      if ($form_state['values']['type'] == 'column') {
        if (!$form_state['values']['column']) {
          form_set_error('column', 'Column is Required!');
        }
        if (!$form_state['values']['sheet']) {
          form_set_error('sheet', 'Worksheet Name is Required!');
        }
      } elseif ($form_state['values']['type'] == 'constant') {
        if (!$form_state['values']['constant_value']) {
          form_set_error('constant_value', 'Value of Constant is Required!');
        }
      }
    } elseif ($form_state['values']['op'] ==  'Save Template') { 
      if (!$form_state['storage']['template']) {
        form_set_error('', 'Tempalte must contain at least one field');
      }
    }
  }
}

/**
* tripal_bulk_loader_admin_template_form_submit
*/
function tripal_bulk_loader_create_template_base_form_submit($form, &$form_state){

  // AHAH Storage ----------------------------------------------------------
  $form_state['rebuild'] = TRUE;
  $form_state['storage']['base_table'] = $form_state['values']['chado_module'];
  $form_state['storage']['template_name'] = $form_state['values']['template_name'];
	
  if (!$form_state['ahah_submission']) {
  		// When template is first defined
  	if ($form_state['values']['op'] ==  'Next Step') {
      $record = array(
        'name' => $form_state['values']['template_name'],
        'template_array' => ' '
      );
      drupal_write_record('tripal_bulk_loader_template', $record);
      $form_state['storage']['template_id'] = $record['template_id'];
      
      // Add Field to Template----------------------------------------------
    } elseif ($form_state['values']['op'] ==  'Add Field') {
      $form_state = tripal_bulk_loader_add_template_field_form_submit($form_state);
      
      // Save Template ----------------------------------------------------
    } 
    
		$record = array(
			'name' => $form_state['values']['template_name'],
			'template_array' => serialize($form_state['storage']['template'])
		);
		//Check if template exists
		$sql = "SELECT count(*) as count FROM tripal_bulk_loader_template WHERE name='%s'";
		if (db_result(db_query($sql, $form_state['values']['template_name']))) {
			// Update Previous
			drupal_write_record('tripal_bulk_loader_template', $record, array('name'));
		} else {
			// Insert New
			drupal_write_record('tripal_bulk_loader_template', $record);
		}
  } //end of if not ahah submission
  else {
    $form_state = tripal_bulk_loader_add_template_field_form_submit($form_state);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Edit Template
//////////////////////////////////////////////////////////////////////////////////////


/**
 * 
 */
function tripal_bulk_loader_edit_template_base_form ($form_state = NULL){
	$form = array();
  
	$sql = "SELECT * FROM {tripal_bulk_loader_template}";
	$resource = db_query($sql);
	$templates = array();
	$templates[''] = 'Select a Template';
	while ($r = db_fetch_object($resource)) {
		$templates[$r->template_id] = $r->name;
	}
	if ($form_state['storage']['template_id']) {
    $form['template_name'] = array(
      '#type' => 'item',
      '#title' => 'Template',
      '#value' => $templates[$form_state['storage']['template_id']],
    );
  } else {
    $form['template_name'] = array(
      '#title'         => t('Template'),
      '#description'   => t('Please select the template you would like to edit.'),
      '#type'          => 'select',
      '#options'       => $templates,
      '#default_value' => $form_state['storage']['template_id'],
      '#weight'        => 0,
      '#required'      => TRUE,
	  );	
  }

  $form['records'] = array(
		'#type' => ($form_state['storage']['template_id'])? 'fieldset' : 'hidden',
		'#title' => t('Current Records'), 
		'#weight' => -10,
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

  $form['records']['submit-reorder'] = array(
    '#type' => 'submit',
    '#value' => 'Save Order',
  );
	
	$form['fields'] = array(
		'#type' => ($form_state['storage']['template_id'])? 'fieldset' : 'hidden',
		'#title' => t('Current Fields'),
		'#weight' => 2,
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
              '#value' => "Edit Field #$i",
            ),
            'delete_submit' => array(
              '#type' => 'submit',
              '#value' => "Delete Field #$i",
            ),
          );
          
          $i++;
        }
    }
    $form['fields']['total_fields'] = array(
      '#type' => 'item',
      '#value' => $i,
    );
    
    $form['fields']['add_field'] = array(
      '#type' => 'submit',
      '#value' => 'Add Field',
    );


	}

	$value = ($form_state['storage']['template_id'])? 'Save Template' : 'Edit Template';
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => $value
	);

	dpm($_SESSION, 'SESSION -main');
	
	return $form;
}

/**
 * 
 */
function tripal_bulk_loader_edit_template_base_form_submit($form, &$form_state){
	
	dpm('SUBMIT: Edit Base');

  $form_state['rebuild'] = TRUE;
  if (preg_match('/^\d+$/', $form_state['values']['template_name'])) {
  	$form_state['storage']['template_id'] = $form_state['values']['template_name'];
	}
	
	$sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
	$result = db_fetch_object(db_query($sql, $form_state['storage']['template_id']));
	$form_state['storage']['template'] = unserialize($result->template_array);
  
  switch ($form_state['values']['op']) {
    // Initialize after template is chosen ----------------------------------------
    case 'Edit Template':
      $form_state['storage']['record2priority'] = array();
      foreach ($form_state['storage']['template'] as $priority => $record_array) {
        if (!is_array($record_array)) { continue; }
        $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
      }
    break;
    
    // Save Reordered Records -----------------------------------------------------
    case 'Save Order':
      dpm('save order');
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
    
    case 'Add Field':
      drupal_goto('admin/tripal/tripal_bulk_loader_template/edit/add_field', array('template_id'=>$form_state['storage']['template_id']));
    break;
    
    default:
      // Delete Field ---------------------------------------------------------------
      if (preg_match('/Delete Field #(\d+)/', $form_state['values']['op'], $matches)) {
        $field_data = $form_state['values']['fields-data'][$matches[1]];
        $priority = $field_data['priority_hidden'];
        $field_key = $field_data['field_index'];
        unset($form_state['storage']['template'][$priority]['fields'][$field_key]);
        if (!$form_state['storage']['template'][$priority]['fields']) {
          unset($form_state['storage']['record2priority'][$form_state['storage']['template'][$priority]['record_id']]);
          unset($form_state['storage']['template'][$priority]);
        } 
        drupal_set_message('Deleted Field from Template.');
      // Edit Field -----------------------------------------------------------------
      } elseif (preg_match('/Edit Field #(\d+)/', $form_state['values']['op'], $matches)) {
        $field_data = $form_state['values']['fields-data'][$matches[1]];
        $priority = $field_data['priority_hidden'];
        $field_key = $field_data['field_index'];
        
      }
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
      '#description'   => t('Please select the template you would like to edit.'),
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

function tripal_bulk_loader_delete_template_base_form_submit ($form, &$form_state) {
	$sql = "DELETE FROM {tripal_bulk_loader_template} WHERE template_id=%d";
	db_query($sql, $form_state['values']['template_name']);
}

//////////////////////////////////////////////////////////////////////////////////////
// Add/Edit Field Forms (meant to be returned as part of a larger form)
//////////////////////////////////////////////////////////////////////////////////////

/**
 * Add Field Form
 *
 * This form meant to be part of a larger form. Currently it is part of the following:
 *  - tripal_bulk_loader_add_template_base_form
 *  - tripal_bulk_loader_edit_template_base_form
 * 
 * @param $form_state
 *   The $form_state from the parent form
 * @return
 *   The parent form with the edit field fieldset added
 *
 * Usage:
 * @code
    $form = array_merge($form, tripal_bulk_loader_add_template_field_form($form_state));
 * @endcode
 */
function tripal_bulk_loader_add_template_field_form (&$form_state = NULL) {
  $form = array();
 	
 	dpm($form_state, 'form state');
 	
 	// get template id from path
 	$template_id = ($_GET['template_id']) ? $_GET['template_id'] : $form_state['values']['template_id'];
 	
 	// if there is no template supplied don't return rest of form
 	if (!$template_id) {
 	  return $form;
 	}
 	
 	if (!$form_state['storage']['template']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $template = db_fetch_object(db_query($sql, $template_id));
  } else {
    $template = $form_state['storage']['template'];
  }
  
  $form['template_name'] = array(
    '#type' => 'item',
    '#title' => 'Template',
    '#value' => $template->name,
  );

  $form['template_id'] = array(
    '#type' => 'hidden',
    '#value' => $template_id,
  );

  $form['referring URL'] = array(
    '#type' => 'hidden',
    '#value' => ($form_state['storage']['referring_URL']) ? $form_state['storage']['referring_URL'] : $_SERVER["HTTP_REFERER"],
  );

  $form['add_fields'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="tripal_bulk_loader_template-add_field">',
    '#suffix' => '</div>',
  );

	$field_type = ($form_state['values']['add-type'])? $form_state['values']['add-type'] : 'NONE';
	$form['add_fields']['add-type'] = array(
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
        'path' => 'admin/tripal/tripal_bulk_loader_template/add/field_type_ahah',
        'wrapper' => 'tripal_bulk_loader_template-add_field',
        'effect' => 'fade'
        ),
	);
	
	if (!$form_state['values']['add-type']) {
    $form['add_fields']['submit-type'] = array(
      '#type' => 'submit',
      '#value' => 'Submit',
    );
  }
  
  if ($form_state['values']['add-type']) {
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
        'path' => 'admin/tripal/tripal_bulk_loader_template/add/record_group_ahah',
        'wrapper' => 'tripal_bulk_loader_template-add_record',
        'effect' => 'fade'
        ),
    );
    
    $form['add_fields']['record_name'] = array(
      '#type' => (preg_match('/NEW/', $form_state['values']['field_group'])) ? 'textfield' : 'hidden',
      '#title' => 'Unique Record Name',
      '#prefix' => '<div id="tripal_bulk_loader_template-add_record">',
      '#suffix' => '</div>',
    );
    
    $form['add_fields']['field_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Human-readable Title for Field'),
      '#default_value' => $form_state['values']['field_title'],
    );
    
    // Spreadsheet column
    $form['add_fields']['columns'] = array(
      '#type' => ($field_type == 'table field')? 'fieldset' : 'hidden',
      '#title' => t('Spreadsheet Column'),
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
      '#type' => ($field_type == 'constant')? 'fieldset' : 'hidden',
      '#title' => t('Constant'),
    );
    
    $form['add_fields']['constant']['constant_value'] = array(
      '#type' => 'textfield',
      '#title' => t('Constant Value'),
      '#description' => t('Specify the value you wish this field to have regardless of spreadsheet data.'),
      '#default_value' => $form_state['values']['constant_value']
    );
    
    // Chado Field
    $form['add_fields']['chado'] = array(
      '#type' => 'fieldset',
      '#title' => t('Chado Field/Column Details'),
      '#description' => t('Specify the Table/Field in chado that this field maps to.'),
    );
    
    $tables = tripal_core_get_chado_tables();
    if ($form_state['values']['record_name']) {
      $record_name = $form_state['values']['record_name'];
      $priority = $form_state['storage']['record2priority'][$record_name];
      $table = $form_state['storage']['template']->template_array[$priority]['table'];
    } else {
      $priority = $form_state['values']['field_group'];
      $table = $form_state['storage']['template']->template_array[$priority]['table'];  
    }
    dpm($table, 'table');
    if (!$table) {
      $table = reset($tables);
    }
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
  
    $chado_fields = array();
    $table_description = module_invoke_all('chado_'.$table.'_schema');
    if ($field_type == 'foreign key') {
      $foreign_field2table = array();
      foreach ($table_description['foreign keys'] as $key_table => $key_array) {
        foreach ($key_array['columns'] as $left_field => $right_field) {
          $chado_fields[$left_field] = $left_field;
          $foreign_field2table[$left_field] = $key_table;
        }
      }
    } else {
      foreach($table_description['fields'] as $field_name => $field_array) {
          $chado_fields[$field_name] = $field_name;
      }
    }
    $form['add_fields']['chado']['add-chado_field'] = array(
      '#type' => 'select',
      '#title' => t('Chado Field/Column'),
      '#options' => $chado_fields,
      '#ahah' => array(
        'path' => 'admin/tripal/tripal_bulk_loader_template/add_field_ahah',
        'wrapper' => 'tripal_bulk_loader_template-add_field',
        'effect' => 'fade'
        ),
    );
  
    // Foreign Key
    $form['add_fields']['foreign_key'] = array(
      '#type' => ($field_type == 'foreign key')? 'fieldset' : 'hidden',
      '#title' => 'Foreign Key',
    );
  
    $groups = array();
    $field = reset($chado_fields);
    $foreign_table = $foreign_field2table[$field];
    foreach ($form_state['storage']['record2priority'] as $record_name => $priority ) {
      if (preg_match('/'.$foreign_table.'/', $form_state['storage']['template']->template_array[$priority]['table'])) {
        $groups[$record_name] = $record_name;
      }
    }
    $groups['NULL'] = 'None';
    $form['add_fields']['foreign_key']['add-field_foreign_key'] = array(
      '#type' => 'select',
      '#title' => 'Record to refer to',
      '#descripion' => 'Select the record that this foreign key shouold refer to. The record needs to already exist and be of the correct table.',
      '#options' => $groups,
    );
      
    // Extra Options
    $form['add_fields']['extra_options'] = array(
      '#type' => 'fieldset',
      '#title' => 'Additional Field Specific Options',
      '#description' => 'If there are additional options that can be specified for the current '
        .'field they will appear here. Options available may include: specifying allowed values, '
        .'indicating a field is required, exposing a field to the user, or specifying which record '
        .'fully describes the foreign key for another table. Only some of these fields may be '
        .'available to any given field.',
    );
  
    $form['add_fields']['submit-add_field'] = array(
        '#type' => 'submit',
        '#value' => 'Add Field'
    );
  }
  
  return $form;
}

/**
 * Add Field Submit
 *
 * This submit meant to be part of a larger form submit. Currently it is part of the following:
 *  - tripal_bulk_loader_add_template_base_form_submit
 *  - tripal_bulk_loader_edit_template_base_form_submit
 * 
 * @param $form_state
 *   The $form_state from the parent submit
 * @return
 *   The parent form_state with any changes made
 *
 * Usage:
 * @code
    $form_state = tripal_bulk_loader_add_template_field_form_submit($form_state);
 * @endcode
 */
function tripal_bulk_loader_add_template_field_form_submit ($form, &$form_state) {
  
  if (!$form_state['storage']['template']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
    $template = db_fetch_object(db_query($sql, $form_state['values']['template_id']));
    $template->template_array = unserialize($template->template_array);
    $form_state['storage']['template'] = $template;
    
    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template']->template_array as $priority => $record_array) {
      if (!is_array($record_array)) { continue; }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }
  }
  
  if (!$form_state['ahah_submission']) {
    if ($form_state['values']['op'] ==  'Add Field') {
      
      
       // If new record
      if (preg_match('/NEW/',$form_state['values']['add-field_group'])) {
        $record_name = $form_state['values']['add-record_name'];
        $priority = sizeof($form_state['storage']['template']) + 1;
        $record2priority[$record_name] = $priority;
        $template[$priority]['table'] = $form_state['storage']['add']['chado_table'];
        $template[$priority]['record_id'] = $record_name;
      } else {
        $priority = $form_state['values']['add-field_group'];
        $record_name = $record2priority[$priority];
      }
      $form_state['storage']['add']['priority'] = $priority;
      
      // Add field to template array
      if ($form_state['storage']['add']['field_type'] == 'table field') {
        $template[$priority]['fields'][] = array(
          'type' => 'table field',
          'title' => $form_state['storage']['add']['field_title'],
          'field' => $form_state['storage']['add']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'spreadsheet sheet' => $form_state['storage']['add']['sheet_name'],
          'spreadsheet column' => $form_state['storage']['add']['column_number'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.
          //'mapping' => array(
          //   'from' => 'to'
          //   '/from re/' => 'to'
          //),        
        );
      } elseif ($form_state['storage']['add']['field_type'] == 'constant') {
        $template[$priority]['fields'][] = array(
          'type' => 'constant',
          'title' => $form_state['storage']['add']['field_title'],
          'field' => $form_state['storage']['add']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'constant value' => $form_state['storage']['add']['constant_value'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.     
        );      
      } elseif ($form_state['storage']['add']['field_type'] == 'foreign key') {
        $template[$priority]['fields'][] = array(
          'type' => 'foreign key',
          'title' => $form_state['storage']['add']['field_title'],
          'field' => $form_state['storage']['add']['chado_field'],
          'foreign key' => $form_state['values']['add-field_foreign_key'],
          //'required' => <true|false>,   
        );        
      }

      // Save Template
      $record = array(
        'template_id' => $form_state['storage']['template_id'],
        'template_array' => serialize($form_state['storage']['template'])
      );
      drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
  
      drupal_set_message('Successfully Added Field to Template');
      drupal_set_message('Template Saved.'); 
      //$_SESSION['tripal_bulk_loader']['edit_template']['add_field'] = FALSE;
    }
  }  

  dpm ($form_state, 'Submit: form state');

}

/**
 * Edit Field Form
 *
 * This form meant to be part of a larger form. Currently it is part of the following:
 *  - tripal_bulk_loader_edit_template_base_form
 * 
 * @param $form_state
 *   The $form_state from the parent form
 * @return
 *   The parent form with the edit field fieldset added
 *
 * Usage:
 * @code
    $form = array_merge($form, tripal_bulk_loader_edit_template_field_form($form_state));
 * @endcode
 */
function tripal_bulk_loader_edit_template_field_form($form_state) {
  $form = array();
  
 	$base_table = $form_state['storage']['base_table'];
 	if ($form_state['storage']['edit']['chado_table']) {
 	  $table = $form_state['storage']['edit']['chado_table'];
  } elseif ($form_state['storage']['template'][$form_state['values']['edit-field_group']]) {
    $table = $form_state['storage']['template'][$form_state['values']['edit-field_group']]['table'];
  } else {
    $table = $base_table;
  }
  
	$form['edit_fields'] = array(
		'#type' => 'fieldset',
		'#title' => t('Edit Fields'),
	);

	$field_type = ($form_state['storage']['edit']['field_type'])? $form_state['storage']['edit']['field_type'] : 'table field';
	$form['edit_fields']['edit-type'] = array(
		'#type' => 'radios',
		'#title' => t('Type of Field'),
		'#options' => array(
			'table field' => t('Spreadsheet: Fields which maps to a Spreadsheet Column'),
			'constant' => t('Constant: Field which remains Constant throughout the Spreadsheet'),
			'foreign key' => t('Foreign Key: Fields which map to a record in another table'),
		),
		'#default_value' => $field_type,
		'#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/edit/change_subform_ahah',
      'wrapper' => 'tripal_bulk_loader_template-subforms',
			'effect' => 'fade'
		),
	);

  // check template array for records then add one more
  $groups = array_flip($form_state['storage']['record2priority']);
  $groups['NONE'] = 'Select a Record';
  $groups['NEW'] = 'New Record';
  $form['edit_fields']['edit-field_group']  = array(
    '#type' => 'select',
    '#title' => 'Record',
    '#description' => 'This is used to group a set of fields together allowing '
      .'multiple records to be inserted into the same table per line of the spreadsheet',
    '#options' => $groups,
    '#default_value' => (preg_match('/\w+.*/',$form_state['values']['edit-field_group'])) ? $form_state['values']['edit-field_group'] : $form_state['storage']['edit_field']['initial_priority'],
		'#ahah' => array(
			'path' => 'admin/tripal/tripal_bulk_loader_template/add/record_group_ahah',
			'wrapper' => 'tripal_bulk_loader_template-subforms',
			'effect' => 'fade'
			),
  );
  
  $form['edit_fields']['edit-record_name'] = array(
    '#type' => (preg_match('/NEW/', $form_state['values']['edit-field_group'])) ? 'textfield' : 'hidden',
    '#title' => 'Unique Record Name',
    '#prefix' => '<div id="add-new_record">',
    '#suffix' => '</div>'
  );
  
	$form['edit_fields']['edit-field_title'] = array(
		'#type' => 'textfield',
		'#title' => t('Human-readable Title for Field'),
		'#default_value' => $form_state['storage']['edit']['field_title'],
	);

	// Spreadsheet column
	$form['edit_fields']['columns'] = array(
		'#type' => ($field_type == 'table field')? 'fieldset' : 'hidden',
		'#title' => t('Spreadsheet Column'),
	);

	$form['edit_fields']['columns']['edit-sheet'] = array(
		'#type' => 'textfield',
		'#title' => t('Worksheet'),
		'#description' => t('Specify the name of the worksheet.'),
		'#size' => 5,
		'#default_value' => ($form_state['storage']['edit']['sheet_name'])? $form_state['storage']['edit']['sheet_name'] : 'Sheet1',    
	);
	
	$form['edit_fields']['columns']['edit-column'] = array(
		'#type' => 'textfield',
		'#title' => t('Column'),
		'#description' => t('Specify the column in the spreadsheet that this field maps to where the first column is 1.'),
		'#size' => 5,
		'#default_value' => $form_state['storage']['edit']['column_number'],
	);

	// Global Value
	$form['edit_fields']['constant'] = array(
		'#type' => ($field_type == 'constant')? 'fieldset' : 'hidden',
		'#title' => t('Constant'),
	);
	
	$form['edit_fields']['constant']['edit-constant_value'] = array(
		'#type' => 'textfield',
		'#title' => t('Constant Value'),
		'#description' => t('Specify the value you wish this field to have regardless of spreadsheet data.'),
		'#default_value' => $form_state['storage']['edit']['constant_value']
	);
	
	// Chado Field
	$form['edit_fields']['chado'] = array(
		'#type' => 'fieldset',
		'#title' => t('Chado Field/Column Details'),
		'#prefix' => '<div id="tripal_bulk_loader-edit_chado_column">',
		'#suffix' => '</div>',
		'#description' => t('Specify the Table/Field in chado that this field maps to.'),
	);
	
	$tables = tripal_core_get_chado_tables();
	$form['edit_fields']['chado']['edit-chado_table'] = array(
		'#type' => 'select',
		'#title' => t('Chado Table'),
		'#options' => $tables,
		'#default_value' => $table,
		'#ahah' => array(
			'path' => 'admin/tripal/tripal_bulk_loader_template/edit/chado_column_ahah',
			'wrapper' => 'tripal_bulk_loader-edit_chado_column',
			'effect' => 'fade'
			),
	);

  $chado_fields = array();
  $table_description = module_invoke_all('chado_'.$table.'_schema');
  if ($field_type == 'foreign key') {
    $foreign_field2table = array();
    foreach ($table_description['foreign keys'] as $key_table => $key_array) {
      foreach ($key_array['columns'] as $left_field => $right_field) {
        $chado_fields[$left_field] = $left_field;
        $foreign_field2table[$left_field] = $key_table;
      }
    }
  } else {
    foreach($table_description['fields'] as $field_name => $field_array) {
      $chado_fields[$field_name] = $field_name;
    }
  }
	$form['edit_fields']['chado']['edit-chado_field'] = array(
		'#type' => 'select',
		'#title' => t('Chado Field/Column'),
		'#options' => $chado_fields,
		'#default_value' => $form_state['storage']['edit']['chado_field'],
		'#ahah' => array(
      'path' => 'admin/tripal/tripal_bulk_loader_template/edit/change_subform_ahah',
      'wrapper' => 'tripal_bulk_loader_template-subforms',
			'effect' => 'fade'
    ),
	);

  // Foreign Key
  $form['edit_fields']['foreign_key'] = array(
    '#type' => ($field_type == 'foreign key')? 'fieldset' : 'hidden',
    '#title' => 'Foreign Key',
  );

  $groups = array();
  $field = reset($chado_fields);
  $foreign_table = $foreign_field2table[$field];
  if ($field) {
    foreach ( $form_state['storage']['record2priority'] as $record_name => $priority ) {
      if (preg_match('/'.$foreign_table.'/', $form_state['storage']['template'][$priority]['table'])) {
        $groups[$record_name] = $record_name;
      }
    }
  }
  $groups['NULL'] = 'None';
  $form['edit_fields']['foreign_key']['edit-field_foreign_key'] = array(
    '#type' => 'select',
    '#title' => 'Record to refer to',
    '#descripion' => 'Select the record that this foreign key shouold refer to. The record needs to already exist and be of the correct table.',
    '#options' => $groups,
  );

  //Extra options
  $form['edit_fields']['extra_options'] = array(
    '#type' => 'fieldset',
    '#title' => 'Additional Field Specific Options',
    '#description' => 'If there are additional options that can be specified for the current '
      .'field they will appear here. Options available may include: specifying allowed values, '
      .'indicating a field is required, exposing a field to the user, or specifying which record '
      .'fully describes the foreign key for another table. Only some of these fields may be '
      .'available to any given field.',
    '#prefix' => '<div id="tripal_bulk_loader-field_extra_options">',
    '#suffix' => '</div>',
  );

	$form['edit_fields']['submit-Edit_field'] = array(
			'#type' => 'submit',
			'#value' => 'Edit Field'
	);
	
  return $form;
}

/**
 * Edit Field Submit
 *
 * This submit meant to be part of a larger form submit. Currently it is part of the following:
 *  - tripal_bulk_loader_edit_template_base_form_submit
 * 
 * @param $form_state
 *   The $form_state from the parent submit
 * @return
 *   The parent form_state with any changes made
 *
 * Usage:
 * @code
    $form_state = tripal_bulk_loader_edit_template_field_form_submit($form_state);
 * @endcode
 */
function tripal_bulk_loader_edit_template_field_form_submit ($form_state) {

  if (preg_match('/Edit Field #(\d+)/', $form_state['values']['op'], $matches) && preg_match('/change_subform/', $form_state['ahah_triggered'])) {
    $form_state['storage']['display_subform'] = 'edit_field';
    $table = $form_state['values']["chado_table_hidden-".$matches[1]];
    $field_key = $form_state['values']["field_index-".$matches[1]];
    $priority = $form_state['values']["field_priority_hidden-".$matches[1]];
    $form_state['storage']['edit_field'] = $form_state['storage']['template'][$priority]['fields'][$field_key];
    $form_state['storage']['edit_field']['table'] = $table;
    $form_state['storage']['edit_field']['initial_priority'] = $priority;
    $form_state['storage']['edit_field']['field_index'] = $field_key;
    
    $set_default = TRUE;
    $field_template = $form_state['storage']['edit_field'];
  } else {
    $set_default = FALSE;
  }
  
  if ($set_default) {
    $form_state['storage']['edit']['field_title'] = $field_template['title'];
    $form_state['storage']['edit']['field_type'] = $field_template['type'];
    $form_state['storage']['edit']['sheet_name'] = $field_template['spreadsheet sheet'];
    $form_state['storage']['edit']['column_number'] = $field_template['spreadsheet column'];
    $form_state['storage']['edit']['constant_value'] = $field_template['constant value'];
    $form_state['storage']['edit']['chado_table'] = $field_template['table'];
    $form_state['storage']['edit']['chado_field'] = $field_template['field'];
  } else {
    $form_state['storage']['edit']['field_title'] = $form_state['values']['edit-field_title'];
    $form_state['storage']['edit']['field_type'] = $form_state['values']['edit-type'];
    $form_state['storage']['edit']['sheet_name'] = $form_state['values']['edit-sheet'];
    $form_state['storage']['edit']['column_number'] = $form_state['values']['edit-column'];
    $form_state['storage']['edit']['constant_value'] = $form_state['values']['edit-constant_value'];
    $form_state['storage']['edit']['chado_table'] = $form_state['values']['edit-chado_table'];
    $form_state['storage']['edit']['chado_field'] = $form_state['values']['edit-chado_field'];
  }
  
  if (!$form_state['ahah_submission']) {
    if ($form_state['values']['op'] ==  'Edit Field') {
      // remove display of edit form
      $form_state['storage']['display_subform'] = '';

      // If new record
      if (preg_match('/NEW/',$form_state['values']['edit-field_group'])) {
        // add new record
        $record_name = $form_state['values']['edit-record_name'];
        $priority = sizeof($form_state['storage']['template']) + 1;
        $old_priority = $form_state['storage']['edit_field']['initial_priority'];
        $field_index = $form_state['storage']['edit_field']['field_index'];
        $form_state['storage']['record2priority'][$record_name] = $priority;
        $form_state['storage']['template'][$priority]['table'] = $form_state['storage']['edit']['chado_table'];
        $form_state['storage']['template'][$priority]['record_id'] = $record_name;
        
      } else {
        $priority = $form_state['values']['edit-field_group'];
        $old_priority = $form_state['storage']['edit_field']['initial_priority'];
        $field_index = $form_state['storage']['edit_field']['field_index'];
        $record_name = $form_state['storage']['record2priority'][$priority];
      }
      
      if ($form_state['storage']['edit']['field_type'] == 'table field') {
        $field = array(
          'type' => 'table field',
          'title' => $form_state['storage']['edit']['field_title'],
          'field' => $form_state['storage']['edit']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'spreadsheet sheet' => $form_state['storage']['edit']['sheet_name'],
          'spreadsheet column' => $form_state['storage']['edit']['column_number'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.
          //'mapping' => array(
          //   'from' => 'to'
          //   '/from re/' => 'to'
          //),        
        );
      } elseif ($form_state['storage']['edit']['field_type'] == 'constant') {
        $field = array(
          'type' => 'constant',
          'title' => $form_state['storage']['edit']['field_title'],
          'field' => $form_state['storage']['edit']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'constant value' => $form_state['storage']['edit']['constant_value'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.     
        );      
      } elseif ($form_state['storage']['edit']['field_type'] == 'foreign key') {
        $field = array(
          'type' => 'foreign key',
          'title' => $form_state['storage']['edit']['field_title'],
          'field' => $form_state['storage']['edit']['chado_field'],
          'foreign key' => $form_state['values']['edit-field_foreign_key'],
          //'required' => <true|false>,
        );      
      }

      
      $form_state['storage']['template'][$priority]['table'] = $form_state['storage']['edit']['chado_table'];
      if ($old_priority != $priority) {
      $form_state['storage']['template'][$priority]['fields'][] = $field;
        unset($form_state['storage']['template'][$old_priority]['fields'][$field_index]);
      } else {
        $form_state['storage']['template'][$priority]['fields'][$field_index] = $field;
      }
      drupal_set_message('Successfully Updated Field'); 
      
      //reset form
      unset($form_state['storage']['edit_field']);
      unset($form_state['storage']['edit']);
    }
  } 
  
  return $form_state;
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
  
  $form_element = $form;
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
  
}

//////////////////////////////////////////////////////////////////////////////////////

/**
 * AHAH Function: Replace $form['subform'] in tripal_bulk_loader_add_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_edit_change_subform_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'change_subform';
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form[$form['subform_formkey']['#value']];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}

/**
 * AHAH Function: Replace $form['subform']['extra_options'] in tripal_bulk_loader_add_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_field_extra_options_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'change_subform';
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form[$form['subform_formkey']['#value']]['extra_options'];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}
/**
 * AHAH Function: Replace $form['add_fields']['chado'] in tripal_bulk_loader_add_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_add_chado_column_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'add_chado_column';
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form['add_fields']['chado'];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}

/**
 * AHAH Function: Replace $form['add_fields'] in tripal_bulk_loader_add_template_base_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_add_field_type_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'add_field_type';
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form['add_fields'];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}

/**
 * AHAH Function: Replace $form['add_fields']['add-record_name'] in tripal_bulk_loader_add_template_base_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_add_new_record_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'add_new_record';
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form['record_name'];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

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
function tripal_bulk_loader_edit_field_type_ahah () {

  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'edit_field_type';
  
  
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

/**
 * AHAH Function: Replace $form['edit_fields']['chado'] in tripal_bulk_loader_edit_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_edit_chado_column_ahah () {
  
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  
  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_triggered'] = 'edit_chado_column';
  
  $form['#programmed'] = $form['#redirect'] = FALSE;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  
  $form_element = $form['edit_fields']['chado'];
  // Remove the wrapper so we don't double it up.
  unset($form_element['#prefix'], $form_element['#suffix']);

  $output = theme('status_messages');
  $output .= drupal_render($form_element);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}