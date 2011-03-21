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
	return drupal_get_form('tripal_bulk_loader_admin_template_form');
}

/*******************************************************************************
 * tripal_bulk_loader_admin_template_form
 */
function tripal_bulk_loader_admin_template_form (&$form_state = NULL) {
	$no_col = variable_get('tripal_bulk_loader_template_number_of_column', 0);
	$name = variable_get('tripal_bulk_loader_template_name', NULL);
	$form = array();
	if ($no_col == 0) {
		$form['template_name'] = array(
   	'#type'          => 'textfield',
      '#title'         => t('Template Name'),
      '#weight'        => 0,
      '#required'      => TRUE,
		'#default_value' => $template_name,
		);
		$form['no_cols'] = array(
   	'#type'          => 'textfield',
      '#title'         => t('Number of Column'),
      '#weight'        => 1,
      '#required'      => TRUE,
		'#maxlength' => 3,
		'#size' =>3,
		);
		$form['addsheet'] = array (
     	'#type'         => 'button',
     	'#value'        => t('Add a sheet'),
     	'#weight'       => 1,
     	'#executes_submit_callback' => TRUE,
		);
	} else {
		$form['template_setting'] = array(
		'#suffix' => '<table id="tripal_bulk_loader_template_add_table"><tr><th>Column#</th><th>Chado Table</th><th>Chado Column</th><th>Associated CVterm</th><th>Translate Regex</th></tr><tr>',
		'#type' => 'item',
		'#title' => t("Template '$name'"),
		);
	}
	$shema = "";
	global $db_url;
	if(is_array($db_url) and array_key_exists('chado',$db_url)){
		$shema = 'public';
	} else {
		$shema = 'chado';
	}
	$sql = "SELECT table_name FROM information_schema.tables WHERE table_schema = '%s' ORDER BY table_name";
	$previous_db = tripal_db_set_active('chado');
	$result = db_query($sql, $shema);
	$allowed_tables = array ();
	while ($tb = db_fetch_object($result)) {
		$allowed_tables[$tb->table_name] = $tb->table_name;
	}
	$first_table = db_result(db_query($sql,$shema));
	$sql = "SELECT column_name FROM information_schema.columns WHERE table_name ='%s'";
	$result = db_query($sql, $first_table);
	$allowed_columns = array();
	while ($col = db_fetch_object($result)) {
		$allowed_columns[$col->column_name] = $col->column_name;
	}
	tripal_db_set_active($previous_db);
	for ($i = 0; $i < $no_col; $i ++) {
		$j = $i + 1;
		$form[$i]['col'] = array(
		'#prefix' => '<td class="tripal_bulk_loader_template_col">',
		'#suffix' => '</td>',
   	'#type' => 'textfield',
		'#maxlength' => 3,
		'#size' => 3,
		'#required'      => TRUE
		);
		global $base_url;
		$form[$i]['chado_table'] = array(
		'#prefix' => '<td class="tripal_bulk_loader_template_chado_table">',
		'#suffix' => '</td>',
   	'#type' => 'select',
		'#options' => $allowed_tables,
	   '#attributes' => array(
         'onChange' => "return tripal_update_chado_columns(this,'$base_url')",
	    ),
		'#required'      => TRUE
		);
		$form[$i]['chado_column'] = array(
		'#prefix' => '<td class="tripal_bulk_loader_template_chado_column">',
		'#suffix' => '</td>',
   	'#type' => 'select',
		'#options' => $allowed_columns,
		);
		$form[$i]['chado_cvterm'] = array(
		'#prefix' => '<td class="tripal_bulk_loader_template_chado_cvterm">',
		'#suffix' => '</td>',
   	'#type' => 'textfield',
		'#size' => 10,
		);
		$form[$i]['chado_regex'] = array(
		'#prefix' => '<td class="tripal_bulk_loader_template_chado_regex">',
		'#suffix' => '</td></tr><tr>',
   	'#type' => 'textfield',
		'#size' => 10,
		);
	}
	if ($no_col != 0) {
		$form['removesheet'] = array (
     	'#type'         => 'button',
     	'#value'        => t('Remove this sheet'),
     	'#weight'       => 49,
     	'#executes_submit_callback' => TRUE,
		'#prefix' => '</tr></table>',
		);
		$form['submit'] = array (
     	'#type'         => 'submit',
     	'#value'        => t('Save'),
     	'#weight'       => 50,
     	'#executes_submit_callback' => TRUE,
		);
	}
	return $form;
}

