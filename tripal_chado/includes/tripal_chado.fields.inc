<?php

/**
 * Implements hook_field_create_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_create_fields() {

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

  // Create the fields for each column in the table.
  tripal_chado_create_fields_base($details);
  // Create custom fields.
  tripal_chado_create_fields_custom($details);
  // Create fields for linking tables.
  tripal_chado_create_fields_linker($details);
}
/**
 *
 * @param unknown $details
 */
function tripal_chado_create_fields_base($details) {

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

  // Get the list of columns for this table and create a new field for each one.
  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {
    $field_name = $table_name . '__' . $column_name;

    // Skip fields with a custom field:
    if ($field_name == 'dbxref_id' or $field_name == 'organism_id') {
      continue;
    }
    if ($table_name == 'feature' and ($field_name == 'md5checksum' or
        $field_name == 'residues' or $field_name == 'seqlen')) {
      continue;
    }
    if ($table_name == 'organism' and ($field_name == 'type_id')) {
      continue;
    }

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // Skip the type field.
    if ($table_name == $type_table and $column_name == $type_field) {
      continue;
    }

    // Set some defaults for the field.
    $info = array(
      'field_name' => $field_name,
      'type' => '',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => $column_name,
        'semantic_web' => tripal_get_chado_semweb_term($table_name, $column_name),
      ),
    );

    // Alter the field info array depending on the column details.
    switch($details['type']) {
      case 'char':
        $info['type'] = 'text';
        $info['settings']['max_length'] = $details['length'];
        break;
      case 'varchar':
        $info['type'] = 'text';
        $info['settings']['max_length'] = $details['length'];
        break;
      case 'text':
        $info['type'] = 'text';
        $info['settings']['max_length'] = 17179869184;
        $info['settings']['text_processing'] = 1;
        break;
      case 'blob':
        // not sure how to support a blob field.
        continue;
        break;
      case 'int':
        $info['type'] = 'number_integer';
        break;
      case 'float':
        $info['type'] = 'number_float';
        $info['settings']['precision'] = 10;
        $info['settings']['scale'] = 2;
        $info['settings']['decimal_separator'] = '.';
        break;
      case 'numeric':
        $info['type'] = 'number_decimal';
        break;
      case 'serial':
        // Serial fields are most likely not needed as a field.
        break;
      case 'boolean':
        $info['type'] = 'list_boolean';
        $info['settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
        break;
      case 'datetime':
        // Use the Drupal Date and Date API to create the field/widget
        $info['type'] = 'datetime';
        break;
    }

    // Set some default semantic web information
    if ($column_name == 'uniquename') {
      $info['settings']['text_processing'] = 0;
    }
    //
    // PUB TABLE
    //
    elseif ($table_name == 'pub' and $column_name == 'uniquename') {
      $info['type'] = 'text';
      $info['settings']['text_processing'] = 0;
    }

    //
    // ANALYSIS TABLE
    //
    elseif ($table_name == 'analysis' and $column_name == 'sourceuri') {
      $info['type'] = 'text';
      $info['settings']['text_processing'] = 0;
    }

    $field = field_create_field($info);
  }
}

/**
 *
 * @param unknown $details
 */
