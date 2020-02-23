<?php
/**
 * @file
 * Functions to install chado schema through Drupal
 */

/**
 * Prepares Chado for Tripal use
 */
function tripal_chado_prepare_form($form, $form_state) {
  $form = [];

  if (variable_get('tripal_chado_is_prepared') == TRUE) {
    drupal_set_message('Your site is prepared.');
  }
  $form['instructions'] = [
    '#type' => 'item',
    '#title' => 'Prepare Drupal for Chado.',
    '#description' => t("Before a Drupal site can use Chado (via Tripal), both
        Chado and Drupal must be prepared a bit more.  Tripal will add some new
        materialized views, custom tables and controlled vocabularies to Chado.
        It will also add some management tables to Drupal and add some default
        content types for biological and ancillary data."),
  ];

  $form['prepare-button'] = [
    '#type' => 'submit',
    '#value' => t('Prepare this site'),
    '#name' => 'prepare-chado',
  ];
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
    $args = [];
    $includes = [
      module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.setup'),
    ];
    tripal_add_job('Prepare Chado', 'tripal_chado',
      'tripal_chado_prepare_chado', $args,
      $user->uid, 10, $includes);
  }
}

/**
 * Submit function for the tripal_chado_prepare_form().
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_prepare_drush_submit() {
  $args = [];
  $includes = [
    module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.setup'),
  ];
  tripal_add_job('Prepare Chado', 'tripal_chado',
    'tripal_chado_prepare_chado', $args,
    1, 10, $includes);

}

/**
 *
 */
