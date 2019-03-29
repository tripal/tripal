<?php

/**
 * @file
 * Adds to the tripal schema description.
 *
 * This file contains hooks for specifying the natural diversity tables
 * when Chado v1.11 is installed.  This is only useful for some specific
 * sites that required the ND tables before they were released.  These
 * files should not be needed for most sites as the ND tables are now part
 * of Chado v1.2. However these hooks remain here for backwards compatibility.
 */

/**
 * Implements hook_chado_schema_v1_11_analysisfeatureprop()
 * Purpose: To describe the structure of 'analysisfeatureprop' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisfeatureprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_chado_chado_schema_v1_11_analysisfeatureprop() {
  $description = [
    'table' => 'analysisfeatureprop',
    'fields' => [
      'analysisfeatureprop_id' => [
        'type' => 'serial',
        'not NULL' => '1',
      ],
      'analysisfeature_id' => [
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
      ],
    ],
    'primary key' => [
      0 => 'analysisfeatureprop_id',
    ],
    'unique keys' => [
      'analysisfeature_id_type_id_rank' => [
        0 => 'analysisfeature_id',
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
      'analysisfeature' => [
        'table' => 'analysisfeature',
        'columns' => [
          'analysisfeature_id' => 'analysisfeature_id',
        ],
      ],
    ],
  ];
  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_tripal_obo_temp()
 * Purpose: To describe the structure of 'tripal_obo_temp' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'tripal_obo_temp' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 */
