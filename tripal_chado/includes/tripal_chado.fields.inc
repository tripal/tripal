<?php

/**
 * Implements hook_chado_bundle_create().
 *
 * This is a Tripal hook. It allows any module to perform tasks after
 * a bundle has been created.
 *
 * @param $bundle
 *  The TripalBundle object.
 */

function tripal_chado_bundle_create($bundle) {
  $entity_type = $bundle->type;

  // Create/Add the new fields for this bundle.
  tripal_chado_bundle_create_fields($entity_type, $bundle);

  // Create/Add the new field instances for this bundle.
  tripal_chado_bundle_create_instances($entity_type, $bundle);
}



/**
 * Implements hook_field_create_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_bundle_create_fields($entity_type, $bundle) {

  // Get the table this bundle is mapped to.
  $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
  $vocab = $term->vocab;
  $params = array(
    'vocabulary' => $vocab->vocabulary,
    'accession' => $term->accession,
  );
  $mapped_table = chado_get_cvterm_mapping($params);

  // Get the details about the mapping of this bundle to the Chado table:
  $details = array(
    'chado_cv_id' => $mapped_table->cvterm->cv_id->cv_id,
    'chado_cvterm_id' => $mapped_table->cvterm->cvterm_id,
    'chado_table' => $mapped_table->chado_table,
    'chado_type_table' => $mapped_table->chado_table,
    'chado_type_column' => $mapped_table->chado_field,
  );

  $info = array();

  // Create the fields for each column in the table.
  tripal_chado_bundle_create_fields_base($info, $details, $entity_type, $bundle);

  // Create custom fields.
  tripal_chado_bundle_create_fields_custom($info, $details, $entity_type, $bundle);

  // Create fields for linking tables.
  tripal_chado_bundle_create_fields_linker($info, $details, $entity_type, $bundle);

  foreach ($info as $field_name => $details) {
    $field_type = $details['type'];

    // If the field already exists then skip it.
    $field = field_info_field($details['field_name']);
    if ($field) {
      continue;
    }

    // Create the field.
    $field = field_create_field($details);
    if (!$field) {
      tripal_set_message(t("Could not create new field: %field.",
          array('%field' =>  $details['field_name'])), TRIPAL_ERROR);
    }
  }
}
/**
 *
 * @param unknown $details
 */
