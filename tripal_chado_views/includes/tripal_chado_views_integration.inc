<?php
/**
 * @file
 * Contains functions used to manage tripal views integrations
 */

/**
 * Purpose: Deletes ALL Chado Views Integrations.
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_delete_all_integrations() {

  db_query("DELETE FROM {tripal_views}");
  db_query("DELETE FROM {tripal_views_field}");
  db_query("DELETE FROM {tripal_views_handlers}");
  db_query("DELETE FROM {tripal_views_join}");

  drupal_set_message("Successfully deleted all views integration.");

}

/**
 * Integrate all chado tables in the schema api.  This integration only occurs
 * once and sets all Chado tables to a priority of 10
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_integrate_all_chado_tables() {
  // The tripal_chado_views and tripal_chado module are so highly tied
  // together it may make sense for them to be one module.  But in the
  // event that one is enabled and the other not we will have problems
  // with the load below.
  module_load_include('inc', 'tripal_chado', 'api/tripal_chado.mviews.api');

  // First integrate all of the Chado tables. Those that are base tables
  // get special treatment.
  $tables = chado_get_table_names(TRUE);

  // Some chado tables might have been created via the Tripal Custom Tables
  // or Tripal Materialized Views interfaces. We need to ensure that the
  // corresponding mview_id and table_id are associated with these tables.
  // @TODO: Add some way to show which integrations are for custom tables.
  //$custom_tables = chado_get_custom_table_names();
  $mview_tables = chado_get_mview_table_names();

  // Hardcode a list of base tables since there isn't really a programatic way
  // to determine which tables in the chado schema should be base tables.
  $base_tables = [
    'acquisition',
    'analysis',
    'assay',
    'biomaterial',
    'contact',
    'cv',
    'cvterm',
    'db',
    'dbxref',
    'environment',
    'expression',
    'feature',
    'featuremap',
    'genotype',
    'library',
    'nd_experiment',
    'nd_geolocation',
    'nd_protocol',
    'nd_reagent',
    'organism',
    'phendesc',
    'phenotype',
    'phenstatement',
    'phylonode',
    'phylotree',
    'project',
    'protocol',
    'pub',
    'stock',
    'study',
    'synonym',
  ];

  // For each chado table, generate an integration array, keeping the above
  // details in mind, and save that integration with Tripal Views through the API.
  foreach ($tables as $tablename) {
    $priority = 10;
    if (!tripal_is_table_integrated($tablename, $priority)) {

      // Assuming that we have a default chado table, genereate an integration
      // array describing it's Tripal Views integration.
      if (in_array($tablename, $base_tables) OR (is_array($mview_tables) and in_array($tablename, $mview_tables))) {
        $table_integration_array = tripal_chado_views_get_integration_array_for_chado_table($tablename, TRUE, $priority);
      }
      else {
        $table_integration_array = tripal_chado_views_get_integration_array_for_chado_table($tablename, FALSE, $priority);
      }

      // Check to see if this table is a Materialized view and if it is,
      // treat it specially :).
      if (is_array($mview_tables) and in_array($tablename, $mview_tables)) {
        $table_integration_array['type'] = 'mview';
      }

      // As long as we were able to generate an integration array,
      // Integrate It!
      if ($table_integration_array) {
        tripal_add_views_integration($table_integration_array);
      }
    }
  }
}

/**
 * Returns the array needed to integrate a given chado table with views
 *
 * @param $tablename
 *   The table to generate the tripal views integration array for
 *
 * @return
 *   The tripal views integration array which is the parameter for
 *   tripal_add_views_integration($defn_array)
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_get_integration_array_for_chado_table($table_name, $base_table = TRUE, $priority = 9) {

  // Get the schema for this table (via the chado schema api)
  $schema = chado_get_schema($table_name);

  // Base definition array
  $defn_array = [
    'table' => $table_name,
    'type' => 'chado',
    'name' => 'Chado ' . ucwords(str_replace('_', ' ', $table_name)),
    'description' => (!empty($schema['description'])) ? $schema['description'] : ' ',
    'priority' => $priority,
    'base_table' => $base_table,
    'fields' => [],
  ];
  // Add fields
  if (!isset($schema['fields'])) {
    tripal_report_error('tripal_chado_views', TRIPAL_NOTICE,
      'There are no fields defined for %table in the Chado Schema API.', ['%table' => $table_name]);
    return FALSE;
  }
  foreach ($schema['fields'] as $field_name => $field_schema) {

    // Base field definition
    if (!empty($field_name)) {
      $defn_array['fields'][$field_name] = [
        'name' => $field_name,
        'title' => ucwords(str_replace('_', ' ', $field_name)),
        'type' => (isset($field_schema['type'])) ? $field_schema['type'] : 'text',
        'description' => (!empty($field_schema['description'])) ? $field_schema['description'] : ucwords(str_replace('_', ' ', $field_name)),
        'handlers' => [],
        'joins' => [],
      ];

      // Add handlers based on type
      if (preg_match('/^int/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field_numeric'),
           * 'filter' => array('name' => 'chado_views_handler_filter_numeric'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field_numeric'],
          'filter' => ['name' => 'views_handler_filter_numeric'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_numeric'],
        ];
      }
      // Set the defaults for a serial type.
      elseif (preg_match('/^serial/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field_numeric'),
           * 'filter' => array('name' => 'chado_views_handler_filter_numeric'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field_numeric'],
          'filter' => ['name' => 'views_handler_filter_numeric'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_numeric'],
        ];
        $defn_array['fields'][$field_name]['type'] = 'int';
      }
      // Set the defaults for a varchar type.
      elseif (preg_match('/^varchar/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field'),
           * 'filter' => array('name' => 'chado_views_handler_filter_string'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field'],
          'filter' => ['name' => 'views_handler_filter_string'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_string'],
        ];
      }
      // Set the defaults for a text type.
      elseif (preg_match('/^text/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field'),
           * 'filter' => array('name' => 'chado_views_handler_filter_string'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field'],
          'filter' => ['name' => 'views_handler_filter_string'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_string'],
        ];
      }
      // Set the defaults for a char type.
      elseif (preg_match('/^char/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field'),
           * 'filter' => array('name' => 'chado_views_handler_filter_string'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field'],
          'filter' => ['name' => 'views_handler_filter_string'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_string'],
        ];
      }
      // Set the defaults for a boolean type.
      elseif (preg_match('/^boolean/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /**
           * 'field' => array('name' => 'chado_views_handler_field_boolean'),
           * 'filter' => array('name' => 'chado_views_handler_filter_boolean_operator'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field_boolean'],
          'filter' => ['name' => 'views_handler_filter_boolean_operator'],
          'sort' => ['name' => 'views_handler_sort'],
        ];
      }
      // Set the defaults for a datatime type.
      elseif (preg_match('/^datetime/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field_date'),
           * 'filter' => array('name' => 'chado_views_handler_filter_date'),
           * 'sort' => array('name' => 'views_handler_sort_date'),
           */
          'field' => ['name' => 'views_handler_field_date'],
          'filter' => ['name' => 'views_handler_filter_date'],
          'sort' => ['name' => 'views_handler_sort_date'],
          'argument' => ['name' => 'views_handler_argument_date'],
        ];
      }
      // Set the defaults for a float type.
      elseif (preg_match('/^float/', $field_schema['type'])) {
        $defn_array['fields'][$field_name]['handlers'] = [
          'field' => ['name' => 'views_handler_field_numeric'],
          'filter' => ['name' => 'views_handler_filter_numeric'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_numeric'],
        ];
      }
      // If the type is not recognize the default to a string handler.
      else {
        $defn_array['fields'][$field_name]['handlers'] = [
          /** D6
           * 'field' => array('name' => 'chado_views_handler_field'),
           * 'filter' => array('name' => 'chado_views_handler_filter_string'),
           * 'sort' => array('name' => 'chado_views_handler_sort'),
           */
          'field' => ['name' => 'views_handler_field'],
          'filter' => ['name' => 'views_handler_filter_string'],
          'sort' => ['name' => 'views_handler_sort'],
          'argument' => ['name' => 'views_handler_argument_string'],
        ];
      }

      // Specify specialty handlers
      if ($field_name == 'type_id' OR $field_name == 'cvterm_id') {
        $defn_array['fields'][$field_name]['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';
      }
      if (preg_match('/name/', $field_name)) {
        $defn_array['fields'][$field_name]['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
      }
    }
  }

  // Add Joins & Relationships for foreign keys to fields
  if (!isset($schema['foreign keys'])) {
    $schema['foreign keys'] = [];
  }
  foreach ($schema['foreign keys'] as $foreign_key_schema) {
    foreach ($foreign_key_schema['columns'] as $left_field => $right_field) {

      // Note: Even though there can only be a single join  for a foreign key
      // we make the joins an array keyed by left_field to ensure that both
      // foeign key and referring_tables (see below) can be processed the same.
      $defn_array['fields'][$left_field]['joins'][$foreign_key_schema['table']][$right_field] = [
        'table' => $foreign_key_schema['table'],
        'field' => $right_field,
        'handler' => 'views_handler_join',
        'relationship_handler' => 'views_handler_relationship',
      ];
    }
  }

  // Add in reverse relationships
  // Note: The array structure is set up with the left_field keyed array to
  // handle more than one join between the same set of tables on different
  // fields. For example, the reverse relationship between feature &
  // feature_relationship needs to join on both the subject_id and object_id.
  if (isset($schema['referring_tables'])) {
    foreach ($schema['referring_tables'] as $referring_table) {

      // D7 @todo: fix referring_tables in schema to list the keys like foreign keys does
      $referring_schema = chado_get_schema($referring_table);
      $referring_schema_fk_columns = $referring_schema['foreign keys'][$table_name]['columns'];
      foreach ($referring_schema_fk_columns as $left_field => $right_field) {

        // Also notice that it doesn't matter whether this is the first or second
        // reverse join on this table ($referring_table) to be added since
        // having $left_field as the key keeps them separate.
        $defn_array['fields'][$right_field]['joins'][$referring_table][$left_field] = [
          'table' => $referring_table,
          'field' => $left_field,
          'relationship_handler' => 'views_handler_relationship',
          'relationship_only' => 1,
        ];

      }
    }
  }

  return $defn_array;
}
