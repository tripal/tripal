<?php

/**
 * @file
 * API functions for Tripal Views Integration
 */

 /**
 * @defgroup tripal_views_api Tripal Views Module API
 * @ingroup tripal_api
 * @{
 * Provides functions for Tripal Views Integrations
 * @}
 */

/**
 * Retrieve the priority of the lightest priority for a given table
 *
 * NOTE: Uses lightest priority (drupal-style) where the range is from -10 to 10
 * and -10 is of highest priority.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a materialized
 *   view or a chado table
 *
 * @return
 *   returns the lowest priority.  If the table has not been integrated, a priority of 10
 *   is returned.
 *
 * @ingroup tripal_views_api
 */
function tripal_views_get_table_lightest_priority($table_name) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT priority FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
  $setup = db_query($sql, array(':table' => $table_name));
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->priority;
  }
  else {
    // default priority is 10
    return 10;
  }
}

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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_get_lightest_priority_setup($table_name) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
  $setup = db_query($sql, array(':table' => $table_name));
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Retrieve the views integration setup with the given priority/table combination
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a materialized
 *   view or a chado table
 * @param $priority
 *   The priority of the integration to retrieve the setup_id for
 *
 * @return
 *   On success, the setup_id to use for integration of this table; otherwise FALSE
 *
 * @ingroup tripal_views_api
 */