function tripal_chado_bundle_create_fields_base(&$info, $details, $entity_type, $bundle) {

  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];

  // Iterate through the columns of the table and see if fields have been
  // created for each one. If not, then create them.
  $schema = chado_get_schema($table_name);
  if (!$schema) {
    return;
  }

  $pkey = $schema['primary key'][0];


  // Get the list of columns for this table and create a new field for each one.
  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {
    // Don't create base fields for the primary key and the type_id field.
    if ($column_name == $pkey or $column_name == $type_field) {
      continue;
    }
    $cvterm = tripal_get_chado_semweb_term($table_name, $column_name, array('return_object' => TRUE));
    if (!$cvterm) {
      tripal_report_error('tripal', TRIPAL_ERROR,
        'Cannot create term for "%table_name.%column_name". Missing an appropriate vocabulary term',
         array('%table_name' => $table_name, '%column_name' => $column_name));
      drupal_set_message(t('Cannot create term for "%table_name.%column_name". Missing an appropriate vocabulary term',
        array('%table_name' => $table_name, '%column_name' => $column_name)), 'error');
      continue;
    }
    $field_name = strtolower($cvterm->dbxref_id->db_id->name . '__' . preg_replace('/ /', '_', $cvterm->name));

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // Skip the type field.
    if ($table_name == $type_table and $column_name == $type_field) {
      continue;
    }

    // Set some defaults for the field.
    $base_info = array(
      'field_name' => $field_name,
      'type' => '',
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => $column_name,
        'base_table' => $table_name,
      ),
    );

    // Alter the field info array depending on the column details.
    switch($details['type']) {
      case 'char':
        $base_info['type'] = 'text';
        $base_info['settings']['max_length'] = $details['length'];
        break;
      case 'varchar':
        $base_info['type'] = 'text';
        $base_info['settings']['max_length'] = $details['length'];
        break;
      case 'text':
        $base_info['type'] = 'text';
        $base_info['settings']['max_length'] = 17179869184;
        $base_info['settings']['text_processing'] = 1;
        break;
      case 'blob':
        // not sure how to support a blob field.
        continue;
        break;
      case 'int':
        $base_info['type'] = 'number_integer';
        break;
      case 'float':
        $base_info['type'] = 'number_float';
        $base_info['settings']['precision'] = 10;
        $base_info['settings']['scale'] = 2;
        $base_info['settings']['decimal_separator'] = '.';
        break;
      case 'numeric':
        $base_info['type'] = 'number_decimal';
        break;
      case 'serial':
        // Serial fields are most likely not needed as a field.
        break;
      case 'boolean':
        $base_info['type'] = 'list_boolean';
        $base_info['settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
        break;
      case 'datetime':
        // Use the Drupal Date and Date API to create the field/widget
        $base_info['type'] = 'datetime';
        break;
    }

    // Set some default semantic web information
    if ($column_name == 'uniquename') {
      $base_info['settings']['text_processing'] = 0;
    }
    //
    // PUB TABLE
    //
    elseif ($table_name == 'pub' and $column_name == 'uniquename') {
      $base_info['type'] = 'text';
      $base_info['settings']['text_processing'] = 0;
    }

    //
    // ANALYSIS TABLE
    //
    elseif ($table_name == 'analysis' and $column_name == 'sourceuri') {
      $base_info['type'] = 'text';
      $base_info['settings']['text_processing'] = 0;
    }

    $info[$field_name] = $base_info;
  }
}

/**
 *
 * @param unknown $details
 */
function tripal_chado_bundle_create_fields_custom(&$info, $details, $entity_type, $bundle) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];
  $schema = chado_get_schema($table_name);


  // BASE ORGANISM_ID
  if ($table_name != 'organism' and array_key_exists('organism_id', $schema['fields'])) {
    $field_name = 'obi__organism';
    $field_type = 'obi__organism';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'organism_id',
        'base_table' => $table_name,
      ),
    );
  }

  // BASE DBXREF
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_name = 'data__accession';
    $field_type = 'data__accession';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'dbxref_id',
        'base_table' => $table_name,
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'dbxref_id'),
      ),
    );
  }

  // FEATURE MD5CHECKSUM
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_checksum';
    $field_type = 'data__sequence_checksum';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'base_table' => $table_name,
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'md5checksum'),
      ),
    );
  }

  // FEATURE RESIDUES
  if ($table_name == 'feature') {
    $field_name = 'data__sequence';
    $field_type = 'data__sequence';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'residues',
        'base_table' => $table_name,
      ),
    );
  }

  // FEATURE SEQLEN
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_length';
    $field_type = 'data__sequence_length';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'seqlen',
        'base_table' => $table_name,
      ),
    );
  }

  // GENE TRANSCRIPTS
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table) and $bundle->label == 'gene') {
    $field_name = 'so__transcript';
    $field_type = 'so__transcript';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => '',
        'base_table' => $table_name,
      ),
    );
  }

  // ORGANISM TYPE_ID
//   if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
//     $field_name = 'taxarank__infraspecific_taxon';
//     $field_type = 'taxarank__infraspecific_taxon';
//     $info[$field_name] = array(
//       'field_name' => $field_name,
//       'type' => $field_type,
//       'cardinality' => 1,
//       'locked' => TRUE,
//       'storage' => array(
//         'type' => 'field_chado_storage',
//       ),
//       'settings' => array(
//         'chado_table' => 'organism',
//         'chado_column' => 'type_id',
//       ),
//     );
//   }
}

