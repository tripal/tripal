<?php
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

/**
 * Implements hook_tripal_bulk_loader_supported_modules
 * 
 * @return 
 *   An array of <chado table> => <Human-readable Name> describing the modules supported
 */
function tripal_bulk_loader_tripal_bulk_loader_supported_modules() {
  return array(
    'feature' => 'Feature',
    'organism' => 'Organism',
    'library' => 'Library',
    'analysis' => 'Analysis',
  );
}

function tripal_bulk_loader_tripal_bulk_loader_feature_related_tables () {
  return array(
    'feature' => 'Feature',
    'analysis' => 'Analysis',
    'element' => 'Element',
    'cvterm' => 'CV Terms',
    'dbxref' => 'External Database References',
    'genotype' => 'Genotypes',
    'phenotype' => 'Phenotypes',
    'pub' => 'Publications',
    'feature_relationship' => 'Relationships between Features',
    'synonym' => 'Synonyms',
    'featureloc' => 'Feature Locations',
    'featurepos' => 'Feature Positions',
    'featureprop' => 'Feature Properties',
    'featurerange' => 'Feature Ranges',
    'library' => 'Library',
    'phylonode' => 'Polynode'
  );
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
  //dpm($form_state, 'form_state: submit');

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
}

//////////////////////////////////////////////////////////////////////////////////////
// Edit Template
//////////////////////////////////////////////////////////////////////////////////////

/**
 * 
 */
function tripal_bulk_loader_edit_template_base_form($form_state = NULL){
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
      '#default_value' => $form_state['storage']['template_id'],
      '#weight'        => 0,
      '#required'      => TRUE,
	);	
	
	$form['fields'] = array(
		'#type' => ($form_state['storage']['template_id'])? 'fieldset' : 'hidden',
		'#title' => t('Fields'),
	);

	if ($form_state['storage']['template']) {	
    
    // List Current Fields -------------------------------------------------------------
    $i=1;
    foreach ($form_state['storage']['template'] as $table => $table_array) {
      if (!is_array($table_array)) { continue; }
      
      foreach ($table_array['field'] as $field_index => $field) {
          $form['fields']["field_name-$i"] = array(
            '#type' => 'item',
            '#value' => $field['title'],
          );

          $form['fields']["chado_table_name-$i"] = array(
            '#type' => 'item',
            '#value' => $table,
          );

          $form['fields']["chado_table_hidden-$i"] = array(
            '#type' => 'hidden',
            '#value' => $table,
          );

          $form['fields']["chado_field_name-$i"] = array(
            '#type' => 'item',
            '#value' => $field['field'],
          );

          $form['fields']["sheet_name-$i"] = array(
            '#type' => 'item',
            '#value' => $field['spreadsheet sheet'],
          );

          $form['fields']["column_num-$i"] = array(
            '#type' => 'item',
            '#value' => $field['spreadsheet column'],
          );

          $form['fields']["constant_value-$i"] = array(
            '#type' => 'item',
            '#value' => $field['constant value'],
          );
          
          $form['fields']["field_index-$i"] = array(
            '#type' => 'hidden',
            '#value' => $field_index
          );
          
          $form['fields']["edit-$i"] = array(
            '#type' => 'submit',
            '#value' => "Edit Field #$i",
            '#ahah' => array(
              'path' => 'admin/tripal/tripal_bulk_loader_template/edit/edit_fields_ahah',
              'wrapper' => 'tripal_bulk_loader-edit_field',
              'effect' => 'fade'
            )
          );

          $form['fields']["delete-$i"] = array(
            '#type' => 'submit',
            '#value' => "Delete Field #$i",
          );
          
          $i++;
      }
    }
    $form['fields']['total_fields'] = array(
      '#type' => 'item',
      '#value' => $i,
    );
    
    // Edit Field Form --------------------------------------------------------------------
    $form = array_merge($form, tripal_bulk_loader_edit_template_field_form($form_state));
    
    // Add Field Form ----------------------------------------------------------------------
		$form = array_merge($form, tripal_bulk_loader_add_template_field_form($form_state));
		$form['add_fields']['#title'] = 'Add Fields';
	}

	$value = ($form_state['storage']['template_id'])? 'Save Template' : 'Edit Template';
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => $value
	);
	
	return $form;
}

/**
 * 
 */
