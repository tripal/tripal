<?php
/**
 * @file
 * @todo Add file header description
 */

/**
 * @defgroup tripal_featuremap_api Library Module API
 * @ingroup tripal_api
 * @ingroup tripal_featuremap
 */

/**
 * Implements hook_chado_schema_v1_11_table()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the map table
 *
 * @ingroup tripal_schema_api
 */
function tripal_featuremap_chado_schema_v1_11_map() {
  $description = array();
  $referring_tables = array(

  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_table()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the map table
 *
 * @ingroup tripal_schema_api
 */
function tripal_featuremap_chado_schema_v1_2_map() {
  $description = array();
  $referring_tables = array(

  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}