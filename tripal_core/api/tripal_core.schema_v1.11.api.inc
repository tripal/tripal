<?php
/**
 * Implements hook_chado_schema_v1_11_acquisition()
 * Purpose: To describe the structure of 'acquisition' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'acquisition' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_acquisition() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_acquisition_relationship()
 * Purpose: To describe the structure of 'acquisition_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'acquisition_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_acquisition_relationship() {
  $description =  array (
    'foreign keys' => array (
      'acquisition' => array (
        'table' => 'acquisition',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'acquisitionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_acquisitionprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_analysis()
 * Purpose: To describe the structure of 'analysis' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysis' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysis() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_analysisfeature()
 * Purpose: To describe the structure of 'analysisfeature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysisfeature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysisfeature() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_analysisfeatureprop()
 * Purpose: To describe the structure of 'analysisfeatureprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysisfeatureprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysisfeatureprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_analysisprop()
 * Purpose: To describe the structure of 'analysisprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'analysisprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysisprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_arraydesign()
 * Purpose: To describe the structure of 'arraydesign' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'arraydesign' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_arraydesign() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_arraydesignprop()
 * Purpose: To describe the structure of 'arraydesignprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'arraydesignprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_arraydesignprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_assay()
 * Purpose: To describe the structure of 'assay' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assay' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assay() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_assay_biomaterial()
 * Purpose: To describe the structure of 'assay_biomaterial' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assay_biomaterial' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assay_biomaterial() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_assay_project()
 * Purpose: To describe the structure of 'assay_project' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assay_project' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assay_project() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_assayprop()
 * Purpose: To describe the structure of 'assayprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'assayprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_assayprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_biomaterial()
 * Purpose: To describe the structure of 'biomaterial' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_biomaterial_dbxref()
 * Purpose: To describe the structure of 'biomaterial_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_biomaterial_relationship()
 * Purpose: To describe the structure of 'biomaterial_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial_relationship() {
  $description =  array (
    'foreign keys' => array (
      'biomaterial' => array (
        'table' => 'biomaterial',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterial_treatment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterial_treatment() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_biomaterialprop()
 * Purpose: To describe the structure of 'biomaterialprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'biomaterialprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_biomaterialprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line()
 * Purpose: To describe the structure of 'cell_line' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_cell_line_cvterm()
 * Purpose: To describe the structure of 'cell_line_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line_cvtermprop()
 * Purpose: To describe the structure of 'cell_line_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_cvtermprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line_dbxref()
 * Purpose: To describe the structure of 'cell_line_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line_feature()
 * Purpose: To describe the structure of 'cell_line_feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_feature() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line_library()
 * Purpose: To describe the structure of 'cell_line_library' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_library' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_library() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line_pub()
 * Purpose: To describe the structure of 'cell_line_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_line_relationship()
 * Purpose: To describe the structure of 'cell_line_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_relationship() {
  $description =  array (
    'foreign keys' => array (
      'cell_line' => array (
        'table' => 'cell_line',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_line_synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_line_synonym() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_lineprop()
 * Purpose: To describe the structure of 'cell_lineprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_lineprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_lineprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cell_lineprop_pub()
 * Purpose: To describe the structure of 'cell_lineprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cell_lineprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cell_lineprop_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_chadoprop()
 * Purpose: To describe the structure of 'chadoprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'chadoprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_chadoprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_channel()
 * Purpose: To describe the structure of 'channel' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'channel' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_channel() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_contact()
 * Purpose: To describe the structure of 'contact' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'contact' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_contact() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_contact_relationship()
 * Purpose: To describe the structure of 'contact_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'contact_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_contact_relationship() {
  $description =  array (
    'foreign keys' => array (
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'control' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_control() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cv()
 * Purpose: To describe the structure of 'cv' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cv' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cv() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_cvprop()
 * Purpose: To describe the structure of 'cvprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_cvterm()
 * Purpose: To describe the structure of 'cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_cvterm_dbxref()
 * Purpose: To describe the structure of 'cvterm_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvterm_dbxref() {
  $description =  array (
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvterm_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvterm_relationship() {
  $description =  array (
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvtermpath' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvtermpath() {
  $description =  array (
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvtermprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_cvtermsynonym()
 * Purpose: To describe the structure of 'cvtermsynonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'cvtermsynonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_cvtermsynonym() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_db()
 * Purpose: To describe the structure of 'db' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'db' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_db() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_dbxref()
 * Purpose: To describe the structure of 'dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_dbxref() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_dbxrefprop()
 * Purpose: To describe the structure of 'dbxrefprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_dbxrefprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_eimage()
 * Purpose: To describe the structure of 'eimage' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'eimage' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_eimage() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_element()
 * Purpose: To describe the structure of 'element' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'element' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_element() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_element_relationship()
 * Purpose: To describe the structure of 'element_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'element_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_element_relationship() {
  $description =  array (
    'foreign keys' => array (
      'element' => array (
        'table' => 'element',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'elementresult' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_elementresult() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_elementresult_relationship()
 * Purpose: To describe the structure of 'elementresult_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'elementresult_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_elementresult_relationship() {
  $description =  array (
    'foreign keys' => array (
      'elementresult' => array (
        'table' => 'elementresult',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'environment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_environment() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_environment_cvterm()
 * Purpose: To describe the structure of 'environment_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'environment_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_environment_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_expression()
 * Purpose: To describe the structure of 'expression' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_expression_cvterm()
 * Purpose: To describe the structure of 'expression_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_expression_cvtermprop()
 * Purpose: To describe the structure of 'expression_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_cvtermprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_expression_image()
 * Purpose: To describe the structure of 'expression_image' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_image' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_image() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_expression_pub()
 * Purpose: To describe the structure of 'expression_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expression_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expression_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_expressionprop()
 * Purpose: To describe the structure of 'expressionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'expressionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_expressionprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature()
 * Purpose: To describe the structure of 'feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_feature_cvterm()
 * Purpose: To describe the structure of 'feature_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_cvterm_dbxref()
 * Purpose: To describe the structure of 'feature_cvterm_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvterm_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvterm_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_cvterm_pub()
 * Purpose: To describe the structure of 'feature_cvterm_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvterm_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvterm_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_cvtermprop()
 * Purpose: To describe the structure of 'feature_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_cvtermprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_feature_dbxref()
 * Purpose: To describe the structure of 'feature_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_expression()
 * Purpose: To describe the structure of 'feature_expression' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_expression' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_expression() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_expressionprop()
 * Purpose: To describe the structure of 'feature_expressionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_expressionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_expressionprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_genotype()
 * Purpose: To describe the structure of 'feature_genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_genotype() {
  $description =  array (
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'chromosome_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_phenotype()
 * Purpose: To describe the structure of 'feature_phenotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_phenotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_phenotype() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_pub()
 * Purpose: To describe the structure of 'feature_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_pubprop()
 * Purpose: To describe the structure of 'feature_pubprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_pubprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_pubprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_feature_relationship()
 * Purpose: To describe the structure of 'feature_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationship() {
  $description =  array (
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'object_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_relationship_pub()
 * Purpose: To describe the structure of 'feature_relationship_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationship_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_feature_relationshipprop()
 * Purpose: To describe the structure of 'feature_relationshipprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationshipprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationshipprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_feature_relationshipprop_pub()
 * Purpose: To describe the structure of 'feature_relationshipprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_relationshipprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_relationshipprop_pub() {
  $description =  array (
    'foreign keys' => array (
      'feature_relationshipprop' => array (
        'table' => 'feature_relationshipprop',
        'columns' => array (
          'feature_relationshipprop_id' => 'feature_relationshipprop_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_feature_synonym()
 * Purpose: To describe the structure of 'feature_synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'feature_synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_feature_synonym() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_featureloc()
 * Purpose: To describe the structure of 'featureloc' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureloc' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureloc() {
  $description =  array (
    'foreign keys' => array (
      'feature' => array (
        'table' => 'feature',
        'columns' => array (
          'srcfeature_id' => 'feature_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_featureloc_pub()
 * Purpose: To describe the structure of 'featureloc_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureloc_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureloc_pub() {
  $description =  array (
    'foreign keys' => array (
      'featureloc' => array (
        'table' => 'featureloc',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featuremap' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featuremap() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_featuremap_pub()
 * Purpose: To describe the structure of 'featuremap_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featuremap_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featuremap_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_featurepos()
 * Purpose: To describe the structure of 'featurepos' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featurepos' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featurepos() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_featureprop()
 * Purpose: To describe the structure of 'featureprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_featureprop_pub()
 * Purpose: To describe the structure of 'featureprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featureprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featureprop_pub() {
  $description =  array (
    'foreign keys' => array (
      'featureprop' => array (
        'table' => 'featureprop',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'featurerange' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_featurerange() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_genotype()
 * Purpose: To describe the structure of 'genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_genotype() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_genotypeprop()
 * Purpose: To describe the structure of 'genotypeprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'genotypeprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_genotypeprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_library()
 * Purpose: To describe the structure of 'library' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_library_cvterm()
 * Purpose: To describe the structure of 'library_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_library_dbxref()
 * Purpose: To describe the structure of 'library_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_library_feature()
 * Purpose: To describe the structure of 'library_feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_feature() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_library_pub()
 * Purpose: To describe the structure of 'library_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_library_synonym()
 * Purpose: To describe the structure of 'library_synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'library_synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_library_synonym() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_libraryprop()
 * Purpose: To describe the structure of 'libraryprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'libraryprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_libraryprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_libraryprop_pub()
 * Purpose: To describe the structure of 'libraryprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'libraryprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_libraryprop_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_magedocumentation()
 * Purpose: To describe the structure of 'magedocumentation' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'magedocumentation' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_magedocumentation() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_mageml()
 * Purpose: To describe the structure of 'mageml' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'mageml' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_mageml() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_materialized_view()
 * Purpose: To describe the structure of 'materialized_view' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'materialized_view' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_materialized_view() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_nd_experiment()
 * Purpose: To describe the structure of 'nd_experiment' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_contact()
 * Purpose: To describe the structure of 'nd_experiment_contact' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_contact' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_contact() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_dbxref()
 * Purpose: To describe the structure of 'nd_experiment_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_genotype()
 * Purpose: To describe the structure of 'nd_experiment_genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_genotype() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_phenotype()
 * Purpose: To describe the structure of 'nd_experiment_phenotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_phenotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_phenotype() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_project()
 * Purpose: To describe the structure of 'nd_experiment_project' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_project' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_project() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_protocol()
 * Purpose: To describe the structure of 'nd_experiment_protocol' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_protocol' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_protocol() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_pub()
 * Purpose: To describe the structure of 'nd_experiment_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_stock()
 * Purpose: To describe the structure of 'nd_experiment_stock' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_stock' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_stock() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_stock_dbxref()
 * Purpose: To describe the structure of 'nd_experiment_stock_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_stock_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_experiment_stockprop()
 * Purpose: To describe the structure of 'nd_experiment_stockprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experiment_stockprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experiment_stockprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_nd_experimentprop()
 * Purpose: To describe the structure of 'nd_experimentprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_experimentprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_experimentprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_geolocation()
 * Purpose: To describe the structure of 'nd_geolocation' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_geolocation' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_geolocation() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_nd_geolocationprop()
 * Purpose: To describe the structure of 'nd_geolocationprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_geolocationprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_geolocationprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_protocol()
 * Purpose: To describe the structure of 'nd_protocol' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_protocol' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_protocol() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_nd_protocol_reagent()
 * Purpose: To describe the structure of 'nd_protocol_reagent' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_protocol_reagent' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_protocol_reagent() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_protocolprop()
 * Purpose: To describe the structure of 'nd_protocolprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_protocolprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_protocolprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_nd_reagent()
 * Purpose: To describe the structure of 'nd_reagent' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_reagent' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_reagent() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_nd_reagent_relationship()
 * Purpose: To describe the structure of 'nd_reagent_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_reagent_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_reagent_relationship() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_nd_reagentprop()
 * Purpose: To describe the structure of 'nd_reagentprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'nd_reagentprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_nd_reagentprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_organism()
 * Purpose: To describe the structure of 'organism' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organism' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_organism() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_organism_dbxref()
 * Purpose: To describe the structure of 'organism_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organism_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_organism_dbxref() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_organismprop()
 * Purpose: To describe the structure of 'organismprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'organismprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_organismprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_phendesc()
 * Purpose: To describe the structure of 'phendesc' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phendesc' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phendesc() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phenotype()
 * Purpose: To describe the structure of 'phenotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype() {
  $description =  array (
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype_comparison' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype_comparison() {
  $description =  array (
    'foreign keys' => array (
      'phenotype' => array (
        'table' => 'phenotype',
        'columns' => array (
          'phenotype2_id' => 'phenotype_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_phenotype_comparison_cvterm()
 * Purpose: To describe the structure of 'phenotype_comparison_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype_comparison_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype_comparison_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phenotype_cvterm()
 * Purpose: To describe the structure of 'phenotype_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenotype_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenotype_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phenstatement()
 * Purpose: To describe the structure of 'phenstatement' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phenstatement' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phenstatement() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phylonode()
 * Purpose: To describe the structure of 'phylonode' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode() {
  $description =  array (
    'foreign keys' => array (
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phylonode_organism()
 * Purpose: To describe the structure of 'phylonode_organism' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_organism' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_organism() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phylonode_pub()
 * Purpose: To describe the structure of 'phylonode_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phylonode_relationship()
 * Purpose: To describe the structure of 'phylonode_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonode_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonode_relationship() {
  $description =  array (
    'foreign keys' => array (
      'phylonode' => array (
        'table' => 'phylonode',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylonodeprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylonodeprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_phylotree()
 * Purpose: To describe the structure of 'phylotree' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylotree' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylotree() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_phylotree_pub()
 * Purpose: To describe the structure of 'phylotree_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'phylotree_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_phylotree_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_project()
 * Purpose: To describe the structure of 'project' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_project() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_project_contact()
 * Purpose: To describe the structure of 'project_contact' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project_contact' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_project_contact() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_project_pub()
 * Purpose: To describe the structure of 'project_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_project_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_project_relationship()
 * Purpose: To describe the structure of 'project_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'project_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_project_relationship() {
  $description =  array (
    'foreign keys' => array (
      'project' => array (
        'table' => 'project',
        'columns' => array (
          'object_project_id' => 'project_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_projectprop()
 * Purpose: To describe the structure of 'projectprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'projectprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_projectprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_protocol()
 * Purpose: To describe the structure of 'protocol' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'protocol' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_protocol() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_protocolparam()
 * Purpose: To describe the structure of 'protocolparam' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'protocolparam' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_protocolparam() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_pub()
 * Purpose: To describe the structure of 'pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pub() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_pub_dbxref()
 * Purpose: To describe the structure of 'pub_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pub_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pub_dbxref() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_pub_relationship()
 * Purpose: To describe the structure of 'pub_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pub_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pub_relationship() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_pubauthor()
 * Purpose: To describe the structure of 'pubauthor' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pubauthor' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pubauthor() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_pubprop()
 * Purpose: To describe the structure of 'pubprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'pubprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_pubprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_quantification()
 * Purpose: To describe the structure of 'quantification' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'quantification' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_quantification() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_quantification_relationship()
 * Purpose: To describe the structure of 'quantification_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'quantification_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_quantification_relationship() {
  $description =  array (
    'foreign keys' => array (
      'quantification' => array (
        'table' => 'quantification',
        'columns' => array (
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
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'quantificationprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_quantificationprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock()
 * Purpose: To describe the structure of 'stock' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_stock_cvterm()
 * Purpose: To describe the structure of 'stock_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_cvtermprop()
 * Purpose: To describe the structure of 'stock_cvtermprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_cvtermprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_cvtermprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_dbxref()
 * Purpose: To describe the structure of 'stock_dbxref' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_dbxref' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_dbxref() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_dbxrefprop()
 * Purpose: To describe the structure of 'stock_dbxrefprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_dbxrefprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_dbxrefprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_genotype()
 * Purpose: To describe the structure of 'stock_genotype' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_genotype' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_genotype() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_pub()
 * Purpose: To describe the structure of 'stock_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_relationship()
 * Purpose: To describe the structure of 'stock_relationship' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_relationship' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_relationship() {
  $description =  array (
    'foreign keys' => array (
      'stock' => array (
        'table' => 'stock',
        'columns' => array (
          'object_id' => 'stock_id',
        ),
      ),
    ),
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_stock_relationship_cvterm()
 * Purpose: To describe the structure of 'stock_relationship_cvterm' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_relationship_cvterm' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_relationship_cvterm() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stock_relationship_pub()
 * Purpose: To describe the structure of 'stock_relationship_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stock_relationship_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stock_relationship_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stockcollection()
 * Purpose: To describe the structure of 'stockcollection' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockcollection' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockcollection() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stockcollection_stock()
 * Purpose: To describe the structure of 'stockcollection_stock' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockcollection_stock' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockcollection_stock() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stockcollectionprop()
 * Purpose: To describe the structure of 'stockcollectionprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockcollectionprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockcollectionprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stockprop()
 * Purpose: To describe the structure of 'stockprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_stockprop_pub()
 * Purpose: To describe the structure of 'stockprop_pub' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'stockprop_pub' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_stockprop_pub() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_study()
 * Purpose: To describe the structure of 'study' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'study' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_study() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_study_assay()
 * Purpose: To describe the structure of 'study_assay' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'study_assay' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_study_assay() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_studydesign()
 * Purpose: To describe the structure of 'studydesign' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studydesign' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studydesign() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_studydesignprop()
 * Purpose: To describe the structure of 'studydesignprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studydesignprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studydesignprop() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_studyfactor()
 * Purpose: To describe the structure of 'studyfactor' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyfactor' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyfactor() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_studyfactorvalue()
 * Purpose: To describe the structure of 'studyfactorvalue' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyfactorvalue' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyfactorvalue() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_studyprop()
 * Purpose: To describe the structure of 'studyprop' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyprop() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_studyprop_feature()
 * Purpose: To describe the structure of 'studyprop_feature' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'studyprop_feature' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_studyprop_feature() {
  $description =  array (
    'foreign keys' => array (
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
 * Implements hook_chado_schema_v1_11_synonym()
 * Purpose: To describe the structure of 'synonym' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'synonym' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_synonym() {
  $description =  array (
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
 * Implements hook_chado_schema_v1_11_tableinfo()
 * Purpose: To describe the structure of 'tableinfo' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'tableinfo' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_tableinfo() {
  $description =  array (
  );
  return $description;
}
/**
 * Implements hook_chado_schema_v1_11_treatment()
 * Purpose: To describe the structure of 'treatment' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the 'treatment' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_treatment() {
  $description =  array (
    'foreign keys' => array (
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
