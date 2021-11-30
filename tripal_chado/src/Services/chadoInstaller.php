<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\Services\bulkPgSchemaInstaller;

class chadoInstaller extends bulkPgSchemaInstaller {

  /**
   * The version of the current and new chado schema specified by $schemaName.
   */
  protected $curVersion;
  protected $newVersion;

  /**
   * The name of the schema we are interested in installing/updating chado for.
   */
  protected $schemaName;

  /**
   * The number of chunk files per version we can install.
   */
  protected $installNumChunks = [
    1.3 => 41,
  ];


  /**
   * Install chado in the specified schema.
   *
   * @param float $version
   *   The version of chado you would like to install.
   */
  public function install($version) {
    $this->newVersion = $version;
    $chado_schema = $this->schemaName;
    $connection = $this->connection;

    // VALIDATION.
    // Check the version is valid.
    if (!in_array($version, ['1.3'])) {
      $this->logger->error("That version is not supported by the installer.");
      return FALSE;
    }
    // Check the schema name is valid.
    if (preg_match('/^[a-z][a-z0-9]+$/', $chado_schema) === 0) {
      // Schema name must be a single word containing only lower case letters
      // or numbers and cannot begin with a number.
      $this->logger->error("Schema name must be a single alphanumeric word beginning with a number and all lowercase.");
      return FALSE;
    }

    // 1) Drop the schema if it already exists.
    $this->dropSchema('genetic_code');
    $this->dropSchema('so');
    $this->dropSchema('frange');
    $this->dropSchema($chado_schema);

    // 2) Create the schema.
    $this->createSchema($chado_schema);

    // 3) Apply SQL files containing table definitions.
    $this->applyDefaultSchema($version);

    // 4) Initialize the schema with basic data.
    $init_file = \Drupal::service('extension.list.module')->getPath('tripal_chado') .
      '/chado_schema/initialize-' . $version . '.sql';
    $success = $this->applySQL($init_file, $chado_schema);
    if ($success) {
      // @upgrade tripal_report_error().
      $this->logger->info("Install of Chado v1.3 (Step 2 of 3) Successful.\n");
    }
    else {
      // @upgrade tripal_report_error().
      $this->logger->info("Installation (Step 2 of 3) Problems!  Please check output for errors.\n");
    }

    // 5) Finally set the version and tell Tripal.
    $vsql = "
      INSERT INTO $chado_schema.chadoprop (type_id, value)
        VALUES (
         (SELECT cvterm_id
          FROM $chado_schema.cvterm CVT
            INNER JOIN $chado_schema.cv CV on CVT.cv_id = CV.cv_id
           WHERE CV.name = 'chado_properties' AND CVT.name = 'version'),
         :version)
    ";
    $this->connection->query($vsql, [':version' => $version]);
    $this->connection->insert('chado_installations')
      ->fields([
        'schema_name' => $chado_schema,
        'version' => $version,
        'created' => \Drupal::time()->getRequestTime(),
        'updated' => \Drupal::time()->getRequestTime(),
      ])
      ->execute();

    // Attempt to add the tripal_gff_temp table into chado
    $this->tripal_chado_add_tripal_gff_temp_table();
    // Attempt to add the tripal_gffprotein_temp table into chado
    $this->tripal_chado_add_tripal_gffprotein_temp_table();
    // Attempt to add the tripal_chado_add_tripal_gffcds_temp table into chado
    $this->tripal_chado_add_tripal_gffcds_temp_table();
    // Attempt to add the tripal_chado_add_tripal_cv_obo table into chado
    $this->tripal_add_tripal_cv_obo_table();
    // Attempt to add the mview table
    $this->tripal_add_tripal_mviews_table();
    // Attempt to add the chado_cvterm_mapping table
    $this->tripal_add_chado_cvterm_mapping();
    // Attempt to add the tripal_cv_defaults
    $this->tripal_add_chado_tripal_cv_defaults_table();
    // Attempt to add the tripal_bundle table
    $this->tripal_add_tripal_bundle_schema();

    // Attempt to add prerequisite ontology data (seems to be needed by the OBO
    // importers) for example
    $this->logger->info("Install of Chado v1.3 (Step 3 of 3) Loading ontologies"
      . " (this can take a few minutes).\n");
    $this->tripal_chado_load_ontologies();
    $this->tripal_feature_install();
    $this->logger->info("Install of Chado v1.3 (Step 3 of 3) Successful.\nInstallation Complete\n");
    
  }


