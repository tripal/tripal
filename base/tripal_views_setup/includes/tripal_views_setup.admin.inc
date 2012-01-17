<?php 

function tripal_views_setup_admin_form(){
	
	$form = array();
	
	$form['#theme'] = 'tripal';
	
	$query_results = db_query('SELECT * FROM public.tripal_views_setup;');
	
	$header = array('Setup ID', 'Name', 'Materialized View ID', 'Base Table Name', 'Description');
	$rows = array();
	
	$results = array();
	while($result = db_fetch_object($query_results)){
		$rows[] = array($result->setup_id, $result->name, $result->mview_id, $result->base_table_name, $result->description,);
		$results[] = $result; 
	}
	
	$options = array();
	foreach ($results as $key => $value) {
		if(!empty($value))
			$options[] = $value->setup_id;// . ' | ' . $value->name . ' | ' . $value->mview_id . ' | ' . $value->base_table_name;
	}
	
	$form['existing_rows'] = array(
		'#type' => 'select',
		'#options' => $options,
		'#description' => '<strong>Select a View Setup to delete from the database.</strong>',
		'#prefix' => theme('table', $header, $rows),
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Remove'),
	);
	
	$form['cancel'] = array(
		'#type' => 'markup',
		'#value' => l(t('Cancel '), 'admin/tripal/'),
	);
	
	$form['new'] = array(
		'#type' => 'markup',
		'#value' => l(t(' New'), 'admin/tripal/tripal_views_setup_new'),
	);
	
	return $form;
}


function tripal_views_setup_admin_form_submit($form, &$form_state){
	$value = $form['existing_rows']['#options'][$form_state['values']['existing_rows']];
	db_query("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;");
	db_query("DELETE FROM public.tripal_views_handlers WHERE setup_id = $value;");
	db_query("DELETE FROM public.tripal_mviews_join WHERE setup_id = $value;");
}

function tripal_views_setup_new_search_form(){
	$form = array();
	
	$form['row_name'] = array(
		'#title' => t('Name'),
		'#type' => 'textfield',
		'#size' => 60,
		'#maxlength' => 128,
		'#description' => 'Name of the Views Setup',
		'#required' => TRUE,
	);
	
		$form['row_description'] = array(
		'#title' => t('Description'),
		'#type' => 'textfield',
		'#size' => 60,
		'#maxlength' => 255,
		'#description' => 'Briefly describe in which view this will be used',
		'#required' => TRUE,
	);
	
	$form['row_mview'] = array(
		'#title' => t('Materialized View'),
		'#type' => 'select',
		'#options' => array('stub'),
		'#maxlength' => 128,
		'#description' => 'Which materialized view to use.',
		'#required' => TRUE,
	);
	
	$form['row_base_table_name'] = array(
		'#title' => t('Base Table Name'),
		'#type' => 'select',
		'#options' => array('stub'),
		'#maxlength' => 128,
		'#description' => 'Select which chado table to use for this view.',
		'#required' => TRUE,
	);
	
	//--tripal_mviews_join
	
	$form['row_view_column'] = array(
		'#title' => t('View Column'),
		'#type' => 'select',
		'#options' => array('stub'),
		'#maxlength' => 128,
		'#description' => 'Which materialized view column to use.',
		'#required' => TRUE,
	);
	
		$form['row_chado_column'] = array(
		'#title' => t('Chado Column'),
		'#type' => 'select',
		'#options' => array('stub'),
		'#maxlength' => 128,
		'#description' => 'Which Chado table column to use.',
		'#required' => TRUE,
	);
	
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Create',
	); 
	
	return $form;
}

function tripal_views_setup_new_search_form_submit($form, &$form_state){
	
}


