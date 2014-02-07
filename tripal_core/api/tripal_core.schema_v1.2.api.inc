<?php
/**
 * @file
 * Describes the chado tables in version 1.2
 */

/**
 * @defgroup tripal_schema_v1_2_api Chado v1.2 Schema API
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
 * implement the hook_chado_schema_v1_2_[table name]() hook where [table name] is the
 * name of the chado table whose schema definition you want to augment.
 * @}
 */

/**
 * Implements hook_chado_schema_v1_2_acquisition()
 * Purpose: To describe the structure of 'acquisition' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'acquisition' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_acquisition() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'acquisition_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'channel_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'acquisitiondate' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => FALSE,
        'default' => 'ow(',
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'uri' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
      'channel' => array(
        'table' => 'channel',
        'columns' => array(
          'channel_id' => 'channel_id',
        ),
      ),
    ),
    'table' => 'acquisition',
    'referring_tables' => array(
      0 => 'acquisition_relationship',
      2 => 'acquisitionprop',
      3 => 'quantification',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_acquisition_relationship()
 * Purpose: To describe the structure of 'acquisition_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'acquisition_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_acquisition_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'acquisition_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'acquisition_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_acquisitionprop()
 * Purpose: To describe the structure of 'acquisitionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'acquisitionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_acquisitionprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'acquisitionprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'acquisition_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'acquisitionprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysis()
 * Purpose: To describe the structure of 'analysis' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysis' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysis() {
  $description =  array(
    'referring_tables' => array(
      0 => 'analysisfeature',
      1 => 'analysisprop',
      2 => 'phylotree',
      3 => 'quantification',
    ),
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysis_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'program' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'programversion' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'algorithm' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'sourcename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'sourceversion' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'sourceuri' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'timeexecuted' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
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
    'foreign keys' => array(
    ),
    'table' => 'analysis',
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysisfeature()
 * Purpose: To describe the structure of 'analysisfeature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisfeature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysisfeature() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysisfeature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'analysis_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rawscore' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => FALSE,
      ),
      'normscore' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => FALSE,
      ),
      'significance' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => FALSE,
      ),
      'identity' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => FALSE,
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
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
    ),
    'table' => 'analysisfeature',
    'referring_tables' => array(
      0 => 'analysisfeatureprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysisfeatureprop()
 * Purpose: To describe the structure of 'analysisfeatureprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisfeatureprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysisfeatureprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysisfeatureprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'analysisfeature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'analysisfeatureprop_id',
    ),
    'unique keys' => array(
      'analysisfeature_id_type_id_rank' => array(
        0 => 'analysisfeature_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'analysisfeature' => array(
        'table' => 'analysisfeature',
        'columns' => array(
          'analysisfeature_id' => 'analysisfeature_id',
        ),
      ),
    ),
    'table' => 'analysisfeatureprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysisprop()
 * Purpose: To describe the structure of 'analysisprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysisprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysisprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'analysis_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'analysisprop_id',
    ),
    'unique keys' => array(
      'analysisprop_c1' => array(
        0 => 'analysis_id',
        1 => 'type_id',
        2 => 'rank',
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
    'table' => 'analysisprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_arraydesign()
 * Purpose: To describe the structure of 'arraydesign' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'arraydesign' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_arraydesign() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'arraydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'manufacturer_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'platformtype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'substratetype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'version' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'array_dimensions' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'element_dimensions' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'num_of_elements' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'num_array_columns' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'num_array_rows' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'num_grid_columns' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'num_grid_rows' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'num_sub_columns' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'num_sub_rows' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'platformtype_id' => 'cvterm_id',
          'substratetype_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
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
    'table' => 'arraydesign',
    'referring_tables' => array(
      0 => 'arraydesignprop',
      1 => 'assay',
      2 => 'element',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_arraydesignprop()
 * Purpose: To describe the structure of 'arraydesignprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'arraydesignprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_arraydesignprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'arraydesignprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'arraydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'arraydesignprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assay()
 * Purpose: To describe the structure of 'assay' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assay' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assay() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'arraydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'assaydate' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => FALSE,
        'default' => 'ow(',
      ),
      'arrayidentifier' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'arraybatchidentifier' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'operator_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
      'arraydesign' => array(
        'table' => 'arraydesign',
        'columns' => array(
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
    'table' => 'assay',
    'referring_tables' => array(
      0 => 'acquisition',
      1 => 'assay_biomaterial',
      2 => 'assay_project',
      3 => 'assayprop',
      4 => 'control',
      5 => 'study_assay',
      6 => 'studyfactorvalue',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assay_biomaterial()
 * Purpose: To describe the structure of 'assay_biomaterial' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assay_biomaterial' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assay_biomaterial() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assay_biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'channel_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'channel' => array(
        'table' => 'channel',
        'columns' => array(
          'channel_id' => 'channel_id',
        ),
      ),
    ),
    'table' => 'assay_biomaterial',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assay_project()
 * Purpose: To describe the structure of 'assay_project' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assay_project' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assay_project() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assay_project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'assay_project',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assayprop()
 * Purpose: To describe the structure of 'assayprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'assayprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assayprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assayprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'assayprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial()
 * Purpose: To describe the structure of 'biomaterial' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'taxon_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'biosourceprovider_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'taxon_id' => 'organism_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'biosourceprovider_id' => 'contact_id',
        ),
      ),
    ),
    'table' => 'biomaterial',
    'referring_tables' => array(
      0 => 'assay_biomaterial',
      1 => 'biomaterial_dbxref',
      2 => 'biomaterial_relationship',
      4 => 'biomaterial_treatment',
      5 => 'biomaterialprop',
      6 => 'treatment',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial_dbxref()
 * Purpose: To describe the structure of 'biomaterial_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'biomaterial_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial_relationship()
 * Purpose: To describe the structure of 'biomaterial_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'biomaterial_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial_treatment()
 * Purpose: To describe the structure of 'biomaterial_treatment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterial_treatment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial_treatment() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_treatment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'treatment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'unittype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'biomaterial_treatment',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterialprop()
 * Purpose: To describe the structure of 'biomaterialprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'biomaterialprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterialprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterialprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'biomaterialprop',
    'referring_tables' => NULL,
  );
  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_cell_line()
 * Purpose: To describe the structure of 'cell_line' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'timeaccessioned' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
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
    'foreign keys' => array(
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
    'table' => 'cell_line',
    'referring_tables' => array(
      0 => 'cell_line_cvterm',
      1 => 'cell_line_dbxref',
      2 => 'cell_line_feature',
      3 => 'cell_line_library',
      4 => 'cell_line_pub',
      5 => 'cell_line_relationship',
      7 => 'cell_line_synonym',
      8 => 'cell_lineprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_cvterm()
 * Purpose: To describe the structure of 'cell_line_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'cell_line_cvterm',
    'referring_tables' => array(
      0 => 'cell_line_cvtermprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_cvtermprop()
 * Purpose: To describe the structure of 'cell_line_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_cvtermprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_cvtermprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'cell_line_cvtermprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_dbxref()
 * Purpose: To describe the structure of 'cell_line_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
    'table' => 'cell_line_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_feature()
 * Purpose: To describe the structure of 'cell_line_feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_feature() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'cell_line_feature',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_library()
 * Purpose: To describe the structure of 'cell_line_library' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_library' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_library() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'cell_line_library',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_pub()
 * Purpose: To describe the structure of 'cell_line_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'cell_line_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_relationship()
 * Purpose: To describe the structure of 'cell_line_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'cell_line_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_synonym()
 * Purpose: To describe the structure of 'cell_line_synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_line_synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_synonym() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'is_internal' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
    'table' => 'cell_line_synonym',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_lineprop()
 * Purpose: To describe the structure of 'cell_lineprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_lineprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_lineprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_lineprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_line_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'cell_lineprop',
    'referring_tables' => array(
      0 => 'cell_lineprop_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_lineprop_pub()
 * Purpose: To describe the structure of 'cell_lineprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cell_lineprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_lineprop_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_lineprop_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cell_lineprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'cell_lineprop_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_chadoprop()
 * Purpose: To describe the structure of 'chadoprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'chadoprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_chadoprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'chadoprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'chadoprop_id',
    ),
    'unique keys' => array(
      'chadoprop_c1' => array(
        0 => 'type_id',
        1 => 'rank',
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
    'table' => 'chadoprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_channel()
 * Purpose: To describe the structure of 'channel' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'channel' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_channel() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'channel_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'definition' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
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
    'foreign keys' => array(
    ),
    'table' => 'channel',
    'referring_tables' => array(
      0 => 'acquisition',
      1 => 'assay_biomaterial',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_contact()
 * Purpose: To describe the structure of 'contact' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'contact' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_contact() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
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
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'table' => 'contact',
    'referring_tables' => array(
      0 => 'arraydesign',
      1 => 'assay',
      2 => 'biomaterial',
      3 => 'contact_relationship',
      5 => 'nd_experiment_contact',
      6 => 'project_contact',
      14 => 'quantification',
      15 => 'stockcollection',
      16 => 'study',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_contact_relationship()
 * Purpose: To describe the structure of 'contact_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'contact_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_contact_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'contact_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'contact_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_control()
 * Purpose: To describe the structure of 'control' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'control' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_control() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'control_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'tableinfo_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'row_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'control_id',
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'tableinfo' => array(
        'table' => 'tableinfo',
        'columns' => array(
          'tableinfo_id' => 'tableinfo_id',
        ),
      ),
      'assay' => array(
        'table' => 'assay',
        'columns' => array(
          'assay_id' => 'assay_id',
        ),
      ),
    ),
    'table' => 'control',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cv()
 * Purpose: To describe the structure of 'cv' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cv' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cv() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'definition' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'foreign keys' => array(
    ),
    'table' => 'cv',
    'referring_tables' => array(
      0 => 'cvprop',
      1 => 'cvterm',
      2 => 'cvtermpath',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvprop()
 * Purpose: To describe the structure of 'cvprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cv_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'cvprop_id',
    ),
    'unique keys' => array(
      'cvprop_c1' => array(
        0 => 'cv_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'cv' => array(
        'table' => 'cv',
        'columns' => array(
          'cv_id' => 'cv_id',
        ),
      ),
    ),
    'table' => 'cvprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvterm()
 * Purpose: To describe the structure of 'cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cv_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ),
      'definition' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_obsolete' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'is_relationshiptype' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'cv' => array(
        'table' => 'cv',
        'columns' => array(
          'cv_id' => 'cv_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'table' => 'cvterm',
    'referring_tables' => array(
      0 => 'acquisition_relationship',
      1 => 'acquisitionprop',
      2 => 'analysisfeatureprop',
      3 => 'analysisprop',
      4 => 'arraydesign',
      6 => 'arraydesignprop',
      7 => 'assayprop',
      8 => 'biomaterial_relationship',
      9 => 'biomaterial_treatment',
      10 => 'biomaterialprop',
      11 => 'cell_line_cvterm',
      12 => 'cell_line_cvtermprop',
      13 => 'cell_line_relationship',
      14 => 'cell_lineprop',
      15 => 'chadoprop',
      23 => 'contact',
      24 => 'contact_relationship',
      25 => 'control',
      26 => 'cvprop',
      27 => 'cvterm_dbxref',
      28 => 'cvterm_relationship',
      31 => 'cvtermpath',
      34 => 'cvtermprop',
      36 => 'cvtermsynonym',
      38 => 'dbxrefprop',
      39 => 'element',
      40 => 'element_relationship',
      41 => 'elementresult_relationship',
      42 => 'environment_cvterm',
      43 => 'expression_cvterm',
      45 => 'expression_cvtermprop',
      46 => 'expressionprop',
      47 => 'feature',
      48 => 'feature_cvterm',
      49 => 'feature_cvtermprop',
      50 => 'feature_expressionprop',
      51 => 'feature_genotype',
      52 => 'feature_pubprop',
      53 => 'feature_relationship',
      54 => 'feature_relationshipprop',
      55 => 'featuremap',
      56 => 'featureprop',
      57 => 'genotype',
      58 => 'genotypeprop',
      66 => 'library',
      67 => 'library_cvterm',
      68 => 'libraryprop',
      69 => 'nd_experiment',
      70 => 'nd_experiment_stock',
      71 => 'nd_experiment_stockprop',
      72 => 'nd_experimentprop',
      73 => 'nd_geolocationprop',
      74 => 'nd_protocol',
      75 => 'nd_protocol_reagent',
      76 => 'nd_protocolprop',
      77 => 'nd_reagent',
      78 => 'nd_reagent_relationship',
      79 => 'nd_reagentprop',
      80 => 'organismprop',
      81 => 'phendesc',
      82 => 'phenotype',
      86 => 'phenotype_comparison_cvterm',
      87 => 'phenotype_cvterm',
      88 => 'phenstatement',
      89 => 'phylonode',
      90 => 'phylonode_relationship',
      91 => 'phylonodeprop',
      92 => 'phylotree',
      93 => 'project_relationship',
      101 => 'projectprop',
      109 => 'protocol',
      110 => 'protocolparam',
      112 => 'pub',
      113 => 'pub_relationship',
      114 => 'pubprop',
      115 => 'quantification_relationship',
      116 => 'quantificationprop',
      117 => 'stock',
      118 => 'stock_cvterm',
      119 => 'stock_cvtermprop',
      127 => 'stock_dbxrefprop',
      135 => 'stock_relationship',
      136 => 'stock_relationship_cvterm',
      144 => 'stockcollection',
      145 => 'stockcollectionprop',
      146 => 'stockprop',
      147 => 'studydesignprop',
      148 => 'studyfactor',
      149 => 'studyprop',
      150 => 'studyprop_feature',
      151 => 'synonym',
      152 => 'treatment',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvterm_dbxref()
 * Purpose: To describe the structure of 'cvterm_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvterm_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_for_definition' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'table' => 'cvterm_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvterm_relationship()
 * Purpose: To describe the structure of 'cvterm_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvterm_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvterm_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'cvterm_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvtermpath()
 * Purpose: To describe the structure of 'cvtermpath' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvtermpath' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvtermpath() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvtermpath_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cv_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pathdistance' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ),
      ),
      'cv' => array(
        'table' => 'cv',
        'columns' => array(
          'cv_id' => 'cv_id',
        ),
      ),
    ),
    'table' => 'cvtermpath',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvtermprop()
 * Purpose: To describe the structure of 'cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvtermprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvtermprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
        'default' => '',
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'cvtermprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvtermsynonym()
 * Purpose: To describe the structure of 'cvtermsynonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'cvtermsynonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvtermsynonym() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvtermsynonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'synonym' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
    'table' => 'cvtermsynonym',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_db()
 * Purpose: To describe the structure of 'db' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'db' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_db() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'db_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'urlprefix' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'url' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
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
    'foreign keys' => array(
    ),
    'table' => 'db',
    'referring_tables' => array(
      0 => 'dbxref',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_dbxref()
 * Purpose: To describe the structure of 'dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'db_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'accession' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'version' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
        'default' => '',
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'table' => 'dbxref',
    'referring_tables' => array(
      0 => 'arraydesign',
      1 => 'assay',
      2 => 'biomaterial',
      3 => 'biomaterial_dbxref',
      4 => 'cell_line_dbxref',
      5 => 'cvterm',
      6 => 'cvterm_dbxref',
      7 => 'dbxrefprop',
      8 => 'element',
      9 => 'feature',
      10 => 'feature_cvterm_dbxref',
      11 => 'feature_dbxref',
      12 => 'library_dbxref',
      13 => 'nd_experiment_dbxref',
      14 => 'nd_experiment_stock_dbxref',
      15 => 'organism_dbxref',
      16 => 'phylonode_dbxref',
      17 => 'phylotree',
      18 => 'protocol',
      19 => 'pub_dbxref',
      20 => 'stock',
      21 => 'stock_dbxref',
      22 => 'study',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_dbxrefprop()
 * Purpose: To describe the structure of 'dbxrefprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_dbxrefprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'dbxrefprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
        'default' => '',
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'table' => 'dbxrefprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_eimage()
 * Purpose: To describe the structure of 'eimage' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'eimage' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_eimage() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'eimage_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'eimage_data' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'eimage_type' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'image_uri' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'eimage_id',
    ),
    'foreign keys' => array(
    ),
    'table' => 'eimage',
    'referring_tables' => array(
      0 => 'expression_image',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_element()
 * Purpose: To describe the structure of 'element' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'element' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_element() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'element_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'arraydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
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
    'table' => 'element',
    'referring_tables' => array(
      0 => 'element_relationship',
      2 => 'elementresult',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_element_relationship()
 * Purpose: To describe the structure of 'element_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'element_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_element_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'element_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'element_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_elementresult()
 * Purpose: To describe the structure of 'elementresult' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'elementresult' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_elementresult() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'elementresult_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'element_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'quantification_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'signal' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => TRUE,
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
    'table' => 'elementresult',
    'referring_tables' => array(
      0 => 'elementresult_relationship',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_elementresult_relationship()
 * Purpose: To describe the structure of 'elementresult_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'elementresult_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_elementresult_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'elementresult_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'elementresult_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_environment()
 * Purpose: To describe the structure of 'environment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'environment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_environment() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'environment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'table' => 'environment',
    'referring_tables' => array(
      0 => 'environment_cvterm',
      1 => 'phendesc',
      2 => 'phenotype_comparison',
      4 => 'phenstatement',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_environment_cvterm()
 * Purpose: To describe the structure of 'environment_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'environment_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_environment_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'environment_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'environment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'environment_cvterm',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression()
 * Purpose: To describe the structure of 'expression' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'md5checksum' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => FALSE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'foreign keys' => array(
    ),
    'table' => 'expression',
    'referring_tables' => array(
      0 => 'expression_cvterm',
      1 => 'expression_image',
      2 => 'expression_pub',
      3 => 'expressionprop',
      4 => 'feature_expression',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_cvterm()
 * Purpose: To describe the structure of 'expression_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'cvterm_type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'expression_cvterm',
    'referring_tables' => array(
      0 => 'expression_cvtermprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_cvtermprop()
 * Purpose: To describe the structure of 'expression_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_cvtermprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_cvtermprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'expression_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'expression_cvtermprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_image()
 * Purpose: To describe the structure of 'expression_image' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_image' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_image() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_image_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'eimage_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'expression_image',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_pub()
 * Purpose: To describe the structure of 'expression_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expression_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'expression_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expressionprop()
 * Purpose: To describe the structure of 'expressionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'expressionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expressionprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expressionprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'expressionprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature()
 * Purpose: To describe the structure of 'feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'residues' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'seqlen' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'md5checksum' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => FALSE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_analysis' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'is_obsolete' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'timeaccessioned' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'table' => 'feature',
    'referring_tables' => array(
      0 => 'analysisfeature',
      1 => 'cell_line_feature',
      2 => 'element',
      3 => 'feature_cvterm',
      4 => 'feature_dbxref',
      5 => 'feature_expression',
      6 => 'feature_genotype',
      8 => 'feature_phenotype',
      9 => 'feature_pub',
      10 => 'feature_relationship',
      12 => 'feature_synonym',
      13 => 'featureloc',
      15 => 'featurepos',
      17 => 'featureprop',
      18 => 'featurerange',
      23 => 'library_feature',
      24 => 'phylonode',
      25 => 'studyprop_feature',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvterm()
 * Purpose: To describe the structure of 'feature_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_not' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
    ),
    'table' => 'feature_cvterm',
    'referring_tables' => array(
      0 => 'feature_cvterm_dbxref',
      1 => 'feature_cvterm_pub',
      2 => 'feature_cvtermprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvterm_dbxref()
 * Purpose: To describe the structure of 'feature_cvterm_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvterm_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvterm_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_cvterm_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvterm_pub()
 * Purpose: To describe the structure of 'feature_cvterm_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvterm_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvterm_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvterm_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_cvterm_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvtermprop()
 * Purpose: To describe the structure of 'feature_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvtermprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvtermprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_cvterm' => array(
        'table' => 'feature_cvterm',
        'columns' => array(
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
    ),
    'table' => 'feature_cvtermprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_dbxref()
 * Purpose: To describe the structure of 'feature_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
    ),
    'table' => 'feature_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_expression()
 * Purpose: To describe the structure of 'feature_expression' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_expression' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_expression() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_expression',
    'referring_tables' => array(
      0 => 'feature_expressionprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_expressionprop()
 * Purpose: To describe the structure of 'feature_expressionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_expressionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_expressionprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_expressionprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_expression_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'feature_expressionprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_genotype()
 * Purpose: To describe the structure of 'feature_genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_genotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'chromosome_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cgroup' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_genotype',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_phenotype()
 * Purpose: To describe the structure of 'feature_phenotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_phenotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_phenotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_phenotype',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_pub()
 * Purpose: To describe the structure of 'feature_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_pub',
    'referring_tables' => array(
      0 => 'feature_pubprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_pubprop()
 * Purpose: To describe the structure of 'feature_pubprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_pubprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_pubprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_pubprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'feature_pubprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationship()
 * Purpose: To describe the structure of 'feature_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'subject_id' => 'feature_id',
          'object_id' => 'feature_id',
        ),
      ),
    ),
    'table' => 'feature_relationship',
    'referring_tables' => array(
      0 => 'feature_relationship_pub',
      1 => 'feature_relationshipprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationship_pub()
 * Purpose: To describe the structure of 'feature_relationship_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationship_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationship_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'feature_relationship' => array(
        'table' => 'feature_relationship',
        'columns' => array(
          'feature_relationship_id' => 'feature_relationship_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
    'table' => 'feature_relationship_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationshipprop()
 * Purpose: To describe the structure of 'feature_relationshipprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationshipprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationshipprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationshipprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'feature_relationshipprop',
    'referring_tables' => array(
      0 => 'feature_relationshipprop_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationshipprop_pub()
 * Purpose: To describe the structure of 'feature_relationshipprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_relationshipprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationshipprop_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationshipprop_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_relationshipprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'feature_relationshipprop_pub',
    'referring_tables' => NULL,
  );
  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_feature_synonym()
 * Purpose: To describe the structure of 'feature_synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'feature_synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_synonym() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'is_internal' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
      'synonym' => array(
        'table' => 'synonym',
        'columns' => array(
          'synonym_id' => 'synonym_id',
        ),
      ),
    ),
    'table' => 'feature_synonym',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureloc()
 * Purpose: To describe the structure of 'featureloc' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureloc' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureloc() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureloc_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'srcfeature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'fmin' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'is_fmin_partial' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'fmax' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'is_fmax_partial' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'strand' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => FALSE,
      ),
      'phase' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'residue_info' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'locgroup' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      0 => 'featureloc_pub',
    ),
    'table' => 'featureloc',
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureloc_pub()
 * Purpose: To describe the structure of 'featureloc_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureloc_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureloc_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureloc_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'featureloc_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'featureloc' => array(
        'table' => 'featureloc',
        'columns' => array(
          'featureloc_id' => 'featureloc_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
    'table' => 'featureloc_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featuremap()
 * Purpose: To describe the structure of 'featuremap' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featuremap' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featuremap() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featuremap_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'unittype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'unittype_id' => 'cvterm_id',
        ),
      ),
    ),
    'table' => 'featuremap',
    'referring_tables' => array(
      0 => 'featuremap_pub',
      1 => 'featurepos',
      2 => 'featurerange',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featuremap_pub()
 * Purpose: To describe the structure of 'featuremap_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featuremap_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featuremap_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featuremap_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'featuremap_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'featuremap_pub_id',
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
    'table' => 'featuremap_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featurepos()
 * Purpose: To describe the structure of 'featurepos' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featurepos' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featurepos() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featurepos_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'featuremap_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'map_feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'mappos' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'featurepos_id',
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
    'table' => 'featurepos',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureprop()
 * Purpose: To describe the structure of 'featureprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    ),
    'table' => 'featureprop',
    'referring_tables' => array(
      0 => 'featureprop_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureprop_pub()
 * Purpose: To describe the structure of 'featureprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featureprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureprop_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureprop_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'featureprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'featureprop' => array(
        'table' => 'featureprop',
        'columns' => array(
          'featureprop_id' => 'featureprop_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
    'table' => 'featureprop_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featurerange()
 * Purpose: To describe the structure of 'featurerange' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'featurerange' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featurerange() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featurerange_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'featuremap_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'leftstartf_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'leftendf_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'rightstartf_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'rightendf_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rangestr' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'featurerange_id',
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
    'table' => 'featurerange',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_genotype()
 * Purpose: To describe the structure of 'genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_genotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'table' => 'genotype',
    'referring_tables' => array(
      0 => 'feature_genotype',
      1 => 'genotypeprop',
      9 => 'nd_experiment_genotype',
      10 => 'phendesc',
      11 => 'phenotype_comparison',
      13 => 'phenstatement',
      14 => 'stock_genotype',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_genotypeprop()
 * Purpose: To describe the structure of 'genotypeprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'genotypeprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_genotypeprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'genotypeprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'genotypeprop_id',
    ),
    'unique keys' => array(
      'genotypeprop_c1' => array(
        0 => 'genotype_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'genotypeprop_idx1' => array(
        0 => 'genotype_id',
      ),
      'genotypeprop_idx2' => array(
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
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
    ),
    'table' => 'genotypeprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library()
 * Purpose: To describe the structure of 'library' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_obsolete' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'timeaccessioned' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => TRUE,
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
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
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
      0 => 'cell_line_library',
      1 => 'library_cvterm',
      2 => 'library_dbxref',
      3 => 'library_feature',
      4 => 'library_pub',
      5 => 'library_synonym',
      6 => 'libraryprop',
    ),
    'table' => 'library',
  );
  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_library_cvterm()
 * Purpose: To describe the structure of 'library_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'library_cvterm',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_dbxref()
 * Purpose: To describe the structure of 'library_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
    'table' => 'library_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_feature()
 * Purpose: To describe the structure of 'library_feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_feature() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'library_feature',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_pub()
 * Purpose: To describe the structure of 'library_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'library_pub',
    'referring_tables' => NULL,
  );
  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_library_synonym()
 * Purpose: To describe the structure of 'library_synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'library_synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_synonym() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'ru',
      ),
      'is_internal' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
    'table' => 'library_synonym',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_libraryprop()
 * Purpose: To describe the structure of 'libraryprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'libraryprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_libraryprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'libraryprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'library_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'libraryprop',
    'referring_tables' => array(
      0 => 'libraryprop_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_libraryprop_pub()
 * Purpose: To describe the structure of 'libraryprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'libraryprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_libraryprop_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'libraryprop_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'libraryprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'libraryprop_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_magedocumentation()
 * Purpose: To describe the structure of 'magedocumentation' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'magedocumentation' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_magedocumentation() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'magedocumentation_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'mageml_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'tableinfo_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'row_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'mageidentifier' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'magedocumentation_id',
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
    'table' => 'magedocumentation',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_mageml()
 * Purpose: To describe the structure of 'mageml' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'mageml' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_mageml() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'mageml_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'mage_package' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'mage_ml' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'mageml_id',
    ),
    'foreign keys' => array(
    ),
    'table' => 'mageml',
    'referring_tables' => array(
      0 => 'magedocumentation',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment()
 * Purpose: To describe the structure of 'nd_experiment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_geolocation_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_id',
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_geolocation' => array(
        'table' => 'nd_geolocation',
        'columns' => array(
          'nd_geolocation_id' => 'nd_geolocation_id',
        ),
      ),
    ),
    'table' => 'nd_experiment',
    'referring_tables' => array(
      0 => 'nd_experiment_contact',
      1 => 'nd_experiment_dbxref',
      2 => 'nd_experiment_genotype',
      3 => 'nd_experiment_phenotype',
      4 => 'nd_experiment_project',
      5 => 'nd_experiment_protocol',
      6 => 'nd_experiment_pub',
      7 => 'nd_experiment_stock',
      8 => 'nd_experimentprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_contact()
 * Purpose: To describe the structure of 'nd_experiment_contact' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_contact' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_contact() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_contact_id',
    ),
    'foreign keys' => array(
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'contact_id' => 'contact_id',
        ),
      ),
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_contact',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_dbxref()
 * Purpose: To describe the structure of 'nd_experiment_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_dbxref_id',
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_genotype()
 * Purpose: To describe the structure of 'nd_experiment_genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_genotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_genotype_id',
    ),
    'unique keys' => array(
      'nd_experiment_genotype_c1' => array(
        0 => 'nd_experiment_id',
        1 => 'genotype_id',
      ),
    ),
    'foreign keys' => array(
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_genotype',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_phenotype()
 * Purpose: To describe the structure of 'nd_experiment_phenotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_phenotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_phenotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_phenotype_id',
    ),
    'unique keys' => array(
      'nd_experiment_phenotype_c1' => array(
        0 => 'nd_experiment_id',
        1 => 'phenotype_id',
      ),
    ),
    'foreign keys' => array(
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype_id' => 'phenotype_id',
        ),
      ),
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_phenotype',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_project()
 * Purpose: To describe the structure of 'nd_experiment_project' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_project' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_project() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_project_id',
    ),
    'foreign keys' => array(
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'project_id' => 'project_id',
        ),
      ),
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_project',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_protocol()
 * Purpose: To describe the structure of 'nd_experiment_protocol' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_protocol' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_protocol() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'nd_protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_protocol_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'nd_protocol' => array(
        'table' => 'nd_protocol',
        'columns' => array(
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_protocol',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_pub()
 * Purpose: To describe the structure of 'nd_experiment_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_pub_id',
    ),
    'unique keys' => array(
      'nd_experiment_pub_c1' => array(
        0 => 'nd_experiment_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'nd_experiment_pub_idx1' => array(
        0 => 'nd_experiment_id',
      ),
      'nd_experiment_pub_idx2' => array(
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
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_stock()
 * Purpose: To describe the structure of 'nd_experiment_stock' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_stock' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_stock() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_stock_id',
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
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_stock',
    'referring_tables' => array(
      0 => 'nd_experiment_stock_dbxref',
      1 => 'nd_experiment_stockprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_stock_dbxref()
 * Purpose: To describe the structure of 'nd_experiment_stock_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_stock_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_stock_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_stock_dbxref_id',
    ),
    'foreign keys' => array(
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'nd_experiment_stock' => array(
        'table' => 'nd_experiment_stock',
        'columns' => array(
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_stock_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_stockprop()
 * Purpose: To describe the structure of 'nd_experiment_stockprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experiment_stockprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_stockprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experiment_stockprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experiment_stockprop_id',
    ),
    'unique keys' => array(
      'nd_experiment_stockprop_c1' => array(
        0 => 'nd_experiment_stock_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_experiment_stock' => array(
        'table' => 'nd_experiment_stock',
        'columns' => array(
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ),
      ),
    ),
    'table' => 'nd_experiment_stockprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experimentprop()
 * Purpose: To describe the structure of 'nd_experimentprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_experimentprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experimentprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_experimentprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_experiment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'nd_experimentprop_id',
    ),
    'unique keys' => array(
      'nd_experimentprop_c1' => array(
        0 => 'nd_experiment_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
    'table' => 'nd_experimentprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_geolocation()
 * Purpose: To describe the structure of 'nd_geolocation' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_geolocation' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_geolocation() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_geolocation_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'latitude' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => FALSE,
      ),
      'longitude' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => FALSE,
      ),
      'geodetic_datum' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '32',
        'not null' => FALSE,
      ),
      'altitude' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_geolocation_id',
    ),
    'foreign keys' => array(
    ),
    'table' => 'nd_geolocation',
    'referring_tables' => array(
      0 => 'nd_experiment',
      1 => 'nd_geolocationprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_geolocationprop()
 * Purpose: To describe the structure of 'nd_geolocationprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_geolocationprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_geolocationprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_geolocationprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_geolocation_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'nd_geolocationprop_id',
    ),
    'unique keys' => array(
      'nd_geolocationprop_c1' => array(
        0 => 'nd_geolocation_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_geolocation' => array(
        'table' => 'nd_geolocation',
        'columns' => array(
          'nd_geolocation_id' => 'nd_geolocation_id',
        ),
      ),
    ),
    'table' => 'nd_geolocationprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_protocol()
 * Purpose: To describe the structure of 'nd_protocol' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_protocol' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_protocol() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_protocol_id',
    ),
    'unique keys' => array(
      'name' => array(
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
    ),
    'table' => 'nd_protocol',
    'referring_tables' => array(
      0 => 'nd_experiment_protocol',
      1 => 'nd_protocol_reagent',
      2 => 'nd_protocolprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_protocol_reagent()
 * Purpose: To describe the structure of 'nd_protocol_reagent' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_protocol_reagent' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_protocol_reagent() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_protocol_reagent_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'reagent_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_protocol_reagent_id',
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_protocol' => array(
        'table' => 'nd_protocol',
        'columns' => array(
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
      'nd_reagent' => array(
        'table' => 'nd_reagent',
        'columns' => array(
          'reagent_id' => 'nd_reagent_id',
        ),
      ),
    ),
    'table' => 'nd_protocol_reagent',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_protocolprop()
 * Purpose: To describe the structure of 'nd_protocolprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_protocolprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_protocolprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_protocolprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'nd_protocolprop_id',
    ),
    'unique keys' => array(
      'nd_protocolprop_c1' => array(
        0 => 'nd_protocol_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_protocol' => array(
        'table' => 'nd_protocol',
        'columns' => array(
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
    ),
    'table' => 'nd_protocolprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_reagent()
 * Purpose: To describe the structure of 'nd_reagent' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_reagent' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_reagent() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_reagent_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '80',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_reagent_id',
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'table' => 'nd_reagent',
    'referring_tables' => array(
      0 => 'nd_protocol_reagent',
      1 => 'nd_reagent_relationship',
      3 => 'nd_reagentprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_reagent_relationship()
 * Purpose: To describe the structure of 'nd_reagent_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_reagent_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_reagent_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_reagent_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_reagent_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_reagent_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'nd_reagent_relationship_id',
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_reagent' => array(
        'table' => 'nd_reagent',
        'columns' => array(
          'subject_reagent_id' => 'nd_reagent_id',
          'object_reagent_id' => 'nd_reagent_id',
        ),
      ),
    ),
    'table' => 'nd_reagent_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_reagentprop()
 * Purpose: To describe the structure of 'nd_reagentprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'nd_reagentprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_reagentprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nd_reagentprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'nd_reagent_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'nd_reagentprop_id',
    ),
    'unique keys' => array(
      'nd_reagentprop_c1' => array(
        0 => 'nd_reagent_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_reagent' => array(
        'table' => 'nd_reagent',
        'columns' => array(
          'nd_reagent_id' => 'nd_reagent_id',
        ),
      ),
    ),
    'table' => 'nd_reagentprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organism()
 * Purpose: To describe the structure of 'organism' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'organism' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organism() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'abbreviation' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'genus' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'species' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'common_name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'comment' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'foreign keys' => array(
    ),
    'referring_tables' => array(
      0 => 'biomaterial',
      1 => 'cell_line',
      2 => 'feature',
      3 => 'library',
      4 => 'organism_dbxref',
      5 => 'organismprop',
      6 => 'phenotype_comparison',
      7 => 'phylonode_organism',
      8 => 'stock',
    ),
    'table' => 'organism',
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organism_dbxref()
 * Purpose: To describe the structure of 'organism_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'organism_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organism_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'organism_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    ),
    'table' => 'organism_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organismprop()
 * Purpose: To describe the structure of 'organismprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'organismprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organismprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'organismprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'table' => 'organismprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phendesc()
 * Purpose: To describe the structure of 'phendesc' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phendesc' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phendesc() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phendesc_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'environment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'phendesc',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype()
 * Purpose: To describe the structure of 'phenotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'observable_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'attr_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'cvalue_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
    'table' => 'phenotype',
    'referring_tables' => array(
      0 => 'feature_phenotype',
      1 => 'nd_experiment_phenotype',
      2 => 'phenotype_comparison',
      4 => 'phenotype_cvterm',
      5 => 'phenstatement',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype_comparison()
 * Purpose: To describe the structure of 'phenotype_comparison' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype_comparison' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype_comparison() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_comparison_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'genotype1_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'environment1_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genotype2_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'environment2_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phenotype1_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phenotype2_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
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
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype1_id' => 'phenotype_id',
          'phenotype2_id' => 'phenotype_id',
        ),
      ),
    ),
    'table' => 'phenotype_comparison',
    'referring_tables' => array(
      0 => 'phenotype_comparison_cvterm',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype_comparison_cvterm()
 * Purpose: To describe the structure of 'phenotype_comparison_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype_comparison_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype_comparison_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_comparison_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phenotype_comparison_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'phenotype_comparison_cvterm',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype_cvterm()
 * Purpose: To describe the structure of 'phenotype_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenotype_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'phenotype_cvterm',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenstatement()
 * Purpose: To describe the structure of 'phenstatement' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phenstatement' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenstatement() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenstatement_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'environment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phenotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
    'table' => 'phenstatement',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode()
 * Purpose: To describe the structure of 'phylonode' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phylotree_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'parent_phylonode_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'left_idx' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'right_idx' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'label' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'distance' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => FALSE,
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
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'parent_phylonode_id' => 'phylonode_id',
        ),
      ),
      'phylotree' => array(
        'table' => 'phylotree',
        'columns' => array(
          'phylotree_id' => 'phylotree_id',
        ),
      ),
    ),
    'table' => 'phylonode',
    'referring_tables' => array(
      0 => 'phylonode',
      1 => 'phylonode_dbxref',
      2 => 'phylonode_organism',
      3 => 'phylonode_pub',
      4 => 'phylonode_relationship',
      6 => 'phylonodeprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_dbxref()
 * Purpose: To describe the structure of 'phylonode_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phylonode_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'phylonode_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_organism()
 * Purpose: To describe the structure of 'phylonode_organism' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_organism' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_organism() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phylonode_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'phylonode_organism',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_pub()
 * Purpose: To describe the structure of 'phylonode_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phylonode_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'phylonode_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_relationship()
 * Purpose: To describe the structure of 'phylonode_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonode_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'phylotree_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
      'phylonode' => array(
        'table' => 'phylonode',
        'columns' => array(
          'subject_id' => 'phylonode_id',
          'object_id' => 'phylonode_id',
        ),
      ),
      'phylotree' => array(
        'table' => 'phylotree',
        'columns' => array(
          'phylotree_id' => 'phylotree_id',
        ),
      ),
    ),
    'table' => 'phylonode_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonodeprop()
 * Purpose: To describe the structure of 'phylonodeprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylonodeprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonodeprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonodeprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phylonode_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
        'default' => '',
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'phylonodeprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylotree()
 * Purpose: To describe the structure of 'phylotree' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylotree' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylotree() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylotree_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'analysis_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'comment' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'phylotree_id',
    ),
    'indexes' => array(
      'phylotree_idx1' => array(
        0 => 'phylotree_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
    ),
    'table' => 'phylotree',
    'referring_tables' => array(
      0 => 'phylonode',
      1 => 'phylonode_relationship',
      2 => 'phylotree_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylotree_pub()
 * Purpose: To describe the structure of 'phylotree_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'phylotree_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylotree_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylotree_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'phylotree_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'phylotree_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project()
 * Purpose: To describe the structure of 'project' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'project' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
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
    'foreign keys' => array(
    ),
    'table' => 'project',
    'referring_tables' => array(
      0 => 'assay_project',
      1 => 'nd_experiment_project',
      2 => 'project_contact',
      10 => 'project_pub',
      18 => 'project_relationship',
      34 => 'projectprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project_contact()
 * Purpose: To describe the structure of 'project_contact' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'project_contact' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project_contact() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'project_contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'project_contact_id',
    ),
    'unique keys' => array(
      'project_contact_c1' => array(
        0 => 'project_id',
        1 => 'contact_id',
      ),
    ),
    'indexes' => array(
      'project_contact_idx1' => array(
        0 => 'project_id',
      ),
      'project_contact_idx2' => array(
        0 => 'contact_id',
      ),
    ),
    'foreign keys' => array(
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'contact_id' => 'contact_id',
        ),
      ),
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'project_id' => 'project_id',
        ),
      ),
    ),
    'table' => 'project_contact',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project_pub()
 * Purpose: To describe the structure of 'project_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'project_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'project_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'project_pub_id',
    ),
    'unique keys' => array(
      'project_pub_c1' => array(
        0 => 'project_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array(
      'project_pub_idx1' => array(
        0 => 'project_id',
      ),
      'project_pub_idx2' => array(
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
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'project_id' => 'project_id',
        ),
      ),
    ),
    'table' => 'project_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project_relationship()
 * Purpose: To describe the structure of 'project_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'project_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'project_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'primary key' => array(
      0 => 'project_relationship_id',
    ),
    'unique keys' => array(
      'project_relationship_c1' => array(
        0 => 'subject_project_id',
        1 => 'object_project_id',
        2 => 'type_id',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'subject_project_id' => 'project_id',
          'object_project_id' => 'project_id',
        ),
      ),
    ),
    'table' => 'project_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_projectprop()
 * Purpose: To describe the structure of 'projectprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'projectprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_projectprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'projectprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'project_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'projectprop_id',
    ),
    'unique keys' => array(
      'projectprop_c1' => array(
        0 => 'project_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'project_id' => 'project_id',
        ),
      ),
    ),
    'table' => 'projectprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_protocol()
 * Purpose: To describe the structure of 'protocol' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'protocol' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_protocol() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'uri' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'protocoldescription' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'hardwaredescription' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'softwaredescription' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
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
    'table' => 'protocol',
    'referring_tables' => array(
      0 => 'acquisition',
      1 => 'arraydesign',
      2 => 'assay',
      3 => 'protocolparam',
      4 => 'quantification',
      5 => 'treatment',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_protocolparam()
 * Purpose: To describe the structure of 'protocolparam' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'protocolparam' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_protocolparam() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'protocolparam_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'datatype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'unittype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'protocolparam_id',
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
    'table' => 'protocolparam',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pub()
 * Purpose: To describe the structure of 'pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'title' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'volumetitle' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'volume' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'series_name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'issue' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'pyear' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'pages' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'miniref' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_obsolete' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => FALSE,
        'default' => 'als',
      ),
      'publisher' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'pubplace' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
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
    'table' => 'pub',
    'referring_tables' => array(
      0 => 'cell_line_cvterm',
      1 => 'cell_line_feature',
      2 => 'cell_line_library',
      3 => 'cell_line_pub',
      4 => 'cell_line_synonym',
      5 => 'cell_lineprop_pub',
      6 => 'expression_pub',
      7 => 'feature_cvterm',
      8 => 'feature_cvterm_pub',
      9 => 'feature_expression',
      10 => 'feature_pub',
      11 => 'feature_relationship_pub',
      12 => 'feature_relationshipprop_pub',
      13 => 'feature_synonym',
      14 => 'featureloc_pub',
      15 => 'featuremap_pub',
      16 => 'featureprop_pub',
      17 => 'library_cvterm',
      18 => 'library_pub',
      19 => 'library_synonym',
      20 => 'libraryprop_pub',
      21 => 'nd_experiment_pub',
      22 => 'phendesc',
      23 => 'phenotype_comparison',
      24 => 'phenotype_comparison_cvterm',
      25 => 'phenstatement',
      26 => 'phylonode_pub',
      27 => 'phylotree_pub',
      28 => 'project_pub',
      36 => 'protocol',
      37 => 'pub_dbxref',
      38 => 'pub_relationship',
      40 => 'pubauthor',
      41 => 'pubprop',
      42 => 'stock_cvterm',
      43 => 'stock_pub',
      44 => 'stock_relationship_cvterm',
      52 => 'stock_relationship_pub',
      53 => 'stockprop_pub',
      54 => 'study',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pub_dbxref()
 * Purpose: To describe the structure of 'pub_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pub_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pub_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pub_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
    'table' => 'pub_dbxref',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pub_relationship()
 * Purpose: To describe the structure of 'pub_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pub_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pub_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pub_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'pub_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pubauthor()
 * Purpose: To describe the structure of 'pubauthor' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pubauthor' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pubauthor() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pubauthor_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'editor' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => FALSE,
        'default' => 'als',
      ),
      'surname' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => TRUE,
      ),
      'givennames' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => FALSE,
      ),
      'suffix' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => FALSE,
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
    'table' => 'pubauthor',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pubprop()
 * Purpose: To describe the structure of 'pubprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'pubprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pubprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pubprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
    'table' => 'pubprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_quantification()
 * Purpose: To describe the structure of 'quantification' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'quantification' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_quantification() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'quantification_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'acquisition_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'operator_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'analysis_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'quantificationdate' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => FALSE,
        'default' => 'ow(',
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'uri' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'table' => 'quantification',
    'referring_tables' => array(
      0 => 'elementresult',
      1 => 'quantification_relationship',
      3 => 'quantificationprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_quantification_relationship()
 * Purpose: To describe the structure of 'quantification_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'quantification_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_quantification_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'quantification_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'quantification_relationship',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_quantificationprop()
 * Purpose: To describe the structure of 'quantificationprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'quantificationprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_quantificationprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'quantificationprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'quantification_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'quantificationprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock()
 * Purpose: To describe the structure of 'stock' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock() {
  $description =  array(
    'description' => 'Any stock can be globally identified by the combination of organism, uniquename and stock type. A stock is the physical entities, either living or preserved, held by collections. Stocks belong to a collection; they have IDs, type, organism, description and may have a genotype.',
    'fields' => array(
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'organism_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_obsolete' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'referring_tables' => array(
      0 => 'nd_experiment_stock',
      1 => 'stock_cvterm',
      2 => 'stock_dbxref',
      3 => 'stock_genotype',
      4 => 'stock_pub',
      5 => 'stock_relationship',
      7 => 'stockcollection_stock',
      8 => 'stockprop',
    ),
    'table' => 'stock',
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_cvterm()
 * Purpose: To describe the structure of 'stock_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_not' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'als',
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
        3 => 'rank',
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
    'table' => 'stock_cvterm',
    'referring_tables' => array(
      0 => 'stock_cvtermprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_cvtermprop()
 * Purpose: To describe the structure of 'stock_cvtermprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_cvtermprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_cvtermprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'stock_cvtermprop_id',
    ),
    'unique keys' => array(
      'stock_cvtermprop_c1' => array(
        0 => 'stock_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'stock_cvtermprop_idx1' => array(
        0 => 'stock_cvterm_id',
      ),
      'stock_cvtermprop_idx2' => array(
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
      'stock_cvterm' => array(
        'table' => 'stock_cvterm',
        'columns' => array(
          'stock_cvterm_id' => 'stock_cvterm_id',
        ),
      ),
    ),
    'table' => 'stock_cvtermprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_dbxref()
 * Purpose: To describe the structure of 'stock_dbxref' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_dbxref() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'is_current' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => TRUE,
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
    'table' => 'stock_dbxref',
    'referring_tables' => array(
      0 => 'stock_dbxrefprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_dbxrefprop()
 * Purpose: To describe the structure of 'stock_dbxrefprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_dbxrefprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_dbxrefprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'stock_dbxrefprop_id',
    ),
    'unique keys' => array(
      'stock_dbxrefprop_c1' => array(
        0 => 'stock_dbxref_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
      'stock_dbxrefprop_idx1' => array(
        0 => 'stock_dbxref_id',
      ),
      'stock_dbxrefprop_idx2' => array(
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
      'stock_dbxref' => array(
        'table' => 'stock_dbxref',
        'columns' => array(
          'stock_dbxref_id' => 'stock_dbxref_id',
        ),
      ),
    ),
    'table' => 'stock_dbxrefprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_genotype()
 * Purpose: To describe the structure of 'stock_genotype' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_genotype() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genotype_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'stock_genotype',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_pub()
 * Purpose: To describe the structure of 'stock_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'stock_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_relationship()
 * Purpose: To describe the structure of 'stock_relationship' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_relationship() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'subject_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'stock_relationship',
    'referring_tables' => array(
      0 => 'stock_relationship_cvterm',
      8 => 'stock_relationship_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_relationship_cvterm()
 * Purpose: To describe the structure of 'stock_relationship_cvterm' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_relationship_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_relationship_cvterm() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_relationship_cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'stock_relationship_cvterm_id',
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
      'stock_relationship' => array(
        'table' => 'stock_relationship',
        'columns' => array(
          'stock_relationship_id' => 'stock_relationship_id',
        ),
      ),
    ),
    'table' => 'stock_relationship_cvterm',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_relationship_pub()
 * Purpose: To describe the structure of 'stock_relationship_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stock_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_relationship_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_relationship_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_relationship_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'stock_relationship_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockcollection()
 * Purpose: To describe the structure of 'stockcollection' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockcollection' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockcollection() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockcollection_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'uniquename' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
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
    'table' => 'stockcollection',
    'referring_tables' => array(
      0 => 'stockcollection_stock',
      1 => 'stockcollectionprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockcollection_stock()
 * Purpose: To describe the structure of 'stockcollection_stock' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockcollection_stock' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockcollection_stock() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockcollection_stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stockcollection_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'stockcollection_stock',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockcollectionprop()
 * Purpose: To describe the structure of 'stockcollectionprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockcollectionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockcollectionprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockcollectionprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stockcollection_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'stockcollectionprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockprop()
 * Purpose: To describe the structure of 'stockprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stock_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'stockprop',
    'referring_tables' => array(
      0 => 'stockprop_pub',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockprop_pub()
 * Purpose: To describe the structure of 'stockprop_pub' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'stockprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockprop_pub() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockprop_pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'stockprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'stockprop_pub',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_study()
 * Purpose: To describe the structure of 'study' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'study' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_study() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'study_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'contact_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'pub_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'dbxref_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
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
    'table' => 'study',
    'referring_tables' => array(
      0 => 'study_assay',
      1 => 'studydesign',
      2 => 'studyprop',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_study_assay()
 * Purpose: To describe the structure of 'study_assay' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'study_assay' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_study_assay() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'study_assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'study_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
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
    'table' => 'study_assay',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studydesign()
 * Purpose: To describe the structure of 'studydesign' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studydesign' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studydesign() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'study_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'studydesign_id',
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
    'table' => 'studydesign',
    'referring_tables' => array(
      0 => 'studydesignprop',
      1 => 'studyfactor',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studydesignprop()
 * Purpose: To describe the structure of 'studydesignprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studydesignprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studydesignprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studydesignprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'studydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'studydesignprop',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyfactor()
 * Purpose: To describe the structure of 'studyfactor' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyfactor' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyfactor() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyfactor_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'studydesign_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => TRUE,
      ),
      'description' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'studyfactor_id',
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
    'table' => 'studyfactor',
    'referring_tables' => array(
      0 => 'studyfactorvalue',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyfactorvalue()
 * Purpose: To describe the structure of 'studyfactorvalue' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyfactorvalue' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyfactorvalue() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyfactorvalue_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'studyfactor_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'assay_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'factorvalue' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array(
      0 => 'studyfactorvalue_id',
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
    'table' => 'studyfactorvalue',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyprop()
 * Purpose: To describe the structure of 'studyprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyprop() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'study_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'value' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
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
    'table' => 'studyprop',
    'referring_tables' => array(
      0 => 'studyprop_feature',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyprop_feature()
 * Purpose: To describe the structure of 'studyprop_feature' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'studyprop_feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyprop_feature() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyprop_feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'studyprop_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
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
    'table' => 'studyprop_feature',
    'referring_tables' => NULL,
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_synonym()
 * Purpose: To describe the structure of 'synonym' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_synonym() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'synonym_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'synonym_sgml' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
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
    'table' => 'synonym',
    'referring_tables' => array(
      0 => 'cell_line_synonym',
      1 => 'feature_synonym',
      2 => 'library_synonym',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_tableinfo()
 * Purpose: To describe the structure of 'tableinfo' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'tableinfo' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_tableinfo() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'tableinfo_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '30',
        'not null' => TRUE,
      ),
      'primary_key_column' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '30',
        'not null' => FALSE,
      ),
      'is_view' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'view_on_table_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'superclass_table_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'is_updateable' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 1,
      ),
      'modification_date' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'date',
        'not null' => TRUE,
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
    'foreign keys' => array(
    ),
    'table' => 'tableinfo',
    'referring_tables' => array(
      0 => 'control',
      1 => 'magedocumentation',
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_treatment()
 * Purpose: To describe the structure of 'treatment' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'treatment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_treatment() {
  $description =  array(
    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'treatment_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'rank' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'biomaterial_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'type_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'protocol_id' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => FALSE,
      ),
      'name' => array(
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array(
      0 => 'treatment_id',
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
    'table' => 'treatment',
    'referring_tables' => array(
      0 => 'biomaterial_treatment',
    ),
  );
  return $description;
}

/**
 * Lists the table names in the v1.2 chado schema
 *
 * @return
 *    An array containing all of the table names
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_get_v1_2_tables() {
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
    'chadoprop',
    'channel',
    'contact',
    'contact_relationship',
    'control',
    'cv',
    'cvprop',
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
    'genotypeprop',
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
    'nd_experiment',
    'nd_experiment_contact',
    'nd_experiment_dbxref',
    'nd_experiment_genotype',
    'nd_experiment_phenotype',
    'nd_experiment_project',
    'nd_experiment_protocol',
    'nd_experiment_pub',
    'nd_experiment_stock',
    'nd_experiment_stock_dbxref',
    'nd_experiment_stockprop',
    'nd_experimentprop',
    'nd_geolocation',
    'nd_geolocationprop',
    'nd_protocol',
    'nd_protocol_reagent',
    'nd_protocolprop',
    'nd_reagent',
    'nd_reagent_relationship',
    'nd_reagentprop',
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
    'project_contact',
    'project_pub',
    'project_relationship',
    'projectprop',
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
    'stock_cvtermprop',
    'stock_dbxref',
    'stock_dbxrefprop',
    'stock_genotype',
    'stock_pub',
    'stock_relationship',
    'stock_relationship_cvterm',
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