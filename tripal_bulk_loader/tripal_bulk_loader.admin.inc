<?php
/*******************************************************************************
 * tripal_bulk_loader_admin_template
 */
function tripal_bulk_loader_admin_template () {
	$add_url = url("admin/tripal/tripal_bulk_loader_template/add");
   $output = "<a href=\"$add_url\">Create a new bulk loader template</a><br>"; 
   $del_url = url("admin/tripal/tripal_bulk_loader_template/delete");
   $output .= "<a href=\"$del_url\">Delete a bulk loader template</a>";
   return $output;
}

/*******************************************************************************
 * tripal_bulk_loader_admin_template_add
 */
function tripal_bulk_loader_admin_template_add () {
  $output = '';
  
  $output .= drupal_get_form('tripal_bulk_loader_create_template_base_form');
  
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

/*******************************************************************************
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
    
    // Add Field (Maps to column in spreadsheet)--------------------------------------
    $form['add_fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 2: Add Fields'),
    );

    $field_type = ($form_state['storage']['field_type'])? $form_state['storage']['field_type'] : 'column';
    $form['add_fields']['type'] = array(
      '#type' => 'radios',
      '#title' => t('Type of Field'),
      '#options' => array(
        'column' => t('Fields which maps to a Spreadsheet Column'),
        'constant' => t('Field which remains Constant throughout the Spreadsheet'),
      ),
      '#default_value' => $field_type,
    );
    
    $form['add_fields']['field_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Human-readable Title for Field')
    );

    // Spreadsheet column
    $form['add_fields']['columns'] = array(
      '#type' => ($field_type == 'column')? 'fieldset' : 'hidden',
      '#title' => t('Spreadsheet Column'),
      '#prefix' => '<div id="tripal-bulk-loader-spreadsheet-column">',
      '#suffix' => '</div>',
    );

    $form['add_fields']['columns']['sheet'] = array(
      '#type' => 'textfield',
      '#title' => t('Worksheet'),
      '#description' => t('Specify the name of the worksheet.'),
      '#size' => 5,
      '#default_value' => ($form_state['storage']['sheet_name'])? $form_state['storage']['sheet_name'] : 'Sheet1',    
    );
    
    $form['add_fields']['columns']['column'] = array(
      '#type' => 'textfield',
      '#title' => t('Column'),
      '#description' => t('Specify the column in the spreadsheet that this field maps to where the first column is 1.'),
      '#size' => 5,
      '#default_value' => $form_state['storage']['column_number'],
    );

    // Global Value
    $form['add_fields']['constant'] = array(
      '#type' => ($field_type == 'constant')? 'fieldset' : 'hidden',
      '#title' => t('Constant'),
      '#prefix' => '<div id="tripal-bulk-loader-constant-column">',
      '#suffix' => '</div>',
    );
    
    $form['add_fields']['constant']['constant_value'] = array(
      '#type' => 'textfield',
      '#title' => t('Constant Value'),
      '#description' => t('Specify the value you wish this field to have regardless of spreadsheet data.'),
      '#default_value' => $form_state['storage']['constant_value']
    );
    
    // Chado Field
    $form['add_fields']['chado'] = array(
      '#type' => 'fieldset',
      '#title' => t('Chado Field/Column Details'),
      '#prefix' => '<div id="tripal-bulk-loader-chado-column">',
      '#suffix' => '</div>',
      '#description' => t('Specify the Table/Field in chado that this field maps to.'),
    );
    
    $related_tables = module_invoke_all('tripal_bulk_loader_'.$base_table.'_related_tables');
    $table = ($form_state['storage']['chado_table'])? $form_state['storage']['chado_table'] : $base_table;
    $form['add_fields']['chado']['chado_table'] = array(
      '#type' => 'select',
      '#title' => t('Chado Table'),
      '#options' => $related_tables,
      '#default_value' => $table,
      '#ahah' => array(
        'path' => 'admin/tripal/tripal_bulk_loader_template/add/chado_column_ahah',
        'wrapper' => 'tripal-bulk-loader-chado-column',
        'effect' => 'fade'
        ),
    );
  
    $table_description = module_invoke_all('chado_'.$table.'_schema');
    $chado_fields = array();
    foreach($table_description['fields'] as $field_name => $field_array) {
      $chado_fields[$field_name] = $field_name;
    }
    $form['add_fields']['chado']['chado_field'] = array(
      '#type' => 'select',
      '#title' => t('Chado Field/Column'),
      '#options' => $chado_fields
    );
  
    $form['add_fields']['submit-add_field'] = array(
        '#type' => 'submit',
        '#value' => 'Add Field'
    );
    

    // List Current Fields------------------------------------------------------------
    $form['current_fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 3: Confirm Fields')
    );
    
    $form['current_fields']['items'] = array(
      '#type' => 'item',
      '#value' => print_r($form_state['storage']['template'],TRUE),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
    
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

/************************************************************************
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

/************************************************************************
* tripal_bulk_loader_admin_template_form_submit
*/
function tripal_bulk_loader_create_template_base_form_submit($form, &$form_state){
  //dpm($form_state, 'form_state: submit');

  // AHAH Storage ----------------------------------------------------------
  $form_state['rebuild'] = TRUE;
  $form_state['storage']['base_table'] = $form_state['values']['chado_module'];
  $form_state['storage']['template_name'] = $form_state['values']['template_name'];
  
  $form_state['storage']['field_title'] = $form_state['values']['field_title'];
  $form_state['storage']['field_type'] = $form_state['values']['type'];
  $form_state['storage']['sheet_name'] = $form_state['values']['sheet'];
  $form_state['storage']['column_number'] = $form_state['values']['column'];
  $form_state['storage']['constant_value'] = $form_state['values']['constant_value'];
  $form_state['storage']['chado_table'] = $form_state['values']['chado_table'];
  $form_state['storage']['chado_field'] = $form_state['values']['chado_field'];

  if (!$form_state['ahah_submission']) {
      // Add Field to Template----------------------------------------------
    if ($form_state['values']['op'] ==  'Add Field') {
      $template = $form_state['storage']['template'];
      
      if ($form_state['storage']['field_type'] == 'column') {
        $template[$form_state['storage']['chado_table']]['field'][] = array(
          'type' => 'table field',
          'title' => $form_state['storage']['field_title'],
          'field' => $form_state['storage']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'spreadsheet sheet' => $form_state['storage']['sheet_name'],
          'spreadsheet column' => $form_state['storage']['column_number'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.
          //'mapping' => array(
          //   'from' => 'to'
          //   '/from re/' => 'to'
          //),        
        );
      } elseif ($form_state['storage']['field_type'] == 'constant') {
        $template[$form_state['storage']['chado_table']]['field'][] = array(
          'type' => 'constant',
          'title' => $form_state['storage']['field_title'],
          'field' => $form_state['storage']['chado_field'],
          //'required' => <true|false>,
          //'allowed values' => empty by default,
          'constant value' => $form_state['storage']['constant_value'],
          //'exposed' => 'true|false'  If exposed, will give a select box first from allowed values if set, second from database if values not set.     
        );      
      }
      
      dpm($template, 'template');
      $form_state['storage']['template'] = $template;
      
      // Save Template ----------------------------------------------------
    } elseif ($form_state['values']['op'] ==  'Save Template') { 
      $record = array(
        'name' => $form_state['values']['template_name'],
        'template_array' => print_r($form_state['storage']['template'],TRUE)
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
    }
  } //end of if not ahah submission
}

function tripal_bulk_loader_chado_column_ahah () {

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