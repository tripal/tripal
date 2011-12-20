<?php 

function tripal_views_search_admin(){
	$add_url = url('admin/tripal/tripal_views_search/new');
	$output = "<div id='add-new-search'><a href=\"$add_url\">Add New Search</a><div>";
	$output .= drupal_get_form(tripal_views_search_admin_form);
	
	dsm($output, 'tvs admin output');
	return $output;
}


function tripal_views_search_admin_form(){
	
	$query_resulst = db_query('SELECT * FROM public.tripal_views_search;');
	
	$form = array();
	$int_i = 0; 
	while($record = db_fetch_object($query_resulst)){
		dpm($record, 'record' . (string)$int_i);
		
		$item_name = 'query-record-' . (string)$int_i; 
		$form[$item_name] = array(
			'#type' => '#checkbox',
			'#title' => t('Search '),
			'#description' => t('')
		);
		
	 	$int_i++;	
	}
	

	return system_settings_form($form);
}

function tripal_views_search_new_search_form(){
	$form = array();
		$form['tripal_views_search_config_mview'] = array(
		'#type' => 'textfield',
		'#title' => t('Materialized View'),
		'#description' => t('Select which materialized view needs to be used for this search'),
		'#required' => TRUE,
	);
	
	/*
	$form['tripal_views_search_config_mview'] = array(
		'#type' => 'select',
		'#title' => t('Materialized View'),
		'#options' => tripal_views_search_mview_list(),
		'#description' => t('Select which materialized view needs to be used for this search'),
		'#required' => TRUE,
	);

	$form['tripal_views_search_config_btable'] = array(
		'#type' => 'select',
		'#title' => t('Chado base table'),
		'#options' => tripal_views_search_btable_list(),
		'#description' => t('Select which chado table needs to be used for this earch'),
		'#required' => TRUE,
	);

	dpm($form, 'tripal_views_search_admin form');
	 */
	
	
	return $form;
}

function tripal_views_search_mview_list(){
	//must return an array of strings ie:
	$options = array(
		'dummyoption' => t('dummyoptiontext'),
	);

	return $options;
}

function tripal_views_search_btable_list(){
	//must return an array of strings ie:
	$options = array(
		'dummyoption2' => t('dummyoptiontext2'),
	);

	return $options;
}




