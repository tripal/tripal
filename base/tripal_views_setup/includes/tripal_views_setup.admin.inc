<?php
/**
 *
 * @ingroup tripal_view_setup
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
 * @ingroup tripal_view_setup
 */
function tripal_views_setup_admin_form_submit($form, &$form_state){
	$value = $form['existing_rows']['#options'][$form_state['values']['existing_rows']];
	db_query("DELETE FROM public.tripal_views_setup WHERE setup_id = $value;");
	db_query("DELETE FROM public.tripal_views_handlers WHERE setup_id = $value;");
	db_query("DELETE FROM public.tripal_mviews_join WHERE setup_id = $value;");
}
/**
 *
 * @ingroup tripal_view_setup
 */

function tripal_views_setup_new_setup_form(&$form_state){
	$form = array();

	$form['#cache'] = TRUE;

	//ahah_helper requires this to register the form with it's module
	ahah_helper_register($form, $form_state);

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
	$mview_options['0'] = 'Select';
	while ($mview_option = db_fetch_array($mview_query)){
		$mview_options[$mview_option['mview_id']] = $mview_option['name'];
	}

	if (!isset($form_state['storage']['mview_id'])){
		$mview_default_value = '0';
	}
	else{
		$mview_default_value = $form_state['storage']['mview_id'];
	}

	dpm($mview_default_value, 'mview default value');
	dpm($mview_options, 'mview options');

	$form['mview_id'] = array(
	    '#type'   => 'fieldset',
	    '#title'  => t('Which materialized view to use'),
	    '#prefix' => '<div id="mview-id-wrapper">', // This is our wrapper div.
	    '#suffix' => '</div>',
	    '#tree'   => TRUE, // Don't forget to set #tree!
	);
	$form['mview_id']['usage'] = array(
      '#title' => t('Materialized View'),
      '#type' => 'select',
      '#options' => $mview_options,
      '#description' => 'Which materialized view to use.',
      '#required' => TRUE,
      '#default_value' => $mview_default_value,
      '#ahah' => array(
	      'path' => ahah_helper_path(array('mview')),
  	     'wrapper' => 'mview-id-wrapper',
         'effect' => 'fade',
         'event' => 'change',
         'method' => 'replace',
	),
	);
	$form['mview_id']['update_usage'] = array(
	    '#type'  => 'submit',
	    '#value' => t('Update usage'),
	// Note that we can simply use the generic submit callback provided by the
	// ahah_helper module here!
	// All it does, is set $form_state['rebuild'] = TRUE.
	    '#submit' => array('ahah_helper_generic_submit'),
	// We set the 'no-js' class, which means this submit button will be hidden
	// automatically by Drupal if JS is enabled.
	    '#attributes' => array('class' => 'no-js'),
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


	dpm(strcmp($mview_default_value['usage'], '0'), 'second mview');

	if (strcmp($mview_default_value['usage'], '0') != 0){


		$mview_id = $mview_default_value['usage'];

		dpm($mview_id, 'mviewid is');

		$form['mview_id'] = array(
			'#type' => 'fieldset',
			'#title' => 'Table Fields Setup',
			'#prefix' => '<div id="fieldset-table-rows-wrapper">',
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
		dpm($chado_tables, 'chado tables');
		$form['mview_id']["fields_headers"] = array(
         '#type' => 'markup',
         '#value' => "<div class=\"field-headers\">".
              "<div class=\"column-id\">Field Name and Type</div>".
              "<div class=\"fields-column-join\">Join Table</div>".
              "<div class=\"fields-column-join-column\">Join Column</div>".
              "<div class=\"fields-column-handler\">Handler</div></div>",
		);
		dpm($form,'form here');
		foreach ($columns as $column){
			dpm($i, 'colmuns');

			$column = trim($column);  // trim trailing and leading spaces
			preg_match("/^(.*?)\ (.*?)$/",$column,$matches);
			$column_name = $matches[1];
			$column_type = $matches[2];

			// first print the field name
			$form['mview_id']["fields_start_$mview_id-$i"] = array(
           '#type' => 'markup',
           '#value' => "<div class=\"fields-new-row\">",
			);

			dpm($form,'form here 2');
			$form['mview_id']["fields_column_name_$mview_id-$i"] = array(
           '#type' => 'markup',
           '#attributes' => array('class' => 'fields-column-name'),
           '#value' => "<div class=\"column-id\"><span class=\"column-name\">$column_name</span>".
                       "<br><span class=\"column-type\">$column_type</span></div>",
			);

			// second print the table join drop down
// 			$table = $form_state['values']["fields_column_join_$mview_id_$i"];
			$form['mview_id']["fields_column_join_$mview_id-$i"] = array(
            '#type' => 'select',
            '#prefix' => "<div class=\"fields-column-join\">",
            '#suffix' => "</div>",
            '#options' => $chado_tables,
            '#required' => FALSE,
            '#default_value' => $table,
			//             '#ahah' => array(
			//       	     'path' => "admin/tripal/tripal_views_setup/ajax/field_col_join_$i",
			//       	     'wrapper' => "fields-column-join-column-$mview_id-$i",
			//               'effect' => 'fade',
			//               'event' => 'change',
			//               'method' => 'replace',
			// 			),
			);
			if($table){
				$table_desc = module_invoke_all('chado_'.$table.'_schema');
				$columns = array_keys($table_desc['fields']);
			} else {
				$columns = array();
			}
			$form['mview_id']["fields_column_join_column_$mview_id-$i"] = array(
            '#type' => 'select',
		      '#prefix' => "<div id=\"fields-column-join-column-$mview_id-$i\" class=\"fields-column-join-column\">",
            '#suffix' => "</div>",
            '#options' => $columns,
            '#required' => FALSE,
			);
			$form['mview_id']["fields_column_handler_$mview_id-$i"] = array(
            '#type' => 'select',
            '#prefix' => "<div class=\"fields-column-handler\">",
            '#suffix' => "</div>",
            '#options' => $handlers,
            '#required' => FALSE,
			);
			$form['mview_id']["fields_end_$i"] = array(
            '#type' => 'markup',
            '#value' => "</div>",
			);
			$i++;
		}
		

// 		$form['submit'] = array(
// 	      '#type' => 'submit',
// 	      '#value' => 'Create',
// 		);

// 		$form['row_counter'] = array(
// 		   '#type' => 'hidden',
// 		   '#value' => $i,
// 		);
	} // end if($form_state['values']['mview_id'])
// 	else{
// 		$form = array(
// 			     '#type' => 'item',
// 			  	  '#prefix' => '<div id="table-rows-wrapper">',
// 			  	  '#suffix' => '</div>',
// 		);
// 	}

	$form['save'] = array(
	    '#type'  => 'submit',
	    '#value' => t('Save'),
	);
	return $form;
}


/**
 *
 * @ingroup tripal_view_setup
 */
function tripal_views_setup_new_setup_form_submit($form, &$form_state){
	dpm($form, 'form on submit');
	dpm($form_state, 'forms_state on submit');
}
