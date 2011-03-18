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

function tripal_bulk_loader_admin_template_form (&$form_state = NULL) {
	$form = array();
	$form['template_name'] = array(
   	'#type'          => 'textfield',
      '#title'         => t('Template Name'),
      '#weight'        => 0,
		'#attributes' => array('id' => 'tripal-bulk-loader-template-name'),
      '#required'      => TRUE
	);
	$form['datafile'] = array(
   	'#type' => 'fieldset',
      '#title' => t("From"),
      '#weight'        => 1,
		'#attributes' => array('id' => 'tripal-bulk-loader-template-from-field'),
	);
	$form['template'] = array(
   	'#type' => 'fieldset',
      '#title' => t("To"),
      '#weight'        => 1,
		'#attributes' => array('id' => 'tripal-bulk-loader-template-to-field'),
	);
	$form['submit'] = array (
     	'#type'         => 'submit',
     	'#value'        => t('Create'),
     	'#weight'       => 2,
     	'#executes_submit_callback' => TRUE,
	);
	return $form;
}

/************************************************************************
* tripal_bulk_loader_admin_template_form_submit
*/
function tripal_bulk_loader_admin_template_form_submit($form, &$form_state){
	$name = $form_state['values']['template_name'];
	$template_array = "array('DUMMY' => 'TEMPLATE ARRAY')";
	$sql = "INSERT INTO {tripal_bulk_loader_template} (name, template_array) VALUES ('%s', '%s')";
	if (db_query($sql, $name, $template_array)) {
		drupal_set_message("Bulk loader template '$name' added.");
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
* function tripal_bulk_loader_admin_template_del_form_submit
*/
function tripal_bulk_loader_admin_template_del_form_submit($form, &$form_state){
	$template = $form_state['values']['template_name'];
	$name = db_result(db_query("SELECT name FROM {tripal_bulk_loader_template} WHERE template_id = $template"));
	$sql = "DELETE FROM {tripal_bulk_loader_template} WHERE template_id = %d";
	if (db_query($sql, $template)) {
		drupal_set_message("Bulk loader template '$name' deleted.");
	}
}