/************************************************************************
* tripal_bulk_loader_admin_template_form_submit
*/
function tripal_bulk_loader_admin_template_form_submit($form, &$form_state){
	$op = $form_state['values']['op'];
	if ($op == 'Add a sheet') {
		$name = $form_state['values']['template_name'];
		$sql = "SELECT template_id FROM {tripal_bulk_loader_template} WHERE name = '%s'";
		$template_id = db_result(db_query($sql, $name));
		if ($template_id) {
			form_set_error('template_name',t("The template '$name' exists. Please use another name."));
			variable_set('tripal_bulk_loader_template_name', NULL);
			variable_set('tripal_bulk_loader_template_number_of_column', NULL);
		} else {
			variable_set('tripal_bulk_loader_template_name', $form_state['values']['template_name']);
			variable_set('tripal_bulk_loader_template_number_of_column', $form_state['values']['no_cols']);
		}
		return;
	} elseif ($op == 'Remove this sheet') {
		variable_set('tripal_bulk_loader_template_name', NULL);
		variable_set('tripal_bulk_loader_template_number_of_column', NULL);
		return;
	} elseif ($op == 'Save') {
		$name = variable_get('tripal_bulk_loader_template_name', NULL);
		$template_array = "array('DUMMY' => 'TEMPLATE ARRAY')";
		$sql = "INSERT INTO {tripal_bulk_loader_template} (name, template_array) VALUES ('%s', '%s')";
		if (db_query($sql, $name, $template_array)) {
			drupal_set_message("Bulk loader template '$name' added.");
			variable_set('tripal_bulk_loader_template_name', NULL);
			variable_set('tripal_bulk_loader_template_number_of_column', NULL);
		}
	}
}

/************************************************************************
* tripal_bulk_loader_admin_template_delete
*/
function tripal_bulk_loader_admin_template_delete () {
	return drupal_get_form('tripal_bulk_loader_admin_template_del_form');
}
/************************************************************************
* tripal_bulk_loader_admin_template_del_from
*/
function tripal_bulk_loader_admin_template_del_form (&$form_state = NULL) {
	$form = array();
	$sql = "SELECT * FROM {tripal_bulk_loader_template}";
	$results = db_query($sql);
	$templates = array();
	while ($template = db_fetch_object($results)) {
		$templates [$template->template_id] = $template->name;
	}
	if ($templates) {
		$form['label'] = array(
   	'#type'          => 'item',
      '#title'         => t('Select a template to delete'),
      '#weight'        => 0,
		);
		$form['template_name'] = array(
   	'#type'          => 'select',
      '#title'         => t('Template Name'),
		'#options'       => $templates,
      '#weight'        => 1,
      '#required'      => TRUE
		);
		$form['submit'] = array (
     	'#type'         => 'submit',
     	'#value'        => t('Delete'),
     	'#weight'       => 2,
     	'#executes_submit_callback' => TRUE,
		);
	} else {
		$form['label'] = array(
   	'#type'          => 'item',
      '#description'         => t('No template available'),
      '#weight'        => 0,
		);
	}
	return $form;
}
/************************************************************************
* tripal_bulk_loader_admin_template_del_form_submit
*/
function tripal_bulk_loader_admin_template_del_form_submit($form, &$form_state){
	$template = $form_state['values']['template_name'];
	$name = db_result(db_query("SELECT name FROM {tripal_bulk_loader_template} WHERE template_id = $template"));
	$sql = "DELETE FROM {tripal_bulk_loader_template} WHERE template_id = %d";
	if (db_query($sql, $template)) {
		drupal_set_message("Bulk loader template '$name' deleted.");
	}
}
/************************************************************************
* tripal_bulk_loader_chado_column_ajax
*/
function tripal_bulk_loader_chado_column_ajax ($table) {
	$sql = "SELECT column_name FROM information_schema.columns WHERE table_name ='%s'";
	$previous_db = tripal_db_set_active('chado');
	$result = db_query($sql, $table);
	tripal_db_set_active($previous_db);
	$cols = array();
	while ($col = db_fetch_object($result)) {
		$cols[$col->column_name] = $col->column_name;
	}
	drupal_json($cols);
}