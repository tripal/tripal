<?php

/**
 * Retrieve the views integration setup with the lightest priority for a given table
 *
 * NOTE: Uses lightest priority (drupal-style) where the range is from -10 to 10
 * and -10 is of highest priority.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a materialized 
 *   view or a chado table
 *
 * @return
 *   On success, the setup_id to use for integration of this table; otherwise FALSE
 */
function tripal_views_get_lightest_priority_setup ($table_name) {

  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name='%s' ORDER BY priority ASC";
  $setup = db_fetch_object(db_query($sql, $table_name));
  if ($setup) {
    return $setup->setup_id;  
  } else {
    return FALSE;
  }
}

/**
 * Checks if you are dealing with the lightest priority setup for a given table
 */
function tripal_views_is_lightest_priority_setup ($setup_id, $table_name) {
  $lightest_priority_setup_id = tripal_views_get_lightest_priority_setup ($table_name);
  if ($lightest_priority_setup_id == $setup_id) {
    return TRUE;
  } else {
    return FALSE;
  }
}

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
  tripal_views_integration_add_entry($defn_array);
 * @endcode
 */
function tripal_views_integration_add_entry($defn_array) {
  $no_errors = TRUE;
  
  // First insert into tripal_views
  $view_record = array(
    'table_name' => $defn_array['table'],
    'name' => $defn_array['name'],
    'comment' => $defn_array['description'],
    'priority' => $defn_array['priority'],
  );
  if ($defn_array['type'] == 'mview') {
      $mview = db_fetch_object(db_query("SELECT mview_id FROM tripal_mviews WHERE mv_table='%s'",$defn_array['table']));
      $view_record['mview_id'] = $mview->mview_id;
      if (!$mview->mview_id) {
        return FALSE;
      }
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
 * Removes a View Integration Entry
 *
 * @param $table_name
 *   The name of the table to remove a views integration entry for
 * @param $priority
 *   The priority of the of views integration entry
 *
 * @return
 *   TRUE on Success; FALSE otherwise
 */
function tripal_views_integration_remove_entry_by_table_name ($table_name, $priority) {

  $views = db_fetch_object(db_query("SELECT * FROM {tripal_views} WHERE table_name='%s' AND priority=%d",$table_name,$priority));
  if ($views->setup_id) {
    tripal_views_integration_remove_entry_by_setup_id($views->setup_id);
    return TRUE;  
  } else {
    return FALSE;
  }
  
}

/**
 * Removes a View Integration Entry
 *
 * @param $setup_id
 *   The setup ID of the views integration entry to remove
 */
function tripal_views_integration_remove_entry_by_setup_id ($setup_id) {

    db_query('DELETE FROM {tripal_views} WHERE setup_id=%d',$setup_id);
    db_query('DELETE FROM {tripal_views_field} WHERE setup_id=%d',$setup_id);
    db_query('DELETE FROM {tripal_views_handlers} WHERE setup_id=%d',$setup_id);
    db_query('DELETE FROM {tripal_views_join} WHERE setup_id=%d',$setup_id);

}