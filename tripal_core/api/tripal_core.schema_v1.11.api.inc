<?php
/**
 * @file
 * Describes the chado tables in version 1.11
 */

/**
 * @defgroup tripal_schema_v1_11_api Chado v1.11 Schema API
 * @ingroup tripal_chado_schema_api
 * @{
 * Provides an application programming interface (API) for describing Chado tables.
 * This API consists of a set of functions, one for each table in Chado.  Each
 * function simply returns a Drupal style array that defines the table.
 *
 * Because Drupal 6 does not handle foreign key (FK) relationships, which are needed to
 * for Tripal Views, they have been added to the schema defintitions below.
 *
 * The functions provided in this documentation should not be called as is, but if you need
 * the Drupal-style array definition for any table, use the following function
 * call:
 *
 *   $table_desc = chado_get_schema($table)
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The chado_get_schema function determines the appropriate version of
 * Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
 *
 * If you need to augment these schema definitions within your own module, you need to
 * implement the hook_chado_schema_v1_11_[table name]() hook where [table name] is the
 * name of the chado table whose schema definition you want to augment.
 * @}
 */

/**
 * Implements hook_chado_schema_v1_11_acquisition()
 * Purpose: To describe the structure of 'acquisition' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'acquisition' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_acquisition() {
  $description =  array(
    'table' => 'acquisition',
    'fields' => array(
      'acquisition_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'channel_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'acquisitiondate' => array(
        'type' => 'datetime',
        'not NULL' => '',
        'default' => 'ow(',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'uri' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'acquisition_id',
    ),
    'unique keys' => array(
      'acquisition_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
      'acquisition_idx1' => array(
        0 => 'assay_id',
      ),
      'acquisition_idx2' => array(
        0 => 'protocol_id',
      ),
      'acquisition_idx3' => array(
        0 => 'channel_id',
      ),
    ),
    'foreign keys' => array(
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
      'channel' => array(
        'table' => 'channel',
        'columns' => array(
          'channel_id' => 'channel_id',
        ),
      ),
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_acquisition_relationship()
 * Purpose: To describe the structure of 'acquisition_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'acquisition_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_acquisition_relationship() {
  $description =  array(
    'table' => 'acquisition_relationship',
    'fields' => array(
      'acquisition_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'acquisition_relationship_id',
    ),
    'unique keys' => array(
      'acquisition_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'acquisition_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'acquisition_relationship_idx2' => array(
        0 => 'type_id',
      ),
      'acquisition_relationship_idx3' => array(
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'acquisition' => array(
        'table' => 'acquisition',
        'columns' => array(
          'subject_id' => 'acquisition_id',
          'object_id' => 'acquisition_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_acquisitionprop()
 * Purpose: To describe the structure of 'acquisitionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'acquisitionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_acquisitionprop() {
  $description =  array(
    'table' => 'acquisitionprop',
    'fields' => array(
      'acquisitionprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'acquisition_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'acquisitionprop_id',
    ),
    'unique keys' => array(
      'acquisitionprop_c1' => array(
        0 => 'acquisition_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'acquisitionprop_idx1' => array(
        0 => 'acquisition_id',
      ),
      'acquisitionprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'acquisition' => array(
        'table' => 'acquisition',
        'columns' => array(
          'acquisition_id' => 'acquisition_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_analysis()
 * Purpose: To describe the structure of 'analysis' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysis' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysis() {
  $description =  array(
    'referring_tables' => array(
      0 => 'analysisfeature',
      1 => 'analysisprop',
      2 => 'phylotree',
      3 => 'quantification',
    ),
    'table' => 'analysis',
    'fields' => array(
      'analysis_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'program' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'programversion' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'algorithm' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'sourcename' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'sourceversion' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'sourceuri' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'timeexecuted' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      0 => 'analysis_id',
    ),
    'unique keys' => array(
      'analysis_c1' => array(
        0 => 'program',
        1 => 'programversion',
        2 => 'sourcename',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_analysisprop()
 *
 * Purpose: To describe the structure of 'analysisprop' to tripal.  This table doesn't
 * exist in Chado v1.1 (it does in v1.2), so we need to describe it here
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysisprop() {
  $description =  array(
    'table' => 'analysisprop',
    'fields' => array(
      'analysisprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'analysis_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'analysisprop_id',
    ),
    'unique keys' => array(
      'analysisprop_c1' => array(
        0 => 'analysis_id',
        1 => 'type_id',
      ),
    ),
    'indexes' => array(
      'analysisprop_idx1' => array(
        0 => 'analysis_id',
      ),
      'analysisprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_analysisfeature()
 * Purpose: To describe the structure of 'analysisfeature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisfeature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysisfeature() {
  $description =  array(
    'table' => 'analysisfeature',
    'fields' => array(
      'analysisfeature_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'analysis_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rawscore' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ),
      'normscore' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ),
      'significance' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ),
      'identity' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'analysisfeature_id',
    ),
    'unique keys' => array(
      'analysisfeature_c1' => array(
        0 => 'feature_id',
        1 => 'analysis_id',
      ),
    ),
    'indexes' => array(
      'analysisfeature_idx1' => array(
        0 => 'feature_id',
      ),
      'analysisfeature_idx2' => array(
        0 => 'analysis_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
    ),
  );
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_arraydesign()
 * Purpose: To describe the structure of 'arraydesign' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'arraydesign' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_arraydesign() {
  $description =  array(
    'table' => 'arraydesign',
    'fields' => array(
      'arraydesign_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'manufacturer_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'platformtype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'substratetype_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'version' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'array_dimensions' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'element_dimensions' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'num_of_elements' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'num_array_columns' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'num_array_rows' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'num_grid_columns' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'num_grid_rows' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'num_sub_columns' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'num_sub_rows' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'arraydesign_id',
    ),
    'unique keys' => array(
      'arraydesign_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
      'arraydesign_idx1' => array(
        0 => 'manufacturer_id',
      ),
      'arraydesign_idx2' => array(
        0 => 'platformtype_id',
      ),
      'arraydesign_idx3' => array(
        0 => 'substratetype_id',
      ),
      'arraydesign_idx4' => array(
        0 => 'protocol_id',
      ),
      'arraydesign_idx5' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'platformtype_id' => 'cvterm_id',
          'substratetype_id' => 'cvterm_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'manufacturer_id' => 'contact_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_arraydesignprop()
 * Purpose: To describe the structure of 'arraydesignprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'arraydesignprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_arraydesignprop() {
  $description =  array(
    'table' => 'arraydesignprop',
    'fields' => array(
      'arraydesignprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'arraydesign_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'arraydesignprop_id',
    ),
    'unique keys' => array(
      'arraydesignprop_c1' => array(
        0 => 'arraydesign_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'arraydesignprop_idx1' => array(
        0 => 'arraydesign_id',
      ),
      'arraydesignprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'arraydesign' => array(
        'table' => 'arraydesign',
        'columns' => array(
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_assay()
 * Purpose: To describe the structure of 'assay' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assay' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assay() {
  $description =  array(
    'table' => 'assay',
    'fields' => array(
      'assay_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'arraydesign_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'assaydate' => array(
        'type' => 'datetime',
        'not NULL' => '',
        'default' => 'ow(',
      ),
      'arrayidentifier' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'arraybatchidentifier' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'operator_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'assay_id',
    ),
    'unique keys' => array(
      'assay_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
      'assay_idx1' => array(
        0 => 'arraydesign_id',
      ),
      'assay_idx2' => array(
        0 => 'protocol_id',
      ),
      'assay_idx3' => array(
        0 => 'operator_id',
      ),
      'assay_idx4' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'operator_id' => 'contact_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
      'arraydesign' => array(
        'table' => 'arraydesign',
        'columns' => array(
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_assay_biomaterial()
 * Purpose: To describe the structure of 'assay_biomaterial' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assay_biomaterial' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assay_biomaterial() {
  $description =  array(
    'table' => 'assay_biomaterial',
    'fields' => array(
      'assay_biomaterial_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'biomaterial_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'channel_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'assay_biomaterial_id',
    ),
    'unique keys' => array(
      'assay_biomaterial_c1' => array(
        0 => 'assay_id',
        1 => 'biomaterial_id',
        2 => 'channel_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'assay_biomaterial_idx1' => array(
        0 => 'assay_id',
      ),
      'assay_biomaterial_idx2' => array(
        0 => 'biomaterial_id',
      ),
      'assay_biomaterial_idx3' => array(
        0 => 'channel_id',
      ),
    ),
    'foreign keys' => array(
      'channel' => array(
        'table' => 'channel',
        'columns' => array(
          'channel_id' => 'channel_id',
        ),
      ),
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
      'biomaterial' => array(
        'table' => 'biomaterial',
        'columns' => array(
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_assay_project()
 * Purpose: To describe the structure of 'assay_project' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assay_project' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assay_project() {
  $description =  array(
    'table' => 'assay_project',
    'fields' => array(
      'assay_project_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'project_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'assay_project_id',
    ),
    'unique keys' => array(
      'assay_project_c1' => array(
        0 => 'assay_id',
        1 => 'project_id',
      ),
    ),
    'indexes' => array(
      'assay_project_idx1' => array(
        0 => 'assay_id',
      ),
      'assay_project_idx2' => array(
        0 => 'project_id',
      ),
    ),
    'foreign keys' => array(
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'project_id' => 'project_id',
        ),
      ),
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_assayprop()
 * Purpose: To describe the structure of 'assayprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assayprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assayprop() {
  $description =  array(
    'table' => 'assayprop',
    'fields' => array(
      'assayprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'assayprop_id',
    ),
    'unique keys' => array(
      'assayprop_c1' => array(
        0 => 'assay_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'assayprop_idx1' => array(
        0 => 'assay_id',
      ),
      'assayprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_biomaterial()
 * Purpose: To describe the structure of 'biomaterial' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial() {
  $description =  array(
    'table' => 'biomaterial',
    'fields' => array(
      'biomaterial_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'taxon_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'biosourceprovider_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'biomaterial_id',
    ),
    'unique keys' => array(
      'biomaterial_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
      'biomaterial_idx1' => array(
        0 => 'taxon_id',
      ),
      'biomaterial_idx2' => array(
        0 => 'biosourceprovider_id',
      ),
      'biomaterial_idx3' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'taxon_id' => 'organism_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'biosourceprovider_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_biomaterial_dbxref()
 * Purpose: To describe the structure of 'biomaterial_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial_dbxref() {
  $description =  array(
    'table' => 'biomaterial_dbxref',
    'fields' => array(
      'biomaterial_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'biomaterial_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'biomaterial_dbxref_id',
    ),
    'unique keys' => array(
      'biomaterial_dbxref_c1' => array(
        0 => 'biomaterial_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'biomaterial_dbxref_idx1' => array(
        0 => 'biomaterial_id',
      ),
      'biomaterial_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'biomaterial' => array(
        'table' => 'biomaterial',
        'columns' => array(
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_biomaterial_relationship()
 * Purpose: To describe the structure of 'biomaterial_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial_relationship() {
  $description =  array(
    'table' => 'biomaterial_relationship',
    'fields' => array(
      'biomaterial_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'biomaterial_relationship_id',
    ),
    'unique keys' => array(
      'biomaterial_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'biomaterial_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'biomaterial_relationship_idx2' => array(
        0 => 'object_id',
      ),
      'biomaterial_relationship_idx3' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'biomaterial' => array(
        'table' => 'biomaterial',
        'columns' => array(
          'subject_id' => 'biomaterial_id',
          'object_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_biomaterial_treatment()
 * Purpose: To describe the structure of 'biomaterial_treatment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial_treatment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial_treatment() {
  $description =  array(
    'table' => 'biomaterial_treatment',
    'fields' => array(
      'biomaterial_treatment_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'biomaterial_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'treatment_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'unittype_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'value' => array(
        'type' => 'float',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'biomaterial_treatment_id',
    ),
    'unique keys' => array(
      'biomaterial_treatment_c1' => array(
        0 => 'biomaterial_id',
        1 => 'treatment_id',
      ),
    ),
    'indexes' => array(
      'biomaterial_treatment_idx1' => array(
        0 => 'biomaterial_id',
      ),
      'biomaterial_treatment_idx2' => array(
        0 => 'treatment_id',
      ),
      'biomaterial_treatment_idx3' => array(
        0 => 'unittype_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'unittype_id' => 'cvterm_id',
        ),
      ),
      'biomaterial' => array(
        'table' => 'biomaterial',
        'columns' => array(
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
      'treatment' => array(
        'table' => 'treatment',
        'columns' => array(
          'treatment_id' => 'treatment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_biomaterialprop()
 * Purpose: To describe the structure of 'biomaterialprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterialprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterialprop() {
  $description =  array(
    'table' => 'biomaterialprop',
    'fields' => array(
      'biomaterialprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'biomaterial_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'biomaterialprop_id',
    ),
    'unique keys' => array(
      'biomaterialprop_c1' => array(
        0 => 'biomaterial_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'biomaterialprop_idx1' => array(
        0 => 'biomaterial_id',
      ),
      'biomaterialprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'biomaterial' => array(
        'table' => 'biomaterial',
        'columns' => array(
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line()
 * Purpose: To describe the structure of 'cell_line' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line() {
  $description =  array(
    'table' => 'cell_line',
    'fields' => array(
      'cell_line_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'timeaccessioned' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_id',
    ),
    'unique keys' => array(
      'cell_line_c1' => array(
        0 => 'uniquename',
        1 => 'organism_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_cvterm()
 * Purpose: To describe the structure of 'cell_line_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_cvterm() {
  $description =  array(
    'table' => 'cell_line_cvterm',
    'fields' => array(
      'cell_line_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_cvterm_id',
    ),
    'unique keys' => array(
      'cell_line_cvterm_c1' => array(
        0 => 'cell_line_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_cvtermprop()
 * Purpose: To describe the structure of 'cell_line_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_cvtermprop() {
  $description =  array(
    'table' => 'cell_line_cvtermprop',
    'fields' => array(
      'cell_line_cvtermprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_cvtermprop_id',
    ),
    'unique keys' => array(
      'cell_line_cvtermprop_c1' => array(
        0 => 'cell_line_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'cell_line_cvterm' => array(
        'table' => 'cell_line_cvterm',
        'columns' => array(
          'cell_line_cvterm_id' => 'cell_line_cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_dbxref()
 * Purpose: To describe the structure of 'cell_line_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_dbxref() {
  $description =  array(
    'table' => 'cell_line_dbxref',
    'fields' => array(
      'cell_line_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_dbxref_id',
    ),
    'unique keys' => array(
      'cell_line_dbxref_c1' => array(
        0 => 'cell_line_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_feature()
 * Purpose: To describe the structure of 'cell_line_feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_feature() {
  $description =  array(
    'table' => 'cell_line_feature',
    'fields' => array(
      'cell_line_feature_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_feature_id',
    ),
    'unique keys' => array(
      'cell_line_feature_c1' => array(
        0 => 'cell_line_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_library()
 * Purpose: To describe the structure of 'cell_line_library' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_library' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_library() {
  $description =  array(
    'table' => 'cell_line_library',
    'fields' => array(
      'cell_line_library_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_library_id',
    ),
    'unique keys' => array(
      'cell_line_library_c1' => array(
        0 => 'cell_line_id',
        1 => 'library_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_pub()
 * Purpose: To describe the structure of 'cell_line_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_pub() {
  $description =  array(
    'table' => 'cell_line_pub',
    'fields' => array(
      'cell_line_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_pub_id',
    ),
    'unique keys' => array(
      'cell_line_pub_c1' => array(
        0 => 'cell_line_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_relationship()
 * Purpose: To describe the structure of 'cell_line_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_relationship() {
  $description =  array(
    'table' => 'cell_line_relationship',
    'fields' => array(
      'cell_line_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_relationship_id',
    ),
    'unique keys' => array(
      'cell_line_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'subject_id' => 'cell_line_id',
          'object_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_line_synonym()
 * Purpose: To describe the structure of 'cell_line_synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_synonym() {
  $description =  array(
    'table' => 'cell_line_synonym',
    'fields' => array(
      'cell_line_synonym_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'synonym_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'is_internal' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      0 => 'cell_line_synonym_id',
    ),
    'unique keys' => array(
      'cell_line_synonym_c1' => array(
        0 => 'synonym_id',
        1 => 'cell_line_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'synonym' => array(
        'table' => 'synonym',
        'columns' => array(
          'synonym_id' => 'synonym_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_lineprop()
 * Purpose: To describe the structure of 'cell_lineprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_lineprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_lineprop() {
  $description =  array(
    'table' => 'cell_lineprop',
    'fields' => array(
      'cell_lineprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_line_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'cell_lineprop_id',
    ),
    'unique keys' => array(
      'cell_lineprop_c1' => array(
        0 => 'cell_line_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'cell_line' => array(
        'table' => 'cell_line',
        'columns' => array(
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cell_lineprop_pub()
 * Purpose: To describe the structure of 'cell_lineprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_lineprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_lineprop_pub() {
  $description =  array(
    'table' => 'cell_lineprop_pub',
    'fields' => array(
      'cell_lineprop_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cell_lineprop_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'cell_lineprop_pub_id',
    ),
    'unique keys' => array(
      'cell_lineprop_pub_c1' => array(
        0 => 'cell_lineprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'cell_lineprop' => array(
        'table' => 'cell_lineprop',
        'columns' => array(
          'cell_lineprop_id' => 'cell_lineprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_channel()
 * Purpose: To describe the structure of 'channel' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'channel' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_channel() {
  $description =  array(
    'table' => 'channel',
    'fields' => array(
      'channel_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'definition' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'channel_id',
    ),
    'unique keys' => array(
      'channel_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_contact()
 * Purpose: To describe the structure of 'contact' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'contact' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_contact() {
  $description =  array(
    'table' => 'contact',
    'fields' => array(
      'contact_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'contact_id',
    ),
    'unique keys' => array(
      'contact_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_contact_relationship()
 * Purpose: To describe the structure of 'contact_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'contact_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_contact_relationship() {
  $description =  array(
    'table' => 'contact_relationship',
    'fields' => array(
      'contact_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'contact_relationship_id',
    ),
    'unique keys' => array(
      'contact_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'contact_relationship_idx1' => array(
        0 => 'type_id',
      ),
      'contact_relationship_idx2' => array(
        0 => 'subject_id',
      ),
      'contact_relationship_idx3' => array(
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'subject_id' => 'contact_id',
          'object_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_control()
 * Purpose: To describe the structure of 'control' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'control' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_control() {
  $description =  array(
    'table' => 'control',
    'fields' => array(
      'control_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'tableinfo_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'row_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'control_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'control_idx1' => array(
        0 => 'type_id',
      ),
      'control_idx2' => array(
        0 => 'assay_id',
      ),
      'control_idx3' => array(
        0 => 'tableinfo_id',
      ),
      'control_idx4' => array(
        0 => 'row_id',
      ),
    ),
    'foreign keys' => array(
      'tableinfo' => array(
        'table' => 'tableinfo',
        'columns' => array(
          'tableinfo_id' => 'tableinfo_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cv()
 * Purpose: To describe the structure of 'cv' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cv' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cv() {
  $description =  array(
    'table' => 'cv',
    'fields' => array(
      'cv_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'definition' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'cv_id',
    ),
    'unique keys' => array(
      'cv_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvterm()
 * Purpose: To describe the structure of 'cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvterm() {
  $description =  array(
    'table' => 'cvterm',
    'fields' => array(
      'cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cv_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '1024',
        'not NULL' => '1',
      ),
      'definition' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_obsolete' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
      'is_relationshiptype' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'cvterm_id',
    ),
    'unique keys' => array(
      'cvterm_c1' => array(
        0 => 'name',
        1 => 'cv_id',
        2 => 'is_obsolete',
      ),
      'cvterm_c2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'cvterm_idx1' => array(
        0 => 'cv_id',
      ),
      'cvterm_idx2' => array(
        0 => 'name',
      ),
      'cvterm_idx3' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cv' => array(
        'table' => 'cv',
        'columns' => array(
          'cv_id' => 'cv_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvterm_dbxref()
 * Purpose: To describe the structure of 'cvterm_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvterm_dbxref() {
  $description =  array(
    'table' => 'cvterm_dbxref',
    'fields' => array(
      'cvterm_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_for_definition' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'cvterm_dbxref_id',
    ),
    'unique keys' => array(
      'cvterm_dbxref_c1' => array(
        0 => 'cvterm_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'cvterm_dbxref_idx1' => array(
        0 => 'cvterm_id',
      ),
      'cvterm_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvterm_relationship()
 * Purpose: To describe the structure of 'cvterm_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvterm_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvterm_relationship() {
  $description =  array(
    'table' => 'cvterm_relationship',
    'fields' => array(
      'cvterm_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'cvterm_relationship_id',
    ),
    'unique keys' => array(
      'cvterm_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'cvterm_relationship_idx1' => array(
        0 => 'type_id',
      ),
      'cvterm_relationship_idx2' => array(
        0 => 'subject_id',
      ),
      'cvterm_relationship_idx3' => array(
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvtermpath()
 * Purpose: To describe the structure of 'cvtermpath' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvtermpath' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvtermpath() {
  $description =  array(
    'table' => 'cvtermpath',
    'fields' => array(
      'cvtermpath_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cv_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pathdistance' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'cvtermpath_id',
    ),
    'unique keys' => array(
      'cvtermpath_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'pathdistance',
      ),
    ),
    'indexes' => array(
      'cvtermpath_idx1' => array(
        0 => 'type_id',
      ),
      'cvtermpath_idx2' => array(
        0 => 'subject_id',
      ),
      'cvtermpath_idx3' => array(
        0 => 'object_id',
      ),
      'cvtermpath_idx4' => array(
        0 => 'cv_id',
      ),
    ),
    'foreign keys' => array(
      'cv' => array(
        'table' => 'cv',
        'columns' => array(
          'cv_id' => 'cv_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvtermprop()
 * Purpose: To describe the structure of 'cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvtermprop() {
  $description =  array(
    'table' => 'cvtermprop',
    'fields' => array(
      'cvtermprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '1',
        'default' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'cvtermprop_id',
    ),
    'unique keys' => array(
      'cvterm_id' => array(
        0 => 'cvterm_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'cvtermprop_idx1' => array(
        0 => 'cvterm_id',
      ),
      'cvtermprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvtermsynonym()
 * Purpose: To describe the structure of 'cvtermsynonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvtermsynonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvtermsynonym() {
  $description =  array(
    'table' => 'cvtermsynonym',
    'fields' => array(
      'cvtermsynonym_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'synonym' => array(
        'type' => 'varchar',
        'length' => '1024',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'cvtermsynonym_id',
    ),
    'unique keys' => array(
      'cvtermsynonym_c1' => array(
        0 => 'cvterm_id',
        1 => 'synonym',
      ),
    ),
    'indexes' => array(
      'cvtermsynonym_idx1' => array(
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_db()
 * Purpose: To describe the structure of 'db' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'db' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_db() {
  $description =  array(
    'table' => 'db',
    'fields' => array(
      'db_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'urlprefix' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'url' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'db_id',
    ),
    'unique keys' => array(
      'db_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_dbxref()
 * Purpose: To describe the structure of 'dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_dbxref() {
  $description =  array(
    'table' => 'dbxref',
    'fields' => array(
      'dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'db_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'accession' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'version' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
        'default' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'dbxref_id',
    ),
    'unique keys' => array(
      'dbxref_c1' => array(
        0 => 'db_id',
        1 => 'accession',
        2 => 'version',
      ),
    ),
    'indexes' => array(
      'dbxref_idx1' => array(
        0 => 'db_id',
      ),
      'dbxref_idx2' => array(
        0 => 'accession',
      ),
      'dbxref_idx3' => array(
        0 => 'version',
      ),
    ),
    'foreign keys' => array(
      'db' => array(
        'table' => 'db',
        'columns' => array(
          'db_id' => 'db_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_dbxrefprop()
 * Purpose: To describe the structure of 'dbxrefprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_dbxrefprop() {
  $description =  array(
    'table' => 'dbxrefprop',
    'fields' => array(
      'dbxrefprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '1',
        'default' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'dbxrefprop_id',
    ),
    'unique keys' => array(
      'dbxrefprop_c1' => array(
        0 => 'dbxref_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'dbxrefprop_idx1' => array(
        0 => 'dbxref_id',
      ),
      'dbxrefprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_eimage()
 * Purpose: To describe the structure of 'eimage' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'eimage' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_eimage() {
  $description =  array(
    'table' => 'eimage',
    'fields' => array(
      'eimage_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'eimage_data' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'eimage_type' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'image_uri' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'eimage_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_element()
 * Purpose: To describe the structure of 'element' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'element' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_element() {
  $description =  array(
    'table' => 'element',
    'fields' => array(
      'element_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'arraydesign_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'element_id',
    ),
    'unique keys' => array(
      'element_c1' => array(
        0 => 'feature_id',
        1 => 'arraydesign_id',
      ),
    ),
    'indexes' => array(
      'element_idx1' => array(
        0 => 'feature_id',
      ),
      'element_idx2' => array(
        0 => 'arraydesign_id',
      ),
      'element_idx3' => array(
        0 => 'type_id',
      ),
      'element_idx4' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'arraydesign' => array(
        'table' => 'arraydesign',
        'columns' => array(
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_element_relationship()
 * Purpose: To describe the structure of 'element_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'element_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_element_relationship() {
  $description =  array(
    'table' => 'element_relationship',
    'fields' => array(
      'element_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'element_relationship_id',
    ),
    'unique keys' => array(
      'element_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'element_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'element_relationship_idx2' => array(
        0 => 'type_id',
      ),
      'element_relationship_idx3' => array(
        0 => 'object_id',
      ),
      'element_relationship_idx4' => array(
        0 => 'value',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'element' => array(
        'table' => 'element',
        'columns' => array(
          'subject_id' => 'element_id',
          'object_id' => 'element_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_elementresult()
 * Purpose: To describe the structure of 'elementresult' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'elementresult' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_elementresult() {
  $description =  array(
    'table' => 'elementresult',
    'fields' => array(
      'elementresult_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'element_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'quantification_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'signal' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'elementresult_id',
    ),
    'unique keys' => array(
      'elementresult_c1' => array(
        0 => 'element_id',
        1 => 'quantification_id',
      ),
    ),
    'indexes' => array(
      'elementresult_idx1' => array(
        0 => 'element_id',
      ),
      'elementresult_idx2' => array(
        0 => 'quantification_id',
      ),
      'elementresult_idx3' => array(
        0 => 'signal',
      ),
    ),
    'foreign keys' => array(
      'quantification' => array(
        'table' => 'quantification',
        'columns' => array(
          'quantification_id' => 'quantification_id',
        ),
      ),
      'element' => array(
        'table' => 'element',
        'columns' => array(
          'element_id' => 'element_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_elementresult_relationship()
 * Purpose: To describe the structure of 'elementresult_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'elementresult_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_elementresult_relationship() {
  $description =  array(
    'table' => 'elementresult_relationship',
    'fields' => array(
      'elementresult_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'elementresult_relationship_id',
    ),
    'unique keys' => array(
      'elementresult_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'elementresult_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'elementresult_relationship_idx2' => array(
        0 => 'type_id',
      ),
      'elementresult_relationship_idx3' => array(
        0 => 'object_id',
      ),
      'elementresult_relationship_idx4' => array(
        0 => 'value',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'elementresult' => array(
        'table' => 'elementresult',
        'columns' => array(
          'subject_id' => 'elementresult_id',
          'object_id' => 'elementresult_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_environment()
 * Purpose: To describe the structure of 'environment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'environment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_environment() {
  $description =  array(
    'table' => 'environment',
    'fields' => array(
      'environment_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'environment_id',
    ),
    'unique keys' => array(
      'environment_c1' => array(
        0 => 'uniquename',
      ),
    ),
    'indexes' => array(
      'environment_idx1' => array(
        0 => 'uniquename',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_environment_cvterm()
 * Purpose: To describe the structure of 'environment_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'environment_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_environment_cvterm() {
  $description =  array(
    'table' => 'environment_cvterm',
    'fields' => array(
      'environment_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'environment_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'environment_cvterm_id',
    ),
    'unique keys' => array(
      'environment_cvterm_c1' => array(
        0 => 'environment_id',
        1 => 'cvterm_id',
      ),
    ),
    'indexes' => array(
      'environment_cvterm_idx1' => array(
        0 => 'environment_id',
      ),
      'environment_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'environment' => array(
        'table' => 'environment',
        'columns' => array(
          'environment_id' => 'environment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expression()
 * Purpose: To describe the structure of 'expression' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression() {
  $description =  array(
    'table' => 'expression',
    'fields' => array(
      'expression_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'md5checksum' => array(
        'type' => 'char',
        'length' => '32',
        'not NULL' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'expression_id',
    ),
    'unique keys' => array(
      'expression_c1' => array(
        0 => 'uniquename',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expression_cvterm()
 * Purpose: To describe the structure of 'expression_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_cvterm() {
  $description =  array(
    'table' => 'expression_cvterm',
    'fields' => array(
      'expression_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'expression_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
      'cvterm_type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'expression_cvterm_id',
    ),
    'unique keys' => array(
      'expression_cvterm_c1' => array(
        0 => 'expression_id',
        1 => 'cvterm_id',
        2 => 'cvterm_type_id',
      ),
    ),
    'indexes' => array(
      'expression_cvterm_idx1' => array(
        0 => 'expression_id',
      ),
      'expression_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
      'expression_cvterm_idx3' => array(
        0 => 'cvterm_type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
          'cvterm_type_id' => 'cvterm_id',
        ),
      ),
      'expression' => array(
        'table' => 'expression',
        'columns' => array(
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expression_cvtermprop()
 * Purpose: To describe the structure of 'expression_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_cvtermprop() {
  $description =  array(
    'table' => 'expression_cvtermprop',
    'fields' => array(
      'expression_cvtermprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'expression_cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'expression_cvtermprop_id',
    ),
    'unique keys' => array(
      'expression_cvtermprop_c1' => array(
        0 => 'expression_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'expression_cvtermprop_idx1' => array(
        0 => 'expression_cvterm_id',
      ),
      'expression_cvtermprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'expression_cvterm' => array(
        'table' => 'expression_cvterm',
        'columns' => array(
          'expression_cvterm_id' => 'expression_cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expression_image()
 * Purpose: To describe the structure of 'expression_image' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_image' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_image() {
  $description =  array(
    'table' => 'expression_image',
    'fields' => array(
      'expression_image_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'expression_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'eimage_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'expression_image_id',
    ),
    'unique keys' => array(
      'expression_image_c1' => array(
        0 => 'expression_id',
        1 => 'eimage_id',
      ),
    ),
    'indexes' => array(
      'expression_image_idx1' => array(
        0 => 'expression_id',
      ),
      'expression_image_idx2' => array(
        0 => 'eimage_id',
      ),
    ),
    'foreign keys' => array(
      'expression' => array(
        'table' => 'expression',
        'columns' => array(
          'expression_id' => 'expression_id',
        ),
      ),
      'eimage' => array(
        'table' => 'eimage',
        'columns' => array(
          'eimage_id' => 'eimage_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expression_pub()
 * Purpose: To describe the structure of 'expression_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_pub() {
  $description =  array(
    'table' => 'expression_pub',
    'fields' => array(
      'expression_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'expression_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'expression_pub_id',
    ),
    'unique keys' => array(
      'expression_pub_c1' => array(
        0 => 'expression_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'expression_pub_idx1' => array(
        0 => 'expression_id',
      ),
      'expression_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'expression' => array(
        'table' => 'expression',
        'columns' => array(
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expressionprop()
 * Purpose: To describe the structure of 'expressionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expressionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expressionprop() {
  $description =  array(
    'table' => 'expressionprop',
    'fields' => array(
      'expressionprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'expression_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'expressionprop_id',
    ),
    'unique keys' => array(
      'expressionprop_c1' => array(
        0 => 'expression_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'expressionprop_idx1' => array(
        0 => 'expression_id',
      ),
      'expressionprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'expression' => array(
        'table' => 'expression',
        'columns' => array(
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature()
 * Purpose: To describe the structure of 'feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature() {
  $description =  array(
    'table' => 'feature',
    'fields' => array(
      'feature_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'residues' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'seqlen' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'md5checksum' => array(
        'type' => 'char',
        'length' => '32',
        'not NULL' => '',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_analysis' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'is_obsolete' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'timeaccessioned' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      0 => 'feature_id',
    ),
    'unique keys' => array(
      'feature_c1' => array(
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'feature_idx1' => array(
        0 => 'dbxref_id',
      ),
      'feature_idx2' => array(
        0 => 'organism_id',
      ),
      'feature_idx3' => array(
        0 => 'type_id',
      ),
      'feature_idx4' => array(
        0 => 'uniquename',
      ),
      'feature_name_ind1' => array(
        0 => 'name',
      ),
    ),
    'foreign keys' => array(
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
      0 => 'analysisfeature',
      1 => 'element',
      2 => 'feature_cvterm',
      3 => 'feature_dbxref',
      4 => 'feature_expression',
      5 => 'feature_genotype',
      6 => 'feature_phenotype',
      7 => 'feature_pub',
      8 => 'feature_relationship',
      9 => 'feature_synonym',
      10 => 'featureloc',
      11 => 'featurepos',
      12 => 'featureprop',
      13 => 'featurerange',
      14 => 'library_feature',
      15 => 'phylonode',
      16 => 'wwwuser_feature',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_cvterm()
 * Purpose: To describe the structure of 'feature_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvterm() {
  $description =  array(
    'table' => 'feature_cvterm',
    'fields' => array(
      'feature_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_not' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'feature_cvterm_id',
    ),
    'unique keys' => array(
      'feature_cvterm_c1' => array(
        0 => 'feature_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_cvterm_idx1' => array(
        0 => 'feature_id',
      ),
      'feature_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
      'feature_cvterm_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_cvterm_dbxref()
 * Purpose: To describe the structure of 'feature_cvterm_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvterm_dbxref() {
  $description =  array(
    'table' => 'feature_cvterm_dbxref',
    'fields' => array(
      'feature_cvterm_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_cvterm_dbxref_id',
    ),
    'unique keys' => array(
      'feature_cvterm_dbxref_c1' => array(
        0 => 'feature_cvterm_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'feature_cvterm_dbxref_idx1' => array(
        0 => 'feature_cvterm_id',
      ),
      'feature_cvterm_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'feature_cvterm' => array(
        'table' => 'feature_cvterm',
        'columns' => array(
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_cvterm_pub()
 * Purpose: To describe the structure of 'feature_cvterm_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvterm_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvterm_pub() {
  $description =  array(
    'table' => 'feature_cvterm_pub',
    'fields' => array(
      'feature_cvterm_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_cvterm_pub_id',
    ),
    'unique keys' => array(
      'feature_cvterm_pub_c1' => array(
        0 => 'feature_cvterm_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_cvterm_pub_idx1' => array(
        0 => 'feature_cvterm_id',
      ),
      'feature_cvterm_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'feature_cvterm' => array(
        'table' => 'feature_cvterm',
        'columns' => array(
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_cvtermprop()
 * Purpose: To describe the structure of 'feature_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvtermprop() {
  $description =  array(
    'table' => 'feature_cvtermprop',
    'fields' => array(
      'feature_cvtermprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'feature_cvtermprop_id',
    ),
    'unique keys' => array(
      'feature_cvtermprop_c1' => array(
        0 => 'feature_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_cvtermprop_idx1' => array(
        0 => 'feature_cvterm_id',
      ),
      'feature_cvtermprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'feature_cvterm' => array(
        'table' => 'feature_cvterm',
        'columns' => array(
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_dbxref()
 * Purpose: To describe the structure of 'feature_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_dbxref() {
  $description =  array(
    'table' => 'feature_dbxref',
    'fields' => array(
      'feature_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      0 => 'feature_dbxref_id',
    ),
    'unique keys' => array(
      'feature_dbxref_c1' => array(
        0 => 'feature_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'feature_dbxref_idx1' => array(
        0 => 'feature_id',
      ),
      'feature_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_expression()
 * Purpose: To describe the structure of 'feature_expression' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_expression' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_expression() {
  $description =  array(
    'table' => 'feature_expression',
    'fields' => array(
      'feature_expression_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'expression_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_expression_id',
    ),
    'unique keys' => array(
      'feature_expression_c1' => array(
        0 => 'expression_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_expression_idx1' => array(
        0 => 'expression_id',
      ),
      'feature_expression_idx2' => array(
        0 => 'feature_id',
      ),
      'feature_expression_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'expression' => array(
        'table' => 'expression',
        'columns' => array(
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_expressionprop()
 * Purpose: To describe the structure of 'feature_expressionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_expressionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_expressionprop() {
  $description =  array(
    'table' => 'feature_expressionprop',
    'fields' => array(
      'feature_expressionprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_expression_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'feature_expressionprop_id',
    ),
    'unique keys' => array(
      'feature_expressionprop_c1' => array(
        0 => 'feature_expression_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_expressionprop_idx1' => array(
        0 => 'feature_expression_id',
      ),
      'feature_expressionprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_expression' => array(
        'table' => 'feature_expression',
        'columns' => array(
          'feature_expression_id' => 'feature_expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_genotype()
 * Purpose: To describe the structure of 'feature_genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_genotype() {
  $description =  array(
    'table' => 'feature_genotype',
    'fields' => array(
      'feature_genotype_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'genotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'chromosome_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cgroup' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_genotype_id',
    ),
    'unique keys' => array(
      'feature_genotype_c1' => array(
        0 => 'feature_id',
        1 => 'genotype_id',
        2 => 'cvterm_id',
        3 => 'chromosome_id',
        4 => 'rank',
        5 => 'cgroup',
      ),
    ),
    'indexes' => array(
      'feature_genotype_idx1' => array(
        0 => 'feature_id',
      ),
      'feature_genotype_idx2' => array(
        0 => 'genotype_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
          'chromosome_id' => 'feature_id',
        ),
      ),
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_phenotype()
 * Purpose: To describe the structure of 'feature_phenotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_phenotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_phenotype() {
  $description =  array(
    'table' => 'feature_phenotype',
    'fields' => array(
      'feature_phenotype_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'phenotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_phenotype_id',
    ),
    'unique keys' => array(
      'feature_phenotype_c1' => array(
        0 => 'feature_id',
        1 => 'phenotype_id',
      ),
    ),
    'indexes' => array(
      'feature_phenotype_idx1' => array(
        0 => 'feature_id',
      ),
      'feature_phenotype_idx2' => array(
        0 => 'phenotype_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_pub()
 * Purpose: To describe the structure of 'feature_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_pub() {
  $description =  array(
    'table' => 'feature_pub',
    'fields' => array(
      'feature_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_pub_id',
    ),
    'unique keys' => array(
      'feature_pub_c1' => array(
        0 => 'feature_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_pub_idx1' => array(
        0 => 'feature_id',
      ),
      'feature_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_pubprop()
 * Purpose: To describe the structure of 'feature_pubprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_pubprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_pubprop() {
  $description =  array(
    'table' => 'feature_pubprop',
    'fields' => array(
      'feature_pubprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'feature_pubprop_id',
    ),
    'unique keys' => array(
      'feature_pubprop_c1' => array(
        0 => 'feature_pub_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_pubprop_idx1' => array(
        0 => 'feature_pub_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_pub' => array(
        'table' => 'feature_pub',
        'columns' => array(
          'feature_pub_id' => 'feature_pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_relationship()
 * Purpose: To describe the structure of 'feature_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationship() {
  $description =  array(
    'table' => 'feature_relationship',
    'fields' => array(
      'feature_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'feature_relationship_id',
    ),
    'unique keys' => array(
      'feature_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'feature_relationship_idx2' => array(
        0 => 'object_id',
      ),
      'feature_relationship_idx3' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'object_id' => 'feature_id',
          'subject_id' => 'feature_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_relationship_pub()
 * Purpose: To describe the structure of 'feature_relationship_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationship_pub() {
  $description =  array(
    'table' => 'feature_relationship_pub',
    'fields' => array(
      'feature_relationship_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_relationship_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_relationship_pub_id',
    ),
    'unique keys' => array(
      'feature_relationship_pub_c1' => array(
        0 => 'feature_relationship_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_relationship_pub_idx1' => array(
        0 => 'feature_relationship_id',
      ),
      'feature_relationship_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'feature_relationship' => array(
        'table' => 'feature_relationship',
        'columns' => array(
          'feature_relationship_id' => 'feature_relationship_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_relationshipprop()
 * Purpose: To describe the structure of 'feature_relationshipprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationshipprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationshipprop() {
  $description =  array(
    'table' => 'feature_relationshipprop',
    'fields' => array(
      'feature_relationshipprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_relationship_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'feature_relationshipprop_id',
    ),
    'unique keys' => array(
      'feature_relationshipprop_c1' => array(
        0 => 'feature_relationship_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_relationshipprop_idx1' => array(
        0 => 'feature_relationship_id',
      ),
      'feature_relationshipprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_relationship' => array(
        'table' => 'feature_relationship',
        'columns' => array(
          'feature_relationship_id' => 'feature_relationship_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_relationshipprop_pub()
 * Purpose: To describe the structure of 'feature_relationshipprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationshipprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationshipprop_pub() {
  $description =  array(
    'table' => 'feature_relationshipprop_pub',
    'fields' => array(
      'feature_relationshipprop_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_relationshipprop_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'feature_relationshipprop_pub_id',
    ),
    'unique keys' => array(
      'feature_relationshipprop_pub_c1' => array(
        0 => 'feature_relationshipprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_relationshipprop_pub_idx1' => array(
        0 => 'feature_relationshipprop_id',
      ),
      'feature_relationshipprop_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'feature_relationshipprop' => array(
        'table' => 'feature_relationshipprop',
        'columns' => array(
          'feature_relationshipprop_id' => 'feature_relationshipprop_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_synonym()
 * Purpose: To describe the structure of 'feature_synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_synonym() {
  $description =  array(
    'table' => 'feature_synonym',
    'fields' => array(
      'feature_synonym_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'synonym_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'is_internal' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      0 => 'feature_synonym_id',
    ),
    'unique keys' => array(
      'feature_synonym_c1' => array(
        0 => 'synonym_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_synonym_idx1' => array(
        0 => 'synonym_id',
      ),
      'feature_synonym_idx2' => array(
        0 => 'feature_id',
      ),
      'feature_synonym_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'synonym' => array(
        'table' => 'synonym',
        'columns' => array(
          'synonym_id' => 'synonym_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featureloc()
 * Purpose: To describe the structure of 'featureloc' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureloc' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureloc() {
  $description =  array(
    'table' => 'featureloc',
    'fields' => array(
      'featureloc_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'srcfeature_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'fmin' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'is_fmin_partial' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'fmax' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'is_fmax_partial' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
      'strand' => array(
        'type' => 'int',
        'size' => 'small',
        'not NULL' => '',
      ),
      'phase' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'residue_info' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'locgroup' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'featureloc_id',
    ),
    'unique keys' => array(
      'featureloc_c1' => array(
        0 => 'feature_id',
        1 => 'locgroup',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'binloc_boxrange' => array(
        0 => 'fmin',
      ),
      'binloc_boxrange_src' => array(
        0 => 'srcfeature_id',
        1 => 'fmin',
      ),
      'featureloc_idx1' => array(
        0 => 'feature_id',
      ),
      'featureloc_idx2' => array(
        0 => 'srcfeature_id',
      ),
      'featureloc_idx3' => array(
        0 => 'srcfeature_id',
        1 => 'fmin',
        2 => 'fmax',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
          'srcfeature_id' => 'feature_id',
        ),
      ),
    ),
    'referring_tables' => array(
      0 => 'analysisfeature',
      1 => 'featureloc_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featureloc_pub()
 * Purpose: To describe the structure of 'featureloc_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureloc_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureloc_pub() {
  $description =  array(
    'table' => 'featureloc_pub',
    'fields' => array(
      'featureloc_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'featureloc_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'featureloc_pub_id',
    ),
    'unique keys' => array(
      'featureloc_pub_c1' => array(
        0 => 'featureloc_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'featureloc_pub_idx1' => array(
        0 => 'featureloc_id',
      ),
      'featureloc_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'featureloc' => array(
        'table' => 'featureloc',
        'columns' => array(
          'featureloc_id' => 'featureloc_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featuremap()
 * Purpose: To describe the structure of 'featuremap' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featuremap' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featuremap() {
  $description =  array(
    'table' => 'featuremap',
    'fields' => array(
      'featuremap_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'unittype_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'featuremap_id',
    ),
    'unique keys' => array(
      'featuremap_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'unittype_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featuremap_pub()
 * Purpose: To describe the structure of 'featuremap_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featuremap_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featuremap_pub() {
  $description =  array(
    'table' => 'featuremap_pub',
    'fields' => array(
      'featuremap_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'featuremap_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'featuremap_pub_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'featuremap_pub_idx1' => array(
        0 => 'featuremap_id',
      ),
      'featuremap_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'featuremap' => array(
        'table' => 'featuremap',
        'columns' => array(
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featurepos()
 * Purpose: To describe the structure of 'featurepos' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featurepos' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featurepos() {
  $description =  array(
    'table' => 'featurepos',
    'fields' => array(
      'featurepos_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'featuremap_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'map_feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'mappos' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'featurepos_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'featurepos_idx1' => array(
        0 => 'featuremap_id',
      ),
      'featurepos_idx2' => array(
        0 => 'feature_id',
      ),
      'featurepos_idx3' => array(
        0 => 'map_feature_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
          'map_feature_id' => 'feature_id',
        ),
      ),
      'featuremap' => array(
        'table' => 'featuremap',
        'columns' => array(
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featureprop()
 * Purpose: To describe the structure of 'featureprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureprop() {
  $description =  array(
    'table' => 'featureprop',
    'fields' => array(
      'featureprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'featureprop_id',
    ),
    'unique keys' => array(
      'featureprop_c1' => array(
        0 => 'feature_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'featureprop_idx1' => array(
        0 => 'feature_id',
      ),
      'featureprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
      0 => 'analysisfeature',
      1 => 'featureprop_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featureprop_pub()
 * Purpose: To describe the structure of 'featureprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureprop_pub() {
  $description =  array(
    'table' => 'featureprop_pub',
    'fields' => array(
      'featureprop_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'featureprop_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'featureprop_pub_id',
    ),
    'unique keys' => array(
      'featureprop_pub_c1' => array(
        0 => 'featureprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'featureprop_pub_idx1' => array(
        0 => 'featureprop_id',
      ),
      'featureprop_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'featureprop' => array(
        'table' => 'featureprop',
        'columns' => array(
          'featureprop_id' => 'featureprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featurerange()
 * Purpose: To describe the structure of 'featurerange' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featurerange' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featurerange() {
  $description =  array(
    'table' => 'featurerange',
    'fields' => array(
      'featurerange_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'featuremap_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'leftstartf_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'leftendf_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'rightstartf_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'rightendf_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rangestr' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'featurerange_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'featurerange_idx1' => array(
        0 => 'featuremap_id',
      ),
      'featurerange_idx2' => array(
        0 => 'feature_id',
      ),
      'featurerange_idx3' => array(
        0 => 'leftstartf_id',
      ),
      'featurerange_idx4' => array(
        0 => 'leftendf_id',
      ),
      'featurerange_idx5' => array(
        0 => 'rightstartf_id',
      ),
      'featurerange_idx6' => array(
        0 => 'rightendf_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
          'leftstartf_id' => 'feature_id',
          'leftendf_id' => 'feature_id',
          'rightstartf_id' => 'feature_id',
          'rightendf_id' => 'feature_id',
        ),
      ),
      'featuremap' => array(
        'table' => 'featuremap',
        'columns' => array(
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_genotype()
 * Purpose: To describe the structure of 'genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_genotype() {
  $description =  array(
    'table' => 'genotype',
    'fields' => array(
      'genotype_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'genotype_id',
    ),
    'unique keys' => array(
      'genotype_c1' => array(
        0 => 'uniquename',
      ),
    ),
    'indexes' => array(
      'genotype_idx1' => array(
        0 => 'uniquename',
      ),
      'genotype_idx2' => array(
        0 => 'name',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_library()
 * Purpose: To describe the structure of 'library' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library() {
  $description =  array(
    'table' => 'library',
    'referring_tables' => array(
      0 => 'cell_line_library',
      1 => 'library_cvterm',
      2 => 'library_dbxref',
      3 => 'library_feature',
      4 => 'library_pub',
      5 => 'library_synonym',
      6 => 'libraryprop',
    ),
    'fields' => array(
      'library_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_obsolete' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
      'timeaccessioned' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      0 => 'library_id',
    ),
    'unique keys' => array(
      'library_c1' => array(
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'library_idx1' => array(
        0 => 'organism_id',
      ),
      'library_idx2' => array(
        0 => 'type_id',
      ),
      'library_idx3' => array(
        0 => 'uniquename',
      ),
      'library_name_ind1' => array(
        0 => 'name',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_library_cvterm()
 * Purpose: To describe the structure of 'library_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_cvterm() {
  $description =  array(
    'table' => 'library_cvterm',
    'fields' => array(
      'library_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'library_cvterm_id',
    ),
    'unique keys' => array(
      'library_cvterm_c1' => array(
        0 => 'library_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'library_cvterm_idx1' => array(
        0 => 'library_id',
      ),
      'library_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
      'library_cvterm_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_library_dbxref()
 * Purpose: To describe the structure of 'library_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_dbxref() {
  $description =  array(
    'table' => 'library_dbxref',
    'fields' => array(
      'library_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      0 => 'library_dbxref_id',
    ),
    'unique keys' => array(
      'library_dbxref_c1' => array(
        0 => 'library_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'library_dbxref_idx1' => array(
        0 => 'library_id',
      ),
      'library_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_library_feature()
 * Purpose: To describe the structure of 'library_feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_feature() {
  $description =  array(
    'table' => 'library_feature',
    'fields' => array(
      'library_feature_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'library_feature_id',
    ),
    'unique keys' => array(
      'library_feature_c1' => array(
        0 => 'library_id',
        1 => 'feature_id',
      ),
    ),
    'indexes' => array(
      'library_feature_idx1' => array(
        0 => 'library_id',
      ),
      'library_feature_idx2' => array(
        0 => 'feature_id',
      ),
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_library_pub()
 * Purpose: To describe the structure of 'library_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_pub() {
  $description =  array(
    'table' => 'library_pub',
    'fields' => array(
      'library_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'library_pub_id',
    ),
    'unique keys' => array(
      'library_pub_c1' => array(
        0 => 'library_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'library_pub_idx1' => array(
        0 => 'library_id',
      ),
      'library_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_library_synonym()
 * Purpose: To describe the structure of 'library_synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_synonym() {
  $description =  array(
    'table' => 'library_synonym',
    'fields' => array(
      'library_synonym_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'synonym_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'ru',
      ),
      'is_internal' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      0 => 'library_synonym_id',
    ),
    'unique keys' => array(
      'library_synonym_c1' => array(
        0 => 'synonym_id',
        1 => 'library_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'library_synonym_idx1' => array(
        0 => 'synonym_id',
      ),
      'library_synonym_idx2' => array(
        0 => 'library_id',
      ),
      'library_synonym_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'synonym' => array(
        'table' => 'synonym',
        'columns' => array(
          'synonym_id' => 'synonym_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_libraryprop()
 * Purpose: To describe the structure of 'libraryprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'libraryprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_libraryprop() {
  $description =  array(
    'table' => 'libraryprop',
    'fields' => array(
      'libraryprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'library_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'libraryprop_id',
    ),
    'unique keys' => array(
      'libraryprop_c1' => array(
        0 => 'library_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'libraryprop_idx1' => array(
        0 => 'library_id',
      ),
      'libraryprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'library' => array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_libraryprop_pub()
 * Purpose: To describe the structure of 'libraryprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'libraryprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_libraryprop_pub() {
  $description =  array(
    'table' => 'libraryprop_pub',
    'fields' => array(
      'libraryprop_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'libraryprop_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'libraryprop_pub_id',
    ),
    'unique keys' => array(
      'libraryprop_pub_c1' => array(
        0 => 'libraryprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'libraryprop_pub_idx1' => array(
        0 => 'libraryprop_id',
      ),
      'libraryprop_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'libraryprop' => array(
        'table' => 'libraryprop',
        'columns' => array(
          'libraryprop_id' => 'libraryprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_magedocumentation()
 * Purpose: To describe the structure of 'magedocumentation' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'magedocumentation' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_magedocumentation() {
  $description =  array(
    'table' => 'magedocumentation',
    'fields' => array(
      'magedocumentation_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'mageml_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'tableinfo_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'row_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'mageidentifier' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'magedocumentation_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'magedocumentation_idx1' => array(
        0 => 'mageml_id',
      ),
      'magedocumentation_idx2' => array(
        0 => 'tableinfo_id',
      ),
      'magedocumentation_idx3' => array(
        0 => 'row_id',
      ),
    ),
    'foreign keys' => array(
      'tableinfo' => array(
        'table' => 'tableinfo',
        'columns' => array(
          'tableinfo_id' => 'tableinfo_id',
        ),
      ),
      'mageml' => array(
        'table' => 'mageml',
        'columns' => array(
          'mageml_id' => 'mageml_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_mageml()
 * Purpose: To describe the structure of 'mageml' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'mageml' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_mageml() {
  $description =  array(
    'table' => 'mageml',
    'fields' => array(
      'mageml_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'mage_package' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'mage_ml' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'mageml_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_materialized_view()
 * Purpose: To describe the structure of 'materialized_view' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'materialized_view' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_materialized_view() {
  $description =  array(
    'table' => 'materialized_view',
    'fields' => array(
      'materialized_view_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'last_update' => array(
        'type' => 'datetime',
        'not NULL' => '',
      ),
      'refresh_time' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '64',
        'not NULL' => '',
      ),
      'mv_schema' => array(
        'type' => 'varchar',
        'length' => '64',
        'not NULL' => '',
      ),
      'mv_table' => array(
        'type' => 'varchar',
        'length' => '128',
        'not NULL' => '',
      ),
      'mv_specs' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'indexed' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'query' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'special_index' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
      'name' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_organism()
 * Purpose: To describe the structure of 'organism' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'organism' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_organism() {
  $description =  array(
    'table' => 'organism',
    'fields' => array(
      'organism_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'abbreviation' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'genus' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'species' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'common_name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'comment' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'organism_id',
    ),
    'unique keys' => array(
      'organism_c1' => array(
        0 => 'genus',
        1 => 'species',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
    'referring_tables' => array(
      0 => 'biomaterial',
      1 => 'feature',
      2 => 'library',
      3 => 'organism_dbxref',
      4 => 'organismprop',
      5 => 'phylonode_organism',
      6 => 'stock',
      7 => 'wwwuser_organism',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_organism_dbxref()
 * Purpose: To describe the structure of 'organism_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'organism_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_organism_dbxref() {
  $description =  array(
    'table' => 'organism_dbxref',
    'fields' => array(
      'organism_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'organism_dbxref_id',
    ),
    'unique keys' => array(
      'organism_dbxref_c1' => array(
        0 => 'organism_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'organism_dbxref_idx1' => array(
        0 => 'organism_id',
      ),
      'organism_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_organismprop()
 * Purpose: To describe the structure of 'organismprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'organismprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_organismprop() {
  $description =  array(
    'table' => 'organismprop',
    'fields' => array(
      'organismprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'organismprop_id',
    ),
    'unique keys' => array(
      'organismprop_c1' => array(
        0 => 'organism_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'organismprop_idx1' => array(
        0 => 'organism_id',
      ),
      'organismprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phendesc()
 * Purpose: To describe the structure of 'phendesc' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phendesc' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phendesc() {
  $description =  array(
    'table' => 'phendesc',
    'fields' => array(
      'phendesc_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'genotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'environment_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phendesc_id',
    ),
    'unique keys' => array(
      'phendesc_c1' => array(
        0 => 'genotype_id',
        1 => 'environment_id',
        2 => 'type_id',
        3 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phendesc_idx1' => array(
        0 => 'genotype_id',
      ),
      'phendesc_idx2' => array(
        0 => 'environment_id',
      ),
      'phendesc_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
      'environment' => array(
        'table' => 'environment',
        'columns' => array(
          'environment_id' => 'environment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phenotype()
 * Purpose: To describe the structure of 'phenotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype() {
  $description =  array(
    'table' => 'phenotype',
    'fields' => array(
      'phenotype_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'observable_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'attr_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'cvalue_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'phenotype_id',
    ),
    'unique keys' => array(
      'phenotype_c1' => array(
        0 => 'uniquename',
      ),
    ),
    'indexes' => array(
      'phenotype_idx1' => array(
        0 => 'cvalue_id',
      ),
      'phenotype_idx2' => array(
        0 => 'observable_id',
      ),
      'phenotype_idx3' => array(
        0 => 'attr_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'observable_id' => 'cvterm_id',
          'attr_id' => 'cvterm_id',
          'cvalue_id' => 'cvterm_id',
          'assay_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phenotype_comparison()
 * Purpose: To describe the structure of 'phenotype_comparison' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype_comparison' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype_comparison() {
  $description =  array(
    'table' => 'phenotype_comparison',
    'fields' => array(
      'phenotype_comparison_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'genotype1_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'environment1_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'genotype2_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'environment2_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'phenotype1_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'phenotype2_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phenotype_comparison_id',
    ),
    'unique keys' => array(
      'phenotype_comparison_c1' => array(
        0 => 'genotype1_id',
        1 => 'environment1_id',
        2 => 'genotype2_id',
        3 => 'environment2_id',
        4 => 'phenotype1_id',
        5 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phenotype_comparison_idx1' => array(
        0 => 'genotype1_id',
      ),
      'phenotype_comparison_idx2' => array(
        0 => 'genotype2_id',
      ),
      'phenotype_comparison_idx4' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype1_id' => 'phenotype_id',
          'phenotype2_id' => 'phenotype_id',
        ),
      ),
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype1_id' => 'genotype_id',
          'genotype2_id' => 'genotype_id',
        ),
      ),
      'environment' => array(
        'table' => 'environment',
        'columns' => array(
          'environment1_id' => 'environment_id',
          'environment2_id' => 'environment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phenotype_comparison_cvterm()
 * Purpose: To describe the structure of 'phenotype_comparison_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype_comparison_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype_comparison_cvterm() {
  $description =  array(
    'table' => 'phenotype_comparison_cvterm',
    'fields' => array(
      'phenotype_comparison_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phenotype_comparison_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'phenotype_comparison_cvterm_id',
    ),
    'unique keys' => array(
      'phenotype_comparison_cvterm_c1' => array(
        0 => 'phenotype_comparison_id',
        1 => 'cvterm_id',
      ),
    ),
    'indexes' => array(
      'phenotype_comparison_cvterm_idx1' => array(
        0 => 'phenotype_comparison_id',
      ),
      'phenotype_comparison_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'phenotype_comparison' => array(
        'table' => 'phenotype_comparison',
        'columns' => array(
          'phenotype_comparison_id' => 'phenotype_comparison_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phenotype_cvterm()
 * Purpose: To describe the structure of 'phenotype_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype_cvterm() {
  $description =  array(
    'table' => 'phenotype_cvterm',
    'fields' => array(
      'phenotype_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phenotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'phenotype_cvterm_id',
    ),
    'unique keys' => array(
      'phenotype_cvterm_c1' => array(
        0 => 'phenotype_id',
        1 => 'cvterm_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'phenotype_cvterm_idx1' => array(
        0 => 'phenotype_id',
      ),
      'phenotype_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phenstatement()
 * Purpose: To describe the structure of 'phenstatement' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenstatement' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenstatement() {
  $description =  array(
    'table' => 'phenstatement',
    'fields' => array(
      'phenstatement_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'genotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'environment_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'phenotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phenstatement_id',
    ),
    'unique keys' => array(
      'phenstatement_c1' => array(
        0 => 'genotype_id',
        1 => 'phenotype_id',
        2 => 'environment_id',
        3 => 'type_id',
        4 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phenstatement_idx1' => array(
        0 => 'genotype_id',
      ),
      'phenstatement_idx2' => array(
        0 => 'phenotype_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype_id' => 'phenotype_id',
        ),
      ),
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
      'environment' => array(
        'table' => 'environment',
        'columns' => array(
          'environment_id' => 'environment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylonode()
 * Purpose: To describe the structure of 'phylonode' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode() {
  $description =  array(
    'table' => 'phylonode',
    'fields' => array(
      'phylonode_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phylotree_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'parent_phylonode_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'left_idx' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'right_idx' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'label' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'distance' => array(
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'phylonode_id',
    ),
    'unique keys' => array(
      'phylotree_id' => array(
        0 => 'phylotree_id',
        1 => 'left_idx',
      ),
      'phylonode_phylotree_id_key1' => array(
        0 => 'phylotree_id',
        1 => 'right_idx',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'phylotree' => array(
        'table' => 'phylotree',
        'columns' => array(
          'phylotree_id' => 'phylotree_id',
        ),
      ),
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'parent_phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylonode_dbxref()
 * Purpose: To describe the structure of 'phylonode_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_dbxref() {
  $description =  array(
    'table' => 'phylonode_dbxref',
    'fields' => array(
      'phylonode_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phylonode_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phylonode_dbxref_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        0 => 'phylonode_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'phylonode_dbxref_idx1' => array(
        0 => 'phylonode_id',
      ),
      'phylonode_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylonode_organism()
 * Purpose: To describe the structure of 'phylonode_organism' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_organism' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_organism() {
  $description =  array(
    'table' => 'phylonode_organism',
    'fields' => array(
      'phylonode_organism_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phylonode_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phylonode_organism_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        0 => 'phylonode_id',
      ),
    ),
    'indexes' => array(
      'phylonode_organism_idx1' => array(
        0 => 'phylonode_id',
      ),
      'phylonode_organism_idx2' => array(
        0 => 'organism_id',
      ),
    ),
    'foreign keys' => array(
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylonode_pub()
 * Purpose: To describe the structure of 'phylonode_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_pub() {
  $description =  array(
    'table' => 'phylonode_pub',
    'fields' => array(
      'phylonode_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phylonode_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phylonode_pub_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        0 => 'phylonode_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phylonode_pub_idx1' => array(
        0 => 'phylonode_id',
      ),
      'phylonode_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylonode_relationship()
 * Purpose: To describe the structure of 'phylonode_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_relationship() {
  $description =  array(
    'table' => 'phylonode_relationship',
    'fields' => array(
      'phylonode_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'phylotree_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phylonode_relationship_id',
    ),
    'unique keys' => array(
      'subject_id' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'phylonode_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'phylonode_relationship_idx2' => array(
        0 => 'object_id',
      ),
      'phylonode_relationship_idx3' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'phylotree' => array(
        'table' => 'phylotree',
        'columns' => array(
          'phylotree_id' => 'phylotree_id',
        ),
      ),
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'subject_id' => 'phylonode_id',
          'object_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylonodeprop()
 * Purpose: To describe the structure of 'phylonodeprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonodeprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonodeprop() {
  $description =  array(
    'table' => 'phylonodeprop',
    'fields' => array(
      'phylonodeprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phylonode_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '1',
        'default' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'phylonodeprop_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        0 => 'phylonode_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'phylonodeprop_idx1' => array(
        0 => 'phylonode_id',
      ),
      'phylonodeprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylotree()
 * Purpose: To describe the structure of 'phylotree' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylotree' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylotree() {
  $description =  array(
    'table' => 'phylotree',
    'fields' => array(
      'phylotree_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'analysis_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'comment' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'phylotree_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'phylotree_idx1' => array(
        0 => 'phylotree_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phylotree_pub()
 * Purpose: To describe the structure of 'phylotree_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylotree_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylotree_pub() {
  $description =  array(
    'table' => 'phylotree_pub',
    'fields' => array(
      'phylotree_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'phylotree_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'phylotree_pub_id',
    ),
    'unique keys' => array(
      'phylotree_id' => array(
        0 => 'phylotree_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phylotree_pub_idx1' => array(
        0 => 'phylotree_id',
      ),
      'phylotree_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'phylotree' => array(
        'table' => 'phylotree',
        'columns' => array(
          'phylotree_id' => 'phylotree_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_project()
 * Purpose: To describe the structure of 'project' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'project' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_project() {
  $description =  array(
    'table' => 'project',
    'fields' => array(
      'project_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'project_id',
    ),
    'unique keys' => array(
      'project_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_protocol()
 * Purpose: To describe the structure of 'protocol' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'protocol' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_protocol() {
  $description =  array(
    'table' => 'protocol',
    'fields' => array(
      'protocol_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'uri' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'protocoldescription' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'hardwaredescription' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'softwaredescription' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'protocol_id',
    ),
    'unique keys' => array(
      'protocol_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
      'protocol_idx1' => array(
        0 => 'type_id',
      ),
      'protocol_idx2' => array(
        0 => 'pub_id',
      ),
      'protocol_idx3' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_protocolparam()
 * Purpose: To describe the structure of 'protocolparam' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'protocolparam' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_protocolparam() {
  $description =  array(
    'table' => 'protocolparam',
    'fields' => array(
      'protocolparam_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'datatype_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'unittype_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'protocolparam_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'protocolparam_idx1' => array(
        0 => 'protocol_id',
      ),
      'protocolparam_idx2' => array(
        0 => 'datatype_id',
      ),
      'protocolparam_idx3' => array(
        0 => 'unittype_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'datatype_id' => 'cvterm_id',
          'unittype_id' => 'cvterm_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_pub()
 * Purpose: To describe the structure of 'pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pub() {
  $description =  array(
    'table' => 'pub',
    'fields' => array(
      'pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'title' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'volumetitle' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'volume' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'series_name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'issue' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'pyear' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'pages' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'miniref' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_obsolete' => array(
        'type' => 'boolean',
        'not NULL' => '',
        'default' => 'als',
      ),
      'publisher' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'pubplace' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'pub_id',
    ),
    'unique keys' => array(
      'pub_c1' => array(
        0 => 'uniquename',
      ),
    ),
    'indexes' => array(
      'pub_idx1' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_pub_dbxref()
 * Purpose: To describe the structure of 'pub_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pub_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pub_dbxref() {
  $description =  array(
    'table' => 'pub_dbxref',
    'fields' => array(
      'pub_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      0 => 'pub_dbxref_id',
    ),
    'unique keys' => array(
      'pub_dbxref_c1' => array(
        0 => 'pub_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'pub_dbxref_idx1' => array(
        0 => 'pub_id',
      ),
      'pub_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_pub_relationship()
 * Purpose: To describe the structure of 'pub_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pub_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pub_relationship() {
  $description =  array(
    'table' => 'pub_relationship',
    'fields' => array(
      'pub_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'pub_relationship_id',
    ),
    'unique keys' => array(
      'pub_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'pub_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'pub_relationship_idx2' => array(
        0 => 'object_id',
      ),
      'pub_relationship_idx3' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'subject_id' => 'pub_id',
          'object_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_pubauthor()
 * Purpose: To describe the structure of 'pubauthor' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pubauthor' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pubauthor() {
  $description =  array(
    'table' => 'pubauthor',
    'fields' => array(
      'pubauthor_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'editor' => array(
        'type' => 'boolean',
        'not NULL' => '',
        'default' => 'als',
      ),
      'surname' => array(
        'type' => 'varchar',
        'length' => '100',
        'not NULL' => '1',
      ),
      'givennames' => array(
        'type' => 'varchar',
        'length' => '100',
        'not NULL' => '',
      ),
      'suffix' => array(
        'type' => 'varchar',
        'length' => '100',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'pubauthor_id',
    ),
    'unique keys' => array(
      'pubauthor_c1' => array(
        0 => 'pub_id',
        1 => 'rank',
      ),
    ),
    'indexes' => array(
      'pubauthor_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_pubprop()
 * Purpose: To describe the structure of 'pubprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pubprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pubprop() {
  $description =  array(
    'table' => 'pubprop',
    'fields' => array(
      'pubprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'pubprop_id',
    ),
    'unique keys' => array(
      'pubprop_c1' => array(
        0 => 'pub_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'pubprop_idx1' => array(
        0 => 'pub_id',
      ),
      'pubprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_quantification()
 * Purpose: To describe the structure of 'quantification' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'quantification' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_quantification() {
  $description =  array(
    'table' => 'quantification',
    'fields' => array(
      'quantification_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'acquisition_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'operator_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'analysis_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'quantificationdate' => array(
        'type' => 'datetime',
        'not NULL' => '',
        'default' => 'ow(',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'uri' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'quantification_id',
    ),
    'unique keys' => array(
      'quantification_c1' => array(
        0 => 'name',
        1 => 'analysis_id',
      ),
    ),
    'indexes' => array(
      'quantification_idx1' => array(
        0 => 'acquisition_id',
      ),
      'quantification_idx2' => array(
        0 => 'operator_id',
      ),
      'quantification_idx3' => array(
        0 => 'protocol_id',
      ),
      'quantification_idx4' => array(
        0 => 'analysis_id',
      ),
    ),
    'foreign keys' => array(
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'operator_id' => 'contact_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
      'acquisition' => array(
        'table' => 'acquisition',
        'columns' => array(
          'acquisition_id' => 'acquisition_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_quantification_relationship()
 * Purpose: To describe the structure of 'quantification_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'quantification_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_quantification_relationship() {
  $description =  array(
    'table' => 'quantification_relationship',
    'fields' => array(
      'quantification_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'quantification_relationship_id',
    ),
    'unique keys' => array(
      'quantification_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'quantification_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'quantification_relationship_idx2' => array(
        0 => 'type_id',
      ),
      'quantification_relationship_idx3' => array(
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'quantification' => array(
        'table' => 'quantification',
        'columns' => array(
          'subject_id' => 'quantification_id',
          'object_id' => 'quantification_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_quantificationprop()
 * Purpose: To describe the structure of 'quantificationprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'quantificationprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_quantificationprop() {
  $description =  array(
    'table' => 'quantificationprop',
    'fields' => array(
      'quantificationprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'quantification_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'quantificationprop_id',
    ),
    'unique keys' => array(
      'quantificationprop_c1' => array(
        0 => 'quantification_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'quantificationprop_idx1' => array(
        0 => 'quantification_id',
      ),
      'quantificationprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'quantification' => array(
        'table' => 'quantification',
        'columns' => array(
          'quantification_id' => 'quantification_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock()
 * Purpose: To describe the structure of 'stock' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock() {
  $description =  array(
    'table' => 'stock',
    'referring_tables' => array(
        1 => 'stock_cvterm',
        2 => 'stock_dbxref',
        3 => 'stock_genotype',
        4 => 'stock_pub',
        5 => 'stock_relationship',
        7 => 'stockcollection_stock',
        8 => 'stockprop',
     ),
    'fields' => array(
      'stock_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'organism_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_obsolete' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      0 => 'stock_id',
    ),
    'unique keys' => array(
      'stock_c1' => array(
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ),
    ),
    'indexes' => array(
      'stock_idx1' => array(
        0 => 'dbxref_id',
      ),
      'stock_idx2' => array(
        0 => 'organism_id',
      ),
      'stock_idx3' => array(
        0 => 'type_id',
      ),
      'stock_idx4' => array(
        0 => 'uniquename',
      ),
      'stock_name_ind1' => array(
        0 => 'name',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_cvterm()
 * Purpose: To describe the structure of 'stock_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_cvterm() {
  $description =  array(
    'table' => 'stock_cvterm',
    'fields' => array(
      'stock_cvterm_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'cvterm_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stock_cvterm_id',
    ),
    'unique keys' => array(
      'stock_cvterm_c1' => array(
        0 => 'stock_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stock_cvterm_idx1' => array(
        0 => 'stock_id',
      ),
      'stock_cvterm_idx2' => array(
        0 => 'cvterm_id',
      ),
      'stock_cvterm_idx3' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_dbxref()
 * Purpose: To describe the structure of 'stock_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_dbxref() {
  $description =  array(
    'table' => 'stock_dbxref',
    'fields' => array(
      'stock_dbxref_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'is_current' => array(
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      0 => 'stock_dbxref_id',
    ),
    'unique keys' => array(
      'stock_dbxref_c1' => array(
        0 => 'stock_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'stock_dbxref_idx1' => array(
        0 => 'stock_id',
      ),
      'stock_dbxref_idx2' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_genotype()
 * Purpose: To describe the structure of 'stock_genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_genotype() {
  $description =  array(
    'table' => 'stock_genotype',
    'fields' => array(
      'stock_genotype_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'genotype_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stock_genotype_id',
    ),
    'unique keys' => array(
      'stock_genotype_c1' => array(
        0 => 'stock_id',
        1 => 'genotype_id',
      ),
    ),
    'indexes' => array(
      'stock_genotype_idx1' => array(
        0 => 'stock_id',
      ),
      'stock_genotype_idx2' => array(
        0 => 'genotype_id',
      ),
    ),
    'foreign keys' => array(
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_pub()
 * Purpose: To describe the structure of 'stock_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_pub() {
  $description =  array(
    'table' => 'stock_pub',
    'fields' => array(
      'stock_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stock_pub_id',
    ),
    'unique keys' => array(
      'stock_pub_c1' => array(
        0 => 'stock_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stock_pub_idx1' => array(
        0 => 'stock_id',
      ),
      'stock_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_relationship()
 * Purpose: To describe the structure of 'stock_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_relationship() {
  $description =  array(
    'table' => 'stock_relationship',
    'fields' => array(
      'stock_relationship_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'subject_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'object_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'stock_relationship_id',
    ),
    'unique keys' => array(
      'stock_relationship_c1' => array(
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array(
      'stock_relationship_idx1' => array(
        0 => 'subject_id',
      ),
      'stock_relationship_idx2' => array(
        0 => 'object_id',
      ),
      'stock_relationship_idx3' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'subject_id' => 'stock_id',
          'object_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_relationship_pub()
 * Purpose: To describe the structure of 'stock_relationship_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_relationship_pub() {
  $description =  array(
    'table' => 'stock_relationship_pub',
    'fields' => array(
      'stock_relationship_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stock_relationship_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stock_relationship_pub_id',
    ),
    'unique keys' => array(
      'stock_relationship_pub_c1' => array(
        0 => 'stock_relationship_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stock_relationship_pub_idx1' => array(
        0 => 'stock_relationship_id',
      ),
      'stock_relationship_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'stock_relationship' => array(
        'table' => 'stock_relationship',
        'columns' => array(
          'stock_relationship_id' => 'stock_relationship_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stockcollection()
 * Purpose: To describe the structure of 'stockcollection' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockcollection' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockcollection() {
  $description =  array(
    'table' => 'stockcollection',
    'fields' => array(
      'stockcollection_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'contact_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ),
      'uniquename' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stockcollection_id',
    ),
    'unique keys' => array(
      'stockcollection_c1' => array(
        0 => 'uniquename',
        1 => 'type_id',
      ),
    ),
    'indexes' => array(
      'stockcollection_idx1' => array(
        0 => 'contact_id',
      ),
      'stockcollection_idx2' => array(
        0 => 'type_id',
      ),
      'stockcollection_idx3' => array(
        0 => 'uniquename',
      ),
      'stockcollection_name_ind1' => array(
        0 => 'name',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'contact_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stockcollection_stock()
 * Purpose: To describe the structure of 'stockcollection_stock' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockcollection_stock' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockcollection_stock() {
  $description =  array(
    'table' => 'stockcollection_stock',
    'fields' => array(
      'stockcollection_stock_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stockcollection_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stockcollection_stock_id',
    ),
    'unique keys' => array(
      'stockcollection_stock_c1' => array(
        0 => 'stockcollection_id',
        1 => 'stock_id',
      ),
    ),
    'indexes' => array(
      'stockcollection_stock_idx1' => array(
        0 => 'stockcollection_id',
      ),
      'stockcollection_stock_idx2' => array(
        0 => 'stock_id',
      ),
    ),
    'foreign keys' => array(
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
      'stockcollection' => array(
        'table' => 'stockcollection',
        'columns' => array(
          'stockcollection_id' => 'stockcollection_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stockcollectionprop()
 * Purpose: To describe the structure of 'stockcollectionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockcollectionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockcollectionprop() {
  $description =  array(
    'table' => 'stockcollectionprop',
    'fields' => array(
      'stockcollectionprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stockcollection_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'stockcollectionprop_id',
    ),
    'unique keys' => array(
      'stockcollectionprop_c1' => array(
        0 => 'stockcollection_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'stockcollectionprop_idx1' => array(
        0 => 'stockcollection_id',
      ),
      'stockcollectionprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'stockcollection' => array(
        'table' => 'stockcollection',
        'columns' => array(
          'stockcollection_id' => 'stockcollection_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stockprop()
 * Purpose: To describe the structure of 'stockprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockprop() {
  $description =  array(
    'table' => 'stockprop',
    'fields' => array(
      'stockprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'stockprop_id',
    ),
    'unique keys' => array(
      'stockprop_c1' => array(
        0 => 'stock_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'stockprop_idx1' => array(
        0 => 'stock_id',
      ),
      'stockprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stockprop_pub()
 * Purpose: To describe the structure of 'stockprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockprop_pub() {
  $description =  array(
    'table' => 'stockprop_pub',
    'fields' => array(
      'stockprop_pub_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'stockprop_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'stockprop_pub_id',
    ),
    'unique keys' => array(
      'stockprop_pub_c1' => array(
        0 => 'stockprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stockprop_pub_idx1' => array(
        0 => 'stockprop_id',
      ),
      'stockprop_pub_idx2' => array(
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array(
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'stockprop' => array(
        'table' => 'stockprop',
        'columns' => array(
          'stockprop_id' => 'stockprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_study()
 * Purpose: To describe the structure of 'study' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'study' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_study() {
  $description =  array(
    'table' => 'study',
    'fields' => array(
      'study_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'contact_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'study_id',
    ),
    'unique keys' => array(
      'study_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
      'study_idx1' => array(
        0 => 'contact_id',
      ),
      'study_idx2' => array(
        0 => 'pub_id',
      ),
      'study_idx3' => array(
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'contact_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_study_assay()
 * Purpose: To describe the structure of 'study_assay' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'study_assay' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_study_assay() {
  $description =  array(
    'table' => 'study_assay',
    'fields' => array(
      'study_assay_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'study_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'study_assay_id',
    ),
    'unique keys' => array(
      'study_assay_c1' => array(
        0 => 'study_id',
        1 => 'assay_id',
      ),
    ),
    'indexes' => array(
      'study_assay_idx1' => array(
        0 => 'study_id',
      ),
      'study_assay_idx2' => array(
        0 => 'assay_id',
      ),
    ),
    'foreign keys' => array(
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
      'study' => array(
        'table' => 'study',
        'columns' => array(
          'study_id' => 'study_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_studydesign()
 * Purpose: To describe the structure of 'studydesign' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studydesign' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studydesign() {
  $description =  array(
    'table' => 'studydesign',
    'fields' => array(
      'studydesign_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'study_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'studydesign_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'studydesign_idx1' => array(
        0 => 'study_id',
      ),
    ),
    'foreign keys' => array(
      'study' => array(
        'table' => 'study',
        'columns' => array(
          'study_id' => 'study_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_studydesignprop()
 * Purpose: To describe the structure of 'studydesignprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studydesignprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studydesignprop() {
  $description =  array(
    'table' => 'studydesignprop',
    'fields' => array(
      'studydesignprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'studydesign_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'studydesignprop_id',
    ),
    'unique keys' => array(
      'studydesignprop_c1' => array(
        0 => 'studydesign_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'studydesignprop_idx1' => array(
        0 => 'studydesign_id',
      ),
      'studydesignprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'studydesign' => array(
        'table' => 'studydesign',
        'columns' => array(
          'studydesign_id' => 'studydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_studyfactor()
 * Purpose: To describe the structure of 'studyfactor' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyfactor' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyfactor() {
  $description =  array(
    'table' => 'studyfactor',
    'fields' => array(
      'studyfactor_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'studydesign_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '1',
      ),
      'description' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'studyfactor_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'studyfactor_idx1' => array(
        0 => 'studydesign_id',
      ),
      'studyfactor_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'studydesign' => array(
        'table' => 'studydesign',
        'columns' => array(
          'studydesign_id' => 'studydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_studyfactorvalue()
 * Purpose: To describe the structure of 'studyfactorvalue' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyfactorvalue' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyfactorvalue() {
  $description =  array(
    'table' => 'studyfactorvalue',
    'fields' => array(
      'studyfactorvalue_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'studyfactor_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'assay_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'factorvalue' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'studyfactorvalue_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'studyfactorvalue_idx1' => array(
        0 => 'studyfactor_id',
      ),
      'studyfactorvalue_idx2' => array(
        0 => 'assay_id',
      ),
    ),
    'foreign keys' => array(
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
      'studyfactor' => array(
        'table' => 'studyfactor',
        'columns' => array(
          'studyfactor_id' => 'studyfactor_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_studyprop()
 * Purpose: To describe the structure of 'studyprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyprop() {
  $description =  array(
    'table' => 'studyprop',
    'fields' => array(
      'studyprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'study_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      0 => 'studyprop_id',
    ),
    'unique keys' => array(
      'study_id' => array(
        0 => 'study_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'studyprop_idx1' => array(
        0 => 'study_id',
      ),
      'studyprop_idx2' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'study' => array(
        'table' => 'study',
        'columns' => array(
          'study_id' => 'study_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_studyprop_feature()
 * Purpose: To describe the structure of 'studyprop_feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyprop_feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyprop_feature() {
  $description =  array(
    'table' => 'studyprop_feature',
    'fields' => array(
      'studyprop_feature_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'studyprop_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'studyprop_feature_id',
    ),
    'unique keys' => array(
      'studyprop_id' => array(
        0 => 'studyprop_id',
        1 => 'feature_id',
      ),
    ),
    'indexes' => array(
      'studyprop_feature_idx1' => array(
        0 => 'studyprop_id',
      ),
      'studyprop_feature_idx2' => array(
        0 => 'feature_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'studyprop' => array(
        'table' => 'studyprop',
        'columns' => array(
          'studyprop_id' => 'studyprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_synonym()
 * Purpose: To describe the structure of 'synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_synonym() {
  $description =  array(
    'table' => 'synonym',
    'fields' => array(
      'synonym_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'synonym_sgml' => array(
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'synonym_id',
    ),
    'unique keys' => array(
      'synonym_c1' => array(
        0 => 'name',
        1 => 'type_id',
      ),
    ),
    'indexes' => array(
      'synonym_idx1' => array(
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_tableinfo()
 * Purpose: To describe the structure of 'tableinfo' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'tableinfo' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_tableinfo() {
  $description =  array(
    'table' => 'tableinfo',
    'fields' => array(
      'tableinfo_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => '30',
        'not NULL' => '1',
      ),
      'primary_key_column' => array(
        'type' => 'varchar',
        'length' => '30',
        'not NULL' => '',
      ),
      'is_view' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
      'view_on_table_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'superclass_table_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'is_updateable' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '1',
      ),
      'modification_date' => array(
        'type' => 'date',
        'not NULL' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      0 => 'tableinfo_id',
    ),
    'unique keys' => array(
      'tableinfo_c1' => array(
        0 => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_treatment()
 * Purpose: To describe the structure of 'treatment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'treatment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_treatment() {
  $description =  array(
    'table' => 'treatment',
    'fields' => array(
      'treatment_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ),
      'biomaterial_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not NULL' => '',
      ),
      'name' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
    ),
    'primary key' => array(
      0 => 'treatment_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'treatment_idx1' => array(
        0 => 'biomaterial_id',
      ),
      'treatment_idx2' => array(
        0 => 'type_id',
      ),
      'treatment_idx3' => array(
        0 => 'protocol_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
      'biomaterial' => array(
        'table' => 'biomaterial',
        'columns' => array(
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}

/**
 * Lists the table names in the v1.11 chado schema
 *
 * @return
 *    An array containing all of the table names
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_get_v1_11_tables() {
  $tables = array(
    'acquisition',
    'acquisition_relationship',
    'acquisitionprop',
    'analysis',
    'analysisfeature',
    'analysisfeatureprop',
    'analysisprop',
    'arraydesign',
    'arraydesignprop',
    'assay',
    'assay_biomaterial',
    'assay_project',
    'assayprop',
    'biomaterial',
    'biomaterial_dbxref',
    'biomaterial_relationship',
    'biomaterial_treatment',
    'biomaterialprop',
    'cell_line',
    'cell_line_cvterm',
    'cell_line_cvtermprop',
    'cell_line_dbxref',
    'cell_line_feature',
    'cell_line_library',
    'cell_line_pub',
    'cell_line_relationship',
    'cell_line_synonym',
    'cell_lineprop',
    'cell_lineprop_pub',
    'channel',
    'contact',
    'contact_relationship',
    'control',
    'cv',
    'cvterm',
    'cvterm_dbxref',
    'cvterm_relationship',
    'cvtermpath',
    'cvtermprop',
    'cvtermsynonym',
    'db',
    'dbxref',
    'dbxrefprop',
    'eimage',
    'element',
    'element_relationship',
    'elementresult',
    'elementresult_relationship',
    'environment',
    'environment_cvterm',
    'expression',
    'expression_cvterm',
    'expression_cvtermprop',
    'expression_image',
    'expression_pub',
    'expressionprop',
    'feature',
    'feature_cvterm',
    'feature_cvterm_dbxref',
    'feature_cvterm_pub',
    'feature_cvtermprop',
    'feature_dbxref',
    'feature_expression',
    'feature_expressionprop',
    'feature_genotype',
    'feature_phenotype',
    'feature_pub',
    'feature_pubprop',
    'feature_relationship',
    'feature_relationship_pub',
    'feature_relationshipprop',
    'feature_relationshipprop_pub',
    'feature_synonym',
    'featureloc',
    'featureloc_pub',
    'featuremap',
    'featuremap_pub',
    'featurepos',
    'featureprop',
    'featureprop_pub',
    'featurerange',
    'genotype',
    'library',
    'library_cvterm',
    'library_dbxref',
    'library_feature',
    'library_pub',
    'library_synonym',
    'libraryprop',
    'libraryprop_pub',
    'magedocumentation',
    'mageml',
    'materialized_view',
    'organism',
    'organism_dbxref',
    'organismprop',
    'phendesc',
    'phenotype',
    'phenotype_comparison',
    'phenotype_comparison_cvterm',
    'phenotype_cvterm',
    'phenstatement',
    'phylonode',
    'phylonode_dbxref',
    'phylonode_organism',
    'phylonode_pub',
    'phylonode_relationship',
    'phylonodeprop',
    'phylotree',
    'phylotree_pub',
    'project',
    'protocol',
    'protocolparam',
    'pub',
    'pub_dbxref',
    'pub_relationship',
    'pubauthor',
    'pubprop',
    'quantification',
    'quantification_relationship',
    'quantificationprop',
    'stock',
    'stock_cvterm',
    'stock_dbxref',
    'stock_genotype',
    'stock_pub',
    'stock_relationship',
    'stock_relationship_pub',
    'stockcollection',
    'stockcollection_stock',
    'stockcollectionprop',
    'stockprop',
    'stockprop_pub',
    'study',
    'study_assay',
    'studydesign',
    'studydesignprop',
    'studyfactor',
    'studyfactorvalue',
    'studyprop',
    'studyprop_feature',
    'synonym',
    'tableinfo',
    'treatment'
  );
  return $tables;
}
