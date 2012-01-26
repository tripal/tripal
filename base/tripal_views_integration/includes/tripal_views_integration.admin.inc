<?php
/**
 * Purpose: Provide Guidance to new Tripal Admin
 *
 * @return 
 *   HTML Formatted text
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_module_description_page() {

  $text .= '<h3>Tripal Views Integration Administrative Tools Quick Links:</h3>';
  $text .= "<ul>
             <li><a href=\"".url("admin/tripal/tripal_views_integration/list") . "\">List of integrated views</a></li>
             <li><a href=\"".url("admin/tripal/tripal_views_integration/new"). "\">Setup integration of a materialized view</a></li>
           </ul>";
#             <li><a href=\"".url("admin/tripal/tripal_feature/aggregate"). "\">Feature Relationship Aggegators</a></li> 

  $text .= '<h3>Module Description:</h3>';
  $text .= '<p>This module provides an interface for integrating <a href="http://drupal.org/project/views">Drupal Views</a>
            with Tripal materialized views.  This will allow site administrators to create custom queries for the materialized views
            and in turn provide custom content pages, custom blocks and custom search forms. The forms allow a site administrator
            to select a materialized view and associate other Chado tables on which the view can join.  Usage of this module requires
            a good understanding of foreign-key relationships in Chado.
            </p>';

  $text .= '<h3>Setup Instructions:</h3>';
  $text .= '<p>After installation of the feature module.  The following tasks should be performed
            <ol>
              <li><b>Install Drupal Views</b>: The <a href="http://drupal.org/project/views">Drupal Views</a> module
                must first be installed before this module can be used.  If you are reading this page then you must have
                Drupal Views installed</li>
              <li><b>Set Permissions</b>: To allow access to site administrators for this module, simply
               <a href="'.url('admin/user/permissions').'">assign permissions</a> to the appropriate user roles for the
               permission type "manage tripal_views_integration". </li>
            </ol>
            </p>';
  $text .= '<h3>Usage Instructions:</h3>';
  $text .= "<p>To use this module follow these steps:
            <ol>
               <li><b>Identify or create a materialized view:</b> Using the <a href=\"".url("admin/tripal/tripal_mviews") . "\">
                 Tripal materialized View</a> interface, identify the view you would like to integrate or create a new one.</li>
               <li><b>Setup the Views Integration</b>: Navigate to the <a href=\"".url("admin/tripal/tripal_views_integration/new") . "\">
                 Tripal views integration setup page</a> to integrate the selected materialized view.  Provide a  user friendly name
                 and description to help you remember the purpose for integrating the view.  Next, select the view you want to integrate
                 from the provided select box.  If your materialized view has fields that can join with other Chado tables, you may 
                 provide those relationships in the provided form.  Finally, if your fields require a special handler for display, you
                 may select it from the drop down provided</li>
               <li><b>Create custom pages/block/search form</b>:  After saving setup information from step 2 above, you will be redirected to the
                 Drupal Views interface</a> where you can create a custom page, block or search form.</li>
               <li><b>Review your integrated views</b>:  A page providing a  
                 <a href=\"".url("admin/tripal/tripal_views_integration/list") . "\">list of all integrated views</a> is provided. You may
                 view this page to see all integrated views, but also to remove any unwanted integrations.</li>
            </ol>
         
            </p>";  
  $text .= '<h3>Features of this Module:</h3>';
  $text .= '<p>This module provides the following functionality
            <ul>
              <li>A dynamic form for integration of an existing materialized view with Drupal Views.  This form allows the site
                  administrator to select an existing view and indicate Chado tables on which the fields may join.</li>
            </ul> 
            
            </p>';
  return $text;
}
/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_admin_form(){

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
   );

   $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel '), 'admin/tripal/'),
   );

   $form['new'] = array(
    '#type' => 'markup',
    '#value' => l(t(' New'), 'admin/tripal/tripal_views_integration/new'),
   );

   return $form;
}

/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_admin_form_submit($form, &$form_state){
   $value = $form['existing_rows']['#options'][$form_state['values']['existing_rows']];
   db_query("DELETE FROM public.tripal_views_integration WHERE setup_id = $value;");
   db_query("DELETE FROM public.tripal_views_handlers WHERE setup_id = $value;");
   db_query("DELETE FROM public.tripal_mviews_join WHERE setup_id = $value;");
}
/**
 *
 * @ingroup tripal_views_integration
 */

function tripal_views_integration_new_setup_form(&$form_state){
   $form = array();

   $form['#cache'] = TRUE;

   //ahah_helper requires this to register the form with it's module
   ahah_helper_register($form, $form_state);

   // field for the name of the 
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

   $form['mview_id'] = array(
      '#title' => t('Materialized View'),
      '#type' => 'select',
      '#options' => $mview_options,
      '#description' => 'Which materialized view to use.',
      '#required' => TRUE,
      '#default_value' => $mview_default_value,
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

      $mview_id = $form_state['storage']['mview_id'];

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
      $chado_tables = array_merge(array('',), $chado_tables);

      $handlers = array();
      $form['view_setup_table']["instructions"] = array(
         '#type' => 'markup',
         '#value' => "Select an optional table to which the fields of the materialized view can join.  If a field does not need to join you may leave the selection blank.",
      );
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

         $form['view_setup_table']["fields_start_$mview_id-$i"] = array(
           '#type' => 'markup',
           '#value' => "<div class=\"fields-new-row\">",
         );

         $form['view_setup_table']["fields_name_$mview_id-$i"] = array(
           '#type' => 'markup',
           '#attributes' => array('class' => 'fields-column-name'),
           '#value' => "<div class=\"column-id\"><span class=\"column-name\">$column_name</span>".
                       "<br><span class=\"column-type\">$column_type</span></div>",
         );

         $table = $form_state['storage']["fields_join_$mview_id-$i"];
         $form['view_setup_table']["fields_join_$mview_id-$i"] = array(
            '#type' => 'select',
            '#prefix' => "<div class=\"fields-column-join\">",
            '#suffix' => "</div>",
            '#options' => $chado_tables,
            '#required' => FALSE,
            '#default_value' => $table,
            '#ahah' => array(
               'path' => ahah_helper_path(array('view_setup_table',"fields_join_column_$mview_id-$i")),
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
            $columns = array();
         }
         $form['view_setup_table']["fields_join_column_$mview_id-$i"] = array(
            '#type' => 'select',
            '#prefix' => "<div id=\"fields-column-join-column-$mview_id-$i\" class=\"fields-column-join-column\">",
            '#suffix' => "</div>",
            '#options' => $columns,
            '#required' => FALSE,
         );
         $form['view_setup_table']["fields_handler_$mview_id-$i"] = array(
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
   
   $form['save'] = array(
           '#type'  => 'submit',
           '#value' => t('Save'),
   );

   return $form;
}


/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_new_setup_form_submit($form, &$form_state){

}

