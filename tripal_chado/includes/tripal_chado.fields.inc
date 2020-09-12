<?php

/**
 * Implements hook_bundle_fields_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_bundle_fields_info($entity_type, $bundle) {

  $chado_bundle = db_select('chado_bundle', 'cb')
    ->fields('cb')
    ->condition('bundle_id', $bundle->id)
    ->execute()
    ->fetchObject();

  // Get the details about the mapping of this bundle to the Chado table:
  $details = [
    'chado_cvterm_id' => $chado_bundle->type_id,
    'chado_table' => $chado_bundle->data_table,
    'chado_type_table' => $chado_bundle->type_linker_table,
    'chado_type_id' => $chado_bundle->type_id,
    'chado_type_column' => $chado_bundle->type_column,
    'chado_type_value' => $chado_bundle->type_value,
    'chado_base_type_id' => $chado_bundle->base_type_id,
  ];

  $info = [];

  // Create the fields for each column in the table.
  tripal_chado_bundle_fields_info_base($info, $details, $entity_type, $bundle);

  // Create custom fields.
  tripal_chado_bundle_fields_info_custom($info, $details, $entity_type, $bundle);

  // Create fields for linking tables.
  tripal_chado_bundle_fields_info_linker($info, $details, $entity_type, $bundle);

  return $info;

}

/**
 *
 * @param unknown $details
 */