/**
 *
 * @param unknown $details
 */
function tripal_chado_bundle_create_fields_linker(&$info, $details, $entity_type, $bundle) {

  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];

  // CONTACTS
  $contact_table = $table_name . '_contact';
  if (chado_table_exists($contact_table)) {
    $schema = chado_get_schema($contact_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_contact';
    $field_type = 'chado_linker__contact';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $contact_table,
        'base_table' => $table_name,
        'chado_column' => 'contact_id',
      ),
    );
  }

  // DBXREF
  $dbxref_table = $table_name . '_dbxref';
  if (chado_table_exists($dbxref_table)) {
    $dbxref_table = $table_name . '_dbxref';
    $schema = chado_get_schema($dbxref_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'sbo__database_cross_reference';
    $field_type = 'sbo__database_cross_reference';
    $info[$field_name] = array(
      'field_name' =>  $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $dbxref_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }

  // EXPRESSION
  $expression_table = $table_name . '_expression';
  if (chado_table_exists($expression_table)) {
    $schema = chado_get_schema($expression_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'go__gene_expression';
    $field_type = 'go__gene_expression';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $expression_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }

  // FEATURELOC
  if ($table_name == 'feature') {
    $schema = chado_get_schema('featureloc');
    $pkey = $schema['primary key'][0];
    $field_name = 'data__sequence_coordinates';
    $field_type = 'data__sequence_coordinates';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'featureloc',
        'chado_column' => $pkey,
        'base_table' => 'feature',
      ),
    );
  }

  // FEATUREPOS
  if ($table_name == 'feature') {
    $schema = chado_get_schema('featurepos');
    $pkey = $schema['primary key'][0];
    $field_name = 'ogi__location_on_map';
    $field_type = 'ogi__location_on_map';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'featurepos',
        'chado_column' => $pkey,
        'base_table' => 'feature',
      ),
    );
  }

  // GENOTYPE
  $genotype_table = $table_name . '_genotype';
  if (chado_table_exists($genotype_table)) {
    $schema = chado_get_schema($genotype_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'so__genotype';
    $field_type = 'so__genotype';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $genotype_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }

  // PHENOTYPE
  $phenotype_table = $table_name . '_phenotype';
  if (chado_table_exists($phenotype_table)) {
    $schema = chado_get_schema($phenotype_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'sbo__phenotype';
    $field_type = 'sbo__phenotype';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $phenotype_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }

  // PROPERTIES
  $prop_table = $table_name . 'prop';
  if (chado_table_exists($prop_table)) {
    // Get the list of existing property types for this table.
    $sql = 'SELECT DISTINCT type_id FROM {' . $prop_table . '}';
    $props = chado_query($sql);
    $schema = chado_get_schema($prop_table);
    $pkey = $schema['primary key'][0];
    while ($prop = $props->fetchObject()) {
      $term = chado_generate_var('cvterm', array('cvterm_id' => $prop->type_id));
      $field_name = strtolower($term->dbxref_id->db_id->name . '__' . $term->name);
      $field_type = 'chado_linker__prop';
      $info[$field_name] = array(
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => 1,
        'locked' => FALSE,
        'storage' => array(
          'type' => 'field_chado_storage',
        ),
        'settings' => array(
          'base_table' => $table_name,
          'chado_table' => $prop_table,
          'chado_column' => $pkey,
        ),
      );
    }
  }

  // PUBLICATIONS
  $pub_table = $table_name . '_pub';
  if (chado_table_exists($pub_table)) {
    $schema = chado_get_schema($pub_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'schema__publication';
    $field_type = 'schema__publication';
    $info[$field_name] =  array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $pub_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }

  // RELATIONSHIPS
  // If the linker table does not exists then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table)) {
    $schema = chado_get_schema($rel_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'sbo__relationship';
    $field_type = 'sbo__relationship';
    $info[$field_name] =  array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }

  // SYNONYMS
  $syn_table = $table_name . '_synonym';
  if (chado_table_exists($syn_table)) {
    $schema = chado_get_schema($syn_table);
    $pkey = $schema['primary key'][0];
    $field_name = 'schema__alternate_name';
    $field_type = 'schema__alternate_name';
    $info[$field_name] =  array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $syn_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
      ),
    );
  }
}

