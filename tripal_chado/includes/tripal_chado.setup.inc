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
 * Prepares Chado for use by Tripal.
 */
function tripal_chado_prepare_chado() {

  try {
    // we want to force the version of Chado to be set properly
    $real_version = chado_get_version(TRUE);

    // get the effective version.  Pass true as second argument
    // to warn the user if the current version is not compatible
    $version = chado_get_version(FALSE, FALSE);


    ///////////////////////////////////////////////////////////////////////////
    //                          Chado DB Module
    ///////////////////////////////////////////////////////////////////////////
    tripal_insert_db(array(
      'name' => 'null',
      'description' => 'No online database.'
    ));
    tripal_insert_db(array(
      'name' => 'local',
      'description' => variable_get('site_name', 'This site.'),
    ));

    ///////////////////////////////////////////////////////////////////////////
    //                          Chado CV Module
    ///////////////////////////////////////////////////////////////////////////
    // Add the cv_root_mview.
    tripal_cv_add_cv_root_mview();

    // Add defaults to the tables that correlate OBO files/references with
    // a chado CV.
    tripal_cv_add_obo_defaults();

    // Create the temp table we will use for loading OBO files.
    tripal_cv_create_tripal_obo_temp();

    // Add the synonym_type vocabulary.
    tripal_insert_cv(
      'synonym_type',
      'A vocabulary used for synonym types (e.g. exact, broad, narrow, related).'
    );

    // Add the Chado ontology CV.
    //$obo_path = '{tripal_chado}/files/cv_property.obo';
    //$obo_id = tripal_insert_obo('Chado CV Properties', $obo_path);
    //tripal_chado_load_obo_v1_2_id($obo_id);
    //tripal_submit_obo_job(array('obo_id' => $obo_id));


    ///////////////////////////////////////////////////////////////////////////
    //                          Chado Misc
    ///////////////////////////////////////////////////////////////////////////
    tripal_insert_cv(
      'synonym_type',
      'A vocabulary for storing synonym types.'
    );
    // set the default vocabularies
    tripal_set_default_cv('synonym', 'type_id', 'synonym_type');

    // Add cvterms.
    tripal_insert_misc_cvterms();


    /////////////////////////////////////////////////////////////////////////////
    //                          Chado Organism Module
    /////////////////////////////////////////////////////////////////////////////
    tripal_insert_cv(
      'organism_property',
      'Contains properties for organisms'
    );

    // Set the default vocabularies.
    tripal_set_default_cv('organismprop', 'type_id', 'organism_property');

    // Add taxonomic terms.
    $obo_id = tripal_insert_obo('Taxonomic Rank', '{tripal_chado}/files/taxrank.obo');
    $cv = chado_select_record('cv', array('name'), array('name' => 'taxonomic_rank'));
    if (count($cv) == 0 and $obo_id) {
      tripal_chado_load_obo_v1_2_id($obo_id);
    }

    /////////////////////////////////////////////////////////////////////////////
    //                        Chado CompAnalysis Module
    /////////////////////////////////////////////////////////////////////////////
    // we may need the analysisfeatureprop table if it doesn't already exist
    tripal_analysis_create_analysisfeatureprop();

    // add cvterms
    tripal_analysis_add_cvterms();

    // add materialized views
    tripal_analysis_add_mview_analysis_organism();

    // set the default vocabularies
    tripal_set_default_cv('analysisprop', 'type_id', 'analysis_property');

    /////////////////////////////////////////////////////////////////////////////
    //                        Chado Contact Module
    /////////////////////////////////////////////////////////////////////////////
    // Add the contactprop table to Chado.
    tripal_contact_add_custom_tables();

    tripal_insert_db(array(
      'name' => 'TContact',
      'description' => 'The Tripal Contact controlled vocabulary.',
    ));
    tripal_insert_db(array(
      'name' => 'TPUB',
      'description' => 'The Tripal Publication controlled vocabulary.',
    ));

    // Add loading of the the tripal contact ontology to the job queue.
    $obo_path = '{tripal_chado}/files/tcontact.obo';
    $obo_id = tripal_insert_obo('Tripal Contacts', $obo_path);
    $cv = chado_select_record('cv', array('name'), array('name' => 'tripal_contact'));
    if (count($cv) == 0 and $obo) {
      tripal_chado_load_obo_v1_2_id($obo_id);
    }
    //tripal_submit_obo_job(array('obo_id' => $obo_id));

      // Add cvterms for relationship types.
    tripal_contact_add_cvs();

    // Set the default vocabularies.
    tripal_set_default_cv('contact', 'type_id', 'tripal_contact');
    tripal_set_default_cv('contactprop', 'type_id', 'tripal_contact');
    tripal_set_default_cv('contact_relationship', 'type_id', 'contact_relationship');

    /////////////////////////////////////////////////////////////////////////////
    //                        Chado Feature Module
    /////////////////////////////////////////////////////////////////////////////
    // Note: the feature_property OBO that came with Chado v1.2 should not
    // be automatically installed.  Some of the terms are duplicates of
    // others in better maintained vocabularies.  New Tripal sites should
    // use those.
    // $obo_path = '{tripal_feature}/files/feature_property.obo';
    // $obo_id = tripal_insert_obo('Chado Feature Properties', $obo_path);
    // tripal_chado_load_obo_v1_2_id($obo_id);
    //// tripal_submit_obo_job(array('obo_id' => $obo_id));

    // Add the vocabularies used by the feature module.
    tripal_feature_add_cvs();

    // Add the materialized view.
    tripal_feature_add_organism_count_mview();

    // Add the custom tables.
    tripal_feature_add_tripal_gff_temp_table();
    tripal_feature_add_tripal_gffcds_temp_table();
    tripal_feature_add_tripal_gffprotein_temp_table();

    // Load the sequence ontology if it isn't already loaded
    $cv = chado_select_record('cv', array('name'), array('name' => 'sequence'));
    $obo = tripal_get_obo(array('obo_name' => 'Sequence Ontology'));
    if (count($cv) == 0 and $obo) {
      print "Installing the Sequence Ontology... This may take a while...\n";
      tripal_chado_load_obo_v1_2_id($obo->obo_id);
    }

    // Set the default vocabularies.
    tripal_set_default_cv('feature', 'type_id', 'sequence');
    tripal_set_default_cv('featureprop', 'type_id', 'feature_property');
    tripal_set_default_cv('feature_relationship', 'type_id', 'feature_relationship');

    /////////////////////////////////////////////////////////////////////////////
    //                            Chado Map Module
    /////////////////////////////////////////////////////////////////////////////
    // add the featuremapprop table to Chado
    tripal_featuremap_add_custom_tables();

    // Add cvterms
    tripal_featuremap_add_cvs();
    tripal_featuremap_add_cvterms();

    // set the default vocabularies
    tripal_set_default_cv('featuremapprop', 'type_id', 'featuremap_property');
    tripal_set_default_cv('featureposprop', 'type_id', 'featurepos_property');
    tripal_set_default_cv('featuremap', 'unittype_id', 'featuremap_units');

    /////////////////////////////////////////////////////////////////////////////
    //                          Chado Library Module
    /////////////////////////////////////////////////////////////////////////////
    // add the materialized view
    tripal_library_add_mview_library_feature_count();

    // add cvterms
    tripal_library_add_cvs();
    tripal_library_add_cvterms();

    // set the default vocabularies
    tripal_set_default_cv('libraryprop', 'type_id', 'library_property');
    tripal_set_default_cv('library', 'type_id', 'library_type');

    /////////////////////////////////////////////////////////////////////////////
    //                          Chado NatDiv Module
    /////////////////////////////////////////////////////////////////////////////
    // add cvterms
    tripal_natural_diversity_add_cvterms();

    /////////////////////////////////////////////////////////////////////////////
    //                          Chado Project Module
    /////////////////////////////////////////////////////////////////////////////
    tripal_project_add_cvs();
    tripal_project_add_cvterms();

    // set the default vocabularies
    tripal_set_default_cv('projectprop', 'type_id', 'project_property');
    tripal_set_default_cv('project_relationship', 'type_id', 'project_relationship');

    /////////////////////////////////////////////////////////////////////////////
    //                          Chado Pub Module
    /////////////////////////////////////////////////////////////////////////////
    global $base_path;

    // add loading of the the tripal pub ontology to the job queue
    $obo_path = '{tripal_chado}/files/tpub.obo';
    $obo_id = tripal_insert_obo('Tripal Publication', $obo_path);
    $cv = chado_select_record('cv', array('name'), array('name' => 'tripal_pub'));
    if (count($cv) == 0 and $obo) {
      tripal_chado_load_obo_v1_2_id($obo_id);
    }

    tripal_pub_add_cvs();
    tripal_pub_add_dbs();

    // add the custom tables
    tripal_pub_add_custom_tables();

    // set the default vocabularies
    tripal_set_default_cv('pub', 'type_id', 'tripal_pub');
    tripal_set_default_cv('pubprop', 'type_id', 'tripal_pub');
    tripal_set_default_cv('pub_relationship', 'type_id', 'pub_relationship');

    // Add the supported loaders
    variable_set('tripal_pub_supported_dbs', array('PMID', 'AGL'));

    /////////////////////////////////////////////////////////////////////////////
      //                          Chado Stock Module
      /////////////////////////////////////////////////////////////////////////////
    // add some controlled vocabularies
    tripal_stock_add_cvs();

    // set the default vocabularies
    tripal_set_default_cv('stock', 'type_id', 'stock_type');
    tripal_set_default_cv('stockprop', 'type_id', 'stock_property');
    tripal_set_default_cv('stock_relationship', 'type_id', 'stock_relationship');

    // add the materialized view
    tripal_stock_add_organism_count_mview();

    /////////////////////////////////////////////////////////////////////////////
    //                              Entity Bundles
    /////////////////////////////////////////////////////////////////////////////

    // Unfortunately, some Chado base tables do not have a type_id, so we must
    // take special action for those tables.  These include: organism and
    // analysis. Until we can find an appropriate controlled vocabulary
    // that is well supported by the community with types for these tables we
    // will have to use in-house terms.

    // Add a term to be used for an inherent 'type_id' for the organism table.
    tripal_insert_cvterm(array(
      'id' => 'local:organism',
      'name' => 'organism',
      'definition' => 'An individual form of life, such as a bacterium, protist, ' .
        'fungus, plant, or animal, composed of a single cell or a complex of cells  ' .
        'in which organelles or organs work together to carry out the various  ' .
        'processes of life. (American Heritage® Dictionary of the English ' .
        'Language, Fifth Edition. Copyright © 2011 by Houghton Mifflin ' .
        'Harcourt Publishing Company).',
      'cv_name' => 'local',
    ));

    // Add a term to be used for an inherent 'type_id' for the organism table.
    tripal_insert_cvterm(array(
      'id' => 'local:analysis',
      'name' => 'analysis',
      'definition' => 'A process as a method of studying the nature of something ' .
        'or of determining its essential features and their relations. ' .
        '(Random House Kernerman Webster\'s College Dictionary, © 2010 K ' .
        'Dictionaries Ltd).',
      'cv_name' => 'local',
    ));

    // TODO: change this to foaf:Project
    tripal_insert_cvterm(array(
      'id' => 'local:project',
      'name' => 'project',
      'definition' => 'A plan or proposal for accomplishing something. ' .
        '(American Heritage® Dictionary of the English Language, Fifth Edition. ' .
        'Copyright © 2011 by Houghton Mifflin Harcourt Publishing Company).',
      'cv_name' => 'local',
    ));

    tripal_insert_cvterm(array(
      'id' => 'local:contact',
      'name' => 'contact',
      'definition' => 'An entity (e.g. individual or organization) through ' .
         'whom a person can gain access to information, favors, ' .
         'influential people, and the like.',
      'cv_name' => 'local',
    ));

    tripal_insert_cvterm(array(
      'id' => 'local:md5_checksum',
      'name' => 'md5_checksum',
      'definition' => 'The MD5 message-digest algorithm is a widely used ' .
         'cryptographic hash function producing a 128-bit (16-byte) hash ' .
         'value, typically expressed in text format as a 32-digit hexadecimal ' .
         'number. MD5 has been utilized in a wide variety of cryptographic ' .
         'applications and is also commonly used to verify data integrity (Wikipedia)',
      'cv_name' => 'local',
    ));

    tripal_insert_cvterm(array(
      'id' => 'local:relationship',
      'name' => 'relationship',
      'definition' => 'The way in which two things are connected.',
      'cv_name' => 'local',
    ));

    // For the TripalBundle entities we will want to associate the cvterm_id,
    // and the chado table and field that it maps to.  We will use a few
    // variables to do this:
    tripal_insert_variable(
      'chado_cvterm_id',
      'The cvterm_id that a TripalBundle maps to.'
    );
    tripal_insert_variable(
      'chado_table',
      'The name of the table to which a TripalBundle maps.'
    );
    tripal_insert_variable(
      'chado_column',
      'The name of the column within the table that a TripalBundle maps to.'
    );

    // We want to provide a set of commonly used entity types by default. This
    // way when a user first installs Tripal there are some commonly used
    // formats.
    module_load_include('inc', 'tripal', 'api/tripal.api');
    module_load_include('inc', 'tripal', 'includes/tripal.admin');

    // Create the 'Organism' entity type. This uses the local:organism term.
    $error = '';
    $term = array('name' => 'organism', 'cv_id' => array('name' => 'local'));
    $cvterm = chado_generate_var('cvterm', $term);
    if (!tripal_create_bundle('local', 'organism', 'organism', $error)) {
      throw new Exception($error);
    }

    // Create the 'Analysis' entity type. This uses the local:analysis term.
    $error = '';
    $term = array('name' => 'analysis', 'cv_id' => array('name' => 'local'));
    $cvterm = chado_generate_var('cvterm', $term);
    if (!tripal_create_bundle('local', 'analysis', 'analysis', $error)) {
      throw new Exception($error);
    }

    // Create the 'Project' entity type. This uses the local:project term.
    $error = '';
    $term = array('name' => 'project', 'cv_id' => array('name' => 'local'));
    $cvterm = chado_generate_var('cvterm', $term);
    if (!tripal_create_bundle('local', 'project', 'project', $error)) {
      throw new Exception($error);
    }

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
function tripal_stock_add_organism_count_mview() {
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
 * Add cvs related to publications
 *
 * @ingroup tripal_pub
 */
function tripal_stock_add_cvs() {

  // Add cv for relationship types
  tripal_insert_cv(
      'stock_relationship',
      'Contains types of relationships between stocks.'
  );
  tripal_insert_cv(
      'stock_property',
      'Contains properties for stocks.'
  );
  tripal_insert_cv(
      'stock_type',
      'Contains a list of types for stocks.'
  );
}

/**
 * Add custom table related to publications
 *  - pubauthor_contact
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_custom_tables() {
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
 * Adds dbs related to publications
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_dbs() {
  // make sure we have our supported databases
  tripal_insert_db(
      array(
        'name' => 'PMID',
        'description' => 'PubMed',
        'url' => 'http://www.ncbi.nlm.nih.gov/pubmed',
        'urlprefix' => 'http://www.ncbi.nlm.nih.gov/pubmed/'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_db(
      array(
        'name' => 'AGL',
        'description' => 'USDA National Agricultural Library',
        'url' => 'http://agricola.nal.usda.gov/'
      ),
      array('update_existing' => TRUE)
  );
}
/**
 * Add cvs related to publications
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_cvs() {


  // Add the cv for pub properties
  tripal_insert_cv(
      'tripal_pub',
      'A heirarchical set of terms for describing a publication. It is intended to be used as the default vocabularies in Tripal for publication types and contact properties.'
  );

  // Add the cv for pub types
  tripal_insert_cv(
      'pub_type',
      'Contains types of publications. This can be used if the tripal_pub vocabulary (which is default for publications in Tripal) is not desired.'
  );

  // Add the cv for pub properties
  tripal_insert_cv(
      'pub_property',
      'Contains properties for publications. This can be used if the tripal_pub vocabulary (which is default for publications in Tripal) is not desired.'
  );

  // Add cv for relationship types
  tripal_insert_cv(
      'pub_relationship',
      'Contains types of relationships between publications.'
  );
}
/**
 * Add cvs pertaining to projects
 *
 * @ingroup tripal_project
 */
function tripal_project_add_cvs() {
  // Add the cv for project properties
  tripal_insert_cv(
      'project_property',
      'Contains properties for projects'
  );

  // Add cv for relationship types
  tripal_insert_cv(
      'project_relationship',
      'Contains Types of relationships between projects.'
  );
}

/**
 * Add cvterms pertaining to projects
 *
 * @ingroup tripal_project
 */
function tripal_project_add_cvterms() {

  // Insert cvterm 'Project Description' into cvterm table of chado
  // database. This CV term is used to keep track of the project
  // description in the projectprop table.
  tripal_insert_cvterm(
      array(
        'name' => 'Project Description',
        'definition'  => 'Description of a project',
        'cv_name' => 'project_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
}
/**
 * Add cvterms related to natural diversity
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_add_cvterms(){

  // add cvterms for the nd_experiment_types
  tripal_insert_cvterm(
      array(
        'name' => 'Genotyping',
        'definition' => 'An experiment where genotypes of individuals are identified.',
        'cv_name' => 'nd_experiment_types',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

  tripal_insert_cvterm(
      array(
        'name' => 'Phenotyping',
        'definition' => 'An experiment where phenotypes of individuals are identified.',
        'cv_name' => 'nd_experiment_types',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

  tripal_insert_cvterm(
      array(
        'name' => 'Location',
        'definition' => 'The name of the location.',
        'cv_name' => 'nd_geolocation_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
}
/**
 * Adds a materialized view keeping track of the type of features associated with each library
 *
 * @ingroup tripal_library
 */
function tripal_library_add_mview_library_feature_count(){
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
 * Adds cvterms needed for the library module
 *
 * @ingroup tripal_library
 */
function tripal_library_add_cvterms() {

  // Insert cvterm 'library_description' into cvterm table of chado
  // database. This CV term is used to keep track of the library
  // description in the libraryprop table.
  tripal_insert_cvterm(
      array(
        'id' => 'local:library_description',
        'name' => 'Library Description',
        'definition' => 'Description of a library',
        'cv_name' => 'library_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

  // add cvterms for the map unit types
  tripal_insert_cvterm(
      array(
        'id' => 'local:cdna_library',
        'name' => 'cdna_library',
        'definition' => 'cDNA library',
        'cv_name' => 'library_type',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'id' => 'local:bac_library',
        'name' => 'bac_library',
        'definition' => 'Bacterial Artifical Chromsome (BAC) library',
        'cv_name' => 'library_type',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'id' => 'local:fosmid_library',
        'name' => 'fosmid_library',
        'definition' => 'Fosmid library',
        'cv_name' => 'library_type',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'id' => 'local:cosmid_library',
        'name' => 'cosmid_library',
        'definition' => 'Cosmid library',
        'cv_name' => 'library_type',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'id' => 'local:yac_library',
        'name' => 'yac_library',
        'definition' => 'Yeast Artificial Chromosome (YAC) library',
        'cv_name' => 'library_type',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'id' => 'local:genomic_library',
        'name' => 'genomic_library',
        'definition' => 'Genomic Library',
        'cv_name' => 'library_type',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
}
/**
 * Adds new CV's used by this module
 *
 * @ingroup tripal_library
 */
function tripal_library_add_cvs(){
  tripal_insert_cv(
      'library_property',
      'Contains properties for libraries.'
  );
  tripal_insert_cv(
      'library_type',
      'Contains terms for types of libraries (e.g. BAC, cDNA, FOSMID, etc).'
  );
}
/**
 * Add custom tables needed by the feature map module
 *  - featuremapprop
 *  - featuremap_dbxref
 *  - featureposprop
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_add_custom_tables(){
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
 * Add cv terms needed by the featuremap module
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_add_cvterms() {

  // add cvterms for the map unit types
  tripal_insert_cvterm(
      array(
        'name' => 'cM',
        'definition' => 'Centimorgan units',
        'cv_name' => 'featuremap_units',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'bp',
        'definition' => 'Base pairs units',
        'cv_name' => 'featuremap_units',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'bin_unit',
        'definition' => 'The bin unit',
        'cv_name' => 'featuremap_units',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'marker_order',
        'definition' => 'Units simply to define marker order.',
        'cv_name' => 'featuremap_units',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'undefined',
        'definition' => 'A catch-all for an undefined unit type',
        'cv_name' => 'featuremap_units',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

  // featurepos properties
  tripal_insert_cvterm(
      array(
        'name' => 'start',
        'definition' => 'The start coordinate for a map feature.',
        'cv_name' => 'featurepos_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'stop',
        'definition' => 'The end coordinate for a map feature',
        'cv_name' => 'featurepos_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

  // add cvterms for map properties
  tripal_insert_cvterm(
      array(
        'name' => 'Map Dbxref',
        'definition' => 'A unique identifer for the map in a remote database.  The '
        . 'format is a database abbreviation and a unique accession separated '
        . 'by a colon.  (e.g. Gramene:tsh1996a)',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'Map Type',
        'definition' => 'The type of Map (e.g. QTL, Physical, etc.)',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'Genome Group',
        'definition' => '',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'URL',
        'definition' => 'A univeral resource locator (URL) reference where the '
        . 'publication can be found.  For maps found online, this would be '
        . 'the web address for the map.',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'Population Type',
        'definition' => 'A brief description of the population type used to generate '
        . 'the map (e.g. RIL, F2, BC1, etc).',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'Population Size',
        'definition' => 'The size of the population used to construct the map.',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'Methods',
        'definition' => 'A brief description of the methods used to construct the map.',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
      array(
        'name' => 'Software',
        'definition' => 'The software used to construct the map.',
        'cv_name' => 'featuremap_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

}
/**
 * Add cvs needed by the featuremap module
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_add_cvs() {

  tripal_insert_cv(
      'featuremap_units',
      'Contains map unit types for the unittype_id column of the featuremap table.'
  );

  tripal_insert_cv(
      'featurepos_property',
      'Contains terms map properties.'
  );

  tripal_insert_cv(
      'featuremap_property',
      'Contains positional types for the feature positions'
  );
}
/**
 * Add cvs related to features
 *
 * @ingroup tripal_pub
 */
function tripal_feature_add_cvs() {

  // Add cv for relationship types
  tripal_insert_cv(
      'feature_relationship',
      'Contains types of relationships between features.'
  );

  // The feature_property CV may already exists. It comes with Chado, but
  // we need to  add it just in case it doesn't get added before the feature
  // module is installed. But as of Tripal v3.0 the Chado version of this
  // vocabulary is no longer loaded by default.
  tripal_insert_cv(
      'feature_property',
      'Stores properties about features'
  );

  // the feature type vocabulary should be the sequence ontology, and even though
  // this ontology should get loaded we will create it here just so that we can
  // set the default vocabulary for the feature.type_id field
  tripal_insert_cv(
      'sequence',
      'The Sequence Ontology'
  );
}
/**
 *
 */
function tripal_feature_add_tripal_gff_temp_table() {
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
function tripal_feature_add_tripal_gffcds_temp_table($skip_recreate = TRUE) {
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
function tripal_feature_add_tripal_gffprotein_temp_table() {
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
function tripal_feature_add_organism_count_mview() {
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
 * Adds any cvs needed by this module.
 *
 * @ingroup tripal_contact
 */
function tripal_contact_add_cvs() {

  // Add the cv for contact properties. This is a default vocabulary in the event
  // that a user does not want to use the tripal_contact vocabulary
  tripal_insert_cv(
    'contact_property',
    'Contains properties for contacts. This can be used if the tripal_contact vocabulary (which is default for contacts in Tripal) is not desired.'
  );

  // add the cv for the contact type. This is a default vocabulary in the event
  // that a user does not want to use the tripal_contact vocabulary
  tripal_insert_cv(
    'contact_type',
    'Contains types of contacts. This can be used if the tripal_contact vocabulary (which is default for contacts in Tripal) is not desired.'
  );

  // Add the cv for the tripal_contact vocabulary which is loaded via the OBO
  tripal_insert_cv(
    'tripal_contact',
    'A heirarchical set of terms for describing a contact. It is intended to be used as the default vocabularies in Tripal for contact types and contact properties.'
  );

  // add the cv for contact relationships
  tripal_insert_cv(
    'contact_relationship',
    'Contains types of relationships between contacts.'
  );
}
/**
 * Add any custom tables needed by this module.
 * - Contactprop: keep track of properties of contact
 *
 * @ingroup tripal_contact
 */
function tripal_contact_add_custom_tables(){
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
function tripal_analysis_create_analysisfeatureprop() {

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
function tripal_analysis_add_mview_analysis_organism() {
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
 * Adds controlled vocabulary terms needed by this module.
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_add_cvterms() {

  tripal_insert_cv(
      'tripal_analysis',
      'Terms used for managing analyses in Tripal'
  );

  // add analysis_date.  This is no longer used (as far as we can tell) but we don't
  // get rid of it in case it is used, so just keep it in the Tripal CV
  tripal_insert_cvterm(
      array(
        'name' => 'analysis_date',
        'definition' => 'The date that an analysis was performed.',
        'cv_name' => 'tripal_analysis',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );

  // add analysis_short_name.  This is no longer used (as far as we can tell) but we don't
  // get rid of it in case it is used, so just keep it in the Tripal CV
  tripal_insert_cvterm(
      array(
        'name' => 'analysis_short_name',
        'definition' => 'A computer legible (no spaces or special characters) '
        . 'abbreviation for the analysis.',
        'cv_name' => 'tripal_analysis',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );


  // the 'analysis_property' vocabulary is for user definable properties wo we
  // will add an 'Analysis Type' to this vocubulary
  tripal_insert_cvterm(
      array(
        'name' => 'Analysis Type',
        'definition' => 'The type of analysis that was performed.',
        'cv_name' => 'analysis_property',
        'is_relationship' => 0,
        'db_name' => 'local'
      ),
      array('update_existing' => TRUE)
  );
}
/**
 * Add's defaults to the tripal_cv_obo table
 *
 * @ingroup tripal_cv
 */
function tripal_cv_add_obo_defaults() {

  // Insert commonly used ontologies into the tables.
  $ontologies = array(
    // array('Relationship Ontology', 'http://purl.obolibrary.org/obo/ro.obo'),
    // array('Relationship Ontology (older deprecated version)', 'http://www.obofoundry.org/ro/ro.obo'),
    array('Sequence Ontology', 'https://raw.githubusercontent.com/The-Sequence-Ontology/SO-Ontologies/master/so-xp-simple.obo'),
    array('Gene Ontology', 'http://geneontology.org/ontology/go.obo'),
    //    array('Cell Ontology', 'https://raw.githubusercontent.com/obophenotype/cell-ontology/master/cl.obo'),
    //    array('Plant Structure Ontology', 'http://palea.cgrb.oregonstate.edu/viewsvn/Poc/trunk/ontology/OBO_format/po_anatomy.obo?view=co'),
    //    array('Plant Growth and Development Stages Ontology', 'http://palea.cgrb.oregonstate.edu/viewsvn/Poc/trunk/ontology/OBO_format/po_temporal.obo?view=co')
  );
  foreach ($ontologies as $o) {
    db_query("INSERT INTO {tripal_cv_obo} (name,path) VALUES (:name, :path)", array(':name' => $o[0], ':path' => $o[1]));
  }
}
/**
 * Add a materialized view of root terms for all chado cvs. This is needed for viewing cv trees
 *
 * @ingroup tripal_cv
 */
function tripal_cv_add_cv_root_mview() {
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
function tripal_cv_create_tripal_obo_temp() {
  // the tripal_obo_temp table is used for temporary housing of records when loading OBO files
  // we create it here using plain SQL because we want it to be in the chado schema but we
  // do not want to use the Tripal Custom Table API because we don't want it to appear in the
  // list of custom tables.  It needs to be available for the Tripal Chado API so we create it
  // here and then define it in the tripal_cv/api/tripal_cv.schema.api.inc
  if (!db_table_exists('chado.tripal_obo_temp')) {
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

/**
 * Adds generic CVterms.
 */
function tripal_insert_misc_cvterms() {
  tripal_insert_cvterm(
    array(
      'name' => 'exact',
      'id' => "local:exact",
      'definition' => 'An exact equivalent; interchangeable with the term name.',
      'is_obsolete' => 0,
      'cv_name' => 'synonym_type',
      'is_relationship' => FALSE
    ),
    array('update_existing' => TRUE)
  );

  tripal_insert_cvterm(
    array(
      'name' => 'broad',
      'id' => "local:broad",
      'definition' => 'The synonym is broader than the name.',
      'is_obsolete' => 0,
      'cv_name' => 'synonym_type',
      'is_relationship' => FALSE
    ),
    array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
    array(
      'name' => 'narrow',
      'id' => "local:narrow",
      'definition' => 'The synonym is narrower or more precise than the name.',
      'is_obsolete' => 0,
      'cv_name' => 'synonym_type',
      'is_relationship' => FALSE
    ),
    array('update_existing' => TRUE)
  );
  tripal_insert_cvterm(
    array(
      'name' => 'related',
      'id' => "local:related",
      'definition' => 'The synonym is related in some way.',
      'is_obsolete' => 0,
      'cv_name' => 'synonym_type',
      'is_relationship' => FALSE
    ),
    array('update_existing' => TRUE)
  );
}