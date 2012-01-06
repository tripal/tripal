<?php 

// function tripal_views_setup_admin(){
	// $add_url = url('admin/tripal/tripal_views_setup/new');
	// $output = "<div id='add-new-search'><a href=\"$add_url\">Add New Search</a><div>";
	// $output .= drupal_get_form('tripal_views_setup_admin_form');
// 
	// return $rows;
// }


function tripal_views_setup_admin_form(){
	
	$query_resulst = db_query('SELECT * FROM public.tripal_views_setup;');
	
	$form = array();
	$int_i = 0;
	
	$records = array();
	while($record = db_fetch_object($query_resulst)){
		$records[] = $record;
	}

	dpm($records, 'records');
	
	$record_fields = array('setup_id', 'mview_id', 'base_table_name', 'name', 'description');

	$form = tripal_helper_form_createformdata($form, $records, $record_fields);
	
	
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Remove'),
	);
	
	$form['cancel'] = array(
		'#type' => 'markup',
		'#value' => l(t('Cancel'), 'admin/tripal/'),
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
	
	foreach ($selected_checkbox as $key => $value) {
		
		dpm((string)$value, 'delete value');
		dpm("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;", 'delete stuff');
		db_query("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;");
		
	}
	
}







function tripal_views_setup_new_search_form(){
	$form = array();
	
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