/**
 * Impelments hook_create_tripalfield_instance().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_bundle_create_instances($entity_type, $bundle) {

  $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
  $vocab = $term->vocab;
  $params = array(
    'vocabulary' => $vocab->vocabulary,
    'accession' => $term->accession,
  );
  $mapped_table = chado_get_cvterm_mapping($params);

  // Get the details about the mapping of this bundle to the Chado table:
  $details = array(
    'chado_cv_id' => $mapped_table->cvterm->cv_id->cv_id,
    'chado_cvterm_id' => $mapped_table->cvterm->cvterm_id,
    'chado_table' => $mapped_table->chado_table,
    'chado_type_table' => $mapped_table->chado_table,
    'chado_type_column' => $mapped_table->chado_field,
  );

  tripal_chado_bundle_create_instances_base($info, $entity_type, $bundle, $details);
  tripal_chado_bundle_create_instances_custom($info, $entity_type, $bundle, $details);
  tripal_chado_bundle_create_instances_linker($info, $entity_type, $bundle, $details);

  foreach ($info as $field_name => $details) {
    // If the field is already attached to this bundle then skip it.
    $field = field_info_field($details['field_name']);
    if ($field and array_key_exists('bundles', $field) and
        array_key_exists('TripalEntity', $field['bundles']) and
        in_array($bundle->name, $field['bundles']['TripalEntity'])) {
      continue;
    }
    // Create the field instance.
    $instance = field_create_instance($details);
  }

}
/**
 * Helper function for the hook_create_tripalfield_instance().
 *
 * Add the field instances that corresond to the columns of the base table.
 *
 * @param $entity_type
 * @param $bundle
 * @param $details
 */
