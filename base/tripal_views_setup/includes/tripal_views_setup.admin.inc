<?php
/**
 *
 * @ingroup tripal_view_search
 */
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

/**
 *
 * @ingroup tripal_view_search
 */
function tripal_views_setup_admin_form_submit($form, &$form_state){
	$value = $form['existing_rows']['#options'][$form_state['values']['existing_rows']];
	db_query("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;");
	db_query("DELETE FROM public.tripal_views_handlers WHERE setup_id = $value;");
	db_query("DELETE FROM public.tripal_mviews_join WHERE setup_id = $value;");
}
/**
 *
 * @ingroup tripal_view_search
 */
function tripal_views_setup_new_search_form(&$form_state){
	$form = array();

	$form['#cache'] = TRUE;

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

	$mview_query = db_query("SELECT mview_id,name FROM {tripal_mviews} ORDER BY name;");
	$mview_options = array();
	while ($mview_option = db_fetch_array($mview_query)){
		$mview_options[$mview_option['mview_id']] = $mview_option['name'];
	}


	$form['mview_id'] = array(
    '#title' => t('Materialized View'),
    '#type' => 'select',
    '#options' => $mview_options,
    '#description' => 'Which materialized view to use.',
    '#required' => TRUE,
      '#ahah' => array(
         'path' => 'admin/tripal/tripal_views_setup/ajax/mview_cols',
         'wrapper' => 'table-rows-div',
         'effect' => 'fade',
         'event' => 'change',
         'method' => 'replace',
	),
	);

	// ignore this for now... we'll come back to it later -- spf
	//	$form['row_base_table_name'] = array(
	//		'#title' => t('Base Table Name'),
	//		'#type' => 'select',
	//		// '#options' => array('stub'),
	//		'#options' => tripal_core_get_chado_tables(),
	//		'#description' => 'Select which chado table to use for this view.',
	//		'#required' => TRUE,
	//	);

	$form['view_setup_table'] = array(
    '#type' => 'item',
  	'#prefix' => '<div id="table-rows-div">',
  	'#suffix' => '</div>',
	);

	dpm($form_state, 'formstate in mainform');
	if($form_state['values']['mview_id']){
		$mview_id = $form_state['values']['mview_id'];
		dpm($mview_id,'mviewidinif');
		
		$form['view_setup_table'] = array(
			'#type' => 'fieldset',
			'#title' => 'New View Setup',
			'#prefix' => '<div id="table-rows-div">',
		  '#suffix' => '</div>',
		);

		// get the columns in this materialized view.  They are separated by commas
		// where the first word is the column name and the rest is the type
		$sql = "SELECT mv_specs FROM {tripal_mviews} WHERE mview_id = $mview_id";
		$mview = db_fetch_object(db_query($sql));
		$columns = explode(",",$mview->mv_specs);

		$i=1;
		$chado_tables = tripal_core_get_chado_tables();
		$chado_tables = array_merge(array('select or leave blank',), $chado_tables);
		$handlers = array();
		$form['view_setup_table']["fields_headers"] = array(
         '#type' => 'markup',
         '#value' => "<div class=\"field-headers\">".
              "<div class=\"column-id\">Field Name and Type</div>".
              "<div class=\"fields-column-join\">Join Table</div>".
              "<div class=\"fields-column-join-column\">Join Column</div>".
              "<div class=\"fields-column-handler\">Handler</div></div>",
		);
		foreach ($columns as $column){

			$column = trim($column);  // trim trailing and leading spaces
			preg_match("/^(.*?)\ (.*?)$/",$column,$matches);
			$column_name = $matches[1];
			$column_type = $matches[2];

			// first print the field name
			$form['view_setup_table']["fields_start_$i"] = array(
         '#type' => 'markup',
         '#value' => "<div class=\"fields-new-row\">",
			);

			$form['view_setup_table']["fields_column_name_$i"] = array(
         '#type' => 'markup',
           '#attributes' => array('class' => 'fields-column-name'),
         '#value' => "<div class=\"column-id\"><span class=\"column-name\">$column_name</span>".
                       "<br><span class=\"column-type\">$column_type</span></div>",
			);

			// second print the table join drop down
			$form['view_setup_table']["fields_column_join_$i"] = array(
         '#type' => 'select',
         '#prefix' => "<div class=\"fields-column-join\">",
         '#suffix' => "</div>",
         '#options' => $chado_tables,
         '#required' => FALSE,
         '#ahah' => array(
//            'path' => 'admin/tripal/tripal_views_setup/ajax/mview_cols',
//            'wrapper' => 'table-rows-div',
//            'effect' => 'fade',
//            'event' => 'change',
//            'method' => 'replace',
					),
			);
			$column_join = array();
			if($form_state['values']['view_setup_table']["fields_column_join_$i"]){
				$column_join = array('hello', 'world');
			}
			$form['view_setup_table']["fields_column_join_column_$i"] = array(
         '#type' => 'select',
         '#prefix' => "<div class=\"fields-column-join-column\">",
         '#suffix' => "</div>",
         '#options' => $column_join,
         '#required' => FALSE,
			);
			$form['view_setup_table']["fields_column_handler_$i"] = array(
         '#type' => 'select',
           '#prefix' => "<div class=\"fields-column-handler\">",
           '#suffix' => "</div>",
         '#options' => $handlers,
         '#required' => FALSE,
			);
			$form['view_setup_table']["fields_end_$i"] = array(
         '#type' => 'markup',
         '#value' => "</div>",
			);
			$i++;
		}

	}
	
	$form['row_counter'] = array(
      '#type' => 'hidden',
      '#value' => $i,
	);

	$form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Create',
	);
	
	dpm($form, 'form');

	return $form;
}
/**
 *
 * @ingroup tripal_view_search
 */
