<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\Entity\TripalEntityType;

class ChadoPreparer {

  /**
   * The name of the schema we are interested in installing/updating chado for.
   */
  protected $schemaName;

  /**
   * The DRUPAL-managed database connection.
   */
  protected $connection;

  /**
   * The drupal logger for tripal.
   */
  protected $logger;

  /**
   * Holds the Job object
   */
  protected $job = NULL;

  /**
   * Constructor: initialize connections.
   */
  public function __construct() {
    $this->connection = \Drupal::database();

    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');
  }

  /**
   * Set the schema name.
   */
  public function setSchema($schema_name) {
    // Schema name must be all lowercase with no special characters.
    // It should also be a single word.
    if (preg_match('/^[a-z][a-z0-9]+$/', $schema_name) === 0) {
      $this->logger->error('The schema name must be a single word containing only lower case letters or numbers and cannot begin with a number.');
      return FALSE;
    }
    else {
      $this->logger->info('Setting Schema to "' . $schema_name . '".');
      $this->schemaName = $schema_name;
      return TRUE;
    }
}

  /**
   * A setter for the job object if this class is being run using a Tripal job.
   */
  public function setJob(\Drupal\tripal\Services\TripalJob $job) {
    $this->job = $job;
    $this->logger->setJob($job);
    return TRUE;
  }

  /**
   * Retrieve the Drupal connection to the database.
   *
   * @return Drupal\database
   *   Current Drupal connection.
   */
  public function getDrupalConnection() {
    return $this->connection;
  }

  /**
   * Retrieves the message logger.
   *
   * @return object
   *
   */
  public function getLogger() {
    return $this->logger;
  }

  /**
   *
   */
  public function prepare() {
    $this->logger->info("Loading ontologies...");
    $this->loadOntologies();

    $this->logger->info("Creating default content types...");
    $this->contentTypes();

    $this->logger->info("Loading Tripal Importer prequisites...");
    // Attempt to add the tripal_gff_temp table into chado
    $this->logger->info("Add Tripal GFF Temp table...");
    $this->tripal_chado_add_tripal_gff_temp_table();
    // Attempt to add the tripal_gffprotein_temp table into chado
    $this->logger->info("Add Tripal GFFPROTEIN Temp table...");
    $this->tripal_chado_add_tripal_gffprotein_temp_table();
    // Attempt to add the tripal_chado_add_tripal_gffcds_temp table into chado
    $this->logger->info("Add Tripal GFFCDS Temp table...");
    $this->tripal_chado_add_tripal_gffcds_temp_table();
    // Attempt to add the tripal_chado_add_tripal_cv_obo table into chado
    $this->logger->info("Add Tripal CV OBO table...");
    $this->tripal_add_tripal_cv_obo_table();
    // Attempt to add the mview table
    $this->logger->info("Add Tripal MVIEWS table...");
    $this->tripal_add_tripal_mviews_table();
    // Attempt to add the chado_cvterm_mapping table
    $this->logger->info("Add Tripal CVTERM mapping...");
    $this->tripal_add_chado_cvterm_mapping();
    // Attempt to add the tripal_cv_defaults
    $this->logger->info("Add Tripal CV defaults...");
    $this->tripal_add_chado_tripal_cv_defaults_table();
    // Attempt to add the tripal_bundle table
    $this->logger->info("Add Tripal bundle schema...");
    $this->tripal_add_tripal_bundle_schema();

    // Attempt to add prerequisite ontology data (seems to be needed by the OBO
    // importers) for example
    $this->logger->info("Load ontologies required for Tripal Importers to function properly...");
    $this->tripal_chado_load_ontologies();

    $this->logger->info("Preparation complete.");
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
   * Loads ontologies necessary for creation of default Tripal content types.
   */
  protected function loadOntologies() {
    \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
      'idspace' => 'NCIT',
      'namespace' => 'ncit',
      'name' => 'NCI Thesaurus OBO Edition',
      'description' => 'The NCIt OBO Edition project aims to increase integration of the NCIt with OBO Library ontologies.',
      'url' => 'http://purl.obolibrary.org/obo/ncit.owl',
      'urlprefix' => ' http://purl.obolibrary.org/obo/{db}_{accession}',
    ]);

    \Drupal::service('tripal.tripalTerm.manager')->addTerm([
      'accession' => 'C25693',
      'name' => 'Subgroup',
      'vocabulary' => [
        'namespace' => 'ncit',
        'idspace' => 'NCIT',
      ],
      'definition' => 'A subdivision of a larger group with members often exhibiting similar characteristics. [ NCI ]',
    ]);

