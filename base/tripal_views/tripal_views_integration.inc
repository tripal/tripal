<?php

/**
 * Purpose: Provide Guidance to new Tripal Admin
 *
 * @return
 *   HTML Formatted text
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_description_page() {

  $text .= '<h3>Tripal Views Quick Links:</h3>';
  $text .= "<ul>
             <li><a href=\"".url("admin/tripal/views/integration/mviews") . "\">List of integrated tables</a></li>
             <li><a href=\"".url("admin/tripal/views/integration/mviews/new"). "\">Integrate a new table</a></li>
           </ul>";

  $text .= '<h3>Views Integration Description:</h3>';
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
function tripal_views_integration_setup_list(){

   $header = array('', 'Drupal Views Type Name', 'Table Name', 'Is Mview', 'Priority', 'Comment','');
   $rows = array();

   // get the list of materialized views
   $tviews = db_query('SELECT * FROM {tripal_views} ORDER BY table_name, priority');
   while($tview = db_fetch_object($tviews)){
         $rows[] = array(
            l('Edit',"admin/tripal/views/integration/edit/".$tview->setup_id) ,
            $tview->name, 
            $tview->table_name, 
            ($tview->mview_id) ? 'Yes' : 'No', 
            $tview->priority,
            $tview->comment,
            l('Delete',"admin/tripal/views/integration/delete/".$tview->setup_id),
         );         
   }
   $rows[] = array(
      'data' => array( 
         array('data' => l('Add a new entry',"admin/tripal/views/integration/new") . " | " .
                         l("Create View",'admin/build/views/add'), 
               'colspan' => 7),
         )
   );
   return theme('table', $header, $rows);
}

/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_delete($setup_id){
  tripal_views_integration_remove_entry_by_setup_id ($setup_id);
  drupal_set_message("Record Deleted");
  drupal_goto('admin/tripal/views/integration');
}

/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_form(&$form_state, $setup_id = NULL){
  
   $form = array();
   $data = array();
   $form['#cache'] = TRUE;

   // ahah_helper requires us to register the form with it's module
   ahah_helper_register($form, $form_state);

   // if a setup_id is provided then we want to get the form defaults
   $setup_obj = array();
   if(isset($setup_id)){
      // get the deafult setup values
      $sql = "SELECT * FROM {tripal_views} WHERE setup_id = %d";
      $setup_obj = db_fetch_object(db_query($sql,$setup_id));
      $mview_id = $setup_obj->mview_id;
      $table_name = $setup_obj->table_name;
      $form_state['storage']['mview_id'] = $mview_id;
      $form_state['storage']['table_name'] = $table_name;

      // get the default field name/description
      $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=%d";
      $query = db_query($sql,$setup_id);
      $default_fields = array();
      while ($field = db_fetch_object($query)) {
        $default_fields[$field->column_name]['name'] = $field->name;
        $default_fields[$field->column_name]['description'] = $field->description;
      }
      
      // get the default join settings and handlers
      $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = %d";
      $query = db_query($sql,$setup_id);
      $default_joins = array();
      while ($join = db_fetch_object($query)){
         $default_joins[$join->base_field]['left_table'] = $join->left_table;
         $default_joins[$join->base_field]['left_field'] = $join->left_field;
      }

      // get the default handlers
      $sql = "SELECT * FROM {tripal_views_handlers} WHERE setup_id = %d";
      $query = db_query($sql,$setup_id);
      $default_handlers = array();
      while ($handler = db_fetch_object($query)){
         $default_handlers[$handler->column_name][$handler->handler_type]['handler_name'] = $handler->handler_name;
         $default_handlers[$handler->column_name][$handler->handler_type]['arguments'] = $handler->arguments;
      }
      
      // get the default join handlers
      $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = %d";
      $query = db_query($sql,$setup_id);
      while ($handler = db_fetch_object($query)){
         $default_handlers[$handler->base_field]['join']['handler_name'] = $handler->handler;
         //$default_handlers[$handler->base_field]['join']['arguments'] = $handler->arguments;
      }   
      
      // add in the setup_id for the form so we know this is an update not an insert
      $form['setup_id'] = array(
         '#type' => 'hidden',
         '#value' => $setup_id,
      );    
   }
   // add a fieldset for the MView & Chado table selectors
   $form['base_table_type'] = array(
     '#type' => 'fieldset',
     '#title' => 'Base Table',
     '#description' => 'Please select either a materialized view or a Chado table for integration with '.
                       'Drupal Views.  In Drupal Views terminology, the selected table becomes the "base table". '.
                       'After you select a table from either list, the fields from that table will appear below '.
                       'and you can specify other tables to join with and handlers.',
   );


   // build the form element that lists the materialized views
   $query = db_query("SELECT mview_id,name FROM {tripal_mviews} ORDER BY name");
   $mview_tables = array();
   $mview_tables['0'] = 'Select';
   while ($mview = db_fetch_object($query)){
     $mview_tables[$mview->mview_id] = $mview->name;
   }
   $form['base_table_type']['mview_id'] = array(
      '#title' => t('Materialized View'),
      '#type' => 'select',
      '#options' => $mview_tables,
      '#description' => 'Which materialized view to use.',
      '#default_value' => $setup_obj->mview_id,
      '#ahah' => array(
         'path' => ahah_helper_path(array('view_setup_table')),
         'wrapper' => 'table-rows-div',
         'effect' => 'fade',
         'event' => 'change',
         'method' => 'replace',
      ),
  );

  // build the form element for the Chado tables
  $chado_tables = tripal_core_get_chado_tables();
  $chado_tables = array_merge(array('Select',), $chado_tables);
  $form['base_table_type']['table_name'] = array(
    '#title' => t('Chado Table'),
    '#type' => 'select',
    '#options' => $chado_tables,
    '#description' => 'Which Chado table to use.',
    '#default_value' => (!$setup_obj->mview_id) ? $setup_obj->table_name : '',
    '#ahah' => array(
       'path' => ahah_helper_path(array('view_setup_table')),
       'wrapper' => 'table-rows-div',
       'effect' => 'fade',
       'event' => 'change',
       'method' => 'replace',
    ),
  );

  $form['views_type'] = array(
     '#type' => 'fieldset',
     '#title' => 'View Type',
     '#description' => 'Here you can provide the "type" of View you want to create.',
   );
  
  // field for the name of the
  $form['views_type']['row_name'] = array(
    '#title' => t('View Type Name'),
    '#type' => 'textfield',
    '#default_value' => $setup_obj->name,
    '#size' => 60,
    '#maxlength' => 128,
    '#description' => 'Provide the view type name.  This is the name that will appear in '.
                      'the Drupal Views interface when adding a new view.  The view type name '.
                      'must be unique.',
    '#required' => TRUE,   
  );
  
  if(isset($setup_id)){
    $form['row_name']['#attributes'] = array('readonly' => 'readonly');
  }
  
  $priorities = array();
  foreach (range(-10,10) as $v) {
    $priorities[$v] = (string) $v;
  }
  $form['views_type']['row_priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#description' => t('The level of priority your Views integration has in relation to the '
      .'default core and module definitions. The views integration definition with the '
      .'lightest priority will be used. For example, if there is a definition created by '
      .'core with a priority of 10 and another by a custom module of 5 and yours is -1 then '
      .'you definition will be used for that table because -1 is lighter then both 5 and 10.'),
    '#options' => $priorities,
    '#default_value' => (isset($setup_obj->priority)) ? $setup_obj->priority : -1,
  );
  
  $form['views_type']['row_description'] = array(
    '#title' => t('Comment'),
    '#type' => 'textarea',
    '#description' => '(Optional). Provide any details regarding this setup you would like. This '.
                      'description will appear when selecting a type for a new Drupal View',
    '#required' => FALSE,
  );

  // we need a div block where the table fields will get put when the
  // AHAH callback is made
  $form['view_setup_table'] = array(
     '#type' => 'item',
       '#prefix' => '<div id="table-rows-div">',
       '#suffix' => '</div>',
  );


  // add the fieldset for the table fields, but only if the $mview_id or $table_name
  // is set. The only times these values are set is if we're editing an existing
  // record or if the AHAH callback is being made.
  if ($form_state['storage']['mview_id'] or $form_state['storage']['table_name']){
    $mview_id = $form_state['storage']['mview_id'];
    $table_name = $form_state['storage']['table_name'];
    $form['view_setup_table'] = array(
     '#type' => 'fieldset',
     '#title' => 'Join Selection',
     '#prefix' => '<div id="fieldset-table-rows-wrapper">',
     '#suffix' => '</div>',
    );

    // get the columns in this materialized view.  They are separated by commas
    // where the first word is the column name and the rest is the type
    $columns = array();
    if($mview_id){
       $sql = "SELECT mv_specs FROM {tripal_mviews} WHERE mview_id = %d";
       $mview = db_fetch_object(db_query($sql,$mview_id));
       $columns = explode(",",$mview->mv_specs);
    } else {
       $table_desc = module_invoke_all('chado_'.$table_name.'_schema');
       $fields = $table_desc['fields'];
       // iterate through the columns and build the format
       // compatible with the code below. The column name is first followed
       // by the type with a separating space
       foreach($fields as $column => $attrs){
          $columns[] = "$column ".$attrs['type'];
       }
    }

    $i=1;
    $form['view_setup_table']["instructions"] = array(
         '#type' => 'markup',
         '#value' => "Select an optional table to which the fields of the ".
                     "materialized view can join.  If a field does not need to ".
                     "join you may leave the selection blank.",
    );
    $data['field_types'] = array();

    // get the list of chado tables to join on
    $chado_join_tables = tripal_core_get_chado_tables();
    $chado_join_tables = array_merge(array('Select a Join Table',), $chado_join_tables);

    // get list of all handlers
    $all_handlers = tripal_views_integration_discover_handlers();
    $handlers_fields = array(0 => "Select a field handler"); 
    $handlers_filters = array(0 => "Select a filter handler");
    $handlers_sort = array(0 => "Select a sort handler");
    $handlers_argument = array(0 => "Select an argument handler");
    $handlers_join = array(0 => "Select a join handler");
    $handlers_rel = array(0 => "Select a relationship handler");
    foreach($all_handlers as $handler){
       if(preg_match("/views_handler_field/",$handler)){
          $handlers_fields[$handler] = $handler;
       }
       if(preg_match("/views_handler_filter/",$handler)){
          $handlers_filters[$handler] = $handler;
       }
       if(preg_match("/views_handler_sort/",$handler)){
          $handlers_sort[$handler] = $handler;
       }
       if(preg_match("/views_handler_argument/",$handler)){
          $handlers_argument[$handler] = $handler;
       }
       if(preg_match("/_join/",$handler)){
          $handlers_join[$handler] = $handler;
       }
       if(preg_match("/views_handler_relationship/",$handler)){
          $handlers_rel[$handler] = $handler;
       }
    }

    // generate a unique $table_id for keeping track of the table
    if($mview_id){
       $table_id = $mview_id;
    } else {
       $table_id = $table_name;
    }

    // Per Row (Fields) --------------
    // now iterate through the columns of the materialized view or 
    // chado table and generate the join and handler fields
    foreach ($columns as $column){
      $column = trim($column);  // trim trailing and leading spaces
      preg_match("/^(.*?)\ (.*?)$/",$column,$matches);
      $column_name = $matches[1];
      $column_type = $matches[2];

      $form['view_setup_table']["$table_id-$i"] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"fields-new-row\">",
           '#suffix' => "</div>"
      );

      // COLUMN I
      $form['view_setup_table']["$table_id-$i"]["fields_name_$table_id-$i"] = array(
        '#type' => 'markup',
        '#prefix' => "<div class=\"column-one\">", 
        '#value' => "<span class=\"column-name\">$column_name</span>".
                    "<br><span class=\"column-type\">$column_type</span>",
        '#suffix' => "</div>",
      );
      $data['field_types'][$column_name] = $column_type;
  
      // COLUMN II
      $form['view_setup_table']["$table_id-$i"]['column-2'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"column-two\">",
           '#suffix' => "</div>"
      );
      
      // set the default values for the human-readable name and description
      $default_name = '';
      $default_descrip = '';
      if (isset($setup_id) && !isset($form_state['storage']["fields_readable_name_$table_id-$i"])){
        $default_name = $default_fields[$column_name]['name'];
        $default_descrip = $default_fields[$column_name]['description'];
      } else {
        $default_name = $form_state['storage']["fields_readable_name_$table_id-$i"];
        $default_descrip = $form_state['storage']["fields_description_$table_id-$i"];
      }
      $form['view_setup_table']["$table_id-$i"]['column-2']["fields_readable_name_$table_id-$i"] = array(
        '#type' => 'textfield',
        '#title' => 'Human-Readable Name',
        '#description' => 'This is the name of the field in the Views UI',
        '#required' => TRUE,
        '#default_value' => $default_name,
      );
      
      $form['view_setup_table']["$table_id-$i"]['column-2']["fields_description_$table_id-$i"] = array(
        '#type' => 'textarea',
        '#title' => 'Short Description',
        '#description' => 'This is the field help in the Views UI',
        '#required' => TRUE,
        '#cols' => 42,
        '#rows' => 3,
        '#default_value' => $default_descrip,
      );
      
      // COLUMN III
      $form['view_setup_table']["$table_id-$i"]['column-3'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"column-three\">",
           '#suffix' => "</div>"
      );
      
      // set the default values for the join table and columns
      $default_join_table = 0;
      $default_join_field = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_join_$table_id-$i"])){
      	$default_join_table = $default_joins[$column_name]['left_table'];
      	$default_join_field = $default_joins[$column_name]['left_field'];
      	$form_state['storage']["fields_join_$table_id-$i"] = $default_join_table;
      	$form_state['storage']["fields_join_column_$table_id-$i"] = $default_join_field;
      }
      else{
      	$default_join_table = $form_state['storage']["fields_join_$table_id-$i"];
      	$default_join_field = $form_state['storage']["fields_join_column_$table_id-$i"];
      }

      $form['view_setup_table']["$table_id-$i"]['column-3']["fields_join_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-column-join\">",
        '#suffix' => "</div>",
        '#options' => $chado_join_tables,
        '#required' => FALSE,
        '#default_value' => $default_join_table,
        '#ahah' => array(
           'path' => ahah_helper_path(array("view_setup_table","$table_id-$i",'column-3',"fields_join_column_$table_id-$i")),
           'wrapper' => "fields-column-join-column-$table_id-$i",
           'effect' => 'fade',
           'event' => 'change',
           'method' => 'replace',
         ),
      );
      
      $columns = array();
      if($default_join_table){
        $table_desc = module_invoke_all('chado_'.$default_join_table.'_schema');
        foreach ($table_desc['fields'] as $column => $def){
           $columns[$column] = $column;
        }
      } else {
        $columns = array('Select Join Column');
      }
      $form['view_setup_table']["$table_id-$i"]['column-3']["fields_join_column_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "  <div id=\"fields-column-join-column-$table_id-$i\" class=\"fields-column-join-column\">",
        '#suffix' => "</div>",
        '#options' => $columns,
        '#required' => FALSE,
        '#default_value' => $default_join_field
      );

      $default_join_handler = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_join_handler_$table_id-$i"])){
         $default_join_handler = $default_handlers[$column_name]['join']['handler_name'];
         $form_state['storage']["fields_join_handler_$table_id-$i"]=$default_join_handler;
      }
      else {
         $default_join_handler = $form_state['storage']["fields_join_handler_$table_id-$i"];
      }
      $form['view_setup_table']["$table_id-$i"]['column-3']["fields_join_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-join-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_join,
        '#required' => FALSE,
         '#default_value' => $default_join_handler,
      );
      
      // COLUMN 4
      $form['view_setup_table']["$table_id-$i"]['column-4'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"column-four\">",
           '#suffix' => "</div>"
      );
      
      // create the handler fields
      $default_field_handler = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_field_handler_$table_id-$i"])){
         $default_field_handler = $default_handlers[$column_name]['field']['handler_name'];
         $form_state['storage']["fields_field_handler_$table_id-$i"] = $default_field_handler;
      }
      else {
      	$default_field_handler = $form_state['storage']["fields_field_handler_$table_id-$i"];
         if(!$default_field_handler){
            if($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial'){
               $default_field_handler = 'chado_views_handler_field_numeric';
            }
            elseif(preg_match("/character varying/",$column_type) or $column_type == 'char' or $column_type == 'text'){
               $default_field_handler = 'chado_views_handler_field';
            }
            elseif($column_type == 'boolean'){
               $default_field_handler = 'chado_views_handler_field_boolean';
            }
            elseif($column_type == 'float'){
               $default_field_handler = 'chado_views_handler_field_numeric';
            }
            elseif($column_type == 'datetime'){
               $default_field_handler = 'chado_views_handler_field_date';
            }
         }
      }

      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_field_handler_$table_id-$i"] = array(
         '#type' => 'select',
         '#prefix' => "<div class=\"fields-field-handler\">",
         '#suffix' => "</div>",
         '#options' => $handlers_fields,
         '#required' => FALSE,
         '#default_value' => $default_field_handler,
      );

      $default_filter_handler = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_filter_handler_$table_id-$i"])){
         $default_filter_handler = $default_handlers[$column_name]['filter']['handler_name'];
         $form_state['storage']["fields_filter_handler_$table_id-$i"]= $default_filter_handler;
      }
      else {
         $default_filter_handler = $form_state['storage']["fields_filter_handler_$table_id-$i"];
         if(!$default_filter_handler){
            if($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial'){
               $default_filter_handler = 'chado_views_handler_filter_numeric';
            }
            elseif(preg_match("/^character varying/",$column_type) or $column_type == 'char' or $column_type == 'text'){
               $default_filter_handler = 'chado_views_handler_filter_string';
            }
            elseif($column_type == 'boolean'){
               $default_filter_handler = 'chado_views_handler_filter_boolean';
            }
            elseif($column_type == 'float'){
               $default_filter_handler = 'chado_views_handler_filter_float';
            }
            elseif($column_type == 'datetime'){
               $default_filter_handler = 'chado_views_handler_filter_date';
            }
         }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_filter_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-filter-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_filters,
        '#required' => FALSE,
        '#default_value' => $default_filter_handler,
      );

      $default_sort_handler = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_sort_handler_$table_id-$i"])){
         $default_sort_handler = $default_handlers[$column_name]['sort']['handler_name'];
         $form_state['storage']["fields_sort_handler_$table_id-$i"] = $default_sort_handler;
      }
      else {
         $default_sort_handler = $form_state['storage']["fields_sort_handler_$table_id-$i"];
         if(!$default_sort_handler){
            if($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial'){
               $default_sort_handler = 'chado_views_handler_sort';
            }
            elseif(preg_match("/character varying/",$column_type) or $column_type == 'char' or $column_type == 'text'){
               $default_sort_handler = 'chado_views_handler_sort';
            }
            elseif($column_type == 'boolean'){
               $default_sort_handler = 'chado_views_handler_sort';
            }
            elseif($column_type == 'float'){
               $default_sort_handler = 'chado_views_handler_sort';
            }
            elseif($column_type == 'datetime'){
               $default_sort_handler = 'chado_views_handler_sort_date';
            }
         }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_sort_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-sort-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_sort,
        '#required' => FALSE,
        '#default_value' => $default_sort_handler,
      );

      $default_argument_handler = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_argument_handler_$table_id-$i"])){
         $default_argument_handler = $default_handlers[$column_name]['argument']['handler_name'];
         $form_state['storage']["fields_argument_handler_$table_id-$i"]=$default_argument_handler ;
      }
      else {
         $default_argument_handler = $form_state['storage']["fields_argument_handler_$table_id-$i"];
         if(!$default_argument_handler){
            if($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial'){
               $default_argument_handler = 'views_handler_argument_numeric';
            }
            elseif(preg_match("/character varying/",$column_type) or $column_type == 'char' or $column_type == 'text'){
               $default_argument_handler = 'views_handler_argument_string';
            }
            elseif($column_type == 'boolean'){
               $default_argument_handler = 'views_handler_argument_numeric';
            }
            elseif($column_type == 'float'){
               $default_argument_handler = 'views_handler_argument_numeric';
            }
            elseif($column_type == 'datetime'){
               $default_argument_handler = 'views_handler_argument_date';
            }
         }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_argument_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-argument-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_argument,
        '#required' => FALSE,
        '#default_value' => $default_argument_handler,
      );

      $default_relationship_handler = 0;
      if(isset($setup_id) && !isset($form_state['storage']["fields_relationship_handler_$table_id-$i"])){
         $default_relationship_handler = $default_handlers[$column_name]['relationship']['handler_name'];
         $form_state['storage']["fields_relationship_handler_$table_id-$i"]=$default_relationship_handler;
      }
      else {
         $default_relationship_handler = $form_state['storage']["fields_relationship_handler_$table_id-$i"];
         if(!$default_relationship_handler){
            if($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial'){
               $default_relationship_handler = 'views_handler_relationship';
            }
            elseif(preg_match("/character varying/",$column_type) or $column_type == 'char' or $column_type == 'text'){
               $default_relationship_handler = 'views_handler_relationship';
            }
            elseif($column_type == 'boolean'){
               $default_relationship_handler = 'views_handler_relationship';
            }
            elseif($column_type == 'float'){
               $default_relationship_handler = 'views_handler_relationship';
            }
            elseif($column_type == 'datetime'){
               $default_relationship_handler = 'views_handler_relationship';
            }
         }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_relationship_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-relationship-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_rel,
        '#required' => FALSE,
        '#default_value' => $default_relationship_handler,
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

  $form['#redirect'] = 'admin/tripal/views/integration';   

  return $form;
}


/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_form_validate($form, &$form_state){
  $name_array = explode(" ", $form_state['values']['row_name']);
  $mview_id = $form_state['values']['mview_id'];
  $table_name = $form_state['values']['table_name'];

//  if(count($name_array) > 1){
//    form_set_error($form_state['values']['row_name'], 'The View type name must be a single word only.');
//  }
  if($mview_id and $table_name){
    form_set_error($form_state['values']['mview_id'], 'Please select either a materialized view or a Chado table but not both');
  }
  if(!$mview_id and !$table_name){
    form_set_error($form_state['values']['mview_id'], 'Please select either a materialized view or a Chado table');
  }
  // TODO: do we need to require that a handler be set for each field and each type of handler?
}

/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_form_submit($form, &$form_state){
   $name = $form_state['values']['row_name'];
   $mview_id = $form_state['values']['mview_id'];
   $table_name = $form_state['values']['table_name'];
   $setup_id = $form_state['values']['setup_id'];

   // get details about this mview
   if ($mview_id) {
     $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = $mview_id";
     $mview = db_fetch_object(db_query($sql));
     $table_name = $mview->mv_table;
     $table_id = $mview_id;
     $type = 'mview';
   } else {
     $type = 'chado';
     $table_id = $table_name;
   }

   // If this is for a materialized view then we want to add/update that record
   $tripal_views_record = array();
   if($mview_id){
      // get details about this mview
      $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = $mview_id";
      $mview = db_fetch_object(db_query($sql));      

      // build the record for insert/update
      $tripal_views_record = array(
       'mview_id' => $mview_id,
       'table_name' => $mview->mv_table,
       'name' => $name,
       'priority' => $form_state['values']['row_priority'],
       'comment' => $form_state['values']['row_description'],
       
      );
   } 
   // if a chado table then...
   if($table_name){
            // build the record for insert/update
      $tripal_views_record = array(
       'table_name' => $table_name,
       'name' => $name,
       'priority' => $form_state['values']['row_priority'],
       'comment' => $form_state['values']['row_description'],
      );
   }

   // perform the insert or update
   if(!$setup_id){  // this is an insert
      if(!drupal_write_record('tripal_views', $tripal_views_record)){
         drupal_set_message("Failed to add record.");
         return;
      }
   } else {  // this is an update
      $tripal_views_record['setup_id'] = $setup_id;
      if(!drupal_write_record('tripal_views', $tripal_views_record,array('setup_id'))){
         drupal_set_message("Failed to update record.");
         return;
      }
   }
   

   
   // if this is an update then clean out the existing joins and handlers so we can add new ones
   if($setup_id){  
      db_query("DELETE FROM {tripal_views_field} WHERE setup_id = %d",$setup_id);
      db_query("DELETE FROM {tripal_views_join} WHERE setup_id = %d",$setup_id);
      db_query("DELETE FROM {tripal_views_handlers} WHERE setup_id = %d",$setup_id);
   }

   // iterate through the columns of the form and add
   // the joins if provided, and the handlers
   $i = 1;
   foreach ($form_state['values']['field_types'] as $key => $value){

      // add the field definition
      $view_field_record = array(
        'setup_id' => $tripal_views_record['setup_id'],
        'column_name' => $key, 
        'name' => $form_state['values']["fields_readable_name_$table_id-$i"], 
        'description' => $form_state['values']["fields_description_$table_id-$i"], 
        'type' => $value,
      );
      drupal_write_record('tripal_views_field', $view_field_record);
      
      // first add the join if it exists
      $left_table = $form_state['values']["fields_join_$table_id-$i"];
      $left_column = $form_state['values']["fields_join_column_$table_id-$i"];

      if($left_column){
         $view_join_record = array(
           'setup_id' => $tripal_views_record['setup_id'],
           'base_table' => $mview->mv_table,
           'base_field' => $key,
           'left_table' => $left_table,
           'left_field' => $left_column,
           'handler' => $form_state['values']["fields_join_handler_$table_id-$i"],
         );
         
         // write the new joins to the database
         drupal_write_record('tripal_views_join', $view_join_record);
      }

      // add the hanlders
      $handlers = array('filter','field','sort','argument','relationship');
     
      foreach($handlers as $handler){
         $handler_name = $form_state['values']["fields_".$handler."_handler_$table_id-$i"];
         if($handler_name){
            $handler_record = array(
            'setup_id' => $tripal_views_record['setup_id'],
            'column_name' => $key, 
            'handler_type' => $handler,
            'handler_name' => $handler_name,
            );
            drupal_write_record('tripal_views_handlers', $handler_record);
         }
      }
      $i++;
   }
   
   if($setup_id){
      drupal_set_message('Record Updated');
   } else {
      drupal_set_message('Record Added');
   }
   // now clear all the caches so that Drupal views picks up our chages
   views_invalidate_cache();
}

/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_discover_handlers() {

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
   // get the views_handlers() function to be called
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
   
   // join handler
   $handlers[] = 'views_join';
   
   return $handlers;
 }
