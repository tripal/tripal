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
        module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.setup'),
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
 * Submit function for the tripal_chado_prepare_form().
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_prepare_drush_submit() {
  $args = array();
  $includes = array(
    module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.setup'),
  );
  tripal_add_job('Prepare Chado', 'tripal_chado',
    'tripal_chado_prepare_chado', $args,
    1, 10, $includes);

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
      'cv_name' => 'ro',
      'db_name' => 'RO',
    ),
    array(
      'name' => 'Gene Ontology',
      'path' => 'http://purl.obolibrary.org/obo/go.obo',
      'auto_load' => FALSE,
      'cv_name' => 'cellualar_component',
      'db_name' => 'GO',
    ),
    array(
      'name' => 'Taxonomic Rank',
      'path' => 'http://purl.obolibrary.org/obo/taxrank.obo',
      'auto_load' => TRUE,
      'cv_name' => 'taxonomic_rank',
      'db_name' => 'TAXRANK'
    ),
    array(
      'name' => 'Tripal Contact',
      'path' => '{tripal_chado}/files/tcontact.obo',
      'auto_load' => TRUE,
      'cv_name' => 'tripal_contact',
      'db_name' => 'TCONTACT'
    ),
    array(
      'name' => 'Tripal Publication',
      'path' => '{tripal_chado}/files/tpub.obo',
      'auto_load' => TRUE,
      'cv_name' => 'tripal_pub',
      'db_name' => 'TPUB',
    ),
     array(
       'name' => 'Sequence Ontology',
       'path' => 'http://purl.obolibrary.org/obo/so.obo',
       'auto_load' => TRUE,
       'cv_name' => 'sequence',
       'db_name' => 'SO',
     ),

  );

  module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter');
  for ($i = 0; $i < count($ontologies); $i++) {
    $obo_id = tripal_insert_obo($ontologies[$i]['name'], $ontologies[$i]['path']);
    if ($ontologies[$i]['auto_load'] == TRUE) {
      // Only load ontologies that are not already in the cv table.
      $cv = tripal_get_cv(array('name' => $ontologies[$i]['cv_name']));
      $db = tripal_get_db(array('name' => $ontologies[$i]['db_name']));
      if (!$cv or !$db) {
        print "Loading ontology: " . $ontologies[$i]['name'] . " ($obo_id)...\n";
        $obo_importer = new OBOImporter();
        $obo_importer->create(array('obo_id' => $obo_id));
        $obo_importer->run();
      }
      else {
        print "Ontology already loaded (skipping): " . $ontologies[$i]['name'] . "...\n";
      }
    }
  }
}
/**
 * Prepares Chado for use by Tripal.
 */