function tripal_views_get_setup_id($table_name, $priority) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority ORDER BY priority ASC";
  $setup = db_query($sql, array(':table' => $table_name, ':priority' => $priority));
  $setup = $setup->fetchObject();
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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_is_integrated($table_name, $priority = NULL) {

  if ($priority) {
    // D7 TODO: Check DBTNG changes work
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority";
    $setup = db_query($sql, array(':table' => $table_name, ':priority' => $priority));
    $setup = $setup->fetchObject();
  }
  else {
    // D7 TODO: Check DBTNG changes work
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
    $setup = db_query($sql, array(':table' => $table_name));
    $setup = $setup->fetchObject();
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
 *
 * @ingroup tripal_views_api
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
 * Programatically enable view
 *
 * @param $view_name
 *   The machine-name of the view to be enabled
 * @param $redirect_link
 *   The path to redirect to. FALSE if no redirect needed
 */
function tripal_views_admin_enable_view($view_name, $redirect_link = FALSE) {

  $status = variable_get('views_defaults', array());
  if (isset($status[$view_name])) {
    $status[$view_name] = FALSE;
    variable_set('views_defaults', $status);
    drupal_set_message("Successfully Enabled $view_name");
  }
  else {
    drupal_set_message("Unable to find a view by the name of '$view_name'. Unable to enable this view.",'error');
  }
  if ($redirect_link) {
    drupal_goto($redirect_link);
  }
}

/**
 * Programatically disable view
 *
 * @param $view_name
 *   The machine-name of the view to be enabled
 * @param $redirect_link
 *   The path to redirect to. FALSE if no redirect needed
 */
function tripal_views_admin_disable_view($view_name, $redirect_link = FALSE) {

  $status = variable_get('views_defaults', array());
  if (isset($status[$view_name])) {
    $status[$view_name] = TRUE;
    variable_set('views_defaults', $status);
    drupal_set_message("Disabled $view_name");
  }
  else {
    drupal_set_message("Unable to find a view by the name of '$view_name'. Unable to disable this view.",'error');
  }
  if ($redirect_link) {
    drupal_goto($redirect_link);
  }
}

/**
 * Purpose: Deletes ALL Tripal Views Integrations.
 */
function tripal_views_delete_all_integrations() {

  db_query("DELETE FROM {tripal_views}");
  db_query("DELETE FROM {tripal_views_field}");
  db_query("DELETE FROM {tripal_views_handlers}");
  db_query("DELETE FROM {tripal_views_join}");

  drupal_set_message("Successfully deleted all views integration.");

}


/**
 * Rebuilds all the default integrations
 *
 * @param $delete_first
 *   If TRUE then all integrations are first deleted.
 */
function tripal_views_rebuild_views_integrations($delete_first = FALSE) {

  if ($delete_first) {
    tripal_views_delete_all_integrations();
  }

  tripal_views_integrate_all_chado_tables();
  drupal_set_message('Successfully rebuilt default Tripal Views Integrations');
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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_integration_add_entry($defn_array) {
  $no_errors = TRUE;

  if (empty($defn_array['table'])) {
    tripal_report_error('tripal_views', TRIPAL_WARNING, 'Recieved integration with no tablename: %defn', array('%defn' => print_r($defn_array,TRUE)));
    $no_errors = FALSE;
    return $no_errors;
  }

  // First insert into tripal_views
  $view_record = array(
    'table_name' => $defn_array['table'],
    'name' => $defn_array['name'],
    'comment' => $defn_array['description'],
    'priority' => $defn_array['priority'],
    'base_table' => $defn_array['base_table'],
  );
  if ($defn_array['type'] == 'mview') {
    // D7 TODO: Check DBTNG changes work
    $mview = db_query("SELECT mview_id FROM {tripal_mviews} WHERE mv_table=:table", array(':table' => $defn_array['table']));
    $mview = $mview->fetchObject();
    $view_record['mview_id'] = $mview->mview_id;
    if (!$mview->mview_id) {
      return FALSE;
    }
  }
  if ($view_record['name']) { // && $view_record['comment']) {  # SPF: commented out 9/24/2012 .. It's not required on the form
    if (isset($defn_array['additional_content'])) {
      // D7 TODO: Check DBTNG changes work
      $setup = db_query(
        "SELECT * FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
        array(':table' => $view_record['table_name'], ':priority' => $view_record['priority'])
      );
      $setup = $setup->fetchObject();
      if (empty($setup->setup_id)) {
        $status = drupal_write_record('tripal_views', $view_record);
      }
      else {
        $view_record['setup_id'] = $setup->setup_id;
        $status = drupal_write_record('tripal_views', $view_record, 'setup_id');
      }
    }
    else {
      $status = drupal_write_record('tripal_views', $view_record);
    }
  }
  else {
    $status = FALSE;
    drupal_set_message(t('Unable to integrate "%table" table due to a missing name field.', array('%table' => $defn_array['table'])), 'error');
  }

  if ($status) {

    // Need to update the tripal_views record so base_table can be false
    // this is a fix because drupal_write_record() puts in defaults if !isset()
    // and a variable is considered not set if it's null!
    // D7 TODO: Check DBTNG changes work
    db_query(
      "UPDATE {tripal_views} SET base_table=:base WHERE table_name=:table AND priority=:priority",
      array(
        ':base' => $defn_array['base_table'],
        ':table' => $defn_array['table'],
        ':priority' => $defn_array['priority']
      )
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
        if (isset($defn_array['additional_content'])) {
          // D7 TODO: Check DBTNG changes work
          $is = db_query(
            "SELECT true as present FROM {tripal_views_field} WHERE column_name=:column AND setup_id=:setup",
            array(
              ':column' => $field_record['column_name'],
              ':setup' => $field_record['setup_id']
              )
            );
          $is = $is->fetchObject();
          if (!$is->present) {
            $status = drupal_write_record('tripal_views_field', $field_record);
          }
          else {
            $status = drupal_write_record('tripal_views_field', $field_record, array('setup_id', 'column_name'));
          }
        }
        else {
          $status = drupal_write_record('tripal_views_field', $field_record);
        }
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

          $join_record['handler'] = (!empty($join['handler'])) ? $join['handler'] : 'views_join';
          $join_record['relationship_handler'] = (!empty($join['relationship_handler'])) ? $join['relationship_handler'] : 'views_handler_relationship';
          $join_record['relationship_only'] = (!empty($join['relationship_only'])) ? $join['relationship_only'] : 0;

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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_integration_export_entry($setup_id) {

  // Main setup details
  // D7 TODO: Check DBTNG changes work
  $r = db_query("SELECT * FROM {tripal_views} WHERE setup_id=:setup", array(':setup' => $setup_id));
  $r = $r->fetchObject();
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
  // D7 TODO: Check DBTNG changes work
  $resource = db_query("SELECT * FROM {tripal_views_field} WHERE setup_id=:setup", array(':setup' => $setup_id));
  foreach ($resource as $r) {
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
  // D7 TODO: Check DBTNG changes work
  $resource = db_query("SELECT * FROM {tripal_views_handlers} WHERE setup_id=:setup", array(':setup' => $setup_id));
  foreach ($resource as $r) {
    $defn_array['fields'][ $r->column_name ]['handlers'][ $r->handler_type ] = array(
      'name' => $r->handler_name
    );
  }

  // Add joins
  // D7 TODO: Check DBTNG changes work
  $resource = db_query("SELECT * FROM {tripal_views_join} WHERE setup_id=:setup", array(':setup' => $setup_id));
  foreach ($resource as $r) {
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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_integration_remove_entry_by_table_name($table_name, $priority) {

  // D7 TODO: Check DBTNG changes work
  $views = db_query(
    "SELECT * FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    array(
      ':table' => $table_name,
      ':priority' => $priority
    )
  );
  $views = $views->fetchObject();
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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_integration_remove_entry_by_setup_id($setup_id) {

    // D7 TODO: Check DBTNG changes work
    db_query('DELETE FROM {tripal_views} WHERE setup_id=:setup', array(':setup' => $setup_id));
    db_query('DELETE FROM {tripal_views_field} WHERE setup_id=:setup', array(':setup' => $setup_id));
    db_query('DELETE FROM {tripal_views_handlers} WHERE setup_id=:setup', array(':setup' => $setup_id));
    db_query('DELETE FROM {tripal_views_join} WHERE setup_id=:setup', array(':setup' => $setup_id));

}

/**
 * Integrate all chado tables in the schema api.  This integration only occurs
 * once and sets all Chado tables to a priority of 10
 *
 * @ingroup tripal_views_api
 */
function tripal_views_integrate_all_chado_tables() {

  $tables = chado_get_table_names(TRUE);

  $base_tables = array(
    'acquisition', 'analysis', 'assay', 'biomaterial', 'contact', 'cv', 'cvterm',
    'db', 'dbxref', 'environment', 'expression', 'feature', 'featuremap', 'genotype',
    'library', 'nd_experiment', 'nd_geolocation', 'nd_protocol', 'nd_reagent',
    'organism', 'phendesc', 'phenotype', 'phenstatement', 'phylonode', 'phylotree',
    'project', 'protocol', 'pub', 'stock', 'study', 'synonym'
  );

  foreach ($tables as $tablename) {

    $priority = 10;
    if (!tripal_views_is_integrated($tablename, $priority)) {
      if (in_array($tablename, $base_tables)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE, $priority);
      }
      else {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE, $priority);
      }
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
 *
 * @ingroup tripal_views_api
 */
function tripal_views_get_integration_array_for_chado_table($table_name, $base_table = TRUE, $priority = 9) {

  // Get the schema for this table (via the chado schema api)
  $schema = chado_get_schema($table_name);

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
    tripal_report_error('tripal_views', TRIPAL_NOTICE, 
      'There are no fields defined for %table in the Chado Schema API.', array('%table' => $table_name));
    return FALSE;
  }
  foreach ($schema['fields'] as $field_name => $field_schema) {

    // Base field definition
    if (!empty($field_name) && !empty($field_schema['type'])) {
      $defn_array['fields'][$field_name] = array(
        'name' => $field_name,
        'title' => ucwords(str_replace('_', ' ', $field_name)),
        'type' => $field_schema['type'],
        'description' => (isset($field_schema['description'])) ? $field_schema['description'] : ucwords(str_replace('_', ' ', $field_name)),
        'handlers' => array(),
        'joins' => array()
      );

      // Add handlers based on type
      if (preg_match('/^int/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /** D6
          'field' => array('name' => 'chado_views_handler_field_numeric'),
          'filter' => array('name' => 'chado_views_handler_filter_numeric'),
          'sort' => array('name' => 'chado_views_handler_sort'),
          */
          'field' => array('name' => 'views_handler_field_numeric'),
          'filter' => array('name' => 'views_handler_filter_numeric'),
          'sort' => array('name' => 'views_handler_sort'),
        );
      }
      elseif (preg_match('/^serial/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /** D6
          'field' => array('name' => 'chado_views_handler_field_numeric'),
          'filter' => array('name' => 'chado_views_handler_filter_numeric'),
          'sort' => array('name' => 'chado_views_handler_sort'),
          */
          'field' => array('name' => 'views_handler_field_numeric'),
          'filter' => array('name' => 'views_handler_filter_numeric'),
          'sort' => array('name' => 'views_handler_sort'),
        );
        $defn_array['fields'][$field_name]['type'] = 'int';
      }
      elseif (preg_match('/^varchar/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /** D6
          'field' => array('name' => 'chado_views_handler_field'),
          'filter' => array('name' => 'chado_views_handler_filter_string'),
          'sort' => array('name' => 'chado_views_handler_sort'),
          */
          'field' => array('name' => 'views_handler_field'),
          'filter' => array('name' => 'views_handler_filter_string'),
          'sort' => array('name' => 'views_handler_sort'),
        );
      }
      elseif (preg_match('/^text/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /** D6
          'field' => array('name' => 'chado_views_handler_field'),
          'filter' => array('name' => 'chado_views_handler_filter_string'),
          'sort' => array('name' => 'chado_views_handler_sort'),
          */
          'field' => array('name' => 'views_handler_field'),
          'filter' => array('name' => 'views_handler_filter_string'),
          'sort' => array('name' => 'views_handler_sort'),
        );
      }
      elseif (preg_match('/^boolean/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /**
          'field' => array('name' => 'chado_views_handler_field_boolean'),
          'filter' => array('name' => 'chado_views_handler_filter_boolean_operator'),
          'sort' => array('name' => 'chado_views_handler_sort'),
          */
          'field' => array('name' => 'views_handler_field_boolean'),
          'filter' => array('name' => 'views_handler_filter_boolean_operator'),
          'sort' => array('name' => 'views_handler_sort'),
        );
      }
      elseif (preg_match('/^datetime/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /** D6
          'field' => array('name' => 'chado_views_handler_field_date'),
          'filter' => array('name' => 'chado_views_handler_filter_date'),
          'sort' => array('name' => 'views_handler_sort_date'),
          */
          'field' => array('name' => 'views_handler_field_date'),
          'filter' => array('name' => 'views_handler_filter_date'),
          'sort' => array('name' => 'views_handler_sort_date'),
        );
      }
      else {
        $defn_array['fields'][$field_name]['handlers'] = array(
          /** D6
          'field' => array('name' => 'chado_views_handler_field'),
          'filter' => array('name' => 'chado_views_handler_filter_string'),
          'sort' => array('name' => 'chado_views_handler_sort'),
          */
          'field' => array('name' => 'views_handler_field'),
          'filter' => array('name' => 'views_handler_filter_string'),
          'sort' => array('name' => 'views_handler_sort'),
        );
      }

      // Specify specialty handlers
      if ($field_name == 'type_id' OR $field_name == 'cvterm_id') {
        $defn_array['fields'][$field_name]['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';
      }
      if (preg_match('/name/',$field_name)) {
        $defn_array['fields'][$field_name]['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
      }
    }
  }

  // Add Joins & Relationships for foreign keys to fields
  if (!isset($schema['foreign keys'])) {
    $schema['foreign keys'] = array();
  }
  foreach ($schema['foreign keys'] as $foreign_key_schema) {
    foreach ($foreign_key_schema['columns'] as $left_field => $right_field) {
      // Join
      $defn_array['fields'][$left_field]['joins'][ $foreign_key_schema['table'] ] = array(
        'table' => $foreign_key_schema['table'],
        'field' => $right_field,
        /**D6 'handler' => 'views_handler_join_chado_aggregator'*/
        'handler' => 'views_handler_join',
        'relationship_handler' => 'views_handler_relationship',
      );
    }
  }

  // Add in reverse relationships
  if (isset($schema['referring_tables'])) {
    foreach ($schema['referring_tables'] as $referring_table) {

      // D7 @todo: fix referring_tables in schema to list the keys like foreign keys does
      $referring_schema = chado_get_schema($referring_table);
      $referring_schema_fk_columns = $referring_schema['foreign keys'][$table_name]['columns'];
      foreach ($referring_schema_fk_columns as $left_field => $right_field) {
        $defn_array['fields'][$right_field]['joins'][ $referring_table ] = array(
          'table' => $referring_table,
          'field' => $left_field,
          'relationship_handler' => 'views_handler_relationship',
          'relationship_only' => 1
        );
      }
    }
  }

  return $defn_array;
}

/**
 * Clone an integration
 *
 * @param $table_name
 *   The table for which you'd like to clone an integration
 * @param $new_priority
 *   The priority of the clone; this is the integration which will be created.
 *   If no priority is supplied then one lighter then the $template_priority will be used.
 * @param $template_priority
 *   The priority of the template to be used for the clone; this is an existing integration.
 *   If no priority is supplied then the lightest priority will be used.
 *
 * @ingroup tripal_views_api
 */
function tripal_views_clone_integration($table_name, $new_priority = NULL, $template_priority = NULL) {

  if (empty($template_priority)) {
    $template_setup_id = tripal_views_get_lightest_priority_setup($table_name);
  }
  else {
    $template_setup_id = tripal_views_get_setup_id($table_name, $template_priority);
  }

  $defn_array = tripal_views_integration_export_entry($template_setup_id);

  if (empty($new_priority)) {
    $defn_array['priority'] = $new_priority;
  }
  else {
    $new_priority = $defn_array['priority'] - 1;
    $defn_array['priority'] = $defn_array['priority'] - 1;
  }

  // D7 TODO: Check DBTNG changes work
  tripal_views_integration_add_entry($defn_array);
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    array(
      ':table' => $table_name,
      ':priority' => $new_priority
    )
  );
  $setup_id = $setup_id->fetchObject();

  if (empty($setup_id)) {
    tripal_report_error('tripal_views', TRIPAL_ERROR, 'Unable to clone the setup for %table in order to add the following field to the integration: %field.',
      array('%table' => $table_name, '%field' => print_r($field_array,TRUE)));
    return FALSE;
  }
  else {
    return $setup_id;
  }

}

/**
 * Adds the given field to an existing or cloned integration. In the case of a cloned
 * integration, the lightest integration is used as the template for the clone.
 *
 * NOTE: If that field already exists in the specified integration then it will first be
 * deleted and the new one added.
 *
 * @param $table_name
 *   The name of the table the integration is for
 * @param $priority
 *   The priority of the integration to use; pass NULL to use the lightest integration
 * @param $field
 *   An array describing the field ot add; uses the same format as the $defn_array
 *
 * @return
 *   TRUE if the field was added successfully; FALSE otherwise
 *
 * @ingroup tripal_views_api
 */
function tripal_views_add_field_to_integration($table_name, $priority, $field) {
  $no_errors = TRUE;

  // If no priority is supplied then add the field to the lightest integration
  if (empty($priority)) {
    $priority = tripal_views_get_table_lightest_priority($table_name);
  }

  // First get the setup_id
  // D7 TODO: Check DBTNG changes work
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    array(
      ':table' => $table_name,
      ':priority' => $priority
    )
  );
  $setup_id = $setup_id->fetchObject();

  // If there isn't an integration matching that table/priority combination
  // then clone the lightest priority integration
  if (empty($setup_id)) {
    $setup_id = tripal_views_clone_integration($table_name, $priority);
  }

  // Now delete any existing field
  db_query("DELETE FROM {tripal_views_field} WHERE setup_id=:setup AND column_name=:column",
    array(':setup' => $setup_id, 'column' => $field['name'])
  );
  db_query("DELETE FROM {tripal_views_handlers} WHERE setup_id=:setup AND column_name=:column",
    array(':setup' => $setup_id, 'column' => $field['name'])
  );
  db_query("DELETE FROM {tripal_views_join} WHERE setup_id=:setup AND base_table=:table AND base_field=:field",
    array(':setup' => $setup_id, ':table' => $table_name, ':field' => $field['name'])
  );

  // Now we need to add/update the field
  $field_record = array(
    'setup_id' => $setup_id,
    'column_name' => $field['name'],
    'name' => $field['title'],
    'description' => $field['description'],
    'type' => $field['type'],
  );
  if ($setup_id && $field['name'] && $field['title'] && $field['description'] && $field['type']) {
    if ($defn_array['additional_content']) {
      // D7 TODO: Check DBTNG changes work
      $is = db_query(
        "SELECT true as present FROM {tripal_views_field} WHERE column_name=:column AND setup_id=:setup",
        array(':column' => $field_record['column_name'], ':setup' => $field_record['setup_id'])
      );
      $is = $is->fetchObject();
      if (!$is->present) {
        $status = drupal_write_record('tripal_views_field', $field_record);
      }
      else {
        $status = drupal_write_record('tripal_views_field', $field_record, array('setup_id', 'column_name'));
      }
    }
    else {
      $status = drupal_write_record('tripal_views_field', $field_record);
    }
  }
  else {
    drupal_set_message(t('Unable to integrate %name field due to missing required fields.', array('%name' => $field['name'])), 'error');
    $status = FALSE;
  }

  if ($status) {

    // Insert Handler Definitions
    foreach ($field['handlers'] as $handler_type => $handler) {
      $handler_record = array(
        'setup_id' => $setup_id,
        'column_name' => $field['name'],
        'handler_type' => $handler_type,
        'handler_name' => $handler['name'],
        'arguments' => serialize($handler)
      );
      if ($setup_id && $field['name'] && $handler_type && $handler['name'] && $handler) {
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
        'setup_id' => $setup_id,
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

      if ($setup_id && $defn_array['table'] && $field['name'] && $join['table'] && $join['field']) {
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

  return $no_errors;
}

/**
 * Remove a join from an integration
 *
 * @param $setup_id
 *   The setup_id of the integration to delete the join from
 * @param $base_table
 *   The name of the base table involved the join
 * @param $base_field
 *   The field from the base table involved in the join
 * @param $left_table
 *   The name of the other table involved in the join
 * @param $left_field
 *   The name of the field from the other table involved in the join
 *
 * @ingroup tripal_views_api
 */
function tripal_views_remove_join_from_integration($setup_id, $base_table, $base_field, $left_table, $left_field) {
  db_query(
    "DELETE FROM {tripal_views_join} WHERE setup_id=:setup AND base_table=:base-table AND base_field=:base-field AND left_table=:left-table AND left_field=:left-field",
    array(
      ':setup' => $setup_id,
      ':base-table' => $base_table,
      ':base-field' => $base_field,
      ':left-table' => $left_table,
      ':left-field' => $left_field
    )
  );
}