function tripal_view_search_ajax_mview_cols(){
	dpm($_POST, 'post');
	 
	// Retrieve the form from the cache
	$form_state = array('storage' => NULL);
	$form_build_id = $_POST['form_build_id'];
	$form = form_get_cache($form_build_id, $form_state);
	 
	// Preparing to process the form
	$args = $form['#parameters'];
	$form_id = array_shift($args);
	$form_state['post'] = $form['#post'] = $_POST;
	$form['#programmed'] = $form['#redirect'] = FALSE;
	 
	// Sets the form_state so that the validate and submit handlers can tell
	// when the form is submitted via AHAH
	$form_state['ahah_submission'] = TRUE;
	 
	// Process the form with drupal_process_form. This function calls the submit
	// handlers, which put whatever was worthy of keeping into $form_state.
	drupal_process_form($form_id, $form, $form_state);
	 
	// You call drupal_rebuild_form which destroys $_POST.
	// The form generator function is called and creates the form again but since
	// it knows to use $form_state, the form will be different.
	// The new form gets cached and processed again, but because $_POST is
	// destroyed, the submit handlers will not be called again.
	$form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

	 
	//    $form = drupal_get_form('tripal_views_setup_fields_form',$mview_id);
	unset($form['view_setup_table']['#prefix'], $form['view_setup_table']['#suffix']);
	$output = theme('status_message') . drupal_render($form['view_setup_table']);
	
	// Final rendering callback.
  drupal_json(array('status' => TRUE, 'data' => $output));
}
/**
 *
 * @ingroup tripal_view_search
 */
// function tripal_views_setup_fields(&$form_state=NULL, $mview_id = NULL){

// 	if(!$mview_id){
// 		return;
// 	}

// 	// get the columns in this materialized view.  They are separated by commas
// 	// where the first word is the column name and the rest is the type
// 	$sql = "SELECT mv_specs FROM {tripal_mviews} WHERE mview_id = $mview_id";
// 	$mview = db_fetch_object(db_query($sql));
// 	$columns = explode(",",$mview->mv_specs);

// 	$i=1;
// 	$chado_tables = tripal_core_get_chado_tables();
// 	$handlers = array();
// 	$form["fields_headers"] = array(
//        '#type' => 'markup',
//        '#value' => "<div class=\"field-headers\">".
//             "<div class=\"column-id\">Field Name and Type</div>".
//             "<div class=\"fields-column-join\">Join Table</div>".
//             "<div class=\"fields-column-join-column\">Join Column</div>".
//             "<div class=\"fields-column-handler\">Handler</div></div>",
// 	);
// 	foreach ($columns as $column){

// 		$column = trim($column);  // trim trailing and leading spaces
// 		preg_match("/^(.*?)\ (.*?)$/",$column,$matches);
// 		$column_name = $matches[1];
// 		$column_type = $matches[2];

// 		// first print the field name
// 		$form["fields_start_$i"] = array(
//        '#type' => 'markup',
//        '#value' => "<div class=\"fields-new-row\">",
// 		);

// 		$form["fields_column_name_$i"] = array(
//        '#type' => 'markup',
//          '#attributes' => array('class' => 'fields-column-name'),
//        '#value' => "<div class=\"column-id\"><span class=\"column-name\">$column_name</span>".
//                      "<br><span class=\"column-type\">$column_type</span></div>",
// 		);

// 		// second print the table join drop down
// 		$chado_tables = array_merge(array(NULL,), $chado_tables);
// 		$form["fields_column_join_$i"] = array(
//        '#type' => 'select',
//        '#prefix' => "<div class=\"fields-column-join\">",
//        '#suffix' => "</div>",
//        '#options' => $chado_tables,
//        '#required' => FALSE,
//        '#ahah' => array(
//          'path' => 'admin/tripal/tripal_views_setup/ajax/field_col_join',
//          'wrapper' => "fields-column-join-column-$i",
//          'effect' => 'fade',
//          'event' => 'change',
//          'method' => 'replace',
// 		),
// 		);
// 		$form["fields_column_join_column_$i"] = array(
//        '#type' => 'select',
//        '#prefix' => "<div id=\"fields-column-join-column-$i\" class=\"fields-column-join-column\">",
//        '#suffix' => "</div>",
//        '#options' => array(),
//        '#required' => FALSE,
// 		);
// 		$form["fields_column_handler_$i"] = array(
//        '#type' => 'select',
//          '#prefix' => "<div class=\"fields-column-handler\">",
//          '#suffix' => "</div>",
//        '#options' => $handlers,
//        '#required' => FALSE,
// 		);
// 		$form["fields_end_$i"] = array(
//        '#type' => 'markup',
//        '#value' => "</div>",
// 		);
// 		$i++;
// 	}

// 	$form['row_counter'] = array(
//     '#type' => 'hidden',
//     '#value' => $i,
// 	);

// 	$form['submit'] = array(
//     '#type' => 'submit',
//     '#value' => 'Create',
// 	);

// 	return $form;
// }

function tripal_views_setup_fields_form_submit($form, &$form_state){
	dpm($form,'formfield');
	dpm($form_state, 'formstatefield');
}

/**
 *
 * @ingroup tripal_view_search
 */
function tripal_view_search_ajax_field_col_join(){
	dpm($_POST);
	dpm($_POST['row_counter'], 'rowcounter');
	drupal_json(array('status' => TRUE, 'data' => 'howdy'));

}

/**
 *
 * @ingroup tripal_view_search
 */
function tripal_views_setup_new_search_form_submit($form, &$form_state){
	dpm($form,'form');
	dpm($form_state, 'formstate');
}
