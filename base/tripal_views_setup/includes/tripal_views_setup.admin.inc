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

		// dpm($record, 'record' . (string)$int_i);
// 		
		// $item_name = 'tripal_views_setup_config_query_record_' . (string)$int_i;
// 		
		// $form[$item_name] = array(
			// '#type' => 'checkbox',
			// '#title' => t($record->name),
			// '#description' => t('Search of mview ' . $record->mview_id . ' and ' . $record->base_table_name),
			// '#required' => TRUE,
		// );
// 		
	 	// $int_i++;	
	// }
	
	$form['featured'] = array(
		'#type' => 'checkboxes',
		'#options' => $options,
		'#default_value' => $status,
	);
	
	dpm($form, 'admin_form');
	return $form;
	//return system_settings_form($form);
}


function tripal_views_setup_form_submit($form_id, $form){
	$form_values = $form['values'];
	$featured = $form_values['featured'];
	
	
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




