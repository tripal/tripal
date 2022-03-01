<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Chado preparer.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of the Chado schema to prepare.
 * $preparer = \Drupal::service('tripal_chado.preparer');
 * $preparer->setParameters([
 *   'output_schemas' => ['chado'],
 * ]);
 * if (!$preparer->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoPreparer extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'preparer';

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one output
   * schema and no input schema as shown:
   * ```
   * ['output_schemas' => ['schema_name'], ]
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in cas of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      // Check input.
      if (!empty($this->parameters['input_schemas'])) {
        throw new ParameterException(
          "No input schema must be specified."
        );
      }

      // Check output.
      if (empty($this->parameters['output_schemas'])
          || (1 != count($this->parameters['output_schemas']))
      ) {
        throw new ParameterException(
          "Invalid number of output schemas. Only one output schema must be specified."
        );
      }

      $bio_tool = \Drupal::service('tripal_biodb.tool');
      $output_schema = $this->outputSchemas[0];

      // Note: schema names have already been validated through BioConnection.
      // Check if the target schema exists.
      if (!$output_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Output schema "'
          . $output_schema->getSchemaName()
          . '" does not exist.'
        );
      }
    }
    catch (\Exception $e) {
      // Log.
      $this->logger->error($e->getMessage());
      // Rethrow.
      throw $e;
    }
  }

  /**
   * Prepare a given chado schema by inserting minimal data.
   *
   * Task parameter array provided to the class constructor includes:
   * - 'input_schemas' array: no input schema
   * - 'output_schemas' array: one output Chado schema that must exist
   *   (required)
   *
   * Example:
   * ```
   * ['output_schemas' => ['chado_schema'], ]
   * ```
   *
   * @return bool
   *   TRUE if the task was performed with success and FALSE if the task was
   *   completed but without the expected success.
   *
   * @throws Drupal\tripal_biodb\Exception\TaskException
   *   Thrown when a major failure prevents the task from being performed.
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   Thrown if parameters are incorrect.
   *
   * @throws Drupal\tripal_biodb\Exception\LockException
   *   Thrown when the locks can't be acquired.
   */
  public function performTask() :bool {
    // Task return status.
    $task_success = FALSE;

    // Validate parameters.
    $this->validateParameters();

    // Acquire locks.
    $success = $this->acquireTaskLocks();
    if (!$success) {
      throw new LockException("Unable to acquire all locks for task. See logs for details.");
    }

    try
    {
      $chado_schema = $this->outputSchemas[0];

      $this->setProgress(0.1);
      $this->logger->notice("Loading ontologies...");
      $this->loadOntologies();

      $this->setProgress(0.5);
      $this->logger->notice("Creating default content types...");
      $this->contentTypes();

      $this->setProgress(1);
      $task_success = TRUE;

      $this->prepare();

      // Release all locks.
      $this->releaseTaskLocks();

      // Cleanup state API.
      $this->state->delete(static::STATE_KEY_DATA_PREFIX . $this->id);
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
      // Cleanup state API.
      $this->state->delete(static::STATE_KEY_DATA_PREFIX . $this->id);
      // Release all locks.
      $this->releaseTaskLocks();

      throw new TaskException(
        "Failed to complete schema integration task.\n"
        . $e->getMessage()
      );
    }

    return $task_success;
  }

  /**
   * Set progress value.
   *
   * @param float $value
   *   New progress value.
   */
  protected function setProgress(float $value) {
    $data = ['progress' => $value];
    $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);
  }

  /**
   * {@inheritdoc}
   */
  public function getProgress() :float {
    $data = $this->state->get(static::STATE_KEY_DATA_PREFIX . $this->id, []);

    if (empty($data)) {
      // No more data available. Assume process ended.
      $progress = 1;
    }
    else {
      $progress = $data['progress'];
    }
    return $progress;
  }

  /**
   * {@inheritdoc}
   */
  public function getStatus() :string {
    $status = '';
    $progress = $this->getProgress();
    if (1 > $progress) {
      $status = 'Integration in progress.';
    }
    else {
      $status = 'Integration done.';
    }
    return $status;
  }
  
  public function prepare() {
    // $this->logger->info("Loading ontologies...");
    // $this->loadOntologies();

    // $this->logger->info("Creating default content types...");
    // $this->contentTypes();

    $this->logger->info("Loading feature prerequisites...");
    $this->tripal_feature_install();

    $this->logger->info("Loading Tripal Importer prerequisites...");
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

    /*
     This currently cannot be implementated as the vocabulary API is being
     re-done. As such, this method is a placeholder.

     See https://github.com/tripal/tripal/blob/7.x-3.x/tripal_chado/includes/setup/tripal_chado.setup.inc
     for the Tripal 3 implementation of this method.

     Vocabularies to be added individually:
     - NCIT: NCI Thesaurus OBO Edition
     - rdfs: Resource Description Framework Schema

     Terms to be added individually:
     - Subgroup (NCIT:C25693)
     - rdfs:comment

     Ontologies to be imported by the OBO Loader:
     - Legacy Relationship Ontology: {tripal_chado}/files/legacy_ro.obo
     - Gene Ontology: http://purl.obolibrary.org/obo/go.obo
     - Taxonomic Rank: http://purl.obolibrary.org/obo/taxrank.obo
     - Tripal Contact: {tripal_chado}/files/tcontact.obo
     - Tripal Publication: {tripal_chado}/files/tpub.obo
     - Sequence Ontology: http://purl.obolibrary.org/obo/so.obo
     - Crop Ontology Germplasm: https://raw.githubusercontent.com/UofS-Pulse-Binfo/kp_entities/master/ontologies/CO_010.obo
     - EDAM Ontology: http://edamontology.org/EDAM.obo

     NOTE: Regarding CO_010 (crop ontology of germplasm), for some reason this
     has been removed from the original crop ontology website. As such, I've linked
     here to a file which loads and is correct. We use 4 terms from this ontology
     for our content types so we may want to consider alternatives.
     One such alternative may be MCPD: http://agroportal.lirmm.fr/ontologies/CO_020

    */

    $this->logger->warning("\tWaiting on completion of the Vocabulary API and Data Loaders.");
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
   * Helper: Create a given set of types.
   *
   * @param $types
   *   An array of types to be created. The key is used to link the type to
   *   it's term and the value is an array of details to be passed to the
   *   create() method for Tripal Entity Types. Some keys should be:
   *    - id: the integer id for this type; this will go away since it
   *        should be automatic.
   *    - name: the machine name for this type. It should be bio_data_[id]
   *        where [id] matches the integer id above. Also should be automatic.
   *    - label: a human-readable label for the content type.
   *    - category: a grouping string to categorize content types in the UI.
   *    - help_text: a single sentence describing the content type -usually
   *        the default is the term definition.
   * @param $terms
   *   An array of terms which must already exist where the key maps to a
   *   content type in the $types array. The value for each item is an array of
   *   details to be passed to the term creation API. Some keys should be:
   *    - accession: the unique identifier for the term (i.e. 2945)
   *    - vocabulary:
   *       - namespace: The name of the vocabulary (i.e. EDAM).
   *       - idspace: the id space of the term (i.e. operation).
   */
  protected function createGivenContentTypes($types, $terms) {
    foreach($terms as $key => $term_details) {
      $type_details = $types[$key];

      $this->logger->notice("  -- Creating " . $type_details['label'] . " (" . $type_details['name'] . ")...");

      // TODO: Create the term once the API is upgraded.
      // $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms($term_details);

      // TODO: Set the term in the type details.
      if (is_object($term)) {
        // $type_details['term_id'] = $term->getID();
        if (!array_key_exists($type_details, 'help_text')) {
          // $type_details['help_text'] = $term->getDefinition();
        }
      }
      else {
        $this->logger->warning("\tNo term attached -waiting on API update.");
      }

      // Check if the type already exists.
      // TODO: use term instead of label once it's available.
      $filter = ['label' => $type_details['label'] ];
      $exists = \Drupal::entityTypeManager()
        ->getStorage('tripal_entity_type')
        ->loadByProperties($filter);

      // Create the Type.
      if (empty($exists)) {
        $tripal_type = TripalEntityType::create($type_details);
        if (is_object($tripal_type)) {
          $tripal_type->save();
          $this->logger->notice("\tSaved successfully.");
        }
        else {
          $this->logger->error("\tCreation Failed! Details provided were: " . print_r($type_details));
        }
      }
      else {
        $this->logger->notice("\tSkipping as the content type already exists.");
      }
    }
  }

  /**
   * Creates the "General" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'General',
   ];
   * @endcode
   */
  protected function generalContentTypes() {

    // The 'Organism' entity type. This uses the obi:organism term.
    $terms['organism'] =[
      'accession' => '0100026',
      'vocabulary' => [
        'idspace' => 'OBI',
      ],
    ];
    $types['organism']= [
      'id' => 1,
      'name' => 'bio_data_1',
      'label' => 'Organism',
      'category' => 'General',
    ];

    // The 'Analysis' entity type. This uses the EDAM:analysis term.
    $terms['analysis'] = [
      'accession' => '2945',
      'vocabulary' => [
        'namespace' => 'EDAM',
        'idspace' => 'operation',
      ],
    ];
    $types['analysis'] = [
      'id' => 2,
      'name' => 'bio_data_2',
      'label' => 'Analysis',
      'category' => 'General',
    ];

    // The 'Project' entity type. bio_data_3
    $terms['project'] =[
      'accession' => 'C47885',
      'vocabulary' => [
        'idspace' => 'NCIT',
      ],
    ];
    $types['project']= [
      'id' => 3,
      'name' => 'bio_data_3',
      'label' => 'Project',
      'category' => 'General',
    ];

    // The 'Study' entity type. bio_data_4
    $terms['study'] =[
      'accession' => '001066',
      'vocabulary' => [
        'idspace' => 'SIO',
      ],
    ];
    $types['study']= [
      'id' => 4,
      'name' => 'bio_data_4',
      'label' => 'Study',
      'category' => 'General',
    ];

    // The 'Contact' entity type. bio_data_5
    $terms['contact'] =[
      'accession' => 'contact',
      'vocabulary' => [
        'idspace' => 'local',
      ],
    ];
    $types['contact']= [
      'id' => 5,
      'name' => 'bio_data_5',
      'label' => 'Contact',
      'category' => 'General',
    ];

    // The 'Publication' entity type. bio_data_6
    $terms['publication'] =[
      'accession' => '0000002',
      'vocabulary' => [
        'idspace' => 'TPUB',
      ],
    ];
    $types['publication']= [
      'id' => 6,
      'name' => 'bio_data_6',
      'label' => 'Publication',
      'category' => 'General',
    ];

    // The 'Protocol' entity type. bio_data_7
    $terms['protocol'] =[
      'accession' => '00101',
      'vocabulary' => [
        'idspace' => 'sep',
      ],
    ];
    $types['protocol']= [
      'id' => 7,
      'name' => 'bio_data_7',
      'label' => 'Protocol',
      'category' => 'General',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Genomic" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Genomic',
   ];
   * @endcode
   */
  protected function genomicContentTypes() {

    // The 'Gene' entity type. This uses the sequence:gene term.
    $terms['gene'] = [
      'accession' => '0000704',
      'vocabulary' => [
        'namespace' => 'sequence',
        'idspace' => 'SO',
      ],
    ];
    $types['gene'] = [
      'id' => 8,
      'name' => 'bio_data_8',
      'label' => 'Gene',
      'category' => 'Genomic',
    ];

    // the 'mRNA' entity type. bio_data_9
    $terms['mRNA'] =[
      'accession' => '0000234',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['mRNA']= [
      'id' => 9,
      'name' => 'bio_data_9',
      'label' => 'mRNA',
      'category' => 'Genomic',
    ];

    // The 'Phylogenetic tree' entity type. bio_data_10
    $terms['phylo'] =[
      'accession' => '0872',
      'vocabulary' => [
        'idspace' => 'data',
      ],
    ];
    $types['phylo']= [
      'id' => 10,
      'name' => 'bio_data_10',
      'label' => 'Phylogenetic Tree',
      'category' => 'Genomic',
    ];

    // The 'Physical Map' entity type. bio_data_11
    $terms['map'] =[
      'accession' => '1280',
      'vocabulary' => [
        'idspace' => 'data',
      ],
    ];
    $types['map']= [
      'id' => 11,
      'name' => 'bio_data_11',
      'label' => 'Physical Map',
      'category' => 'Genomic',
    ];

    // The 'DNA Library' entity type. bio_data_12
    $terms['library'] =[
      'accession' => 'C16223',
      'vocabulary' => [
        'idspace' => 'NCIT',
      ],
    ];
    $types['library']= [
      'id' => 12,
      'name' => 'bio_data_12',
      'label' => 'DNA Library',
      'category' => 'Genomic',
    ];

    // The 'Genome Assembly' entity type. bio_data_13
    $terms['assembly'] =[
      'accession' => '0525',
      'vocabulary' => [
        'idspace' => 'operation',
      ],
    ];
    $types['assembly']= [
      'id' => 13,
      'name' => 'bio_data_13',
      'label' => 'Genome Assembly',
      'category' => 'Genomic',
    ];

    // The 'Genome Annotation' entity type. bio_data_14
    $terms['annotation'] =[
      'accession' => '0362',
      'vocabulary' => [
        'idspace' => 'operation',
      ],
    ];
    $types['annotation']= [
      'id' => 14,
      'name' => 'bio_data_14',
      'label' => 'Genome Assembly',
      'category' => 'Genomic',
    ];

    // The 'Genome Project' entity type. bio_data_15
    $terms['genomeproject'] =[
      'accession' => 'Genome Project',
      'vocabulary' => [
        'idspace' => 'local',
      ],
    ];
    $types['genomeproject']= [
      'id' => 15,
      'name' => 'bio_data_15',
      'label' => 'Genome Project',
      'category' => 'Genomic',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Genetic" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Genetic',
   ];
   * @endcode
   */
  protected function geneticContentTypes() {

    // The 'Genetic Map' entity type. bio_data_16
    $terms['map'] =[
      'accession' => '1278',
      'vocabulary' => [
        'idspace' => 'data',
      ],
    ];
    $types['map']= [
      'id' => 16,
      'name' => 'bio_data_16',
      'label' => 'Genetic Map',
      'category' => 'Genetic',
    ];

    // The 'QTL' entity type. bio_data_17
    $terms['qtl'] =[
      'accession' => '0000771',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['qtl']= [
      'id' => 17,
      'name' => 'bio_data_17',
      'label' => 'QTL',
      'category' => 'Genetic',
    ];

    // The 'Sequence Variant' entity type. bio_data_18
    $terms['variant'] =[
      'accession' => '0001060',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['variant']= [
      'id' => 18,
      'name' => 'bio_data_18',
      'label' => 'Sequence Variant',
      'category' => 'Genetic',
    ];

    // The 'Genetic Marker' entity type. bio_data_19
    $terms['marker'] =[
      'accession' => '0001645',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['marker']= [
      'id' => 19,
      'name' => 'bio_data_19',
      'label' => 'Genetic Marker',
      'category' => 'Genetic',
    ];

    // The 'Heritable Phenotypic Marker' entity type. bio_data_20
    $terms['hpn'] =[
      'accession' => '0001500',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['hpn']= [
      'id' => 20,
      'name' => 'bio_data_20',
      'label' => 'Heritable Phenotypic Marker',
      'category' => 'Genetic',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Germplasm" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Germplasm',
   ];
   * @endcode
   */
  protected function germplasmContentTypes() {

    // The 'Phenotypic Trait' entity type. bio_data_28
    $terms['trait'] =[
      'accession' => 'C85496',
      'vocabulary' => [
        'idspace' => 'NCIT',
      ],
    ];
    $types['trait']= [
      'id' => 28,
      'name' => 'bio_data_28',
      'label' => 'Phenotypic Trait',
      'category' => 'Germplasm',
    ];

    // The 'Germplasm Accession' entity type. bio_data_21
    $terms['accession'] = [
      'accession' => '0000044',
      'vocabulary' => [
        'namespace' => 'germplasm_ontology',
        'idspace' => 'CO_010',
      ],
    ];
    $types['accession'] = [
      'id' => 21,
      'name' => 'bio_data_21',
      'label' => 'Germplasm Accession',
      'category' => 'Germplasm',
    ];

    // The 'Breeding Cross' entity type. bio_data_22
    $terms['cross'] =[
      'accession' => '0000255',
      'vocabulary' => [
        'idspace' => 'CO_010',
      ],
    ];
    $types['cross']= [
      'id' => 22,
      'name' => 'bio_data_22',
      'label' => 'Breeding Cross',
      'category' => 'Germplasm',
    ];

    // The 'Germplasm Variety' entity type. bio_data_23
    $terms['variety'] =[
      'accession' => '0000029',
      'vocabulary' => [
        'idspace' => 'CO_010',
      ],
    ];
    $types['variety']= [
      'id' => 23,
      'name' => 'bio_data_23',
      'label' => 'Germplasm Variety',
      'category' => 'Germplasm',
    ];

    // The 'Recombinant Inbred Line' entity type. bio_data_24
    $terms['ril'] =[
      'accession' => '0000162',
      'vocabulary' => [
        'idspace' => 'CO_010',
      ],
    ];
    $types['ril']= [
      'id' => 24,
      'name' => 'bio_data_24',
      'label' => 'Recombinant Inbred Line',
      'category' => 'Germplasm',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Expression" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Expression',
   ];
   * @endcode
   */
  protected function expressionContentTypes() {

    // The 'biological sample' entity type. bio_data_25
    $terms['sample'] =[
      'accession' => '00195',
      'vocabulary' => [
        'idspace' => 'sep',
      ],
    ];
    $types['sample']= [
      'id' => 25,
      'name' => 'bio_data_25',
      'label' => 'Biological Sample',
      'category' => 'Expression',
    ];

    // The 'Assay' entity type. bio_data_26
    $terms['assay'] =[
      'accession' => '0000070',
      'vocabulary' => [
        'idspace' => 'OBI',
      ],
    ];
    $types['assay']= [
      'id' => 26,
      'name' => 'bio_data_26',
      'label' => 'Assay',
      'category' => 'Expression',
    ];

    // The 'Array Design' entity type. bio_data_27
    $terms['design'] =[
      'accession' => '0000269',
      'vocabulary' => [
        'idspace' => 'EFO',
      ],
    ];
    $types['design']= [
      'id' => 27,
      'name' => 'bio_data_27',
      'label' => 'Array Design',
      'category' => 'Expression',
    ];

    $this->createGivenContentTypes($types, $terms);
  }
}
