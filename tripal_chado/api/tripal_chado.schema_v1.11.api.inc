<?php
/**
 * @file
 * Describes the chado tables in version 1.11
 */

/**
 * @defgroup tripal_schema_v1_11_api Chado v1.11 Schema
 * @ingroup tripal_chado_schema_api
 * @{
 * Provides an application programming interface (API) for describing Chado
 *   tables. This API consists of a set of functions, one for each table in
 *   Chado.  Each function simply returns a Drupal style array that defines the
 *   table.
 *
 * Because Drupal 6 does not handle foreign key (FK) relationships, which are
 *   needed to for Tripal Views, they have been added to the schema
 *   defintitions below.
 *
 * The functions provided in this documentation should not be called as is, but
 *   if you need the Drupal-style array definition for any table, use the
 *   following function call:
 *
 *   $table_desc = chado_get_schema($table)
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The chado_get_schema function determines the appropriate version
 *   of Chado and uses the Drupal hook infrastructure to call the appropriate
 *   hook function to retrieve the table schema.
 *
 * If you need to augment these schema definitions within your own module, you
 *   need to implement the hook_chado_schema_v1_11_[table name]() hook where
 *   [table name] is the name of the chado table whose schema definition you
 *   want to augment.
 * @}
 */

/**
 * Implements hook_chado_schema_v1_11_acquisition()
 * Purpose: To describe the structure of 'acquisition' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_acquisition() {
  $description = [
    'table' => 'acquisition',
    'fields' => [
      'acquisition_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'channel_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'acquisitiondate' => [
        'type' => 'datetime',
        'not NULL' => '',
        'default' => 'ow(',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'uri' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'acquisition_id',
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
    'foreign keys' => [
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
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_acquisition_relationship()
 * Purpose: To describe the structure of 'acquisition_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_acquisition_relationship() {
  $description = [
    'table' => 'acquisition_relationship',
    'fields' => [
      'acquisition_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'acquisition_relationship_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'acquisition' => [
        'table' => 'acquisition',
        'columns' => [
          'subject_id' => 'acquisition_id',
          'object_id' => 'acquisition_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_acquisitionprop()
 * Purpose: To describe the structure of 'acquisitionprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_acquisitionprop() {
  $description = [
    'table' => 'acquisitionprop',
    'fields' => [
      'acquisitionprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'acquisition_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'acquisitionprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'acquisition' => [
        'table' => 'acquisition',
        'columns' => [
          'acquisition_id' => 'acquisition_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_analysis()
 * Purpose: To describe the structure of 'analysis' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_analysis() {
  $description = [
    'referring_tables' => [
      0 => 'analysisfeature',
      1 => 'analysisprop',
      2 => 'phylotree',
      3 => 'quantification',
    ],
    'table' => 'analysis',
    'fields' => [
      'analysis_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'program' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'programversion' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'algorithm' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'sourcename' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'sourceversion' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'sourceuri' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'timeexecuted' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
    ],
    'primary key' => [
      0 => 'analysis_id',
    ],
    'unique keys' => [
      'analysis_c1' => [
        0 => 'program',
        1 => 'programversion',
        2 => 'sourcename',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_analysisprop()
 *
 * Purpose: To describe the structure of 'analysisprop' to tripal.  This table
 * doesn't exist in Chado v1.1 (it does in v1.2), so we need to describe it
 * here
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
function tripal_chado_chado_schema_v1_11_analysisprop() {
  $description = [
    'table' => 'analysisprop',
    'fields' => [
      'analysisprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'analysis_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'analysisprop_id',
    ],
    'unique keys' => [
      'analysisprop_c1' => [
        0 => 'analysis_id',
        1 => 'type_id',
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
    'foreign keys' => [
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_analysisfeature()
 * Purpose: To describe the structure of 'analysisfeature' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_analysisfeature() {
  $description = [
    'table' => 'analysisfeature',
    'fields' => [
      'analysisfeature_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'analysis_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rawscore' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ],
      'normscore' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ],
      'significance' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ],
      'identity' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'analysisfeature_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_arraydesign()
 * Purpose: To describe the structure of 'arraydesign' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_arraydesign() {
  $description = [
    'table' => 'arraydesign',
    'fields' => [
      'arraydesign_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'manufacturer_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'platformtype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'substratetype_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'version' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'array_dimensions' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'element_dimensions' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'num_of_elements' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'num_array_columns' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'num_array_rows' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'num_grid_columns' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'num_grid_rows' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'num_sub_columns' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'num_sub_rows' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'arraydesign_id',
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
          'platformtype_id' => 'cvterm_id',
          'substratetype_id' => 'cvterm_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'manufacturer_id' => 'contact_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_arraydesignprop()
 * Purpose: To describe the structure of 'arraydesignprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_arraydesignprop() {
  $description = [
    'table' => 'arraydesignprop',
    'fields' => [
      'arraydesignprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'arraydesign_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'arraydesignprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'arraydesign' => [
        'table' => 'arraydesign',
        'columns' => [
          'arraydesign_id' => 'arraydesign_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_assay()
 * Purpose: To describe the structure of 'assay' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_assay() {
  $description = [
    'table' => 'assay',
    'fields' => [
      'assay_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'arraydesign_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'assaydate' => [
        'type' => 'datetime',
        'not NULL' => '',
        'default' => 'ow(',
      ],
      'arrayidentifier' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'arraybatchidentifier' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'operator_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'assay_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
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
      'arraydesign' => [
        'table' => 'arraydesign',
        'columns' => [
          'arraydesign_id' => 'arraydesign_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_assay_biomaterial()
 * Purpose: To describe the structure of 'assay_biomaterial' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_assay_biomaterial() {
  $description = [
    'table' => 'assay_biomaterial',
    'fields' => [
      'assay_biomaterial_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'biomaterial_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'channel_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'assay_biomaterial_id',
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
    'foreign keys' => [
      'channel' => [
        'table' => 'channel',
        'columns' => [
          'channel_id' => 'channel_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_assay_project()
 * Purpose: To describe the structure of 'assay_project' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_assay_project() {
  $description = [
    'table' => 'assay_project',
    'fields' => [
      'assay_project_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'project_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'assay_project_id',
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
    'foreign keys' => [
      'project' => [
        'table' => 'project',
        'columns' => [
          'project_id' => 'project_id',
        ],
      ],
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_assayprop()
 * Purpose: To describe the structure of 'assayprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_assayprop() {
  $description = [
    'table' => 'assayprop',
    'fields' => [
      'assayprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'assayprop_id',
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_biomaterial()
 * Purpose: To describe the structure of 'biomaterial' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_biomaterial() {
  $description = [
    'table' => 'biomaterial',
    'fields' => [
      'biomaterial_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'taxon_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'biosourceprovider_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_id',
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
          'taxon_id' => 'organism_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'biosourceprovider_id' => 'contact_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_biomaterial_dbxref()
 * Purpose: To describe the structure of 'biomaterial_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_biomaterial_dbxref() {
  $description = [
    'table' => 'biomaterial_dbxref',
    'fields' => [
      'biomaterial_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'biomaterial_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_dbxref_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_biomaterial_relationship()
 * Purpose: To describe the structure of 'biomaterial_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_biomaterial_relationship() {
  $description = [
    'table' => 'biomaterial_relationship',
    'fields' => [
      'biomaterial_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_relationship_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'subject_id' => 'biomaterial_id',
          'object_id' => 'biomaterial_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_biomaterial_treatment()
 * Purpose: To describe the structure of 'biomaterial_treatment' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_biomaterial_treatment() {
  $description = [
    'table' => 'biomaterial_treatment',
    'fields' => [
      'biomaterial_treatment_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'biomaterial_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'treatment_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'unittype_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'value' => [
        'type' => 'float',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'biomaterial_treatment_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'unittype_id' => 'cvterm_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_biomaterialprop()
 * Purpose: To describe the structure of 'biomaterialprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_biomaterialprop() {
  $description = [
    'table' => 'biomaterialprop',
    'fields' => [
      'biomaterialprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'biomaterial_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'biomaterialprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line()
 * Purpose: To describe the structure of 'cell_line' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line() {
  $description = [
    'table' => 'cell_line',
    'fields' => [
      'cell_line_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'timeaccessioned' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
      'timelastmodified' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_id',
    ],
    'unique keys' => [
      'cell_line_c1' => [
        0 => 'uniquename',
        1 => 'organism_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_cvterm()
 * Purpose: To describe the structure of 'cell_line_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_cvterm() {
  $description = [
    'table' => 'cell_line_cvterm',
    'fields' => [
      'cell_line_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_cvterm_id',
    ],
    'unique keys' => [
      'cell_line_cvterm_c1' => [
        0 => 'cell_line_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
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
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_cvtermprop()
 * Purpose: To describe the structure of 'cell_line_cvtermprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_cvtermprop() {
  $description = [
    'table' => 'cell_line_cvtermprop',
    'fields' => [
      'cell_line_cvtermprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_cvtermprop_id',
    ],
    'unique keys' => [
      'cell_line_cvtermprop_c1' => [
        0 => 'cell_line_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'cell_line_cvterm' => [
        'table' => 'cell_line_cvterm',
        'columns' => [
          'cell_line_cvterm_id' => 'cell_line_cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_dbxref()
 * Purpose: To describe the structure of 'cell_line_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_dbxref() {
  $description = [
    'table' => 'cell_line_dbxref',
    'fields' => [
      'cell_line_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'true',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_dbxref_id',
    ],
    'unique keys' => [
      'cell_line_dbxref_c1' => [
        0 => 'cell_line_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_feature()
 * Purpose: To describe the structure of 'cell_line_feature' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_feature() {
  $description = [
    'table' => 'cell_line_feature',
    'fields' => [
      'cell_line_feature_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_feature_id',
    ],
    'unique keys' => [
      'cell_line_feature_c1' => [
        0 => 'cell_line_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_library()
 * Purpose: To describe the structure of 'cell_line_library' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_library() {
  $description = [
    'table' => 'cell_line_library',
    'fields' => [
      'cell_line_library_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_library_id',
    ],
    'unique keys' => [
      'cell_line_library_c1' => [
        0 => 'cell_line_id',
        1 => 'library_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_pub()
 * Purpose: To describe the structure of 'cell_line_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_pub() {
  $description = [
    'table' => 'cell_line_pub',
    'fields' => [
      'cell_line_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_pub_id',
    ],
    'unique keys' => [
      'cell_line_pub_c1' => [
        0 => 'cell_line_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_relationship()
 * Purpose: To describe the structure of 'cell_line_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_relationship() {
  $description = [
    'table' => 'cell_line_relationship',
    'fields' => [
      'cell_line_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_relationship_id',
    ],
    'unique keys' => [
      'cell_line_relationship_c1' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'subject_id' => 'cell_line_id',
          'object_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_line_synonym()
 * Purpose: To describe the structure of 'cell_line_synonym' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_line_synonym() {
  $description = [
    'table' => 'cell_line_synonym',
    'fields' => [
      'cell_line_synonym_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'synonym_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'is_internal' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
    ],
    'primary key' => [
      0 => 'cell_line_synonym_id',
    ],
    'unique keys' => [
      'cell_line_synonym_c1' => [
        0 => 'synonym_id',
        1 => 'cell_line_id',
        2 => 'pub_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'synonym' => [
        'table' => 'synonym',
        'columns' => [
          'synonym_id' => 'synonym_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_lineprop()
 * Purpose: To describe the structure of 'cell_lineprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_lineprop() {
  $description = [
    'table' => 'cell_lineprop',
    'fields' => [
      'cell_lineprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_line_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'cell_lineprop_id',
    ],
    'unique keys' => [
      'cell_lineprop_c1' => [
        0 => 'cell_line_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'cell_line' => [
        'table' => 'cell_line',
        'columns' => [
          'cell_line_id' => 'cell_line_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cell_lineprop_pub()
 * Purpose: To describe the structure of 'cell_lineprop_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cell_lineprop_pub() {
  $description = [
    'table' => 'cell_lineprop_pub',
    'fields' => [
      'cell_lineprop_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cell_lineprop_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'cell_lineprop_pub_id',
    ],
    'unique keys' => [
      'cell_lineprop_pub_c1' => [
        0 => 'cell_lineprop_id',
        1 => 'pub_id',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'cell_lineprop' => [
        'table' => 'cell_lineprop',
        'columns' => [
          'cell_lineprop_id' => 'cell_lineprop_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_channel()
 * Purpose: To describe the structure of 'channel' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_channel() {
  $description = [
    'table' => 'channel',
    'fields' => [
      'channel_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'definition' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'channel_id',
    ],
    'unique keys' => [
      'channel_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_contact()
 * Purpose: To describe the structure of 'contact' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_contact() {
  $description = [
    'table' => 'contact',
    'fields' => [
      'contact_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'contact_id',
    ],
    'unique keys' => [
      'contact_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_contact_relationship()
 * Purpose: To describe the structure of 'contact_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_contact_relationship() {
  $description = [
    'table' => 'contact_relationship',
    'fields' => [
      'contact_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'contact_relationship_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_control()
 * Purpose: To describe the structure of 'control' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_control() {
  $description = [
    'table' => 'control',
    'fields' => [
      'control_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'tableinfo_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'row_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'control_id',
    ],
    'unique keys' => [
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
    'foreign keys' => [
      'tableinfo' => [
        'table' => 'tableinfo',
        'columns' => [
          'tableinfo_id' => 'tableinfo_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cv()
 * Purpose: To describe the structure of 'cv' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cv() {
  $description = [
    'table' => 'cv',
    'fields' => [
      'cv_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'definition' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'cv_id',
    ],
    'unique keys' => [
      'cv_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cvterm()
 * Purpose: To describe the structure of 'cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cvterm() {
  $description = [
    'table' => 'cvterm',
    'fields' => [
      'cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cv_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '1024',
        'not NULL' => '1',
      ],
      'definition' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_obsolete' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
      'is_relationshiptype' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'cvterm_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'cv' => [
        'table' => 'cv',
        'columns' => [
          'cv_id' => 'cv_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cvterm_dbxref()
 * Purpose: To describe the structure of 'cvterm_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cvterm_dbxref() {
  $description = [
    'table' => 'cvterm_dbxref',
    'fields' => [
      'cvterm_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_for_definition' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'cvterm_dbxref_id',
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
          'cvterm_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cvterm_relationship()
 * Purpose: To describe the structure of 'cvterm_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cvterm_relationship() {
  $description = [
    'table' => 'cvterm_relationship',
    'fields' => [
      'cvterm_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'cvterm_relationship_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cvtermpath()
 * Purpose: To describe the structure of 'cvtermpath' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cvtermpath() {
  $description = [
    'table' => 'cvtermpath',
    'fields' => [
      'cvtermpath_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cv_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pathdistance' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'cvtermpath_id',
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
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cvtermprop()
 * Purpose: To describe the structure of 'cvtermprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cvtermprop() {
  $description = [
    'table' => 'cvtermprop',
    'fields' => [
      'cvtermprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '1',
        'default' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'cvtermprop_id',
    ],
    'unique keys' => [
      'cvterm_id' => [
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_cvtermsynonym()
 * Purpose: To describe the structure of 'cvtermsynonym' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_cvtermsynonym() {
  $description = [
    'table' => 'cvtermsynonym',
    'fields' => [
      'cvtermsynonym_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'synonym' => [
        'type' => 'varchar',
        'length' => '1024',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'cvtermsynonym_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_db()
 * Purpose: To describe the structure of 'db' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_db() {
  $description = [
    'table' => 'db',
    'fields' => [
      'db_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'urlprefix' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'url' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'db_id',
    ],
    'unique keys' => [
      'db_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_dbxref()
 * Purpose: To describe the structure of 'dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_dbxref() {
  $description = [
    'table' => 'dbxref',
    'fields' => [
      'dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'db_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'accession' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'version' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
        'default' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'dbxref_id',
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
    'foreign keys' => [
      'db' => [
        'table' => 'db',
        'columns' => [
          'db_id' => 'db_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_dbxrefprop()
 * Purpose: To describe the structure of 'dbxrefprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_dbxrefprop() {
  $description = [
    'table' => 'dbxrefprop',
    'fields' => [
      'dbxrefprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '1',
        'default' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'dbxrefprop_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_eimage()
 * Purpose: To describe the structure of 'eimage' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_eimage() {
  $description = [
    'table' => 'eimage',
    'fields' => [
      'eimage_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'eimage_data' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'eimage_type' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'image_uri' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'eimage_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_element()
 * Purpose: To describe the structure of 'element' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_element() {
  $description = [
    'table' => 'element',
    'fields' => [
      'element_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'arraydesign_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'element_id',
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_element_relationship()
 * Purpose: To describe the structure of 'element_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_element_relationship() {
  $description = [
    'table' => 'element_relationship',
    'fields' => [
      'element_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'element_relationship_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'element' => [
        'table' => 'element',
        'columns' => [
          'subject_id' => 'element_id',
          'object_id' => 'element_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_elementresult()
 * Purpose: To describe the structure of 'elementresult' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_elementresult() {
  $description = [
    'table' => 'elementresult',
    'fields' => [
      'elementresult_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'element_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'quantification_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'signal' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'elementresult_id',
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
    'foreign keys' => [
      'quantification' => [
        'table' => 'quantification',
        'columns' => [
          'quantification_id' => 'quantification_id',
        ],
      ],
      'element' => [
        'table' => 'element',
        'columns' => [
          'element_id' => 'element_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_elementresult_relationship()
 * Purpose: To describe the structure of 'elementresult_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_elementresult_relationship() {
  $description = [
    'table' => 'elementresult_relationship',
    'fields' => [
      'elementresult_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'elementresult_relationship_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'elementresult' => [
        'table' => 'elementresult',
        'columns' => [
          'subject_id' => 'elementresult_id',
          'object_id' => 'elementresult_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_environment()
 * Purpose: To describe the structure of 'environment' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_environment() {
  $description = [
    'table' => 'environment',
    'fields' => [
      'environment_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'environment_id',
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
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_environment_cvterm()
 * Purpose: To describe the structure of 'environment_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_environment_cvterm() {
  $description = [
    'table' => 'environment_cvterm',
    'fields' => [
      'environment_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'environment_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'environment_cvterm_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'environment' => [
        'table' => 'environment',
        'columns' => [
          'environment_id' => 'environment_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_expression()
 * Purpose: To describe the structure of 'expression' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_expression() {
  $description = [
    'table' => 'expression',
    'fields' => [
      'expression_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'md5checksum' => [
        'type' => 'char',
        'length' => '32',
        'not NULL' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'expression_id',
    ],
    'unique keys' => [
      'expression_c1' => [
        0 => 'uniquename',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_expression_cvterm()
 * Purpose: To describe the structure of 'expression_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_expression_cvterm() {
  $description = [
    'table' => 'expression_cvterm',
    'fields' => [
      'expression_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'expression_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
      'cvterm_type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'expression_cvterm_id',
    ],
    'unique keys' => [
      'expression_cvterm_c1' => [
        0 => 'expression_id',
        1 => 'cvterm_id',
        2 => 'cvterm_type_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
          'cvterm_type_id' => 'cvterm_id',
        ],
      ],
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_expression_cvtermprop()
 * Purpose: To describe the structure of 'expression_cvtermprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_expression_cvtermprop() {
  $description = [
    'table' => 'expression_cvtermprop',
    'fields' => [
      'expression_cvtermprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'expression_cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'expression_cvtermprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'expression_cvterm' => [
        'table' => 'expression_cvterm',
        'columns' => [
          'expression_cvterm_id' => 'expression_cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_expression_image()
 * Purpose: To describe the structure of 'expression_image' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_expression_image() {
  $description = [
    'table' => 'expression_image',
    'fields' => [
      'expression_image_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'expression_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'eimage_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'expression_image_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_expression_pub()
 * Purpose: To describe the structure of 'expression_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_expression_pub() {
  $description = [
    'table' => 'expression_pub',
    'fields' => [
      'expression_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'expression_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'expression_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_expressionprop()
 * Purpose: To describe the structure of 'expressionprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_expressionprop() {
  $description = [
    'table' => 'expressionprop',
    'fields' => [
      'expressionprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'expression_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'expressionprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature()
 * Purpose: To describe the structure of 'feature' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature() {
  $description = [
    'table' => 'feature',
    'fields' => [
      'feature_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'residues' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'seqlen' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'md5checksum' => [
        'type' => 'char',
        'length' => '32',
        'not NULL' => '',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_analysis' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'is_obsolete' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'timeaccessioned' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
      'timelastmodified' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
    ],
    'primary key' => [
      0 => 'feature_id',
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
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
    'referring_tables' => [
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_cvterm()
 * Purpose: To describe the structure of 'feature_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_cvterm() {
  $description = [
    'table' => 'feature_cvterm',
    'fields' => [
      'feature_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_not' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvterm_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_cvterm_dbxref()
 * Purpose: To describe the structure of 'feature_cvterm_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_cvterm_dbxref() {
  $description = [
    'table' => 'feature_cvterm_dbxref',
    'fields' => [
      'feature_cvterm_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvterm_dbxref_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_cvterm_pub()
 * Purpose: To describe the structure of 'feature_cvterm_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_cvterm_pub() {
  $description = [
    'table' => 'feature_cvterm_pub',
    'fields' => [
      'feature_cvterm_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvterm_pub_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_cvtermprop()
 * Purpose: To describe the structure of 'feature_cvtermprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_cvtermprop() {
  $description = [
    'table' => 'feature_cvtermprop',
    'fields' => [
      'feature_cvtermprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'feature_cvtermprop_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_dbxref()
 * Purpose: To describe the structure of 'feature_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_dbxref() {
  $description = [
    'table' => 'feature_dbxref',
    'fields' => [
      'feature_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'true',
      ],
    ],
    'primary key' => [
      0 => 'feature_dbxref_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_expression()
 * Purpose: To describe the structure of 'feature_expression' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_expression() {
  $description = [
    'table' => 'feature_expression',
    'fields' => [
      'feature_expression_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'expression_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_expression_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'expression' => [
        'table' => 'expression',
        'columns' => [
          'expression_id' => 'expression_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_expressionprop()
 * Purpose: To describe the structure of 'feature_expressionprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_expressionprop() {
  $description = [
    'table' => 'feature_expressionprop',
    'fields' => [
      'feature_expressionprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_expression_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'feature_expressionprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'feature_expression' => [
        'table' => 'feature_expression',
        'columns' => [
          'feature_expression_id' => 'feature_expression_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_genotype()
 * Purpose: To describe the structure of 'feature_genotype' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_genotype() {
  $description = [
    'table' => 'feature_genotype',
    'fields' => [
      'feature_genotype_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'genotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'chromosome_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cgroup' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_genotype_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_phenotype()
 * Purpose: To describe the structure of 'feature_phenotype' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_phenotype() {
  $description = [
    'table' => 'feature_phenotype',
    'fields' => [
      'feature_phenotype_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'phenotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_phenotype_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_pub()
 * Purpose: To describe the structure of 'feature_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_pub() {
  $description = [
    'table' => 'feature_pub',
    'fields' => [
      'feature_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_pubprop()
 * Purpose: To describe the structure of 'feature_pubprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_pubprop() {
  $description = [
    'table' => 'feature_pubprop',
    'fields' => [
      'feature_pubprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'feature_pubprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'feature_pub' => [
        'table' => 'feature_pub',
        'columns' => [
          'feature_pub_id' => 'feature_pub_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_relationship()
 * Purpose: To describe the structure of 'feature_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_relationship() {
  $description = [
    'table' => 'feature_relationship',
    'fields' => [
      'feature_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationship_id',
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
      'feature_relationship_idx2' => [
        0 => 'object_id',
      ],
      'feature_relationship_idx3' => [
        0 => 'type_id',
      ],
    ],
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'object_id' => 'feature_id',
          'subject_id' => 'feature_id',
        ],
      ],
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_relationship_pub()
 * Purpose: To describe the structure of 'feature_relationship_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_relationship_pub() {
  $description = [
    'table' => 'feature_relationship_pub',
    'fields' => [
      'feature_relationship_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_relationship_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationship_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'feature_relationship' => [
        'table' => 'feature_relationship',
        'columns' => [
          'feature_relationship_id' => 'feature_relationship_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_relationshipprop()
 * Purpose: To describe the structure of 'feature_relationshipprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_relationshipprop() {
  $description = [
    'table' => 'feature_relationshipprop',
    'fields' => [
      'feature_relationshipprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_relationship_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationshipprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'feature_relationship' => [
        'table' => 'feature_relationship',
        'columns' => [
          'feature_relationship_id' => 'feature_relationship_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_relationshipprop_pub()
 * Purpose: To describe the structure of 'feature_relationshipprop_pub' to
 * tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_relationshipprop_pub() {
  $description = [
    'table' => 'feature_relationshipprop_pub',
    'fields' => [
      'feature_relationshipprop_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_relationshipprop_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'feature_relationshipprop_pub_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_feature_synonym()
 * Purpose: To describe the structure of 'feature_synonym' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_feature_synonym() {
  $description = [
    'table' => 'feature_synonym',
    'fields' => [
      'feature_synonym_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'synonym_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'is_internal' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
    ],
    'primary key' => [
      0 => 'feature_synonym_id',
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
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featureloc()
 * Purpose: To describe the structure of 'featureloc' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featureloc() {
  $description = [
    'table' => 'featureloc',
    'fields' => [
      'featureloc_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'srcfeature_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'fmin' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'is_fmin_partial' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'fmax' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'is_fmax_partial' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
      'strand' => [
        'type' => 'int',
        'size' => 'small',
        'not NULL' => '',
      ],
      'phase' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'residue_info' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'locgroup' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'featureloc_id',
    ],
    'unique keys' => [
      'featureloc_c1' => [
        0 => 'feature_id',
        1 => 'locgroup',
        2 => 'rank',
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
      'featureloc_idx2' => [
        0 => 'srcfeature_id',
      ],
      'featureloc_idx3' => [
        0 => 'srcfeature_id',
        1 => 'fmin',
        2 => 'fmax',
      ],
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
      0 => 'analysisfeature',
      1 => 'featureloc_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featureloc_pub()
 * Purpose: To describe the structure of 'featureloc_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featureloc_pub() {
  $description = [
    'table' => 'featureloc_pub',
    'fields' => [
      'featureloc_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'featureloc_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'featureloc_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'featureloc' => [
        'table' => 'featureloc',
        'columns' => [
          'featureloc_id' => 'featureloc_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featuremap()
 * Purpose: To describe the structure of 'featuremap' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featuremap() {
  $description = [
    'table' => 'featuremap',
    'fields' => [
      'featuremap_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'unittype_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_id',
    ],
    'unique keys' => [
      'featuremap_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'unittype_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featuremap_pub()
 * Purpose: To describe the structure of 'featuremap_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featuremap_pub() {
  $description = [
    'table' => 'featuremap_pub',
    'fields' => [
      'featuremap_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'featuremap_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'featuremap_pub_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
      'featuremap_pub_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremap_pub_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featurepos()
 * Purpose: To describe the structure of 'featurepos' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featurepos() {
  $description = [
    'table' => 'featurepos',
    'fields' => [
      'featurepos_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'featuremap_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'map_feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'mappos' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'featurepos_id',
    ],
    'unique keys' => [
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
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
          'map_feature_id' => 'feature_id',
        ],
      ],
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featureprop()
 * Purpose: To describe the structure of 'featureprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featureprop() {
  $description = [
    'table' => 'featureprop',
    'fields' => [
      'featureprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'featureprop_id',
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
      0 => 'analysisfeature',
      1 => 'featureprop_pub',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featureprop_pub()
 * Purpose: To describe the structure of 'featureprop_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featureprop_pub() {
  $description = [
    'table' => 'featureprop_pub',
    'fields' => [
      'featureprop_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'featureprop_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'featureprop_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'featureprop' => [
        'table' => 'featureprop',
        'columns' => [
          'featureprop_id' => 'featureprop_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_featurerange()
 * Purpose: To describe the structure of 'featurerange' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_featurerange() {
  $description = [
    'table' => 'featurerange',
    'fields' => [
      'featurerange_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'featuremap_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'leftstartf_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'leftendf_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'rightstartf_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'rightendf_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rangestr' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'featurerange_id',
    ],
    'unique keys' => [
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
    'foreign keys' => [
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
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_genotype()
 * Purpose: To describe the structure of 'genotype' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_genotype() {
  $description = [
    'table' => 'genotype',
    'fields' => [
      'genotype_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'genotype_id',
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
    'foreign keys' => [
    ],
    'referring_tables' => [
      'analysisfeature',
      'feature_genotype',
      'phendesc',
      'phenotype_comparison',
      'phenstatement',
      'stock_genotype',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_library()
 * Purpose: To describe the structure of 'library' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_library() {
  $description = [
    'table' => 'library',
    'referring_tables' => [
      0 => 'cell_line_library',
      1 => 'library_cvterm',
      2 => 'library_dbxref',
      3 => 'library_feature',
      4 => 'library_pub',
      5 => 'library_synonym',
      6 => 'libraryprop',
    ],
    'fields' => [
      'library_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_obsolete' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
      'timeaccessioned' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
      'timelastmodified' => [
        'type' => 'datetime',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
    ],
    'primary key' => [
      0 => 'library_id',
    ],
    'unique keys' => [
      'library_c1' => [
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ],
    ],
    'indexes' => [
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_library_cvterm()
 * Purpose: To describe the structure of 'library_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_library_cvterm() {
  $description = [
    'table' => 'library_cvterm',
    'fields' => [
      'library_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'library_cvterm_id',
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
    'foreign keys' => [
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
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_library_dbxref()
 * Purpose: To describe the structure of 'library_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_library_dbxref() {
  $description = [
    'table' => 'library_dbxref',
    'fields' => [
      'library_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'true',
      ],
    ],
    'primary key' => [
      0 => 'library_dbxref_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_library_feature()
 * Purpose: To describe the structure of 'library_feature' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_library_feature() {
  $description = [
    'table' => 'library_feature',
    'fields' => [
      'library_feature_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'library_feature_id',
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
    'foreign keys' => [
      'feature' => [
        'table' => 'feature',
        'columns' => [
          'feature_id' => 'feature_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_library_pub()
 * Purpose: To describe the structure of 'library_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_library_pub() {
  $description = [
    'table' => 'library_pub',
    'fields' => [
      'library_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'library_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_library_synonym()
 * Purpose: To describe the structure of 'library_synonym' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_library_synonym() {
  $description = [
    'table' => 'library_synonym',
    'fields' => [
      'library_synonym_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'synonym_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'true',
      ],
      'is_internal' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
    ],
    'primary key' => [
      0 => 'library_synonym_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_libraryprop()
 * Purpose: To describe the structure of 'libraryprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_libraryprop() {
  $description = [
    'table' => 'libraryprop',
    'fields' => [
      'libraryprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'library_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'libraryprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'library' => [
        'table' => 'library',
        'columns' => [
          'library_id' => 'library_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_libraryprop_pub()
 * Purpose: To describe the structure of 'libraryprop_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_libraryprop_pub() {
  $description = [
    'table' => 'libraryprop_pub',
    'fields' => [
      'libraryprop_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'libraryprop_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'libraryprop_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'libraryprop' => [
        'table' => 'libraryprop',
        'columns' => [
          'libraryprop_id' => 'libraryprop_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_magedocumentation()
 * Purpose: To describe the structure of 'magedocumentation' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_magedocumentation() {
  $description = [
    'table' => 'magedocumentation',
    'fields' => [
      'magedocumentation_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'mageml_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'tableinfo_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'row_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'mageidentifier' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'magedocumentation_id',
    ],
    'unique keys' => [
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
    'foreign keys' => [
      'tableinfo' => [
        'table' => 'tableinfo',
        'columns' => [
          'tableinfo_id' => 'tableinfo_id',
        ],
      ],
      'mageml' => [
        'table' => 'mageml',
        'columns' => [
          'mageml_id' => 'mageml_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_mageml()
 * Purpose: To describe the structure of 'mageml' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_mageml() {
  $description = [
    'table' => 'mageml',
    'fields' => [
      'mageml_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'mage_package' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'mage_ml' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'mageml_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_materialized_view()
 * Purpose: To describe the structure of 'materialized_view' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_materialized_view() {
  $description = [
    'table' => 'materialized_view',
    'fields' => [
      'materialized_view_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'last_update' => [
        'type' => 'datetime',
        'not NULL' => '',
      ],
      'refresh_time' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '64',
        'not NULL' => '',
      ],
      'mv_schema' => [
        'type' => 'varchar',
        'length' => '64',
        'not NULL' => '',
      ],
      'mv_table' => [
        'type' => 'varchar',
        'length' => '128',
        'not NULL' => '',
      ],
      'mv_specs' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'indexed' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'query' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'special_index' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
    ],
    'unique keys' => [
      'name' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_organism()
 * Purpose: To describe the structure of 'organism' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_organism() {
  $description = [
    'table' => 'organism',
    'fields' => [
      'organism_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'abbreviation' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'genus' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'species' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'common_name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'comment' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'organism_id',
    ],
    'unique keys' => [
      'organism_c1' => [
        0 => 'genus',
        1 => 'species',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
    'referring_tables' => [
      0 => 'biomaterial',
      1 => 'feature',
      2 => 'library',
      3 => 'organism_dbxref',
      4 => 'organismprop',
      5 => 'phylonode_organism',
      6 => 'stock',
      7 => 'wwwuser_organism',
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_organism_dbxref()
 * Purpose: To describe the structure of 'organism_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_organism_dbxref() {
  $description = [
    'table' => 'organism_dbxref',
    'fields' => [
      'organism_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'organism_dbxref_id',
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_organismprop()
 * Purpose: To describe the structure of 'organismprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_organismprop() {
  $description = [
    'table' => 'organismprop',
    'fields' => [
      'organismprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'organismprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phendesc()
 * Purpose: To describe the structure of 'phendesc' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phendesc() {
  $description = [
    'table' => 'phendesc',
    'fields' => [
      'phendesc_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'genotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'environment_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phendesc_id',
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phenotype()
 * Purpose: To describe the structure of 'phenotype' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phenotype() {
  $description = [
    'table' => 'phenotype',
    'fields' => [
      'phenotype_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'observable_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'attr_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'cvalue_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phenotype_comparison()
 * Purpose: To describe the structure of 'phenotype_comparison' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phenotype_comparison() {
  $description = [
    'table' => 'phenotype_comparison',
    'fields' => [
      'phenotype_comparison_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'genotype1_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'environment1_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'genotype2_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'environment2_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'phenotype1_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'phenotype2_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_comparison_id',
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
    'foreign keys' => [
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
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype1_id' => 'phenotype_id',
          'phenotype2_id' => 'phenotype_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phenotype_comparison_cvterm()
 * Purpose: To describe the structure of 'phenotype_comparison_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phenotype_comparison_cvterm() {
  $description = [
    'table' => 'phenotype_comparison_cvterm',
    'fields' => [
      'phenotype_comparison_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phenotype_comparison_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_comparison_cvterm_id',
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
    'foreign keys' => [
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
      'phenotype_comparison' => [
        'table' => 'phenotype_comparison',
        'columns' => [
          'phenotype_comparison_id' => 'phenotype_comparison_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phenotype_cvterm()
 * Purpose: To describe the structure of 'phenotype_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phenotype_cvterm() {
  $description = [
    'table' => 'phenotype_cvterm',
    'fields' => [
      'phenotype_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phenotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'phenotype_cvterm_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'cvterm_id' => 'cvterm_id',
        ],
      ],
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phenstatement()
 * Purpose: To describe the structure of 'phenstatement' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phenstatement() {
  $description = [
    'table' => 'phenstatement',
    'fields' => [
      'phenstatement_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'genotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'environment_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'phenotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phenstatement_id',
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
      'phenotype' => [
        'table' => 'phenotype',
        'columns' => [
          'phenotype_id' => 'phenotype_id',
        ],
      ],
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylonode()
 * Purpose: To describe the structure of 'phylonode' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylonode() {
  $description = [
    'table' => 'phylonode',
    'fields' => [
      'phylonode_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phylotree_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'parent_phylonode_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'left_idx' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'right_idx' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'label' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'distance' => [
        'type' => 'float',
        'size' => 'big',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_id',
    ],
    'unique keys' => [
      'phylotree_id' => [
        0 => 'phylotree_id',
        1 => 'left_idx',
      ],
      'phylonode_phylotree_id_key1' => [
        0 => 'phylotree_id',
        1 => 'right_idx',
      ],
    ],
    'indexes' => [
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylonode_dbxref()
 * Purpose: To describe the structure of 'phylonode_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylonode_dbxref() {
  $description = [
    'table' => 'phylonode_dbxref',
    'fields' => [
      'phylonode_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phylonode_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_dbxref_id',
    ],
    'unique keys' => [
      'phylonode_id' => [
        0 => 'phylonode_id',
        1 => 'dbxref_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylonode_organism()
 * Purpose: To describe the structure of 'phylonode_organism' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylonode_organism() {
  $description = [
    'table' => 'phylonode_organism',
    'fields' => [
      'phylonode_organism_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phylonode_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_organism_id',
    ],
    'unique keys' => [
      'phylonode_id' => [
        0 => 'phylonode_id',
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
    'foreign keys' => [
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylonode_pub()
 * Purpose: To describe the structure of 'phylonode_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylonode_pub() {
  $description = [
    'table' => 'phylonode_pub',
    'fields' => [
      'phylonode_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phylonode_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_pub_id',
    ],
    'unique keys' => [
      'phylonode_id' => [
        0 => 'phylonode_id',
        1 => 'pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylonode_relationship()
 * Purpose: To describe the structure of 'phylonode_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylonode_relationship() {
  $description = [
    'table' => 'phylonode_relationship',
    'fields' => [
      'phylonode_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'phylotree_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phylonode_relationship_id',
    ],
    'unique keys' => [
      'subject_id' => [
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
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
    'foreign keys' => [
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
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'subject_id' => 'phylonode_id',
          'object_id' => 'phylonode_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylonodeprop()
 * Purpose: To describe the structure of 'phylonodeprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylonodeprop() {
  $description = [
    'table' => 'phylonodeprop',
    'fields' => [
      'phylonodeprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phylonode_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '1',
        'default' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'phylonodeprop_id',
    ],
    'unique keys' => [
      'phylonode_id' => [
        0 => 'phylonode_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'phylonode' => [
        'table' => 'phylonode',
        'columns' => [
          'phylonode_id' => 'phylonode_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylotree()
 * Purpose: To describe the structure of 'phylotree' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylotree() {
  $description = [
    'table' => 'phylotree',
    'fields' => [
      'phylotree_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'analysis_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'comment' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'phylotree_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
      'phylotree_idx1' => [
        0 => 'phylotree_id',
      ],
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_phylotree_pub()
 * Purpose: To describe the structure of 'phylotree_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_phylotree_pub() {
  $description = [
    'table' => 'phylotree_pub',
    'fields' => [
      'phylotree_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'phylotree_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'phylotree_pub_id',
    ],
    'unique keys' => [
      'phylotree_id' => [
        0 => 'phylotree_id',
        1 => 'pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'phylotree' => [
        'table' => 'phylotree',
        'columns' => [
          'phylotree_id' => 'phylotree_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_project()
 * Purpose: To describe the structure of 'project' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_project() {
  $description = [
    'table' => 'project',
    'fields' => [
      'project_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'project_id',
    ],
    'unique keys' => [
      'project_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_protocol()
 * Purpose: To describe the structure of 'protocol' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_protocol() {
  $description = [
    'table' => 'protocol',
    'fields' => [
      'protocol_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'uri' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'protocoldescription' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'hardwaredescription' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'softwaredescription' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'protocol_id',
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
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_protocolparam()
 * Purpose: To describe the structure of 'protocolparam' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_protocolparam() {
  $description = [
    'table' => 'protocolparam',
    'fields' => [
      'protocolparam_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'datatype_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'unittype_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'protocolparam_id',
    ],
    'unique keys' => [
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'datatype_id' => 'cvterm_id',
          'unittype_id' => 'cvterm_id',
        ],
      ],
      'protocol' => [
        'table' => 'protocol',
        'columns' => [
          'protocol_id' => 'protocol_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_pub()
 * Purpose: To describe the structure of 'pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_pub() {
  $description = [
    'table' => 'pub',
    'fields' => [
      'pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'title' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'volumetitle' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'volume' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'series_name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'issue' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'pyear' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'pages' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'miniref' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_obsolete' => [
        'type' => 'boolean',
        'not NULL' => '',
        'default' => 'false',
      ],
      'publisher' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'pubplace' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'pub_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_pub_dbxref()
 * Purpose: To describe the structure of 'pub_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_pub_dbxref() {
  $description = [
    'table' => 'pub_dbxref',
    'fields' => [
      'pub_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'true',
      ],
    ],
    'primary key' => [
      0 => 'pub_dbxref_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_pub_relationship()
 * Purpose: To describe the structure of 'pub_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_pub_relationship() {
  $description = [
    'table' => 'pub_relationship',
    'fields' => [
      'pub_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'pub_relationship_id',
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
          'subject_id' => 'pub_id',
          'object_id' => 'pub_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_pubauthor()
 * Purpose: To describe the structure of 'pubauthor' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_pubauthor() {
  $description = [
    'table' => 'pubauthor',
    'fields' => [
      'pubauthor_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'editor' => [
        'type' => 'boolean',
        'not NULL' => '',
        'default' => 'false',
      ],
      'surname' => [
        'type' => 'varchar',
        'length' => '100',
        'not NULL' => '1',
      ],
      'givennames' => [
        'type' => 'varchar',
        'length' => '100',
        'not NULL' => '',
      ],
      'suffix' => [
        'type' => 'varchar',
        'length' => '100',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'pubauthor_id',
    ],
    'unique keys' => [
      'pubauthor_c1' => [
        0 => 'pub_id',
        1 => 'rank',
      ],
    ],
    'indexes' => [
      'pubauthor_idx2' => [
        0 => 'pub_id',
      ],
    ],
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_pubprop()
 * Purpose: To describe the structure of 'pubprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_pubprop() {
  $description = [
    'table' => 'pubprop',
    'fields' => [
      'pubprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'pubprop_id',
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
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_quantification()
 * Purpose: To describe the structure of 'quantification' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_quantification() {
  $description = [
    'table' => 'quantification',
    'fields' => [
      'quantification_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'acquisition_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'operator_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'analysis_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'quantificationdate' => [
        'type' => 'datetime',
        'not NULL' => '',
        'default' => 'ow(',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'uri' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'quantification_id',
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
    'foreign keys' => [
      'analysis' => [
        'table' => 'analysis',
        'columns' => [
          'analysis_id' => 'analysis_id',
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
      'acquisition' => [
        'table' => 'acquisition',
        'columns' => [
          'acquisition_id' => 'acquisition_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_quantification_relationship()
 * Purpose: To describe the structure of 'quantification_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_quantification_relationship() {
  $description = [
    'table' => 'quantification_relationship',
    'fields' => [
      'quantification_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'quantification_relationship_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'quantification' => [
        'table' => 'quantification',
        'columns' => [
          'subject_id' => 'quantification_id',
          'object_id' => 'quantification_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_quantificationprop()
 * Purpose: To describe the structure of 'quantificationprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_quantificationprop() {
  $description = [
    'table' => 'quantificationprop',
    'fields' => [
      'quantificationprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'quantification_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'quantificationprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'quantification' => [
        'table' => 'quantification',
        'columns' => [
          'quantification_id' => 'quantification_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock()
 * Purpose: To describe the structure of 'stock' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock() {
  $description = [
    'table' => 'stock',
    'referring_tables' => [
      1 => 'stock_cvterm',
      2 => 'stock_dbxref',
      3 => 'stock_genotype',
      4 => 'stock_pub',
      5 => 'stock_relationship',
      7 => 'stockcollection_stock',
      8 => 'stockprop',
    ],
    'fields' => [
      'stock_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'organism_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_obsolete' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'false',
      ],
    ],
    'primary key' => [
      0 => 'stock_id',
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
      'organism' => [
        'table' => 'organism',
        'columns' => [
          'organism_id' => 'organism_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock_cvterm()
 * Purpose: To describe the structure of 'stock_cvterm' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock_cvterm() {
  $description = [
    'table' => 'stock_cvterm',
    'fields' => [
      'stock_cvterm_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'cvterm_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stock_cvterm_id',
    ],
    'unique keys' => [
      'stock_cvterm_c1' => [
        0 => 'stock_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
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
    'foreign keys' => [
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
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock_dbxref()
 * Purpose: To describe the structure of 'stock_dbxref' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock_dbxref() {
  $description = [
    'table' => 'stock_dbxref',
    'fields' => [
      'stock_dbxref_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'is_current' => [
        'type' => 'boolean',
        'not NULL' => '1',
        'default' => 'true',
      ],
    ],
    'primary key' => [
      0 => 'stock_dbxref_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock_genotype()
 * Purpose: To describe the structure of 'stock_genotype' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock_genotype() {
  $description = [
    'table' => 'stock_genotype',
    'fields' => [
      'stock_genotype_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'genotype_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stock_genotype_id',
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
    'foreign keys' => [
      'genotype' => [
        'table' => 'genotype',
        'columns' => [
          'genotype_id' => 'genotype_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock_pub()
 * Purpose: To describe the structure of 'stock_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock_pub() {
  $description = [
    'table' => 'stock_pub',
    'fields' => [
      'stock_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stock_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock_relationship()
 * Purpose: To describe the structure of 'stock_relationship' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock_relationship() {
  $description = [
    'table' => 'stock_relationship',
    'fields' => [
      'stock_relationship_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'subject_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'object_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'stock_relationship_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'subject_id' => 'stock_id',
          'object_id' => 'stock_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stock_relationship_pub()
 * Purpose: To describe the structure of 'stock_relationship_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stock_relationship_pub() {
  $description = [
    'table' => 'stock_relationship_pub',
    'fields' => [
      'stock_relationship_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stock_relationship_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stock_relationship_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'stock_relationship' => [
        'table' => 'stock_relationship',
        'columns' => [
          'stock_relationship_id' => 'stock_relationship_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stockcollection()
 * Purpose: To describe the structure of 'stockcollection' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stockcollection() {
  $description = [
    'table' => 'stockcollection',
    'fields' => [
      'stockcollection_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'contact_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '',
      ],
      'uniquename' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stockcollection_id',
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
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stockcollection_stock()
 * Purpose: To describe the structure of 'stockcollection_stock' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stockcollection_stock() {
  $description = [
    'table' => 'stockcollection_stock',
    'fields' => [
      'stockcollection_stock_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stockcollection_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stockcollection_stock_id',
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
    'foreign keys' => [
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
      'stockcollection' => [
        'table' => 'stockcollection',
        'columns' => [
          'stockcollection_id' => 'stockcollection_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stockcollectionprop()
 * Purpose: To describe the structure of 'stockcollectionprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stockcollectionprop() {
  $description = [
    'table' => 'stockcollectionprop',
    'fields' => [
      'stockcollectionprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stockcollection_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'stockcollectionprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'stockcollection' => [
        'table' => 'stockcollection',
        'columns' => [
          'stockcollection_id' => 'stockcollection_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stockprop()
 * Purpose: To describe the structure of 'stockprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stockprop() {
  $description = [
    'table' => 'stockprop',
    'fields' => [
      'stockprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'stockprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'stock' => [
        'table' => 'stock',
        'columns' => [
          'stock_id' => 'stock_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_stockprop_pub()
 * Purpose: To describe the structure of 'stockprop_pub' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_stockprop_pub() {
  $description = [
    'table' => 'stockprop_pub',
    'fields' => [
      'stockprop_pub_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'stockprop_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'stockprop_pub_id',
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
    'foreign keys' => [
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'stockprop' => [
        'table' => 'stockprop',
        'columns' => [
          'stockprop_id' => 'stockprop_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_study()
 * Purpose: To describe the structure of 'study' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_study() {
  $description = [
    'table' => 'study',
    'fields' => [
      'study_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'contact_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'study_id',
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
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'pub' => [
        'table' => 'pub',
        'columns' => [
          'pub_id' => 'pub_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_study_assay()
 * Purpose: To describe the structure of 'study_assay' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_study_assay() {
  $description = [
    'table' => 'study_assay',
    'fields' => [
      'study_assay_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'study_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'study_assay_id',
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
    'foreign keys' => [
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'study' => [
        'table' => 'study',
        'columns' => [
          'study_id' => 'study_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_studydesign()
 * Purpose: To describe the structure of 'studydesign' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_studydesign() {
  $description = [
    'table' => 'studydesign',
    'fields' => [
      'studydesign_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'study_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'studydesign_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
      'studydesign_idx1' => [
        0 => 'study_id',
      ],
    ],
    'foreign keys' => [
      'study' => [
        'table' => 'study',
        'columns' => [
          'study_id' => 'study_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_studydesignprop()
 * Purpose: To describe the structure of 'studydesignprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_studydesignprop() {
  $description = [
    'table' => 'studydesignprop',
    'fields' => [
      'studydesignprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'studydesign_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'studydesignprop_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'studydesign' => [
        'table' => 'studydesign',
        'columns' => [
          'studydesign_id' => 'studydesign_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_studyfactor()
 * Purpose: To describe the structure of 'studyfactor' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_studyfactor() {
  $description = [
    'table' => 'studyfactor',
    'fields' => [
      'studyfactor_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'studydesign_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '1',
      ],
      'description' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'studyfactor_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
      'studyfactor_idx1' => [
        0 => 'studydesign_id',
      ],
      'studyfactor_idx2' => [
        0 => 'type_id',
      ],
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'studydesign' => [
        'table' => 'studydesign',
        'columns' => [
          'studydesign_id' => 'studydesign_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_studyfactorvalue()
 * Purpose: To describe the structure of 'studyfactorvalue' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_studyfactorvalue() {
  $description = [
    'table' => 'studyfactorvalue',
    'fields' => [
      'studyfactorvalue_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'studyfactor_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'assay_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'factorvalue' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'studyfactorvalue_id',
    ],
    'unique keys' => [
    ],
    'indexes' => [
      'studyfactorvalue_idx1' => [
        0 => 'studyfactor_id',
      ],
      'studyfactorvalue_idx2' => [
        0 => 'assay_id',
      ],
    ],
    'foreign keys' => [
      'assay' => [
        'table' => 'assay',
        'columns' => [
          'assay_id' => 'assay_id',
        ],
      ],
      'studyfactor' => [
        'table' => 'studyfactor',
        'columns' => [
          'studyfactor_id' => 'studyfactor_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_studyprop()
 * Purpose: To describe the structure of 'studyprop' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_studyprop() {
  $description = [
    'table' => 'studyprop',
    'fields' => [
      'studyprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'study_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'value' => [
        'type' => 'text',
        'not NULL' => '',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
    ],
    'primary key' => [
      0 => 'studyprop_id',
    ],
    'unique keys' => [
      'study_id' => [
        0 => 'study_id',
        1 => 'type_id',
        2 => 'rank',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'study' => [
        'table' => 'study',
        'columns' => [
          'study_id' => 'study_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_studyprop_feature()
 * Purpose: To describe the structure of 'studyprop_feature' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_studyprop_feature() {
  $description = [
    'table' => 'studyprop_feature',
    'fields' => [
      'studyprop_feature_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'studyprop_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'studyprop_feature_id',
    ],
    'unique keys' => [
      'studyprop_id' => [
        0 => 'studyprop_id',
        1 => 'feature_id',
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
      'studyprop' => [
        'table' => 'studyprop',
        'columns' => [
          'studyprop_id' => 'studyprop_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_synonym()
 * Purpose: To describe the structure of 'synonym' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_synonym() {
  $description = [
    'table' => 'synonym',
    'fields' => [
      'synonym_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'synonym_sgml' => [
        'type' => 'varchar',
        'length' => '255',
        'not NULL' => '1',
      ],
    ],
    'primary key' => [
      0 => 'synonym_id',
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
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_tableinfo()
 * Purpose: To describe the structure of 'tableinfo' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_tableinfo() {
  $description = [
    'table' => 'tableinfo',
    'fields' => [
      'tableinfo_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => '30',
        'not NULL' => '1',
      ],
      'primary_key_column' => [
        'type' => 'varchar',
        'length' => '30',
        'not NULL' => '',
      ],
      'is_view' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
      'view_on_table_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'superclass_table_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'is_updateable' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '1',
      ],
      'modification_date' => [
        'type' => 'date',
        'not NULL' => '1',
        'default' => 'ow(',
      ],
    ],
    'primary key' => [
      0 => 'tableinfo_id',
    ],
    'unique keys' => [
      'tableinfo_c1' => [
        0 => 'name',
      ],
    ],
    'indexes' => [
    ],
    'foreign keys' => [
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_treatment()
 * Purpose: To describe the structure of 'treatment' to tripal
 *
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
function tripal_chado_chado_schema_v1_11_treatment() {
  $description = [
    'table' => 'treatment',
    'fields' => [
      'treatment_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'rank' => [
        'type' => 'int',
        'not NULL' => '1',
        'default' => '0',
      ],
      'biomaterial_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not NULL' => '1',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not NULL' => '',
      ],
      'name' => [
        'type' => 'text',
        'not NULL' => '',
      ],
    ],
    'primary key' => [
      0 => 'treatment_id',
    ],
    'unique keys' => [
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
    'foreign keys' => [
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
      'biomaterial' => [
        'table' => 'biomaterial',
        'columns' => [
          'biomaterial_id' => 'biomaterial_id',
        ],
      ],
    ],
  ];
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
function tripal_chado_chado_get_v1_11_tables() {
  $tables = [
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
    'treatment',
  ];
  return $tables;
}
