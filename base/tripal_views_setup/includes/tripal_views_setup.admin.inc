<?php 

// function tripal_views_setup_admin(){
	// $add_url = url('admin/tripal/tripal_views_setup/new');
	// $output = "<div id='add-new-search'><a href=\"$add_url\">Add New Search</a><div>";
	// $output .= drupal_get_form('tripal_views_setup_admin_form');
// 
	// return $rows;
// }


function tripal_views_setup_admin_form(){
	
	// $query_results = db_query('SELECT * FROM public.tripal_views_setup;');
	$result = db_query('SELECT * FROM public.tripal_views_setup;');
	
	$form = array();
	$int_i = 0;
	
		
	//set limit for pager]
	$limit = 20;
  //define table header
  $header = array(
    '', //note empty value, will use this later
    array('data' => t('Setup ID'), 'field' => 'setup_id', 'sort' => 'asc'),
    array('data' => t('MView ID'), 'field' => 'mview_id'),
    array('data' => t('Base Table Name'), 'field' => 'base_table_name'),
  );
	
	//allows sorting
	$tablesort = tablesort_sql($header);
	//add pager to results
	// $result = pager_query($query_results,$limit, $tablesort);
	
	while ($item = db_fetch_object($result)){
		//add row key
		$checkboxes[$item->setup_id] = '';
		$form['setup_id'][$item->setup_id] = array(
			'#value' => $item->setup_id,
		);
		$form['mview_id'][$item->mview_id] = array(
			'#value' => $item->mview_id,
		);
		
		$form['base_table_name'][$item->setup_id] = array(
			'#value' => $item->base_table_name,
		);
		
	}

	$form['checkboxes'] = array('#type' => 'checkboxes', '#options' => $checkboxes);
	$form['pager'] = array('#value' => theme('pager', NULL, $limit, 0));	
	
	
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
	
	
	dpm($form, 'admin_form');
	return $form;
	//return system_settings_form($form);
}


function tripal_views_setup_admin_form_submit($form_id, $form){
	$form_values = $form['values'];
	dpm($form, 'submitted form');
	dpm($form_values, 'submitted form values');
	$featured = $form_values['row_checkboxes'];
	$selected_checkbox = array();
	
	foreach($featured as $key => $value){
		if($value){
			$selected_checkbox[] = "'$value'";
		}
	}
	
	dpm($selected_checkbox, 'selected');
	
	// foreach ($selected_checkbox as $key => $value) {
// 		
		// dpm((string)$value, 'delete value');
		// dpm("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;", 'delete stuff');
		// db_query("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;");
// 		
	// }
	
}







function tripal_views_setup_new_search_form(){
	$form = array();
	
	$form['q1'] = array(
		'#type' => 'select',
		'#options' => tripal_views_setup_mview_list(),
		
	);
	
	return $form;
}

function tripal_views_setup_mview_list(){
	//must return an array of strings ie:
	$options = array(
		'dummyoption' => t('dummyoptiontext'),
	);

	return $options;
}

function tripal_views_setup_btable_list(){
	//must return an array of strings ie:
	$options = array(
		'dummyoption2' => t('dummyoptiontext2'),
	);

	return $options;
}