function tripal_chado_prepare_chado($job = NULL) {

  // Retrieve the job arguement in order to report progress.
  if (is_int($job)) {
    $job = new TripalJob();
    $job->load($job);
  }
  $report_progress = TRUE;
  if (!is_object($job)) {
    $report_progress = FALSE;
  }

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

    // Create custom tables depending on the Chado version installed.
    drush_print("Creating Tripal Materialized Views and Custom Tables...");
    $chado_version = chado_get_version();
    if ($chado_version == '1.1') {
      tripal_chado_add_v1_1_custom_tables();
      tripal_chado_add_vx_x_custom_tables();
    }
    if ($chado_version == '1.2') {
      tripal_chado_add_v1_2_custom_tables();
      tripal_chado_add_vx_x_custom_tables();
    }
    if ($chado_version == '1.3') {
      tripal_chado_add_vx_x_custom_tables();
      tripal_chado_fix_v1_3_custom_tables();
    }

    if ($report_progress) {
      $job->setProgress(5);
    }

    // Import commonly used ontologies if needed.
    drush_print("Loading Ontologies...");
    tripal_chado_load_ontologies();

    // Populate the mviews based on controlled vocabularies.
    drush_print('Populating materialized view db2cv_mview...');
    $mview_id = tripal_get_mview_id('db2cv_mview');
    tripal_populate_mview($mview_id);

    drush_print('Populating materialized view cv_root_mview...');
    $mview_id = tripal_get_mview_id('cv_root_mview');
    tripal_populate_mview($mview_id);

    if ($report_progress) {
      $job->setProgress(50);
    }

    // Populate the semantic web associations for Chado tables/fields.
    drush_print("Making semantic connections for Chado tables/fields...");
    tripal_chado_populate_chado_semweb_table();

    if ($report_progress) {
      $job->setProgress(60);
    }

    // Initialize the population of the chado_cvterm_mapping table.  This will
    // map existing data types already in Chado so that when users want to
    // add new content types it simplifies the form for them.
    drush_print("Map Chado Controlled vocabularies to Tripal Terms...");
    tripal_chado_map_cvterms();

    if ($report_progress) {
      $job->setProgress(70);
    }

    drush_print("Creating common Tripal Content Types...");

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
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Organism" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(74);
    }

    // Create the 'Analysis' entity type. This uses the local:analysis term.
    $error = '';
    $args = array(
      'vocabulary' => 'operation',
      'accession' => '2945',
      'term_name' => 'Analysis',
      'storage_args' => array(
        'data_table' => 'analysis',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'operation', 'accession' => '2945'));
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Analysis" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(78);
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
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Project" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(82);
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
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Map" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    // Add cvterm mapping for the Map entity type
    $identifier = array(
      'cv_id' => array('name' => 'EDAM'),
      'name' => 'Map'
    );
    $cvterm = tripal_get_cvterm($identifier);
    tripal_chado_add_cvterm_mapping($cvterm->cvterm_id, 'featuremap', NULL);
    if ($report_progress) {
      $job->setProgress(86);
    }

    // Import a publication so we get all of the properties before
    // creating the content type.
    tripal_import_pub_by_dbxref('PMID:24163125');

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
    if ($term) {
        $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Publication" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    // Add cvterm mapping for the Publication entity type
    $identifier = array(
      'cv_id' => array('name' => 'tripal_pub'),
      'name' => 'Publication'
    );
    $cvterm = tripal_get_cvterm($identifier);
    tripal_chado_add_cvterm_mapping($cvterm->cvterm_id, 'pub', NULL);

    // Now remove the publication that was added above.
    $values = array(
      'dbxref_id' => array(
        'accession' => '24163125',
        'db_id' => array(
          'name' => 'PMID',
        ),
      ),
    );
    $result = chado_select_record('pub_dbxref', array('pub_id'), $values);
    chado_delete_record('pub', array('pub_id' => $result[0]->pub_id));
    if ($report_progress) {
      $job->setProgress(90);
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
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Gene" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(94);
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
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered  creating "mRNA" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(98);
    }

    // Create the 'biological sample' entity type.
    $error = '';
    $args = array(
      'vocabulary' => 'sep',
      'accession' => '00195',
      'term_name' => 'biological sample',
      'storage_args' => array(
        'data_table' => 'biomaterial',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'sep', 'accession' => '00195'));
    if ($term) {
        $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Biological Sample" Tripal Content Type.';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(99);
    }

    // Create the 'Phylogenetic tree' entity type.
    $error = '';
    $args = array(
      'vocabulary' => 'data',
      'accession' => '0872',
      'term_name' => 'Phylogenetic tree',
      'storage_args' => array(
        'data_table' => 'phylotree',
      )
    );
    $term = tripal_load_term_entity(array('vocabulary' => 'data', 'accession' => '0872'));
    if ($term) {
      $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        $msg = (isset($error['!message'])) ? $error['!message'] : 'Error Encountered creating "Phylogenetic tree" Tripal Content Type';
        throw new Exception($msg);
      }
    }
    if ($report_progress) {
      $job->setProgress(99);
    }

    // Add the supported loaders
    variable_set('tripal_pub_supported_dbs', array('PMID', 'AGL'));

    // Set a variable to indicate the site is prepared.
    variable_set('tripal_chado_is_prepared', TRUE);
  }
  catch (Exception $e) {
    $job->logMessage($e);
    throw new Exception($e);
  }
}

/**
 * For Chado v1.1 Tripal provides some new custom tables.
 *
 * For Chado v1.2 or greater these tables are not needed as they are part of the
 * schema update.
 */
function tripal_chado_add_v1_1_custom_tables(){
  module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.chado_v1_1');
  tripal_chado_add_analysisfeatureprop_table();
}

/**
 * For Chado v1.2 Tripal provides some new custom tables.
 *
 * For Chado v1.3 these tables are not needed as they are part of the
 * schema update.
 */
function tripal_chado_add_v1_2_custom_tables(){
  module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.chado_v1.2');
  tripal_chado_add_contactprop_table();
  tripal_chado_add_featuremap_dbxref_table();
  tripal_chado_add_featuremapprop_table();
  tripal_chado_add_featureposprop_table();
  tripal_chado_add_pubauthor_contact_table();
}

/**
 * Add custom tables for any version of Chado.
 *
 * These are tables that Chado uses to manage the site (i.e. temporary
 * loading tables) and not for primary data storage.
 */
function tripal_chado_add_vx_x_custom_tables(){
  module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.chado_vx_x');

  // Add in custom tables.
  tripal_chado_add_tripal_gff_temp_table();
  tripal_chado_add_tripal_gffcds_temp_table();
  tripal_chado_add_tripal_gffprotein_temp_table();
  tripal_chado_add_tripal_obo_temp_table();

  // Add in materialized views.
  tripal_chado_add_organism_stock_count_mview();
  tripal_chado_add_library_feature_count_mview();
  tripal_chado_add_organism_feature_count_mview();
  tripal_chado_add_analysis_organism_mview();
  tripal_chado_add_cv_root_mview_mview();
  tripal_chado_add_db2cv_mview_mview();

}

/**
 * Many of the custom tables created for Chado v1.2 are now in Chado v1.3.
 *
 * These tables need not be tracked by Tripal anymore as custom tables and
 * in some cases the Chado version has different columns so we need to
 * adjust them.
 */
function tripal_chado_fix_v1_3_custom_tables() {


  // Update the featuremap_dbxref table by adding an is_current field.
  if (!chado_column_exists('featuremap_dbxref', 'is_current')) {
    chado_query("ALTER TABLE {featuremap_dbxref} ADD COLUMN is_current boolean DEFAULT true NOT NULL,;");
  }

  // Remove the previously managed custom tables from the
  // tripal_custom_tables table.
  db_delete('tripal_custom_tables')
    ->condition('table_name', array('analysisfeatureprop', 'featuremap_dbxref', 'contactprop', 'featuremapprop', 'featureposprop', 'pubauthor_contact'))
    ->execute();
}