function tripal_chado_bundle_fields_info_base(&$info, $details, $entity_type, $bundle) {

  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_column = $details['chado_type_column'];
  $cvterm_id = $details['chado_cvterm_id'];
  $type_value = $details['chado_type_value'];

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

    // Skip the source columns  in the analysis table. We have a custom
    // field for those columns
    if ($table_name == 'analysis' and ($column_name == 'sourceuri' or
        $column_name == 'sourceversion' or $column_name == 'sourcename')) {
      continue;
    }

    // Skip the infraspecific type_id and name from the organism table as we
    // have a special field for those.
    if ($table_name == 'organism' and ($column_name == 'type_id' or
        $column_name == 'infraspecific_name')) {
      continue;
    }

    // Skip the cvterm.is_relationshptype.
    if ($table_name == 'cvterm' and $column_name == 'is_relationshiptype') {
      continue;
    }

    // The biosourceprovider_id and taxon_id are handled by custom fields.
    if ($table_name == 'biomaterial' and (
        $column_name == 'biosourceprovider_id' or $column_name == 'taxon_id')) {
      continue;
    }

    // Don't create base fields for the primary key and the type_id field.
    if ($column_name == $pkey or $column_name == $type_column) {
      continue;
    }
    $cvterm = chado_get_semweb_term($table_name, $column_name, ['return_object' => TRUE]);
    if (!$cvterm) {
      tripal_report_error('tripal', TRIPAL_ERROR,
        'Cannot create field for "%table_name.%column_name". Missing an appropriate vocabulary term',
        ['%table_name' => $table_name, '%column_name' => $column_name]);
      drupal_set_message(t('Cannot create field for "%table_name.%column_name". Missing an appropriate vocabulary term',
        [
          '%table_name' => $table_name,
          '%column_name' => $column_name,
        ]), 'error');
      continue;
    }
    $field_name = strtolower($cvterm->dbxref_id->db_id->name . '__' . preg_replace('/[^\w]/', '_', $cvterm->name));
    $field_name = substr($field_name, 0, 32);

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // If the type_id defines the content type and it's part of the
    // base table and this column is the type_id then skip it.
    if (!$type_table and $type_column and $column_name == $type_column) {
      continue;
    }

    // Skip the type ID as it will be handled by a custom field.
    if ($column_name == 'type_id') {
      continue;
    }

    // Set some defaults for the field.
    $base_info = [
      'field_name' => $field_name,
      'type' => '',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
        'sql' => [],
      ],
    ];

    // Alter the field info array depending on the column details.
    switch ($details['type']) {
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
        continue 2;
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
        $base_info['settings']['allowed_values'] = [0 => "No", 1 => "Yes"];
        break;
      case 'datetime':
        // Use the Drupal Date and Date API to create the field/widget
        $base_info['type'] = 'datetime';
        break;
    }

    // Set some defaults for biomaterial table
    if ($table_name == 'biomaterial' and $column_name == 'name') {
      $base_info['type'] = 'text';
      $base_info['settings']['max_length'] = '2048';
      $base_info['settings']['text_processing'] = 0;
    }

    // Set some default semantic web information
    if ($column_name == 'uniquename') {
      $base_info['settings']['text_processing'] = 0;
    }

    // Sometimes the boolean fields are listed as integer.  We need to
    // correct for that.
    if ($column_name == 'is_obsolete' or $column_name == 'is_analysis' or
      $column_name == 'is_relationshiptype' or $column_name == 'is_for_definition' or
      $column_name == 'is_view' or $column_name == 'is_updateable') {
      $base_info['type'] = 'list_boolean';
      $base_info['settings']['allowed_values'] = [0 => "No", 1 => "Yes"];
    }

    //
    // PUB TABLE
    //
    if ($table_name == 'pub' and (
        $column_name == 'uniquename' or $column_name == 'title' or
        $column_name == 'volumetitle')) {
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
function tripal_chado_bundle_fields_info_custom(&$info, $details, $entity_type, $bundle) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_column = $details['chado_type_column'];
  $cvterm_id = $details['chado_cvterm_id'];
  $type_value = $details['chado_type_value'];
  $base_type_id = $details['chado_base_type_id'];

  $schema = chado_get_schema($table_name);

  // Add the additional_type field to all tables with a type_id that is not used
  // as the type column nor has a $base_type_id (i.e. the content type uses a
  // prop or linker table to resolve the type).
  if (array_key_exists('type_id', $schema['fields']) and 'type_id' != $type_column and
    !$base_type_id and $table_name != 'organism') {

    $field_name = 'schema__additional_type';
    $field_type = 'schema__additional_type';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // BASE ARRAYDESIGN TABLE
  if ($table_name == 'arraydesign') {
    $field_name = 'ncit__technology_platform';
    $field_type = 'schema__additional_type';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

    $field_name = 'efo__substrate_type';
    $field_type = 'schema__additional_type';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // BASE ORGANISM_ID
  if ($table_name != 'organism' and
    (array_key_exists('organism_id', $schema['fields']) or
      array_key_exists('taxon_id', $schema['fields']))) {
    $field_name = 'obi__organism';
    $field_type = 'obi__organism';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  if ($table_name == 'biomaterial') {
    $field_name = 'biomaterial__provider_id';
    $field_type = 'local__contact';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }
  if ($table_name == 'arraydesign') {
    $field_name = 'efo__array_manufacturer';
    $field_type = 'local__contact';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // BASE ORGANISM_ID
  if ($table_name == 'cvterm') {
    $field_name = 'sio__vocabulary';
    $field_type = 'sio__vocabulary';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // BASE DBXREF
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_name = 'data__accession';
    $field_type = 'data__accession';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // FEATURE MD5CHECKSUM
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_checksum';
    $field_type = 'data__sequence_checksum';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // FEATURE RESIDUES
  if ($table_name == 'feature') {
    $field_name = 'data__sequence';
    $field_type = 'data__sequence';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // FEATURE SEQLEN
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_length';
    $field_type = 'data__sequence_length';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // PROTEIN & CDS
  if ($table_name == 'feature' and
    ($bundle->term->name == 'mRNA' or $bundle->term->name == 'transcript')) {
    $field_name = 'data__protein_sequence';
    $field_type = 'data__protein_sequence';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

    $field_name = 'so__cds';
    $field_type = 'so__cds';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // GENE TRANSCRIPTS
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table) and ($bundle->term->name == 'gene')) {
    $field_name = 'so__transcript';
    $field_type = 'so__transcript';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // ORGANISM TYPE_ID
  if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
    $field_name = 'taxrank__infraspecific_taxon';
    $field_type = 'taxrank__infraspecific_taxon';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // FEATUREMAP UNITTYPE_ID
  if ($table_name == 'featuremap') {
    $field_name = 'uo__unit';
    $field_type = 'uo__unit';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // Analysis source
  if ($table_name == 'analysis') {
    $field_name = 'local__source_data';
    $field_type = 'local__source_data';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // Add an image field to the Organism type.  This is a Drupal field and
  // not stored in Chado, but is used for backwards compatibility.
  if ($table_name == 'organism') {
    $field_name = 'data__image';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => 'image',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_sql_storage',
      ],
    ];
  }

  // Add field for viewing the phylogenetic tree.
  if ($table_name == 'phylotree') {
    $field_name = 'operation__phylotree_vis';
    $field_type = 'operation__phylotree_vis';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // Protocol.
  if ($table_name != 'protocol' and
    array_key_exists('protocol_id', $schema['fields'])) {
    $field_name = 'sep__protocol';
    $field_type = 'sep__protocol';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // Assay operator.
  if ($table_name == 'assay') {
    $field_name = 'assay__operator_id';
    $field_type = 'local__contact';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
    $field_name = 'efo__array_design';
    $field_type = 'efo__array_design';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // For the pub_id field in the base table.
  $schema = chado_get_schema($table_name);
  if (array_key_exists('pub_id', $schema['fields']) and $table_name != 'pub') {

    // Remove the schema__publication added by the
    // tripal_chado_bunde_instances_info_base function.
    unset($info['schema__publication']);

    $field_name = 'schema__publication_single';
    $field_type = 'schema__publication';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // Analysis Id
  if (array_key_exists('analysis_id', $schema['fields']) and $table_name != 'analysis') {
    $field_name = 'operation__analysis';
    $field_type = 'operation__analysis';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }
}

/**
 *
 * @param $details
 */
function tripal_chado_bundle_fields_info_linker(&$info, $details, $entity_type, $bundle) {

  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_column = $details['chado_type_column'];
  $cvterm_id = $details['chado_cvterm_id'];
  $type_value = $details['chado_type_value'];

  // CONTACTS
  $contact_table = $table_name . '_contact';
  if (chado_table_exists($contact_table)) {
    $schema = chado_get_schema($contact_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_contact';
    $field_type = 'chado_linker__contact';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // DBXREF
  $dbxref_table = $table_name . '_dbxref';
  if (chado_table_exists($dbxref_table)) {
    $field_name = 'sbo__database_cross_reference';
    $field_type = 'sbo__database_cross_reference';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // EXPRESSION
  // TODO: this should only show up on gene or mRNA bunldes, not every feature.
  //   $expression_table = $table_name . '_expression';
  //   if (chado_table_exists($expression_table)) {
  //     $field_name = 'go__gene_expression';
  //     $field_type = 'go__gene_expression';
  //     $info[$field_name] = array(
  //       'field_name' => $field_name,
  //       'type' => $field_type,
  //       'cardinality' => FIELD_CARDINALITY_UNLIMITED,
  //       'locked' => FALSE,
  //       'storage' => array(
  //         'type' => 'field_chado_storage',
  //       ),
  //     );
  //   }

  // FEATURELOC
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_coordinates';
    $field_type = 'data__sequence_coordinates';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // FEATUREPOS
  if ($table_name == 'feature') {
    $field_name = 'ogi__location_on_map';
    $field_type = 'ogi__location_on_map';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  //   // GENOTYPE
  //   $genotype_table = $table_name . '_genotype';
  //   if (chado_table_exists($genotype_table)) {
  //     $field_name = 'so__genotype';
  //     $field_type = 'so__genotype';
  //     $info[$field_name] = array(
  //       'field_name' => $field_name,
  //       'type' => $field_type,
  //       'cardinality' => FIELD_CARDINALITY_UNLIMITED,
  //       'locked' => FALSE,
  //       'storage' => array(
  //         'type' => 'field_chado_storage',
  //       ),
  //     );
  //   }

  //   // PHENOTYPE
  //   $phenotype_table = $table_name . '_phenotype';
  //   if (chado_table_exists($phenotype_table)) {
  //     $field_name = 'sbo__phenotype';
  //     $field_type = 'sbo__phenotype';
  //     $info[$field_name] = array(
  //       'field_name' => $field_name,
  //       'type' => $field_type,
  //       'cardinality' => FIELD_CARDINALITY_UNLIMITED,
  //       'locked' => FALSE,
  //       'storage' => array(
  //         'type' => 'field_chado_storage',
  //       ),
  //     );
  //   }

  // PROPERTIES
  $prop_table = $table_name . 'prop';
  if (chado_table_exists($prop_table)) {

    $props = tripal_chado_bundle_get_properties($table_name, $prop_table, $type_table, $type_column, $cvterm_id, $type_value);
    foreach ($props as $term) {

      $field_name = strtolower(preg_replace('/[^\w]/', '_', $term->dbxref_id->db_id->name . '__' . $term->name));

      // The field name can only be 32 chars, but if our name is longer we need
      // to add some random chars to ensure we don't have naming conflicts
      // with other terms (e.g. mitochondrial_genetic_code and
      // mitochondrial_genetic_code_name)
      if (strlen($field_name) >= 32) {
        $field_name = substr($field_name, 0, 20) . '_' . $term->cvterm_id;
      }
      $field_type = 'chado_linker__prop';

      // Don't try to add a property that uses the same term as another field.
      if (array_key_exists($field_name, $info)) {
        tripal_report_error('chado_fields', TRIPAL_WARNING,
          'A field of type !type already exists, yet a property wants to use the same term. The property cannot be added.',
          ['!type' => $field_name],
          ['drupal_set_message' => TRUE]);
        continue;
      }
      $info[$field_name] = [
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => 1,
        'locked' => FALSE,
        'storage' => [
          'type' => 'field_chado_storage',
        ],
      ];
    }
  }

  // CVTERMS
  $term_table = $table_name . '_cvterm';
  if (chado_table_exists($term_table)) {
    $schema = chado_get_schema($term_table);
    $pkey = $schema['primary key'][0];
    $lkey = key($schema['foreign keys'][$table_name]['columns']);
    $rkey = $schema['foreign keys'][$table_name]['columns'][$lkey];

    $field_name = 'sio__annotation';
    $field_type = 'sio__annotation';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // PUBLICATIONS
  $pub_table = $table_name . '_pub';
  if (chado_table_exists($pub_table)) {
    $field_name = 'schema__publication';
    $field_type = 'schema__publication';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // PUBLICATIONS (in reverse)
  // We want to be able to show all of the content that a publication links
  // to. The sio__references field does that.
  if ($table_name == 'pub') {
    $field_name = 'sio__references';
    $field_type = 'sio__references';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // RELATIONSHIPS
  // If the linker table does not exists then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table)) {
    $field_name = 'sbo__relationship';
    $field_type = 'sbo__relationship';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  // SYNONYMS
  $syn_table = $table_name . '_synonym';
  if (chado_table_exists($syn_table)) {
    $field_name = 'schema__alternate_name';
    $field_type = 'schema__alternate_name';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
      'settings' => [
      ],
    ];
  }
}

/**
 * Impelments hook_create_tripalfield_instance().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_bundle_instances_info($entity_type, $bundle) {

  $chado_bundle = db_select('chado_bundle', 'cb')
    ->fields('cb')
    ->condition('bundle_id', $bundle->id)
    ->execute()
    ->fetchObject();

  $details = [
    'chado_cvterm_id' => $chado_bundle->type_id,
    'chado_table' => $chado_bundle->data_table,
    'chado_type_table' => $chado_bundle->type_linker_table,
    'chado_type_id' => $chado_bundle->type_id,
    'chado_type_column' => $chado_bundle->type_column,
    'chado_type_value' => $chado_bundle->type_value,
    'chado_base_type_id' => $chado_bundle->base_type_id,
  ];

  $info = [];
  tripal_chado_bundle_instances_info_base($info, $entity_type, $bundle, $details);
  tripal_chado_bundle_instances_info_custom($info, $entity_type, $bundle, $details);
  tripal_chado_bundle_instances_info_linker($info, $entity_type, $bundle, $details);

  return $info;

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
function tripal_chado_bundle_instances_info_base(&$info, $entity_type, $bundle, $details) {
  $fields = [];

  // Get Chado information
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_column = $details['chado_type_column'];
  $cvterm_id = $details['chado_cvterm_id'];
  $type_value = $details['chado_type_value'];

  // Iterate through the columns of the table and see if fields have been
  // created for each one. If not, then create them.
  $schema = chado_get_schema($table_name);
  if (!$schema) {
    return;
  }

  $pkey = $schema['primary key'][0];

  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {

    // Skip the source columns  in the analysis table. We have a custom
    // field for those columns
    if ($table_name == 'analysis' and ($column_name == 'sourceuri' or
        $column_name == 'sourceversion' or $column_name == 'sourcename')) {
      continue;
    }

    // Skip the infraspecific type_id and name from the organism table as we
    // have a special field for those.
    if ($table_name == 'organism' and ($column_name == 'type_id' or
        $column_name == 'infraspecific_name')) {
      continue;
    }

    // Skip the cvterm.is_relationshptype.
    if ($table_name == 'cvterm' and $column_name == 'is_relationshiptype') {
      continue;
    }

    // The biosourceprovider_id and taxon_id are handled by custom fields.
    if ($table_name == 'biomaterial' and (
        $column_name == 'biosourceprovider_id' or $column_name == 'taxon_id')) {
      continue;
    }

    // Don't create base fields for the primary key and the type_id field.
    if ($column_name == $pkey or $column_name == $type_column) {
      continue;
    }

    $cvterm = chado_get_semweb_term($table_name, $column_name, ['return_object' => TRUE]);
    if (!$cvterm) {
      // We already provided an error when creating the base field.  So
      // don't create another one here.
      continue;
    }
    $field_name = strtolower($cvterm->dbxref_id->db_id->name . '__' . preg_replace('/[^\w]/', '_', $cvterm->name));
    $field_name = substr($field_name, 0, 32);

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // If the type_id defines the content type and it's part of the
    // base table and this column is the type_id then skip it.
    if (!$type_table and $type_column and $column_name == $type_column) {
      continue;
    }

    // Skip the type ID as it will be handled by a custom field.
    if ($column_name == 'type_id') {
      continue;
    }

    $base_info = [
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
      'description' => '',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'term_vocabulary' => $cvterm->dbxref_id->db_id->name,
        'term_name' => $cvterm->name,
        'term_accession' => $cvterm->dbxref_id->accession,
        'chado_table' => $table_name,
        'chado_column' => $column_name,
        'base_table' => $table_name,
      ],
      'widget' => [
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'region' => 'Left',
          'settings' => [],
        ],
      ],
    ];

    // Determine if the field is required.
    if (array_key_exists('not null', $details) and $details['not null'] === TRUE) {
      $base_info['required'] = TRUE;
    }

    // Alter the field info array depending on the column details.
    switch ($details['type']) {
      case 'char':
        $base_info['widget']['type'] = 'text_textfield';
        break;
      case 'varchar':
        $base_info['widget']['type'] = 'text_textfield';
        break;
      case 'text':
        $base_info['display']['default']['label'] = 'above';
        $base_info['widget']['type'] = 'text_textarea';
        $base_info['settings']['text_processing'] = '1';
        $base_info['settings']['format'] = 'full_html';
        break;
      case 'blob':
        // not sure how to support a blob field.
        continue 2;
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
        $base_info['widget']['settings']['year_range'] = '-10:+10';

        break;
    }

    // Set some default semantic web information
    if ($column_name == 'uniquename') {
      $base_info['label'] = 'Identifier';
      $base_info['widget']['type'] = 'text_textfield';
      if ($details['type'] == 'text') {
        unset($base_info['display']['default']['label']);
        unset($base_info['settings']['text_processing']);
        unset($base_info['settings']['format']);
      }
    }
    if ($base_info['label'] == 'Timeaccessioned') {
      $base_info['label'] = 'Time Accessioned';
      $base_info['description'] = 'The time that this record was first added to the database.';
    }
    if ($base_info['label'] == 'Timelastmodified') {
      $base_info['label'] = 'Time Last Modified';
      $base_info['description'] = 'The time that this record was last modified. The default is the current time.';
    }

    // Sometimes the boolean fields are listed as integer.  We need to
    // correct for that.
    if ($column_name == 'is_obsolete' or $column_name == 'is_analysis' or
      $column_name == 'is_relationshiptype' or $column_name == 'is_for_definition' or
      $column_name == 'is_view' or $column_name == 'is_updateable') {
      $base_info['widget']['type'] = 'options_onoff';
      $base_info['required'] = FALSE;
    }
    //
    // ORGANISM TABLE
    //
    if ($table_name == 'organism' and $column_name == 'comment') {
      $base_info['label'] = 'Description';
    }
    //
    // BIOMATERIAL TABLE
    //
    if ($table_name == 'biomaterial' and $column_name == 'name') {
      $base_info['widget'] = [];
      $base_info['widget']['type'] = 'text_textfield';
      unset($base_info['display']['default']['label']);
      unset($base_info['settings']['text_processing']);
      unset($base_info['settings']['format']);
    }
    //
    // FEATUREMAP TABLE
    //
    if ($table_name == 'featuremap' and $column_name == 'name') {
      $base_info['required'] = TRUE;
    }
    //
    // PUB TABLE
    //
    if ($table_name == 'pub') {
      if ($column_name == 'title' or $column_name == 'type_id') {
        $base_info['required'] = TRUE;
      }
      if ($column_name == 'uniquename' or $column_name == 'title' or $column_name == 'volumetitle') {
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
      }
      if ($column_name == 'uniquename') {
        $base_info['label'] = 'Unique Local Identifier';
        $base_info['required'] = TRUE;
        $base_info['description'] = 'This publication is housed in Chado whic requires a unique identifer (or name) be provided for every publication. This identifier need not be shown to end-users but it is required. Each site must decide on a format for this unique name.';
      }
      if ($column_name == 'title') {
        $base_info['description'] = 'The title of the published work.';
      }
      if ($column_name == 'series_name') {
        $base_info['description'] = 'The name media that produces a series of publications (e.g. journal, conference proceedings, etc.).';
      }
      if ($column_name == 'issue') {
        $base_info['description'] = 'The issue of the series (e.g. journal) where the publication was printed.';
      }
      if ($column_name == 'volume') {
        $base_info['description'] = 'The volume of the series (e.g. journal) where the publication was printed.';
      }
      if ($column_name == 'pyear') {
        $base_info['label'] = 'Publication Year';
        $base_info['required'] = TRUE;
      }
      if ($column_name == 'pages') {
        $base_info['label'] = 'Page Numbers';
      }
      if ($column_name == 'pubplace') {
        $base_info['label'] = 'Publication Location';
      }
      if ($column_name == 'volumetitle') {
        $base_info['label'] = 'Volumn Title';
        $base_info['label'] = 'The title of the volume (if applicable).';
      }
      if ($column_name == 'miniref') {
        $base_info['label'] = 'Mini Local identifier';
        $base_info['description'] = 'Some sites use small identifiers for each publication. if your site uses this please provide the proper miniref for this publication.';
      }
    }

    //
    // ANALYSIS TABLE
    //
    if ($table_name == 'analysis' and $column_name == 'name') {
      $base_info['required'] = TRUE;
    }
    if ($table_name == 'analysis' and $column_name == 'program') {
      $base_info['description'] = 'The program name (e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package then provide a very brief description of the pipeline, workflow or method.';
      $base_info['label'] = 'Program, Pipeline, Workflow or Method Name';
    }
    if ($table_name == 'analysis' and $column_name == 'algorithm') {
      $base_info['label'] = 'Algorithm';
      $base_info['description'] = 'The name of the algorithm used to produce the dataset if different from the program.';
    }
    if ($table_name == 'analysis' and $column_name == 'programversion') {
      $base_info['label'] = 'Program Version';
      $base_info['description'] = 'The version of the program used to perform this analysis. (e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter "n/a" if no version is available or applicable.';
    }
    if ($table_name == 'analysis' and $column_name == 'timeexecuted') {
      $base_info['label'] = 'Date Performed';
      $base_info['description'] = 'The date and time when the analysis was performed.';
    }

    if ($table_name == 'analysis' and ($column_name == 'sourceuri' or
        $column_name == 'sourceversion' or $column_name == 'sourcename')) {
      // Skip the source columns  in the analysis table. We have a custom
      // field for those columns
      continue;
    }
    //
    // PROJECT TABLE
    //
    if ($table_name == 'project' and $column_name == 'description') {
      $base_info['label'] = 'Short Description';
    }

    //
    // CONTACT TABLE
    //
    if ($table_name == 'contact' and $column_name == 'description') {
      $base_info['label'] = 'Short Description';
    }
    //
    // PROTOCOL TABLE
    //
    if ($table_name == 'protocol') {
      if ($column_name == 'protocoldescription') {
        $base_info['label'] = 'Protocol Description';
      }
      if ($column_name == 'uri') {
        $base_info['label'] = 'URI';
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
      }
      if ($column_name == 'name') {
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
      }
      if ($column_name == 'hardwaredescription') {
        $base_info['label'] = 'Instrument Description';
        $base_info['description'] = 'The description of instruments used for this protocol';
      }
      if ($column_name == 'softwaredescription') {
        $base_info['label'] = 'Software Description';
        $base_info['description'] = 'The description of software used for this protocol';
      }
      if ($column_name == 'pub_id') {
        $base_info['label'] = 'Publication';
      }
    }
    //
    // ASSAY TABLE
    //
    if ($table_name == 'assay') {
      if ($column_name == 'name') {
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
        $base_info['required'] = TRUE;
        $base_info['description'] = 'A unique name for this assay..';
      }
      if ($column_name == 'protcol_id') {
        $base_info['label'] = 'Protocol';
      }
      if ($column_name == 'arraybatchidentifier') {
        $base_info['label'] = 'Array Batch Identifier';
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
        $base_info['description'] = 'A unique identifier for the array batch.';
      }
      if ($column_name == 'operator_id') {
        $base_info['label'] = 'Operator';
        $base_info['description'] = 'The individual who performed the assay.';
      }
      if ($column_name == 'arrayidentifier') {
        $base_info['label'] = 'Array Identifier';
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
        $base_info['description'] = 'A unique alternate identifier for the array.';
      }
      if ($column_name == 'arraydesign_id') {
        $base_info['label'] = 'Array Design';
      }
      if ($column_name == 'assaydate') {
        $base_info['label'] = 'Assay Date';
        $base_info['description'] = 'The date the assay was performed';
      }
    }
    //
    // ARRAYDESIGN TABLE
    //
    if ($table_name == 'arraydesign') {
      if ($column_name == 'name' or $column_name == 'version' or
        $column_name == 'array_dimensions' or $column_name == 'element_dimensions') {
        $base_info['widget']['type'] = 'text_textfield';
        $base_info['settings']['text_processing'] = '0';
      }
      if ($column_name == 'platformtype_id') {
        $base_info['label'] = 'Platform type';
      }
      if ($column_name == 'substratetype_id') {
        $base_info['label'] = 'Substrate Type';
      }
      if ($column_name == 'manufacturer_id') {
        $base_info['label'] = 'Manufacturer';
      }

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
function tripal_chado_bundle_instances_info_custom(&$info, $entity_type, $bundle, $details) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_column = $details['chado_type_column'];
  $cvterm_id = $details['chado_cvterm_id'];
  $type_value = $details['chado_type_value'];
  $base_type_id = $details['chado_base_type_id'];
  $schema = chado_get_schema($table_name);

  // Add the additional_type field to all tables with a type_id that is not used
  // as the type column nor has a $base_type_id (i.e. the content type uses a
  // prop or linker table to resolve the type).
  if (array_key_exists('type_id', $schema['fields']) and 'type_id' != $type_column and
    !$base_type_id and $table_name != 'organism') {
    $field_name = 'schema__additional_type';
    $is_required = FALSE;
    if (array_key_exists('not null', $schema['fields']['type_id']) and
      $schema['fields']['type_id']['not null']) {
      $is_required = TRUE;
    }
    $label = ucwords(preg_replace('/_/', ' ', $table_name)) . ' Type';
    $default_vocab = '';
    $parent_term = '';
    switch ($table_name) {
      case 'pub':
        $default_vocab = 'tripal_pub';
        $label = 'Publication Type';
        $parent_term = 'TPUB:0000015';
        $description = 'Select the type.';
        break;
      case 'contact':
        $default_vocab = 'tripal_contact';
        $parent_term = 'TCONTACT:0000001';
        $description = 'Select the type.';
        break;
      default:
        $description = t('Enter the name of the term that specifies the type. ' .
          'The type must be the name of a term in a controlled vocabulary and ' .
          'the controlled vocabulary should already be loaded into this site.');
    }
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => $label,
      'description' => $description,
      'required' => $is_required,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'type_id',
        'base_table' => $table_name,
        'vocabulary' => $default_vocab,
        'parent_term' => $parent_term,
        'term_vocabulary' => 'schema',
        'term_name' => 'additionalType',
        'term_accession' => 'additionalType',
      ],
      'widget' => [
        'type' => 'schema__additional_type_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'schema__additional_type_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  if ($table_name == 'arraydesign') {
    $field_name = 'ncit__technology_platform';
    $default_vocab = '';
    $parent_term = '';
    $description = 'Select the platform type.';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Platform Type',
      'description' => $description,
      'required' => TRUE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'platformtype_id',
        'base_table' => $table_name,
        'vocabulary' => $default_vocab,
        'parent_term' => $parent_term,
        'term_vocabulary' => 'NCIT',
        'term_name' => 'Technology Platform',
        'term_accession' => 'C45378',
      ],
      'widget' => [
        'type' => 'schema__additional_type_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'schema__additional_type_formatter',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'efo__substrate_type';
    $default_vocab = '';
    $parent_term = '';
    $description = 'Select the substrate type.';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Substrate Type',
      'description' => $description,
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'substratetype_id',
        'base_table' => $table_name,
        'vocabulary' => $default_vocab,
        'parent_term' => $parent_term,
        'term_vocabulary' => 'EFO',
        'term_name' => 'substrate type',
        'term_accession' => '0005522',
      ],
      'widget' => [
        'type' => 'schema__additional_type_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'schema__additional_type_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // BASE ORGANISM_ID
  if ($table_name != 'organism' and
    (array_key_exists('organism_id', $schema['fields']) or
      array_key_exists('taxon_id', $schema['fields']))) {

    $field_name = 'obi__organism';
    $is_required = FALSE;
    $table_column = 'organism_id';
    if ($table_name == 'biomaterial') {
      $is_required = FALSE;
      $table_column = 'taxon_id';
    }
    elseif (array_key_exists('not null', $schema['fields']['organism_id']) and
      $schema['fields']['organism_id']['not null']) {
      $is_required = TRUE;
    }
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Organism',
      'description' => 'The full scientific name for a species..',
      'required' => $is_required,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => $table_column,
        'base_table' => $table_name,
        'term_accession' => '0100026',
        'term_vocabulary' => 'OBI',
        'term_name' => 'organism',
      ],
      'widget' => [
        'type' => 'obi__organism_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'obi__organism_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  if ($table_name == 'biomaterial') {
    $field_name = 'biomaterial__provider_id';
    $field_type = 'local__contact';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Contact',
      'description' => 'An indvidual or organization that serves as a contact for this record.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'biomaterial',
        'chado_column' => 'biosourceprovider_id',
        'base_table' => 'biomaterial',
        'term_accession' => 'contact',
        'term_vocabulary' => 'local',
        'term_name' => 'contact',
      ],
      'widget' => [
        'type' => 'local__contact_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'local__contact_formatter',
          'settings' => [],
        ],
      ],
    ];
  }
  if ($table_name == 'arraydesign') {
    $field_name = 'efo__array_manufacturer';
    $field_type = 'local__contact';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Manufacturer',
      'description' => 'A manufacturer\'s contact details',
      'required' => TRUE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'arraydesign',
        'chado_column' => 'manufacturer_id',
        'base_table' => 'arraydesign',
        'term_vocabulary' => 'EFO',
        'term_name' => 'aarray manufacturer',
        'term_accession' => '0001728',
      ],
      'widget' => [
        'type' => 'local__contact_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'local__contact_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // BASE CVTERM
  if ($table_name == 'cvterm') {
    $field_name = 'sio__vocabulary';
    $is_required = TRUE;
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Vocabulary',
      'description' => 'A controlled vocabulary.',
      'required' => $is_required,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'cv_id',
        'base_table' => $table_name,
        'term_accession' => '001080',
        'term_vocabulary' => 'SIO',
        'term_name' => 'vocabulary',
      ],
      'widget' => [
        'type' => 'sio__vocabulary_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'sio__vocabulary_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // BASE DBXREF
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_name = 'data__accession';
    $required = FALSE;
    if (array_key_exists('not null', $schema['fields']['dbxref_id']) and
      $schema['fields']['dbxref_id']['not null']) {
      $required = TRUE;
    }
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Accession',
      'description' => 'This field specifies the unique stable accession (ID) for
        this record. It requires that this site have a database entry.',
      'required' => $required,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'dbxref_id',
        'base_table' => $table_name,
        'term_accession' => '2091',
        'term_vocabulary' => 'data',
        'term_name' => 'Accession',
      ],
      'widget' => [
        'type' => 'data__accession_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'data__accession_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // FEATURE MD5CHECKSUM
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_checksum';
    $info[$field_name] = [
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
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'base_table' => $table_name,
        'term_accession' => '2190',
        'term_vocabulary' => 'data',
        'term_name' => 'Sequence checksum',
      ],
      'widget' => [
        'type' => 'data__sequence_checksum_widget',
        'settings' => [
          'display_label' => 1,
          'md5_fieldname' => 'feature__md5checksum',
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'data__sequence_checksum_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // FEATURE RESIDUES
  if ($table_name == 'feature') {
    $field_name = 'data__sequence';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence',
      'description' => 'One or more molecular sequences, possibly with associated annotation.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $table_name,
        'chado_column' => 'residues',
        'base_table' => $table_name,
        'term_accession' => '2044',
        'term_vocabulary' => 'data',
        'term_name' => 'Sequence',
      ],
      'widget' => [
        'type' => 'data__sequence_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'above',
          'type' => 'data__sequence_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // FEATURE SEQLEN
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_length';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence Length',
      'description' => 'The number of residues in the raw sequence.  This length
        is only for the assigned raw sequence and does not represent the length of any
        sequences derived from alignments. If this value is zero but aligned sequences
        are present then this record has no official assigned sequence.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'seqlen',
        'base_table' => $table_name,
        'term_accession' => '1249',
        'term_vocabulary' => 'data',
        'term_name' => 'delete	Sequence length',
      ],
      'widget' => [
        'type' => 'data__sequence_length_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'data__sequence_length_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // PROTEIN & CDS SEQUENCES.
  if ($table_name == 'feature' and
    ($bundle->term->name == 'mRNA' or $bundle->term->name == 'transcript')) {
    $field_name = 'data__protein_sequence';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Protein Sequence',
      'description' => 'Protein sequences.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'feature',
        'chado_column' => 'residues',
        'base_table' => 'feature',
        'term_accession' => '2976',
        'term_vocabulary' => 'data',
        'term_name' => 'Protein sequence',
      ],
      'widget' => [
        'type' => 'data__protein_sequence_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'above',
          'type' => 'data__protein_sequence_formatter',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'so__cds';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Coding Sequence (CDS)',
      'description' => 'Coding sequences.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'featureprop',
        'chado_column' => 'value',
        'base_table' => 'feature',
        'term_vocabulary' => 'SO',
        'term_name' => 'CDS',
        'term_accession' => '0000316',
      ],
      'widget' => [
        'type' => 'so__cds_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'above',
          'type' => 'so__cds_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // GENE TRANSCRIPTS
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table) and ($bundle->term->name == 'gene')) {
    $field_name = 'so__transcript';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Transcripts',
      'description' => 'Transcripts that are part of this gene.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $rel_table,
        'chado_column' => '',
        'base_table' => $table_name,
        'term_vocabulary' => 'SO',
        'term_name' => 'transcript',
        'term_accession' => '0000673',
      ],
      'widget' => [
        'type' => 'so__transcript_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'above',
          'type' => 'so__transcript_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // ORGANISM TYPE_ID
  if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
    $field_name = 'taxrank__infraspecific_taxon';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Infraspecific Taxon',
      'description' => 'The Infraspecific Taxon.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => 'organism',
        'chado_column' => 'type_id',
        'base_table' => 'organism',
        'term_vocabulary' => 'TAXRANK',
        'term_name' => 'infraspecific_taxon',
        'term_accession' => '0000046',
      ],
      'widget' => [
        'type' => 'taxrank__infraspecific_taxon_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'taxrank__infraspecific_taxon_formatter',
          'settings' => [],
        ],
      ],
    ];
  }


  // FEATURE MAP UNITS
  if ($table_name == 'featuremap') {
    $field_name = 'uo__unit';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Units',
      'description' => 'The map\'s unit type.',
      'required' => TRUE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'unittype_id',
        'base_table' => $table_name,
        'term_accession' => '0000000',
        'term_vocabulary' => 'UO',
        'term_name' => 'unit',
      ],
      'widget' => [
        'type' => 'uo__unit_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'uo__unit_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // the analysis source.
  if ($table_name == 'analysis') {
    $field_name = 'local__source_data';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Data Source',
      'description' => 'The source where data was obtained for this analysis.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'analysis_id',
        'base_table' => $table_name,
        'term_accession' => 'source_data',
        'term_vocabulary' => 'local',
        'term_name' => 'source_data',
      ],
      'widget' => [
        'type' => 'local__source_data_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'local__source_data_formatter',
          'settings' => [],
        ],
      ],
    ];

  }
  // Add an image field to the Organism type.  This is a Drupal field and
  // not stored in Chado, but is used for backwards compatibility.
  if ($table_name == 'organism') {
    $field_name = 'data__image';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Organism Image',
      'description' => 'An image for the organism',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'data',
        'term_name' => 'Image',
        'term_accession' => '2968',
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
        ],
      ],
    ];
  }

  // Phylotree Viewer
  if ($table_name == 'phylotree') {
    $field_name = 'operation__phylotree_vis';
    $schema = chado_get_schema('phylotree');
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Tree View',
      'description' => 'Rendering of a phylogenetic tree.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'phylotree',
        'chado_column' => 'type_id',
        'base_table' => 'phylotree',
        'term_accession' => '0567',
        'term_vocabulary' => 'operation',
        'term_name' => 'Phylogenetic tree visualisation',
      ],
      'widget' => [
        'type' => 'operation__phylotree_vis_widget',
        'settings' => [
          'display_label' => 0,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'operation__phylotree_vis_formatter',
          'settings' => [],
        ],
      ],
    ];
  }


  // PROTOCOL FIELD
  if ($table_name != 'protocol' and
    (array_key_exists('protocol_id', $schema['fields']))) {

    $field_name = 'sep__protocol';
    $is_required = FALSE;
    $table_column = 'protocol_id';
    if (array_key_exists('not null', $schema['fields']['protocol_id']) and
      $schema['fields']['protocol_id']['not null']) {
      $is_required = TRUE;
    }
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Protocol',
      'description' => 'The parameterizable description of a process',
      'required' => $is_required,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => $table_column,
        'base_table' => $table_name,
        'term_accession' => '00101',
        'term_vocabulary' => 'sep',
        'term_name' => 'Protocol',
      ],
      'widget' => [
        'type' => 'sep__protocol_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'sep__protocol_formatter',
          'settings' => [],
        ],
      ],
    ];
  }
  // pub_id field in table.
  $schema = chado_get_schema($table_name);
  if (array_key_exists('pub_id', $schema['fields']) and $table_name != 'pub') {

    // Remove the schema__publication added by the
    // tripal_chado_bunde_instances_info_base function.
    unset($info['schema__publication']);

    $field_name = 'schema__publication_single';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Publication',
      'description' => 'This record has been referenced or is sourced from these publications.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'pub_id',
        'base_table' => $table_name,
        'term_accession' => 'publication',
        'term_vocabulary' => 'schema',
        'term_name' => 'publication',
      ],
      'widget' => [
        'type' => 'schema__publication_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'schema__publication_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  if ($table_name == 'assay') {
    // Remove the ncit__operator added by the
    // tripal_chado_bunde_instances_info_base function.
    unset($info['ncit__operator']);

    $field_name = 'assay__operator_id';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Operator',
      'description' => 'The individual responsible for performing the assay.',
      'required' => TRUE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => 'assay',
        'chado_column' => 'operator_id',
        'base_table' => 'assay',
        'term_vocabulary' => 'NCIT',
        'term_name' => 'Operator',
        'term_accession' => 'C48036',

      ],
      'widget' => [
        'type' => 'local__contact_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'local__contact_formatter',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'efo__array_design';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Array design',
      'description' => 'An instrument design which describes the design of the array.',
      'required' => TRUE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => 'assay',
        'chado_column' => 'arraydesign_id',
        'base_table' => 'assay',
        'term_vocabulary' => 'EFO',
        'term_name' => 'array design',
        'term_accession' => '0000269',

      ],
      'widget' => [
        'type' => 'efo__array_design_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'efo__array_design_formatter',
          'settings' => [],
        ],
      ],
    ];
  }
  // Analysis Id
  if (array_key_exists('analysis_id', $schema['fields']) and $table_name != 'analysis') {
    $field_name = 'operation__analysis';
    $is_required = FALSE;
    if (array_key_exists('not null', $schema['fields']['analysis_id']) and
      $schema['fields']['analysis_id']['not null']) {
      $is_required = TRUE;
    }
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Analysis',
      'description' => 'Application of analytical methods to existing data of a specific type.',
      'required' => $is_required,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $table_name,
        'chado_column' => 'analysis_id',
        'base_table' => $table_name,
        'term_vocabulary' => 'operation',
        'term_name' => 'Analysis',
        'term_accession' => '2945',

      ],
      'widget' => [
        'type' => 'operation__analysis_widget',
        'settings' => [
          'display_label' => 0,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'operation__analysis_formatter',
          'settings' => [],
        ],
      ],
    ];
  }
}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 * @param unknown $details
 */
function tripal_chado_bundle_instances_info_linker(&$info, $entity_type, $bundle, $details) {

  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_column = $details['chado_type_column'];
  $cvterm_id = $details['chado_cvterm_id'];
  $type_value = $details['chado_type_value'];

  // CONTACTS
  $contact_table = $table_name . '_contact';
  if (chado_table_exists($contact_table)) {
    $field_name = $table_name . '_contact';
    $info[$field_name] = $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Contact',
      'description' => 'Associates an indviddual or organization with this record',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $contact_table,
        'base_table' => $table_name,
        'chado_column' => 'contact_id',
        'term_accession' => 'contact',
        'term_vocabulary' => 'local',
        'term_name' => 'contact',
      ],
      'widget' => [
        'type' => 'chado_linker__contact_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'chado_linker__contact_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // DBXREF
  $dbxref_table = $table_name . '_dbxref';
  if (chado_table_exists($dbxref_table)) {
    $field_name = 'sbo__database_cross_reference';
    $schema = chado_get_schema($dbxref_table);
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Cross Reference',
      'description' => 'The IDs where this record may be available in other external online databases.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $dbxref_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'term_vocabulary' => 'SBO',
        'term_name' => 'Database Cross Reference',
        'term_accession' => '0000554',
      ],
      'widget' => [
        'type' => 'sbo__database_cross_reference_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'sbo__database_cross_reference_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // EXPRESSION
  // TODO: this should only show up on gene or mRNA and the GO must be
  // laoded or this field will crash things.
  //   $expression_table = $table_name . '_expression';
  //   if (chado_table_exists($expression_table)) {
  //     $field_name = 'go__gene_expression';
  //     $schema = chado_get_schema($expression_table);
  //     $pkey = $schema['primary key'][0];
  //     $info[$field_name] = array(
  //       'field_name' => $field_name,
  //       'entity_type' => $entity_type,
  //       'bundle' => $bundle->name,
  //       'label' => 'Gene expression',
  //       'description' => 'Information about the expression of this record.',
  //       'required' => FALSE,
  //       'settings' => array(
  //         'auto_attach' => FALSE,
  //         'chado_table' => $expression_table,
  //         'chado_column' => $pkey,
  //         'base_table' => $table_name,
  //         'term_accession' => '',
  //         'term_vocabulary' => '',
  //         'term_name' => '',
  //       ),
  //       'widget' => array(
  //         'type' => 'go__gene_expression_widget',
  //         'settings' => array(
  //           'display_label' => 1,
  //         ),
  //       ),
  //       'display' => array(
  //         'default' => array(
  //           'label' => 'above',
  //           'type' => 'go__gene_expression_formatter',
  //           'settings' => array(),
  //         ),
  //       ),
  //     );
  //   }

  // FEATURELOC
  if ($table_name == 'feature') {
    $field_name = 'data__sequence_coordinates';
    $schema = chado_get_schema('featureloc');
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Sequence Coordinates',
      'description' => 'The locations on other genomic sequences where this
        record has been aligned.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'featureloc',
        'chado_column' => $pkey,
        'base_table' => 'feature',
        'term_accession' => '2012',
        'term_vocabulary' => 'data',
        'term_name' => 'Sequence coordinates',
      ],
      'widget' => [
        'type' => 'data__sequence_coordinates_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'data__sequence_coordinates_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // FEATUREPOS
  if ($table_name == 'feature') {
    $field_name = 'ogi__location_on_map';
    $schema = chado_get_schema('featurepos');
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Location on Map',
      'description' => 'The positions on a genetic map.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => 'featurepos',
        'chado_column' => $pkey,
        'base_table' => 'feature',
        'term_accession' => '0000021',
        'term_vocabulary' => 'OGI',
        'term_name' => 'location on map',
      ],
      'widget' => [
        'type' => 'ogi__location_on_map_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'ogi__location_on_map_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  //   // GENOTYPE
  //   $genotype_table = $table_name . '_genotype';
  //   if (chado_table_exists($genotype_table)) {
  //     $field_name = 'so__genotype';
  //     $schema = chado_get_schema($genotype_table);
  //     $pkey = $schema['primary key'][0];
  //     $info[$field_name] = array(
  //       'field_name' => $field_name,
  //       'entity_type' => $entity_type,
  //       'bundle' => $bundle->name,
  //       'label' => 'Genotype',
  //       'description' => 'The genotypes associated with this record.',
  //       'required' => FALSE,
  //       'settings' => array(
  //         'auto_attach' => FALSE,
  //         'chado_table' => $genotype_table,
  //         'chado_column' => $pkey,
  //         'base_table' => $table_name,
  //         'term_accession' => '',
  //         'term_vocabulary' => '',
  //         'term_name' => '',
  //       ),
  //       'widget' => array(
  //         'type' => 'so__genotype_widget',
  //         'settings' => array(
  //           'display_label' => 1,
  //         ),
  //       ),
  //       'display' => array(
  //         'default' => array(
  //           'label' => 'hidden',
  //           'type' => 'so__genotype_formatter',
  //           'settings' => array(),
  //         ),
  //       ),
  //     );
  //   }

  //   // PHENOTYPE
  //   $phenotype_table = $table_name . '_phenotype';
  //   if (chado_table_exists($phenotype_table)) {
  //     $field_name = 'sbo__phenotype';
  //     $schema = chado_get_schema($phenotype_table);
  //     $pkey = $schema['primary key'][0];
  //     $info[$field_name] = array(
  //       'field_name' => $field_name,
  //       'entity_type' => $entity_type,
  //       'bundle' => $bundle->name,
  //       'label' => 'Phenotype',
  //       'description' => 'The phenotypes associated with this record.',
  //       'required' => FALSE,
  //       'settings' => array(
  //         'auto_attach' => FALSE,
  //         'chado_table' => $phenotype_table,
  //         'chado_column' => $pkey,
  //         'base_table' => $table_name,
  //         'term_accession' => '',
  //         'term_vocabulary' => '',
  //         'term_name' => '',
  //       ),
  //       'widget' => array(
  //         'type' => 'sbo__phenotype_widget',
  //         'settings' => array(
  //           'display_label' => 1,
  //         ),
  //       ),
  //       'display' => array(
  //         'default' => array(
  //           'label' => 'hidden',
  //           'type' => 'sbo__phenotype_formatter',
  //           'settings' => array(),
  //         ),
  //       ),
  //     );
  //   }

  // PROPERTIES
  $prop_table = $table_name . 'prop';
  if (chado_table_exists($prop_table)) {

    $schema = chado_get_schema($prop_table);
    $pkey = $schema['primary key'][0];

    $props = tripal_chado_bundle_get_properties($table_name, $prop_table, $type_table, $type_column, $cvterm_id, $type_value);
    foreach ($props as $term) {

      $field_name = strtolower(preg_replace('/[^\w]/', '_', $term->dbxref_id->db_id->name . '__' . $term->name));

      // The field name can only be 32 chars, but if our name is longer we need
      // to add some random chars to ensure we don't have naming conflicts
      // with other terms (e.g. mitochondrial_genetic_code and
      // mitochondrial_genetic_code_name)
      if (strlen($field_name) >= 32) {
        $field_name = substr($field_name, 0, 20) . '_' . $term->cvterm_id;
      }
      $info[$field_name] = [
        'field_name' => $field_name,
        'entity_type' => $entity_type,
        'bundle' => $bundle->name,
        'label' => ucwords(preg_replace('/_/', ' ', $term->name)),
        'description' => $term->definition,
        'required' => FALSE,
        'settings' => [
          'auto_attach' => TRUE,
          'term_vocabulary' => $term->dbxref_id->db_id->name,
          'term_accession' => $term->dbxref_id->accession,
          'term_name' => $term->name,
          'base_table' => $table_name,
          'chado_table' => $prop_table,
          'chado_column' => $pkey,
        ],
        'widget' => [
          'type' => 'chado_linker__prop_widget',
          'settings' => [
            'display_label' => 1,
          ],
        ],
        'display' => [
          'default' => [
            'label' => 'hidden',
            'type' => 'chado_linker__prop_formatter',
            'settings' => [],
          ],
        ],
      ];

      // Make some customizations to some fields
      if ($term->name == 'Citation' and $term->cv_id->name == 'tripal_pub') {
        $info[$field_name]['required'] = TRUE;
        $info[$field_name]['description'] = t('All publications must have a unique citation.
          Please enter the full citation for this publication.  For PubMed style citations list
          the last name of the author followed by initials. Each author should be separated by a comma. Next comes
          the title, followed by the series title (e.g. journal name), publication date (4 digit year, 3 character Month, day), volume, issue and page numbers. You may also use HTML to provide a link in the citation.
          Below is an example: <pre>Medeiros PM, Ladio AH, Santos AM, Albuquerque UP. <a href="http://www.ncbi.nlm.nih.gov/pubmed/23462414" target="_blank">Does the selection of medicinal plants by Brazilian local populations
            suffer taxonomic influence?</a> J Ethnopharmacol. 2013 Apr 19; 146(3):842-52.</pre>');
        $info[$field_name]['settings']['rows'] = 3;
      }
      if ($term->name == 'Abstract' and $term->cv_id->name == 'tripal_pub') {
        $info[$field_name]['settings']['rows'] = 5;
      }
    }
  }

  // CVTERMS
  $term_table = $table_name . '_cvterm';
  if (chado_table_exists($term_table)) {
    $schema = chado_get_schema($term_table);
    $pkey = $schema['primary key'][0];
    $lkey = key($schema['foreign keys'][$table_name]['columns']);
    $rkey = $schema['foreign keys'][$table_name]['columns'][$lkey];

    $field_name = 'sio__annotation';
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Annotations',
      'description' => 'Annotations that are associated with this record.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $term_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'term_vocabulary' => 'SIO',
        'term_name' => 'annotation',
        'term_accession' => '001166',
      ],
      'widget' => [
        'type' => 'sio__annotation_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'sio__annotation_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // PUBLICATIONS
  $pub_table = $table_name . '_pub';
  if (chado_table_exists($pub_table)) {
    $field_name = 'schema__publication';
    $schema = chado_get_schema($pub_table);
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Publication',
      'description' => 'This record has been referenced or is sourced from these publications.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $pub_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'term_accession' => 'publication',
        'term_vocabulary' => 'schema',
        'term_name' => 'publication',
      ],
      'widget' => [
        'type' => 'schema__publication_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'schema__publication_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // PUBLICATIONS (in reverse)
  // We want to be able to show all of the content that a publication links
  // to. The sio__references field does that.
  if ($table_name == 'pub') {
    $field_name = 'sio__references';
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'References',
      'description' => 'Records that are referred to by the publication.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $table_name,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'term_accession' => '000631',
        'term_vocabulary' => 'SIO',
        'term_name' => 'references',
      ],
      'widget' => [
        'type' => 'sio__references_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'sio__references_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // RELATIONSHIPS
  // If the linker table does not exists then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table)) {
    $field_name = 'sbo__relationship';
    $schema = chado_get_schema($rel_table);
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Relationship',
      'description' => 'Other records with relationships to this record.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $rel_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'term_vocabulary' => 'SBO',
        'term_name' => 'Relationship',
        'term_accession' => '0000374',
      ],
      'widget' => [
        'type' => 'sbo__relationship_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'sbo__relationship_formatter',
          'settings' => [
            'title' => 'Relationships',
            'empty' => 'There are no relationships',
          ],
        ],
      ],
    ];
  }
  return;
  // SYNONYMS
  $syn_table = $table_name . '_synonym';
  if (chado_table_exists($syn_table)) {
    $field_name = 'schema__alternate_name';
    $schema = chado_get_schema($syn_table);
    $pkey = $schema['primary key'][0];
    $info[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Synonyms',
      'description' => 'Alternate names, aliases or synonyms for this record.',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'chado_table' => $syn_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'term_accession' => 'alternateName',
        'term_vocabulary' => 'schema',
        'term_name' => 'alternateName',
      ],
      'widget' => [
        'type' => 'schema__alternate_name_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'schema__alternate_name_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

}

/**
 * Used to find all of the properties for a given table.
 *
 * @param $table_name
 *   The name of the base table.
 * @param unknown $prop_table
 *   The name of the property table.
 * @param $type_table
 *   The name of the table that contains the type specifier.
 * @param $type_column
 *   The name of the column that contains the type specifier.
 *
 * @return
 *   An array of cvterm objects for the properties to be added as fields.
 */
function tripal_chado_bundle_get_properties($table_name, $prop_table, $type_table, $type_column, $cvterm_id, $type_value) {

  $tschema = chado_get_schema($table_name);
  $schema = chado_get_schema($prop_table);
  $tpkey = $tschema['primary key'][0];
  $pkey = $schema['primary key'][0];

  $props = NULL;

  // Property tables can be a bit tricky because not all property types
  // in the prop table are appropriate for each type of data.  Also som
  // bundle types are resolved via a property.  So, we have to distinguish
  // between these two cases.
  $sql = '';
  $args = [];

  // First, If this is the case where the base table is 'cvterm' then we are wanting
  // to get properties from the cvterm table using the cv_id as the
  // differentiator.
  if ($prop_table == 'cvtermprop') {
    $sql = "
        SELECT DISTINCT P.type_id
        FROM {" . db_escape_table($prop_table) . "} P
          INNER JOIN {" . db_escape_table($table_name) . "} T on T.$tpkey = P.$tpkey
        WHERE T.cv_id = :cv_id
      ";
    $args[':cv_id'] = $type_value;
    $props = chado_query($sql, $args);
  }
  // Second, is this the case where all of the records in the table are
  // of this type?  If so, then all properties apply
  else {
    if (!$type_column) {
      $sql = 'SELECT DISTINCT type_id FROM {' . db_escape_table($prop_table) . '}';
      $props = chado_query($sql, $args);
    }
    // Third, if this is the case where a content type is uniquely identified
    // by a type_id value in the base table, then only properties associated
    // with that type ID should be used.
    else {
      if ($type_column and !$type_table) {
        $sql = "
        SELECT DISTINCT P.type_id
        FROM {" . db_escape_table($prop_table) . "} P
          INNER JOIN {" . db_escape_table($table_name) . "} T on T.$tpkey = P.$tpkey
        WHERE T.$type_column = :cvterm_id
      ";
        $args[':cvterm_id'] = $cvterm_id;
        $props = chado_query($sql, $args);
      }
      // Fourth, if this is the case where a content type is uniquely identified
      // via a term/value pair in the prop table.
      else {
        if ($type_column and $type_table == $prop_table and !empty($type_value)) {
          $sql = "
        SELECT DISTINCT P2.type_id
        FROM {" . db_escape_table($prop_table) . "} P1
          INNER JOIN {" . db_escape_table($table_name) . "} T on T.$tpkey = P1.$tpkey
          INNER JOIN {" . db_escape_table($prop_table) . "} P2 on T.$tpkey = P2.$tpkey
        WHERE P1.$type_column = :cvterm_id AND P1.value = :prop_value AND
          P2.type_id != P1.type_id
       ";
          $args[':cvterm_id'] = $cvterm_id;
          $args[':prop_value'] = $type_value;
          $props = chado_query($sql, $args);
        }
        // Fifth, if this is the case where the content type is uniquely identified
        // via another table (e.g. cvterm linking table) and not this prop table.
        else {
          if ($type_column and $type_table != $prop_table and empty($type_value)) {
            $sql = "
         SELECT DISTINCT P.type_id
         FROM {" . db_escape_table($prop_table) . "} P
           INNER JOIN {" . db_escape_table($table_name) . "} T on T.$tpkey = P.$tpkey
           INNER JOIN {" . db_escape_table($type_table) . "} TT on TT.$tpkey = T.$tpkey
         WHERE TT.$type_column = :cvterm_id
       ";
            $args[':cvterm_id'] = $cvterm_id;
            $props = chado_query($sql, $args);
          }
        }
      }
    }
  }
  if (!$props) {
    return [];
  }

  // Iterate through all of the properties and do some final checks to see
  // which ones should be added.
  $prop_arr = [];
  if (!$props) {
    return $prop_arr;
  }
  while ($prop = $props->fetchObject()) {
    $term = chado_generate_var('cvterm', ['cvterm_id' => $prop->type_id]);
    $term = chado_expand_var($term, 'field', 'cvterm.definition');


    // The tripal_analysis_KEGG, tripal_analysis_blast, and
    // tripal_analysis_interpro modules store results in the analysisprop
    // table which is probably not the best place, but we don't want to
    // create a ton of fields for this.
    if ($prop_table == 'analysisprop' and
      ($term->dbxref_id->db_id->name == 'KEGG_BRITE' or
        $term->dbxref_id->db_id->name == 'tripal')) {
      continue;
    }

    // The Tripal publication importer adds properties to publications that
    // are also represented in the table fields.  We want editing of pub
    // related fields to always go back to the pub table, so we do not
    // want prop fields to show up.
    if ($table_name == 'pub') {
      $skip_pub_property = FALSE;
      $pub_terms = chado_get_semweb_terms('pub', ['return_object' => TRUE]);
      foreach ($pub_terms as $pub_column => $mapped_term) {
        $term_accession = $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession;
        $mapped_accession = $mapped_term->dbxref_id->db_id->name . ':' . $mapped_term->dbxref_id->accession;
        if ($term_accession == $mapped_accession) {
          $skip_pub_property = TRUE;
        }
      }
      if ($skip_pub_property) {
        continue;
      }
    }

    // Add the term to our list!
    $prop_arr[] = $term;
  }
  return $prop_arr;

}

/**
 * Implements hook_bundle_create_user_field().
 *
 * A priviledged user has the ability to add new fields to the bundle. The
 * chado_linker__prop and chado_linker__cvterm fields are allowed to be
 * added dynamically by the user.  But, Drupal doesn't know how to deal with
 * it, so this function is called for any field attached to a TripalEntity
 * bundle type. Any fields whose TripalField::$module argument is set to
 * 'tripal_chado' and that can be added dynamically will result in a call
 * to this function.
 */
function tripal_chado_bundle_create_user_field($new_field, $bundle) {

  // Get the table this bundle is mapped to.
  $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
  $vocab = $term->vocab;
  $params = [
    'vocabulary' => $vocab->vocabulary,
    'accession' => $term->accession,
  ];
  $chado_table = $bundle->data_table;
  $chado_type_table = $bundle->type_linker_table;
  $chado_type_column = $bundle->type_column;
  $chado_type_id = $bundle->type_id;
  $chado_type_value = $bundle->type_value;

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
      field_create_field([
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => [
          'type' => 'field_chado_storage',
        ],
      ]);

      // Now add the instance
      field_create_instance([
        'field_name' => $field_name,
        'entity_type' => 'TripalEntity',
        'bundle' => $bundle->name,
        'label' => $new_field['label'],
        'description' => '',
        'required' => FALSE,
        'settings' => [
          'auto_attach' => TRUE,
          'base_table' => $chado_table,
          'chado_table' => $table_name,
          'chado_column' => $pkey,
          'term_vocabulary' => '',
          'term_accession' => '',
          'term_name' => '',
        ],
        'widget' => [
          'type' => 'chado_linker__prop_widget',
          'settings' => [
            'display_label' => 1,
          ],
        ],
        'display' => [
          'default' => [
            'label' => 'inline',
            'type' => 'chado_linker__prop_formatter',
            'settings' => [],
          ],
        ],
      ]);
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
      field_create_field([
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => [
          'type' => 'field_chado_storage',
        ],
        'settings' => [
          'base_table' => $chado_table,
          'chado_table' => $table_name,
          'chado_column' => $pkey,
        ],
      ]);

      // Now add the instance
      field_create_instance([
        'field_name' => $field_name,
        'entity_type' => 'TripalEntity',
        'bundle' => $bundle->name,
        'label' => $new_field['label'],
        'description' => '',
        'required' => FALSE,
        'settings' => [
          'auto_attach' => TRUE,
        ],
        'widget' => [
          'type' => 'chado_linker__cvterm_widget',
          'settings' => [
            'display_label' => 1,
          ],
        ],
        'display' => [
          'default' => [
            'label' => 'above',
            'type' => 'chado_linker__cvterm_formatter',
            'settings' => [],
          ],
        ],
      ]);
    }
    else {
      drupal_set_message('Cannot add a property field to this entity. Chado does not support annotations for this data type.', 'error');
    }
  }
}


/**
 * Allows for altering of a field's instance setting form.
 *
 * This appears to be a Drupal hook but is actually a custom function created
 * by this module. It is called by the tripal_form_alter() function of this
 * module.
 *
 * Here we put additional form elements for any field, regardless if it is
 * a tripalField or not.
 *
 * @param $form
 *   The form array.  Alterations to the form can be made within this array.
 * @param $form_state
 *   The form state array.
 */
function tripal_chado_field_instance_settings_form_alter(&$form, $form_state) {
  global $language;
  $field = $form['#field'];
  $instance = $form['#instance'];

  $base_table = array_key_exists('base_table', $instance['settings']) ? $instance['settings']['base_table'] : '';
  $chado_table = array_key_exists('chado_table', $instance['settings']) ? $instance['settings']['chado_table'] : '';
  $chado_column = array_key_exists('chado_column', $instance['settings']) ? $instance['settings']['chado_column'] : '';


  if ($chado_table) {
    // Construct a table for the vocabulary information.
    $headers = [];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Base Table',
        'header' => TRUE,
        'width' => '20%',
      ],
      $base_table,
    ];
    $rows[] = [
      [
        'data' => 'Record Table',
        'header' => TRUE,
        'width' => '20%',
      ],
      $chado_table,
    ];
    $rows[] = [
      [
        'data' => 'ID Column',
        'header' => TRUE,
        'width' => '20%',
      ],
      $chado_column,
    ];
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
      ],
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => [],
      'empty' => '',
    ];

    $form['chado_mapping'] = [
      '#type' => 'fieldset',
      '#title' => 'Chado Mapping',
      '#description' => t('This field maps to data in Chado to the following table:'),
    ];
    $form['chado_mapping']['details'] = [
      '#type' => 'item',
      '#markup' => theme_table($table),
    ];
  }
}

/**
 * Implements hook_form_FROM_ID_alter()
 */
function tripal_chado_form_tripalbundle_form_alter(&$form, $form_state) {
  global $language;
  $bundle = $form['#bundle'];

  $term = entity_load('TripalTerm', ['id' => $bundle->term_id]);
  $term = reset($term);
  $vocab = $term->vocab;
  $params = [
    'vocabulary' => $vocab->vocabulary,
    'accession' => $term->accession,
  ];
  $chado_table = $bundle->data_table;
  $chado_column = $bundle->type_column;
  $chado_type_table = $bundle->type_linker_table;
  $chado_type_id = $bundle->type_id;
  $chado_type_value = $bundle->type_value;

  // Construct a table for the vocabulary information.
  $headers = [];
  $rows = [];
  $rows[] = [
    [
      'data' => 'Chado Table',
      'header' => TRUE,
      'width' => '20%',
    ],
    $chado_table,
  ];
  if ($chado_column) {
    $rows[] = [
      [
        'data' => 'Type Column',
        'header' => TRUE,
        'width' => '20%',
      ],
      $chado_column,
    ];
  }
  if ($chado_type_table) {
    $rows[] = [
      [
        'data' => 'Association Table',
        'header' => TRUE,
        'width' => '20%',
      ],
      $chado_type_table,
    ];
  }
  if ($chado_type_table and $chado_type_id) {
    $rows[] = [
      [
        'data' => 'CVTerm Table ID',
        'header' => TRUE,
        'width' => '20%',
      ],
      $chado_type_id,
    ];
  }
  if ($chado_type_value) {
    $rows[] = [
      [
        'data' => 'Property Value',
        'header' => TRUE,
        'width' => '20%',
      ],
      $chado_type_value,
    ];
  }
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  $form['chado_mapping'] = [
    '#type' => 'item',
    '#title' => 'Chado Mapping',
    '#markup' => theme_table($table),
    '#description' => t('This content type maps to the table in Chado
        listed above.  Chado allows multiple data types to be housed
        in a single table. Therefore, the column that is used to
        differentiate between data types is also listed above.'),
    '#weight' => 0,
  ];
}

/**
 * Implements hook_field_views_data_alter();
 */
function tripal_chado_field_views_data_alter(&$result, $field) {

  // This module creates the data__image field for managing images on
  // bio_data content types. But we want it to render correctly
  // so we need to set some handlers.
  if ($field['field_name'] == 'data__image') {
    foreach ($result as $key => $fields) {
      // It's not clear how to identify our bundle types, but they do have
      // a double underscore in their name to separate the vocabulary from
      // the accession, so we'll use that to find them.
      if (preg_match('/__/', $key)) {
        $result[$key]['data__image']['field']['handler'] = 'tripal_views_handler_field_image';
      }
    }
  }
}
