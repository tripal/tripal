<?php
/**
 * Implements hook_chado_schema_v1_2_acquisition()
 * Purpose: To describe the structure of 'acquisition' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'acquisition' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_acquisition() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'acquisition_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'channel_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'acquisitiondate' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => false,
        'default' => 'ow(',
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'uri' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'acquisition_id',
    ),
    'unique keys' => array (
      'acquisition_c1' => array (
        0 => 'name',
      ),
    ),
    'indexes' => array (
      'acquisition_idx1' => array (
        0 => 'assay_id',
      ),
      'acquisition_idx2' => array (
        0 => 'protocol_id',
      ),
      'acquisition_idx3' => array (
        0 => 'channel_id',
      ),
    ),
    'foreign keys' => array (
      'protocol' => array (
        'table' => 'protocol',
        'columns' => array (
          'protocol_id' => 'protocol_id',
        ),
      ),
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
      'channel' => array (
        'table' => 'channel',
        'columns' => array (
          'channel_id' => 'channel_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_acquisition_relationship()
 * Purpose: To describe the structure of 'acquisition_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'acquisition_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_acquisition_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'acquisition_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'acquisition_relationship_id',
    ),
    'unique keys' => array (
      'acquisition_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'acquisition_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'acquisition_relationship_idx2' => array (
        0 => 'type_id',
      ),
      'acquisition_relationship_idx3' => array (
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'acquisition' => array (
        'table' => 'acquisition',
        'columns' => array (
          'subject_id' => 'acquisition_id',
          'object_id' => 'acquisition_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_acquisitionprop()
 * Purpose: To describe the structure of 'acquisitionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'acquisitionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_acquisitionprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'acquisitionprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'acquisition_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'acquisitionprop_id',
    ),
    'unique keys' => array (
      'acquisitionprop_c1' => array (
        0 => 'acquisition_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'acquisitionprop_idx1' => array (
        0 => 'acquisition_id',
      ),
      'acquisitionprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'acquisition' => array (
        'table' => 'acquisition',
        'columns' => array (
          'acquisition_id' => 'acquisition_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysis()
 * Purpose: To describe the structure of 'analysis' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysis' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysis() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'analysis_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'program' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'programversion' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'algorithm' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'sourcename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'sourceversion' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'sourceuri' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'timeexecuted' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
    ),
    'primary key' => array (
      0 => 'analysis_id',
    ),
    'unique keys' => array (
      'analysis_c1' => array (
        0 => 'program',
        1 => 'programversion',
        2 => 'sourcename',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysisfeature()
 * Purpose: To describe the structure of 'analysisfeature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysisfeature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysisfeature() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'analysisfeature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'analysis_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rawscore' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => false,
      ),
      'normscore' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => false,
      ),
      'significance' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => false,
      ),
      'identity' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'analysisfeature_id',
    ),
    'unique keys' => array (
      'analysisfeature_c1' => array (
        0 => 'feature_id',
        1 => 'analysis_id',
      ),
    ),
    'indexes' => array (
      'analysisfeature_idx1' => array (
        0 => 'feature_id',
      ),
      'analysisfeature_idx2' => array (
        0 => 'analysis_id',
      ),
    ),
    'foreign keys' => array (
      'analysis' => array (
        'table' => 'analysis',
        'columns' => array (
          'analysis_id' => 'analysis_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysisfeatureprop()
 * Purpose: To describe the structure of 'analysisfeatureprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysisfeatureprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysisfeatureprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'analysisfeatureprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'analysisfeature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'analysisfeatureprop_id',
    ),
    'unique keys' => array (
      'analysisfeature_id_type_id_rank' => array (
        0 => 'analysisfeature_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'analysisfeature' => array (
        'table' => 'analysisfeature',
        'columns' => array (
          'analysisfeature_id' => 'analysisfeature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_analysisprop()
 * Purpose: To describe the structure of 'analysisprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysisprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_analysisprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'analysisprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'analysis_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'analysisprop_id',
    ),
    'unique keys' => array (
      'analysisprop_c1' => array (
        0 => 'analysis_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'analysisprop_idx1' => array (
        0 => 'analysis_id',
      ),
      'analysisprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'analysis' => array (
        'table' => 'analysis',
        'columns' => array (
          'analysis_id' => 'analysis_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_arraydesign()
 * Purpose: To describe the structure of 'arraydesign' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'arraydesign' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_arraydesign() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'arraydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'manufacturer_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'platformtype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'substratetype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'version' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'array_dimensions' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'element_dimensions' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'num_of_elements' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'num_array_columns' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'num_array_rows' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'num_grid_columns' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'num_grid_rows' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'num_sub_columns' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'num_sub_rows' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'arraydesign_id',
    ),
    'unique keys' => array (
      'arraydesign_c1' => array (
        0 => 'name',
      ),
    ),
    'indexes' => array (
      'arraydesign_idx1' => array (
        0 => 'manufacturer_id',
      ),
      'arraydesign_idx2' => array (
        0 => 'platformtype_id',
      ),
      'arraydesign_idx3' => array (
        0 => 'substratetype_id',
      ),
      'arraydesign_idx4' => array (
        0 => 'protocol_id',
      ),
      'arraydesign_idx5' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'platformtype_id' => 'cvterm_id',
          'substratetype_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'manufacturer_id' => 'contact_id',
        ),
      ),
      'protocol' => array (
        'table' => 'protocol',
        'columns' => array (
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_arraydesignprop()
 * Purpose: To describe the structure of 'arraydesignprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'arraydesignprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_arraydesignprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'arraydesignprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'arraydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'arraydesignprop_id',
    ),
    'unique keys' => array (
      'arraydesignprop_c1' => array (
        0 => 'arraydesign_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'arraydesignprop_idx1' => array (
        0 => 'arraydesign_id',
      ),
      'arraydesignprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'arraydesign' => array (
        'table' => 'arraydesign',
        'columns' => array (
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assay()
 * Purpose: To describe the structure of 'assay' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assay' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assay() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'arraydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'assaydate' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => false,
        'default' => 'ow(',
      ),
      'arrayidentifier' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'arraybatchidentifier' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'operator_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'assay_id',
    ),
    'unique keys' => array (
      'assay_c1' => array (
        0 => 'name',
      ),
    ),
    'indexes' => array (
      'assay_idx1' => array (
        0 => 'arraydesign_id',
      ),
      'assay_idx2' => array (
        0 => 'protocol_id',
      ),
      'assay_idx3' => array (
        0 => 'operator_id',
      ),
      'assay_idx4' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'operator_id' => 'contact_id',
        ),
      ),
      'arraydesign' => array (
        'table' => 'arraydesign',
        'columns' => array (
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
      'protocol' => array (
        'table' => 'protocol',
        'columns' => array (
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assay_biomaterial()
 * Purpose: To describe the structure of 'assay_biomaterial' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assay_biomaterial' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assay_biomaterial() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'assay_biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'channel_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'assay_biomaterial_id',
    ),
    'unique keys' => array (
      'assay_biomaterial_c1' => array (
        0 => 'assay_id',
        1 => 'biomaterial_id',
        2 => 'channel_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'assay_biomaterial_idx1' => array (
        0 => 'assay_id',
      ),
      'assay_biomaterial_idx2' => array (
        0 => 'biomaterial_id',
      ),
      'assay_biomaterial_idx3' => array (
        0 => 'channel_id',
      ),
    ),
    'foreign keys' => array (
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
      'channel' => array (
        'table' => 'channel',
        'columns' => array (
          'channel_id' => 'channel_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assay_project()
 * Purpose: To describe the structure of 'assay_project' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assay_project' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assay_project() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'assay_project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'assay_project_id',
    ),
    'unique keys' => array (
      'assay_project_c1' => array (
        0 => 'assay_id',
        1 => 'project_id',
      ),
    ),
    'indexes' => array (
      'assay_project_idx1' => array (
        0 => 'assay_id',
      ),
      'assay_project_idx2' => array (
        0 => 'project_id',
      ),
    ),
    'foreign keys' => array (
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'project_id' => 'project_id',
        ),
      ),
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_assayprop()
 * Purpose: To describe the structure of 'assayprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assayprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_assayprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'assayprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'assayprop_id',
    ),
    'unique keys' => array (
      'assayprop_c1' => array (
        0 => 'assay_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'assayprop_idx1' => array (
        0 => 'assay_id',
      ),
      'assayprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial()
 * Purpose: To describe the structure of 'biomaterial' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'taxon_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'biosourceprovider_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'biomaterial_id',
    ),
    'unique keys' => array (
      'biomaterial_c1' => array (
        0 => 'name',
      ),
    ),
    'indexes' => array (
      'biomaterial_idx1' => array (
        0 => 'taxon_id',
      ),
      'biomaterial_idx2' => array (
        0 => 'biosourceprovider_id',
      ),
      'biomaterial_idx3' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'taxon_id' => 'organism_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'biosourceprovider_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial_dbxref()
 * Purpose: To describe the structure of 'biomaterial_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'biomaterial_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'biomaterial_dbxref_id',
    ),
    'unique keys' => array (
      'biomaterial_dbxref_c1' => array (
        0 => 'biomaterial_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'biomaterial_dbxref_idx1' => array (
        0 => 'biomaterial_id',
      ),
      'biomaterial_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial_relationship()
 * Purpose: To describe the structure of 'biomaterial_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'biomaterial_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'biomaterial_relationship_id',
    ),
    'unique keys' => array (
      'biomaterial_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'biomaterial_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'biomaterial_relationship_idx2' => array (
        0 => 'object_id',
      ),
      'biomaterial_relationship_idx3' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
          'subject_id' => 'biomaterial_id',
          'object_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterial_treatment()
 * Purpose: To describe the structure of 'biomaterial_treatment' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial_treatment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterial_treatment() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'biomaterial_treatment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'treatment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'unittype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'biomaterial_treatment_id',
    ),
    'unique keys' => array (
      'biomaterial_treatment_c1' => array (
        0 => 'biomaterial_id',
        1 => 'treatment_id',
      ),
    ),
    'indexes' => array (
      'biomaterial_treatment_idx1' => array (
        0 => 'biomaterial_id',
      ),
      'biomaterial_treatment_idx2' => array (
        0 => 'treatment_id',
      ),
      'biomaterial_treatment_idx3' => array (
        0 => 'unittype_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'unittype_id' => 'cvterm_id',
        ),
      ),
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
      'treatment' => array (
        'table' => 'treatment',
        'columns' => array (
          'treatment_id' => 'treatment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_biomaterialprop()
 * Purpose: To describe the structure of 'biomaterialprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterialprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_biomaterialprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'biomaterialprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'biomaterialprop_id',
    ),
    'unique keys' => array (
      'biomaterialprop_c1' => array (
        0 => 'biomaterial_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'biomaterialprop_idx1' => array (
        0 => 'biomaterial_id',
      ),
      'biomaterialprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line()
 * Purpose: To describe the structure of 'cell_line' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'timeaccessioned' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
      'timelastmodified' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_id',
    ),
    'unique keys' => array (
      'cell_line_c1' => array (
        0 => 'uniquename',
        1 => 'organism_id',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_cvterm()
 * Purpose: To describe the structure of 'cell_line_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_cvterm_id',
    ),
    'unique keys' => array (
      'cell_line_cvterm_c1' => array (
        0 => 'cell_line_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_cvtermprop()
 * Purpose: To describe the structure of 'cell_line_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_cvtermprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_cvtermprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_cvtermprop_id',
    ),
    'unique keys' => array (
      'cell_line_cvtermprop_c1' => array (
        0 => 'cell_line_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'cell_line_cvterm' => array (
        'table' => 'cell_line_cvterm',
        'columns' => array (
          'cell_line_cvterm_id' => 'cell_line_cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_dbxref()
 * Purpose: To describe the structure of 'cell_line_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'ru',
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_dbxref_id',
    ),
    'unique keys' => array (
      'cell_line_dbxref_c1' => array (
        0 => 'cell_line_id',
        1 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_feature()
 * Purpose: To describe the structure of 'cell_line_feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_feature() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_feature_id',
    ),
    'unique keys' => array (
      'cell_line_feature_c1' => array (
        0 => 'cell_line_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_library()
 * Purpose: To describe the structure of 'cell_line_library' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_library' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_library() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_library_id',
    ),
    'unique keys' => array (
      'cell_line_library_c1' => array (
        0 => 'cell_line_id',
        1 => 'library_id',
        2 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_pub()
 * Purpose: To describe the structure of 'cell_line_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_pub_id',
    ),
    'unique keys' => array (
      'cell_line_pub_c1' => array (
        0 => 'cell_line_id',
        1 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_relationship()
 * Purpose: To describe the structure of 'cell_line_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_relationship_id',
    ),
    'unique keys' => array (
      'cell_line_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'subject_id' => 'cell_line_id',
          'object_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_line_synonym()
 * Purpose: To describe the structure of 'cell_line_synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_line_synonym() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_line_synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'is_internal' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
    ),
    'primary key' => array (
      0 => 'cell_line_synonym_id',
    ),
    'unique keys' => array (
      'cell_line_synonym_c1' => array (
        0 => 'synonym_id',
        1 => 'cell_line_id',
        2 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'synonym' => array (
        'table' => 'synonym',
        'columns' => array (
          'synonym_id' => 'synonym_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_lineprop()
 * Purpose: To describe the structure of 'cell_lineprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_lineprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_lineprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_lineprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_line_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cell_lineprop_id',
    ),
    'unique keys' => array (
      'cell_lineprop_c1' => array (
        0 => 'cell_line_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
          'cell_line_id' => 'cell_line_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cell_lineprop_pub()
 * Purpose: To describe the structure of 'cell_lineprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_lineprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cell_lineprop_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cell_lineprop_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cell_lineprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'cell_lineprop_pub_id',
    ),
    'unique keys' => array (
      'cell_lineprop_pub_c1' => array (
        0 => 'cell_lineprop_id',
        1 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'cell_lineprop' => array (
        'table' => 'cell_lineprop',
        'columns' => array (
          'cell_lineprop_id' => 'cell_lineprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_chadoprop()
 * Purpose: To describe the structure of 'chadoprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'chadoprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_chadoprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'chadoprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'chadoprop_id',
    ),
    'unique keys' => array (
      'chadoprop_c1' => array (
        0 => 'type_id',
        1 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_channel()
 * Purpose: To describe the structure of 'channel' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'channel' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_channel() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'channel_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'definition' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'channel_id',
    ),
    'unique keys' => array (
      'channel_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_contact()
 * Purpose: To describe the structure of 'contact' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'contact' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_contact() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'contact_id',
    ),
    'unique keys' => array (
      'contact_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_contact_relationship()
 * Purpose: To describe the structure of 'contact_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'contact_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_contact_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'contact_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'contact_relationship_id',
    ),
    'unique keys' => array (
      'contact_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'contact_relationship_idx1' => array (
        0 => 'type_id',
      ),
      'contact_relationship_idx2' => array (
        0 => 'subject_id',
      ),
      'contact_relationship_idx3' => array (
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'subject_id' => 'contact_id',
          'object_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_control()
 * Purpose: To describe the structure of 'control' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'control' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_control() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'control_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'tableinfo_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'row_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'control_id',
    ),
    'indexes' => array (
      'control_idx1' => array (
        0 => 'type_id',
      ),
      'control_idx2' => array (
        0 => 'assay_id',
      ),
      'control_idx3' => array (
        0 => 'tableinfo_id',
      ),
      'control_idx4' => array (
        0 => 'row_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'tableinfo' => array (
        'table' => 'tableinfo',
        'columns' => array (
          'tableinfo_id' => 'tableinfo_id',
        ),
      ),
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cv()
 * Purpose: To describe the structure of 'cv' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cv' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cv() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cv_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'definition' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'cv_id',
    ),
    'unique keys' => array (
      'cv_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvprop()
 * Purpose: To describe the structure of 'cvprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cv_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cvprop_id',
    ),
    'unique keys' => array (
      'cvprop_c1' => array (
        0 => 'cv_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'cv' => array (
        'table' => 'cv',
        'columns' => array (
          'cv_id' => 'cv_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvterm()
 * Purpose: To describe the structure of 'cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cv_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => true,
      ),
      'definition' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_obsolete' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
      'is_relationshiptype' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cvterm_id',
    ),
    'unique keys' => array (
      'cvterm_c1' => array (
        0 => 'name',
        1 => 'cv_id',
        2 => 'is_obsolete',
      ),
      'cvterm_c2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'cvterm_idx1' => array (
        0 => 'cv_id',
      ),
      'cvterm_idx2' => array (
        0 => 'name',
      ),
      'cvterm_idx3' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'cv' => array (
        'table' => 'cv',
        'columns' => array (
          'cv_id' => 'cv_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvterm_dbxref()
 * Purpose: To describe the structure of 'cvterm_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvterm_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvterm_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_for_definition' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cvterm_dbxref_id',
    ),
    'unique keys' => array (
      'cvterm_dbxref_c1' => array (
        0 => 'cvterm_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'cvterm_dbxref_idx1' => array (
        0 => 'cvterm_id',
      ),
      'cvterm_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvterm_relationship()
 * Purpose: To describe the structure of 'cvterm_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvterm_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvterm_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvterm_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'cvterm_relationship_id',
    ),
    'unique keys' => array (
      'cvterm_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'cvterm_relationship_idx1' => array (
        0 => 'type_id',
      ),
      'cvterm_relationship_idx2' => array (
        0 => 'subject_id',
      ),
      'cvterm_relationship_idx3' => array (
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
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
 * Implements hook_chado_schema_v1_2_cvtermpath()
 * Purpose: To describe the structure of 'cvtermpath' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvtermpath' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvtermpath() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvtermpath_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cv_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pathdistance' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'cvtermpath_id',
    ),
    'unique keys' => array (
      'cvtermpath_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'pathdistance',
      ),
    ),
    'indexes' => array (
      'cvtermpath_idx1' => array (
        0 => 'type_id',
      ),
      'cvtermpath_idx2' => array (
        0 => 'subject_id',
      ),
      'cvtermpath_idx3' => array (
        0 => 'object_id',
      ),
      'cvtermpath_idx4' => array (
        0 => 'cv_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
          'subject_id' => 'cvterm_id',
          'object_id' => 'cvterm_id',
        ),
      ),
      'cv' => array (
        'table' => 'cv',
        'columns' => array (
          'cv_id' => 'cv_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvtermprop()
 * Purpose: To describe the structure of 'cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvtermprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvtermprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
        'default' => '',
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'cvtermprop_id',
    ),
    'unique keys' => array (
      'cvterm_id' => array (
        0 => 'cvterm_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'cvtermprop_idx1' => array (
        0 => 'cvterm_id',
      ),
      'cvtermprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_cvtermsynonym()
 * Purpose: To describe the structure of 'cvtermsynonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvtermsynonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_cvtermsynonym() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'cvtermsynonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'synonym' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'cvtermsynonym_id',
    ),
    'unique keys' => array (
      'cvtermsynonym_c1' => array (
        0 => 'cvterm_id',
        1 => 'synonym',
      ),
    ),
    'indexes' => array (
      'cvtermsynonym_idx1' => array (
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_db()
 * Purpose: To describe the structure of 'db' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'db' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_db() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'db_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'urlprefix' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'url' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'db_id',
    ),
    'unique keys' => array (
      'db_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_dbxref()
 * Purpose: To describe the structure of 'dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'db_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'accession' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'version' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
        'default' => '',
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'dbxref_id',
    ),
    'unique keys' => array (
      'dbxref_c1' => array (
        0 => 'db_id',
        1 => 'accession',
        2 => 'version',
      ),
    ),
    'indexes' => array (
      'dbxref_idx1' => array (
        0 => 'db_id',
      ),
      'dbxref_idx2' => array (
        0 => 'accession',
      ),
      'dbxref_idx3' => array (
        0 => 'version',
      ),
    ),
    'foreign keys' => array (
      'db' => array (
        'table' => 'db',
        'columns' => array (
          'db_id' => 'db_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_dbxrefprop()
 * Purpose: To describe the structure of 'dbxrefprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_dbxrefprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'dbxrefprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
        'default' => '',
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'dbxrefprop_id',
    ),
    'unique keys' => array (
      'dbxrefprop_c1' => array (
        0 => 'dbxref_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'dbxrefprop_idx1' => array (
        0 => 'dbxref_id',
      ),
      'dbxrefprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_eimage()
 * Purpose: To describe the structure of 'eimage' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'eimage' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_eimage() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'eimage_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'eimage_data' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'eimage_type' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'image_uri' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'eimage_id',
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_element()
 * Purpose: To describe the structure of 'element' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'element' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_element() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'element_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'arraydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'element_id',
    ),
    'unique keys' => array (
      'element_c1' => array (
        0 => 'feature_id',
        1 => 'arraydesign_id',
      ),
    ),
    'indexes' => array (
      'element_idx1' => array (
        0 => 'feature_id',
      ),
      'element_idx2' => array (
        0 => 'arraydesign_id',
      ),
      'element_idx3' => array (
        0 => 'type_id',
      ),
      'element_idx4' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'arraydesign' => array (
        'table' => 'arraydesign',
        'columns' => array (
          'arraydesign_id' => 'arraydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_element_relationship()
 * Purpose: To describe the structure of 'element_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'element_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_element_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'element_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'element_relationship_id',
    ),
    'unique keys' => array (
      'element_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'element_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'element_relationship_idx2' => array (
        0 => 'type_id',
      ),
      'element_relationship_idx3' => array (
        0 => 'object_id',
      ),
      'element_relationship_idx4' => array (
        0 => 'value',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'element' => array (
        'table' => 'element',
        'columns' => array (
          'subject_id' => 'element_id',
          'object_id' => 'element_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_elementresult()
 * Purpose: To describe the structure of 'elementresult' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'elementresult' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_elementresult() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'elementresult_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'element_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'quantification_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'signal' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'elementresult_id',
    ),
    'unique keys' => array (
      'elementresult_c1' => array (
        0 => 'element_id',
        1 => 'quantification_id',
      ),
    ),
    'indexes' => array (
      'elementresult_idx1' => array (
        0 => 'element_id',
      ),
      'elementresult_idx2' => array (
        0 => 'quantification_id',
      ),
      'elementresult_idx3' => array (
        0 => 'signal',
      ),
    ),
    'foreign keys' => array (
      'quantification' => array (
        'table' => 'quantification',
        'columns' => array (
          'quantification_id' => 'quantification_id',
        ),
      ),
      'element' => array (
        'table' => 'element',
        'columns' => array (
          'element_id' => 'element_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_elementresult_relationship()
 * Purpose: To describe the structure of 'elementresult_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'elementresult_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_elementresult_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'elementresult_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'elementresult_relationship_id',
    ),
    'unique keys' => array (
      'elementresult_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'elementresult_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'elementresult_relationship_idx2' => array (
        0 => 'type_id',
      ),
      'elementresult_relationship_idx3' => array (
        0 => 'object_id',
      ),
      'elementresult_relationship_idx4' => array (
        0 => 'value',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'elementresult' => array (
        'table' => 'elementresult',
        'columns' => array (
          'subject_id' => 'elementresult_id',
          'object_id' => 'elementresult_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_environment()
 * Purpose: To describe the structure of 'environment' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'environment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_environment() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'environment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'environment_id',
    ),
    'unique keys' => array (
      'environment_c1' => array (
        0 => 'uniquename',
      ),
    ),
    'indexes' => array (
      'environment_idx1' => array (
        0 => 'uniquename',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_environment_cvterm()
 * Purpose: To describe the structure of 'environment_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'environment_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_environment_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'environment_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'environment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'environment_cvterm_id',
    ),
    'unique keys' => array (
      'environment_cvterm_c1' => array (
        0 => 'environment_id',
        1 => 'cvterm_id',
      ),
    ),
    'indexes' => array (
      'environment_cvterm_idx1' => array (
        0 => 'environment_id',
      ),
      'environment_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'environment' => array (
        'table' => 'environment',
        'columns' => array (
          'environment_id' => 'environment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression()
 * Purpose: To describe the structure of 'expression' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'md5checksum' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => false,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'expression_id',
    ),
    'unique keys' => array (
      'expression_c1' => array (
        0 => 'uniquename',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_cvterm()
 * Purpose: To describe the structure of 'expression_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'expression_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
      'cvterm_type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'expression_cvterm_id',
    ),
    'unique keys' => array (
      'expression_cvterm_c1' => array (
        0 => 'expression_id',
        1 => 'cvterm_id',
        2 => 'cvterm_type_id',
      ),
    ),
    'indexes' => array (
      'expression_cvterm_idx1' => array (
        0 => 'expression_id',
      ),
      'expression_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
      'expression_cvterm_idx3' => array (
        0 => 'cvterm_type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
          'cvterm_type_id' => 'cvterm_id',
        ),
      ),
      'expression' => array (
        'table' => 'expression',
        'columns' => array (
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_cvtermprop()
 * Purpose: To describe the structure of 'expression_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_cvtermprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'expression_cvtermprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'expression_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'expression_cvtermprop_id',
    ),
    'unique keys' => array (
      'expression_cvtermprop_c1' => array (
        0 => 'expression_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'expression_cvtermprop_idx1' => array (
        0 => 'expression_cvterm_id',
      ),
      'expression_cvtermprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'expression_cvterm' => array (
        'table' => 'expression_cvterm',
        'columns' => array (
          'expression_cvterm_id' => 'expression_cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_image()
 * Purpose: To describe the structure of 'expression_image' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_image' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_image() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'expression_image_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'eimage_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'expression_image_id',
    ),
    'unique keys' => array (
      'expression_image_c1' => array (
        0 => 'expression_id',
        1 => 'eimage_id',
      ),
    ),
    'indexes' => array (
      'expression_image_idx1' => array (
        0 => 'expression_id',
      ),
      'expression_image_idx2' => array (
        0 => 'eimage_id',
      ),
    ),
    'foreign keys' => array (
      'expression' => array (
        'table' => 'expression',
        'columns' => array (
          'expression_id' => 'expression_id',
        ),
      ),
      'eimage' => array (
        'table' => 'eimage',
        'columns' => array (
          'eimage_id' => 'eimage_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expression_pub()
 * Purpose: To describe the structure of 'expression_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expression_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'expression_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'expression_pub_id',
    ),
    'unique keys' => array (
      'expression_pub_c1' => array (
        0 => 'expression_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'expression_pub_idx1' => array (
        0 => 'expression_id',
      ),
      'expression_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'expression' => array (
        'table' => 'expression',
        'columns' => array (
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_expressionprop()
 * Purpose: To describe the structure of 'expressionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expressionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_expressionprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'expressionprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'expressionprop_id',
    ),
    'unique keys' => array (
      'expressionprop_c1' => array (
        0 => 'expression_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'expressionprop_idx1' => array (
        0 => 'expression_id',
      ),
      'expressionprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'expression' => array (
        'table' => 'expression',
        'columns' => array (
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature()
 * Purpose: To describe the structure of 'feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'residues' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'seqlen' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'md5checksum' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => false,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_analysis' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'is_obsolete' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'timeaccessioned' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
      'timelastmodified' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
    ),
    'primary key' => array (
      0 => 'feature_id',
    ),
    'unique keys' => array (
      'feature_c1' => array (
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'feature_idx1' => array (
        0 => 'dbxref_id',
      ),
      'feature_idx2' => array (
        0 => 'organism_id',
      ),
      'feature_idx3' => array (
        0 => 'type_id',
      ),
      'feature_idx4' => array (
        0 => 'uniquename',
      ),
      'feature_name_ind1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvterm()
 * Purpose: To describe the structure of 'feature_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_not' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'feature_cvterm_id',
    ),
    'unique keys' => array (
      'feature_cvterm_c1' => array (
        0 => 'feature_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'feature_cvterm_idx1' => array (
        0 => 'feature_id',
      ),
      'feature_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
      'feature_cvterm_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvterm_dbxref()
 * Purpose: To describe the structure of 'feature_cvterm_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvterm_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_cvterm_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_cvterm_dbxref_id',
    ),
    'unique keys' => array (
      'feature_cvterm_dbxref_c1' => array (
        0 => 'feature_cvterm_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'feature_cvterm_dbxref_idx1' => array (
        0 => 'feature_cvterm_id',
      ),
      'feature_cvterm_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'feature_cvterm' => array (
        'table' => 'feature_cvterm',
        'columns' => array (
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvterm_pub()
 * Purpose: To describe the structure of 'feature_cvterm_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvterm_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvterm_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_cvterm_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_cvterm_pub_id',
    ),
    'unique keys' => array (
      'feature_cvterm_pub_c1' => array (
        0 => 'feature_cvterm_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'feature_cvterm_pub_idx1' => array (
        0 => 'feature_cvterm_id',
      ),
      'feature_cvterm_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'feature_cvterm' => array (
        'table' => 'feature_cvterm',
        'columns' => array (
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_cvtermprop()
 * Purpose: To describe the structure of 'feature_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_cvtermprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_cvtermprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'feature_cvtermprop_id',
    ),
    'unique keys' => array (
      'feature_cvtermprop_c1' => array (
        0 => 'feature_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'feature_cvtermprop_idx1' => array (
        0 => 'feature_cvterm_id',
      ),
      'feature_cvtermprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_cvterm' => array (
        'table' => 'feature_cvterm',
        'columns' => array (
          'feature_cvterm_id' => 'feature_cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_dbxref()
 * Purpose: To describe the structure of 'feature_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'ru',
      ),
    ),
    'primary key' => array (
      0 => 'feature_dbxref_id',
    ),
    'unique keys' => array (
      'feature_dbxref_c1' => array (
        0 => 'feature_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'feature_dbxref_idx1' => array (
        0 => 'feature_id',
      ),
      'feature_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_expression()
 * Purpose: To describe the structure of 'feature_expression' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_expression' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_expression() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_expression_id',
    ),
    'unique keys' => array (
      'feature_expression_c1' => array (
        0 => 'expression_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'feature_expression_idx1' => array (
        0 => 'expression_id',
      ),
      'feature_expression_idx2' => array (
        0 => 'feature_id',
      ),
      'feature_expression_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'expression' => array (
        'table' => 'expression',
        'columns' => array (
          'expression_id' => 'expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_expressionprop()
 * Purpose: To describe the structure of 'feature_expressionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_expressionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_expressionprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_expressionprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_expression_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'feature_expressionprop_id',
    ),
    'unique keys' => array (
      'feature_expressionprop_c1' => array (
        0 => 'feature_expression_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'feature_expressionprop_idx1' => array (
        0 => 'feature_expression_id',
      ),
      'feature_expressionprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_expression' => array (
        'table' => 'feature_expression',
        'columns' => array (
          'feature_expression_id' => 'feature_expression_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_genotype()
 * Purpose: To describe the structure of 'feature_genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_genotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'chromosome_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cgroup' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_genotype_id',
    ),
    'unique keys' => array (
      'feature_genotype_c1' => array (
        0 => 'feature_id',
        1 => 'genotype_id',
        2 => 'cvterm_id',
        3 => 'chromosome_id',
        4 => 'rank',
        5 => 'cgroup',
      ),
    ),
    'indexes' => array (
      'feature_genotype_idx1' => array (
        0 => 'feature_id',
      ),
      'feature_genotype_idx2' => array (
        0 => 'genotype_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
          'chromosome_id' => 'feature_id',
        ),
      ),
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype_id' => 'genotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_phenotype()
 * Purpose: To describe the structure of 'feature_phenotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_phenotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_phenotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_phenotype_id',
    ),
    'unique keys' => array (
      'feature_phenotype_c1' => array (
        0 => 'feature_id',
        1 => 'phenotype_id',
      ),
    ),
    'indexes' => array (
      'feature_phenotype_idx1' => array (
        0 => 'feature_id',
      ),
      'feature_phenotype_idx2' => array (
        0 => 'phenotype_id',
      ),
    ),
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'phenotype' => array (
        'table' => 'phenotype',
        'columns' => array (
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_pub()
 * Purpose: To describe the structure of 'feature_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_pub_id',
    ),
    'unique keys' => array (
      'feature_pub_c1' => array (
        0 => 'feature_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'feature_pub_idx1' => array (
        0 => 'feature_id',
      ),
      'feature_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_pubprop()
 * Purpose: To describe the structure of 'feature_pubprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_pubprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_pubprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_pubprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'feature_pubprop_id',
    ),
    'unique keys' => array (
      'feature_pubprop_c1' => array (
        0 => 'feature_pub_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'feature_pubprop_idx1' => array (
        0 => 'feature_pub_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_pub' => array (
        'table' => 'feature_pub',
        'columns' => array (
          'feature_pub_id' => 'feature_pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationship()
 * Purpose: To describe the structure of 'feature_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'feature_relationship_id',
    ),
    'unique keys' => array (
      'feature_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'feature_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'feature_relationship_idx2' => array (
        0 => 'object_id',
      ),
      'feature_relationship_idx3' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'subject_id' => 'feature_id',
          'object_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationship_pub()
 * Purpose: To describe the structure of 'feature_relationship_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationship_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_relationship_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_relationship_pub_id',
    ),
    'unique keys' => array (
      'feature_relationship_pub_c1' => array (
        0 => 'feature_relationship_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'feature_relationship_pub_idx1' => array (
        0 => 'feature_relationship_id',
      ),
      'feature_relationship_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'feature_relationship' => array (
        'table' => 'feature_relationship',
        'columns' => array (
          'feature_relationship_id' => 'feature_relationship_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationshipprop()
 * Purpose: To describe the structure of 'feature_relationshipprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationshipprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationshipprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_relationshipprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'feature_relationshipprop_id',
    ),
    'unique keys' => array (
      'feature_relationshipprop_c1' => array (
        0 => 'feature_relationship_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'feature_relationshipprop_idx1' => array (
        0 => 'feature_relationship_id',
      ),
      'feature_relationshipprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature_relationship' => array (
        'table' => 'feature_relationship',
        'columns' => array (
          'feature_relationship_id' => 'feature_relationship_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_relationshipprop_pub()
 * Purpose: To describe the structure of 'feature_relationshipprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationshipprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_relationshipprop_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_relationshipprop_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_relationshipprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'feature_relationshipprop_pub_id',
    ),
    'unique keys' => array (
      'feature_relationshipprop_pub_c1' => array (
        0 => 'feature_relationshipprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'feature_relationshipprop_pub_idx1' => array (
        0 => 'feature_relationshipprop_id',
      ),
      'feature_relationshipprop_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'feature_relationshipprop' => array (
        'table' => 'feature_relationshipprop',
        'columns' => array (
          'feature_relationshipprop_id' => 'feature_relationshipprop_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_feature_synonym()
 * Purpose: To describe the structure of 'feature_synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_feature_synonym() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'feature_synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'is_internal' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
    ),
    'primary key' => array (
      0 => 'feature_synonym_id',
    ),
    'unique keys' => array (
      'feature_synonym_c1' => array (
        0 => 'synonym_id',
        1 => 'feature_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'feature_synonym_idx1' => array (
        0 => 'synonym_id',
      ),
      'feature_synonym_idx2' => array (
        0 => 'feature_id',
      ),
      'feature_synonym_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'synonym' => array (
        'table' => 'synonym',
        'columns' => array (
          'synonym_id' => 'synonym_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureloc()
 * Purpose: To describe the structure of 'featureloc' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureloc' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureloc() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featureloc_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'srcfeature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'fmin' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'is_fmin_partial' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'fmax' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'is_fmax_partial' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'strand' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => false,
      ),
      'phase' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'residue_info' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'locgroup' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'featureloc_id',
    ),
    'unique keys' => array (
      'featureloc_c1' => array (
        0 => 'feature_id',
        1 => 'locgroup',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'binloc_boxrange' => array (
        0 => 'fmin',
      ),
      'binloc_boxrange_src' => array (
        0 => 'srcfeature_id',
        1 => 'fmin',
      ),
      'featureloc_idx1' => array (
        0 => 'feature_id',
      ),
      'featureloc_idx2' => array (
        0 => 'srcfeature_id',
      ),
      'featureloc_idx3' => array (
        0 => 'srcfeature_id',
        1 => 'fmin',
        2 => 'fmax',
      ),
    ),
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
          'srcfeature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureloc_pub()
 * Purpose: To describe the structure of 'featureloc_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureloc_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureloc_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featureloc_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'featureloc_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'featureloc_pub_id',
    ),
    'unique keys' => array (
      'featureloc_pub_c1' => array (
        0 => 'featureloc_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'featureloc_pub_idx1' => array (
        0 => 'featureloc_id',
      ),
      'featureloc_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'featureloc' => array (
        'table' => 'featureloc',
        'columns' => array (
          'featureloc_id' => 'featureloc_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featuremap()
 * Purpose: To describe the structure of 'featuremap' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featuremap' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featuremap() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featuremap_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'unittype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'featuremap_id',
    ),
    'unique keys' => array (
      'featuremap_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'unittype_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featuremap_pub()
 * Purpose: To describe the structure of 'featuremap_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featuremap_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featuremap_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featuremap_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'featuremap_pub_id',
    ),
    'indexes' => array (
      'featuremap_pub_idx1' => array (
        0 => 'featuremap_id',
      ),
      'featuremap_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featurepos()
 * Purpose: To describe the structure of 'featurepos' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featurepos' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featurepos() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featurepos_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'map_feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'mappos' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'featurepos_id',
    ),
    'indexes' => array (
      'featurepos_idx1' => array (
        0 => 'featuremap_id',
      ),
      'featurepos_idx2' => array (
        0 => 'feature_id',
      ),
      'featurepos_idx3' => array (
        0 => 'map_feature_id',
      ),
    ),
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
          'map_feature_id' => 'feature_id',
        ),
      ),
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureprop()
 * Purpose: To describe the structure of 'featureprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featureprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'featureprop_id',
    ),
    'unique keys' => array (
      'featureprop_c1' => array (
        0 => 'feature_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'featureprop_idx1' => array (
        0 => 'feature_id',
      ),
      'featureprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featureprop_pub()
 * Purpose: To describe the structure of 'featureprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featureprop_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featureprop_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'featureprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'featureprop_pub_id',
    ),
    'unique keys' => array (
      'featureprop_pub_c1' => array (
        0 => 'featureprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'featureprop_pub_idx1' => array (
        0 => 'featureprop_id',
      ),
      'featureprop_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'featureprop' => array (
        'table' => 'featureprop',
        'columns' => array (
          'featureprop_id' => 'featureprop_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_featurerange()
 * Purpose: To describe the structure of 'featurerange' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featurerange' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_featurerange() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'featurerange_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'leftstartf_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'leftendf_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'rightstartf_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'rightendf_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rangestr' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'featurerange_id',
    ),
    'indexes' => array (
      'featurerange_idx1' => array (
        0 => 'featuremap_id',
      ),
      'featurerange_idx2' => array (
        0 => 'feature_id',
      ),
      'featurerange_idx3' => array (
        0 => 'leftstartf_id',
      ),
      'featurerange_idx4' => array (
        0 => 'leftendf_id',
      ),
      'featurerange_idx5' => array (
        0 => 'rightstartf_id',
      ),
      'featurerange_idx6' => array (
        0 => 'rightendf_id',
      ),
    ),
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
          'leftstartf_id' => 'feature_id',
          'leftendf_id' => 'feature_id',
          'rightstartf_id' => 'feature_id',
          'rightendf_id' => 'feature_id',
        ),
      ),
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_genotype()
 * Purpose: To describe the structure of 'genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_genotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'genotype_id',
    ),
    'unique keys' => array (
      'genotype_c1' => array (
        0 => 'uniquename',
      ),
    ),
    'indexes' => array (
      'genotype_idx1' => array (
        0 => 'uniquename',
      ),
      'genotype_idx2' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_genotypeprop()
 * Purpose: To describe the structure of 'genotypeprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'genotypeprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_genotypeprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'genotypeprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'genotypeprop_id',
    ),
    'unique keys' => array (
      'genotypeprop_c1' => array (
        0 => 'genotype_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'genotypeprop_idx1' => array (
        0 => 'genotype_id',
      ),
      'genotypeprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype_id' => 'genotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library()
 * Purpose: To describe the structure of 'library' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_obsolete' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
      'timeaccessioned' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
      'timelastmodified' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => true,
        'default' => 'ow(',
      ),
    ),
    'primary key' => array (
      0 => 'library_id',
    ),
    'unique keys' => array (
      'library_c1' => array (
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'library_idx1' => array (
        0 => 'organism_id',
      ),
      'library_idx2' => array (
        0 => 'type_id',
      ),
      'library_idx3' => array (
        0 => 'uniquename',
      ),
      'library_name_ind1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_cvterm()
 * Purpose: To describe the structure of 'library_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'library_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'library_cvterm_id',
    ),
    'unique keys' => array (
      'library_cvterm_c1' => array (
        0 => 'library_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'library_cvterm_idx1' => array (
        0 => 'library_id',
      ),
      'library_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
      'library_cvterm_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_dbxref()
 * Purpose: To describe the structure of 'library_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'library_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'ru',
      ),
    ),
    'primary key' => array (
      0 => 'library_dbxref_id',
    ),
    'unique keys' => array (
      'library_dbxref_c1' => array (
        0 => 'library_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'library_dbxref_idx1' => array (
        0 => 'library_id',
      ),
      'library_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_feature()
 * Purpose: To describe the structure of 'library_feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_feature() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'library_feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'library_feature_id',
    ),
    'unique keys' => array (
      'library_feature_c1' => array (
        0 => 'library_id',
        1 => 'feature_id',
      ),
    ),
    'indexes' => array (
      'library_feature_idx1' => array (
        0 => 'library_id',
      ),
      'library_feature_idx2' => array (
        0 => 'feature_id',
      ),
    ),
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_pub()
 * Purpose: To describe the structure of 'library_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'library_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'library_pub_id',
    ),
    'unique keys' => array (
      'library_pub_c1' => array (
        0 => 'library_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'library_pub_idx1' => array (
        0 => 'library_id',
      ),
      'library_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_library_synonym()
 * Purpose: To describe the structure of 'library_synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_library_synonym() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'library_synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'ru',
      ),
      'is_internal' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
    ),
    'primary key' => array (
      0 => 'library_synonym_id',
    ),
    'unique keys' => array (
      'library_synonym_c1' => array (
        0 => 'synonym_id',
        1 => 'library_id',
        2 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'library_synonym_idx1' => array (
        0 => 'synonym_id',
      ),
      'library_synonym_idx2' => array (
        0 => 'library_id',
      ),
      'library_synonym_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'synonym' => array (
        'table' => 'synonym',
        'columns' => array (
          'synonym_id' => 'synonym_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_libraryprop()
 * Purpose: To describe the structure of 'libraryprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'libraryprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_libraryprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'libraryprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'library_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'libraryprop_id',
    ),
    'unique keys' => array (
      'libraryprop_c1' => array (
        0 => 'library_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'libraryprop_idx1' => array (
        0 => 'library_id',
      ),
      'libraryprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'library' => array (
        'table' => 'library',
        'columns' => array (
          'library_id' => 'library_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_libraryprop_pub()
 * Purpose: To describe the structure of 'libraryprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'libraryprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_libraryprop_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'libraryprop_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'libraryprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'libraryprop_pub_id',
    ),
    'unique keys' => array (
      'libraryprop_pub_c1' => array (
        0 => 'libraryprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'libraryprop_pub_idx1' => array (
        0 => 'libraryprop_id',
      ),
      'libraryprop_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'libraryprop' => array (
        'table' => 'libraryprop',
        'columns' => array (
          'libraryprop_id' => 'libraryprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_magedocumentation()
 * Purpose: To describe the structure of 'magedocumentation' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'magedocumentation' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_magedocumentation() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'magedocumentation_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'mageml_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'tableinfo_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'row_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'mageidentifier' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'magedocumentation_id',
    ),
    'indexes' => array (
      'magedocumentation_idx1' => array (
        0 => 'mageml_id',
      ),
      'magedocumentation_idx2' => array (
        0 => 'tableinfo_id',
      ),
      'magedocumentation_idx3' => array (
        0 => 'row_id',
      ),
    ),
    'foreign keys' => array (
      'tableinfo' => array (
        'table' => 'tableinfo',
        'columns' => array (
          'tableinfo_id' => 'tableinfo_id',
        ),
      ),
      'mageml' => array (
        'table' => 'mageml',
        'columns' => array (
          'mageml_id' => 'mageml_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_mageml()
 * Purpose: To describe the structure of 'mageml' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'mageml' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_mageml() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'mageml_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'mage_package' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'mage_ml' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'mageml_id',
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_materialized_view()
 * Purpose: To describe the structure of 'materialized_view' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'materialized_view' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_materialized_view() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment()
 * Purpose: To describe the structure of 'nd_experiment' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_geolocation_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_id',
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_geolocation' => array (
        'table' => 'nd_geolocation',
        'columns' => array (
          'nd_geolocation_id' => 'nd_geolocation_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_contact()
 * Purpose: To describe the structure of 'nd_experiment_contact' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_contact' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_contact() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_contact_id',
    ),
    'foreign keys' => array (
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_dbxref()
 * Purpose: To describe the structure of 'nd_experiment_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_dbxref_id',
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_genotype()
 * Purpose: To describe the structure of 'nd_experiment_genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_genotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_genotype_id',
    ),
    'unique keys' => array (
      'nd_experiment_genotype_c1' => array (
        0 => 'nd_experiment_id',
        1 => 'genotype_id',
      ),
    ),
    'foreign keys' => array (
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype_id' => 'genotype_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_phenotype()
 * Purpose: To describe the structure of 'nd_experiment_phenotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_phenotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_phenotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_phenotype_id',
    ),
    'unique keys' => array (
      'nd_experiment_phenotype_c1' => array (
        0 => 'nd_experiment_id',
        1 => 'phenotype_id',
      ),
    ),
    'foreign keys' => array (
      'phenotype' => array (
        'table' => 'phenotype',
        'columns' => array (
          'phenotype_id' => 'phenotype_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_project()
 * Purpose: To describe the structure of 'nd_experiment_project' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_project' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_project() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_project_id',
    ),
    'foreign keys' => array (
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'project_id' => 'project_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_protocol()
 * Purpose: To describe the structure of 'nd_experiment_protocol' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_protocol' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_protocol() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'nd_protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_protocol_id',
    ),
    'foreign keys' => array (
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'nd_protocol' => array (
        'table' => 'nd_protocol',
        'columns' => array (
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_pub()
 * Purpose: To describe the structure of 'nd_experiment_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_pub_id',
    ),
    'unique keys' => array (
      'nd_experiment_pub_c1' => array (
        0 => 'nd_experiment_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'nd_experiment_pub_idx1' => array (
        0 => 'nd_experiment_id',
      ),
      'nd_experiment_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_stock()
 * Purpose: To describe the structure of 'nd_experiment_stock' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_stock' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_stock() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_stock_id',
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_stock_dbxref()
 * Purpose: To describe the structure of 'nd_experiment_stock_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_stock_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_stock_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_stock_dbxref_id',
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'nd_experiment_stock' => array (
        'table' => 'nd_experiment_stock',
        'columns' => array (
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experiment_stockprop()
 * Purpose: To describe the structure of 'nd_experiment_stockprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_stockprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experiment_stockprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experiment_stockprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experiment_stockprop_id',
    ),
    'unique keys' => array (
      'nd_experiment_stockprop_c1' => array (
        0 => 'nd_experiment_stock_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_experiment_stock' => array (
        'table' => 'nd_experiment_stock',
        'columns' => array (
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_experimentprop()
 * Purpose: To describe the structure of 'nd_experimentprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experimentprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_experimentprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_experimentprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_experiment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'nd_experimentprop_id',
    ),
    'unique keys' => array (
      'nd_experimentprop_c1' => array (
        0 => 'nd_experiment_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_experiment' => array (
        'table' => 'nd_experiment',
        'columns' => array (
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_geolocation()
 * Purpose: To describe the structure of 'nd_geolocation' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_geolocation' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_geolocation() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_geolocation_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'latitude' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => false,
      ),
      'longitude' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => false,
      ),
      'geodetic_datum' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '32',
        'not null' => false,
      ),
      'altitude' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'nd_geolocation_id',
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_geolocationprop()
 * Purpose: To describe the structure of 'nd_geolocationprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_geolocationprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_geolocationprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_geolocationprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_geolocation_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'nd_geolocationprop_id',
    ),
    'unique keys' => array (
      'nd_geolocationprop_c1' => array (
        0 => 'nd_geolocation_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_geolocation' => array (
        'table' => 'nd_geolocation',
        'columns' => array (
          'nd_geolocation_id' => 'nd_geolocation_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_protocol()
 * Purpose: To describe the structure of 'nd_protocol' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_protocol' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_protocol() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_protocol_id',
    ),
    'unique keys' => array (
      'name' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_protocol_reagent()
 * Purpose: To describe the structure of 'nd_protocol_reagent' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_protocol_reagent' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_protocol_reagent() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_protocol_reagent_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'reagent_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_protocol_reagent_id',
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_protocol' => array (
        'table' => 'nd_protocol',
        'columns' => array (
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
      'nd_reagent' => array (
        'table' => 'nd_reagent',
        'columns' => array (
          'reagent_id' => 'nd_reagent_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_protocolprop()
 * Purpose: To describe the structure of 'nd_protocolprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_protocolprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_protocolprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_protocolprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'nd_protocolprop_id',
    ),
    'unique keys' => array (
      'nd_protocolprop_c1' => array (
        0 => 'nd_protocol_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_protocol' => array (
        'table' => 'nd_protocol',
        'columns' => array (
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_reagent()
 * Purpose: To describe the structure of 'nd_reagent' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_reagent' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_reagent() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_reagent_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '80',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'nd_reagent_id',
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_reagent_relationship()
 * Purpose: To describe the structure of 'nd_reagent_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_reagent_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_reagent_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_reagent_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_reagent_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_reagent_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'nd_reagent_relationship_id',
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_reagent' => array (
        'table' => 'nd_reagent',
        'columns' => array (
          'subject_reagent_id' => 'nd_reagent_id',
          'object_reagent_id' => 'nd_reagent_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_nd_reagentprop()
 * Purpose: To describe the structure of 'nd_reagentprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_reagentprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_nd_reagentprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'nd_reagentprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'nd_reagent_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'nd_reagentprop_id',
    ),
    'unique keys' => array (
      'nd_reagentprop_c1' => array (
        0 => 'nd_reagent_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'nd_reagent' => array (
        'table' => 'nd_reagent',
        'columns' => array (
          'nd_reagent_id' => 'nd_reagent_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organism()
 * Purpose: To describe the structure of 'organism' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organism' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organism() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'abbreviation' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'genus' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'species' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'common_name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'comment' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'organism_id',
    ),
    'unique keys' => array (
      'organism_c1' => array (
        0 => 'genus',
        1 => 'species',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organism_dbxref()
 * Purpose: To describe the structure of 'organism_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organism_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organism_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'organism_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'organism_dbxref_id',
    ),
    'unique keys' => array (
      'organism_dbxref_c1' => array (
        0 => 'organism_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'organism_dbxref_idx1' => array (
        0 => 'organism_id',
      ),
      'organism_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organism_feature_count()
 * Purpose: To describe the structure of 'organism_feature_count' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organism_feature_count' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organism_feature_count() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_organismprop()
 * Purpose: To describe the structure of 'organismprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organismprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_organismprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'organismprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'organismprop_id',
    ),
    'unique keys' => array (
      'organismprop_c1' => array (
        0 => 'organism_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'organismprop_idx1' => array (
        0 => 'organism_id',
      ),
      'organismprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phendesc()
 * Purpose: To describe the structure of 'phendesc' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phendesc' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phendesc() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phendesc_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'environment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phendesc_id',
    ),
    'unique keys' => array (
      'phendesc_c1' => array (
        0 => 'genotype_id',
        1 => 'environment_id',
        2 => 'type_id',
        3 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'phendesc_idx1' => array (
        0 => 'genotype_id',
      ),
      'phendesc_idx2' => array (
        0 => 'environment_id',
      ),
      'phendesc_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype_id' => 'genotype_id',
        ),
      ),
      'environment' => array (
        'table' => 'environment',
        'columns' => array (
          'environment_id' => 'environment_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype()
 * Purpose: To describe the structure of 'phenotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'observable_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'attr_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'cvalue_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'phenotype_id',
    ),
    'unique keys' => array (
      'phenotype_c1' => array (
        0 => 'uniquename',
      ),
    ),
    'indexes' => array (
      'phenotype_idx1' => array (
        0 => 'cvalue_id',
      ),
      'phenotype_idx2' => array (
        0 => 'observable_id',
      ),
      'phenotype_idx3' => array (
        0 => 'attr_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
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
 * Implements hook_chado_schema_v1_2_phenotype_comparison()
 * Purpose: To describe the structure of 'phenotype_comparison' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype_comparison' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype_comparison() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phenotype_comparison_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'genotype1_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'environment1_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'genotype2_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'environment2_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'phenotype1_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'phenotype2_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phenotype_comparison_id',
    ),
    'unique keys' => array (
      'phenotype_comparison_c1' => array (
        0 => 'genotype1_id',
        1 => 'environment1_id',
        2 => 'genotype2_id',
        3 => 'environment2_id',
        4 => 'phenotype1_id',
        5 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'phenotype_comparison_idx1' => array (
        0 => 'genotype1_id',
      ),
      'phenotype_comparison_idx2' => array (
        0 => 'genotype2_id',
      ),
      'phenotype_comparison_idx4' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype1_id' => 'genotype_id',
          'genotype2_id' => 'genotype_id',
        ),
      ),
      'environment' => array (
        'table' => 'environment',
        'columns' => array (
          'environment1_id' => 'environment_id',
          'environment2_id' => 'environment_id',
        ),
      ),
      'phenotype' => array (
        'table' => 'phenotype',
        'columns' => array (
          'phenotype1_id' => 'phenotype_id',
          'phenotype2_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype_comparison_cvterm()
 * Purpose: To describe the structure of 'phenotype_comparison_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype_comparison_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype_comparison_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phenotype_comparison_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phenotype_comparison_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'phenotype_comparison_cvterm_id',
    ),
    'unique keys' => array (
      'phenotype_comparison_cvterm_c1' => array (
        0 => 'phenotype_comparison_id',
        1 => 'cvterm_id',
      ),
    ),
    'indexes' => array (
      'phenotype_comparison_cvterm_idx1' => array (
        0 => 'phenotype_comparison_id',
      ),
      'phenotype_comparison_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'phenotype_comparison' => array (
        'table' => 'phenotype_comparison',
        'columns' => array (
          'phenotype_comparison_id' => 'phenotype_comparison_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenotype_cvterm()
 * Purpose: To describe the structure of 'phenotype_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenotype_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phenotype_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'phenotype_cvterm_id',
    ),
    'unique keys' => array (
      'phenotype_cvterm_c1' => array (
        0 => 'phenotype_id',
        1 => 'cvterm_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'phenotype_cvterm_idx1' => array (
        0 => 'phenotype_id',
      ),
      'phenotype_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'phenotype' => array (
        'table' => 'phenotype',
        'columns' => array (
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phenstatement()
 * Purpose: To describe the structure of 'phenstatement' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenstatement' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phenstatement() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phenstatement_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'environment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'phenotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phenstatement_id',
    ),
    'unique keys' => array (
      'phenstatement_c1' => array (
        0 => 'genotype_id',
        1 => 'phenotype_id',
        2 => 'environment_id',
        3 => 'type_id',
        4 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'phenstatement_idx1' => array (
        0 => 'genotype_id',
      ),
      'phenstatement_idx2' => array (
        0 => 'phenotype_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype_id' => 'genotype_id',
        ),
      ),
      'environment' => array (
        'table' => 'environment',
        'columns' => array (
          'environment_id' => 'environment_id',
        ),
      ),
      'phenotype' => array (
        'table' => 'phenotype',
        'columns' => array (
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode()
 * Purpose: To describe the structure of 'phylonode' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylonode_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phylotree_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'parent_phylonode_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'left_idx' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'right_idx' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'label' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'distance' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'phylonode_id',
    ),
    'unique keys' => array (
      'phylotree_id' => array (
        0 => 'phylotree_id',
        1 => 'left_idx',
      ),
      'phylonode_phylotree_id_key1' => array (
        0 => 'phylotree_id',
        1 => 'right_idx',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
          'parent_phylonode_id' => 'phylonode_id',
        ),
      ),
      'phylotree' => array (
        'table' => 'phylotree',
        'columns' => array (
          'phylotree_id' => 'phylotree_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_dbxref()
 * Purpose: To describe the structure of 'phylonode_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylonode_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phylonode_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phylonode_dbxref_id',
    ),
    'unique keys' => array (
      'phylonode_id' => array (
        0 => 'phylonode_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'phylonode_dbxref_idx1' => array (
        0 => 'phylonode_id',
      ),
      'phylonode_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_organism()
 * Purpose: To describe the structure of 'phylonode_organism' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_organism' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_organism() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylonode_organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phylonode_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phylonode_organism_id',
    ),
    'unique keys' => array (
      'phylonode_id' => array (
        0 => 'phylonode_id',
      ),
    ),
    'indexes' => array (
      'phylonode_organism_idx1' => array (
        0 => 'phylonode_id',
      ),
      'phylonode_organism_idx2' => array (
        0 => 'organism_id',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_pub()
 * Purpose: To describe the structure of 'phylonode_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylonode_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phylonode_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phylonode_pub_id',
    ),
    'unique keys' => array (
      'phylonode_id' => array (
        0 => 'phylonode_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'phylonode_pub_idx1' => array (
        0 => 'phylonode_id',
      ),
      'phylonode_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonode_relationship()
 * Purpose: To describe the structure of 'phylonode_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonode_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylonode_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'phylotree_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phylonode_relationship_id',
    ),
    'unique keys' => array (
      'subject_id' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'phylonode_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'phylonode_relationship_idx2' => array (
        0 => 'object_id',
      ),
      'phylonode_relationship_idx3' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
          'subject_id' => 'phylonode_id',
          'object_id' => 'phylonode_id',
        ),
      ),
      'phylotree' => array (
        'table' => 'phylotree',
        'columns' => array (
          'phylotree_id' => 'phylotree_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylonodeprop()
 * Purpose: To describe the structure of 'phylonodeprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonodeprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylonodeprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylonodeprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phylonode_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
        'default' => '',
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'phylonodeprop_id',
    ),
    'unique keys' => array (
      'phylonode_id' => array (
        0 => 'phylonode_id',
        1 => 'type_id',
        2 => 'value',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'phylonodeprop_idx1' => array (
        0 => 'phylonode_id',
      ),
      'phylonodeprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
          'phylonode_id' => 'phylonode_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylotree()
 * Purpose: To describe the structure of 'phylotree' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylotree' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylotree() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylotree_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'analysis_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'comment' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'phylotree_id',
    ),
    'indexes' => array (
      'phylotree_idx1' => array (
        0 => 'phylotree_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'analysis' => array (
        'table' => 'analysis',
        'columns' => array (
          'analysis_id' => 'analysis_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_phylotree_pub()
 * Purpose: To describe the structure of 'phylotree_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylotree_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_phylotree_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'phylotree_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'phylotree_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'phylotree_pub_id',
    ),
    'unique keys' => array (
      'phylotree_id' => array (
        0 => 'phylotree_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'phylotree_pub_idx1' => array (
        0 => 'phylotree_id',
      ),
      'phylotree_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'phylotree' => array (
        'table' => 'phylotree',
        'columns' => array (
          'phylotree_id' => 'phylotree_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project()
 * Purpose: To describe the structure of 'project' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'project_id',
    ),
    'unique keys' => array (
      'project_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project_contact()
 * Purpose: To describe the structure of 'project_contact' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project_contact' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project_contact() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'project_contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'project_contact_id',
    ),
    'unique keys' => array (
      'project_contact_c1' => array (
        0 => 'project_id',
        1 => 'contact_id',
      ),
    ),
    'indexes' => array (
      'project_contact_idx1' => array (
        0 => 'project_id',
      ),
      'project_contact_idx2' => array (
        0 => 'contact_id',
      ),
    ),
    'foreign keys' => array (
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'project_id' => 'project_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project_pub()
 * Purpose: To describe the structure of 'project_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'project_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'project_pub_id',
    ),
    'unique keys' => array (
      'project_pub_c1' => array (
        0 => 'project_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'project_pub_idx1' => array (
        0 => 'project_id',
      ),
      'project_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'project_id' => 'project_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_project_relationship()
 * Purpose: To describe the structure of 'project_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_project_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'project_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'project_relationship_id',
    ),
    'unique keys' => array (
      'project_relationship_c1' => array (
        0 => 'subject_project_id',
        1 => 'object_project_id',
        2 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'subject_project_id' => 'project_id',
          'object_project_id' => 'project_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_projectprop()
 * Purpose: To describe the structure of 'projectprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'projectprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_projectprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'projectprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'project_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'projectprop_id',
    ),
    'unique keys' => array (
      'projectprop_c1' => array (
        0 => 'project_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'project_id' => 'project_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_protocol()
 * Purpose: To describe the structure of 'protocol' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'protocol' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_protocol() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'uri' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'protocoldescription' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'hardwaredescription' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'softwaredescription' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'protocol_id',
    ),
    'unique keys' => array (
      'protocol_c1' => array (
        0 => 'name',
      ),
    ),
    'indexes' => array (
      'protocol_idx1' => array (
        0 => 'type_id',
      ),
      'protocol_idx2' => array (
        0 => 'pub_id',
      ),
      'protocol_idx3' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_protocolparam()
 * Purpose: To describe the structure of 'protocolparam' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'protocolparam' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_protocolparam() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'protocolparam_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'datatype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'unittype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'protocolparam_id',
    ),
    'indexes' => array (
      'protocolparam_idx1' => array (
        0 => 'protocol_id',
      ),
      'protocolparam_idx2' => array (
        0 => 'datatype_id',
      ),
      'protocolparam_idx3' => array (
        0 => 'unittype_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'datatype_id' => 'cvterm_id',
          'unittype_id' => 'cvterm_id',
        ),
      ),
      'protocol' => array (
        'table' => 'protocol',
        'columns' => array (
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pub()
 * Purpose: To describe the structure of 'pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'title' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'volumetitle' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'volume' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'series_name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'issue' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'pyear' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'pages' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'miniref' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_obsolete' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => false,
        'default' => 'als',
      ),
      'publisher' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'pubplace' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'pub_id',
    ),
    'unique keys' => array (
      'pub_c1' => array (
        0 => 'uniquename',
      ),
    ),
    'indexes' => array (
      'pub_idx1' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pub_dbxref()
 * Purpose: To describe the structure of 'pub_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pub_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pub_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'pub_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'ru',
      ),
    ),
    'primary key' => array (
      0 => 'pub_dbxref_id',
    ),
    'unique keys' => array (
      'pub_dbxref_c1' => array (
        0 => 'pub_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'pub_dbxref_idx1' => array (
        0 => 'pub_id',
      ),
      'pub_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pub_relationship()
 * Purpose: To describe the structure of 'pub_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pub_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pub_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'pub_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'pub_relationship_id',
    ),
    'unique keys' => array (
      'pub_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'pub_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'pub_relationship_idx2' => array (
        0 => 'object_id',
      ),
      'pub_relationship_idx3' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'subject_id' => 'pub_id',
          'object_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pubauthor()
 * Purpose: To describe the structure of 'pubauthor' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pubauthor' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pubauthor() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'pubauthor_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'editor' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => false,
        'default' => 'als',
      ),
      'surname' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => true,
      ),
      'givennames' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => false,
      ),
      'suffix' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'pubauthor_id',
    ),
    'unique keys' => array (
      'pubauthor_c1' => array (
        0 => 'pub_id',
        1 => 'rank',
      ),
    ),
    'indexes' => array (
      'pubauthor_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_pubprop()
 * Purpose: To describe the structure of 'pubprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pubprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_pubprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'pubprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'pubprop_id',
    ),
    'unique keys' => array (
      'pubprop_c1' => array (
        0 => 'pub_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'pubprop_idx1' => array (
        0 => 'pub_id',
      ),
      'pubprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_quantification()
 * Purpose: To describe the structure of 'quantification' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'quantification' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_quantification() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'quantification_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'acquisition_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'operator_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'analysis_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'quantificationdate' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => false,
        'default' => 'ow(',
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'uri' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'quantification_id',
    ),
    'unique keys' => array (
      'quantification_c1' => array (
        0 => 'name',
        1 => 'analysis_id',
      ),
    ),
    'indexes' => array (
      'quantification_idx1' => array (
        0 => 'acquisition_id',
      ),
      'quantification_idx2' => array (
        0 => 'operator_id',
      ),
      'quantification_idx3' => array (
        0 => 'protocol_id',
      ),
      'quantification_idx4' => array (
        0 => 'analysis_id',
      ),
    ),
    'foreign keys' => array (
      'analysis' => array (
        'table' => 'analysis',
        'columns' => array (
          'analysis_id' => 'analysis_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'operator_id' => 'contact_id',
        ),
      ),
      'protocol' => array (
        'table' => 'protocol',
        'columns' => array (
          'protocol_id' => 'protocol_id',
        ),
      ),
      'acquisition' => array (
        'table' => 'acquisition',
        'columns' => array (
          'acquisition_id' => 'acquisition_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_quantification_relationship()
 * Purpose: To describe the structure of 'quantification_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'quantification_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_quantification_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'quantification_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'quantification_relationship_id',
    ),
    'unique keys' => array (
      'quantification_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'quantification_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'quantification_relationship_idx2' => array (
        0 => 'type_id',
      ),
      'quantification_relationship_idx3' => array (
        0 => 'object_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'quantification' => array (
        'table' => 'quantification',
        'columns' => array (
          'subject_id' => 'quantification_id',
          'object_id' => 'quantification_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_quantificationprop()
 * Purpose: To describe the structure of 'quantificationprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'quantificationprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_quantificationprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'quantificationprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'quantification_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'quantificationprop_id',
    ),
    'unique keys' => array (
      'quantificationprop_c1' => array (
        0 => 'quantification_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'quantificationprop_idx1' => array (
        0 => 'quantification_id',
      ),
      'quantificationprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'quantification' => array (
        'table' => 'quantification',
        'columns' => array (
          'quantification_id' => 'quantification_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock()
 * Purpose: To describe the structure of 'stock' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'organism_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_obsolete' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
    ),
    'primary key' => array (
      0 => 'stock_id',
    ),
    'unique keys' => array (
      'stock_c1' => array (
        0 => 'organism_id',
        1 => 'uniquename',
        2 => 'type_id',
      ),
    ),
    'indexes' => array (
      'stock_idx1' => array (
        0 => 'dbxref_id',
      ),
      'stock_idx2' => array (
        0 => 'organism_id',
      ),
      'stock_idx3' => array (
        0 => 'type_id',
      ),
      'stock_idx4' => array (
        0 => 'uniquename',
      ),
      'stock_name_ind1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'organism' => array (
        'table' => 'organism',
        'columns' => array (
          'organism_id' => 'organism_id',
        ),
      ),
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_cvterm()
 * Purpose: To describe the structure of 'stock_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_not' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'als',
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'stock_cvterm_id',
    ),
    'unique keys' => array (
      'stock_cvterm_c1' => array (
        0 => 'stock_id',
        1 => 'cvterm_id',
        2 => 'pub_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'stock_cvterm_idx1' => array (
        0 => 'stock_id',
      ),
      'stock_cvterm_idx2' => array (
        0 => 'cvterm_id',
      ),
      'stock_cvterm_idx3' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_cvtermprop()
 * Purpose: To describe the structure of 'stock_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_cvtermprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_cvtermprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'stock_cvtermprop_id',
    ),
    'unique keys' => array (
      'stock_cvtermprop_c1' => array (
        0 => 'stock_cvterm_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'stock_cvtermprop_idx1' => array (
        0 => 'stock_cvterm_id',
      ),
      'stock_cvtermprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock_cvterm' => array (
        'table' => 'stock_cvterm',
        'columns' => array (
          'stock_cvterm_id' => 'stock_cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_dbxref()
 * Purpose: To describe the structure of 'stock_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_dbxref() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => true,
        'default' => 'ru',
      ),
    ),
    'primary key' => array (
      0 => 'stock_dbxref_id',
    ),
    'unique keys' => array (
      'stock_dbxref_c1' => array (
        0 => 'stock_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'stock_dbxref_idx1' => array (
        0 => 'stock_id',
      ),
      'stock_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_dbxrefprop()
 * Purpose: To describe the structure of 'stock_dbxrefprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_dbxrefprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_dbxrefprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'stock_dbxrefprop_id',
    ),
    'unique keys' => array (
      'stock_dbxrefprop_c1' => array (
        0 => 'stock_dbxref_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'stock_dbxrefprop_idx1' => array (
        0 => 'stock_dbxref_id',
      ),
      'stock_dbxrefprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock_dbxref' => array (
        'table' => 'stock_dbxref',
        'columns' => array (
          'stock_dbxref_id' => 'stock_dbxref_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_genotype()
 * Purpose: To describe the structure of 'stock_genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_genotype' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_genotype() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'genotype_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'stock_genotype_id',
    ),
    'unique keys' => array (
      'stock_genotype_c1' => array (
        0 => 'stock_id',
        1 => 'genotype_id',
      ),
    ),
    'indexes' => array (
      'stock_genotype_idx1' => array (
        0 => 'stock_id',
      ),
      'stock_genotype_idx2' => array (
        0 => 'genotype_id',
      ),
    ),
    'foreign keys' => array (
      'genotype' => array (
        'table' => 'genotype',
        'columns' => array (
          'genotype_id' => 'genotype_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_pub()
 * Purpose: To describe the structure of 'stock_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'stock_pub_id',
    ),
    'unique keys' => array (
      'stock_pub_c1' => array (
        0 => 'stock_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'stock_pub_idx1' => array (
        0 => 'stock_id',
      ),
      'stock_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_relationship()
 * Purpose: To describe the structure of 'stock_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_relationship' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_relationship() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'subject_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'object_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'stock_relationship_id',
    ),
    'unique keys' => array (
      'stock_relationship_c1' => array (
        0 => 'subject_id',
        1 => 'object_id',
        2 => 'type_id',
        3 => 'rank',
      ),
    ),
    'indexes' => array (
      'stock_relationship_idx1' => array (
        0 => 'subject_id',
      ),
      'stock_relationship_idx2' => array (
        0 => 'object_id',
      ),
      'stock_relationship_idx3' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'subject_id' => 'stock_id',
          'object_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_relationship_cvterm()
 * Purpose: To describe the structure of 'stock_relationship_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_relationship_cvterm' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_relationship_cvterm() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_relationship_cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'cvterm_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'stock_relationship_cvterm_id',
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'cvterm_id' => 'cvterm_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'stock_relationship' => array (
        'table' => 'stock_relationship',
        'columns' => array (
          'stock_relationship_id' => 'stock_relationship_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stock_relationship_pub()
 * Purpose: To describe the structure of 'stock_relationship_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stock_relationship_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stock_relationship_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_relationship_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'stock_relationship_pub_id',
    ),
    'unique keys' => array (
      'stock_relationship_pub_c1' => array (
        0 => 'stock_relationship_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'stock_relationship_pub_idx1' => array (
        0 => 'stock_relationship_id',
      ),
      'stock_relationship_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'stock_relationship' => array (
        'table' => 'stock_relationship',
        'columns' => array (
          'stock_relationship_id' => 'stock_relationship_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockcollection()
 * Purpose: To describe the structure of 'stockcollection' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockcollection' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockcollection() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stockcollection_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => false,
      ),
      'uniquename' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'stockcollection_id',
    ),
    'unique keys' => array (
      'stockcollection_c1' => array (
        0 => 'uniquename',
        1 => 'type_id',
      ),
    ),
    'indexes' => array (
      'stockcollection_idx1' => array (
        0 => 'contact_id',
      ),
      'stockcollection_idx2' => array (
        0 => 'type_id',
      ),
      'stockcollection_idx3' => array (
        0 => 'uniquename',
      ),
      'stockcollection_name_ind1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockcollection_stock()
 * Purpose: To describe the structure of 'stockcollection_stock' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockcollection_stock' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockcollection_stock() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stockcollection_stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stockcollection_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'stockcollection_stock_id',
    ),
    'unique keys' => array (
      'stockcollection_stock_c1' => array (
        0 => 'stockcollection_id',
        1 => 'stock_id',
      ),
    ),
    'indexes' => array (
      'stockcollection_stock_idx1' => array (
        0 => 'stockcollection_id',
      ),
      'stockcollection_stock_idx2' => array (
        0 => 'stock_id',
      ),
    ),
    'foreign keys' => array (
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
      'stockcollection' => array (
        'table' => 'stockcollection',
        'columns' => array (
          'stockcollection_id' => 'stockcollection_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockcollectionprop()
 * Purpose: To describe the structure of 'stockcollectionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockcollectionprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockcollectionprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stockcollectionprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stockcollection_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'stockcollectionprop_id',
    ),
    'unique keys' => array (
      'stockcollectionprop_c1' => array (
        0 => 'stockcollection_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'stockcollectionprop_idx1' => array (
        0 => 'stockcollection_id',
      ),
      'stockcollectionprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'stockcollection' => array (
        'table' => 'stockcollection',
        'columns' => array (
          'stockcollection_id' => 'stockcollection_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockprop()
 * Purpose: To describe the structure of 'stockprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stockprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stock_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'stockprop_id',
    ),
    'unique keys' => array (
      'stockprop_c1' => array (
        0 => 'stock_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'stockprop_idx1' => array (
        0 => 'stock_id',
      ),
      'stockprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'stock_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_stockprop_pub()
 * Purpose: To describe the structure of 'stockprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockprop_pub' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_stockprop_pub() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'stockprop_pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'stockprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'stockprop_pub_id',
    ),
    'unique keys' => array (
      'stockprop_pub_c1' => array (
        0 => 'stockprop_id',
        1 => 'pub_id',
      ),
    ),
    'indexes' => array (
      'stockprop_pub_idx1' => array (
        0 => 'stockprop_id',
      ),
      'stockprop_pub_idx2' => array (
        0 => 'pub_id',
      ),
    ),
    'foreign keys' => array (
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'stockprop' => array (
        'table' => 'stockprop',
        'columns' => array (
          'stockprop_id' => 'stockprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_study()
 * Purpose: To describe the structure of 'study' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'study' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_study() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'study_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'contact_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'pub_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'dbxref_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'study_id',
    ),
    'unique keys' => array (
      'study_c1' => array (
        0 => 'name',
      ),
    ),
    'indexes' => array (
      'study_idx1' => array (
        0 => 'contact_id',
      ),
      'study_idx2' => array (
        0 => 'pub_id',
      ),
      'study_idx3' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'pub' => array (
        'table' => 'pub',
        'columns' => array (
          'pub_id' => 'pub_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_study_assay()
 * Purpose: To describe the structure of 'study_assay' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'study_assay' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_study_assay() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'study_assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'study_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'study_assay_id',
    ),
    'unique keys' => array (
      'study_assay_c1' => array (
        0 => 'study_id',
        1 => 'assay_id',
      ),
    ),
    'indexes' => array (
      'study_assay_idx1' => array (
        0 => 'study_id',
      ),
      'study_assay_idx2' => array (
        0 => 'assay_id',
      ),
    ),
    'foreign keys' => array (
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
      'study' => array (
        'table' => 'study',
        'columns' => array (
          'study_id' => 'study_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studydesign()
 * Purpose: To describe the structure of 'studydesign' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studydesign' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studydesign() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'studydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'study_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'studydesign_id',
    ),
    'indexes' => array (
      'studydesign_idx1' => array (
        0 => 'study_id',
      ),
    ),
    'foreign keys' => array (
      'study' => array (
        'table' => 'study',
        'columns' => array (
          'study_id' => 'study_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studydesignprop()
 * Purpose: To describe the structure of 'studydesignprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studydesignprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studydesignprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'studydesignprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'studydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'studydesignprop_id',
    ),
    'unique keys' => array (
      'studydesignprop_c1' => array (
        0 => 'studydesign_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'studydesignprop_idx1' => array (
        0 => 'studydesign_id',
      ),
      'studydesignprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'studydesign' => array (
        'table' => 'studydesign',
        'columns' => array (
          'studydesign_id' => 'studydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyfactor()
 * Purpose: To describe the structure of 'studyfactor' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyfactor' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyfactor() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'studyfactor_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'studydesign_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => true,
      ),
      'description' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'studyfactor_id',
    ),
    'indexes' => array (
      'studyfactor_idx1' => array (
        0 => 'studydesign_id',
      ),
      'studyfactor_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'studydesign' => array (
        'table' => 'studydesign',
        'columns' => array (
          'studydesign_id' => 'studydesign_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyfactorvalue()
 * Purpose: To describe the structure of 'studyfactorvalue' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyfactorvalue' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyfactorvalue() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'studyfactorvalue_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'studyfactor_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'assay_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'factorvalue' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'studyfactorvalue_id',
    ),
    'indexes' => array (
      'studyfactorvalue_idx1' => array (
        0 => 'studyfactor_id',
      ),
      'studyfactorvalue_idx2' => array (
        0 => 'assay_id',
      ),
    ),
    'foreign keys' => array (
      'assay' => array (
        'table' => 'assay',
        'columns' => array (
          'assay_id' => 'assay_id',
        ),
      ),
      'studyfactor' => array (
        'table' => 'studyfactor',
        'columns' => array (
          'studyfactor_id' => 'studyfactor_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyprop()
 * Purpose: To describe the structure of 'studyprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyprop' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyprop() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'studyprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'study_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'studyprop_id',
    ),
    'unique keys' => array (
      'study_id' => array (
        0 => 'study_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'studyprop_idx1' => array (
        0 => 'study_id',
      ),
      'studyprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'study' => array (
        'table' => 'study',
        'columns' => array (
          'study_id' => 'study_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_studyprop_feature()
 * Purpose: To describe the structure of 'studyprop_feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyprop_feature' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_studyprop_feature() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'studyprop_feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'studyprop_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'feature_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'studyprop_feature_id',
    ),
    'unique keys' => array (
      'studyprop_id' => array (
        0 => 'studyprop_id',
        1 => 'feature_id',
      ),
    ),
    'indexes' => array (
      'studyprop_feature_idx1' => array (
        0 => 'studyprop_id',
      ),
      'studyprop_feature_idx2' => array (
        0 => 'feature_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'feature_id' => 'feature_id',
        ),
      ),
      'studyprop' => array (
        'table' => 'studyprop',
        'columns' => array (
          'studyprop_id' => 'studyprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_synonym()
 * Purpose: To describe the structure of 'synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'synonym' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_synonym() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'synonym_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'synonym_sgml' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'synonym_id',
    ),
    'unique keys' => array (
      'synonym_c1' => array (
        0 => 'name',
        1 => 'type_id',
      ),
    ),
    'indexes' => array (
      'synonym_idx1' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_tableinfo()
 * Purpose: To describe the structure of 'tableinfo' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'tableinfo' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_tableinfo() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'tableinfo_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '30',
        'not null' => true,
      ),
      'primary_key_column' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '30',
        'not null' => false,
      ),
      'is_view' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
      'view_on_table_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'superclass_table_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'is_updateable' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 1,
      ),
      'modification_date' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'date',
        'not null' => true,
        'default' => 'ow(',
      ),
    ),
    'primary key' => array (
      0 => 'tableinfo_id',
    ),
    'unique keys' => array (
      'tableinfo_c1' => array (
        0 => 'name',
      ),
    ),
    'foreign keys' => array (
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_treatment()
 * Purpose: To describe the structure of 'treatment' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'treatment' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_core_chado_schema_v1_2_treatment() {
  $description =  array (
    'description' => 'TODO: please describe this table!',
    'fields' => array (
      'treatment_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => true,
      ),
      'rank' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
      'biomaterial_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => true,
      ),
      'protocol_id' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => false,
      ),
      'name' => array (
        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => false,
      ),
    ),
    'primary key' => array (
      0 => 'treatment_id',
    ),
    'indexes' => array (
      'treatment_idx1' => array (
        0 => 'biomaterial_id',
      ),
      'treatment_idx2' => array (
        0 => 'type_id',
      ),
      'treatment_idx3' => array (
        0 => 'protocol_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'protocol' => array (
        'table' => 'protocol',
        'columns' => array (
          'protocol_id' => 'protocol_id',
        ),
      ),
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
          'biomaterial_id' => 'biomaterial_id',
        ),
      ),
    ),
  );
  return $description;
}