function tripal_chado_bundle_create_instances_base(&$info, $entity_type, $bundle, $details) {
  $fields = array();

  // Get Chado information
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];

  // Iterate through the columns of the table and see if fields have been
  // created for each one. If not, then create them.
  $schema = chado_get_schema($table_name);
  if (!$schema) {
    return;
  }

  $pkey = $schema['primary key'][0];

  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {
    // Don't create base fields for the primary key and the type_id field.
    if ($column_name == $pkey or $column_name == $type_field) {
      continue;
    }
    $cvterm = tripal_get_chado_semweb_term($table_name, $column_name, array('return_object' => TRUE));
    if (!$cvterm) {
      // We already provided an error when creating the base field.  So
      // don't create another one here.
      continue;
    }

    $field_name = strtolower($cvterm->dbxref_id->db_id->name . '__' . preg_replace('/ /', '_', $cvterm->name));

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // Skip the type field.
    if ($table_name == $type_table and $column_name == $type_field) {
      continue;
    }

    $base_info =  array(
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
      'description' => '',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
        'term_vocabulary' => $cvterm->dbxref_id->db_id->name,
        'term_name' => $cvterm->name,
        'term_accession' => $cvterm->dbxref_id->accession,
      ),
      'widget' => array(
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'settings' => array(),
        ),
      ),
    );

    // Determine if the field is required.
    if (array_key_exists('not null', $details) and $details['not null'] === TRUE) {
      $base_info['required'] = TRUE;
    }

    // Alter the field info array depending on the column details.
    switch($details['type']) {
      case 'char':
        $base_info['widget']['type'] = 'text_textfield';
        break;
      case 'varchar':
        $base_info['widget']['type'] = 'text_textfield';
        break;
      case 'text':
        $base_info['widget']['type'] = 'text_textarea';
        $base_info['widget']['settings']['format'] = filter_default_format();
        break;
      case 'blob':
        // not sure how to support a blob field.
        continue;
        break;
      case 'int':
        $base_info['widget']['type'] = 'number';
        break;
      case 'float':
        $base_info['widget']['type'] = 'number';
        break;
      case 'numeric':
        $base_info['widget']['type'] = 'number';
        break;
      case 'serial':
        // Serial fields are most likely not needed as a field.
        break;
      case 'boolean':
        $base_info['widget']['type'] = 'options_onoff';
        $base_info['required'] = FALSE;
        break;
      case 'datetime':
        $base_info['widget']['type'] = 'date_select';
        $base_info['widget']['settings']['increment'] = 1;
        $base_info['widget']['settings']['tz_handling'] = 'none';
        $base_info['widget']['settings']['collapsible'] = TRUE;

        // TODO: Add settings so that the minutes increment by 1.
        // And turn off the timezone, as the Chado field doesn't support it.
        break;
    }

    // Set some default semantic web information
    if ($column_name == 'uniquename') {
      $base_info['label'] = 'Identifier';
      $base_info['widget_type'] = 'text_textfield';
    }
    elseif ($base_info['label'] == 'Timeaccessioned') {
      $base_info['label'] = 'Time Accessioned';
      $base_info['description'] = 'Please enter the time that this record was first added to the database.';
    }
    elseif ($base_info['label'] == 'Timelastmodified') {
      $base_info['label'] = 'Time Last Modified';
      $base_info['description'] = 'Please enter the time that this record was last modified. The default is the current time.';
    }
    //
    // ORGANISM TABLE
    //
    elseif ($table_name == 'organism' and $column_name == 'comment') {
      $base_info['label'] = 'Description';
    }
    //
    // PUB TABLE
    //
    elseif ($table_name == 'pub' and $column_name == 'uniquename') {
      $base_info['widget_type'] = 'text_textfield';
    }

    //
    // ANALYSIS TABLE
    //
    elseif ($table_name == 'analysis' and $column_name == 'program') {
      $base_info['description'] = 'The program name (e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package then provide a very brief description of the pipeline, workflow or method.';
      $base_info['label'] = 'Program, Pipeline, Workflow or Method Name.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'sourceuri') {
      $base_info['widget_type'] = 'text_textfield';
      $base_info['label'] = 'Source URL';
      $base_info['description'] = 'The URL where the original source data was derived.  Ideally, this should link to the page where more information about the source data can be found.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'sourcename') {
      $base_info['label'] = 'Source Name';
      $base_info['description'] = 'The name of the source data. This could be a file name, data set or a small description for how the data was collected. For long descriptions use the larger description field.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'sourceversion') {
      $base_info['label'] = 'Source Version';
      $base_info['description'] = 'If hte source data set has a version include it here.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'algorithm') {
      $base_info['label'] = 'Source Version';
      $base_info['description'] = 'The name of the algorithm used to produce the dataset if different from the program.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'programversion') {
      $base_info['label'] = 'Program Version';
      $base_info['description'] = 'The version of the program used to perform this analysis. (e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter "n/a" if no version is available or applicable.';
    }
    //
    // PROJECT TABLE
    //
    elseif ($table_name == 'project' and $column_name == 'description') {
      $base_info['label'] = 'Short Description';
    }
    $info[$field_name] = $base_info;
  }
}

/**
 * Helper function for the hook_create_tripalfield_instance().
 *
 * Adds custom fields for base fields.  These override the settings provided
 * in the tripal_chado_create_tripalfield_instance_base() function.
 *
 * @param $entity_type
 * @param $bundle
 * @param $details
 */