function tripal_chado_load_ontologies() {

  // Before we can load ontologies we need a few terms that unfortunately
  // don't get added until later. We'll add them now so the loader works.
  chado_insert_db([
    'name' => 'NCIT',
    'description' => 'NCI Thesaurus OBO Edition.',
    'url' => 'http://purl.obolibrary.org/obo/ncit.owl',
    'urlprefix' => ' http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'ncit',
    'The NCIt OBO Edition project aims to increase integration of the NCIt with OBO Library ontologies. NCIt is a reference terminology that includes broad coverage of the cancer domain, including cancer related diseases, findings and abnormalities. NCIt OBO Edition releases should be considered experimental.'
  );

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C25693',
    'name' => 'Subgroup',
    'cv_name' => 'ncit',
    'definition' => 'A subdivision of a larger group with members often exhibiting similar characteristics. [ NCI ]',
  ]);


  // Add the rdfs:comment vocabulary.
  chado_insert_db([
    'name' => 'rdfs',
    'description' => 'Resource Description Framework Schema',
    'url' => 'https://www.w3.org/TR/rdf-schema/',
    'urlprefix' => 'http://www.w3.org/2000/01/rdf-schema#{accession}',
  ]);
  chado_insert_cv(
    'rdfs',
    'Resource Description Framework Schema'
  );
  $name = chado_insert_cvterm([
    'id' => 'rdfs:comment',
    'name' => 'comment',
    'cv_name' => 'rdfs',
    'definition' => 'A human-readable description of a resource\'s name.',
  ]);

  // Insert commonly used ontologies into the tables.
  $ontologies = [
    [
      'name' => 'Relationship Ontology (legacy)',
      'path' => '{tripal_chado}/files/legacy_ro.obo',
      'auto_load' => FALSE,
      'cv_name' => 'ro',
      'db_name' => 'RO',
    ],
    [
      'name' => 'Gene Ontology',
      'path' => 'http://purl.obolibrary.org/obo/go.obo',
      'auto_load' => FALSE,
      'cv_name' => 'cellualar_component',
      'db_name' => 'GO',
    ],
    [
      'name' => 'Taxonomic Rank',
      'path' => 'http://purl.obolibrary.org/obo/taxrank.obo',
      'auto_load' => TRUE,
      'cv_name' => 'taxonomic_rank',
      'db_name' => 'TAXRANK',
    ],
    [
      'name' => 'Tripal Contact',
      'path' => '{tripal_chado}/files/tcontact.obo',
      'auto_load' => TRUE,
      'cv_name' => 'tripal_contact',
      'db_name' => 'TContact',
    ],
    [
      'name' => 'Tripal Publication',
      'path' => '{tripal_chado}/files/tpub.obo',
      'auto_load' => TRUE,
      'cv_name' => 'tripal_pub',
      'db_name' => 'TPUB',
    ],
    [
      'name' => 'Sequence Ontology',
      'path' => 'http://purl.obolibrary.org/obo/so.obo',
      'auto_load' => TRUE,
      'cv_name' => 'sequence',
      'db_name' => 'SO',
    ],

  ];

  module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter');
  for ($i = 0; $i < count($ontologies); $i++) {
    $obo_id = chado_insert_obo($ontologies[$i]['name'], $ontologies[$i]['path']);
    if ($ontologies[$i]['auto_load'] == TRUE) {
      // Only load ontologies that are not already in the cv table.
      $cv = chado_get_cv(['name' => $ontologies[$i]['cv_name']]);
      $db = chado_get_db(['name' => $ontologies[$i]['db_name']]);
      if (!$cv or !$db) {
        print "Loading ontology: " . $ontologies[$i]['name'] . " ($obo_id)...\n";
        $obo_importer = new OBOImporter();
        $obo_importer->create(['obo_id' => $obo_id]);
        $obo_importer->run();
        $obo_importer->postRun();
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

    drush_print('Populating materialized view cv_root_mview...');
    $mview_id = chado_get_mview_id('cv_root_mview');
    chado_populate_mview($mview_id);

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

    // Populate the mviews based on controlled vocabularies.
    drush_print('Populating materialized view db2cv_mview...');
    $mview_id = chado_get_mview_id('db2cv_mview');
    chado_populate_mview($mview_id);

    drush_print("Creating common Tripal Content Types...");
    drush_print("This may take awhile if you are upgrading a site that has lots of data...");
    if ($report_progress) {
      $job->setProgress(85);
    }
    tripal_chado_prepare_general_types($job);
    tripal_chado_prepare_genomic_types($job);
    tripal_chado_prepare_genetic_types($job);
    tripal_chado_prepare_germplasm_types($job);
    tripal_chado_prepare_expression_types($job);

    // Add the supported loaders
    variable_set('tripal_pub_supported_dbs', ['PMID', 'AGL']);

    // Set a variable to indicate the site is prepared.
    variable_set('tripal_chado_is_prepared', TRUE);

    if ($report_progress) {
      $job->setProgress(100);
    }
  } catch (Exception $e) {
    $job->logMessage($e);
    throw new Exception($e);
  }

  // Clear the Drupal menu cache so that the new content types have "add" links.
  menu_cache_clear_all();
}

/**
 * Creates the "General" category of content types.
 */
function tripal_chado_prepare_general_types($job) {

  //
  // Create the 'Organism' entity type. This uses the obi:organism term.
  //
  $args = [
    'vocabulary' => 'OBI',
    'accession' => '0100026',
    'term_name' => 'organism',
    'storage_args' => [
      'data_table' => 'organism',
    ],
    'category' => 'General',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Analysis' entity type. This uses the local:analysis term.
  //
  $args = [
    'vocabulary' => 'operation',
    'accession' => '2945',
    'term_name' => 'Analysis',
    'storage_args' => [
      'data_table' => 'analysis',
    ],
    'category' => 'General',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Project' entity type. This uses the local:project term.
  //
  $args = [
    'vocabulary' => 'NCIT',
    'accession' => 'C47885',
    'term_name' => 'Project',
    'storage_args' => [
      'data_table' => 'project',
    ],
    'category' => 'General',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);


  //
  // Create the 'Study' entity type. This uses the local:project term.
  //
  $args = [
    'vocabulary' => 'SIO',
    'accession' => '001066',
    'term_name' => 'study',
    'label' => 'Study',
    'storage_args' => [
      'data_table' => 'study',
    ],
    'category' => 'General',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Contact' entity type. This uses the local:contact term.
  //
  $args = [
    'vocabulary' => 'local',
    'accession' => 'contact',
    'term_name' => 'contact',
    'storage_args' => [
      'data_table' => 'contact',
    ],
    'category' => 'General',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Publication' entity type.
  //
  $args = [
    'vocabulary' => 'TPUB',
    'accession' => '0000002',
    'term_name' => 'Publication',
    'storage_args' => [
      'data_table' => 'pub',
    ],
    'category' => 'General',
  ];
  $bundle = tripal_load_bundle_entity(['accession' => $args['vocabulary'] . ':' . $args['accession']]);
  if (!$bundle) {
    // Import a publication so we get all of the properties before
    // creating the content type.
    chado_import_pub_by_dbxref('PMID:24163125');

    _tripal_chado_prepare_create_bundle($args, $job);

    // Now remove the publication that was added above.
    $values = [
      'dbxref_id' => [
        'accession' => '24163125',
        'db_id' => [
          'name' => 'PMID',
        ],
      ],
    ];
    $result = chado_select_record('pub_dbxref', ['pub_id'], $values);
    chado_delete_record('pub', ['pub_id' => $result[0]->pub_id]);
  }

  //
  // Create the 'Protocol' entity type.
  //
  $args = [
    'vocabulary' => 'sep',
    'accession' => '00101',
    'term_name' => 'Protocol',
    'storage_args' => [
      'data_table' => 'protocol',
    ],
    'category' => 'General',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);
}

/**
 * Creates the "Genomic" category of content types.
 */
function tripal_chado_prepare_genomic_types($job) {
  //
  // Create the 'Gene' entity type.
  //
  $args = [
    'vocabulary' => 'SO',
    'accession' => '0000704',
    'term_name' => 'gene',
    'storage_args' => [
      'data_table' => 'feature',
      'type_column' => 'type_id',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'mRNA' entity type.
  //
  $args = [
    'vocabulary' => 'SO',
    'accession' => '0000234',
    'term_name' => 'mRNA',
    'storage_args' => [
      'data_table' => 'feature',
      'type_column' => 'type_id',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Phylogenetic tree' entity type.
  //
  $args = [
    'vocabulary' => 'data',
    'accession' => '0872',
    'term_name' => 'Phylogenetic tree',
    'storage_args' => [
      'data_table' => 'phylotree',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  // Create the 'Physical Map' entity type.
  $cvterm = tripal_get_cvterm(['id' => 'rdfs:type']);
  $args = [
    'vocabulary' => 'data',
    'accession' => '1280',
    'term_name' => 'Physical Map',
    'storage_args' => [
      'data_table' => 'featuremap',
      'type_linker_table' => 'featuremapprop',
      'type_column' => 'type_id',
      'type_id' => $cvterm->cvterm_id,
      'type_value' => 'physical',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  // Create the 'DNA Library' entity type.
  $args = [
    'vocabulary' => 'NCIT',
    'accession' => 'C16223',
    'term_name' => 'DNA Library',
    'storage_args' => [
      'data_table' => 'library',
      'type_column' => 'type_id',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  // Create the 'Genome Assembly' entity type.
  $cvterm = tripal_get_cvterm(['id' => 'rdfs:type']);
  $args = [
    'vocabulary' => 'operation',
    'accession' => '0525',
    'term_name' => 'Genome assembly',
    'label' => 'Genome Assembly',
    'storage_args' => [
      'data_table' => 'analysis',
      'type_linker_table' => 'analysisprop',
      'type_column' => 'type_id',
      'type_id' => $cvterm->cvterm_id,
      'type_value' => 'genome_assembly',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  // Create the 'Genome Annotation' entity type.
  $cvterm = tripal_get_cvterm(['id' => 'rdfs:type']);
  $args = [
    'vocabulary' => 'operation',
    'accession' => '0362',
    'term_name' => 'Genome annotation',
    'label' => 'Genome Annotation',
    'storage_args' => [
      'data_table' => 'analysis',
      'type_linker_table' => 'analysisprop',
      'type_column' => 'type_id',
      'type_id' => $cvterm->cvterm_id,
      'type_value' => 'genome_annotation',
    ],
    'category' => 'Genomic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  // Create the 'Genome Annotation' entity type.
  $cvterm = tripal_get_cvterm(['id' => 'rdfs:type']);
  $args = [
    'vocabulary' => 'local',
    'accession' => 'Genome Project',
    'term_name' => 'Genome Project',
    'storage_args' => [
      'data_table' => 'project',
      'type_linker_table' => 'projectprop',
      'type_column' => 'type_id',
      'type_id' => $cvterm->cvterm_id,
      'type_value' => 'genome_project',
    ],
    'category' => 'Genomic',
  ];
  $bundle = tripal_load_bundle_entity(['accession' => $args['vocabulary'] . ':' . $args['accession']]);
  _tripal_chado_prepare_create_bundle($args, $job);
}

/**
 * Creates the "Expression" category of content types.
 */
function tripal_chado_prepare_expression_types($job) {
  //
  // Create the 'biological sample' entity type.
  //
  $args = [
    'vocabulary' => 'sep',
    'accession' => '00195',
    'term_name' => 'biological sample',
    'storage_args' => [
      'data_table' => 'biomaterial',
    ],
    'Expression',
  ];
  $bundle = tripal_load_bundle_entity(['accession' => $args['vocabulary'] . ':' . $args['accession']]);
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Assay' entity type.
  //
  $args = [
    'vocabulary' => 'OBI',
    'accession' => '0000070',
    'term_name' => 'assay',
    'label' => 'Assay',
    'storage_args' => [
      'data_table' => 'assay',
    ],
    'Expression',
  ];
  $bundle = tripal_load_bundle_entity(['accession' => $args['vocabulary'] . ':' . $args['accession']]);
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Array Design' entity type.
  //
  $args = [
    'vocabulary' => 'EFO',
    'accession' => '0000269',
    'term_name' => 'Assay Design',
    'storage_args' => [
      'data_table' => 'arraydesign',
    ],
    'Expression',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);
}

/**
 * Creates the "Germplasm/Breeding" category of content types.
 */
function tripal_chado_prepare_germplasm_types($job) {

  //
  // Create the 'Phenotypic Trait' entity type.
  //
  /**
   * SPF:  We need a bit more testing before we add this conteont type as
   * it resolves to the cvterm table. Currently, it can't be created.
   * $args = array(
   * 'vocabulary' => 'NCIT',
   * 'accession' => 'C85496',
   * 'term_name' => 'Phenotypic Trait',
   * 'storage_args' => array(
   * 'data_table' => 'cvterm',
   * 'type_column' => 'type_id',
   * ),
   * 'category' => 'Germplasm/Breeding',
   * );
   * _tripal_chado_prepare_create_bundle($args, $job);
   */

  //
  // Create the 'Germplasm Accession' entity type.
  //
  $args = [
    'vocabulary' => 'CO_010',
    'accession' => '0000044',
    'term_name' => 'accession',
    'label' => 'Germplasm Accession',
    'storage_args' => [
      'data_table' => 'stock',
      'type_column' => 'type_id',
    ],
    'category' => 'Germplasm/Breeding',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Breeding Cross' entity type.
  //
  $args = [
    'vocabulary' => 'CO_010',
    'accession' => '0000255',
    'label' => 'Generated Germplasm (Breeding Cross)',
    'term_name' => 'generated germplasm',
    'storage_args' => [
      'data_table' => 'stock',
      'type_column' => 'type_id',
    ],
    'category' => 'Germplasm/Breeding',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Germplasm Variety' entity type.
  //
  $args = [
    'vocabulary' => 'CO_010',
    'accession' => '0000029',
    'label' => 'Cultivar (Germplasm Variety)',
    'term_name' => 'cultivar',
    'storage_args' => [
      'data_table' => 'stock',
      'type_column' => 'type_id',
    ],
    'category' => 'Germplasm/Breeding',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Germplasm Variety' entity type.
  //
  $args = [
    'vocabulary' => 'CO_010',
    'accession' => '0000162',
    'label' => 'Recombinant Inbred Line',
    'term_name' => '414 inbred line',
    'storage_args' => [
      'data_table' => 'stock',
      'type_column' => 'type_id',
    ],
    'category' => 'Germplasm/Breeding',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);
}

/**
 * Creates the "Genetic" category of content types.
 */
function tripal_chado_prepare_genetic_types($job) {

  //
  // Create the 'Genetic Map' entity type.
  //
  $cvterm = tripal_get_cvterm(['id' => 'rdfs:type']);
  $args = [
    'vocabulary' => 'data',
    'accession' => '1278',
    'label' => 'Genetic Map',
    'term_name' => 'Genetic map',
    'storage_args' => [
      'data_table' => 'featuremap',
      'type_linker_table' => 'featuremapprop',
      'type_column' => 'type_id',
      'type_id' => $cvterm->cvterm_id,
      'type_value' => 'genetic',
    ],
    'category' => 'Genetic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'QTL' entity type.
  //
  $args = [
    'vocabulary' => 'SO',
    'accession' => '0000771',
    'term_name' => 'QTL',
    'storage_args' => [
      'data_table' => 'feature',
      'type_column' => 'type_id',
    ],
    'category' => 'Genetic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Sequence Variant' entity type.
  //
  $args = [
    'vocabulary' => 'SO',
    'accession' => '0001060',
    'label' => "Sequence Variant",
    'term_name' => 'sequence_variant',
    'storage_args' => [
      'data_table' => 'feature',
      'type_column' => 'type_id',
    ],
    'category' => 'Genetic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);

  //
  // Create the 'Genetic Marker' entity type.
  //
  $args = [
    'vocabulary' => 'SO',
    'accession' => '0001645',
    'term_name' => 'genetic_marker',
    'label' => "Genetic Marker",
    'storage_args' => [
      'data_table' => 'feature',
      'type_column' => 'type_id',
    ],
    'category' => 'Genetic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);


  //
  // Create the 'Heritable Phenotypic Marker' entity type.
  //
  $args = [
    'vocabulary' => 'SO',
    'accession' => '0001500',
    'term_name' => 'heritable_phenotypic_marker',
    'label' => "Heritable Phenotypic Marker",

    'storage_args' => [
      'data_table' => 'feature',
      'type_column' => 'type_id',
    ],
    'category' => 'Genetic',
  ];
  _tripal_chado_prepare_create_bundle($args, $job);
}

/**
 * A helper function to consolidate the  code used to create a bundle.
 */
function _tripal_chado_prepare_create_bundle($args, $job) {

  $bundle = tripal_load_bundle_entity(['accession' => $args['vocabulary'] . ':' . $args['accession']]);
  if (!$bundle) {
    drush_print("Creating " . $args['term_name'] . "...");
    if (!tripal_create_bundle($args, $job)) {
      $msg = t('Error encountered creating !type Content Type.', ['!type' => $args['term_name']]);
      throw new Exception($msg);
    }
  }
  else {

    // Update the bundle category in case it was set incorrectly.
    $category = array_key_exists('category', $args) ? $args['category'] : 'Other';
    tripal_set_bundle_variable('bundle_category', $bundle->id, $category);

    drush_print("Content type already created (skipping): " . $args['term_name'] . "...");
  }
}

/**
 * For Chado v1.1 Tripal provides some new custom tables.
 *
 * For Chado v1.2 or greater these tables are not needed as they are part of the
 * schema update.
 */
function tripal_chado_add_v1_1_custom_tables() {
  module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.chado_v1_1');
  tripal_chado_add_analysisfeatureprop_table();
}

/**
 * For Chado v1.2 Tripal provides some new custom tables.
 *
 * For Chado v1.3 these tables are not needed as they are part of the
 * schema update.
 */
function tripal_chado_add_v1_2_custom_tables() {
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
function tripal_chado_add_vx_x_custom_tables() {
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
    chado_query("ALTER TABLE {featuremap_dbxref} ADD COLUMN is_current boolean DEFAULT true NOT NULL;");
  }

  // Remove the previously managed custom tables from the
  // tripal_custom_tables table.
  db_delete('tripal_custom_tables')
    ->condition('table_name', [
      'analysisfeatureprop',
      'featuremap_dbxref',
      'contactprop',
      'featuremapprop',
      'featureposprop',
      'pubauthor_contact',
    ])
    ->execute();
}
