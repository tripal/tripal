<?php

/**
 * Implements hook_chado_schema_v1_2_tripal_obo_temp()
 * Purpose: To describe the structure of 'tripal_obo_temp' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'tripal_obo_temp' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_cv_chado_schema_v1_2_tripal_obo_temp() {
  $schema = tripal_cv_get_custom_tables('tripal_obo_temp');
  $description = $schema['tripal_obo_temp'];
  return $description;
}