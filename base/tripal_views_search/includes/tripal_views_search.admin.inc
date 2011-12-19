<?php 

function tripal_views_search_admin(){
	$form = array();

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

	return system_settings_form($form);
}

/* this needs to go into the function tripal_views_search_block()
 $limitnum = variable_get("onthisdate_maxdisp", 3);

$query = "SELECT nid, title, created FROM " .
"{node} WHERE created >= %d " .
"AND created <= %d";

$query_result = db_query_range($query, $start_time, $end_time, 0, $limitnum);
*/

//TODO: validation for the admin pages


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

/**
 *
 * Use this hook for the admin form page,
 * once changes are saved, use this to update
 * the tripal_views_search related tables.
 *
 * @param array $form
 * @param array $form_state
 * @param int $form_id
 */
function tripal_views_search_form_alter(&$form, &$form_state, $form_id){
	if(strstr('tripal-views-search-admin', $form['#id'] != FALSE)){
		$form['#submit']['tripal_views_search_admin_submit'] = array();
	}
}

/**
 *
 * the actual submit callback function
 * @param int $form_id
 * @param array $form_values
 */
function tripal_views_search_admin_submit($form_id, &$form_values){
	//populate db tables based on settings
}