function tripal_chado_chado_schema_v1_2_tripal_obo_temp() {
  $schema = [
    'table' => 'tripal_obo_temp',
    'fields' => [
      'id' => [
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'stanza' => [
        'type' => 'text',
        'not null' => TRUE,
      ],
      'type' => [
        'type' => 'varchar',
        'length' => '50',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'tripal_obo_temp_idx0' => ['id'],
      'tripal_obo_temp_idx0' => ['type'],
    ],
    'unique keys' => [
      'tripal_obo_temp_uq0' => ['id'],
    ],
  ];
  return $schema;
}

/**
 * Implements hook_chado_schema_v1_3_tripal_obo_temp()
 * Purpose: To describe the structure of 'tripal_obo_temp' to tripal
 *
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'tripal_obo_temp' table
 *
 * @ingroup tripal_chado_v1.3_schema_api
 */
function tripal_chado_chado_schema_v1_3_tripal_obo_temp() {
  $schema = [
    'table' => 'tripal_obo_temp',
    'fields' => [
      'id' => [
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ],
      'stanza' => [
        'type' => 'text',
        'not null' => TRUE,
      ],
      'type' => [
        'type' => 'varchar',
        'length' => '50',
        'not null' => TRUE,
      ],
    ],
    'indexes' => [
      'tripal_obo_temp_idx0' => ['id'],
      'tripal_obo_temp_idx0' => ['type'],
    ],
    'unique keys' => [
      'tripal_obo_temp_uq0' => ['id'],
    ],
  ];
  return $schema;
}


/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment() {
  $description = [];

  $description = [
    'table' => 'nd_experiment',
    'fields' => [
      'nd_experiment_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_geolocation_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_id',
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
      'nd_experiment_contact',
      'nd_experiment_dbxref',
      'nd_experiment_genotype',
      'nd_experiment_phenotype',
      'nd_experiment_project',
      'nd_experiment_protocol',
      'nd_experiment_pub',
      'nd_experiment_stock',
      'nd_experimentprop',
    ],
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_contact table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_contact() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_contact',
    'fields' => [
      'nd_experiment_contact_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'contact_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_contact_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_dbxref table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_dbxref() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_dbxref',
    'fields' => [
      'nd_experiment_dbxref_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_dbxref_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_genotype table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_genotype() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_genotype',
    'fields' => [
      'nd_experiment_genotype_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'genotype_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_genotype_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_phenotype table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_phenotype() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_phenotype',
    'fields' => [
      'nd_experiment_phenotype_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'phenotype_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_phenotype_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_project table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_project() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_project',
    'fields' => [
      'nd_experiment_project_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'project_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_project_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_protocol table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_protocol() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_protocol',
    'fields' => [
      'nd_experiment_protocol_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'protocol_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_protocol_id',
    ],
    'foreign keys' => [
      'nd_experiment' => [
        'table' => 'nd_experiment',
        'columns' => [
          'nd_experiment_id' => 'nd_experiment_id',
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

    ],
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_pub table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_pub() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_pub',
    'fields' => [
      'nd_experiment_pub_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'pub_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_pub_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_stock table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_stock() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_stock',
    'fields' => [
      'nd_experiment_stock_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'stock_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_stock_id',
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
      'nd_experiment_stock_dbxref',
      'nd_experiment_stockprop',
    ],
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_stockprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_stockprop() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_stockprop',
    'fields' => [
      'nd_experiment_stockprop_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'nd_experiment_stock_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'value' => [
        'type' => 'varchar',
      ],
      'rank' => [
        'type' => 'int',
        'not null' => 1,
        'default' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_stockprop_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experiment_stock_dbxref table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experiment_stock_dbxref() {
  $description = [];

  $description = [
    'table' => 'nd_experiment_stock_dbxref',
    'fields' => [
      'nd_experiment_stock_dbxref_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'nd_experiment_stock_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_experiment_stock_dbxref_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_experimentprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_experimentprop() {
  $description = [];

  $description = [
    'table' => 'nd_experimentprop',
    'fields' => [
      'nd_experimentprop_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'nd_experiment_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'value' => [
        'type' => 'varchar',
      ],
      'rank' => [
        'type' => 'int',
        'not null' => 1,
        'default' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_experimentprop_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_geolocation table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_geolocation() {
  $description = [];

  $description = [
    'table' => 'nd_reagent',
    'fields' => [
      'nd_geolocation_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'description' => [
        'type' => 'varchar',
      ],
      'latitude' => [
        'type' => 'real',
      ],
      'longitude' => [
        'type' => 'real',
      ],
      'altitude' => [
        'type' => 'real',
      ],
      'geodetic_datum' => [
        'type' => 'varchar',
      ],
    ],
    'primary key' => [
      '0' => 'nd_geolocation_id',
    ],
    'referring_tables' => [
      'nd_experiment',
      'nd_geolocationprop',
    ],
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_geolocationprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_geolocationprop() {
  $description = [];

  $description = [
    'table' => 'nd_geolocationprop',
    'fields' => [
      'nd_geolocationprop_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'nd_geolocation_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'value' => [
        'type' => 'varchar',
      ],
      'rank' => [
        'type' => 'int',
        'not null' => 1,
        'default' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_geolocationprop_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_protocol table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_protocol() {
  $description = [];

  $description = [
    'table' => 'nd_protocol',
    'fields' => [
      'nd_protocol_id' => [
        'type' => 'serial',
        'not null' => 1,
      ],
      'name' => [
        'type' => 'varchar',
        'not null' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_protocol_id',
    ],
    'referring_tables' => [
      'nd_experiment_protocol',
      'nd_protocol_reagent',
      'nd_protocolprop',
    ],
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_protocol_reagent table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_protocol_reagent() {
  $description = [];

  $description = [
    'table' => 'nd_protocol_reagent',
    'fields' => [
      'nd_protocol_reagent_id' => [
        'type' => 'serial',
        'not null' => 1,
      ],
      'nd_protocol_id' => [
        'type' => 'int',
        'not null' => 1,
      ],
      'nd_reagent_id' => [
        'type' => 'int',
        'not null' => 1,
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_protocol_reagent_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_protocolprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_protocolprop() {
  $description = [];

  $description = [
    'table' => 'nd_protocolprop',
    'fields' => [
      'nd_protocolprop_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'nd_protocol_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'value' => [
        'type' => 'varchar',
      ],
      'rank' => [
        'type' => 'int',
        'not null' => 1,
        'default' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_protocolprop_id',
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
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_reagent table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_reagent() {
  $description = [];

  $description = [
    'table' => 'nd_reagent',
    'fields' => [
      'nd_reagent_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'name' => [
        'type' => 'varchar',
        'length' => 80,
        'not null' => TRUE,
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'feature_id' => [
        'type' => 'int',
      ],
    ],
    'primary key' => [
      '0' => 'nd_reagent_id',
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
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_reagent_relationship table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_reagent_relationship() {
  $description = [];

  $description = [
    'table' => 'nd_reagent_relationship',
    'fields' => [
      'nd_reagent_relationship_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'subject_reagent_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'object_reagent_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
    ],
    'primary key' => [
      '0' => 'nd_reagent_relationship_id',
    ],
    'foreign keys' => [
      'nd_reagent' => [
        'table' => 'feature',
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
  ];

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the nd_reagentprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_chado_chado_schema_v1_11_nd_reagentprop() {
  $description = [];

  $description = [
    'table' => 'nd_reagentprop',
    'fields' => [
      'nd_reagentprop_id' => [
        'type' => 'serial',
        'not null' => '1',
      ],
      'nd_reagent_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => '1',
      ],
      'value' => [
        'type' => 'varchar',
      ],
      'rank' => [
        'type' => 'int',
        'not null' => 1,
        'default' => 1,
      ],
    ],
    'primary key' => [
      '0' => 'nd_reagentprop_id',
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
