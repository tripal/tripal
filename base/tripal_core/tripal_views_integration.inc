<?php

/**
 * Purpose: Provide Guidance to new Tripal Admin
 *
 * @return
 *   HTML Formatted text
 *
 * @ingroup tripal_views_integration
 */
function tripal_core_views_description_page() {

  $text .= '<h3>Tripal Views Quick Links:</h3>';
  $text .= "<ul>
             <li><a href=\"".url("admin/tripal/views/mviews") . "\">List of Materialized Views (MViews)</a></li>
             <li><a href=\"".url("admin/tripal/views/mviews/new") . "\">Create a new MViews</a></li>
             <li><a href=\"".url("admin/tripal/views/integration/mviews") . "\">List of integrated MViews</a></li>
             <li><a href=\"".url("admin/tripal/views/integration/mviews/new"). "\">Integrate a MView</a></li>
             <li><a href=\"".url("admin/tripal/views/integration/chado"). "\">List of integrated Chado tables</a></li>
             <li><a href=\"".url("admin/tripal/views/integration/chado/new"). "\">Integrate a Chado tables</a></li>
           </ul>";

  $text .= '<h3>Views Integartion Description:</h3>';
  $text .= '<p>Tripal Views provides an interface for integrating <a href="http://drupal.org/project/views">Drupal Views</a>
            with Tripal materialized views.  This will allow site administrators to create custom queries for the materialized views
            and in turn provide custom content pages, custom blocks and custom search forms. The forms allow a site administrator
            to select a materialized view and associate other Chado tables on which the view can join.  Usage of this module requires
            a good understanding of foreign-key relationships in Chado.
            </p>';

  $text .= '<h3>Setup Instructions:</h3>';
  $text .= '<p>After installation of the Tripal core module.  The following tasks should be performed
            <ol>
              <li><b>Set Permissions</b>: To allow access to site administrators for this module, simply
               <a href="'.url('admin/user/permissions').'">assign permissions</a> to the appropriate user roles for the
               permission type "manage tripal_views_integration". </li>
            </ol>
            </p>';
  $text .= '<h3>Usage Instructions:</h3>';
  $text .= "<p>To use Tripal Views integration follow these steps:
            <ol>
               <li><b>Identify or create a materialized view:</b> Using the <a href=\"".url("admin/tripal/views/mviews") . "\">
                 Tripal materialized View</a> interface, identify the view you would like to integrate or create a new one.</li>
               <li><b>Setup the Views Integration</b>: Navigate to the <a href=\"".url("admin/tripal/views/integration/mviews/new") . "\">
                 Tripal views integration setup page</a> to integrate the selected materialized view.  Provide a  user friendly name
                 and description to help you remember the purpose for integrating the view.  Next, select the view you want to integrate
                 from the provided select box.  If your materialized view has fields that can join with other Chado tables, you may
                 provide those relationships in the provided form.  Finally, if your fields require a special handler for display, you
                 may select it from the drop down provided</li>
               <li><b>Create custom pages/block/search form</b>:  After saving setup information from step 2 above, you will be redirected to the
                 Drupal Views interface</a> where you can create a custom page, block or search form.</li>
               <li><b>Review your integrated views</b>:  A page providing a
                 <a href=\"".url("admin/tripal/views/integration/mviews/list") . "\">list of all integrated views</a> is provided. You may
                 view this page to see all integrated views, but also to remove any unwanted integrations.</li>
            </ol>
         
            </p>";
  return $text;
}
/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_core_views_integration_admin_form(){

  $form = array();

  $form['#theme'] = 'tripal';

  $query_results = db_query('SELECT * FROM public.tripal_views_integration;');

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
  	'#submit' => array('tripal_core_views_integration_admin_form_submit'),
  );
  
//   $form[''] = array(
//     '#type' => 'submit',
//     '#value' => 'Edit',
//     '#submit' => array('tripal_core_views_integration_admin_edit_form_submit'),
//   );
  
  $form['button']['edit_form'] = array(
    '#type' => 'submit',
    '#value' => t('Edit'),
    '#submit' => array('tripal_core_views_integration_admin_edit_form_submit'),
  );

  $form['new'] = array(
    '#type' => 'markup',
    '#value' => l(t(' Add a New MView'), 'admin/tripal/views/integration/mviews/new'),
  );

  return $form;
}