  /**
   * Implements hook_install().
   *
   * @ingroup tripal_legacy_feature
   */
  function tripal_feature_install() {

    // Note: the feature_property OBO that came with Chado v1.2 should not
    // be automatically installed.  Some of the terms are duplicates of
    // others in better maintained vocabularies.  New Tripal sites should
    // use those.
    // $obo_path = '{tripal_feature}/files/feature_property.obo';
    // $obo_id = tripal_insert_obo('Chado Feature Properties', $obo_path);
    // tripal_submit_obo_job(array('obo_id' => $obo_id));

    // Add the vocabularies used by the feature module.
    $this->tripal_feature_add_cvs();

    // Set the default vocabularies.
    tripal_set_default_cv('feature', 'type_id', 'sequence');
    tripal_set_default_cv('featureprop', 'type_id', 'feature_property');
    tripal_set_default_cv('feature_relationship', 'type_id', 'feature_relationship');
  }  

  /**
   * Add cvs related to publications
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

    for ($i = 0; $i < count($ontologies); $i++) {
      $obo_id = chado_insert_obo($ontologies[$i]['name'], $ontologies[$i]['path']);
    }    
    /*
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
    */
  }


  /**
   * The base table for TripalEntity entities.
   *
   * This table contains a list of Biological Data Types.
   * For the example above (5 genes and 10 mRNAs), there would only be two records in
   * this table one for "gene" and another for "mRNA".
   */
  function tripal_add_tripal_bundle_schema() {
    $tableExists = \Drupal::database()->schema()->tableExists('tripal_bundle');
    if(!$tableExists) {    
      $schema = array(
        'description' => 'Stores information about defined tripal data types.',
        'fields' => array(
          'id' => array(
            'type' => 'serial',
            'not null' => TRUE,
            'description' => 'Primary Key: Unique numeric ID.',
          ),
          'type' => array(
            'description' => 'The type of entity (e.g. TripalEntity).',
            'type' => 'varchar',
            'length' => 64,
            'not null' => TRUE,
            'default' => '',
          ),
          'term_id' => array(
            'description' => 'The term_id for the type of entity. This term_id corresponds to a TripalTerm record.',
            'type' => 'int',
            'not null' => TRUE,
          ),
          'name' => array(
            'description' => 'The name of the bundle. This should be an official vocabulary ID (e.g. SO, RO, GO) followed by an underscore and the term accession.',
            'type' => 'varchar',
            'length' => 1024,
            'not null' => TRUE,
            'default' => '',
          ),
          'label' => array(
            'description' => 'The human-readable name of this bundle.',
            'type' => 'varchar',
            'length' => 255,
            'not null' => TRUE,
            'default' => '',
          ),
        ),
        'indexes' => array(
          'name' => array('name'),
          'term_id' => array('term_id'),
          'label' => array('label'),
        ),
        'primary key' => array('id'),
        'unique keys' => array(
          'name' => array('name'),
        ),
      );
      \Drupal::database()->schema()->createTable('tripal_bundle', $schema);
    }
    else {
      print "tripal_bundle table already exists... bypassing...\n";
    }  
  }

  /**
   * * Table definition for the tripal_cv_defaults table
   * @param unknown $schema
   */
  function tripal_add_chado_tripal_cv_defaults_table() {
    $tableExists = \Drupal::database()->schema()->tableExists('tripal_cv_defaults');
    if(!$tableExists) {  
      $schema = array(
        'fields' => array(
          'cv_default_id' => array(
            'type' => 'serial',
            'unsigned' => TRUE,
            'not null' => TRUE
          ),
          'table_name' => array(
            'type' => 'varchar',
            'length' => 128,
            'not null' => TRUE,
          ),
          'field_name' => array(
            'type' => 'varchar',
            'length' => 128,
            'not null' => TRUE,
          ),
          'cv_id' => array(
            'type' => 'int',
            'not null' => TRUE,
          )
        ),
        'indexes' => array(
          'tripal_cv_defaults_idx1' => array('table_name', 'field_name'),
        ),
        'unique keys' => array(
          'tripal_cv_defaults_unq1' => array('table_name', 'field_name', 'cv_id'),
        ),
        'primary key' => array('cv_default_id')
      );
      \Drupal::database()->schema()->createTable('tripal_cv_defaults', $schema);
      // chado_create_custom_table('tripal_mviews', $schema, TRUE, NULL, FALSE);
    }
    else {
      print "tripal_cv_defaults table already exists... bypassing...\n";
    } 
  }

  public function tripal_add_chado_cvterm_mapping() {
    $tableExists = \Drupal::database()->schema()->tableExists('chado_cvterm_mapping');
    if(!$tableExists) {    
      $schema = array (
        'fields' => array (
          'mapping_id' => array(
            'type' => 'serial',
            'not null' => TRUE
          ),
          'cvterm_id' => array (
            'type' => 'int',
            'not null' => TRUE
          ),
          'chado_table' => array (
            'type' => 'varchar',
            'length' => 128,
            'not null' => TRUE
          ),
          'chado_field' => array (
            'type' => 'varchar',
            'length' => 128,
            'not null' => FALSE
          ),
        ),
        'primary key' => array (
          0 => 'mapping_id'
        ),
        'unique key' => array(
          'cvterm_id',
        ),
        'indexes' => array(
          'tripal_cvterm2table_idx1' => array('cvterm_id'),
          'tripal_cvterm2table_idx2' => array('chado_table'),
          'tripal_cvterm2table_idx3' => array('chado_table', 'chado_field'),
        ),
      ); 
      \Drupal::database()->schema()->createTable('chado_cvterm_mapping', $schema);
      // chado_create_custom_table('tripal_mviews', $schema, TRUE, NULL, FALSE);
    }
    else {
      print "chado_cvterm_mapping table already exists... bypassing...\n";
    }       
  }

  public function tripal_add_tripal_mviews_table() {
    $tableExists = \Drupal::database()->schema()->tableExists('tripal_mviews');
    if(!$tableExists) {
      $schema = array(
        'fields' => array(
          'mview_id' => array(
            'type' => 'serial',
            'unsigned' => TRUE,
            'not null' => TRUE
          ),
          'name' => array(
            'type' => 'varchar',
            'length' => 255,
            'not null' => TRUE
          ),
          'modulename' => array(
            'type' => 'varchar',
            'length' => 50,
            'not null' => TRUE,
            'description' => 'The module name that provides the callback for this job'
          ),
          'mv_table' => array(
            'type' => 'varchar',
            'length' => 128,
            'not null' => FALSE
          ),
          'mv_specs' => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => FALSE
          ),
          'mv_schema' => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => FALSE
          ),
          'indexed' => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => FALSE
          ),
          'query' => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => TRUE
          ),
          'special_index' => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => FALSE
          ),
          'last_update' => array(
            'type' => 'int',
            'not null' => FALSE,
            'description' => 'UNIX integer time'
          ),
          'status'        => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => FALSE
          ),
          'comment' => array(
            'type' => 'text',
            'size' => 'normal',
            'not null' => FALSE
          ),
        ),
        'indexes' => array(
          'mview_id' => array('mview_id')
        ),
        'unique keys' => array(
          'mv_table' => array('mv_table'),
          'mv_name' => array('name'),
        ),
        'primary key' => array('mview_id'),
      );
      \Drupal::database()->schema()->createTable('tripal_mviews', $schema);
      // chado_create_custom_table('tripal_mviews', $schema, TRUE, NULL, FALSE);
    }
    else {
      print "tripal_mviews table already exists... bypassing...\n";
    }
  }  

  public function tripal_add_tripal_cv_obo_table() {
    $tableExists = \Drupal::database()->schema()->tableExists('tripal_cv_obo');
    if(!$tableExists) {    
      $schema = [
        // 'table' => 'tripal_cv_obo',
        'fields' => [
          'obo_id' => [
            'type' => 'serial',
            'unsigned' => TRUE,
            'not null' => TRUE
          ],
          'name' => [
            'type' => 'varchar',
            'length' => 255
          ],
          'path'  => [
            'type' => 'varchar',
            'length' => 1024
          ],
        ],
        'indexes' => [
          'tripal_cv_obo_idx1' => ['obo_id'],
        ],
        'primary key' => ['obo_id'],
      ];
      \Drupal::database()->schema()->createTable('tripal_cv_obo', $schema);
    }
    // chado_create_custom_table('tripal_cv_obo', $schema, TRUE, NULL, FALSE);
  }

  public function tripal_chado_add_tripal_gff_temp_table() {
    $tableExists = chado_table_exists('tripal_gff_temp');
    if(!$tableExists) {
      $schema = [
        // 'table' => 'tripal_gff_temp',
        'fields' => [
          'feature_id' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'organism_id' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'uniquename' => [
            'type' => 'text',
            'not null' => TRUE,
          ],
          'type_name' => [
            'type' => 'varchar',
            'length' => '1024',
            'not null' => TRUE,
          ],
        ],
        'indexes' => [
          'tripal_gff_temp_idx0' => ['feature_id'],
          'tripal_gff_temp_idx0' => ['organism_id'],
          'tripal_gff_temp_idx1' => ['uniquename'],
        ],
        'unique keys' => [
          'tripal_gff_temp_uq0' => ['feature_id'],
          'tripal_gff_temp_uq1' => ['uniquename', 'organism_id', 'type_name'],
        ],
      ];
      chado_create_custom_table('tripal_gff_temp', $schema, TRUE, NULL, FALSE);
    }
    else {
      print "tripal_gff_temp chado table already exists... bypassing...\n";
    }
  }

  public function tripal_chado_add_tripal_gffprotein_temp_table() {
    $tableExists = chado_table_exists('tripal_gffprotein_temp');
    if(!$tableExists) {    
      $schema = [
        // 'table' => 'tripal_gffprotein_temp',
        'fields' => [
          'feature_id' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'parent_id' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'fmin' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'fmax' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
        ],
        'indexes' => [
          'tripal_gff_temp_idx0' => ['feature_id'],
          'tripal_gff_temp_idx0' => ['parent_id'],
        ],
        'unique keys' => [
          'tripal_gff_temp_uq0' => ['feature_id'],
        ],
      ];
      chado_create_custom_table('tripal_gffprotein_temp', $schema, TRUE, NULL, FALSE);
    }
    else {
      print "tripal_gffprotein_temp chado table already exists... bypassing...\n";
    }
  }
  
  public function tripal_chado_add_tripal_gffcds_temp_table() {
    $tableExists = chado_table_exists('tripal_gffcds_temp');
    if(!$tableExists) {    
      $schema = [
        // 'table' => 'tripal_gffcds_temp',
        'fields' => [
          'feature_id' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'parent_id' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'phase' => [
            'type' => 'int',
            'not null' => FALSE,
          ],
          'strand' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'fmin' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
          'fmax' => [
            'type' => 'int',
            'not null' => TRUE,
          ],
        ],
        'indexes' => [
          'tripal_gff_temp_idx0' => ['feature_id'],
          'tripal_gff_temp_idx0' => ['parent_id'],
        ],
      ];
      chado_create_custom_table('tripal_gffcds_temp', $schema, TRUE, NULL, FALSE);
    }
    else {
      print "tripal_gffcds_temp chado table already exists... bypassing...\n";
    }
  }

  /**
   * Updates chado in the specified schema.
   *
   * @param float $version
   *   The version of chado you would like to update to.
   */
  public function update($version) {
    $this->newVersion = $version;

    // @todo implement update.
  }

  /**
   * Applies the table definition SQL files.
   *
   * @param float $version
   *   The version of the chado schema to install.
   * @return bool
   *   Whether the install was successful.
   */
  protected function applyDefaultSchema($version) {
    $chado_schema = $this->schemaName;
    $numChunks = $this->installNumChunks[$version];

    //   Since the schema SQL file is large we have split it into
    //   multiple chunks. This loop will load each chunk...
    $failed = FALSE;
    $module_path = \Drupal::service('extension.list.module')->getPath('tripal_chado');
    $path = $module_path . '/chado_schema/parts-v' . $version . '/';
    for ($i = 1; $i <= $numChunks; $i++) {

      $file = $path . 'default_schema-' . $version . '.part' . $i . '.sql';
      $success = $this->applySQL($file, $chado_schema);

      if ($success) {
        // @upgrade tripal_report_error().
        $this->logger->info("  Import part $i of $numChunks Successful!");
      }
      else {
        $failed = TRUE;
        // @upgrade tripal_report_error().
        $this->logger->error("Schema installation part $i of $numChunks Failed...");
          break;
      }
    }

    // Set back to the default connection.
    $drupal_schema = chado_get_schema_name('drupal');
    $this->connection->query("SET search_path = $drupal_schema");

    // Finally report back to the admin how we did.
    if ($failed) {
      // @upgrade tripal_report_error().
      $this->logger->error("Installation (Step 1 of 2) Problems!  Please check output above for errors.");
      return FALSE;
    }
    else {
      // @upgrade tripal_report_error().
      $this->logger->info("Install of Chado v1.3 (Step 1 of 2) Successful.\n");
      return TRUE;
    }
  }
}
