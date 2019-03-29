<?php
/**
 * @file
 * Describes the chado tables in version 1.3
 */

/**
 * @defgroup tripal_schema_v1_3_api Chado v1.3 Schema
 * @ingroup tripal_chado_schema_api
 * @{
 * Provides an application programming interface (API) for describing Chado
 * tables. This API consists of a set of functions, one for each table in Chado.
 * Each function simply returns a Drupal style array that defines the table.
 *
 * Because Drupal does not handle foreign key (FK) relationships, which are
 * needed to for Tripal Views, they have been added to the schema defintitions
 * below.
 *
 * The functions provided in this documentation should not be called as is,
 * but if you need the Drupal-style array definition for any table, use the
 * following function call:
 *
 *   $table_desc = chado_get_schema($table)
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The chado_get_schema function determines the appropriate version
 * of Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
 *
 * If you need to augment these schema definitions within your own module,
 * you need to implement the hook_chado_schema_v1_3_[table name]() hook where
 * [table name] is the name of the chado table whose schema definition you
 * want to augment.
 * @}
 */

/**
 * Implements hook_chado_schema_v1_3_acquisition()
 *
 * Purpose: To describe the structure of 'acquisition' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'acquisition' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_acquisition() {
  $description = [
    'description' => '',
    'table' => 'acquisition',
    'fields' => [
      'acquisition_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'channel_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'acquisitiondate' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => FALSE,
        'default' => 'now()',
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'uri' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'acquisition_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
      'acquisition_idx1' => [
        0 => 'assay_id',
      ],
      'acquisition_idx2' => [
        0 => 'protocol_id',
      ],
      'acquisition_idx3' => [
        0 => 'channel_id',
      ],
    ],
    'primary key' => [
      0 => 'acquisition_id',
    ],
    'foreign keys' => [
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
      'channel' => [
        'table' => 'channel',
        'columns' => [
          'channel_id' => 'channel_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'acquisitionprop',
      1 => 'acquisition_relationship',
      3 => 'quantification',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_acquisitionprop()
 *
 * Purpose: To describe the structure of 'acquisitionprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'acquisitionprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_acquisitionprop() {
  $description = [
    'description' => '',
    'table' => 'acquisitionprop',
    'fields' => [
      'acquisitionprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'acquisition_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'acquisitionprop_c1' => [
        0 => 'acquisition_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'acquisitionprop_idx1' => [
        0 => 'acquisition_id',
      ],
      'acquisitionprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'acquisitionprop_id',
    ],
    'foreign keys' => [
      'acquisition' => [
        'table' => 'acquisition',
        'columns' => [
          'acquisition_id' => 'acquisition_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_acquisition_relationship()
 *
 * Purpose: To describe the structure of 'acquisition_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'acquisition_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_acquisition_relationship() {
  $description = [
    'description' => '',
    'table' => 'acquisition_relationship',
    'fields' => [
      'acquisition_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'acquisition_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'acquisition_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'acquisition_relationship_idx2' => [
        0 => 'type_id',
      ],
      'acquisition_relationship_idx3' => [
        0 => 'object_id',
      ],
    ],
    'primary key' => [
      0 => 'acquisition_relationship_id',
    ],
    'foreign keys' => [
      'acquisition' => [
        'table' => 'acquisition',
        'columns' => [
          'subject_id' => 'acquisition_id',
          'object_id' => 'acquisition_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysis()
 *
 * Purpose: To describe the structure of 'analysis' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysis' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysis() {
  $description = [
    'description' => '',
    'table' => 'analysis',
    'fields' => [
      'analysis_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'program' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'programversion' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'algorithm' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'sourcename' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'sourceversion' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'sourceuri' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'timeexecuted' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
    ],
    'unique keys' => [
      'analysis_c1' => [
        0 => 'program',
        1 => 'programversion',
        2 => 'sourcename',
      ],
    ],
    'primary key' => [
      0 => 'analysis_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'analysis_cvterm',
      1 => 'analysis_dbxref',
      2 => 'analysisfeature',
      3 => 'analysis_organism',
      4 => 'analysisprop',
      5 => 'analysis_pub',
      6 => 'analysis_relationship',
      8 => 'nd_experiment_analysis',
      9 => 'phylotree',
      10 => 'project_analysis',
      11 => 'quantification',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysis_cvterm()
 *
 * Purpose: To describe the structure of 'analysis_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysis_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysis_cvterm() {
  $description = [
    'description' => '',
    'table' => 'analysis_cvterm',
    'fields' => [
      'analysis_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_not' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'analysis_cvterm_c1' => [
        0 => 'analysis_id',
        1 => 'cvterm_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'analysis_cvterm_idx1' => [
        0 => 'analysis_id',
      ],
      'analysis_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
    ],
    'primary key' => [
      0 => 'analysis_cvterm_id',
    ],
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysis_dbxref()
 *
 * Purpose: To describe the structure of 'analysis_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysis_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysis_dbxref() {
  $description = [
    'description' => '',
    'table' => 'analysis_dbxref',
    'fields' => [
      'analysis_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'analysis_dbxref_c1' => [
        0 => 'analysis_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'analysis_dbxref_idx1' => [
        0 => 'analysis_id',
      ],
      'analysis_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'analysis_dbxref_id',
    ],
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysisfeature()
 *
 * Purpose: To describe the structure of 'analysisfeature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysisfeature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysisfeature() {
  $description = [
    'description' => '',
    'table' => 'analysisfeature',
    'fields' => [
      'analysisfeature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rawscore' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => FALSE,
      ],
      'normscore' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => FALSE,
      ],
      'significance' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => FALSE,
      ],
      'identity' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'analysisfeature_c1' => [
        0 => 'feature_id',
        1 => 'analysis_id',
      ],
    ],
    'indexes' => [
      'analysisfeature_idx1' => [
        0 => 'feature_id',
      ],
      'analysisfeature_idx2' => [
        0 => 'analysis_id',
      ],
    ],
    'primary key' => [
      0 => 'analysisfeature_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'analysisfeatureprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysisfeatureprop()
 *
 * Purpose: To describe the structure of 'analysisfeatureprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysisfeatureprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysisfeatureprop() {
  $description = [
    'description' => '',
    'table' => 'analysisfeatureprop',
    'fields' => [
      'analysisfeatureprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'analysisfeature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'analysisfeature_id_type_id_rank' => [
        0 => 'analysisfeature_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'analysisfeatureprop_idx1' => [
        0 => 'analysisfeature_id',
      ],
      'analysisfeatureprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'analysisfeatureprop_id',
    ],
    'foreign keys' => [
      'analysisfeature' => [
        'table' => 'analysisfeature',
        'columns' => [
          'analysisfeature_id' => 'analysisfeature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysis_organism()
 *
 * Purpose: To describe the structure of 'analysis_organism' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysis_organism' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysis_organism() {
  $description = [
    'description' => '',
    'table' => 'analysis_organism',
    'fields' => [
      'analysis_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'networkmod_qtl_indx0' => [
        0 => 'analysis_id',
      ],
      'networkmod_qtl_indx1' => [
        0 => 'organism_id',
      ],
    ],
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysisprop()
 *
 * Purpose: To describe the structure of 'analysisprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysisprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysisprop() {
  $description = [
    'description' => '',
    'table' => 'analysisprop',
    'fields' => [
      'analysisprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'analysisprop_c1' => [
        0 => 'analysis_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'analysisprop_idx1' => [
        0 => 'analysis_id',
      ],
      'analysisprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'analysisprop_id',
    ],
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysis_pub()
 *
 * Purpose: To describe the structure of 'analysis_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysis_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysis_pub() {
  $description = [
    'description' => '',
    'table' => 'analysis_pub',
    'fields' => [
      'analysis_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'analysis_pub_c1' => [
        0 => 'analysis_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'analysis_pub_idx1' => [
        0 => 'analysis_id',
      ],
      'analysis_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'analysis_pub_id',
    ],
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_analysis_relationship()
 *
 * Purpose: To describe the structure of 'analysis_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'analysis_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_analysis_relationship() {
  $description = [
    'description' => '',
    'table' => 'analysis_relationship',
    'fields' => [
      'analysis_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'analysis_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'analysis_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'analysis_relationship_idx2' => [
        0 => 'object_id',
      ],
      'analysis_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'analysis_relationship_id',
    ],
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'subject_id' => 'analysis_id',
          'object_id' => 'analysis_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_arraydesign()
 *
 * Purpose: To describe the structure of 'arraydesign' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'arraydesign' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_arraydesign() {
  $description = [
    'description' => '',
    'table' => 'arraydesign',
    'fields' => [
      'arraydesign_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'manufacturer_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'platformtype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'substratetype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'version' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'array_dimensions' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'element_dimensions' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'num_of_elements' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'num_array_columns' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'num_array_rows' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'num_grid_columns' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'num_grid_rows' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'num_sub_columns' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'num_sub_rows' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'arraydesign_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
      'arraydesign_idx1' => [
        0 => 'manufacturer_id',
      ],
      'arraydesign_idx2' => [
        0 => 'platformtype_id',
      ],
      'arraydesign_idx3' => [
        0 => 'substratetype_id',
      ],
      'arraydesign_idx4' => [
        0 => 'protocol_id',
      ],
      'arraydesign_idx5' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'arraydesign_id',
    ],
    'foreign keys' => [
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'manufacturer_id' => 'contact_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'platformtype_id' => 'cvterm_id',
          'substratetype_id' => 'cvterm_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'arraydesignprop',
      1 => 'assay',
      2 => 'element',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_arraydesignprop()
 *
 * Purpose: To describe the structure of 'arraydesignprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'arraydesignprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_arraydesignprop() {
  $description = [
    'description' => '',
    'table' => 'arraydesignprop',
    'fields' => [
      'arraydesignprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'arraydesign_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'arraydesignprop_c1' => [
        0 => 'arraydesign_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'arraydesignprop_idx1' => [
        0 => 'arraydesign_id',
      ],
      'arraydesignprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'arraydesignprop_id',
    ],
    'foreign keys' => [
      'arraydesign' => [
        'table' => 'arraydesign',
        'columns' => [
          'arraydesign_id' => 'arraydesign_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_assay()
 *
 * Purpose: To describe the structure of 'assay' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'assay' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_assay() {
  $description = [
    'description' => '',
    'table' => 'assay',
    'fields' => [
      'assay_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'arraydesign_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'assaydate' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => FALSE,
        'default' => 'now()',
      ],
      'arrayidentifier' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'arraybatchidentifier' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'operator_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'assay_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
      'assay_idx1' => [
        0 => 'arraydesign_id',
      ],
      'assay_idx2' => [
        0 => 'protocol_id',
      ],
      'assay_idx3' => [
        0 => 'operator_id',
      ],
      'assay_idx4' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'assay_id',
    ],
    'foreign keys' => [
      'arraydesign' => [
        'table' => 'arraydesign',
        'columns' => [
          'arraydesign_id' => 'arraydesign_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'operator_id' => 'contact_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'acquisition',
      1 => 'assay_biomaterial',
      2 => 'assay_project',
      3 => 'assayprop',
      4 => 'control',
      5 => 'study_assay',
      6 => 'studyfactorvalue',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_assay_biomaterial()
 *
 * Purpose: To describe the structure of 'assay_biomaterial' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'assay_biomaterial' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_assay_biomaterial() {
  $description = [
    'description' => '',
    'table' => 'assay_biomaterial',
    'fields' => [
      'assay_biomaterial_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'biomaterial_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'channel_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'assay_biomaterial_c1' => [
        0 => 'assay_id',
        1 => 'biomaterial_id',
        2 => 'channel_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'assay_biomaterial_idx1' => [
        0 => 'assay_id',
      ],
      'assay_biomaterial_idx2' => [
        0 => 'biomaterial_id',
      ],
      'assay_biomaterial_idx3' => [
        0 => 'channel_id',
      ],
    ],
    'primary key' => [
      0 => 'assay_biomaterial_id',
    ],
    'foreign keys' => [
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
      'channel' => [
        'table' => 'channel',
        'columns' => [
          'channel_id' => 'channel_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_assay_project()
 *
 * Purpose: To describe the structure of 'assay_project' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'assay_project' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_assay_project() {
  $description = [
    'description' => '',
    'table' => 'assay_project',
    'fields' => [
      'assay_project_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'assay_project_c1' => [
        0 => 'assay_id',
        1 => 'project_id',
      ],
    ],
    'indexes' => [
      'assay_project_idx1' => [
        0 => 'assay_id',
      ],
      'assay_project_idx2' => [
        0 => 'project_id',
      ],
    ],
    'primary key' => [
      0 => 'assay_project_id',
    ],
    'foreign keys' => [
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_assayprop()
 *
 * Purpose: To describe the structure of 'assayprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'assayprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_assayprop() {
  $description = [
    'description' => '',
    'table' => 'assayprop',
    'fields' => [
      'assayprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'assayprop_c1' => [
        0 => 'assay_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'assayprop_idx1' => [
        0 => 'assay_id',
      ],
      'assayprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'assayprop_id',
    ],
    'foreign keys' => [
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_biomaterial()
 *
 * Purpose: To describe the structure of 'biomaterial' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'biomaterial' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_biomaterial() {
  $description = [
    'description' => '',
    'table' => 'biomaterial',
    'fields' => [
      'biomaterial_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'taxon_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'biosourceprovider_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'biomaterial_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
      'biomaterial_idx1' => [
        0 => 'taxon_id',
      ],
      'biomaterial_idx2' => [
        0 => 'biosourceprovider_id',
      ],
      'biomaterial_idx3' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'taxon_id' => 'organism_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'biosourceprovider_id' => 'contact_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'assay_biomaterial',
      1 => 'biomaterial_dbxref',
      2 => 'biomaterialprop',
      3 => 'biomaterial_relationship',
      5 => 'biomaterial_treatment',
      6 => 'treatment',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_biomaterial_dbxref()
 *
 * Purpose: To describe the structure of 'biomaterial_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'biomaterial_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_biomaterial_dbxref() {
  $description = [
    'description' => '',
    'table' => 'biomaterial_dbxref',
    'fields' => [
      'biomaterial_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'biomaterial_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'biomaterial_dbxref_c1' => [
        0 => 'biomaterial_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'biomaterial_dbxref_idx1' => [
        0 => 'biomaterial_id',
      ],
      'biomaterial_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_dbxref_id',
    ],
    'foreign keys' => [
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_biomaterialprop()
 *
 * Purpose: To describe the structure of 'biomaterialprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'biomaterialprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_biomaterialprop() {
  $description = [
    'description' => '',
    'table' => 'biomaterialprop',
    'fields' => [
      'biomaterialprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'biomaterial_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'biomaterialprop_c1' => [
        0 => 'biomaterial_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'biomaterialprop_idx1' => [
        0 => 'biomaterial_id',
      ],
      'biomaterialprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'biomaterialprop_id',
    ],
    'foreign keys' => [
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_biomaterial_relationship()
 *
 * Purpose: To describe the structure of 'biomaterial_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'biomaterial_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_biomaterial_relationship() {
  $description = [
    'description' => '',
    'table' => 'biomaterial_relationship',
    'fields' => [
      'biomaterial_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'biomaterial_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'biomaterial_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'biomaterial_relationship_idx2' => [
        0 => 'object_id',
      ],
      'biomaterial_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_relationship_id',
    ],
    'foreign keys' => [
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'subject_id' => 'biomaterial_id',
          'object_id' => 'biomaterial_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_biomaterial_treatment()
 *
 * Purpose: To describe the structure of 'biomaterial_treatment' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'biomaterial_treatment' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_biomaterial_treatment() {
  $description = [
    'description' => '',
    'table' => 'biomaterial_treatment',
    'fields' => [
      'biomaterial_treatment_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'biomaterial_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'treatment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'unittype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'float',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'biomaterial_treatment_c1' => [
        0 => 'biomaterial_id',
        1 => 'treatment_id',
      ],
    ],
    'indexes' => [
      'biomaterial_treatment_idx1' => [
        0 => 'biomaterial_id',
      ],
      'biomaterial_treatment_idx2' => [
        0 => 'treatment_id',
      ],
      'biomaterial_treatment_idx3' => [
        0 => 'unittype_id',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_treatment_id',
    ],
    'foreign keys' => [
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
      'treatment' => [
        'table' => 'treatment',
        'columns' => [
          'treatment_id' => 'treatment_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'unittype_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line()
 *
 * Purpose: To describe the structure of 'cell_line' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line() {
  $description = [
    'description' => '',
    'table' => 'cell_line',
    'fields' => [
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'timeaccessioned' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
      'timelastmodified' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
    ],
    'unique keys' => [
      'cell_line_c1' => [
        0 => 'uniquename',
        1 => 'organism_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'cell_line_cvterm',
      1 => 'cell_line_dbxref',
      2 => 'cell_line_feature',
      3 => 'cell_line_library',
      4 => 'cell_lineprop',
      5 => 'cell_line_pub',
      6 => 'cell_line_relationship',
      8 => 'cell_line_synonym',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_cvterm()
 *
 * Purpose: To describe the structure of 'cell_line_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_cvterm() {
  $description = [
    'description' => '',
    'table' => 'cell_line_cvterm',
    'fields' => [
      'cell_line_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cell_line_cvterm_c1' => [
        0 => 'cell_line_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_cvterm_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'cell_line_cvtermprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_cvtermprop()
 *
 * Purpose: To describe the structure of 'cell_line_cvtermprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_cvtermprop() {
  $description = [
    'description' => '',
    'table' => 'cell_line_cvtermprop',
    'fields' => [
      'cell_line_cvtermprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cell_line_cvtermprop_c1' => [
        0 => 'cell_line_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_cvtermprop_id',
    ],
    'foreign keys' => [
      'cell_line_cvterm' => [
        'table' => 'cell_line_cvterm',
        'columns' => [
          'cell_line_cvterm_id' => 'cell_line_cvterm_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_dbxref()
 *
 * Purpose: To describe the structure of 'cell_line_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_dbxref() {
  $description = [
    'description' => '',
    'table' => 'cell_line_dbxref',
    'fields' => [
      'cell_line_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'cell_line_dbxref_c1' => [
        0 => 'cell_line_id',
        1 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_dbxref_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_feature()
 *
 * Purpose: To describe the structure of 'cell_line_feature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_feature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_feature() {
  $description = [
    'description' => '',
    'table' => 'cell_line_feature',
    'fields' => [
      'cell_line_feature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'cell_line_feature_c1' => [
        0 => 'cell_line_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_feature_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_library()
 *
 * Purpose: To describe the structure of 'cell_line_library' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_library' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_library() {
  $description = [
    'description' => '',
    'table' => 'cell_line_library',
    'fields' => [
      'cell_line_library_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'cell_line_library_c1' => [
        0 => 'cell_line_id',
        1 => 'library_id',
        2 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_library_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_lineprop()
 *
 * Purpose: To describe the structure of 'cell_lineprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_lineprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_lineprop() {
  $description = [
    'description' => '',
    'table' => 'cell_lineprop',
    'fields' => [
      'cell_lineprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cell_lineprop_c1' => [
        0 => 'cell_line_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'cell_lineprop_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'cell_lineprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_lineprop_pub()
 *
 * Purpose: To describe the structure of 'cell_lineprop_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_lineprop_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_lineprop_pub() {
  $description = [
    'description' => '',
    'table' => 'cell_lineprop_pub',
    'fields' => [
      'cell_lineprop_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_lineprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'cell_lineprop_pub_c1' => [
        0 => 'cell_lineprop_id',
        1 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_lineprop_pub_id',
    ],
    'foreign keys' => [
      'cell_lineprop' => [
        'table' => 'cell_lineprop',
        'columns' => [
          'cell_lineprop_id' => 'cell_lineprop_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_pub()
 *
 * Purpose: To describe the structure of 'cell_line_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_pub() {
  $description = [
    'description' => '',
    'table' => 'cell_line_pub',
    'fields' => [
      'cell_line_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'cell_line_pub_c1' => [
        0 => 'cell_line_id',
        1 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_pub_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_relationship()
 *
 * Purpose: To describe the structure of 'cell_line_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_relationship() {
  $description = [
    'description' => '',
    'table' => 'cell_line_relationship',
    'fields' => [
      'cell_line_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'cell_line_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_relationship_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'subject_id' => 'cell_line_id',
          'object_id' => 'cell_line_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cell_line_synonym()
 *
 * Purpose: To describe the structure of 'cell_line_synonym' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cell_line_synonym' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cell_line_synonym() {
  $description = [
    'description' => '',
    'table' => 'cell_line_synonym',
    'fields' => [
      'cell_line_synonym_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cell_line_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'synonym_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'is_internal' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
    ],
    'unique keys' => [
      'cell_line_synonym_c1' => [
        0 => 'synonym_id',
        1 => 'cell_line_id',
        2 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_synonym_id',
    ],
    'foreign keys' => [
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
      'synonym' => [
        'table' => 'synonym',
        'columns' => [
          'synonym_id' => 'synonym_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_chadoprop()
 *
 * Purpose: To describe the structure of 'chadoprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'chadoprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_chadoprop() {
  $description = [
    'description' => '',
    'table' => 'chadoprop',
    'fields' => [
      'chadoprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'chadoprop_c1' => [
        0 => 'type_id',
        1 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'chadoprop_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_channel()
 *
 * Purpose: To describe the structure of 'channel' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'channel' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_channel() {
  $description = [
    'description' => '',
    'table' => 'channel',
    'fields' => [
      'channel_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'definition' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'channel_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'channel_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'acquisition',
      1 => 'assay_biomaterial',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_contact()
 *
 * Purpose: To describe the structure of 'contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_contact() {
  $description = [
    'description' => '',
    'table' => 'contact',
    'fields' => [
      'contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'contact_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'contact_id',
    ],
    'indexes' => [
      'library_contact_idx2' => [
        0 => 'contact_id',
      ],
      'pubauthor_contact_idx2' => [
        0 => 'contact_id',
      ],
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'arraydesign',
      1 => 'assay',
      2 => 'biomaterial',
      3 => 'contactprop',
      4 => 'contact_relationship',
      6 => 'feature_contact',
      7 => 'featuremap_contact',
      8 => 'library_contact',
      9 => 'nd_experiment_contact',
      10 => 'project_contact',
      11 => 'pubauthor_contact',
      12 => 'quantification',
      13 => 'stockcollection',
      14 => 'study',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_contactprop()
 *
 * Purpose: To describe the structure of 'contactprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'contactprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_contactprop() {
  $description = [
    'description' => '',
    'table' => 'contactprop',
    'fields' => [
      'contactprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'contactprop_c1' => [
        0 => 'contact_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'contactprop_idx1' => [
        0 => 'contact_id',
      ],
      'contactprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'contactprop_id',
    ],
    'foreign keys' => [
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_contact_relationship()
 *
 * Purpose: To describe the structure of 'contact_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'contact_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_contact_relationship() {
  $description = [
    'description' => '',
    'table' => 'contact_relationship',
    'fields' => [
      'contact_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'contact_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'contact_relationship_idx1' => [
        0 => 'type_id',
      ],
      'contact_relationship_idx2' => [
        0 => 'subject_id',
      ],
      'contact_relationship_idx3' => [
        0 => 'object_id',
      ],
    ],
    'primary key' => [
      0 => 'contact_relationship_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'subject_id' => 'contact_id',
          'object_id' => 'contact_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_control()
 *
 * Purpose: To describe the structure of 'control' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'control' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_control() {
  $description = [
    'description' => '',
    'table' => 'control',
    'fields' => [
      'control_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'tableinfo_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'row_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'indexes' => [
      'control_idx1' => [
        0 => 'type_id',
      ],
      'control_idx2' => [
        0 => 'assay_id',
      ],
      'control_idx3' => [
        0 => 'tableinfo_id',
      ],
      'control_idx4' => [
        0 => 'row_id',
      ],
    ],
    'primary key' => [
      0 => 'control_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'tableinfo' => [
        'table' => 'tableinfo',
        'columns' => [
          'tableinfo_id' => 'tableinfo_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cv()
 *
 * Purpose: To describe the structure of 'cv' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cv' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cv() {
  $description = [
    'description' => '',
    'table' => 'cv',
    'fields' => [
      'cv_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'definition' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'cv_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'cv_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'cvprop',
      1 => 'cvterm',
      2 => 'cvtermpath',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvprop()
 *
 * Purpose: To describe the structure of 'cvprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvprop() {
  $description = [
    'description' => '',
    'table' => 'cvprop',
    'fields' => [
      'cvprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cv_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cvprop_c1' => [
        0 => 'cv_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'cvprop_id',
    ],
    'foreign keys' => [
      'cv' => [
        'table' => 'cv',
        'columns' => [
          'cv_id' => 'cv_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cv_root_mview()
 *
 * Purpose: To describe the structure of 'cv_root_mview' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cv_root_mview' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cv_root_mview() {
  $description = [
    'description' => '',
    'table' => 'cv_root_mview',
    'fields' => [
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cv_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cv_name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'cv_root_mview_indx1' => [
        0 => 'cvterm_id',
      ],
      'cv_root_mview_indx2' => [
        0 => 'cv_id',
      ],
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvterm()
 *
 * Purpose: To describe the structure of 'cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvterm() {
  $description = [
    'description' => '',
    'table' => 'cvterm',
    'fields' => [
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cv_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ],
      'definition' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_obsolete' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'is_relationshiptype' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cvterm_c1' => [
        0 => 'name',
        1 => 'cv_id',
        2 => 'is_obsolete',
      ],
      'cvterm_c2' => [
        0 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'cvterm_idx1' => [
        0 => 'cv_id',
      ],
      'cvterm_idx2' => [
        0 => 'name',
      ],
      'cvterm_idx3' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'cvterm_id',
    ],
    'foreign keys' => [
      'cv' => [
        'table' => 'cv',
        'columns' => [
          'cv_id' => 'cv_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'acquisitionprop',
      1 => 'acquisition_relationship',
      2 => 'analysis_cvterm',
      3 => 'analysisfeatureprop',
      4 => 'analysisprop',
      5 => 'analysis_relationship',
      6 => 'arraydesign',
      8 => 'arraydesignprop',
      9 => 'assayprop',
      10 => 'biomaterialprop',
      11 => 'biomaterial_relationship',
      12 => 'biomaterial_treatment',
      13 => 'cell_line_cvterm',
      14 => 'cell_line_cvtermprop',
      15 => 'cell_lineprop',
      16 => 'cell_line_relationship',
      17 => 'chadoprop',
      18 => 'contact',
      19 => 'contactprop',
      20 => 'contact_relationship',
      21 => 'control',
      22 => 'cvprop',
      23 => 'cvterm_dbxref',
      24 => 'cvtermpath',
      27 => 'cvtermprop',
      29 => 'cvterm_relationship',
      32 => 'cvtermsynonym',
      34 => 'dbprop',
      35 => 'dbxrefprop',
      36 => 'element',
      37 => 'element_relationship',
      38 => 'elementresult_relationship',
      39 => 'environment_cvterm',
      40 => 'expression_cvterm',
      42 => 'expression_cvtermprop',
      43 => 'expressionprop',
      44 => 'feature',
      45 => 'feature_cvterm',
      46 => 'feature_cvtermprop',
      47 => 'feature_expressionprop',
      48 => 'feature_genotype',
      49 => 'featuremap',
      50 => 'featuremapprop',
      51 => 'featureposprop',
      52 => 'featureprop',
      53 => 'feature_pubprop',
      54 => 'feature_relationship',
      55 => 'feature_relationshipprop',
      56 => 'genotype',
      57 => 'genotypeprop',
      58 => 'library',
      59 => 'library_cvterm',
      60 => 'library_expressionprop',
      61 => 'library_featureprop',
      62 => 'libraryprop',
      63 => 'library_relationship',
      64 => 'nd_experiment',
      65 => 'nd_experiment_analysis',
      66 => 'nd_experimentprop',
      67 => 'nd_experiment_stock',
      68 => 'nd_experiment_stockprop',
      69 => 'nd_geolocationprop',
      70 => 'nd_protocol',
      71 => 'nd_protocolprop',
      72 => 'nd_protocol_reagent',
      73 => 'nd_reagent',
      74 => 'nd_reagentprop',
      75 => 'nd_reagent_relationship',
      76 => 'organism',
      77 => 'organism_cvterm',
      78 => 'organism_cvtermprop',
      79 => 'organismprop',
      80 => 'organism_relationship',
      81 => 'phendesc',
      82 => 'phenotype',
      86 => 'phenotype_comparison_cvterm',
      87 => 'phenotype_cvterm',
      88 => 'phenotypeprop',
      89 => 'phenstatement',
      90 => 'phylonode',
      91 => 'phylonodeprop',
      92 => 'phylonode_relationship',
      93 => 'phylotree',
      94 => 'phylotreeprop',
      95 => 'projectprop',
      96 => 'project_relationship',
      97 => 'protocol',
      98 => 'protocolparam',
      100 => 'pub',
      101 => 'pubprop',
      102 => 'pub_relationship',
      103 => 'quantificationprop',
      104 => 'quantification_relationship',
      105 => 'stock',
      106 => 'stockcollection',
      107 => 'stockcollectionprop',
      108 => 'stock_cvterm',
      109 => 'stock_cvtermprop',
      110 => 'stock_dbxrefprop',
      111 => 'stock_feature',
      112 => 'stock_featuremap',
      113 => 'stockprop',
      114 => 'stock_relationship',
      115 => 'stock_relationship_cvterm',
      116 => 'studydesignprop',
      117 => 'studyfactor',
      118 => 'studyprop',
      119 => 'studyprop_feature',
      120 => 'synonym',
      121 => 'treatment',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvterm_dbxref()
 *
 * Purpose: To describe the structure of 'cvterm_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvterm_dbxref() {
  $description = [
    'description' => '',
    'table' => 'cvterm_dbxref',
    'fields' => [
      'cvterm_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_for_definition' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cvterm_dbxref_c1' => [
        0 => 'cvterm_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'cvterm_dbxref_idx1' => [
        0 => 'cvterm_id',
      ],
      'cvterm_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'cvterm_dbxref_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvtermpath()
 *
 * Purpose: To describe the structure of 'cvtermpath' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvtermpath' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvtermpath() {
  $description = [
    'description' => '',
    'table' => 'cvtermpath',
    'fields' => [
      'cvtermpath_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cv_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pathdistance' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'cvtermpath_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'pathdistance',
      ],
    ],
    'indexes' => [
      'cvtermpath_idx1' => [
        0 => 'type_id',
      ],
      'cvtermpath_idx2' => [
        0 => 'subject_id',
      ],
      'cvtermpath_idx3' => [
        0 => 'object_id',
      ],
      'cvtermpath_idx4' => [
        0 => 'cv_id',
      ],
    ],
    'primary key' => [
      0 => 'cvtermpath_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ],
      ],
      'cv' => [
        'table' => 'cv',
        'columns' => [
          'cv_id' => 'cv_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvtermprop()
 *
 * Purpose: To describe the structure of 'cvtermprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvtermprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvtermprop() {
  $description = [
    'description' => '',
    'table' => 'cvtermprop',
    'fields' => [
      'cvtermprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
        'default' => '',
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'cvtermprop_cvterm_id_type_id_value_rank_key' => [
        0 => 'cvterm_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'cvtermprop_idx1' => [
        0 => 'cvterm_id',
      ],
      'cvtermprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'cvtermprop_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvterm_relationship()
 *
 * Purpose: To describe the structure of 'cvterm_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvterm_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvterm_relationship() {
  $description = [
    'description' => '',
    'table' => 'cvterm_relationship',
    'fields' => [
      'cvterm_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'cvterm_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'cvterm_relationship_idx1' => [
        0 => 'type_id',
      ],
      'cvterm_relationship_idx2' => [
        0 => 'subject_id',
      ],
      'cvterm_relationship_idx3' => [
        0 => 'object_id',
      ],
    ],
    'primary key' => [
      0 => 'cvterm_relationship_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_cvtermsynonym()
 *
 * Purpose: To describe the structure of 'cvtermsynonym' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'cvtermsynonym' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_cvtermsynonym() {
  $description = [
    'description' => '',
    'table' => 'cvtermsynonym',
    'fields' => [
      'cvtermsynonym_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'synonym' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'cvtermsynonym_c1' => [
        0 => 'cvterm_id',
        1 => 'synonym',
      ],
    ],
    'indexes' => [
      'cvtermsynonym_idx1' => [
        0 => 'cvterm_id',
      ],
    ],
    'primary key' => [
      0 => 'cvtermsynonym_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_db()
 *
 * Purpose: To describe the structure of 'db' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'db' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_db() {
  $description = [
    'description' => '',
    'table' => 'db',
    'fields' => [
      'db_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'urlprefix' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'url' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'db_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'db_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'dbprop',
      1 => 'dbxref',
      2 => 'stockcollection_db',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_dbprop()
 *
 * Purpose: To describe the structure of 'dbprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'dbprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_dbprop() {
  $description = [
    'description' => '',
    'table' => 'dbprop',
    'fields' => [
      'dbprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'db_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'dbprop_c1' => [
        0 => 'db_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'dbprop_idx1' => [
        0 => 'db_id',
      ],
      'dbprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'dbprop_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'db' => [
        'table' => 'db',
        'columns' => [
          'db_id' => 'db_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_dbxref()
 *
 * Purpose: To describe the structure of 'dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_dbxref() {
  $description = [
    'description' => '',
    'table' => 'dbxref',
    'fields' => [
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'db_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'accession' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ],
      'version' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
        'default' => '',
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'dbxref_c1' => [
        0 => 'db_id',
        1 => 'accession',
        2 => 'version',
      ],
    ],
    'indexes' => [
      'dbxref_idx1' => [
        0 => 'db_id',
      ],
      'dbxref_idx2' => [
        0 => 'accession',
      ],
      'dbxref_idx3' => [
        0 => 'version',
      ],
    ],
    'primary key' => [
      0 => 'dbxref_id',
    ],
    'foreign keys' => [
      'db' => [
        'table' => 'db',
        'columns' => [
          'db_id' => 'db_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'analysis_dbxref',
      1 => 'arraydesign',
      2 => 'assay',
      3 => 'biomaterial',
      4 => 'biomaterial_dbxref',
      5 => 'cell_line_dbxref',
      6 => 'cvterm',
      7 => 'cvterm_dbxref',
      8 => 'dbxrefprop',
      9 => 'element',
      10 => 'feature',
      11 => 'feature_cvterm_dbxref',
      12 => 'feature_dbxref',
      13 => 'featuremap_dbxref',
      14 => 'library_dbxref',
      15 => 'nd_experiment_dbxref',
      16 => 'nd_experiment_stock_dbxref',
      17 => 'organism_dbxref',
      18 => 'phylonode_dbxref',
      19 => 'phylotree',
      20 => 'project_dbxref',
      21 => 'protocol',
      22 => 'pub_dbxref',
      23 => 'stock',
      24 => 'stock_dbxref',
      25 => 'study',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_dbxrefprop()
 *
 * Purpose: To describe the structure of 'dbxrefprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_dbxrefprop() {
  $description = [
    'description' => '',
    'table' => 'dbxrefprop',
    'fields' => [
      'dbxrefprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
        'default' => '',
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'dbxrefprop_c1' => [
        0 => 'dbxref_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'dbxrefprop_idx1' => [
        0 => 'dbxref_id',
      ],
      'dbxrefprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'dbxrefprop_id',
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_eimage()
 *
 * Purpose: To describe the structure of 'eimage' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'eimage' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_eimage() {
  $description = [
    'description' => '',
    'table' => 'eimage',
    'fields' => [
      'eimage_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'eimage_data' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'eimage_type' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'image_uri' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
    ],
    'primary key' => [
      0 => 'eimage_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'expression_image',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_element()
 *
 * Purpose: To describe the structure of 'element' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'element' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_element() {
  $description = [
    'description' => '',
    'table' => 'element',
    'fields' => [
      'element_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'arraydesign_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'element_c1' => [
        0 => 'feature_id',
        1 => 'arraydesign_id',
      ],
    ],
    'indexes' => [
      'element_idx1' => [
        0 => 'feature_id',
      ],
      'element_idx2' => [
        0 => 'arraydesign_id',
      ],
      'element_idx3' => [
        0 => 'type_id',
      ],
      'element_idx4' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'element_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'arraydesign' => [
        'table' => 'arraydesign',
        'columns' => [
          'arraydesign_id' => 'arraydesign_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'element_relationship',
      2 => 'elementresult',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_element_relationship()
 *
 * Purpose: To describe the structure of 'element_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'element_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_element_relationship() {
  $description = [
    'description' => '',
    'table' => 'element_relationship',
    'fields' => [
      'element_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'element_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'element_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'element_relationship_idx2' => [
        0 => 'type_id',
      ],
      'element_relationship_idx3' => [
        0 => 'object_id',
      ],
      'element_relationship_idx4' => [
        0 => 'value',
      ],
    ],
    'primary key' => [
      0 => 'element_relationship_id',
    ],
    'foreign keys' => [
      'element' => [
        'table' => 'element',
        'columns' => [
          'subject_id' => 'element_id',
          'object_id' => 'element_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_elementresult()
 *
 * Purpose: To describe the structure of 'elementresult' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'elementresult' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_elementresult() {
  $description = [
    'description' => '',
    'table' => 'elementresult',
    'fields' => [
      'elementresult_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'element_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'quantification_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'signal' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'elementresult_c1' => [
        0 => 'element_id',
        1 => 'quantification_id',
      ],
    ],
    'indexes' => [
      'elementresult_idx1' => [
        0 => 'element_id',
      ],
      'elementresult_idx2' => [
        0 => 'quantification_id',
      ],
      'elementresult_idx3' => [
        0 => 'signal',
      ],
    ],
    'primary key' => [
      0 => 'elementresult_id',
    ],
    'foreign keys' => [
      'element' => [
        'table' => 'element',
        'columns' => [
          'element_id' => 'element_id',
        ],
      ],
      'quantification' => [
        'table' => 'quantification',
        'columns' => [
          'quantification_id' => 'quantification_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'elementresult_relationship',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_elementresult_relationship()
 *
 * Purpose: To describe the structure of 'elementresult_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'elementresult_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_elementresult_relationship() {
  $description = [
    'description' => '',
    'table' => 'elementresult_relationship',
    'fields' => [
      'elementresult_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'elementresult_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'elementresult_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'elementresult_relationship_idx2' => [
        0 => 'type_id',
      ],
      'elementresult_relationship_idx3' => [
        0 => 'object_id',
      ],
      'elementresult_relationship_idx4' => [
        0 => 'value',
      ],
    ],
    'primary key' => [
      0 => 'elementresult_relationship_id',
    ],
    'foreign keys' => [
      'elementresult' => [
        'table' => 'elementresult',
        'columns' => [
          'subject_id' => 'elementresult_id',
          'object_id' => 'elementresult_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_environment()
 *
 * Purpose: To describe the structure of 'environment' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'environment' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_environment() {
  $description = [
    'description' => '',
    'table' => 'environment',
    'fields' => [
      'environment_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'environment_c1' => [
        0 => 'uniquename',
      ],
    ],
    'indexes' => [
      'environment_idx1' => [
        0 => 'uniquename',
      ],
    ],
    'primary key' => [
      0 => 'environment_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'environment_cvterm',
      1 => 'phendesc',
      2 => 'phenotype_comparison',
      4 => 'phenstatement',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_environment_cvterm()
 *
 * Purpose: To describe the structure of 'environment_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'environment_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_environment_cvterm() {
  $description = [
    'description' => '',
    'table' => 'environment_cvterm',
    'fields' => [
      'environment_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'environment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'environment_cvterm_c1' => [
        0 => 'environment_id',
        1 => 'cvterm_id',
      ],
    ],
    'indexes' => [
      'environment_cvterm_idx1' => [
        0 => 'environment_id',
      ],
      'environment_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
    ],
    'primary key' => [
      0 => 'environment_cvterm_id',
    ],
    'foreign keys' => [
      'environment' => [
        'table' => 'environment',
        'columns' => [
          'environment_id' => 'environment_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_expression()
 *
 * Purpose: To describe the structure of 'expression' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'expression' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_expression() {
  $description = [
    'description' => '',
    'table' => 'expression',
    'fields' => [
      'expression_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'md5checksum' => [
        'size' => 'normal',
        'type' => 'char',
        'length' => '32',
        'not null' => FALSE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'expression_c1' => [
        0 => 'uniquename',
      ],
    ],
    'primary key' => [
      0 => 'expression_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'expression_cvterm',
      1 => 'expression_image',
      2 => 'expressionprop',
      3 => 'expression_pub',
      4 => 'feature_expression',
      5 => 'library_expression',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_expression_cvterm()
 *
 * Purpose: To describe the structure of 'expression_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'expression_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_expression_cvterm() {
  $description = [
    'description' => '',
    'table' => 'expression_cvterm',
    'fields' => [
      'expression_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'cvterm_type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'expression_cvterm_c1' => [
        0 => 'expression_id',
        1 => 'cvterm_id',
        2 => 'rank',
        3 => 'cvterm_type_id',
      ],
    ],
    'indexes' => [
      'expression_cvterm_idx1' => [
        0 => 'expression_id',
      ],
      'expression_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
      'expression_cvterm_idx3' => [
        0 => 'cvterm_type_id',
      ],
    ],
    'primary key' => [
      0 => 'expression_cvterm_id',
    ],
    'foreign keys' => [
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
          'cvterm_type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'expression_cvtermprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_expression_cvtermprop()
 *
 * Purpose: To describe the structure of 'expression_cvtermprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'expression_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_expression_cvtermprop() {
  $description = [
    'description' => '',
    'table' => 'expression_cvtermprop',
    'fields' => [
      'expression_cvtermprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'expression_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'expression_cvtermprop_c1' => [
        0 => 'expression_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'expression_cvtermprop_idx1' => [
        0 => 'expression_cvterm_id',
      ],
      'expression_cvtermprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'expression_cvtermprop_id',
    ],
    'foreign keys' => [
      'expression_cvterm' => [
        'table' => 'expression_cvterm',
        'columns' => [
          'expression_cvterm_id' => 'expression_cvterm_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_expression_image()
 *
 * Purpose: To describe the structure of 'expression_image' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'expression_image' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_expression_image() {
  $description = [
    'description' => '',
    'table' => 'expression_image',
    'fields' => [
      'expression_image_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'eimage_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'expression_image_c1' => [
        0 => 'expression_id',
        1 => 'eimage_id',
      ],
    ],
    'indexes' => [
      'expression_image_idx1' => [
        0 => 'expression_id',
      ],
      'expression_image_idx2' => [
        0 => 'eimage_id',
      ],
    ],
    'primary key' => [
      0 => 'expression_image_id',
    ],
    'foreign keys' => [
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
      'eimage' => [
        'table' => 'eimage',
        'columns' => [
          'eimage_id' => 'eimage_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_expressionprop()
 *
 * Purpose: To describe the structure of 'expressionprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'expressionprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_expressionprop() {
  $description = [
    'description' => '',
    'table' => 'expressionprop',
    'fields' => [
      'expressionprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'expressionprop_c1' => [
        0 => 'expression_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'expressionprop_idx1' => [
        0 => 'expression_id',
      ],
      'expressionprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'expressionprop_id',
    ],
    'foreign keys' => [
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_expression_pub()
 *
 * Purpose: To describe the structure of 'expression_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'expression_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_expression_pub() {
  $description = [
    'description' => '',
    'table' => 'expression_pub',
    'fields' => [
      'expression_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'expression_pub_c1' => [
        0 => 'expression_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'expression_pub_idx1' => [
        0 => 'expression_id',
      ],
      'expression_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'expression_pub_id',
    ],
    'foreign keys' => [
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature()
 *
 * Purpose: To describe the structure of 'feature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature() {
  $description = [
    'description' => '',
    'table' => 'feature',
    'fields' => [
      'feature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'residues' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'seqlen' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'md5checksum' => [
        'size' => 'normal',
        'type' => 'char',
        'length' => '32',
        'not null' => FALSE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_analysis' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'is_obsolete' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'timeaccessioned' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
      'timelastmodified' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
    ],
    'unique keys' => [
      'feature_c1' => [
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'feature_idx1' => [
        0 => 'dbxref_id',
      ],
      'feature_idx2' => [
        0 => 'organism_id',
      ],
      'feature_idx3' => [
        0 => 'type_id',
      ],
      'feature_idx4' => [
        0 => 'uniquename',
      ],
      'feature_name_ind1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'feature_id',
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'analysisfeature',
      1 => 'cell_line_feature',
      2 => 'element',
      3 => 'feature_contact',
      4 => 'feature_cvterm',
      5 => 'feature_dbxref',
      6 => 'feature_expression',
      7 => 'feature_genotype',
      9 => 'featureloc',
      11 => 'feature_phenotype',
      12 => 'featurepos',
      14 => 'featureprop',
      15 => 'feature_pub',
      16 => 'featurerange',
      21 => 'feature_relationship',
      23 => 'feature_synonym',
      24 => 'library_feature',
      25 => 'nd_reagent',
      26 => 'phylonode',
      27 => 'project_feature',
      28 => 'stock_feature',
      29 => 'studyprop_feature',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_contact()
 *
 * Purpose: To describe the structure of 'feature_contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_contact() {
  $description = [
    'description' => '',
    'table' => 'feature_contact',
    'fields' => [
      'feature_contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_contact_c1' => [
        0 => 'feature_id',
        1 => 'contact_id',
      ],
    ],
    'indexes' => [
      'feature_contact_idx1' => [
        0 => 'feature_id',
      ],
      'feature_contact_idx2' => [
        0 => 'contact_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_contact_id',
    ],
    'foreign keys' => [
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_cvterm()
 *
 * Purpose: To describe the structure of 'feature_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_cvterm() {
  $description = [
    'description' => '',
    'table' => 'feature_cvterm',
    'fields' => [
      'feature_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_not' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'feature_cvterm_c1' => [
        0 => 'feature_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'feature_cvterm_idx1' => [
        0 => 'feature_id',
      ],
      'feature_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
      'feature_cvterm_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvterm_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_cvterm_dbxref',
      1 => 'feature_cvtermprop',
      2 => 'feature_cvterm_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_cvterm_dbxref()
 *
 * Purpose: To describe the structure of 'feature_cvterm_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_cvterm_dbxref() {
  $description = [
    'description' => '',
    'table' => 'feature_cvterm_dbxref',
    'fields' => [
      'feature_cvterm_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_cvterm_dbxref_c1' => [
        0 => 'feature_cvterm_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'feature_cvterm_dbxref_idx1' => [
        0 => 'feature_cvterm_id',
      ],
      'feature_cvterm_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvterm_dbxref_id',
    ],
    'foreign keys' => [
      'feature_cvterm' => [
        'table' => 'feature_cvterm',
        'columns' => [
          'feature_cvterm_id' => 'feature_cvterm_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_cvtermprop()
 *
 * Purpose: To describe the structure of 'feature_cvtermprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_cvtermprop() {
  $description = [
    'description' => '',
    'table' => 'feature_cvtermprop',
    'fields' => [
      'feature_cvtermprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'feature_cvtermprop_c1' => [
        0 => 'feature_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'feature_cvtermprop_idx1' => [
        0 => 'feature_cvterm_id',
      ],
      'feature_cvtermprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvtermprop_id',
    ],
    'foreign keys' => [
      'feature_cvterm' => [
        'table' => 'feature_cvterm',
        'columns' => [
          'feature_cvterm_id' => 'feature_cvterm_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_cvterm_pub()
 *
 * Purpose: To describe the structure of 'feature_cvterm_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_cvterm_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_cvterm_pub() {
  $description = [
    'description' => '',
    'table' => 'feature_cvterm_pub',
    'fields' => [
      'feature_cvterm_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_cvterm_pub_c1' => [
        0 => 'feature_cvterm_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'feature_cvterm_pub_idx1' => [
        0 => 'feature_cvterm_id',
      ],
      'feature_cvterm_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvterm_pub_id',
    ],
    'foreign keys' => [
      'feature_cvterm' => [
        'table' => 'feature_cvterm',
        'columns' => [
          'feature_cvterm_id' => 'feature_cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_dbxref()
 *
 * Purpose: To describe the structure of 'feature_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_dbxref() {
  $description = [
    'description' => '',
    'table' => 'feature_dbxref',
    'fields' => [
      'feature_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'feature_dbxref_c1' => [
        0 => 'feature_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'feature_dbxref_idx1' => [
        0 => 'feature_id',
      ],
      'feature_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_dbxref_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_expression()
 *
 * Purpose: To describe the structure of 'feature_expression' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_expression' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_expression() {
  $description = [
    'description' => '',
    'table' => 'feature_expression',
    'fields' => [
      'feature_expression_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_expression_c1' => [
        0 => 'expression_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
      'feature_expression_idx1' => [
        0 => 'expression_id',
      ],
      'feature_expression_idx2' => [
        0 => 'feature_id',
      ],
      'feature_expression_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_expression_id',
    ],
    'foreign keys' => [
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_expressionprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_expressionprop()
 *
 * Purpose: To describe the structure of 'feature_expressionprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_expressionprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_expressionprop() {
  $description = [
    'description' => '',
    'table' => 'feature_expressionprop',
    'fields' => [
      'feature_expressionprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'feature_expressionprop_c1' => [
        0 => 'feature_expression_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'feature_expressionprop_idx1' => [
        0 => 'feature_expression_id',
      ],
      'feature_expressionprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_expressionprop_id',
    ],
    'foreign keys' => [
      'feature_expression' => [
        'table' => 'feature_expression',
        'columns' => [
          'feature_expression_id' => 'feature_expression_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_genotype()
 *
 * Purpose: To describe the structure of 'feature_genotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_genotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_genotype() {
  $description = [
    'description' => '',
    'table' => 'feature_genotype',
    'fields' => [
      'feature_genotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'genotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'chromosome_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cgroup' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_genotype_c1' => [
        0 => 'feature_id',
        1 => 'genotype_id',
        2 => 'cvterm_id',
        3 => 'chromosome_id',
        4 => 'rank',
        5 => 'cgroup',
      ],
    ],
    'indexes' => [
      'feature_genotype_idx1' => [
        0 => 'feature_id',
      ],
      'feature_genotype_idx2' => [
        0 => 'genotype_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_genotype_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
          'chromosome_id' => 'feature_id',
        ],
      ],
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featureloc()
 *
 * Purpose: To describe the structure of 'featureloc' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featureloc' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featureloc() {
  $description = [
    'description' => '',
    'table' => 'featureloc',
    'fields' => [
      'featureloc_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'srcfeature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'fmin' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'is_fmin_partial' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'fmax' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'is_fmax_partial' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'strand' => [
        'size' => 'small',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'phase' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'residue_info' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'locgroup' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'indexes' => [
      'binloc_boxrange' => [
        0 => 'fmin',
      ],
      'binloc_boxrange_src' => [
        0 => 'srcfeature_id',
        1 => 'fmin',
      ],
      'featureloc_idx1' => [
        0 => 'feature_id',
      ],
      'featureloc_idx1b' => [
        0 => 'feature_id',
        1 => 'fmin',
        2 => 'fmax',
      ],
      'featureloc_idx2' => [
        0 => 'srcfeature_id',
      ],
      'featureloc_idx3' => [
        0 => 'srcfeature_id',
        1 => 'fmin',
        2 => 'fmax',
      ],
    ],
    'unique keys' => [
      'featureloc_c1' => [
        0 => 'feature_id',
        1 => 'locgroup',
        2 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'featureloc_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
          'srcfeature_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'featureloc_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featureloc_pub()
 *
 * Purpose: To describe the structure of 'featureloc_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featureloc_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featureloc_pub() {
  $description = [
    'description' => '',
    'table' => 'featureloc_pub',
    'fields' => [
      'featureloc_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featureloc_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'featureloc_pub_c1' => [
        0 => 'featureloc_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'featureloc_pub_idx1' => [
        0 => 'featureloc_id',
      ],
      'featureloc_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'featureloc_pub_id',
    ],
    'foreign keys' => [
      'featureloc' => [
        'table' => 'featureloc',
        'columns' => [
          'featureloc_id' => 'featureloc_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featuremap()
 *
 * Purpose: To describe the structure of 'featuremap' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featuremap' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featuremap() {
  $description = [
    'description' => '',
    'table' => 'featuremap',
    'fields' => [
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'unittype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'featuremap_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'unittype_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'featuremap_contact',
      1 => 'featuremap_dbxref',
      2 => 'featuremap_organism',
      3 => 'featuremapprop',
      4 => 'featuremap_pub',
      5 => 'featurepos',
      6 => 'featurerange',
      7 => 'stock_featuremap',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featuremap_contact()
 *
 * Purpose: To describe the structure of 'featuremap_contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featuremap_contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featuremap_contact() {
  $description = [
    'description' => '',
    'table' => 'featuremap_contact',
    'fields' => [
      'featuremap_contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'featuremap_contact_c1' => [
        0 => 'featuremap_id',
        1 => 'contact_id',
      ],
    ],
    'indexes' => [
      'featuremap_contact_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremap_contact_idx2' => [
        0 => 'contact_id',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_contact_id',
    ],
    'foreign keys' => [
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featuremap_dbxref()
 *
 * Purpose: To describe the structure of 'featuremap_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featuremap_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featuremap_dbxref() {
  $description = [
    'description' => '',
    'table' => 'featuremap_dbxref',
    'fields' => [
      'featuremap_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      /* @todo: this unique constraint is missing from the actual Chado schema.
       * It should be included.
       * 'feature_dbxref_c1' => array(
       * 0 => 'featuremap_id',
       * 1 => 'dbxref_id',
       * ),
       */
    ],
    'indexes' => [
      'featuremap_dbxref_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremap_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_dbxref_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featuremap_organism()
 *
 * Purpose: To describe the structure of 'featuremap_organism' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featuremap_organism' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featuremap_organism() {
  $description = [
    'description' => '',
    'table' => 'featuremap_organism',
    'fields' => [
      'featuremap_organism_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'featuremap_organism_c1' => [
        0 => 'featuremap_id',
        1 => 'organism_id',
      ],
    ],
    'indexes' => [
      'featuremap_organism_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremap_organism_idx2' => [
        0 => 'organism_id',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_organism_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featuremapprop()
 *
 * Purpose: To describe the structure of 'featuremapprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featuremapprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featuremapprop() {
  $description = [
    'description' => '',
    'table' => 'featuremapprop',
    'fields' => [
      'featuremapprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'featuremapprop_c1' => [
        0 => 'featuremap_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'featuremapprop_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremapprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'featuremapprop_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featuremap_pub()
 *
 * Purpose: To describe the structure of 'featuremap_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featuremap_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featuremap_pub() {
  $description = [
    'description' => '',
    'table' => 'featuremap_pub',
    'fields' => [
      'featuremap_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      /* @todo: this unique constraint is missing from the actual Chado schema.
       * It should be included.
       * 'feature_pub_c1' => array(
       * 0 => 'featuremap_id',
       * 1 => 'pub_id',
       * ),
       */
    ],
    'indexes' => [
      'featuremap_pub_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremap_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_pub_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_phenotype()
 *
 * Purpose: To describe the structure of 'feature_phenotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_phenotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_phenotype() {
  $description = [
    'description' => '',
    'table' => 'feature_phenotype',
    'fields' => [
      'feature_phenotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'phenotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_phenotype_c1' => [
        0 => 'feature_id',
        1 => 'phenotype_id',
      ],
    ],
    'indexes' => [
      'feature_phenotype_idx1' => [
        0 => 'feature_id',
      ],
      'feature_phenotype_idx2' => [
        0 => 'phenotype_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_phenotype_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featurepos()
 *
 * Purpose: To describe the structure of 'featurepos' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featurepos' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featurepos() {
  $description = [
    'description' => '',
    'table' => 'featurepos',
    'fields' => [
      'featurepos_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'map_feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'mappos' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'featurepos_idx1' => [
        0 => 'featuremap_id',
      ],
      'featurepos_idx2' => [
        0 => 'feature_id',
      ],
      'featurepos_idx3' => [
        0 => 'map_feature_id',
      ],
    ],
    'primary key' => [
      0 => 'featurepos_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
          'map_feature_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'featureposprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featureposprop()
 *
 * Purpose: To describe the structure of 'featureposprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featureposprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featureposprop() {
  $description = [
    'description' => '',
    'table' => 'featureposprop',
    'fields' => [
      'featureposprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featurepos_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'featureposprop_c1' => [
        0 => 'featurepos_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'featureposprop_idx1' => [
        0 => 'featurepos_id',
      ],
      'featureposprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'featureposprop_id',
    ],
    'foreign keys' => [
      'featurepos' => [
        'table' => 'featurepos',
        'columns' => [
          'featurepos_id' => 'featurepos_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featureprop()
 *
 * Purpose: To describe the structure of 'featureprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featureprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featureprop() {
  $description = [
    'description' => '',
    'table' => 'featureprop',
    'fields' => [
      'featureprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'featureprop_c1' => [
        0 => 'feature_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'featureprop_idx1' => [
        0 => 'feature_id',
      ],
      'featureprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'featureprop_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'featureprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featureprop_pub()
 *
 * Purpose: To describe the structure of 'featureprop_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featureprop_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featureprop_pub() {
  $description = [
    'description' => '',
    'table' => 'featureprop_pub',
    'fields' => [
      'featureprop_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featureprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'featureprop_pub_c1' => [
        0 => 'featureprop_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'featureprop_pub_idx1' => [
        0 => 'featureprop_id',
      ],
      'featureprop_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'featureprop_pub_id',
    ],
    'foreign keys' => [
      'featureprop' => [
        'table' => 'featureprop',
        'columns' => [
          'featureprop_id' => 'featureprop_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_pub()
 *
 * Purpose: To describe the structure of 'feature_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_pub() {
  $description = [
    'description' => '',
    'table' => 'feature_pub',
    'fields' => [
      'feature_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_pub_c1' => [
        0 => 'feature_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'feature_pub_idx1' => [
        0 => 'feature_id',
      ],
      'feature_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_pub_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_pubprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_pubprop()
 *
 * Purpose: To describe the structure of 'feature_pubprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_pubprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_pubprop() {
  $description = [
    'description' => '',
    'table' => 'feature_pubprop',
    'fields' => [
      'feature_pubprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'feature_pubprop_c1' => [
        0 => 'feature_pub_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'feature_pubprop_idx1' => [
        0 => 'feature_pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_pubprop_id',
    ],
    'foreign keys' => [
      'feature_pub' => [
        'table' => 'feature_pub',
        'columns' => [
          'feature_pub_id' => 'feature_pub_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_featurerange()
 *
 * Purpose: To describe the structure of 'featurerange' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'featurerange' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_featurerange() {
  $description = [
    'description' => '',
    'table' => 'featurerange',
    'fields' => [
      'featurerange_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'leftstartf_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'leftendf_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'rightstartf_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'rightendf_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rangestr' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'featurerange_idx1' => [
        0 => 'featuremap_id',
      ],
      'featurerange_idx2' => [
        0 => 'feature_id',
      ],
      'featurerange_idx3' => [
        0 => 'leftstartf_id',
      ],
      'featurerange_idx4' => [
        0 => 'leftendf_id',
      ],
      'featurerange_idx5' => [
        0 => 'rightstartf_id',
      ],
      'featurerange_idx6' => [
        0 => 'rightendf_id',
      ],
    ],
    'primary key' => [
      0 => 'featurerange_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
          'leftstartf_id' => 'feature_id',
          'leftendf_id' => 'feature_id',
          'rightstartf_id' => 'feature_id',
          'rightendf_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_relationship()
 *
 * Purpose: To describe the structure of 'feature_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_relationship() {
  $description = [
    'description' => '',
    'table' => 'feature_relationship',
    'fields' => [
      'feature_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'feature_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'feature_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'feature_relationship_idx1b' => [
        0 => 'object_id',
        1 => 'subject_id',
        2 => 'type_id',
      ],
      'feature_relationship_idx2' => [
        0 => 'object_id',
      ],
      'feature_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationship_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'subject_id' => 'feature_id',
          'object_id' => 'feature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_relationshipprop',
      1 => 'feature_relationship_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_relationshipprop()
 *
 * Purpose: To describe the structure of 'feature_relationshipprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_relationshipprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_relationshipprop() {
  $description = [
    'description' => '',
    'table' => 'feature_relationshipprop',
    'fields' => [
      'feature_relationshipprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_relationship_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'feature_relationshipprop_c1' => [
        0 => 'feature_relationship_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'feature_relationshipprop_idx1' => [
        0 => 'feature_relationship_id',
      ],
      'feature_relationshipprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationshipprop_id',
    ],
    'foreign keys' => [
      'feature_relationship' => [
        'table' => 'feature_relationship',
        'columns' => [
          'feature_relationship_id' => 'feature_relationship_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_relationshipprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_relationshipprop_pub()
 *
 * Purpose: To describe the structure of 'feature_relationshipprop_pub' to
 * tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_relationshipprop_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_relationshipprop_pub() {
  $description = [
    'description' => '',
    'table' => 'feature_relationshipprop_pub',
    'fields' => [
      'feature_relationshipprop_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_relationshipprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_relationshipprop_pub_c1' => [
        0 => 'feature_relationshipprop_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'feature_relationshipprop_pub_idx1' => [
        0 => 'feature_relationshipprop_id',
      ],
      'feature_relationshipprop_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationshipprop_pub_id',
    ],
    'foreign keys' => [
      'feature_relationshipprop' => [
        'table' => 'feature_relationshipprop',
        'columns' => [
          'feature_relationshipprop_id' => 'feature_relationshipprop_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_relationship_pub()
 *
 * Purpose: To describe the structure of 'feature_relationship_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_relationship_pub() {
  $description = [
    'description' => '',
    'table' => 'feature_relationship_pub',
    'fields' => [
      'feature_relationship_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_relationship_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'feature_relationship_pub_c1' => [
        0 => 'feature_relationship_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'feature_relationship_pub_idx1' => [
        0 => 'feature_relationship_id',
      ],
      'feature_relationship_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationship_pub_id',
    ],
    'foreign keys' => [
      'feature_relationship' => [
        'table' => 'feature_relationship',
        'columns' => [
          'feature_relationship_id' => 'feature_relationship_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_feature_synonym()
 *
 * Purpose: To describe the structure of 'feature_synonym' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'feature_synonym' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_feature_synonym() {
  $description = [
    'description' => '',
    'table' => 'feature_synonym',
    'fields' => [
      'feature_synonym_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'synonym_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'is_internal' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
    ],
    'unique keys' => [
      'feature_synonym_c1' => [
        0 => 'synonym_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
      'feature_synonym_idx1' => [
        0 => 'synonym_id',
      ],
      'feature_synonym_idx2' => [
        0 => 'feature_id',
      ],
      'feature_synonym_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'feature_synonym_id',
    ],
    'foreign keys' => [
      'synonym' => [
        'table' => 'synonym',
        'columns' => [
          'synonym_id' => 'synonym_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_genotype()
 *
 * Purpose: To describe the structure of 'genotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'genotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_genotype() {
  $description = [
    'description' => '',
    'table' => 'genotype',
    'fields' => [
      'genotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'genotype_c1' => [
        0 => 'uniquename',
      ],
    ],
    'indexes' => [
      'genotype_idx1' => [
        0 => 'uniquename',
      ],
      'genotype_idx2' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'genotype_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_genotype',
      1 => 'genotypeprop',
      2 => 'nd_experiment_genotype',
      3 => 'phendesc',
      4 => 'phenotype_comparison',
      6 => 'phenstatement',
      7 => 'stock_genotype',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_genotypeprop()
 *
 * Purpose: To describe the structure of 'genotypeprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'genotypeprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_genotypeprop() {
  $description = [
    'description' => '',
    'table' => 'genotypeprop',
    'fields' => [
      'genotypeprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'genotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'genotypeprop_c1' => [
        0 => 'genotype_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'genotypeprop_idx1' => [
        0 => 'genotype_id',
      ],
      'genotypeprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'genotypeprop_id',
    ],
    'foreign keys' => [
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library()
 *
 * Purpose: To describe the structure of 'library' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library() {
  $description = [
    'description' => '',
    'table' => 'library',
    'fields' => [
      'library_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_obsolete' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'timeaccessioned' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
      'timelastmodified' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => TRUE,
        'default' => 'now()',
      ],
    ],
    'unique keys' => [
      'library_c1' => [
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'library_contact_idx1' => [
        0 => 'library_id',
      ],
      'library_idx1' => [
        0 => 'organism_id',
      ],
      'library_idx2' => [
        0 => 'type_id',
      ],
      'library_idx3' => [
        0 => 'uniquename',
      ],
      'library_name_ind1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'library_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'cell_line_library',
      1 => 'library_contact',
      2 => 'library_cvterm',
      3 => 'library_dbxref',
      4 => 'library_expression',
      5 => 'library_feature',
      6 => 'libraryprop',
      7 => 'library_pub',
      8 => 'library_relationship',
      10 => 'library_synonym',
      11 => 'stock_library',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_contact()
 *
 * Purpose: To describe the structure of 'library_contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_contact() {
  $description = [
    'description' => '',
    'table' => 'library_contact',
    'fields' => [
      'library_contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_contact_c1' => [
        0 => 'library_id',
        1 => 'contact_id',
      ],
    ],
    'primary key' => [
      0 => 'library_contact_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_cvterm()
 *
 * Purpose: To describe the structure of 'library_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_cvterm() {
  $description = [
    'description' => '',
    'table' => 'library_cvterm',
    'fields' => [
      'library_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_cvterm_c1' => [
        0 => 'library_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
      'library_cvterm_idx1' => [
        0 => 'library_id',
      ],
      'library_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
      'library_cvterm_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'library_cvterm_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_dbxref()
 *
 * Purpose: To describe the structure of 'library_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_dbxref() {
  $description = [
    'description' => '',
    'table' => 'library_dbxref',
    'fields' => [
      'library_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'library_dbxref_c1' => [
        0 => 'library_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'library_dbxref_idx1' => [
        0 => 'library_id',
      ],
      'library_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'library_dbxref_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_expression()
 *
 * Purpose: To describe the structure of 'library_expression' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_expression' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_expression() {
  $description = [
    'description' => '',
    'table' => 'library_expression',
    'fields' => [
      'library_expression_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_expression_c1' => [
        0 => 'library_id',
        1 => 'expression_id',
      ],
    ],
    'indexes' => [
      'library_expression_idx1' => [
        0 => 'library_id',
      ],
      'library_expression_idx2' => [
        0 => 'expression_id',
      ],
      'library_expression_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'library_expression_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'library_expressionprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_expressionprop()
 *
 * Purpose: To describe the structure of 'library_expressionprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_expressionprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_expressionprop() {
  $description = [
    'description' => '',
    'table' => 'library_expressionprop',
    'fields' => [
      'library_expressionprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_expression_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'library_expressionprop_c1' => [
        0 => 'library_expression_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'library_expressionprop_idx1' => [
        0 => 'library_expression_id',
      ],
      'library_expressionprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'library_expressionprop_id',
    ],
    'foreign keys' => [
      'library_expression' => [
        'table' => 'library_expression',
        'columns' => [
          'library_expression_id' => 'library_expression_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_feature()
 *
 * Purpose: To describe the structure of 'library_feature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_feature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_feature() {
  $description = [
    'description' => '',
    'table' => 'library_feature',
    'fields' => [
      'library_feature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_feature_c1' => [
        0 => 'library_id',
        1 => 'feature_id',
      ],
    ],
    'indexes' => [
      'library_feature_idx1' => [
        0 => 'library_id',
      ],
      'library_feature_idx2' => [
        0 => 'feature_id',
      ],
    ],
    'primary key' => [
      0 => 'library_feature_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'library_featureprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_featureprop()
 *
 * Purpose: To describe the structure of 'library_featureprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_featureprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_featureprop() {
  $description = [
    'description' => '',
    'table' => 'library_featureprop',
    'fields' => [
      'library_featureprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'library_featureprop_c1' => [
        0 => 'library_feature_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'library_featureprop_idx1' => [
        0 => 'library_feature_id',
      ],
      'library_featureprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'library_featureprop_id',
    ],
    'foreign keys' => [
      'library_feature' => [
        'table' => 'library_feature',
        'columns' => [
          'library_feature_id' => 'library_feature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_libraryprop()
 *
 * Purpose: To describe the structure of 'libraryprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'libraryprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_libraryprop() {
  $description = [
    'description' => '',
    'table' => 'libraryprop',
    'fields' => [
      'libraryprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'libraryprop_c1' => [
        0 => 'library_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'libraryprop_idx1' => [
        0 => 'library_id',
      ],
      'libraryprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'libraryprop_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'libraryprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_libraryprop_pub()
 *
 * Purpose: To describe the structure of 'libraryprop_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'libraryprop_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_libraryprop_pub() {
  $description = [
    'description' => '',
    'table' => 'libraryprop_pub',
    'fields' => [
      'libraryprop_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'libraryprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'libraryprop_pub_c1' => [
        0 => 'libraryprop_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'libraryprop_pub_idx1' => [
        0 => 'libraryprop_id',
      ],
      'libraryprop_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'libraryprop_pub_id',
    ],
    'foreign keys' => [
      'libraryprop' => [
        'table' => 'libraryprop',
        'columns' => [
          'libraryprop_id' => 'libraryprop_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_pub()
 *
 * Purpose: To describe the structure of 'library_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_pub() {
  $description = [
    'description' => '',
    'table' => 'library_pub',
    'fields' => [
      'library_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_pub_c1' => [
        0 => 'library_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'library_pub_idx1' => [
        0 => 'library_id',
      ],
      'library_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'library_pub_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_relationship()
 *
 * Purpose: To describe the structure of 'library_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_relationship() {
  $description = [
    'description' => '',
    'table' => 'library_relationship',
    'fields' => [
      'library_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'library_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'library_relationship_idx2' => [
        0 => 'object_id',
      ],
      'library_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'library_relationship_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'subject_id' => 'library_id',
          'object_id' => 'library_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'library_relationship_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_relationship_pub()
 *
 * Purpose: To describe the structure of 'library_relationship_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_relationship_pub() {
  $description = [
    'description' => '',
    'table' => 'library_relationship_pub',
    'fields' => [
      'library_relationship_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_relationship_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'library_relationship_pub_c1' => [
        0 => 'library_relationship_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'library_relationship_pub_idx1' => [
        0 => 'library_relationship_id',
      ],
      'library_relationship_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'library_relationship_pub_id',
    ],
    'foreign keys' => [
      'library_relationship' => [
        'table' => 'library_relationship',
        'columns' => [
          'library_relationship_id' => 'library_relationship_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_library_synonym()
 *
 * Purpose: To describe the structure of 'library_synonym' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'library_synonym' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_library_synonym() {
  $description = [
    'description' => '',
    'table' => 'library_synonym',
    'fields' => [
      'library_synonym_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'synonym_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
      'is_internal' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
    ],
    'unique keys' => [
      'library_synonym_c1' => [
        0 => 'synonym_id',
        1 => 'library_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
      'library_synonym_idx1' => [
        0 => 'synonym_id',
      ],
      'library_synonym_idx2' => [
        0 => 'library_id',
      ],
      'library_synonym_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'library_synonym_id',
    ],
    'foreign keys' => [
      'synonym' => [
        'table' => 'synonym',
        'columns' => [
          'synonym_id' => 'synonym_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_magedocumentation()
 *
 * Purpose: To describe the structure of 'magedocumentation' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'magedocumentation' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_magedocumentation() {
  $description = [
    'description' => '',
    'table' => 'magedocumentation',
    'fields' => [
      'magedocumentation_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'mageml_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'tableinfo_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'row_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'mageidentifier' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'magedocumentation_idx1' => [
        0 => 'mageml_id',
      ],
      'magedocumentation_idx2' => [
        0 => 'tableinfo_id',
      ],
      'magedocumentation_idx3' => [
        0 => 'row_id',
      ],
    ],
    'primary key' => [
      0 => 'magedocumentation_id',
    ],
    'foreign keys' => [
      'mageml' => [
        'table' => 'mageml',
        'columns' => [
          'mageml_id' => 'mageml_id',
        ],
      ],
      'tableinfo' => [
        'table' => 'tableinfo',
        'columns' => [
          'tableinfo_id' => 'tableinfo_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_mageml()
 *
 * Purpose: To describe the structure of 'mageml' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'mageml' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_mageml() {
  $description = [
    'description' => '',
    'table' => 'mageml',
    'fields' => [
      'mageml_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'mage_package' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'mage_ml' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
    ],
    'primary key' => [
      0 => 'mageml_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'magedocumentation',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_materialized_view()
 *
 * Purpose: To describe the structure of 'materialized_view' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'materialized_view' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_materialized_view() {
  $description = [
    'description' => '',
    'table' => 'materialized_view',
    'fields' => [
      'materialized_view_id' => [
        'size' => 'normal',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'last_update' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => FALSE,
      ],
      'refresh_time' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '64',
        'not null' => FALSE,
      ],
      'mv_schema' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '64',
        'not null' => FALSE,
      ],
      'mv_table' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '128',
        'not null' => FALSE,
      ],
      'mv_specs' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'indexed' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'query' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'special_index' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'materialized_view_name_key' => [
        0 => 'name',
      ],
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment()
 *
 * Purpose: To describe the structure of 'nd_experiment' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment',
    'fields' => [
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_geolocation_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_experiment_idx1' => [
        0 => 'nd_geolocation_id',
      ],
      'nd_experiment_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_id',
    ],
    'foreign keys' => [
      'nd_geolocation' => [
        'table' => 'nd_geolocation',
        'columns' => [
          'nd_geolocation_id' => 'nd_geolocation_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'nd_experiment_analysis',
      1 => 'nd_experiment_contact',
      2 => 'nd_experiment_dbxref',
      3 => 'nd_experiment_genotype',
      4 => 'nd_experiment_phenotype',
      5 => 'nd_experiment_project',
      6 => 'nd_experimentprop',
      7 => 'nd_experiment_protocol',
      8 => 'nd_experiment_pub',
      9 => 'nd_experiment_stock',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_analysis()
 *
 * Purpose: To describe the structure of 'nd_experiment_analysis' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_analysis' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_analysis() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_analysis',
    'fields' => [
      'nd_experiment_analysis_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'nd_experiment_analysis_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_analysis_idx2' => [
        0 => 'analysis_id',
      ],
      'nd_experiment_analysis_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_analysis_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_contact()
 *
 * Purpose: To describe the structure of 'nd_experiment_contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_contact() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_contact',
    'fields' => [
      'nd_experiment_contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_experiment_contact_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_contact_idx2' => [
        0 => 'contact_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_contact_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_dbxref()
 *
 * Purpose: To describe the structure of 'nd_experiment_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_dbxref() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_dbxref',
    'fields' => [
      'nd_experiment_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_experiment_dbxref_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_dbxref_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_genotype()
 *
 * Purpose: To describe the structure of 'nd_experiment_genotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_genotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_genotype() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_genotype',
    'fields' => [
      'nd_experiment_genotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'genotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'nd_experiment_genotype_c1' => [
        0 => 'nd_experiment_id',
        1 => 'genotype_id',
      ],
    ],
    'indexes' => [
      'nd_experiment_genotype_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_genotype_idx2' => [
        0 => 'genotype_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_genotype_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_phenotype()
 *
 * Purpose: To describe the structure of 'nd_experiment_phenotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_phenotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_phenotype() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_phenotype',
    'fields' => [
      'nd_experiment_phenotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'phenotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'nd_experiment_phenotype_c1' => [
        0 => 'nd_experiment_id',
        1 => 'phenotype_id',
      ],
    ],
    'indexes' => [
      'nd_experiment_phenotype_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_phenotype_idx2' => [
        0 => 'phenotype_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_phenotype_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_project()
 *
 * Purpose: To describe the structure of 'nd_experiment_project' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_project' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_project() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_project',
    'fields' => [
      'nd_experiment_project_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'nd_experiment_project_c1' => [
        0 => 'project_id',
        1 => 'nd_experiment_id',
      ],
    ],
    'indexes' => [
      'nd_experiment_project_idx1' => [
        0 => 'project_id',
      ],
      'nd_experiment_project_idx2' => [
        0 => 'nd_experiment_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_project_id',
    ],
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experimentprop()
 *
 * Purpose: To describe the structure of 'nd_experimentprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experimentprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experimentprop() {
  $description = [
    'description' => '',
    'table' => 'nd_experimentprop',
    'fields' => [
      'nd_experimentprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'nd_experimentprop_c1' => [
        0 => 'nd_experiment_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'nd_experimentprop_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experimentprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experimentprop_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_protocol()
 *
 * Purpose: To describe the structure of 'nd_experiment_protocol' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_protocol' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_protocol() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_protocol',
    'fields' => [
      'nd_experiment_protocol_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'nd_protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_experiment_protocol_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_protocol_idx2' => [
        0 => 'nd_protocol_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_protocol_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'nd_protocol' => [
        'table' => 'nd_protocol',
        'columns' => [
          'nd_protocol_id' => 'nd_protocol_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_pub()
 *
 * Purpose: To describe the structure of 'nd_experiment_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_pub() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_pub',
    'fields' => [
      'nd_experiment_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'nd_experiment_pub_c1' => [
        0 => 'nd_experiment_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'nd_experiment_pub_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_pub_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_stock()
 *
 * Purpose: To describe the structure of 'nd_experiment_stock' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_stock' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_stock() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_stock',
    'fields' => [
      'nd_experiment_stock_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_experiment_stock_idx1' => [
        0 => 'nd_experiment_id',
      ],
      'nd_experiment_stock_idx2' => [
        0 => 'stock_id',
      ],
      'nd_experiment_stock_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_stock_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'nd_experiment_stock_dbxref',
      1 => 'nd_experiment_stockprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_stock_dbxref()
 *
 * Purpose: To describe the structure of 'nd_experiment_stock_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_stock_dbxref() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_stock_dbxref',
    'fields' => [
      'nd_experiment_stock_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_experiment_stock_dbxref_idx1' => [
        0 => 'nd_experiment_stock_id',
      ],
      'nd_experiment_stock_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_stock_dbxref_id',
    ],
    'foreign keys' => [
      'nd_experiment_stock' => [
        'table' => 'nd_experiment_stock',
        'columns' => [
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_experiment_stockprop()
 *
 * Purpose: To describe the structure of 'nd_experiment_stockprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_experiment_stockprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_experiment_stockprop() {
  $description = [
    'description' => '',
    'table' => 'nd_experiment_stockprop',
    'fields' => [
      'nd_experiment_stockprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_experiment_stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'nd_experiment_stockprop_c1' => [
        0 => 'nd_experiment_stock_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'nd_experiment_stockprop_idx1' => [
        0 => 'nd_experiment_stock_id',
      ],
      'nd_experiment_stockprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_experiment_stockprop_id',
    ],
    'foreign keys' => [
      'nd_experiment_stock' => [
        'table' => 'nd_experiment_stock',
        'columns' => [
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_geolocation()
 *
 * Purpose: To describe the structure of 'nd_geolocation' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_geolocation' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_geolocation() {
  $description = [
    'description' => '',
    'table' => 'nd_geolocation',
    'fields' => [
      'nd_geolocation_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'latitude' => [
        'size' => 'normal',
        'type' => 'float',
        'not null' => FALSE,
      ],
      'longitude' => [
        'size' => 'normal',
        'type' => 'float',
        'not null' => FALSE,
      ],
      'geodetic_datum' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '32',
        'not null' => FALSE,
      ],
      'altitude' => [
        'size' => 'normal',
        'type' => 'float',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'nd_geolocation_idx1' => [
        0 => 'latitude',
      ],
      'nd_geolocation_idx2' => [
        0 => 'longitude',
      ],
      'nd_geolocation_idx3' => [
        0 => 'altitude',
      ],
    ],
    'primary key' => [
      0 => 'nd_geolocation_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'nd_experiment',
      1 => 'nd_geolocationprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_geolocationprop()
 *
 * Purpose: To describe the structure of 'nd_geolocationprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_geolocationprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_geolocationprop() {
  $description = [
    'description' => '',
    'table' => 'nd_geolocationprop',
    'fields' => [
      'nd_geolocationprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_geolocation_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'nd_geolocationprop_c1' => [
        0 => 'nd_geolocation_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'nd_geolocationprop_idx1' => [
        0 => 'nd_geolocation_id',
      ],
      'nd_geolocationprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_geolocationprop_id',
    ],
    'foreign keys' => [
      'nd_geolocation' => [
        'table' => 'nd_geolocation',
        'columns' => [
          'nd_geolocation_id' => 'nd_geolocation_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_protocol()
 *
 * Purpose: To describe the structure of 'nd_protocol' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_protocol' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_protocol() {
  $description = [
    'description' => '',
    'table' => 'nd_protocol',
    'fields' => [
      'nd_protocol_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_protocol_idx1' => [
        0 => 'type_id',
      ],
    ],
    'unique keys' => [
      'nd_protocol_name_key' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'nd_protocol_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'nd_experiment_protocol',
      1 => 'nd_protocolprop',
      2 => 'nd_protocol_reagent',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_protocolprop()
 *
 * Purpose: To describe the structure of 'nd_protocolprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_protocolprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_protocolprop() {
  $description = [
    'description' => '',
    'table' => 'nd_protocolprop',
    'fields' => [
      'nd_protocolprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'nd_protocolprop_c1' => [
        0 => 'nd_protocol_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'nd_protocolprop_idx1' => [
        0 => 'nd_protocol_id',
      ],
      'nd_protocolprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_protocolprop_id',
    ],
    'foreign keys' => [
      'nd_protocol' => [
        'table' => 'nd_protocol',
        'columns' => [
          'nd_protocol_id' => 'nd_protocol_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_protocol_reagent()
 *
 * Purpose: To describe the structure of 'nd_protocol_reagent' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_protocol_reagent' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_protocol_reagent() {
  $description = [
    'description' => '',
    'table' => 'nd_protocol_reagent',
    'fields' => [
      'nd_protocol_reagent_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'reagent_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_protocol_reagent_idx1' => [
        0 => 'nd_protocol_id',
      ],
      'nd_protocol_reagent_idx2' => [
        0 => 'reagent_id',
      ],
      'nd_protocol_reagent_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_protocol_reagent_id',
    ],
    'foreign keys' => [
      'nd_protocol' => [
        'table' => 'nd_protocol',
        'columns' => [
          'nd_protocol_id' => 'nd_protocol_id',
        ],
      ],
      'nd_reagent' => [
        'table' => 'nd_reagent',
        'columns' => [
          'reagent_id' => 'nd_reagent_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_reagent()
 *
 * Purpose: To describe the structure of 'nd_reagent' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_reagent' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_reagent() {
  $description = [
    'description' => '',
    'table' => 'nd_reagent',
    'fields' => [
      'nd_reagent_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '80',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'nd_reagent_idx1' => [
        0 => 'type_id',
      ],
      'nd_reagent_idx2' => [
        0 => 'feature_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_reagent_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'nd_protocol_reagent',
      1 => 'nd_reagentprop',
      2 => 'nd_reagent_relationship',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_reagentprop()
 *
 * Purpose: To describe the structure of 'nd_reagentprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_reagentprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_reagentprop() {
  $description = [
    'description' => '',
    'table' => 'nd_reagentprop',
    'fields' => [
      'nd_reagentprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'nd_reagent_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'nd_reagentprop_c1' => [
        0 => 'nd_reagent_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'nd_reagentprop_idx1' => [
        0 => 'nd_reagent_id',
      ],
      'nd_reagentprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_reagentprop_id',
    ],
    'foreign keys' => [
      'nd_reagent' => [
        'table' => 'nd_reagent',
        'columns' => [
          'nd_reagent_id' => 'nd_reagent_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_nd_reagent_relationship()
 *
 * Purpose: To describe the structure of 'nd_reagent_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'nd_reagent_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_nd_reagent_relationship() {
  $description = [
    'description' => '',
    'table' => 'nd_reagent_relationship',
    'fields' => [
      'nd_reagent_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_reagent_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_reagent_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'nd_reagent_relationship_idx1' => [
        0 => 'subject_reagent_id',
      ],
      'nd_reagent_relationship_idx2' => [
        0 => 'object_reagent_id',
      ],
      'nd_reagent_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'nd_reagent_relationship_id',
    ],
    'foreign keys' => [
      'nd_reagent' => [
        'table' => 'nd_reagent',
        'columns' => [
          'subject_reagent_id' => 'nd_reagent_id',
          'object_reagent_id' => 'nd_reagent_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism()
 *
 * Purpose: To describe the structure of 'organism' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism() {
  $description = [
    'description' => '',
    'table' => 'organism',
    'fields' => [
      'organism_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'abbreviation' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'genus' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'species' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'common_name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'infraspecific_name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => FALSE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'comment' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'organism_c1' => [
        0 => 'genus',
        1 => 'species',
        2 => 'type_id',
        3 => 'infraspecific_name',
      ],
    ],
    'primary key' => [
      0 => 'organism_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'analysis_organism',
      1 => 'biomaterial',
      2 => 'cell_line',
      3 => 'feature',
      4 => 'featuremap_organism',
      5 => 'library',
      6 => 'organism_cvterm',
      7 => 'organism_dbxref',
      8 => 'organismprop',
      9 => 'organism_pub',
      10 => 'organism_relationship',
      12 => 'phenotype_comparison',
      13 => 'phylonode_organism',
      14 => 'stock',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism_cvterm()
 *
 * Purpose: To describe the structure of 'organism_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism_cvterm() {
  $description = [
    'description' => '',
    'table' => 'organism_cvterm',
    'fields' => [
      'organism_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'organism_cvterm_c1' => [
        0 => 'organism_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
      'organism_cvterm_idx1' => [
        0 => 'organism_id',
      ],
      'organism_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
    ],
    'primary key' => [
      0 => 'organism_cvterm_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'organism_cvtermprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism_cvtermprop()
 *
 * Purpose: To describe the structure of 'organism_cvtermprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism_cvtermprop() {
  $description = [
    'description' => '',
    'table' => 'organism_cvtermprop',
    'fields' => [
      'organism_cvtermprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organism_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'organism_cvtermprop_c1' => [
        0 => 'organism_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'organism_cvtermprop_idx1' => [
        0 => 'organism_cvterm_id',
      ],
      'organism_cvtermprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'organism_cvtermprop_id',
    ],
    'foreign keys' => [
      'organism_cvterm' => [
        'table' => 'organism_cvterm',
        'columns' => [
          'organism_cvterm_id' => 'organism_cvterm_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism_dbxref()
 *
 * Purpose: To describe the structure of 'organism_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism_dbxref() {
  $description = [
    'description' => '',
    'table' => 'organism_dbxref',
    'fields' => [
      'organism_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'organism_dbxref_c1' => [
        0 => 'organism_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'organism_dbxref_idx1' => [
        0 => 'organism_id',
      ],
      'organism_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'organism_dbxref_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism_feature_count()
 *
 * Purpose: To describe the structure of 'organism_feature_count' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism_feature_count' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism_feature_count() {
  $description = [
    'description' => '',
    'table' => 'organism_feature_count',
    'fields' => [
      'organism_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'genus' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'species' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'common_name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'num_features' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_type' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'organism_feature_count_idx1' => [
        0 => 'organism_id',
      ],
      'organism_feature_count_idx2' => [
        0 => 'cvterm_id',
      ],
      'organism_feature_count_idx3' => [
        0 => 'feature_type',
      ],
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organismprop()
 *
 * Purpose: To describe the structure of 'organismprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organismprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organismprop() {
  $description = [
    'description' => '',
    'table' => 'organismprop',
    'fields' => [
      'organismprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'organismprop_c1' => [
        0 => 'organism_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'organismprop_idx1' => [
        0 => 'organism_id',
      ],
      'organismprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'organismprop_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'organismprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organismprop_pub()
 *
 * Purpose: To describe the structure of 'organismprop_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organismprop_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organismprop_pub() {
  $description = [
    'description' => '',
    'table' => 'organismprop_pub',
    'fields' => [
      'organismprop_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organismprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'organismprop_pub_c1' => [
        0 => 'organismprop_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'organismprop_pub_idx1' => [
        0 => 'organismprop_id',
      ],
      'organismprop_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'organismprop_pub_id',
    ],
    'foreign keys' => [
      'organismprop' => [
        'table' => 'organismprop',
        'columns' => [
          'organismprop_id' => 'organismprop_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism_pub()
 *
 * Purpose: To describe the structure of 'organism_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism_pub() {
  $description = [
    'description' => '',
    'table' => 'organism_pub',
    'fields' => [
      'organism_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'organism_pub_c1' => [
        0 => 'organism_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'organism_pub_idx1' => [
        0 => 'organism_id',
      ],
      'organism_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'organism_pub_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_organism_relationship()
 *
 * Purpose: To describe the structure of 'organism_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'organism_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_organism_relationship() {
  $description = [
    'description' => '',
    'table' => 'organism_relationship',
    'fields' => [
      'organism_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'organism_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'organism_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'organism_relationship_idx2' => [
        0 => 'object_id',
      ],
      'organism_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'organism_relationship_id',
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'object_id' => 'organism_id',
          'subject_id' => 'organism_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phendesc()
 *
 * Purpose: To describe the structure of 'phendesc' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phendesc' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phendesc() {
  $description = [
    'description' => '',
    'table' => 'phendesc',
    'fields' => [
      'phendesc_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'genotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'environment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'phendesc_c1' => [
        0 => 'genotype_id',
        1 => 'environment_id',
        2 => 'type_id',
        3 => 'pub_id',
      ],
    ],
    'indexes' => [
      'phendesc_idx1' => [
        0 => 'genotype_id',
      ],
      'phendesc_idx2' => [
        0 => 'environment_id',
      ],
      'phendesc_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'phendesc_id',
    ],
    'foreign keys' => [
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
      'environment' => [
        'table' => 'environment',
        'columns' => [
          'environment_id' => 'environment_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phenotype()
 *
 * Purpose: To describe the structure of 'phenotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phenotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phenotype() {
  $description = [
    'description' => '',
    'table' => 'phenotype',
    'fields' => [
      'phenotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'observable_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'attr_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'cvalue_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'phenotype_c1' => [
        0 => 'uniquename',
      ],
    ],
    'indexes' => [
      'phenotype_idx1' => [
        0 => 'cvalue_id',
      ],
      'phenotype_idx2' => [
        0 => 'observable_id',
      ],
      'phenotype_idx3' => [
        0 => 'attr_id',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'observable_id' => 'cvterm_id',
          'attr_id' => 'cvterm_id',
          'cvalue_id' => 'cvterm_id',
          'assay_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'feature_phenotype',
      1 => 'nd_experiment_phenotype',
      2 => 'phenotype_comparison',
      4 => 'phenotype_cvterm',
      5 => 'phenotypeprop',
      6 => 'phenstatement',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phenotype_comparison()
 *
 * Purpose: To describe the structure of 'phenotype_comparison' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phenotype_comparison' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phenotype_comparison() {
  $description = [
    'description' => '',
    'table' => 'phenotype_comparison',
    'fields' => [
      'phenotype_comparison_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'genotype1_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'environment1_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'genotype2_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'environment2_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'phenotype1_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'phenotype2_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'phenotype_comparison_c1' => [
        0 => 'genotype1_id',
        1 => 'environment1_id',
        2 => 'genotype2_id',
        3 => 'environment2_id',
        4 => 'phenotype1_id',
        5 => 'pub_id',
      ],
    ],
    'indexes' => [
      'phenotype_comparison_idx1' => [
        0 => 'genotype1_id',
      ],
      'phenotype_comparison_idx2' => [
        0 => 'genotype2_id',
      ],
      'phenotype_comparison_idx4' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_comparison_id',
    ],
    'foreign keys' => [
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype1_id' => 'genotype_id',
          'genotype2_id' => 'genotype_id',
        ],
      ],
      'environment' => [
        'table' => 'environment',
        'columns' => [
          'environment1_id' => 'environment_id',
          'environment2_id' => 'environment_id',
        ],
      ],
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype1_id' => 'phenotype_id',
          'phenotype2_id' => 'phenotype_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'phenotype_comparison_cvterm',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phenotype_comparison_cvterm()
 *
 * Purpose: To describe the structure of 'phenotype_comparison_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phenotype_comparison_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phenotype_comparison_cvterm() {
  $description = [
    'description' => '',
    'table' => 'phenotype_comparison_cvterm',
    'fields' => [
      'phenotype_comparison_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phenotype_comparison_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'phenotype_comparison_cvterm_c1' => [
        0 => 'phenotype_comparison_id',
        1 => 'cvterm_id',
      ],
    ],
    'indexes' => [
      'phenotype_comparison_cvterm_idx1' => [
        0 => 'phenotype_comparison_id',
      ],
      'phenotype_comparison_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_comparison_cvterm_id',
    ],
    'foreign keys' => [
      'phenotype_comparison' => [
        'table' => 'phenotype_comparison',
        'columns' => [
          'phenotype_comparison_id' => 'phenotype_comparison_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phenotype_cvterm()
 *
 * Purpose: To describe the structure of 'phenotype_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phenotype_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phenotype_cvterm() {
  $description = [
    'description' => '',
    'table' => 'phenotype_cvterm',
    'fields' => [
      'phenotype_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phenotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'phenotype_cvterm_c1' => [
        0 => 'phenotype_id',
        1 => 'cvterm_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'phenotype_cvterm_idx1' => [
        0 => 'phenotype_id',
      ],
      'phenotype_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_cvterm_id',
    ],
    'foreign keys' => [
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phenotypeprop()
 *
 * Purpose: To describe the structure of 'phenotypeprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phenotypeprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phenotypeprop() {
  $description = [
    'description' => '',
    'table' => 'phenotypeprop',
    'fields' => [
      'phenotypeprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phenotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'phenotypeprop_c1' => [
        0 => 'phenotype_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'phenotypeprop_idx1' => [
        0 => 'phenotype_id',
      ],
      'phenotypeprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'phenotypeprop_id',
    ],
    'foreign keys' => [
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phenstatement()
 *
 * Purpose: To describe the structure of 'phenstatement' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phenstatement' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phenstatement() {
  $description = [
    'description' => '',
    'table' => 'phenstatement',
    'fields' => [
      'phenstatement_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'genotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'environment_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'phenotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'phenstatement_c1' => [
        0 => 'genotype_id',
        1 => 'phenotype_id',
        2 => 'environment_id',
        3 => 'type_id',
        4 => 'pub_id',
      ],
    ],
    'indexes' => [
      'phenstatement_idx1' => [
        0 => 'genotype_id',
      ],
      'phenstatement_idx2' => [
        0 => 'phenotype_id',
      ],
    ],
    'primary key' => [
      0 => 'phenstatement_id',
    ],
    'foreign keys' => [
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
      'environment' => [
        'table' => 'environment',
        'columns' => [
          'environment_id' => 'environment_id',
        ],
      ],
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylonode()
 *
 * Purpose: To describe the structure of 'phylonode' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylonode' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylonode() {
  $description = [
    'description' => '',
    'table' => 'phylonode',
    'fields' => [
      'phylonode_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylotree_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'parent_phylonode_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'left_idx' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'right_idx' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'label' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'distance' => [
        'size' => 'big',
        'type' => 'float',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'parent_phylonode_id' => [
        0 => 'parent_phylonode_id',
      ],
    ],
    'unique keys' => [
      'phylonode_phylotree_id_left_idx_key' => [
        0 => 'phylotree_id',
        1 => 'left_idx',
      ],
      'phylonode_phylotree_id_right_idx_key' => [
        0 => 'phylotree_id',
        1 => 'right_idx',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_id',
    ],
    'foreign keys' => [
      'phylotree' => [
        'table' => 'phylotree',
        'columns' => [
          'phylotree_id' => 'phylotree_id',
        ],
      ],
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'parent_phylonode_id' => 'phylonode_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'phylonode',
      1 => 'phylonode_dbxref',
      2 => 'phylonode_organism',
      3 => 'phylonodeprop',
      4 => 'phylonode_pub',
      5 => 'phylonode_relationship',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylonode_dbxref()
 *
 * Purpose: To describe the structure of 'phylonode_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylonode_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylonode_dbxref() {
  $description = [
    'description' => '',
    'table' => 'phylonode_dbxref',
    'fields' => [
      'phylonode_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylonode_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'phylonode_dbxref_idx1' => [
        0 => 'phylonode_id',
      ],
      'phylonode_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'unique keys' => [
      'phylonode_dbxref_phylonode_id_dbxref_id_key' => [
        0 => 'phylonode_id',
        1 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_dbxref_id',
    ],
    'foreign keys' => [
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylonode_organism()
 *
 * Purpose: To describe the structure of 'phylonode_organism' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylonode_organism' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylonode_organism() {
  $description = [
    'description' => '',
    'table' => 'phylonode_organism',
    'fields' => [
      'phylonode_organism_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylonode_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'phylonode_organism_idx1' => [
        0 => 'phylonode_id',
      ],
      'phylonode_organism_idx2' => [
        0 => 'organism_id',
      ],
    ],
    'unique keys' => [
      'phylonode_organism_phylonode_id_key' => [
        0 => 'phylonode_id',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_organism_id',
    ],
    'foreign keys' => [
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylonodeprop()
 *
 * Purpose: To describe the structure of 'phylonodeprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylonodeprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylonodeprop() {
  $description = [
    'description' => '',
    'table' => 'phylonodeprop',
    'fields' => [
      'phylonodeprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylonode_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
        'default' => '',
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'indexes' => [
      'phylonodeprop_idx1' => [
        0 => 'phylonode_id',
      ],
      'phylonodeprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'unique keys' => [
      'phylonodeprop_phylonode_id_type_id_value_rank_key' => [
        0 => 'phylonode_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'phylonodeprop_id',
    ],
    'foreign keys' => [
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylonode_pub()
 *
 * Purpose: To describe the structure of 'phylonode_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylonode_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylonode_pub() {
  $description = [
    'description' => '',
    'table' => 'phylonode_pub',
    'fields' => [
      'phylonode_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylonode_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'phylonode_pub_idx1' => [
        0 => 'phylonode_id',
      ],
      'phylonode_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'unique keys' => [
      'phylonode_pub_phylonode_id_pub_id_key' => [
        0 => 'phylonode_id',
        1 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_pub_id',
    ],
    'foreign keys' => [
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylonode_relationship()
 *
 * Purpose: To describe the structure of 'phylonode_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylonode_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylonode_relationship() {
  $description = [
    'description' => '',
    'table' => 'phylonode_relationship',
    'fields' => [
      'phylonode_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'phylotree_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'phylonode_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'phylonode_relationship_idx2' => [
        0 => 'object_id',
      ],
      'phylonode_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_relationship_id',
    ],
    'unique keys' => [
      'phylonode_relationship_subject_id_object_id_type_id_key' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'foreign keys' => [
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'subject_id' => 'phylonode_id',
          'object_id' => 'phylonode_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'phylotree' => [
        'table' => 'phylotree',
        'columns' => [
          'phylotree_id' => 'phylotree_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylotree()
 *
 * Purpose: To describe the structure of 'phylotree' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylotree' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylotree() {
  $description = [
    'description' => '',
    'table' => 'phylotree',
    'fields' => [
      'phylotree_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'comment' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'phylotree_idx1' => [
        0 => 'phylotree_id',
      ],
    ],
    'primary key' => [
      0 => 'phylotree_id',
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'phylonode',
      1 => 'phylonode_relationship',
      2 => 'phylotreeprop',
      3 => 'phylotree_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylotreeprop()
 *
 * Purpose: To describe the structure of 'phylotreeprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylotreeprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylotreeprop() {
  $description = [
    'description' => '',
    'table' => 'phylotreeprop',
    'fields' => [
      'phylotreeprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylotree_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'phylotreeprop_c1' => [
        0 => 'phylotree_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'phylotreeprop_idx1' => [
        0 => 'phylotree_id',
      ],
      'phylotreeprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'phylotreeprop_id',
    ],
    'foreign keys' => [
      'phylotree' => [
        'table' => 'phylotree',
        'columns' => [
          'phylotree_id' => 'phylotree_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_phylotree_pub()
 *
 * Purpose: To describe the structure of 'phylotree_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'phylotree_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_phylotree_pub() {
  $description = [
    'description' => '',
    'table' => 'phylotree_pub',
    'fields' => [
      'phylotree_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'phylotree_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'phylotree_pub_idx1' => [
        0 => 'phylotree_id',
      ],
      'phylotree_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'unique keys' => [
      'phylotree_pub_phylotree_id_pub_id_key' => [
        0 => 'phylotree_id',
        1 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'phylotree_pub_id',
    ],
    'foreign keys' => [
      'phylotree' => [
        'table' => 'phylotree',
        'columns' => [
          'phylotree_id' => 'phylotree_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project()
 *
 * Purpose: To describe the structure of 'project' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project() {
  $description = [
    'description' => '',
    'table' => 'project',
    'fields' => [
      'project_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'project_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'project_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'assay_project',
      1 => 'nd_experiment_project',
      2 => 'project_analysis',
      3 => 'project_contact',
      4 => 'project_dbxref',
      5 => 'project_feature',
      6 => 'projectprop',
      7 => 'project_pub',
      8 => 'project_relationship',
      10 => 'project_stock',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_analysis()
 *
 * Purpose: To describe the structure of 'project_analysis' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_analysis' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_analysis() {
  $description = [
    'description' => '',
    'table' => 'project_analysis',
    'fields' => [
      'project_analysis_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'project_analysis_c1' => [
        0 => 'project_id',
        1 => 'analysis_id',
      ],
    ],
    'indexes' => [
      'project_analysis_idx1' => [
        0 => 'project_id',
      ],
      'project_analysis_idx2' => [
        0 => 'analysis_id',
      ],
    ],
    'primary key' => [
      0 => 'project_analysis_id',
    ],
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_contact()
 *
 * Purpose: To describe the structure of 'project_contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_contact() {
  $description = [
    'description' => '',
    'table' => 'project_contact',
    'fields' => [
      'project_contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'project_contact_c1' => [
        0 => 'project_id',
        1 => 'contact_id',
      ],
    ],
    'indexes' => [
      'project_contact_idx1' => [
        0 => 'project_id',
      ],
      'project_contact_idx2' => [
        0 => 'contact_id',
      ],
    ],
    'primary key' => [
      0 => 'project_contact_id',
    ],
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_dbxref()
 *
 * Purpose: To describe the structure of 'project_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_dbxref() {
  $description = [
    'description' => '',
    'table' => 'project_dbxref',
    'fields' => [
      'project_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'project_dbxref_c1' => [
        0 => 'project_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'project_dbxref_idx1' => [
        0 => 'project_id',
      ],
      'project_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'project_dbxref_id',
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_feature()
 *
 * Purpose: To describe the structure of 'project_feature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_feature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_feature() {
  $description = [
    'description' => '',
    'table' => 'project_feature',
    'fields' => [
      'project_feature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'project_feature_c1' => [
        0 => 'feature_id',
        1 => 'project_id',
      ],
    ],
    'indexes' => [
      'project_feature_idx1' => [
        0 => 'feature_id',
      ],
      'project_feature_idx2' => [
        0 => 'project_id',
      ],
    ],
    'primary key' => [
      0 => 'project_feature_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_projectprop()
 *
 * Purpose: To describe the structure of 'projectprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'projectprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_projectprop() {
  $description = [
    'description' => '',
    'table' => 'projectprop',
    'fields' => [
      'projectprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'projectprop_c1' => [
        0 => 'project_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'primary key' => [
      0 => 'projectprop_id',
    ],
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_pub()
 *
 * Purpose: To describe the structure of 'project_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_pub() {
  $description = [
    'description' => '',
    'table' => 'project_pub',
    'fields' => [
      'project_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'project_pub_c1' => [
        0 => 'project_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'project_pub_idx1' => [
        0 => 'project_id',
      ],
      'project_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'project_pub_id',
    ],
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_relationship()
 *
 * Purpose: To describe the structure of 'project_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_relationship() {
  $description = [
    'description' => '',
    'table' => 'project_relationship',
    'fields' => [
      'project_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'project_relationship_c1' => [
        0 => 'subject_project_id',
        1 => 'object_project_id',
        2 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'project_relationship_id',
    ],
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'subject_project_id' => 'project_id',
          'object_project_id' => 'project_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_project_stock()
 *
 * Purpose: To describe the structure of 'project_stock' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'project_stock' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_project_stock() {
  $description = [
    'description' => '',
    'table' => 'project_stock',
    'fields' => [
      'project_stock_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'project_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'project_stock_c1' => [
        0 => 'stock_id',
        1 => 'project_id',
      ],
    ],
    'indexes' => [
      'project_stock_idx1' => [
        0 => 'stock_id',
      ],
      'project_stock_idx2' => [
        0 => 'project_id',
      ],
    ],
    'primary key' => [
      0 => 'project_stock_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_protocol()
 *
 * Purpose: To describe the structure of 'protocol' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'protocol' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_protocol() {
  $description = [
    'description' => '',
    'table' => 'protocol',
    'fields' => [
      'protocol_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'uri' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'protocoldescription' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'hardwaredescription' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'softwaredescription' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'protocol_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
      'protocol_idx1' => [
        0 => 'type_id',
      ],
      'protocol_idx2' => [
        0 => 'pub_id',
      ],
      'protocol_idx3' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'protocol_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'acquisition',
      1 => 'arraydesign',
      2 => 'assay',
      3 => 'protocolparam',
      4 => 'quantification',
      5 => 'treatment',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_protocolparam()
 *
 * Purpose: To describe the structure of 'protocolparam' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'protocolparam' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_protocolparam() {
  $description = [
    'description' => '',
    'table' => 'protocolparam',
    'fields' => [
      'protocolparam_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'datatype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'unittype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'indexes' => [
      'protocolparam_idx1' => [
        0 => 'protocol_id',
      ],
      'protocolparam_idx2' => [
        0 => 'datatype_id',
      ],
      'protocolparam_idx3' => [
        0 => 'unittype_id',
      ],
    ],
    'primary key' => [
      0 => 'protocolparam_id',
    ],
    'foreign keys' => [
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'datatype_id' => 'cvterm_id',
          'unittype_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_pub()
 *
 * Purpose: To describe the structure of 'pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_pub() {
  $description = [
    'description' => '',
    'table' => 'pub',
    'fields' => [
      'pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'title' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'volumetitle' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'volume' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'series_name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'issue' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'pyear' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'pages' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'miniref' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_obsolete' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => FALSE,
        'default' => FALSE,
      ],
      'publisher' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'pubplace' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'pub_c1' => [
        0 => 'uniquename',
      ],
    ],
    'indexes' => [
      'pub_idx1' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'pub_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'analysis_pub',
      1 => 'cell_line_cvterm',
      2 => 'cell_line_feature',
      3 => 'cell_line_library',
      4 => 'cell_lineprop_pub',
      5 => 'cell_line_pub',
      6 => 'cell_line_synonym',
      7 => 'expression_pub',
      8 => 'feature_cvterm',
      9 => 'feature_cvterm_pub',
      10 => 'feature_expression',
      11 => 'featureloc_pub',
      12 => 'featuremap_pub',
      13 => 'featureprop_pub',
      14 => 'feature_pub',
      15 => 'feature_relationshipprop_pub',
      16 => 'feature_relationship_pub',
      17 => 'feature_synonym',
      18 => 'library_cvterm',
      19 => 'library_expression',
      20 => 'libraryprop_pub',
      21 => 'library_pub',
      22 => 'library_relationship_pub',
      23 => 'library_synonym',
      24 => 'nd_experiment_pub',
      25 => 'organism_cvterm',
      26 => 'organismprop_pub',
      27 => 'organism_pub',
      28 => 'phendesc',
      29 => 'phenotype_comparison',
      30 => 'phenotype_comparison_cvterm',
      31 => 'phenstatement',
      32 => 'phylonode_pub',
      33 => 'phylotree_pub',
      34 => 'project_pub',
      35 => 'protocol',
      36 => 'pubauthor',
      37 => 'pub_dbxref',
      38 => 'pubprop',
      39 => 'pub_relationship',
      41 => 'stock_cvterm',
      42 => 'stockprop_pub',
      43 => 'stock_pub',
      44 => 'stock_relationship_cvterm',
      45 => 'stock_relationship_pub',
      46 => 'study',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_pubauthor()
 *
 * Purpose: To describe the structure of 'pubauthor' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'pubauthor' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_pubauthor() {
  $description = [
    'description' => '',
    'table' => 'pubauthor',
    'fields' => [
      'pubauthor_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'editor' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => FALSE,
        'default' => FALSE,
      ],
      'surname' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '100',
        'not null' => TRUE,
      ],
      'givennames' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '100',
        'not null' => FALSE,
      ],
      'suffix' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '100',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'pubauthor_c1' => [
        0 => 'pub_id',
        1 => 'rank',
      ],
    ],
    'indexes' => [
      'pubauthor_contact_idx1' => [
        0 => 'pubauthor_id',
      ],
      'pubauthor_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'pubauthor_id',
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'pubauthor_contact',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_pubauthor_contact()
 *
 * Purpose: To describe the structure of 'pubauthor_contact' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'pubauthor_contact' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_pubauthor_contact() {
  $description = [
    'description' => '',
    'table' => 'pubauthor_contact',
    'fields' => [
      'pubauthor_contact_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pubauthor_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'pubauthor_contact_c1' => [
        0 => 'contact_id',
        1 => 'pubauthor_id',
      ],
    ],
    'primary key' => [
      0 => 'pubauthor_contact_id',
    ],
    'foreign keys' => [
      'pubauthor' => [
        'table' => 'pubauthor',
        'columns' => [
          'pubauthor_id' => 'pubauthor_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_pub_dbxref()
 *
 * Purpose: To describe the structure of 'pub_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'pub_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_pub_dbxref() {
  $description = [
    'description' => '',
    'table' => 'pub_dbxref',
    'fields' => [
      'pub_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'pub_dbxref_c1' => [
        0 => 'pub_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'pub_dbxref_idx1' => [
        0 => 'pub_id',
      ],
      'pub_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'pub_dbxref_id',
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_pubprop()
 *
 * Purpose: To describe the structure of 'pubprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'pubprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_pubprop() {
  $description = [
    'description' => '',
    'table' => 'pubprop',
    'fields' => [
      'pubprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'pubprop_c1' => [
        0 => 'pub_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'pubprop_idx1' => [
        0 => 'pub_id',
      ],
      'pubprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'pubprop_id',
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_pub_relationship()
 *
 * Purpose: To describe the structure of 'pub_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'pub_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_pub_relationship() {
  $description = [
    'description' => '',
    'table' => 'pub_relationship',
    'fields' => [
      'pub_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'pub_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'pub_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'pub_relationship_idx2' => [
        0 => 'object_id',
      ],
      'pub_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'pub_relationship_id',
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'subject_id' => 'pub_id',
          'object_id' => 'pub_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_quantification()
 *
 * Purpose: To describe the structure of 'quantification' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'quantification' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_quantification() {
  $description = [
    'description' => '',
    'table' => 'quantification',
    'fields' => [
      'quantification_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'acquisition_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'operator_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'analysis_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'quantificationdate' => [
        'size' => 'normal',
        'type' => 'datetime',
        'not null' => FALSE,
        'default' => 'now()',
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'uri' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'quantification_c1' => [
        0 => 'name',
        1 => 'analysis_id',
      ],
    ],
    'indexes' => [
      'quantification_idx1' => [
        0 => 'acquisition_id',
      ],
      'quantification_idx2' => [
        0 => 'operator_id',
      ],
      'quantification_idx3' => [
        0 => 'protocol_id',
      ],
      'quantification_idx4' => [
        0 => 'analysis_id',
      ],
    ],
    'primary key' => [
      0 => 'quantification_id',
    ],
    'foreign keys' => [
      'acquisition' => [
        'table' => 'acquisition',
        'columns' => [
          'acquisition_id' => 'acquisition_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'operator_id' => 'contact_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'elementresult',
      1 => 'quantificationprop',
      2 => 'quantification_relationship',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_quantificationprop()
 *
 * Purpose: To describe the structure of 'quantificationprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'quantificationprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_quantificationprop() {
  $description = [
    'description' => '',
    'table' => 'quantificationprop',
    'fields' => [
      'quantificationprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'quantification_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'quantificationprop_c1' => [
        0 => 'quantification_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'quantificationprop_idx1' => [
        0 => 'quantification_id',
      ],
      'quantificationprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'quantificationprop_id',
    ],
    'foreign keys' => [
      'quantification' => [
        'table' => 'quantification',
        'columns' => [
          'quantification_id' => 'quantification_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_quantification_relationship()
 *
 * Purpose: To describe the structure of 'quantification_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'quantification_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_quantification_relationship() {
  $description = [
    'description' => '',
    'table' => 'quantification_relationship',
    'fields' => [
      'quantification_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'quantification_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'quantification_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'quantification_relationship_idx2' => [
        0 => 'type_id',
      ],
      'quantification_relationship_idx3' => [
        0 => 'object_id',
      ],
    ],
    'primary key' => [
      0 => 'quantification_relationship_id',
    ],
    'foreign keys' => [
      'quantification' => [
        'table' => 'quantification',
        'columns' => [
          'subject_id' => 'quantification_id',
          'object_id' => 'quantification_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock()
 *
 * Purpose: To describe the structure of 'stock' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock() {
  $description = [
    'description' => '',
    'table' => 'stock',
    'fields' => [
      'stock_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'organism_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_obsolete' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
    ],
    'unique keys' => [
      'stock_c1' => [
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'stock_idx1' => [
        0 => 'dbxref_id',
      ],
      'stock_idx2' => [
        0 => 'organism_id',
      ],
      'stock_idx3' => [
        0 => 'type_id',
      ],
      'stock_idx4' => [
        0 => 'uniquename',
      ],
      'stock_name_ind1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'stock_id',
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'nd_experiment_stock',
      1 => 'project_stock',
      2 => 'stockcollection_stock',
      3 => 'stock_cvterm',
      4 => 'stock_dbxref',
      5 => 'stock_feature',
      6 => 'stock_featuremap',
      7 => 'stock_genotype',
      8 => 'stock_library',
      9 => 'stockprop',
      10 => 'stock_pub',
      11 => 'stock_relationship',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stockcollection()
 *
 * Purpose: To describe the structure of 'stockcollection' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stockcollection' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stockcollection() {
  $description = [
    'description' => '',
    'table' => 'stockcollection',
    'fields' => [
      'stockcollection_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ],
      'uniquename' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stockcollection_c1' => [
        0 => 'uniquename',
        1 => 'type_id',
      ],
    ],
    'indexes' => [
      'stockcollection_idx1' => [
        0 => 'contact_id',
      ],
      'stockcollection_idx2' => [
        0 => 'type_id',
      ],
      'stockcollection_idx3' => [
        0 => 'uniquename',
      ],
      'stockcollection_name_ind1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'stockcollection_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'stockcollection_db',
      1 => 'stockcollectionprop',
      2 => 'stockcollection_stock',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stockcollection_db()
 *
 * Purpose: To describe the structure of 'stockcollection_db' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stockcollection_db' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stockcollection_db() {
  $description = [
    'description' => '',
    'table' => 'stockcollection_db',
    'fields' => [
      'stockcollection_db_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stockcollection_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'db_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stockcollection_db_c1' => [
        0 => 'stockcollection_id',
        1 => 'db_id',
      ],
    ],
    'indexes' => [
      'stockcollection_db_idx1' => [
        0 => 'stockcollection_id',
      ],
      'stockcollection_db_idx2' => [
        0 => 'db_id',
      ],
    ],
    'primary key' => [
      0 => 'stockcollection_db_id',
    ],
    'foreign keys' => [
      'db' => [
        'table' => 'db',
        'columns' => [
          'db_id' => 'db_id',
        ],
      ],
      'stockcollection' => [
        'table' => 'stockcollection',
        'columns' => [
          'stockcollection_id' => 'stockcollection_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stockcollectionprop()
 *
 * Purpose: To describe the structure of 'stockcollectionprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stockcollectionprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stockcollectionprop() {
  $description = [
    'description' => '',
    'table' => 'stockcollectionprop',
    'fields' => [
      'stockcollectionprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stockcollection_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stockcollectionprop_c1' => [
        0 => 'stockcollection_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'stockcollectionprop_idx1' => [
        0 => 'stockcollection_id',
      ],
      'stockcollectionprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stockcollectionprop_id',
    ],
    'foreign keys' => [
      'stockcollection' => [
        'table' => 'stockcollection',
        'columns' => [
          'stockcollection_id' => 'stockcollection_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stockcollection_stock()
 *
 * Purpose: To describe the structure of 'stockcollection_stock' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stockcollection_stock' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stockcollection_stock() {
  $description = [
    'description' => '',
    'table' => 'stockcollection_stock',
    'fields' => [
      'stockcollection_stock_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stockcollection_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stockcollection_stock_c1' => [
        0 => 'stockcollection_id',
        1 => 'stock_id',
      ],
    ],
    'indexes' => [
      'stockcollection_stock_idx1' => [
        0 => 'stockcollection_id',
      ],
      'stockcollection_stock_idx2' => [
        0 => 'stock_id',
      ],
    ],
    'primary key' => [
      0 => 'stockcollection_stock_id',
    ],
    'foreign keys' => [
      'stockcollection' => [
        'table' => 'stockcollection',
        'columns' => [
          'stockcollection_id' => 'stockcollection_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_cvterm()
 *
 * Purpose: To describe the structure of 'stock_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_cvterm() {
  $description = [
    'description' => '',
    'table' => 'stock_cvterm',
    'fields' => [
      'stock_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_not' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stock_cvterm_c1' => [
        0 => 'stock_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'stock_cvterm_idx1' => [
        0 => 'stock_id',
      ],
      'stock_cvterm_idx2' => [
        0 => 'cvterm_id',
      ],
      'stock_cvterm_idx3' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_cvterm_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'stock_cvtermprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_cvtermprop()
 *
 * Purpose: To describe the structure of 'stock_cvtermprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_cvtermprop() {
  $description = [
    'description' => '',
    'table' => 'stock_cvtermprop',
    'fields' => [
      'stock_cvtermprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stock_cvtermprop_c1' => [
        0 => 'stock_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'stock_cvtermprop_idx1' => [
        0 => 'stock_cvterm_id',
      ],
      'stock_cvtermprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_cvtermprop_id',
    ],
    'foreign keys' => [
      'stock_cvterm' => [
        'table' => 'stock_cvterm',
        'columns' => [
          'stock_cvterm_id' => 'stock_cvterm_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_dbxref()
 *
 * Purpose: To describe the structure of 'stock_dbxref' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_dbxref() {
  $description = [
    'description' => '',
    'table' => 'stock_dbxref',
    'fields' => [
      'stock_dbxref_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'size' => 'normal',
        'type' => 'boolean',
        'not null' => TRUE,
        'default' => 'true',
      ],
    ],
    'unique keys' => [
      'stock_dbxref_c1' => [
        0 => 'stock_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'stock_dbxref_idx1' => [
        0 => 'stock_id',
      ],
      'stock_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_dbxref_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'stock_dbxrefprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_dbxrefprop()
 *
 * Purpose: To describe the structure of 'stock_dbxrefprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_dbxrefprop() {
  $description = [
    'description' => '',
    'table' => 'stock_dbxrefprop',
    'fields' => [
      'stock_dbxrefprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stock_dbxrefprop_c1' => [
        0 => 'stock_dbxref_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'stock_dbxrefprop_idx1' => [
        0 => 'stock_dbxref_id',
      ],
      'stock_dbxrefprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_dbxrefprop_id',
    ],
    'foreign keys' => [
      'stock_dbxref' => [
        'table' => 'stock_dbxref',
        'columns' => [
          'stock_dbxref_id' => 'stock_dbxref_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_feature()
 *
 * Purpose: To describe the structure of 'stock_feature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_feature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_feature() {
  $description = [
    'description' => '',
    'table' => 'stock_feature',
    'fields' => [
      'stock_feature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stock_feature_c1' => [
        0 => 'feature_id',
        1 => 'stock_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'stock_feature_idx1' => [
        0 => 'stock_feature_id',
      ],
      'stock_feature_idx2' => [
        0 => 'feature_id',
      ],
      'stock_feature_idx3' => [
        0 => 'stock_id',
      ],
      'stock_feature_idx4' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_feature_id',
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_featuremap()
 *
 * Purpose: To describe the structure of 'stock_featuremap' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_featuremap' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_featuremap() {
  $description = [
    'description' => '',
    'table' => 'stock_featuremap',
    'fields' => [
      'stock_featuremap_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'stock_featuremap_c1' => [
        0 => 'featuremap_id',
        1 => 'stock_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
      'stock_featuremap_idx1' => [
        0 => 'featuremap_id',
      ],
      'stock_featuremap_idx2' => [
        0 => 'stock_id',
      ],
      'stock_featuremap_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_featuremap_id',
    ],
    'foreign keys' => [
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_genotype()
 *
 * Purpose: To describe the structure of 'stock_genotype' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_genotype' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_genotype() {
  $description = [
    'description' => '',
    'table' => 'stock_genotype',
    'fields' => [
      'stock_genotype_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'genotype_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stock_genotype_c1' => [
        0 => 'stock_id',
        1 => 'genotype_id',
      ],
    ],
    'indexes' => [
      'stock_genotype_idx1' => [
        0 => 'stock_id',
      ],
      'stock_genotype_idx2' => [
        0 => 'genotype_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_genotype_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_library()
 *
 * Purpose: To describe the structure of 'stock_library' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_library' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_library() {
  $description = [
    'description' => '',
    'table' => 'stock_library',
    'fields' => [
      'stock_library_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'library_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stock_library_c1' => [
        0 => 'library_id',
        1 => 'stock_id',
      ],
    ],
    'indexes' => [
      'stock_library_idx1' => [
        0 => 'library_id',
      ],
      'stock_library_idx2' => [
        0 => 'stock_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_library_id',
    ],
    'foreign keys' => [
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stockprop()
 *
 * Purpose: To describe the structure of 'stockprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stockprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stockprop() {
  $description = [
    'description' => '',
    'table' => 'stockprop',
    'fields' => [
      'stockprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stockprop_c1' => [
        0 => 'stock_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'stockprop_idx1' => [
        0 => 'stock_id',
      ],
      'stockprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stockprop_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'stockprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stockprop_pub()
 *
 * Purpose: To describe the structure of 'stockprop_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stockprop_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stockprop_pub() {
  $description = [
    'description' => '',
    'table' => 'stockprop_pub',
    'fields' => [
      'stockprop_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stockprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stockprop_pub_c1' => [
        0 => 'stockprop_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'stockprop_pub_idx1' => [
        0 => 'stockprop_id',
      ],
      'stockprop_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'stockprop_pub_id',
    ],
    'foreign keys' => [
      'stockprop' => [
        'table' => 'stockprop',
        'columns' => [
          'stockprop_id' => 'stockprop_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_pub()
 *
 * Purpose: To describe the structure of 'stock_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_pub() {
  $description = [
    'description' => '',
    'table' => 'stock_pub',
    'fields' => [
      'stock_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stock_pub_c1' => [
        0 => 'stock_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'stock_pub_idx1' => [
        0 => 'stock_id',
      ],
      'stock_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_pub_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_relationship()
 *
 * Purpose: To describe the structure of 'stock_relationship' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_relationship' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_relationship() {
  $description = [
    'description' => '',
    'table' => 'stock_relationship',
    'fields' => [
      'stock_relationship_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'subject_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'object_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'stock_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
      'stock_relationship_idx1' => [
        0 => 'subject_id',
      ],
      'stock_relationship_idx2' => [
        0 => 'object_id',
      ],
      'stock_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_relationship_id',
    ],
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'subject_id' => 'stock_id',
          'object_id' => 'stock_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'stock_relationship_cvterm',
      1 => 'stock_relationship_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_relationship_cvterm()
 *
 * Purpose: To describe the structure of 'stock_relationship_cvterm' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_relationship_cvterm' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_relationship_cvterm() {
  $description = [
    'description' => '',
    'table' => 'stock_relationship_cvterm',
    'fields' => [
      'stock_relationship_cvterm_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_relationship_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'cvterm_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'primary key' => [
      0 => 'stock_relationship_cvterm_id',
    ],
    'foreign keys' => [
      'stock_relationship' => [
        'table' => 'stock_relationship',
        'columns' => [
          'stock_relationship_id' => 'stock_relationship_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_stock_relationship_pub()
 *
 * Purpose: To describe the structure of 'stock_relationship_pub' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'stock_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_stock_relationship_pub() {
  $description = [
    'description' => '',
    'table' => 'stock_relationship_pub',
    'fields' => [
      'stock_relationship_pub_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'stock_relationship_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'stock_relationship_pub_c1' => [
        0 => 'stock_relationship_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
      'stock_relationship_pub_idx1' => [
        0 => 'stock_relationship_id',
      ],
      'stock_relationship_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'primary key' => [
      0 => 'stock_relationship_pub_id',
    ],
    'foreign keys' => [
      'stock_relationship' => [
        'table' => 'stock_relationship',
        'columns' => [
          'stock_relationship_id' => 'stock_relationship_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_study()
 *
 * Purpose: To describe the structure of 'study' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'study' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_study() {
  $description = [
    'description' => '',
    'table' => 'study',
    'fields' => [
      'study_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pub_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'dbxref_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'unique keys' => [
      'study_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
      'study_idx1' => [
        0 => 'contact_id',
      ],
      'study_idx2' => [
        0 => 'pub_id',
      ],
      'study_idx3' => [
        0 => 'dbxref_id',
      ],
    ],
    'primary key' => [
      0 => 'study_id',
    ],
    'foreign keys' => [
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'study_assay',
      1 => 'studydesign',
      2 => 'studyprop',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_study_assay()
 *
 * Purpose: To describe the structure of 'study_assay' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'study_assay' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_study_assay() {
  $description = [
    'description' => '',
    'table' => 'study_assay',
    'fields' => [
      'study_assay_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'study_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'study_assay_c1' => [
        0 => 'study_id',
        1 => 'assay_id',
      ],
    ],
    'indexes' => [
      'study_assay_idx1' => [
        0 => 'study_id',
      ],
      'study_assay_idx2' => [
        0 => 'assay_id',
      ],
    ],
    'primary key' => [
      0 => 'study_assay_id',
    ],
    'foreign keys' => [
      'study' => [
        'table' => 'study',
        'columns' => [
          'study_id' => 'study_id',
        ],
      ],
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_studydesign()
 *
 * Purpose: To describe the structure of 'studydesign' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'studydesign' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_studydesign() {
  $description = [
    'description' => '',
    'table' => 'studydesign',
    'fields' => [
      'studydesign_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'study_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'studydesign_idx1' => [
        0 => 'study_id',
      ],
    ],
    'primary key' => [
      0 => 'studydesign_id',
    ],
    'foreign keys' => [
      'study' => [
        'table' => 'study',
        'columns' => [
          'study_id' => 'study_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'studydesignprop',
      1 => 'studyfactor',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_studydesignprop()
 *
 * Purpose: To describe the structure of 'studydesignprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'studydesignprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_studydesignprop() {
  $description = [
    'description' => '',
    'table' => 'studydesignprop',
    'fields' => [
      'studydesignprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'studydesign_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'unique keys' => [
      'studydesignprop_c1' => [
        0 => 'studydesign_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'studydesignprop_idx1' => [
        0 => 'studydesign_id',
      ],
      'studydesignprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'studydesignprop_id',
    ],
    'foreign keys' => [
      'studydesign' => [
        'table' => 'studydesign',
        'columns' => [
          'studydesign_id' => 'studydesign_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_studyfactor()
 *
 * Purpose: To describe the structure of 'studyfactor' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'studyfactor' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_studyfactor() {
  $description = [
    'description' => '',
    'table' => 'studyfactor',
    'fields' => [
      'studyfactor_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'studydesign_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => TRUE,
      ],
      'description' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'studyfactor_idx1' => [
        0 => 'studydesign_id',
      ],
      'studyfactor_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'studyfactor_id',
    ],
    'foreign keys' => [
      'studydesign' => [
        'table' => 'studydesign',
        'columns' => [
          'studydesign_id' => 'studydesign_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'studyfactorvalue',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_studyfactorvalue()
 *
 * Purpose: To describe the structure of 'studyfactorvalue' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'studyfactorvalue' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_studyfactorvalue() {
  $description = [
    'description' => '',
    'table' => 'studyfactorvalue',
    'fields' => [
      'studyfactorvalue_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'studyfactor_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'assay_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'factorvalue' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'indexes' => [
      'studyfactorvalue_idx1' => [
        0 => 'studyfactor_id',
      ],
      'studyfactorvalue_idx2' => [
        0 => 'assay_id',
      ],
    ],
    'primary key' => [
      0 => 'studyfactorvalue_id',
    ],
    'foreign keys' => [
      'studyfactor' => [
        'table' => 'studyfactor',
        'columns' => [
          'studyfactor_id' => 'studyfactor_id',
        ],
      ],
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_studyprop()
 *
 * Purpose: To describe the structure of 'studyprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'studyprop' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_studyprop() {
  $description = [
    'description' => '',
    'table' => 'studyprop',
    'fields' => [
      'studyprop_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'study_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'indexes' => [
      'studyprop_idx1' => [
        0 => 'study_id',
      ],
      'studyprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'studyprop_id',
    ],
    'unique keys' => [
      'studyprop_study_id_type_id_rank_key' => [
        0 => 'study_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'foreign keys' => [
      'study' => [
        'table' => 'study',
        'columns' => [
          'study_id' => 'study_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'studyprop_feature',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_studyprop_feature()
 *
 * Purpose: To describe the structure of 'studyprop_feature' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'studyprop_feature' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_studyprop_feature() {
  $description = [
    'description' => '',
    'table' => 'studyprop_feature',
    'fields' => [
      'studyprop_feature_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'studyprop_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'feature_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'studyprop_feature_idx1' => [
        0 => 'studyprop_id',
      ],
      'studyprop_feature_idx2' => [
        0 => 'feature_id',
      ],
    ],
    'primary key' => [
      0 => 'studyprop_feature_id',
    ],
    'unique keys' => [
      'studyprop_feature_studyprop_id_feature_id_key' => [
        0 => 'studyprop_id',
        1 => 'feature_id',
      ],
    ],
    'foreign keys' => [
      'studyprop' => [
        'table' => 'studyprop',
        'columns' => [
          'studyprop_id' => 'studyprop_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_synonym()
 *
 * Purpose: To describe the structure of 'synonym' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'synonym' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_synonym() {
  $description = [
    'description' => '',
    'table' => 'synonym',
    'fields' => [
      'synonym_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'synonym_sgml' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
    ],
    'unique keys' => [
      'synonym_c1' => [
        0 => 'name',
        1 => 'type_id',
      ],
    ],
    'indexes' => [
      'synonym_idx1' => [
        0 => 'type_id',
      ],
    ],
    'primary key' => [
      0 => 'synonym_id',
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'cell_line_synonym',
      1 => 'feature_synonym',
      2 => 'library_synonym',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_tableinfo()
 *
 * Purpose: To describe the structure of 'tableinfo' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'tableinfo' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_tableinfo() {
  $description = [
    'description' => '',
    'table' => 'tableinfo',
    'fields' => [
      'tableinfo_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '30',
        'not null' => TRUE,
      ],
      'primary_key_column' => [
        'size' => 'normal',
        'type' => 'varchar',
        'length' => '30',
        'not null' => FALSE,
      ],
      'is_view' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'view_on_table_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'superclass_table_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'is_updateable' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 1,
      ],
      'modification_date' => [
        'size' => 'normal',
        'type' => 'date',
        'not null' => TRUE,
        'default' => 'now()',
      ],
    ],
    'unique keys' => [
      'tableinfo_c1' => [
        0 => 'name',
      ],
    ],
    'primary key' => [
      0 => 'tableinfo_id',
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'control',
      1 => 'magedocumentation',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_3_treatment()
 *
 * Purpose: To describe the structure of 'treatment' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 * @see chado_generate_var()
 * @see chado_expan_var()
 *
 * @return
 *    An array describing the 'treatment' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_schema_v1_3_treatment() {
  $description = [
    'description' => '',
    'table' => 'treatment',
    'fields' => [
      'treatment_id' => [
        'size' => 'big',
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'rank' => [
        'size' => 'normal',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
      'biomaterial_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'protocol_id' => [
        'size' => 'big',
        'type' => 'int',
        'not null' => FALSE,
      ],
      'name' => [
        'size' => 'normal',
        'type' => 'text',
        'not null' => FALSE,
      ],
    ],
    'indexes' => [
      'treatment_idx1' => [
        0 => 'biomaterial_id',
      ],
      'treatment_idx2' => [
        0 => 'type_id',
      ],
      'treatment_idx3' => [
        0 => 'protocol_id',
      ],
    ],
    'primary key' => [
      0 => 'treatment_id',
    ],
    'foreign keys' => [
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
    ],
    'referring_tables' => [
      0 => 'biomaterial_treatment',
    ],
  ];
  return $description;
}

/**
 * Lists the table names in the v1.3 chado schema
 *
 * @return
 *    An array containing all of the table names
 *
 * @ingroup tripal_chado_v1.3_schema_api
 *
 */
function tripal_chado_chado_get_v1_3_tables() {
  $tables = [
    'acquisition',
    'acquisitionprop',
    'acquisition_relationship',
    'analysis',
    'analysis_cvterm',
    'analysis_dbxref',
    'analysisfeature',
    'analysisfeatureprop',
    'analysis_organism',
    'analysisprop',
    'analysis_pub',
    'analysis_relationship',
    'arraydesign',
    'arraydesignprop',
    'assay',
    'assay_biomaterial',
    'assay_project',
    'assayprop',
    'biomaterial',
    'biomaterial_dbxref',
    'biomaterialprop',
    'biomaterial_relationship',
    'biomaterial_treatment',
    'cell_line',
    'cell_line_cvterm',
    'cell_line_cvtermprop',
    'cell_line_dbxref',
    'cell_line_feature',
    'cell_line_library',
    'cell_lineprop',
    'cell_lineprop_pub',
    'cell_line_pub',
    'cell_line_relationship',
    'cell_line_synonym',
    'chadoprop',
    'channel',
    'contact',
    'contactprop',
    'contact_relationship',
    'control',
    'cv',
    'cvprop',
    'cv_root_mview',
    'cvterm',
    'cvterm_dbxref',
    'cvtermpath',
    'cvtermprop',
    'cvterm_relationship',
    'cvtermsynonym',
    'db',
    'dbprop',
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
    'expressionprop',
    'expression_pub',
    'feature',
    'feature_contact',
    'feature_cvterm',
    'feature_cvterm_dbxref',
    'feature_cvtermprop',
    'feature_cvterm_pub',
    'feature_dbxref',
    'feature_expression',
    'feature_expressionprop',
    'feature_genotype',
    'featureloc',
    'featureloc_pub',
    'featuremap',
    'featuremap_contact',
    'featuremap_dbxref',
    'featuremap_organism',
    'featuremapprop',
    'featuremap_pub',
    'feature_phenotype',
    'featurepos',
    'featureposprop',
    'featureprop',
    'featureprop_pub',
    'feature_pub',
    'feature_pubprop',
    'featurerange',
    'feature_relationship',
    'feature_relationshipprop',
    'feature_relationshipprop_pub',
    'feature_relationship_pub',
    'feature_synonym',
    'genotype',
    'genotypeprop',
    'library',
    'library_contact',
    'library_cvterm',
    'library_dbxref',
    'library_expression',
    'library_expressionprop',
    'library_feature',
    'library_featureprop',
    'libraryprop',
    'libraryprop_pub',
    'library_pub',
    'library_relationship',
    'library_relationship_pub',
    'library_synonym',
    'magedocumentation',
    'mageml',
    'materialized_view',
    'nd_experiment',
    'nd_experiment_analysis',
    'nd_experiment_contact',
    'nd_experiment_dbxref',
    'nd_experiment_genotype',
    'nd_experiment_phenotype',
    'nd_experiment_project',
    'nd_experimentprop',
    'nd_experiment_protocol',
    'nd_experiment_pub',
    'nd_experiment_stock',
    'nd_experiment_stock_dbxref',
    'nd_experiment_stockprop',
    'nd_geolocation',
    'nd_geolocationprop',
    'nd_protocol',
    'nd_protocolprop',
    'nd_protocol_reagent',
    'nd_reagent',
    'nd_reagentprop',
    'nd_reagent_relationship',
    'organism',
    'organism_cvterm',
    'organism_cvtermprop',
    'organism_dbxref',
    'organism_feature_count',
    'organismprop',
    'organismprop_pub',
    'organism_pub',
    'organism_relationship',
    'phendesc',
    'phenotype',
    'phenotype_comparison',
    'phenotype_comparison_cvterm',
    'phenotype_cvterm',
    'phenotypeprop',
    'phenstatement',
    'phylonode',
    'phylonode_dbxref',
    'phylonode_organism',
    'phylonodeprop',
    'phylonode_pub',
    'phylonode_relationship',
    'phylotree',
    'phylotreeprop',
    'phylotree_pub',
    'project',
    'project_analysis',
    'project_contact',
    'project_dbxref',
    'project_feature',
    'projectprop',
    'project_pub',
    'project_relationship',
    'project_stock',
    'protocol',
    'protocolparam',
    'pub',
    'pubauthor',
    'pubauthor_contact',
    'pub_dbxref',
    'pubprop',
    'pub_relationship',
    'quantification',
    'quantificationprop',
    'quantification_relationship',
    'stock',
    'stockcollection',
    'stockcollection_db',
    'stockcollectionprop',
    'stockcollection_stock',
    'stock_cvterm',
    'stock_cvtermprop',
    'stock_dbxref',
    'stock_dbxrefprop',
    'stock_feature',
    'stock_featuremap',
    'stock_genotype',
    'stock_library',
    'stockprop',
    'stockprop_pub',
    'stock_pub',
    'stock_relationship',
    'stock_relationship_cvterm',
    'stock_relationship_pub',
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
    'treatment',
  ];
  return $tables;
}
