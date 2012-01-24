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
function tripal_views_setup_new_search_form($form_state){
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

  $form['table-rows-div'] = array(
    '#type' => 'markup',
    '#value' => '<div id="table-rows-div">',
  );
  return $form;
}
/**
*
* @ingroup tripal_view_search
*/
function tripal_view_search_ajax_mview_cols(){
  dpm($_POST, 'post');
   $mview_id = $_POST['mview_id'];
   $form = drupal_get_form('tripal_views_setup_fields_form',$mview_id);
   drupal_json(array('status' => TRUE, 'data' => $form));
}
/**
*
* @ingroup tripal_view_search
*/
function tripal_views_setup_fields_form(&$form_state=NULL, $mview_id = NULL){

  if(!$mview_id){
      return;
   }

   // get the columns in this materialized view.  They are separated by commas
   // where the first word is the column name and the rest is the type
  $sql = "SELECT mv_specs FROM {tripal_mviews} WHERE mview_id = $mview_id";
  $mview = db_fetch_object(db_query($sql));
   $columns = explode(",",$mview->mv_specs);

   $i=1;
   $chado_tables = tripal_core_get_chado_tables();
   $handlers = array();
   $form["fields_headers"] = array(
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
     $form["fields_start_$i"] = array(
       '#type' => 'markup',
       '#value' => "<div class=\"fields-new-row\">",
     );

     $form["fields_column_name_$i"] = array(
       '#type' => 'markup',
         '#attributes' => array('class' => 'fields-column-name'),
       '#value' => "<div class=\"column-id\"><span class=\"column-name\">$column_name</span>".
                     "<br><span class=\"column-type\">$column_type</span></div>",
     );

      // second print the table join drop down
     $chado_tables = array_merge(array(NULL,), $chado_tables);
     $form["fields_column_join_$i"] = array(
       '#type' => 'select',
       '#prefix' => "<div class=\"fields-column-join\">",
       '#suffix' => "</div>",
       '#options' => $chado_tables,
       '#required' => FALSE,
       '#ahah' => array(
         'path' => 'admin/tripal/tripal_views_setup/ajax/field_col_join',
         'wrapper' => "fields-column-join-column-$i",
         'effect' => 'fade',
         'event' => 'change',
         'method' => 'replace',
        ),
     );
     $form["fields_column_join_column_$i"] = array(
       '#type' => 'select',
       '#prefix' => "<div id=\"fields-column-join-column-$i\" class=\"fields-column-join-column\">",
       '#suffix' => "</div>",
       '#options' => array(),
       // '#ahah' => array(
         // 'path' => 'admin/tripal/tripal_views_setup/ajax/field_col_join',
         // 'wrapper' => 'field-col-join-div',
         // 'effect' => 'fade',
         // 'event' => 'change',
         // 'method' => 'replace',
        // ),
       '#required' => FALSE,
     );
      $form["fields_column_handler_$i"] = array(
       '#type' => 'select',
         '#prefix' => "<div class=\"fields-column-handler\">",
         '#suffix' => "</div>",
       '#options' => $handlers,
       '#required' => FALSE,
     );
     $form["fields_end_$i"] = array(
       '#type' => 'markup',
       '#value' => "</div>",
     );
      $i++;
   }

/*	$form['row_view_column'] = array(
    '#title' => t('View Column'),
    '#type' => 'select',
    '#options' => array('stub'),
    '#description' => 'Which materialized view column to use.',
    '#required' => TRUE,

  );

   $form['row_chado_column'] = array(
    '#title' => t('Chado Column'),
    '#type' => 'select',
    '#options' => array('stub'),
    '#description' => 'Which Chado table column to use.',
    '#required' => TRUE,
   );
  */

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Create',
  );

   return $form;
}

function tripal_views_setup_fields_form_submit($form, &$form_state){
  dpm($form,'formfield');
  dpm($form_state, 'formstatefield');
}
// function rowmview_ahah(){
  // $query = "SELECT mv_specs FROM tripal_mviews;";
  // $row_options = db_fetch_array(db_query($query));
//
//
// }

// function tripal_views_setup_ajax_db_mview_columns(){
  // $mview = $_POST['row_mview'];
  // $form = drupal_get_form('tripal_views_setup_mview_column_form', $mview);
  // drupal_json(array('status' => TRUE, 'data' => $form));
// }

// function tripal_views_setup_mview_column_form(&$form_state = NULL, $mview = NULL){
  // if ($mview){
    // $mview_query = db_fetch_array(db_query("SELECT mv_specs FROM tripal_mviews WHERE name = $mview;"));
//
    // $form['row_mview']
  // }
//
// }

// function tripal_views_setup_ajax_db_chado_tables(){
// }

/**
*
* @ingroup tripal_view_search
*/
function tripal_view_search_ajax_field_col_join(){
  dpm($_POST, 'post');

}

/**
*
* @ingroup tripal_view_search
*/
function tripal_views_setup_new_search_form_submit($form, &$form_state){
  dpm($form,'form');
  dpm($form_state, 'formstate');
}