function tripal_bulk_loader_edit_template_base_form_submit($form, &$form_state){
	
  $form_state['rebuild'] = TRUE;
	$form_state['storage']['template_id'] = $form_state['values']['template_name'];
	
	$sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=%d";
	$result = db_fetch_object(db_query($sql, $form_state['storage']['template_id']));
	$form_state['storage']['template'] = unserialize($result->template_array);
	$form_state['storage']['base_table'] = $form_state['storage']['template']['module'];
  
  if (!$form_state['ahah_submission']) {
      
      // Add Field to Template----------------------------------------------
    if (preg_match('/Add Field/', $form_state['values']['op'])) {
      $form_state = tripal_bulk_loader_add_template_field_form_submit($form_state);
    } elseif (preg_match('/Edit Field$/', $form_state['values']['op'])) {
      $form_state = tripal_bulk_loader_edit_template_field_form_submit($form_state);
    } elseif (preg_match('/Delete Field #(\d+)/', $form_state['values']['op'], $matches)) {
      $table = $form_state['values']["chado_table_hidden-".$matches[1]];
      $field_key = $form_state['values']["field_index-".$matches[1]];
      unset($form_state['storage']['template'][$table]['field'][$field_key]);
      
      $set_default = TRUE;
      $field_template = $form_state['storage']['edit_field'];      
    }
    
    // Save Template ----------------------------------------------------
		$record = array(
			'template_id' => $form_state['values']['template_name'],
			'template_array' => serialize($form_state['storage']['template'])
		);
		drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
  } //end of if not ahah submission
  else {
    //Add AHAH associated with Add Form
    $form_state = tripal_bulk_loader_add_template_field_form_submit($form_state);    
    $form_state = tripal_bulk_loader_edit_template_field_form_submit($form_state);
  }
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
// Add/Edit Field Forms (meant to be returned as part of a larger form
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
function tripal_bulk_loader_add_template_field_form ($form_state) {
  $form = array();
 
 	$base_table = $form_state['storage']['base_table'];
 	
	$form['add_fields'] = array(
		'#type' => 'fieldset',
		'#title' => t('Step 2: Add Fields'),
		'#prefix' => '<div id="tripal_bulk_loader-add_field">',
		'#suffix' => '</div>',
	);

	$field_type = ($form_state['storage']['add']['field_type'])? $form_state['storage']['add']['field_type'] : 'table field';
	$form['add_fields']['add-type'] = array(
		'#type' => 'radios',
		'#title' => t('Type of Field'),
		'#options' => array(
			'table field' => t('Fields which maps to a Spreadsheet Column'),
			'constant' => t('Field which remains Constant throughout the Spreadsheet'),
		),
		'#default_value' => $field_type,
		'#ahah' => array(
			'path' => 'admin/tripal/tripal_bulk_loader_template/add/field_type_ahah',
			'wrapper' => 'tripal_bulk_loader-add_field',
			'effect' => 'fade'
			),
	);
	
	$form['add_fields']['add-field_title'] = array(
		'#type' => 'textfield',
		'#title' => t('Human-readable Title for Field')
	);

	// Spreadsheet column
	$form['add_fields']['columns'] = array(
		'#type' => ($field_type == 'table field')? 'fieldset' : 'hidden',
		'#title' => t('Spreadsheet Column'),
	);

	$form['add_fields']['columns']['add-sheet'] = array(
		'#type' => 'textfield',
		'#title' => t('Worksheet'),
		'#description' => t('Specify the name of the worksheet.'),
		'#size' => 5,
		'#default_value' => ($form_state['storage']['add']['sheet_name'])? $form_state['storage']['add']['sheet_name'] : 'Sheet1',    
	);
	
	$form['add_fields']['columns']['add-column'] = array(
		'#type' => 'textfield',
		'#title' => t('Column'),
		'#description' => t('Specify the column in the spreadsheet that this field maps to where the first column is 1.'),
		'#size' => 5,
		'#default_value' => $form_state['storage']['add']['column_number'],
	);

	// Global Value
	$form['add_fields']['constant'] = array(
		'#type' => ($field_type == 'constant')? 'fieldset' : 'hidden',
		'#title' => t('Constant'),
	);
	
	$form['add_fields']['constant']['add-constant_value'] = array(
		'#type' => 'textfield',
		'#title' => t('Constant Value'),
		'#description' => t('Specify the value you wish this field to have regardless of spreadsheet data.'),
		'#default_value' => $form_state['storage']['add']['constant_value']
	);
	
	// Chado Field
	$form['add_fields']['chado'] = array(
		'#type' => 'fieldset',
		'#title' => t('Chado Field/Column Details'),
		'#prefix' => '<div id="tripal_bulk_loader-add_chado_column">',
		'#suffix' => '</div>',
		'#description' => t('Specify the Table/Field in chado that this field maps to.'),
	);
	
	$related_tables = module_invoke_all('tripal_bulk_loader_'.$base_table.'_related_tables');
	$table = ($form_state['storage']['add']['chado_table'])? $form_state['storage']['add']['chado_table'] : $base_table;
	$form['add_fields']['chado']['add-chado_table'] = array(
		'#type' => 'select',
		'#title' => t('Chado Table'),
		'#options' => $related_tables,
		'#default_value' => $table,
		'#ahah' => array(
			'path' => 'admin/tripal/tripal_bulk_loader_template/add/chado_column_ahah',
			'wrapper' => 'tripal_bulk_loader-add_chado_column',
			'effect' => 'fade'
			),
	);

	$table_description = module_invoke_all('chado_'.$table.'_schema');
	$chado_fields = array();
	foreach($table_description['fields'] as $field_name => $field_array) {
		$chado_fields[$field_name] = $field_name;
	}
	$form['add_fields']['chado']['add-chado_field'] = array(
		'#type' => 'select',
		'#title' => t('Chado Field/Column'),
		'#options' => $chado_fields
	);

	$form['add_fields']['submit-add_field'] = array(
			'#type' => 'submit',
			'#value' => 'Add Field'
	);
 
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
function tripal_bulk_loader_add_template_field_form_submit ($form_state) {
  $form_state['storage']['add']['field_title'] = $form_state['values']['add-field_title'];
  $form_state['storage']['add']['field_type'] = $form_state['values']['add-type'];
  $form_state['storage']['add']['sheet_name'] = $form_state['values']['add-sheet'];
  $form_state['storage']['add']['column_number'] = $form_state['values']['add-column'];
  $form_state['storage']['add']['constant_value'] = $form_state['values']['add-constant_value'];
  $form_state['storage']['add']['chado_table'] = $form_state['values']['add-chado_table'];
  $form_state['storage']['add']['chado_field'] = $form_state['values']['add-chado_field'];

  if (!$form_state['ahah_submission']) {
    if ($form_state['values']['op'] ==  'Add Field') {
      $template = $form_state['storage']['template'];
      
      $template['module'] = $form_state['storage']['base_table'];
      
      if ($form_state['storage']['add']['field_type'] == 'table field') {
        $template[$form_state['storage']['add']['chado_table']]['field'][] = array(
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
        $template[$form_state['storage']['add']['chado_table']]['field'][] = array(
          'type' => 'constant',
          'title' => $form_state['storage']['add']['field_title'],
          'field' => $form_state['storage']['add']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'constant value' => $form_state['storage']['add']['constant_value'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.     
        );      
      }
      drupal_set_message('Successfully Added Field to Template');
      
      $form_state['storage']['template'] = $template;
    }
  }  

  return $form_state;
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
 	
	$form['edit_fields'] = array(
		'#type' => 'fieldset',
		'#title' => t('Edit Fields'),
		'#prefix' => '<div id="tripal_bulk_loader-edit_field">',
		'#suffix' => '</div>',
	);

	$field_type = ($form_state['storage']['edit']['field_type'])? $form_state['storage']['edit']['field_type'] : 'table field';
	$form['edit_fields']['edit-type'] = array(
		'#type' => 'radios',
		'#title' => t('Type of Field'),
		'#options' => array(
			'table field' => t('Fields which maps to a Spreadsheet Column'),
			'constant' => t('Field which remains Constant throughout the Spreadsheet'),
		),
		'#default_value' => $field_type,
		'#ahah' => array(
			'path' => 'admin/tripal/tripal_bulk_loader_template/edit/field_type_ahah',
			'wrapper' => 'tripal_bulk_loader-edit_field',
			'effect' => 'fade'
			),
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
	
	$related_tables = module_invoke_all('tripal_bulk_loader_'.$base_table.'_related_tables');
	$table = ($form_state['storage']['edit']['chado_table'])? $form_state['storage']['edit']['chado_table'] : $base_table;
	$form['edit_fields']['chado']['edit-chado_table'] = array(
		'#type' => 'select',
		'#title' => t('Chado Table'),
		'#options' => $related_tables,
		'#default_value' => $table,
		'#ahah' => array(
			'path' => 'admin/tripal/tripal_bulk_loader_template/edit/chado_column_ahah',
			'wrapper' => 'tripal_bulk_loader-edit_chado_column',
			'effect' => 'fade'
			),
	);

	$table_description = module_invoke_all('chado_'.$table.'_schema');
	$chado_fields = array();
	foreach($table_description['fields'] as $field_name => $field_array) {
		$chado_fields[$field_name] = $field_name;
	}
	$form['edit_fields']['chado']['edit-chado_field'] = array(
		'#type' => 'select',
		'#title' => t('Chado Field/Column'),
		'#options' => $chado_fields,
		'#default_value' => $form_state['storage']['edit']['chado_field'],
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

  if (preg_match('/Edit Field #(\d+)/', $form_state['values']['op'], $matches)) {
    $table = $form_state['values']["chado_table_hidden-".$matches[1]];
    $field_key = $form_state['values']["field_index-".$matches[1]];
    $form_state['storage']['edit_field'] = $form_state['storage']['template'][$table]['field'][$field_key];
    $form_state['storage']['edit_field']['table'] = $table;
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
      dpm($form_state, 'form_state before');
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
      }
      
      $table = $form_state['storage']['edit_field']['table'];
      $field_index = $form_state['storage']['edit_field']['field_index'];
      $form_state['storage']['template'][$table]['field'][$field_index] = $field;
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
 * AHAH Function: Replace $form['edit_fields'] in tripal_bulk_loader_edit_template_base_form
 *
 * @return
 *  JSON Data printed to the screen
 */
function tripal_bulk_loader_edit_fields_ahah () {

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