function tripal_chado_bundle_create_instances_custom(&$info, $entity_type, $bundle, $details) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];
  $schema = chado_get_schema($table_name);

  // BASE ORGANISM_ID
  if ($table_name != 'organism' and array_key_exists('organism_id', $schema['fields'])) {
    $field_name = 'obi__organism';
    $is_required = FALSE;
    if (array_key_exists('not null', $schema['fields']['organism_id']) and
        $schema['fields']['organism_id']['not null']) {
      $is_required = TRUE;
    }
    $info[$field_name] =  array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Organism',
      'description' => 'Select an organism.',
      'required' => $is_required,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'obi__organism_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'obi__organism_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // BASE DBXREF
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_name = 'data__accession';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Accession',
      'description' => 'This field specifies the unique stable accession (ID) for
        this record. It requires that this site have a database entry.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'data__accession_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'data__accession_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // FEATURE MD5CHECKSUM
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_checksum';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence Checksum',
      'description' => 'The MD5 checksum for the sequence. The checksum here
        will always be unique for the raw unformatted sequence. To verify that the
        sequence has not been corrupted, download the raw sequence and use an MD5 tool
        to calculate the value. If the value calculated is identical the one shown
        here, then the downloaded sequence is uncorrupted.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'data__sequence_checksum_widget',
        'settings' => array(
          'display_label' => 1,
          'md5_fieldname' => 'feature__md5checksum',
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'data__sequence_checksum_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // FEATURE RESIDUES
  if ($table_name == 'feature') {
    $field_name = 'data__sequence';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence',
      'description' => 'All available sequences for this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'data__sequence_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'data__sequence_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // FEATURE SEQLEN
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_length';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence Length',
      'description' => 'The number of residues in the raw sequence.  This length
        is only for the assigned raw sequence and does not represent the length of any
        sequences derived from alignments. If this value is zero but aligned sequences
        are present then this record has no official assigned sequence.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'data__sequence_length_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'data__sequence_length_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // GENE TRANSCRIPTS
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table) and $bundle->label == 'gene') {
    $field_name = 'so__transcript';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Transcripts',
      'description' => 'Transcripts that are part of this gene.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'so__transcript_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'so__transcript_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // ORGANISM TYPE_ID
//   if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
//     $field_name = 'taxarank__infraspecific_taxon';
//     $info[$field_name] = array(
//       'field_name' => $field_name,
//       'entity_type' => $entity_type,
//       'bundle' => $bundle->name,
//       'label' => 'Infraspecific Taxon',
//       'description' => 'The Infraspecific Taxon.',
//       'required' => FALSE,
//       'settings' => array(
//         'auto_attach' => TRUE,
//       ),
//       'widget' => array(
//         'type' => 'taxarank__infraspecific_taxon_widget',
//         'settings' => array(
//           'display_label' => 1,
//         ),
//       ),
//       'display' => array(
//         'default' => array(
//           'label' => 'inline',
//           'type' => 'taxarank__infraspecific_taxon_formatter',
//           'settings' => array(),
//         ),
//       ),
//     );
//   }
}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 * @param unknown $details
 */
function tripal_chado_bundle_create_instances_linker(&$info, $entity_type, $bundle, $details) {

  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];

  // CONTACTS
  $contact_table = $table_name . '_contact';
  if (chado_table_exists($contact_table)) {
    $field_name = $table_name . '_contact';
    $info[$field_name] =     $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Contact',
      'description' => 'Associates an indviddual or organization with this record',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'local__contact_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'local__contact_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // DBXREF
  $dbxref_table = $table_name . '_dbxref';
  if (chado_table_exists($dbxref_table)) {
    $field_name = 'sbo__database_cross_reference';
    $info[$field_name] =  array(
      'field_name' =>  $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Database Cross Reference',
      'description' => 'The IDs where this record may be available in other external online databases.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'sbo__database_cross_reference_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'sbo__database_cross_reference_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // EXPRESSION
  $expression_table = $table_name . '_expression';
  if (chado_table_exists($expression_table)) {
    $field_name = 'go__gene_expression';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Gene expression',
      'description' => 'Information about the expression of this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'go__gene_expression_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'go__gene_expression_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // FEATURELOC
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_coordinates';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence Coordinates',
      'description' => 'The locations on other genomic sequences where this
        record has been aligned.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'data__sequence_coordinates_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'data__sequence_coordinates_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // FEATUREPOS
  if ($table_name == 'feature') {
    $field_name = 'ogi__location_on_map';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Location on Map',
      'description' => 'The positions on a genetic map.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'ogi__location_on_map_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'ogi__location_on_map_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // GENOTYPE
  $genotype_table = $table_name . '_genotype';
  if (chado_table_exists($genotype_table)) {
    $field_name = 'so__genotype';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Genotype',
      'description' => 'The genotypes associated with this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'so__genotype_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'so__genotype_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // PHENOTYPE
  $phenotype_table = $table_name . '_phenotype';
  if (chado_table_exists($phenotype_table)) {
    $field_name = 'sbo__phenotype';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Phenotype',
      'description' => 'The phenotypes associated with this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'sbo__phenotype_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'sbo__phenotype_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // TODO: we need to add a property for each type that already exists
  // in the prop table.
   // PROPERTIES
   $prop_table = $table_name . 'prop';
   if (chado_table_exists($prop_table)) {
     // Get the list of existing property types for this table.
     $sql = 'SELECT DISTINCT type_id FROM {' . $prop_table . '}';
     $props = chado_query($sql);
     while ($prop = $props->fetchObject()) {
       $term = chado_generate_var('cvterm', array('cvterm_id' => $prop->type_id));
       $field_name = strtolower($term->dbxref_id->db_id->name . '__' . $term->name);
       $info[$field_name] = array(
         'field_name' => $field_name,
         'entity_type' => $entity_type,
         'bundle' => $bundle->name,
         'label' => ucwords(preg_replace('/_/', ' ', $term->name)),
         'description' => $term->definition,
         'required' => FALSE,
         'settings' => array(
           'auto_attach' => TRUE,
           'term_vocabulary' => $term->dbxref_id->db_id->name,
           'term_accession' => $term->dbxref_id->accession,
           'term_name' => $term->name,
         ),
         'widget' => array(
           'type' => 'chado_linker__prop_widget',
           'settings' => array(
             'display_label' => 1,
           ),
         ),
         'display' => array(
           'default' => array(
             'label' => 'inline',
             'type' => 'chado_linker__prop_formatter',
             'settings' => array(),
           ),
         ),
       );
     }
   }


  // PUBLICATIONS
  $pub_table = $table_name . '_pub';
  if (chado_table_exists($pub_table)) {
    $field_name = 'schema__publication';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Publication',
      'description' => 'This record has been referenced or is sourced from
        these publications.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'schema__publication_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'schema__publication_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // RELATIONSHIPS
  // If the linker table does not exists then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table)) {
    $field_name = 'sbo__relationship';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Relationship',
      'description' => 'Other records with relationships to this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'sbo__relationship_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'sbo__relationship_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  // SYNONYMS
  $syn_table = $table_name . '_synonym';
  if (chado_table_exists($syn_table)) {
    $field_name = 'schema__alternate_name';
    $info[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Synonyms',
      'description' => 'Alternate names, aliases or synonyms for this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'schema__alternate_name_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'schema__alternate_name_formatter',
          'settings' => array(),
        ),
      ),
    );
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_field_overview_form is used for ordering and configuring the
 * fields attached to an entity.
 *
 * This function removes the property adder field as that is really not meant
 * for users to show or manage.
 */
function tripal_chado_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {

  // Remove the kvproperty_addr field as it isn't ever displayed. It's just used
  // on the add/edit form of an entity for adding new property fields.
  $fields_names = element_children($form['fields']);
  foreach ($fields_names as $field_name) {
    $field_info = field_info_field($field_name);
    if ($field_info['type'] == 'kvproperty_adder') {
      unset($form['fields'][$field_name]);
    }
    if ($field_info['type'] == 'cvterm_class_adder') {
      unset($form['fields'][$field_name]);
    }
  }
}

/**
 * Implements hook_form_field_ui_field_overview_add_new().
 */
function tripal_chado_form_field_ui_field_overview_add_new($new_field, $bundle) {
  // Get the table this bundle is mapped to.
  $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
  $vocab = $term->vocab;
  $params = array(
    'vocabulary' => $vocab->vocabulary,
    'accession' => $term->accession,
  );
  $mapped_table = chado_get_cvterm_mapping($params);
  $chado_table = $mapped_table->chado_table;
  $chado_type_table = $mapped_table->chado_table;
  $chado_type_column = $mapped_table->chado_field;

  // We allow site admins to add new chado_linker__prop fields to an entity.
  // This function will allow us to properly add them.  But at this point we
  // don't know the controlled vocabulary term.  We'll have to use the
  // defaults and let the user set it using the interface.
  if ($new_field['type'] == 'chado_linker__prop') {
    $table_name = $chado_table . 'prop';

    if (chado_table_exists($table_name)) {
      $schema = chado_get_schema($table_name);
      $pkey = $schema['primary key'][0];
      $field_name = $new_field['field_name'];
      $field_type = 'chado_linker__prop';

      // First add the field.
      field_create_field(array(
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => array(
          'type' => 'field_chado_storage',
        ),
        'settings' => array(
          'base_table' => $chado_table,
          'chado_table' => $table_name,
          'chado_column' => $pkey,
        ),
      ));

      // Now add the instance
      field_create_instance(array(
        'field_name' => $field_name,
        'entity_type' => 'TripalEntity',
        'bundle' => $bundle->name,
        'label' => $new_field['label'],
        'description' => '',
        'required' => FALSE,
        'settings' => array(
          'auto_attach' => TRUE,
        ),
        'widget' => array(
          'type' => 'chado_linker__prop_widget',
          'settings' => array(
            'display_label' => 1,
          ),
        ),
        'display' => array(
          'default' => array(
            'label' => 'inline',
            'type' => 'chado_linker__prop_formatter',
            'settings' => array(),
          ),
        ),
      ));
    }
    else {
      drupal_set_message('Cannot add a property field to this entity. Chado does not support properties for this data type.', 'error');
    }
  }

  // We allow site admins to add new chado_linker__cvterm fields to an entity.
  // This function will allow us to properly add them.  But at this point we
  // don't know the controlled vocabulary term.  We'll have to use the
  // defaults and let the user set it using the interface.

  if ($new_field['type'] == 'chado_linker__cvterm') {
    $table_name = $chado_table . '_cvterm';

    if (chado_table_exists($table_name)) {
      $schema = chado_get_schema($table_name);
      $pkey = $schema['primary key'][0];
      $field_name = $new_field['field_name'];
      $field_type = 'chado_linker__cvterm';

      // First add the field.
      field_create_field(array(
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => array(
          'type' => 'field_chado_storage',
        ),
        'settings' => array(
          'base_table' => $chado_table,
          'chado_table' => $table_name,
          'chado_column' => $pkey,
        ),
      ));

      // Now add the instance
      field_create_instance(array(
        'field_name' => $field_name,
        'entity_type' => 'TripalEntity',
        'bundle' => $bundle->name,
        'label' => $new_field['label'],
        'description' => '',
        'required' => FALSE,
        'settings' => array(
          'auto_attach' => TRUE,
        ),
        'widget' => array(
          'type' => 'chado_linker__cvterm_widget',
          'settings' => array(
            'display_label' => 1,
          ),
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'chado_linker__cvterm_formatter',
            'settings' => array(),
          ),
        ),
      ));
    }
    else {
      drupal_set_message('Cannot add a property field to this entity. Chado does not support annotations for this data type.', 'error');
    }
  }
}