function tripal_chado_create_fields_custom($details) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];
  $schema = chado_get_schema($table_name);

  // BASE DBXREF
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_name = $table_name . '__dbxref_id';
    $field_type = 'chado_base__dbxref_id';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'dbxref_id',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'dbxref_id'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // BASE ORGANISM_ID
  if ($table_name != 'organism' and array_key_exists('organism_id', $schema['fields'])) {
    $field_name = $table_name . '__organism_id';
    $field_type = 'chado_base__organism_id';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'organism_id',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'organism_id'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURE MD5CHECKSUM
  if ($table_name == 'feature') {
    $field_name = $table_name . '__md5checksum';
    $field_type = 'chado_feature__md5checksum';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'md5checksum'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURE RESIDUES
  if ($table_name == 'feature') {
    $field_name = 'feature__residues';
    $field_type = 'chado_feature__residues';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'residues',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'residues'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURE SEQLEN
  if ($table_name == 'feature') {
    $field_name = 'feature__seqlen';
    $field_type = 'chado_feature__seqlen';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'seqlen',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'seqlen'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // GENE TRANSCRIPTS
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table) and $this->bundle->label == 'gene') {
    $field_name = 'gene_transcripts';
    $field_type = 'chado_gene__transcripts';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => 'md5checksum',
        'base_table' => $table_name,
        'semantic_web' => 'SO:0000673',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // ORGANISM TYPE_ID
  if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
    $field_name = 'organism__type_id';
    $field_type = 'chado_organism__type_id';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'organism',
        'chado_column' => 'type_id',
        'semantic_web' => tripal_get_chado_semweb_term('organism', 'type_id'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }
}

/**
 *
 * @param unknown $details
 */
function tripal_chado_create_fields_linker($details) {

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
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $contact_table,
        'chado_column' => 'contact_id',
        'base_table' => $table_name,
        'semantic_web' => 'local:contact'
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // CVTERM
  $cvterm_table = $table_name . '_cvterm';
  if (chado_table_exists($cvterm_table)) {
    $field_name = $table_name . '_cvterm';
    $field_type = 'chado_linker__cvterm_adder';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // DBXREF
  $dbxref_table = $table_name . '_dbxref';
  if (chado_table_exists($dbxref_table)) {
    $dbxref_table = $table_name . '_dbxref';
    $schema = chado_get_schema($dbxref_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_dbxref';
    $field_type = 'chado_linker__dbxref';
    $info = array(
      'field_name' =>  $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $dbxref_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'SBO:0000554',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }
  // EXPRESSION
  $expression_table = $table_name . '_expression';
  if (chado_table_exists($expression_table)) {
    $schema = chado_get_schema($expression_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_expression';
    $field_type = 'chado_linker__expression';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $expression_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'local:expression',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURELOC
  if ($table_name == 'feature') {
    $schema = chado_get_schema('featureloc');
    $pkey = $schema['primary key'][0];
    $field_name = 'featureloc';
    $field_type = 'chado_linker__featureloc';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'featureloc',
        'chado_column' => $pkey,
        'base_table' => 'feature',
        'semantic_web' => 'SO:position_of',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }
  // GENOTYPE
  $genotype_table = $table_name . '_genotype';
  if (chado_table_exists($genotype_table)) {
    $schema = chado_get_schema($genotype_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_genotype';
    $field_type = 'chado_linker__genotype';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $genotype_table,
        'chado_column' => $pkey,
        'semantic_web' => 'SO:0001027',
        'base_table' => $table_name,
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // PHENOTYPE
  $phenotype_table = $table_name . '_phenotype';
  if (chado_table_exists($phenotype_table)) {
    $schema = chado_get_schema($phenotype_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_phenotype';
    $field_type = 'chado_linker__phenotype';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $phenotype_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'SBO:0000358',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // PROPERTIES
  $prop_table = $table_name . 'prop';
  if (chado_table_exists($prop_table)) {
    $field_name = $table_name . 'prop';
    $field_type = 'chado_linker__prop_adder';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // PUBLICATIONS
  $pub_table = $table_name . '_pub';
  if (chado_table_exists($pub_table)) {
    $schema = chado_get_schema($pub_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_pub';
    $field_type = 'chado_linker__pub';
    $info =  array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $pub_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'schema:publication',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // RELATIONSHIPS
  // If the linker table does not exists then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table)) {
    $schema = chado_get_schema($rel_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_relationship';
    $field_type = 'chado_linker__relationship';
    $info =  array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'SBO:0000374',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // SYNONYMS
  $syn_table = $table_name . '_synonym';
  if (chado_table_exists($syn_table)) {
    $schema = chado_get_schema($syn_table);
    $pkey = $schema['primary key'][0];
    $field_name = $table_name . '_synonym';
    $field_type = 'chado_linker__synonym';
    $info =  array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $syn_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'schema:alternateName',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }
}


/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 * @param unknown $details
 */
function tripal_chado_create_instance_base($entity_type, $bundle, $details) {
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

  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {
    $field_name = $table_name . '__' . $column_name;

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // Skip the type field.
    if ($table_name == $type_table and $column_name == $type_field) {
      continue;
    }

    $info =  array(
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle_name,
      'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
      'description' => '',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
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
      $info['required'] = TRUE;
    }

    // Alter the field info array depending on the column details.
    switch($details['type']) {
      case 'char':
        $info['widget']['type'] = 'text_textfield';
        break;
      case 'varchar':
        $info['widget']['type'] = 'text_textfield';
        break;
      case 'text':
        $info['widget']['type'] = 'text_textarea';
        $info['widget']['settings']['format'] = filter_default_format();
        break;
      case 'blob':
        // not sure how to support a blob field.
        continue;
        break;
      case 'int':
        $info['widget']['type'] = 'number';
        break;
      case 'float':
        $info['widget']['type'] = 'number';
        break;
      case 'numeric':
        $info['widget']['type'] = 'number';
        break;
      case 'serial':
        // Serial fields are most likely not needed as a field.
        break;
      case 'boolean':
        $info['widget']['type'] = 'options_onoff';
        break;
      case 'datetime':
        $info['widget']['type'] = 'date_select';
        $info['widget']['settings']['increment'] = 1;
        $info['widget']['settings']['tz_handling'] = 'none';
        $info['widget']['settings']['collapsible'] = TRUE;

        // TODO: Add settings so that the minutes increment by 1.
        // And turn off the timezone, as the Chado field doesn't support it.
        break;
    }

    // Set some default semantic web information
    if ($column_name == 'uniquename') {
      $info['label'] = 'Identifier';
      $info['widget_type'] = 'text_textfield';
    }
    elseif ($info['label'] == 'Timeaccessioned') {
      $info['label'] = 'Time Accessioned';
      $info['description'] = 'Please enter the time that this record was first added to the database.';
    }
    elseif ($info['label'] == 'Timelastmodified') {
      $info['label'] = 'Time Last Modified';
      $info['description'] = 'Please enter the time that this record was last modified. The default is the current time.';
    }
    //
    // ORGANISM TABLE
    //
    elseif ($table_name == 'organism' and $column_name == 'comment') {
      $info['label'] = 'Description';
    }
    //
    // PUB TABLE
    //
    elseif ($table_name == 'pub' and $column_name == 'uniquename') {
      $info['widget_type'] = 'text_textfield';
    }

    //
    // ANALYSIS TABLE
    //
    elseif ($table_name == 'analysis' and $column_name == 'program') {
      $info['description'] = 'The program name (e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package then provide a very brief description of the pipeline, workflow or method.';
      $info['label'] = 'Program, Pipeline, Workflow or Method Name.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'sourceuri') {
      $info['widget_type'] = 'text_textfield';
      $info['label'] = 'Source URL';
      $info['description'] = 'The URL where the original source data was derived.  Ideally, this should link to the page where more information about the source data can be found.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'sourcename') {
      $info['label'] = 'Source Name';
      $info['description'] = 'The name of the source data. This could be a file name, data set or a small description for how the data was collected. For long descriptions use the larger description field.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'sourceversion') {
      $info['label'] = 'Source Version';
      $info['description'] = 'If hte source data set has a version include it here.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'algorithm') {
      $info['label'] = 'Source Version';
      $info['description'] = 'The name of the algorithm used to produce the dataset if different from the program.';
    }
    elseif ($table_name == 'analysis' and $column_name == 'programversion') {
      $info['label'] = 'Program Version';
      $info['description'] = 'The version of the program used to perform this analysis. (e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter "n/a" if no version is available or applicable.';
    }
    //
    // PROJECT TABLE
    //
    elseif ($table_name == 'project' and $column_name == 'description') {
      $info['label'] = 'Short Description';
    }
    $instance = field_create_instance($info);
  }
}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 * @param unknown $details
 */
function tripal_chado_create_instance_custom($entity_type, $bundle, $details) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];
  $schema = chado_get_schema($table_name);

  // BASE DBXREF
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_name = $table_name . '__dbxref_id';
    $info = array(
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
        'type' => 'chado_base__dbxref_id_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'chado_base__dbxref_id_formatter',
          'settings' => array(),
        ),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // BASE ORGANISM_ID
  if ($table_name != 'organism' and array_key_exists('organism_id', $schema['fields'])) {
    $field_name = $table_name . '__organism_id';
    $field_type = 'chado_base__organism_id';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'organism_id',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'organism_id'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURE MD5CHECKSUM
  if ($table_name == 'feature') {
    $field_name = $table_name . '__md5checksum';
    $field_type = 'chado_feature__md5checksum';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'md5checksum'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURE RESIDUES
  if ($table_name == 'feature') {
    $field_name = 'feature__residues';
    $field_type = 'chado_feature__residues';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'residues',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'residues'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // FEATURE SEQLEN
  if ($table_name == 'feature') {
    $field_name = 'feature__seqlen';
    $field_type = 'chado_feature__seqlen';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'seqlen',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'seqlen'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // GENE TRANSCRIPTS
  $rel_table = $table_name . '_relationship';
  if (chado_table_exists($rel_table) and $this->bundle->label == 'gene') {
    $field_name = 'gene_transcripts';
    $field_type = 'chado_gene__transcripts';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => 'md5checksum',
        'base_table' => $table_name,
        'semantic_web' => 'SO:0000673',
      ),
    );
    $field = new $field_type(array('info' => $info));
  }

  // ORGANISM TYPE_ID
  if ($table_name == 'organism' and array_key_exists('type_id', $schema['fields'])) {
    $field_name = 'organism__type_id';
    $field_type = 'chado_organism__type_id';
    $info = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'organism',
        'chado_column' => 'type_id',
        'semantic_web' => tripal_get_chado_semweb_term('organism', 'type_id'),
      ),
    );
    $field = new $field_type(array('info' => $info));
  }
}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 * @param unknown $details
 */
function tripal_chado_create_instance_linker($entity_type, $bundle, $details) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];
  $cv_id      = $details['chado_cv_id'];
  $cvterm_id  = $details['chado_cvterm_id'];
}


/**
 * Implements hook_field_create_instance_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_create_instance($entity_type, $bundle) {

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

  tripal_chado_create_instance_base($entity_type, $bundle, $details);
  tripal_chado_create_instance_custom($entity_type, $bundle, $details);
  tripal_chado_create_instance_linker($entity_type, $bundle, $details);

}
/**
 * A helper function for the tripal_chado_field_create_instance_info() function.
 *
 * This function adds in the custom fields info by instantiating the class
 * for the custom field, calling the create_instance_info() function and
 * returning the info array.
 *
 * @param $entity_type
 *   The type of entity (e.g TripalEntity)
 * @param $bundle
 *   The bundle object.
 * @param $details
 *   An array containing the mapping of the bundle to the Chado table.
 */
function tripal_chado_field_create_instance_info_custom($entity_type, $bundle, $details) {
  $info = array();
  $fields = tripal_get_fields('tripal_chado', $entity_type, $bundle, $details);
  foreach ($fields as $field) {
    $field_name = $field->getFieldName();
    if ($field->canAttach()) {
      $info[$field_name] = $field->createInstanceInfo();
    }
  }
  return $info;
}

/**
 * Implements hook_field_widget_info().
 *
 * This function would normally provide a large info array for all of the
 * widgets provided by this module.  But instead it will call a hook that
 * can be implmented within each individual field file.  This will allow
 * all of the code for a single field to be self contained in a single file.
 */
function tripal_chado_field_widget_info() {
  $info = array();

  $field_types = tripal_get_field_types('tripal_chado');
  foreach ($field_types as $field_type) {
    $info += $field_type::widgetInfo();
  }
  return $info;
}
/**
 * Implements hook_field_formatter_info().
 *
 * This function would normally provide a large info array for all of the
 * formatters provided by this module.  But instead it will call a hook that
 * can be implmented within each individual field file.  This will allow
 * all of the code for a single field to be self contained in a single file.
 */
function tripal_chado_field_formatter_info() {
  $info = array();

  $field_types = tripal_get_field_types('tripal_chado');
  foreach ($field_types as $field_type) {
    $info += $field_type::formatterInfo();
  }
  return $info;
}

/**
 * Implements hook_field_settings_form()
 */
function tripal_chado_field_settings_form($field, $instance, $has_data) {
  $form = array();
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::fieldSettingsForm($field, $instance, $has_data);
  }
  return $form;
}
/**
 * Implements hook_instance_settings_form()
 */
function tripal_chado_field_instance_settings_form($field, $instance) {
  $form = array();
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::instanceSettingsForm($field, $instance);
  }
  return $form;
}

/**
 * Implements hook_field_settings_form_validate().
 *
 * This is not an actual Drpual hook, but rather a Tripal created hook
 * to alow the TripalField objects to have a fieldSettingsFormValidate()
 * member function.
 */
function tripal_chado_field_settings_form_validate($form, &$form_state) {
  $field = $form['#field'];
  $instance = $form['#instance'];
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::fieldSettingsFormValidate($field, $instance, $form, $form_state);
  }
}
/**
 * Implements hook_instance_settings_form_validate().
 *
 * This is not an actual Drpual hook, but rather a Tripal created hook
 * to alow the TripalField objects to have a instanceSettingsFormValidate()
 * member function.
 */
function tripal_chado_field_instance_settings_form_validate($form, &$form_state) {
  $field = $form['#field'];
  $instance = $form['#instance'];
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::instanceSettingsFormValidate($field, $instance, $form, $form_state);
  }
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function tripal_chado_field_formatter_settings_summary($field, $instance, $view_mode) {

  $summary = '';
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::formatterSettingsSummary($field, $instance, $view_mode);
  }
  return $summary;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function tripal_chado_field_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {

  $form = array();
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::formatterSettingsForm(field, $instance, $view_mode, $form, $form_state);
  }
  return $form;
}



/**
 * Implements hook_field_formatter_view().
 */
function tripal_chado_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $element = array();
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $field_type::formatterView($element, $entity_type, $entity, $field, $instance, $langcode, $items, $display);
  }
  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_chado_field_widget_form(&$form, &$form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $field_type::widgetForm($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  }
  return $widget;


}


/**
 * Implements hook_field_widget_form_alter().
 */
function tripal_chado_field_widget_form_alter(&$element, &$form_state, $context) {

  if (array_key_exists('#field_name', $element)) {
    $field_name = $element['#field_name'];
    $matches = array();

    if (preg_match('/(.+?)__(.+?)$/', $field_name, $matches)) {
      $tablename = $matches[1];
      $colname = $matches[2];
      $schema = chado_get_schema($tablename);
      if (!$schema) {
        return;
      }

      // The timelastmodified field exists in many Chado tables.  We want
      // the form element to update to the most recent time rather than the time
      // in the database.
      if ($colname == 'timelastmodified' and $schema['fields'][$colname]['type'] == 'datetime') {
        // We want the default value for the field to be the current time.
        $element['#default_value']['value'] = format_date(time(), 'custom', "Y-m-d H:i:s", 'UTC');
        $element['#date_items']['value'] = $element['#default_value']['value'];
      }
      // We want the date combo fieldset to be collaspible so we will
      // add our own theme_wrapper to replace the one added by the date
      // module.
      if (array_key_exists($colname, $schema['fields']) and $schema['fields'][$colname]['type'] == 'datetime') {
        $element['#theme_wrappers'] = array('tripal_chado_date_combo');
      }
    }
  }
}

/**
 * Implements hook_field_validate()
 */
function tripal_chado_field_validate($entity_type, $entity, $field, $instance,
     $langcode, $items, &$errors) {

  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $bundle = isset($entity->bundle) ? $entity->bundle : NULL;
    $form = $field_type::widgetFormValidate($entity_type, $entity, $field, $instance,
        $langcode, $items, $errors);
  }
}

/**
 * Implements hook_field_submit()
 *
 * This is a TripalEntity specific hook.
 */
function tripal_chado_field_submit($entity_type, $entity, $field, $instance,
    $langcode, &$items, $form, &$form_state) {

  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    $form = $field_type::widgetFormSubmit($entity_type, $entity, $field, $instance,
        $langcode, $items, $form, $form_state);
  }
}



/**
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_display_overview_form is used for formatting the display
 * or layout of fields attached to an entity and shown on the entity view page.
 *
 * This function removes the cvterm class and property adder field as those are
 * really not meant for users to show or manage.
 */
function tripal_chado_form_field_ui_display_overview_form_alter(&$form, &$form_state, $form_id) {
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
 * Implements hook_field_is_empty().
 */
function tripal_chado_field_is_empty($item, $field) {

  // If there is no value field then the field is empty.
  if (!array_key_exists('value', $item)) {
    return TRUE;
  }

  // Iterate through all of the fields and if at least one has a value
  // the field is not empty.
  foreach ($item as $form_field_name => $value) {
    if (isset($value) and $value != NULL and $value != '') {
      return FALSE;
    }
  }

  // Otherwise, the field is empty.
  return TRUE;
}










