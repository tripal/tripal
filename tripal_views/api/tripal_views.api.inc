<?php

/**
 * @file
 * API functions for Tripal Views Integration
 */

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
function tripal_views_get_lightest_priority_setup($table_name) {

  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name='%s' ORDER BY priority ASC";
  $setup = db_fetch_object(db_query($sql, $table_name));
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Check to see if this table already has an integration record with the given priority
 *
 * @param $table_name
 *   The name of the table to check for integration
 * @param $priority (optional)
 *   The priority of record to check for
 *
 * @return
 *  If the table is already integrated, the setup_id of the existing integration
 *  record is returned (If priority is not specified this will be the lightest record);
 *  Otherwise the table is not already integrated and FALSE is returned.
 */
function tripal_views_is_integrated($table_name, $priority = NULL) {

  if ($priority) {
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name='%s' AND priority=%d";
    $setup = db_fetch_object(db_query($sql, $table_name, $priority));
  }
  else {
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name='%s' ORDER BY priority ASC";
    $setup = db_fetch_object(db_query($sql, $table_name));
  }
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Checks if you are dealing with the lightest priority setup for a given table
 *
 * @param $setup_id
 *   The ID of the setup to check (is this setup the lightest one?)
 * @param $table_name
 *   The name of the table associated with this setup
 *
 * @return TRUE is this is the lightest priority; FALSE otherwise
 */
function tripal_views_is_lightest_priority_setup($setup_id, $table_name) {

  $lightest_priority_setup_id = tripal_views_get_lightest_priority_setup($table_name);
  if ($lightest_priority_setup_id == $setup_id) {
    return TRUE;
  }
  else {
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
    'base_table' => TRUE //either TRUE or FALSE depending on whether the current table should show up in the add view list
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
    'base_table' => $defn_array['base_table'],
  );
  if ($defn_array['type'] == 'mview') {
      $mview = db_fetch_object(db_query("SELECT mview_id FROM {tripal_mviews} WHERE mv_table='%s'", $defn_array['table']));
      $view_record['mview_id'] = $mview->mview_id;
      if (!$mview->mview_id) {
        return FALSE;
      }
  }
  if ($view_record['name'] && $view_record['comment']) {
    $status = drupal_write_record('tripal_views', $view_record);
  }
  else {
    $status = FALSE;
    drupal_set_message(t('Unable to integrate %table table due to a missing name or comment field.', array('%table' => $defn_array['table'])), 'error');
  }

  if ($status) {

    // Need to update the tripal_views record so base_table can be false
    // this is a fix because drupal_write_record() puts in defaults if !isset()
    // and a variable is considered not set if it's null!
    db_query(
      "UPDATE {tripal_views} SET base_table=%d WHERE table_name='%s' AND priority=%d",
      $defn_array['base_table'],
      $defn_array['table'],
      $defn_array['priority']
    );

    // Insert Field Definitions
    foreach ($defn_array['fields'] as $field) {
      $field_record = array(
        'setup_id' => $view_record['setup_id'],
        'column_name' => $field['name'],
        'name' => $field['title'],
        'description' => $field['description'],
        'type' => $field['type'],
      );
      if ($view_record['setup_id'] && $field['name'] && $field['title'] && $field['description'] && $field['type']) {
        $status = drupal_write_record('tripal_views_field', $field_record);
      }
      else {
        drupal_set_message(t('Unable to integrate %name field due to missing required fields.', array('%name' => $field['name'])), 'error');
        $status = FALSE;
      }

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
          if ($view_record['setup_id'] && $field['name'] && $handler_type && $handler['name'] && $handler) {
            $status = drupal_write_record('tripal_views_handlers', $handler_record);
          }
          else {
            $status = FALSE;
          }
          if (!$status) {
            drupal_set_message(t('Unable to integrate %handler_type handler: %handler_name', array('%handler_type' => $handler_type, '%handler_name' => $handler['name'])), 'error');
            $no_errors = FALSE;
          }
        }

        // Insert Joins
        if (!is_array($field['joins'])) {
          $field['joins'] = array();
        }
        foreach ($field['joins'] as $join) {
          $join_record = array(
            'setup_id' => $view_record['setup_id'],
            'base_table' => $defn_array['table'],
            'base_field' => $field['name'],
            'left_table' => $join['table'],
            'left_field' => $join['field'],
          );

          if (!empty($join['handler'])) {
            $join_record['handler'] = $join['handler'];
          }
          else {
            $join_record['handler'] = 'views_join';
          }

          if ($view_record['setup_id'] && $defn_array['table'] && $field['name'] && $join['table'] && $join['field']) {
            $status = drupal_write_record('tripal_views_join', $join_record);
          }
          else {
            $status = FALSE;
          }
          if (!$status) {
            drupal_set_message(
              t(
                'Unable to join %left_table.%left_field with %table.%field',
                array(
                  '%left_table' => $join['table'],
                  '%left_field' => $join['field'],
                  '%table' => $defn_array['table'],
                  '%field' => $field['name']
                )
              ),
              'error'
            );
            $no_errors = FALSE;
          }
        }

      }
      else {
        drupal_set_message(t('Unable to integrate %field_name field', array('%field_name' => $field['name'])), 'error');
        $no_errors = FALSE;
      }
    }

  }
  else {
    drupal_set_message(t('Unable to set default tripal views integration'), 'error');
    $no_errors = FALSE;
  }

  return $no_errors;
}

