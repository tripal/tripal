<?php

/**
 * Add views integration records into the tripal_views* tables
 *
 * @param $defn_array
 *   An array describing the structure and fields of the table
 *
 * @return
 *   True/False if completed successfully/not
 *
 * Example usage (in hook_install()):
 * @code
  $defn_array = array(
    'table' => 'feature', //tablename or materialized view name
    'name' => 'Sequence Features', // Human readable name
    'type' => 'chado', //either chado or mview depending on tablename
    'description' => 'Create a listing of features.', //description seen when creating a view of this type
    'priority' => 10, //For Base tripal modules: 10; custom modules: 9 to 0;
    'fields' => array(
      'feature_id' => array(
        'name' => 'feature_id', //field name in database
        'title' => 'Feature ID', //human-readable name -seen in Views UI
        'description' => 'This is the unique identifier for features', //help/description seen in Views UI
        'type' => 'int', // the type of field
        'handlers' => array(  //possible keys are field, filter, sort, argument, relationship
          'field' => array(
            'name' => 'chado_views_handler_numeric' //name of handler
          ),
          'filter' => array( ... ),
          ...
        ),
        'join' => array( //describe a table that joins to this one via this field
          'table' => 'featureprop', //table to join to
          'field' => 'feature_id', //field in above table (featureprop)
          'handler' => 'views_handler_join_chado_aggregator', //handler to use
        ),
      )
    ),
  );
  tripal_core_views_integration_add_entry($defn_array);
 * @endcode
 */
function tripal_core_views_integration_add_entry($defn_array) {
  $no_errors = TRUE;
  
  // First insert into tripal_views
  $view_record = array(
    'name' => $defn_array['name'],
    'comment' => $defn_array['description'],
    'priority' => $defn_array['priority'],
  );
  switch($defn_array['type']) {
    case 'chado':
      $view_record['table_name'] = $defn_array['table'];
      break;
    case 'mview':
      $mview = db_fetch_object(db_query("SELECT mview_id FROM tripal_mviews WHERE mv_table='%s'",$defn_array['table']));
      $view_record['mview_id'] = $mview->mview_id;
      $view_record['table_name'] = $defn_array['table'];
      break;
  }
  $status = drupal_write_record('tripal_views',$view_record);
  
  if ($status) {
    
    // Insert Field Definitions
    foreach ($defn_array['fields'] as $field) {
      $field_record = array(
      	'setup_id' => $view_record['setup_id'],
        'column_name' => $field['name'],
        'name' => $field['title'],
        'description' => $field['description'],
        'type' => $field['type'],
      );
      $status = drupal_write_record('tripal_views_field',$field_record);
      
      if ($status) {
      
        // Insert Handler Definitions
        foreach ($field['handlers'] as $handler_type => $handler) {
          $handler_record = array(
            'setup_id' => $view_record['setup_id'],
            'column_name' => $field['name'],
            'handler_type' => $handler_type,
            'handler_name' => $handler['name'],
            'arguments' => serialize($handler)
          );
          $status = drupal_write_record('tripal_views_handlers',$handler_record);
          if (!$status) {
            drupal_set_message('Unable to integrate '.$handler_type.' handler: '.$handler['name'], 'error');
            $no_errors = FALSE;
          }
        }
        
        // Insert Joins
        if (!is_array($field['joins'])) { $field['joins'] = array(); }
        foreach($field['joins'] as $join) {
          $join_record = array(
            'setup_id' => $view_record['setup_id'],
            'base_table' => $defn_array['table'],
            'base_field' => $field['name'],
            'left_table' => $join['table'],
            'left_field' => $join['field'],
          );

          if (!empty($join['handler'])) {
          	$join_record['handler'] = $join['handler'];
          } else {
          	$join_record['handler'] = 'views_join';
          }

          $status = drupal_write_record('tripal_views_join',$join_record);
          if (!$status) {
            drupal_set_message('Unable to join '.$join['table'].'.'.$join['field'].' with '.$defn_array['table'].'.'.$field['name'], 'error');
            $no_errors = FALSE;
          }
        }
        
      } else {
        drupal_set_message('Unable to integrate field: '.$field['name'],'error');
        $no_errors = FALSE;
      }
    }
    
  } else {
    drupal_set_message('Unable to set default views integration','error');
    $no_errors = FALSE;
  }
  
  return $no_errors;
}

/**
 *
 */
function tripal_core_views_integration_remove_entry($tablename, $priority) {

  $views = db_fetch_object(db_query("SELECT * FROM {tripal_views} WHERE table_name='%s'",$tablename));
  if ($views->setup_id) {
    db_query('DELETE FROM {tripal_views} WHERE setup_id=%d',$views->setup_id);
    db_query('DELETE FROM {tripal_views_field} WHERE setup_id=%d',$views->setup_id);
    db_query('DELETE FROM {tripal_views_handlers} WHERE setup_id=%d',$views->setup_id);
    db_query('DELETE FROM {tripal_views_join} WHERE setup_id=%d',$views->setup_id);
  }
}