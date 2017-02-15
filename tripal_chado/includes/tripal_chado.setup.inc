<?php
/**
 * @file
 * Functions to install chado schema through Drupal
 */

/**
 * Prepares Chado for Tripal use
 */
function tripal_chado_prepare_form($form, $form_state) {
  $form = array();

  if (variable_get('tripal_chado_is_prepared') == TRUE) {
    drupal_set_message('Your site is prepared.');
  }
  $form['instructions'] = array(
    '#type' => 'item',
    '#title' => 'Prepare Drupal for Chado.',
    '#description' => t("Before a Drupal site can use Chado (via Tripal), both
        Chado and Drupal must be prepared a bit more.  Tripal will add some new
        materialized views, custom tables and controlled vocabularies to Chado.
        It will also add some management tables to Drupal. You only are
        required to prepare your Drupal site if this is a brand-new Drupal
        installation or if Chado was installed outside of Tripal.  If you
        installed Chado using Tripal then you do not need to run this step.
        If you are upgrading from a previous version of Tripal, you do not
        need to prepare your site, and you can click the 'Skip' button."),
  );

  $form['prepare-button'] = array(
    '#type' => 'submit',
    '#value' => t('Prepare this site'),
    '#name' => 'prepare-chado',
  );
  $form['skip-button'] = array(
    '#type' => 'submit',
    '#value' => t('Skip'),
    '#name' => 'prepare-skip',
  );
  return $form;
}

/**
 * Submit function for the tripal_chado_prepare_form().
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_prepare_form_submit($form, $form_state) {
   if ($form_state['clicked_button']['#name'] == "prepare-chado") {
     global $user;
     $args = array();
     $includes = array(
        module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.setup'),
        module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader'),
     );
     tripal_add_job('Prepare Chado', 'tripal_chado',
       'tripal_chado_prepare_chado', $args,
       $user->uid, 10, $includes);
   }
   if ($form_state['clicked_button']['#name'] == "prepare-skip") {
     variable_set('tripal_chado_is_prepared', TRUE);
   }
}

/**
 *
 */
function tripal_chado_load_ontologies() {

  // Insert commonly used ontologies into the tables.
  $ontologies = array(
    array(
      'name' => 'Relationship Ontology (legacy)',
      'path' => '{tripal_chado}/files/legacy_ro.obo',
      'auto_load' => FALSE,
      'cv_name' => 'ro'
    ),
    array(
      'name' => 'Gene Ontology',
      'path' => 'http://purl.obolibrary.org/obo/go.obo',
      'auto_load' => FALSE,
      'cv_name' => 'cellualar_component'
    ),
    array(
      'name' => 'Taxonomic Rank',
      'path' => 'http://purl.obolibrary.org/obo/taxrank.obo',
      'auto_load' => TRUE,
      'cv_name' => 'taxonomic_rank'
    ),
    array(
      'name' => 'Tripal Contact',
      'path' => '{tripal_chado}/files/tcontact.obo',
      'auto_load' => TRUE,
      'cv_name' => 'tripal_contact'
    ),
    array(
      'name' => 'Tripal Publication',
      'path' => '{tripal_chado}/files/tpub.obo',
      'auto_load' => TRUE,
      'cv_name' => 'tripal_pub'
    ),
     array(
       'name' => 'Sequence Ontology',
       'path' => 'http://purl.obolibrary.org/obo/so.obo',
       'auto_load' => TRUE,
       'cv_name' => 'sequence'
     ),

  );
  for ($i = 0; $i < count($ontologies); $i++) {
    $obo_id = tripal_insert_obo($ontologies[$i]['name'], $ontologies[$i]['path']);
    if ($ontologies[$i]['auto_load'] == TRUE) {
      // Only load ontologies that are not already in the cv table.
      $cv = tripal_get_cv(array('name' => $ontologies[$i]['cv_name']));
      if (!$cv) {
        tripal_chado_load_obo_v1_2_id($obo_id);
      }
    }
  }
}
/**
 * Prepares Chado for use by Tripal.
 */
function tripal_chado_prepare_chado() {

  try {

    // We want to provide a set of commonly used entity types by default. This
    // way when a user first installs Tripal there are some commonly used
    // formats.
    module_load_include('inc', 'tripal', 'api/tripal.api');
    module_load_include('inc', 'tripal', 'includes/tripal.admin');
    module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.semweb');

    // Get the effective version.  Pass true as second argument
    // to warn the user if the current version is not compatible.
    $version = chado_get_version(FALSE, FALSE);

    // We want to force the version of Chado to be set properly.
    $real_version = chado_get_version(TRUE);

    // Create the temp table we will use for loading OBO files.
    tripal_chado_add_tripal_obo_temp_table();

    // Import commonly used ontologies if needed.
    tripal_chado_load_ontologies();

    // Populate the semantic web associations for Chado tables/fields.
    tripal_chado_populate_chado_semweb_table();

    // Initialize the population of the chado_cvterm_mapping table.  This will
    // map existing data types already in Chado so that when users want to
    // add new content types it simplifies the form for them.
    tripal_chado_map_cvterms();

    // Create the 'Organism' entity type. This uses the obi:organism term.
    $error = '';
    $args = array(
      'vocabulary' => 'OBI',
      'accession' => '0100026',
      'term_name' => 'organism',
      'storage_args' => array(
        'data_table' => 'organism',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'OBI', 'accession' => '0100026'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Create the 'Analysis' entity type. This uses the local:analysis term.
    $error = '';
    $args = array(
      'vocabulary' => 'local',
      'accession' => 'analysis',
      'term_name' => 'analysis',
      'storage_args' => array(
        'data_table' => 'analysis',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'local', 'accession' => 'analysis'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Create the 'Project' entity type. This uses the local:project term.
    $error = '';
    $args = array(
      'vocabulary' => 'local',
      'accession' => 'project',
      'term_name' => 'project',
      'storage_args' => array(
        'data_table' => 'project',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'local', 'accession' => 'project'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Create the 'Map' entity type. This uses the local:project term.
    $error = '';
    $args = array(
      'vocabulary' => 'data',
      'accession' => '1274',
      'term_name' => 'Map',
      'storage_args' => array(
        'data_table' => 'featuremap',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'data', 'accession' => '1274'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Create the 'Publication' entity type.
    $error = '';
    $args = array(
      'vocabulary' => 'TPUB',
      'accession' => '0000002',
      'term_name' => 'Publication',
      'storage_args' => array(
        'data_table' => 'pub',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'TPUB', 'accession' => '0000002'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Create the 'Gene' entity type.
    $error = '';
    $args = array(
      'vocabulary' => 'SO',
      'accession' => '0000704',
      'term_name' => 'gene',
      'storage_args' => array(
        'data_table' => 'feature',
        'type_column' => 'type_id',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'SO', 'accession' => '0000704'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Create the 'mRNA' entity type.
    $error = '';
    $args = array(
      'vocabulary' => 'SO',
      'accession' => '0000234',
      'term_name' => 'mRNA',
      'storage_args' => array(
        'data_table' => 'feature',
        'type_column' => 'type_id',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'SO', 'accession' => '0000234'));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    // Add the cv_root_mview.
    tripal_chado_add_cv_root_mview_mview();

    // Support for Analyses
    // we may need the analysisfeatureprop table if it doesn't already exist
    tripal_chado_add_analysisfeatureprop_table();

    // add materialized views
    tripal_chado_add_analysis_organism_mview();

    // Support for Contacts
    // Add the contactprop table to Chado.
    tripal_chado_add_contactprop_table();

    // Support for Features
    // Add the materialized view.
    tripal_chado_add_organism_feature_count_mview();

    // Add the custom tables for the GFF loader.
    tripal_chado_add_tripal_gff_temp_table();
    tripal_chado_add_tripal_gffcds_temp_table();
    tripal_chado_add_tripal_gffprotein_temp_table();

    // Support for FeatureMaps.
    // add the featuremapprop table to Chado
    tripal_chado_add_featuremap_dbxref_table();
    tripal_chado_add_featuremapprop_table();
    //tripal_chado_add_featuremappos_table();

    // Support for Libraries.
    // add the materialized view
    tripal_chado_add_library_feature_count_mview();

    // Support for Pubs.
    // add the custom tables
    tripal_chado_add_pubauthor_contact_table();
    // Add the supported loaders
    variable_set('tripal_pub_supported_dbs', array('PMID', 'AGL'));

    // Support for Stocks.
    // add the materialized view
    tripal_chado_add_organism_stock_count_mview();

    // Set a variable to indicate the site is prepared.
    variable_set('tripal_chado_is_prepared', TRUE);
  }
  catch (Exception $e) {
    throw new Exception($e);
  }
}

/**
 * Creates a materialized view that stores the type & number of stocks per organism
 *
 * @ingroup tripal_stock
 */
function tripal_chado_add_organism_stock_count_mview() {
  $view_name = 'organism_stock_count';
  $comment = 'Stores the type and number of stocks per organism';

  $schema = array(
    'description' => $comment,
    'table' => $view_name,
    'fields' => array(
      'organism_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genus' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'species' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'common_name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'num_stocks' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'stock_type' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'organism_stock_count_idx1' => array('organism_id'),
      'organism_stock_count_idx2' => array('cvterm_id'),
      'organism_stock_count_idx3' => array('stock_type'),
    ),
  );

  $sql = "
    SELECT
        O.organism_id, O.genus, O.species, O.common_name,
        count(S.stock_id) as num_stocks,
        CVT.cvterm_id, CVT.name as stock_type
     FROM organism O
        INNER JOIN stock S  ON O.Organism_id = S.organism_id
        INNER JOIN cvterm CVT ON S.type_id     = CVT.cvterm_id
     GROUP BY
        O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
  ";

  tripal_add_mview($view_name, 'tripal_stock', $schema, $sql, $comment);
}

/**
 * Add custom table related to publications
 *  - pubauthor_contact
 *
 * @ingroup tripal_pub
 */
function tripal_chado_add_pubauthor_contact_table() {
  $schema = array (
    'table' => 'pubauthor_contact',
    'fields' => array (
      'pubauthor_contact_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'contact_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'pubauthor_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'pubauthor_contact_id',
    ),
    'unique keys' => array (
      'pubauthor_contact_c1' => array (
        0 => 'contact_id',
        1 => 'pubauthor_id',
      ),
    ),
    'foreign keys' => array (
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
      'pubauthor' => array (
        'table' => 'pubauthor',
        'columns' => array (
          'pubauthor_id' => 'pubauthor_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('pubauthor_contact', $schema, TRUE);
}


/**
 * Adds a materialized view keeping track of the type of features associated with each library
 *
 * @ingroup tripal_library
 */
function tripal_chado_add_library_feature_count_mview(){
  $view_name = 'library_feature_count';
  $comment = 'Provides count of feature by type that are associated with all libraries';

  $schema = array(
    'table' => $view_name,
    'description' => $comment,
    'fields' => array(
      'library_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      'num_features' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_type' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'library_feature_count_idx1' => array('library_id'),
    ),
  );

  $sql = "
    SELECT
      L.library_id, L.name,
      count(F.feature_id) as num_features,
      CVT.name as feature_type
    FROM library L
      INNER JOIN library_feature LF  ON LF.library_id = L.library_id
      INNER JOIN feature F           ON LF.feature_id = F.feature_id
      INNER JOIN cvterm CVT          ON F.type_id     = CVT.cvterm_id
    GROUP BY L.library_id, L.name, CVT.name
  ";

  tripal_add_mview($view_name, 'tripal_library', $schema, $sql, $comment);
}

/**
 * Add custom tables needed by the feature map module
 *  - featuremapprop
 *  - featuremap_dbxref
 *  - featureposprop
 *
 * @ingroup tripal_featuremap
 */
function tripal_chado_add_featuremapprop_table(){
  // add the featuremaprop table to Chado
  $schema = array (
    'table' => 'featuremapprop',
    'fields' => array (
      'featuremapprop_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'featuremapprop_id',
    ),
    'unique keys' => array (
      'featuremapprop_c1' => array (
        0 => 'featuremap_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'featuremapprop_idx1' => array (
        0 => 'featuremap_id',
      ),
      'featuremapprop_idx2' => array (
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
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('featuremapprop', $schema, TRUE);
}
/**
 *
 */
function tripal_chado_add_featuremap_dbxref_table(){
  // add the featuremap_dbxref table to Chado
  $schema = array (
    'table' => 'featuremap_dbxref',
    'fields' => array (
      'featuremap_dbxref_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'featuremap_dbxref_id',
    ),
    'unique keys' => array (
      'featuremap_dbxref_c1' => array (
        0 => 'featuremap_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'featuremap_dbxref_idx1' => array (
        0 => 'featuremap_dbxref_id',
      ),
      'featuremap_dbxref_idx2' => array (
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
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
    'referring_tables' => NULL,
  );
  chado_create_custom_table('featuremap_dbxref', $schema, TRUE);
}
/**
 *
 */
function tripal_chado_add_featureposprop_table(){
  $schema = array (
    'table' => 'featureposprop',
    'fields' => array (
      'featureposprop_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'featurepos_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'featureposprop_id',
    ),
    'unique keys' => array (
      'featureposprop_id' => array (
        0 => 'featurepos_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'featureposprop_c1' => array (
        0 => 'featurepos_id',
      ),
      'featureposprop_idx2' => array (
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
      'featurepos' => array (
        'table' => 'featurepos',
        'columns' => array (
          'featurepos_id' => 'featurepos_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('featureposprop', $schema, TRUE);
}

/**
 *
 */
function tripal_chado_add_tripal_gff_temp_table() {
  $schema = array(
    'table' => 'tripal_gff_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'uniquename' => array(
        'type' => 'text',
        'not null' => TRUE,
      ),
      'type_name' => array(
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('organism_id'),
      'tripal_gff_temp_idx1' => array('uniquename'),
    ),
    'unique keys' => array(
      'tripal_gff_temp_uq0' => array('feature_id'),
      'tripal_gff_temp_uq1' => array('uniquename', 'organism_id', 'type_name'),
    ),
  );
  chado_create_custom_table('tripal_gff_temp', $schema, TRUE);
}

/**
 *
 */
function tripal_chado_add_tripal_gffcds_temp_table($skip_recreate = TRUE) {
  $schema = array(
    'table' => 'tripal_gffcds_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'parent_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phase' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'strand' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmin' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmax' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('parent_id'),
    ),
  );
  chado_create_custom_table('tripal_gffcds_temp', $schema, $skip_recreate);
}

/**
 *
 */
function tripal_chado_add_tripal_gffprotein_temp_table() {
  $schema = array(
    'table' => 'tripal_gffprotein_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'parent_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmin' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmax' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('parent_id'),
    ),
    'unique keys' => array(
      'tripal_gff_temp_uq0' => array('feature_id'),
    ),
  );
  chado_create_custom_table('tripal_gffprotein_temp', $schema, TRUE);
}
/**
 * Creates a materialized view that stores the type & number of features per organism
 *
 * @ingroup tripal_feature
 */
function tripal_chado_add_organism_feature_count_mview() {
  $view_name = 'organism_feature_count';
  $comment = 'Stores the type and number of features per organism';

  $schema = array(
    'description' => $comment,
    'table' => $view_name,
    'fields' => array(
      'organism_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genus' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'species' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'common_name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'num_features' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_type' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'organism_feature_count_idx1' => array('organism_id'),
      'organism_feature_count_idx2' => array('cvterm_id'),
      'organism_feature_count_idx3' => array('feature_type'),
    ),
  );

  $sql = "
    SELECT
        O.organism_id, O.genus, O.species, O.common_name,
        count(F.feature_id) as num_features,
        CVT.cvterm_id, CVT.name as feature_type
     FROM organism O
        INNER JOIN feature F  ON O.Organism_id = F.organism_id
        INNER JOIN cvterm CVT ON F.type_id     = CVT.cvterm_id
     GROUP BY
        O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
  ";

  tripal_add_mview($view_name, 'tripal_feature', $schema, $sql, $comment);
}
/**
 * Add any custom tables needed by this module.
 * - Contactprop: keep track of properties of contact
 *
 * @ingroup tripal_contact
 */
function tripal_chado_add_contactprop_table(){
  $schema = array (
    'table' => 'contactprop',
    'fields' => array (
      'contactprop_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'contact_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'contactprop_id',
    ),
    'unique keys' => array (
      'contactprop_c1' => array (
        0 => 'contact_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'contactprop_idx1' => array (
        0 => 'contact_id',
      ),
      'contactprop_idx2' => array (
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
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('contactprop', $schema, TRUE);
}
/**
 * Create a legacy custom chado table (analysisfeatureprop) to store properties of
 * analysisfeature links.
 *
 * @ingroup tripal_analysis
 */
function tripal_chado_add_analysisfeatureprop_table() {

  // Create analysisfeatureprop table in chado.  This is needed for Chado
  // version 1.11, the table exists in Chado 1.2.
  if (!db_table_exists('chado.analysisfeatureprop')) {
    $sql = "
      CREATE TABLE {analysisfeatureprop} (
        analysisfeatureprop_id SERIAL PRIMARY KEY,
        analysisfeature_id     INTEGER NOT NULL,
        type_id                INTEGER NOT NULL,
        value                  TEXT,
        rank                   INTEGER NOT NULL,
        CONSTRAINT analysisfeature_id_type_id_rank UNIQUE (analysisfeature_id, type_id, rank),
        CONSTRAINT analysisfeatureprop_analysisfeature_id_fkey FOREIGN KEY (analysisfeature_id) REFERENCES {analysisfeature}(analysisfeature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED,
        CONSTRAINT analysisfeatureprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES {cvterm}(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED
      )
    ";
    chado_query($sql);
  }
}
/**
 * Creates a view showing the link between an organism & it's analysis through associated features.
 *
 * @ingroup tripal_analysis
 */
function tripal_chado_add_analysis_organism_mview() {
  $view_name = 'analysis_organism';
  $comment = t('This view is for associating an organism (via it\'s associated features) to an analysis.');

  // this is the SQL used to identify the organism to which an analsysis
  // has been used.  This is obtained though the analysisfeature -> feature -> organism
  // joins
  $sql = "
    SELECT DISTINCT A.analysis_id, O.organism_id
    FROM analysis A
      INNER JOIN analysisfeature AF ON A.analysis_id = AF.analysis_id
      INNER JOIN feature F          ON AF.feature_id = F.feature_id
      INNER JOIN organism O         ON O.organism_id = F.organism_id
  ";

  // the schema array for describing this view
  $schema = array(
    'table' => $view_name,
    'description' => $comment,
    'fields' => array(
      'analysis_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'networkmod_qtl_indx0' => array('analysis_id'),
      'networkmod_qtl_indx1' => array('organism_id'),
    ),
    'foreign keys' => array(
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
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

  // add the view
  tripal_add_mview($view_name, 'tripal_analysis', $schema, $sql, $comment);
}

/**
 * Add a materialized view of root terms for all chado cvs. This is needed for viewing cv trees
 *
 * @ingroup tripal_cv
 */
function tripal_chado_add_cv_root_mview_mview() {
  $mv_name = 'cv_root_mview';
  $comment = 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees';
  $schema = array(
    'table' => $mv_name,
    'description' => $comment,
    'fields' => array(
      'name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cv_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cv_name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'cv_root_mview_indx1' => array('cvterm_id'),
      'cv_root_mview_indx2' => array('cv_id'),
    ),
  );

  $sql = "
    SELECT DISTINCT CVT.name,CVT.cvterm_id, CV.cv_id, CV.name
    FROM cvterm_relationship CVTR
      INNER JOIN cvterm CVT on CVTR.object_id = CVT.cvterm_id
      INNER JOIN cv CV on CV.cv_id = CVT.cv_id
    WHERE CVTR.object_id not in
      (SELECT subject_id FROM cvterm_relationship)
  ";

  // Create the MView
  tripal_add_mview($mv_name, 'tripal_cv', $schema, $sql, $comment);
}
/**
 * Creates a temporary table to store obo details while loading an obo file
 *
 * @ingroup tripal_cv
 */
function tripal_chado_add_tripal_obo_temp_table() {
  // the tripal_obo_temp table is used for temporary housing of records when loading OBO files
  // we create it here using plain SQL because we want it to be in the chado schema but we
  // do not want to use the Tripal Custom Table API because we don't want it to appear in the
  // list of custom tables.  It needs to be available for the Tripal Chado API so we create it
  // here and then define it in the tripal_cv/api/tripal_cv.schema.api.inc
  if (!chado_table_exists('tripal_obo_temp')) {
    $sql = "
      CREATE TABLE {tripal_obo_temp} (
        id character varying(255) NOT NULL,
        stanza text NOT NULL,
        type character varying(50) NOT NULL,
        CONSTRAINT tripal_obo_temp_uq0 UNIQUE (id)
      );
    ";
    chado_query($sql);
    $sql = "CREATE INDEX tripal_obo_temp_idx0 ON {tripal_obo_temp} USING btree (id)";
    chado_query($sql);
    $sql = "CREATE INDEX tripal_obo_temp_idx1 ON {tripal_obo_temp} USING btree (type)";
    chado_query($sql);
  }
}