function tripal_core_views_integration_admin_edit_form_submit($form, &$form_state) {
	$form_state['rebuild'] = FALSE;
	unset($form_state['storage']);
// 	$form_state['redirect'] = 'admin/tripal/views/integration/edit_form';
	$form_state['redirect'] = 'admin/tripal/views/integration/mviews/edit/'. $form['existing_rows']['#options'][$form_state['values']['existing_rows']];
	return $form_state;
}
/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_core_views_integration_admin_form_submit($form, &$form_state){
  $value = $form['existing_rows']['#options'][$form_state['values']['existing_rows']];
  db_query("DELETE FROM public.tripal_views_integration WHERE setup_id = $value;");
  db_query("DELETE FROM public.tripal_views_handlers WHERE setup_id = $value;");
  db_query("DELETE FROM public.tripal_mviews_join WHERE setup_id = $value;");
}
/**
 *
 * @ingroup tripal_views_integration
 */

function tripal_core_views_integration_new_setup_form(&$form_state, $setup_id){
	$setup_id = $setup_id['0'];
	dpm($setup_id,'setupid');
	dpm($args, 'args');
	if(isset($setup_id)){
		$setup_obj = db_fetch_object(db_query("SELECT * FROM {tripal_views_integration} WHERE setup_id = '$setup_id';"));
		dpm($setup_obj,'$setup_obj');
		$mview_id = $setup_obj->mview_id;
		$form_state['storage']['mview_id'] = $mview_id;
		$column_query = db_query("SELECT * FROM {tripal_mviews_join} WHERE setup_id = '$setup_id';");
		$mview_joins = array();
		while ($mviews_join = db_fetch_object($column_query)){
			$mview_joins[] = $mviews_join;
		}

		dpm($mview_joins,'mviewjoins');
	}
	dpm($mview_id,'$$mview_id');
  $form = array();

  $data = array();

  $form['#cache'] = TRUE;

  //ahah_helper requires this to register the form with it's module
  ahah_helper_register($form, $form_state);

  // field for the name of the
  $form['row_name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $setup_obj->name,
    '#size' => 60,
    '#maxlength' => 128,
    '#description' => 'Name of the Views Setup',
    '#required' => TRUE,
  );

  $form['row_description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
  	'#default_value' => $setup_obj->description,
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

  $form['mview_id'] = array(
    '#title' => t('Materialized View'),
    '#type' => 'select',
    '#options' => $mview_options,
    '#description' => 'Which materialized view to use.',
    '#required' => TRUE,
//     '#value' => $setup_obj->mview_id,
//     '#default_value' => $mview_default_value,
  	'#default_value' => $setup_obj->mview_id,
    '#ahah' => array(
       'path' => ahah_helper_path(array('view_setup_table')),
       'wrapper' => 'table-rows-div',
       'effect' => 'fade',
       'event' => 'change',
       'method' => 'replace',
  ),
  );

  // ignore this for now... we'll come back to it later -- spf
  //   $form['row_base_table_name'] = array(
  //      '#title' => t('Base Table Name'),
  //      '#type' => 'select',
  //      // '#options' => array('stub'),
  //      '#options' => tripal_core_get_chado_tables(),
  //      '#description' => 'Select which chado table to use for this view.',
  //      '#required' => TRUE,
  //   );

  $form['view_setup_table'] = array(
     '#type' => 'item',
       '#prefix' => '<div id="table-rows-div">',
       '#suffix' => '</div>',
  );


  if ($form_state['storage']['mview_id']){

  	if(!isset($mview_id)){
    	$mview_id = $form_state['storage']['mview_id'];
  	}
  	
    $form['view_setup_table'] = array(
     '#type' => 'fieldset',
     '#title' => 'Join Selection',
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
    $chado_tables = array_merge(array('Select a Join Table',), $chado_tables);

    $form['view_setup_table']["instructions"] = array(
         '#type' => 'markup',
         '#value' => "Select an optional table to which the fields of the materialized view can join.  If a field does not need to join you may leave the selection blank.",
    );

    $data['field_types'] = array();


    // get list of all handlers
    $all_handlers = tripal_core_views_integration_discover_handlers();
    $handlers_fields = array();
    $handlers_filters = array();
    
    $handlers_fields[] = "Select a Field Handler"; 
    $handlers_filters[] = "Select a Filter Handler";
    foreach($all_handlers as $handler){
       if(preg_match("/views_handler_field/",$handler)){
          $handlers_fields[] = $handler;
       }
       if(preg_match("/views_handler_filter/",$handler)){
          $handlers_filters[] = $handler;
       }
    }

    foreach ($columns as $column){
      $column = trim($column);  // trim trailing and leading spaces
      preg_match("/^(.*?)\ (.*?)$/",$column,$matches);
      $column_name = $matches[1];
      $column_type = $matches[2];

      $form['view_setup_table']["fields_start_$mview_id-$i"] = array(
           '#type' => 'markup',
           '#value' => "<div class=\"fields-new-row\">",
      );

      $form['view_setup_table']["fields_name_$mview_id-$i"] = array(
        '#type' => 'markup',
        '#attributes' => array('class' => 'fields-column-name'),
        '#value' => "<div class=\"column-id\">".
                      "<span class=\"column-name\">$column_name</span>".
                      "<br><span class=\"column-type\">$column_type</span>".
                    "</div>".
                    "<div class=\"column-form-fields\"><div class=\"column-one\">",
      );

      $data['field_types'][$column_name] = $column_type;

      if(isset($setup_id)){
      	$p = $i - 1;
      	$table = $mview_joins["$p"]->chado_table_join;
      	$form_state['storage']["fields_join_$mview_id-$i"] = $table;
      	dpm($table,'table');
      }
      else{
      	$table = $form_state['storage']["fields_join_$mview_id-$i"];
      }
      $form['view_setup_table']["fields_join_$mview_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-column-join\">",
        '#suffix' => "</div>",
        '#options' => $chado_tables,
        '#required' => FALSE,
        '#default_value' => $table,
        '#ahah' => array(
           'path' => ahah_helper_path(array("view_setup_table","fields_join_column_$mview_id-$i")),
           'wrapper' => "fields-column-join-column-$mview_id-$i",
           'effect' => 'fade',
           'event' => 'change',
           'method' => 'replace',
         ),
      );
      
      if($table){
        $table_desc = module_invoke_all('chado_'.$table.'_schema');
        $columns = array_keys($table_desc['fields']);
      } else {
        $columns = array('Select Join Column');
      }
      $form['view_setup_table']["fields_join_column_$mview_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "  <div id=\"fields-column-join-column-$mview_id-$i\" class=\"fields-column-join-column\">",
        '#suffix' => "</div></div>",
        '#options' => $columns,
        '#required' => FALSE,
      );      
      $column_name = $mview_joins["$p"]->view_column;
      $default_handlers = db_fetch_object(db_query("SELECT handler_filter, handler_field FROM {tripal_views_handlers} WHERE setup_id = '$setup_id' AND column_name = '$column_name';"));
      dpm($default_handlers, 'default handlers');
      dpm($handlers_fields,'handlers');
      $form['view_setup_table']["fields_field_handler_$mview_id-$i"] = array(
         '#type' => 'select',
         '#prefix' => "<div class=\"column-two\">".
                        "<div class=\"fields-field-handler\">",
         '#suffix' => "</div>",
         '#options' => $handlers_fields,
         '#required' => FALSE,
//          '#'
      );
      $form['view_setup_table']["fields_filter_handler_$mview_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-filter-handler\">",
        '#suffix' => "</div></div>",
        '#options' => $handlers_filters,
        '#required' => FALSE,
      );
      
      $form['view_setup_table']["fields_end_$i"] = array(
        '#type' => 'markup',
        '#value' => "</div></div>",
      );
      $i++;
   }
   $form['view_setup_table']['save'] = array(
      '#type'  => 'submit',
      '#value' => t('Save'),
   );

    $data['row_count'] = $i - 1;
  }

  //use this to put values into $form_state['values']
  $form['data'] = array();

  //need to find out if storing $form['data'][$key]['#value'] = $value <- is an issue
  //since it will give me errors if i try to stare an array instead of $value
  //and yet $value can be an array ie "field_types"
  foreach ($data as $key => $value) {
    $form['data'][$key] = array(
    	'#type' => 'hidden',
    	'#value' => $value,
    );
  }

  return $form;
}


/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_core_views_integration_new_setup_form_validate($form, &$form_state){
  $name_array = explode(" ", $form_state['values']['row_name']);
  if(count($name_array) > 1){
    form_set_error($form_state['values']['row_name'], 'Name must be ONE word only.');
  }
  //TODO: write validation function for this new form
}
/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_core_views_integration_new_setup_form_submit($form, &$form_state){
  $name = $form_state['values']['row_name'];
  $mview_id = $form_state['values']['mview_id'];
  $tripal_views_integration_record = array(
    'mview_id' => $mview_id,
    'name' => $name,
  	 'description' => $form_state['values']['row_description'],
  );

  drupal_write_record('tripal_views_integration', $tripal_views_integration_record);

  $i = 1;
  foreach ($form_state['values']['field_types'] as $key => $value){
    $mview_join_record = array(
        'setup_id' => $tripal_views_integration_record['setup_id'],
        'view_table' => $form['mview_id']['#options'][$form_state['values']['mview_id']],
        'view_column' => $key,
        'chado_table_join' => $form_state['values']["fields_join_$mview_id-$i"],
        'chado_column' => $form['view_setup_table']["fields_join_column_$mview_id-$i"]['#options'][$form_state['values']["fields_join_column_$mview_id-$i"]],
    );
    drupal_write_record('tripal_mviews_join', $mview_join_record);
    
    $handlers_record = array(
      'setup_id' => $tripal_views_integration_record['setup_id'],
      'column_name' => $key,//TODO: should we change this to an mview_join_id from tripal_view_join?
      'handler_filter' => $form['view_setup_table']["fields_filter_handler_$mview_id-$i"]['#options'][$form_state['values']["fields_filter_handler_$mview_id-$i"]],
    	'handler_field' => $form['view_setup_table']["fields_field_handler_$mview_id-$i"]['#options'][$form_state['values']["fields_field_handler_$mview_id-$i"]],
    );
    drupal_write_record('tripal_views_handlers', $handlers_record);
    $i++;
  }

}

/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_core_views_integration_discover_handlers() {

   $handlers = array();

   // Get handlers from all modules.
   foreach (module_implements('views_handlers') as $module) {
     $function = $module . '_views_handlers';
     $result = $function();
     if (!is_array($result)) {
       continue;
     }
     foreach ($result['handlers'] as $handler => $parent){
        $handlers[] = $handler;
     }
   }

   // these handlers are hard coded because I could not 
   // get the views_views_handlers() function to be called
   // in the code above.  However, we will be creating 
   // Chado wrappers for many of these and once that work
   // is done these will no longer be needed.

   // argument handlers
   $handlers[] = 'views_handler_argument';
   $handlers[] = 'views_handler_argument_numeric';
   $handlers[] = 'views_handler_argument_formula';
   $handlers[] = 'views_handler_argument_date';
   $handlers[] = 'views_handler_argument_string';
   $handlers[] = 'views_handler_argument_many_to_one';
   $handlers[] = 'views_handler_argument_null';

   // field handlers
   $handlers[] = 'views_handler_field';
   $handlers[] = 'views_handler_field_date';
   $handlers[] = 'views_handler_field_boolean';
   $handlers[] = 'views_handler_field_markup';
   $handlers[] = 'views_handler_field_xss';
   $handlers[] = 'views_handler_field_url';
   $handlers[] = 'views_handler_field_file_size';
   $handlers[] = 'views_handler_field_prerender_list';
   $handlers[] = 'views_handler_field_numeric';
   $handlers[] = 'views_handler_field_custom';
   $handlers[] = 'views_handler_field_counter';

   // filter handlers
   $handlers[] = 'views_handler_filter';
   $handlers[] = 'views_handler_filter_equality';
   $handlers[] = 'views_handler_filter_string';
   $handlers[] = 'views_handler_filter_boolean_operator';
   $handlers[] = 'views_handler_filter_boolean_operator_string';
   $handlers[] = 'views_handler_filter_in_operator';
   $handlers[] = 'views_handler_filter_numeric';
   $handlers[] = 'views_handler_filter_float';
   $handlers[] = 'views_handler_filter_date';
   $handlers[] = 'views_handler_filter_many_to_one';

   // relationship handlers
   $handlers[] = 'views_handler_relationship';

   // sort handlers
   $handlers[] = 'views_handler_sort';
   $handlers[] = 'views_handler_sort_formula';
   $handlers[] = 'views_handler_sort_date';
   $handlers[] = 'views_handler_sort_menu_hierarchy';
   $handlers[] = 'views_handler_sort_random';
   return $handlers;
 }