    \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
      'idspace' => 'rdfs',
      'namespace' => 'rdfs',
      'name' => 'Resource Description Framework Schema',
      'description' => 'RDF Schema provides a data-modelling vocabulary for RDF data.',
      'url' => 'https://www.w3.org/TR/rdf-schema/',
      'urlprefix' => 'http://www.w3.org/2000/01/rdf-schema#{accession}',
    ]);

    \Drupal::service('tripal.tripalTerm.manager')->addTerm([
      'accession' => 'comment',
      'name' => 'comment',
      'vocabulary' => [
        'namespace' => 'rdfs',
        'idspace' => 'rdfs',
      ],
      'definition' => 'A human-readable description of a resource\'s name.',
    ]);

    // TODO:
    // T3 loads many terms we need for default content types through the OBO
    // Importer. As of April 19, 2021 the OBO importer has not yet been migrated
    // to T4 and therefore cannot be used. As a result, I will be adding a few
    // terms here individually using the vocab and term managers. In the
    // future, this should be replaced with calls to the OBO importer so that
    // the terms can be imported automatically.
    $terms = [];

    // Organism.
    $terms['SO:0100026'] = [
      'accession' => '0100026',
      'name' => 'organism',
      'definition' => 'A material entity that is an individual living system, such as animal, plant, bacteria or virus, that is capable of replicating or reproducing, growth and maintenance in the right environment. An organism may be unicellular or made up, like humans, of many billions of cells divided into specialized tissues and organs.',
      'vocabulary' => [
        'name' => 'The Ontology for Biomedical Investigation',
        'namespace' => 'obi',
        'idspace' => 'OBI',
        'description' => 'The Ontology for Biomedical Investigations (OBI) will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data.',
        'url' => 'http://obi-ontology.org/page/Main_Page',
        'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
      ],
    ];

    // Gene.
    $terms['SO:0000704'] = [
      'accession' => '0000704',
      'name' => 'gene',
      'definition' => 'A region (or regions) that includes all of the sequence elements necessary to encode a functional transcript. A gene may include regulatory regions, transcribed regions and/or other functional sequence regions.',
      'vocabulary' => [
        'name' => 'The Sequence Ontology',
        'namespace' => 'sequence',
        'idspace' => 'SO',
        'description' => 'The Sequence Ontology (SO) is a collaborative ontology project for the definition of sequence features used in biological sequence annotation.',
        'url' => 'http://www.sequenceontology.org/',
        'urlprefix' => 'http://www.sequenceontology.org/browser/current_svn/term/{db}:{accession}',
      ],
    ];

    // Germplasm Accession.
    $terms['CO_010:0000044'] = [
      'accession' => '0000044',
      'name' => 'accession',
      'definition' => '',
      'vocabulary' => [
        'name' => 'GCP germplasm ontology',
        'namespace' => 'germplasm_ontology',
        'idspace' => 'CO_010',
        'description' => 'Provides desciptors for germplasm collections. Adapted from Descriptors for Banana (Musa spp.) (1996) and Descriptors for Mango by Bioversity.',
        'url' => 'http://www.cropontology.org/ontology/CO_010/Germplasm',
        'urlprefix' => 'http://www.cropontology.org/terms/{db}:{accession}/index.html',
      ],
    ];

    // Analysis.
    $terms['2945'] = [
      'accession' => '2945',
      'name' => 'Analysis',
      'definition' => 'Apply analytical methods to existing data of a specific type.',
      'vocabulary' => [
        'name' => 'EDAM - Ontology of bioscientific data analysis',
        'namespace' => 'EDAM',
        'idspace' => 'operation',
        'description' => 'EDAM is a comprehensive ontology of well-established, familiar concepts that are prevalent within computational biology, bioinformatics, and bioimage informatics.',
        'url' => 'http://edamontology.org/page',
        'urlprefix' => 'http://edamontology.org/{db}_{accession}',
      ],
    ];

    /* Template for adding more.
    $terms[''] = [
      'accession' => '',
      'name' => '',
      'definition' => '',
      'vocabulary' => [
        'name' => '',
        'namespace' => '',
        'idspace' => '',
        'description' => '',
        'url' => '',
        'urlprefix' => '',
      ],
    ]; */

    // Actually add the terms described above.
    // The term manager will create the vocabulary if it doesn't exist.
    foreach ($terms as $term) {
      \Drupal::service('tripal.tripalTerm.manager')->addTerm($term);
    }

    // ###################################################################
    // Begin T3 code to be converted to D8 when the OBO importer is ready.
    // ###################################################################
    /*
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
    */
  }

  /**
   * Creates default content types.
   */
  protected function contentTypes() {
    $this->generalContentTypes();
    $this->genomicContentTypes();
    $this->geneticContentTypes();
    $this->germplasmContentTypes();
    $this->expressionContentTypes();
  }

  /**
   * Creates the "General" category of content types.
   */
  protected function generalContentTypes() {
    // Create the 'Organism' entity type. This uses the obi:organism term.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '0100026',
      'vocabulary' => [
        'namespace' => 'obi',
        'idspace' => 'OBI',
      ],
    ]);

    $organism = TripalEntityType::create([
      'id' => 1,
      'name' => 'bio_data_1',
      'label' => 'Organism',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'General',
    ]);
    $organism->save();

    // Create the 'Analysis' entity type. This uses the EDAM:analysis term.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '2945',
      'vocabulary' => [
        'namespace' => 'EDAM',
        'idspace' => 'operation',
      ],
    ]);

    $analysis = TripalEntityType::create([
      'id' => 2,
      'name' => 'bio_data_2',
      'label' => 'Analysis',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'General',
    ]);
    $analysis->save();

    // TODO: Create the 'Project' entity type. bio_data_3

    // TODO: Create the 'Study' entity type. bio_data_4

    // TODO: Create the 'Contact' entity type. bio_data_5

    // TODO: Create the 'Publication' entity type. bio_data_6

    // TODO: Create the 'Protocol' entity type. bio_data_7
  }

  /**
   * Creates the "Genomic" category of content types.
   */
  protected function genomicContentTypes() {
    // Create the 'Gene' entity type. This uses the sequence:gene term.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '0000704',
      'vocabulary' => [
        'namespace' => 'sequence',
        'idspace' => 'SO',
      ],
    ]);

    $gene = TripalEntityType::create([
      'id' => 8,
      'name' => 'bio_data_8',
      'label' => 'Gene',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'Genomic',
    ]);
    $gene->save();

    // TODO: Create the 'mRNA' entity type. bio_data_9

    // TODO: Create the 'Phylogenetic tree' entity type. bio_data_10

    // TODO: Create the 'Physical Map' entity type. bio_data_11

    // TODO: Create the 'DNA Library' entity type. bio_data_12

    // TODO: Create the 'Genome Assembly' entity type. bio_data_13

    // TODO: Create the 'Genome Annotation' entity type. bio_data_14

    // TODO: Create the 'Genome Project' entity type. bio_data_15
  }

  /**
   * Creates the "Genetic" category of content types.
   */
  protected function geneticContentTypes() {
    // TODO: Create the 'Genetic Map' entity type. bio_data_16

    // TODO: Create the 'QTL' entity type. bio_data_17

    // TODO: Create the 'Sequence Variant' entity type. bio_data_18

    // TODO: Create the 'Genetic Marker' entity type. bio_data_19

    // TODO: Create the 'Heritable Phenotypic Marker' entity type. bio_data_20
  }

  /**
   * Creates the "Germplasm/Breeding" category of content types.
   */
  protected function germplasmContentTypes() {
    // TODO: Create the 'Phenotypic Trait' entity type.

    // Create the 'Germplasm Accession' entity type.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '0000044',
      'vocabulary' => [
        'namespace' => 'germplasm_ontology',
        'idspace' => 'CO_010',
      ],
    ]);

    $germplasm_accession = TripalEntityType::create([
      'id' => 21,
      'name' => 'bio_data_21',
      'label' => 'Germplasm Accession',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'Germplasm/Breeding',
    ]);
    $germplasm_accession->save();

    // TODO: Create the 'Breeding Cross' entity type. bio_data_22

    // TODO: Create the 'Germplasm Variety' entity type. bio_data_23

    // TODO: Create the 'Recombinant Inbred Line' entity type. bio_data_24
  }

  /**
   * Creates the "Expression" category of content types.
   */
  protected function expressionContentTypes() {
    // TODO: Create the 'biological sample' entity type. bio_data_25

    // TODO: Create the 'Assay' entity type. bio_data_26

    // TODO: Create the 'Array Design' entity type. bio_data_27
  }
}
