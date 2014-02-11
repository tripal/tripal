<?php
/**
 * @file
 * Contains functions used to manage tripal views integrations
 */

/**
 * Purpose: Deletes ALL Tripal Views Integrations.
 *
 * @ingroup tripal_views
 */
function tripal_views_delete_all_integrations() {

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
 * @ingroup tripal_views
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
 * @ingroup tripal_views
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