/**
 * Export Views integration records
 *
 * @param $setup_id
 *   The unique setup id of the tripal views integration
 *
 * @return
 *   A views integration definition array as used by tripal_views_integration_add_entry()
 */
function tripal_views_integration_export_entry($setup_id) {

  // Main setup details
  $r = db_fetch_object(db_query("SELECT * FROM {tripal_views} WHERE setup_id=%d", $setup_id));
  $defn_array = array(
    'table' => $r->table_name,
    'name' => $r->name,
    'type' => ($r->mview_id) ? 'mview' : 'chado',
    'description' => $r->comment,
    'priority' => $r->priority,
    'base_table' => $r->base_table,
    'fields' => array(),
  );

  // Add fields
  $resource = db_query("SELECT * FROM {tripal_views_field} WHERE setup_id=%d", $setup_id);
  while ($r = db_fetch_object($resource)) {
    $defn_array['fields'][ $r->column_name ] = array(
        'name' => $r->column_name,
        'title' => $r->name,
        'description' => $r->description,
        'type' => $r->type,
        'handlers' => array(),
        'joins' => array()
    );
  }

  // Add handlers
  $resource = db_query("SELECT * FROM {tripal_views_handlers} WHERE setup_id=%d", $setup_id);
  while ($r = db_fetch_object($resource)) {
    $defn_array['fields'][ $r->column_name ]['handlers'][ $r->handler_type ] = array(
      'name' => $r->handler_name
    );
  }

  // Add joins
  $resource = db_query("SELECT * FROM {tripal_views_join} WHERE setup_id=%d", $setup_id);
  while ($r = db_fetch_object($resource)) {
    $defn_array['fields'][ $r->base_field ]['joins'][ $r->left_table ] = array(
      'table' => $r->left_table,
      'field' => $r->left_field,
      'handler' => $r->handler,
    );
  }

  return $defn_array;
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
function tripal_views_integration_remove_entry_by_table_name($table_name, $priority) {

  $views = db_fetch_object(db_query(
    "SELECT * FROM {tripal_views} WHERE table_name='%s' AND priority=%d",
    $table_name,
    $priority
  ));
  if ($views->setup_id) {
    tripal_views_integration_remove_entry_by_setup_id($views->setup_id);
    return TRUE;
  }
  else {
    return FALSE;
  }

}

/**
 * Removes a View Integration Entry
 *
 * @param $setup_id
 *   The setup ID of the views integration entry to remove
 */
function tripal_views_integration_remove_entry_by_setup_id($setup_id) {

    db_query('DELETE FROM {tripal_views} WHERE setup_id=%d', $setup_id);
    db_query('DELETE FROM {tripal_views_field} WHERE setup_id=%d', $setup_id);
    db_query('DELETE FROM {tripal_views_handlers} WHERE setup_id=%d', $setup_id);
    db_query('DELETE FROM {tripal_views_join} WHERE setup_id=%d', $setup_id);

}

/**
 * Integrate all chado tables in the schema api
 */
function tripal_views_integrate_all_chado_tables() {

  $tables = tripal_core_get_chado_tables();
  foreach ($tables as $tablename) {
    if (!tripal_views_is_integrated($tablename, 10)) {
      $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE, 10);
      if ($table_integration_array) {
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
  }
}

/**
 * Returns the array needed to integrate a given chado table with views
 *
 * @param $tablename
 *   The table to generate the tripal views integration array for
 * @return
 *   The tripal views integration array which is the parameter for
 *   tripal_views_integration_add_entry($defn_array)
 */
function tripal_views_get_integration_array_for_chado_table($table_name, $base_table = TRUE, $priority = 9) {

  // Get the schema for this table (via the chado schema api)
  $schema = tripal_core_get_chado_table_schema($table_name);

  // Base definition array
  $defn_array = array(
    'table' => $table_name,
    'type' => 'chado',
    'name' => 'Chado ' . ucwords(str_replace('_', ' ', $table_name)),
    'description' => (!empty($schema['description'])) ? $schema['description'] : ' ',
    'priority' => $priority,
    'base_table' => $base_table,
    'fields' => array(),
  );
  // Add fields
  if (!isset($schema['fields'])) {
    watchdog('tripal_views', 'There are no fields defined for %table in the Chado Schema API.', array('%table' => $table_name), WATCHDOG_NOTICE);
    return FALSE;
  }
  foreach ($schema['fields'] as $field_name => $field_schema) {

    // Base field definition
    if (!empty($field_name) && !empty($field_schema['type'])) {
      $defn_array['fields'][$field_name] = array(
        'name' => $field_name,
        'title' => ucwords(str_replace('_', ' ', $field_name)),
        'type' => $field_schema['type'],
        'description' => ($field_schema['description']) ? $field_schema['description'] : ucwords(str_replace('_', ' ', $field_name)),
        'handlers' => array(),
        'joins' => array()
      );

      // Add handlers based on type
      if (preg_match('/^int/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field_numeric'),
          'filter' => array('name' => 'chado_views_handler_filter_numeric'),
          'sort' => array('name' => 'chado_views_handler_sort'),
        );
      }
      elseif (preg_match('/^serial/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field_numeric'),
          'filter' => array('name' => 'chado_views_handler_filter_numeric'),
          'sort' => array('name' => 'chado_views_handler_sort'),
        );
        $defn_array['fields'][$field_name]['type'] = 'int';
      }
      elseif (preg_match('/^varchar/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field'),
          'filter' => array('name' => 'chado_views_handler_filter_string'),
          'sort' => array('name' => 'chado_views_handler_sort'),
        );
      }
      elseif (preg_match('/^text/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field'),
          'filter' => array('name' => 'chado_views_handler_filter_string'),
          'sort' => array('name' => 'chado_views_handler_sort'),
        );
      }
      elseif (preg_match('/^boolean/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field_boolean'),
          'filter' => array('name' => 'chado_views_handler_filter_boolean_operator'),
          'sort' => array('name' => 'chado_views_handler_sort'),
        );
      }
      elseif (preg_match('/^datetime/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field_date'),
          'filter' => array('name' => 'chado_views_handler_filter_date'),
          'sort' => array('name' => 'views_handler_sort_date'),
        );
      }
      else {
        $defn_array['fields'][$field_name]['handlers'] = array(
          'field' => array('name' => 'chado_views_handler_field'),
          'filter' => array('name' => 'chado_views_handler_filter_string'),
          'sort' => array('name' => 'chado_views_handler_sort'),
        );
      }

      // Specify specialty handlers
      if ($field_name == 'type_id' OR $field_name == 'cvterm_id') {
        $defn_array['fields'][$field_name]['handlers']['filter']['name'] = 'chado_views_handler_filter_select_cvterm';
      }
    }
  }

  // Add Joins & Relationship Handlers to fields
  if (!isset($schema['foreign keys'])) {
    $schema['foreign keys'] = array();
    watchdog('tripal_views','There are no foreign keys defined for %table in the Chado Schema API.', array('%table' => $table_name), WATCHDOG_WARNING);
  }
  foreach ($schema['foreign keys'] as $foreign_key_schema) {
    foreach ($foreign_key_schema['columns'] as $left_field => $right_field) {
      // Join
      $defn_array['fields'][$left_field]['joins'][ $foreign_key_schema['table'] ] = array(
        'table' => $foreign_key_schema['table'],
        'field' => $right_field,
        'handler' => 'views_handler_join_chado_aggregator'
      );

      // Relationship Handler
      $defn_array['fields'][$left_field]['handlers']['relationship'] = array(
        'name' => 'chado_views_handler_relationship',
        'base' => $foreign_key_schema['table'],
        'base field' => $right_field,
        'label' => $table_name . ' ' . $left_field . ' to ' . $foreign_key_schema['table'] . ' ' .$right_field
      );
    }
  }

  return $defn_array;
}
