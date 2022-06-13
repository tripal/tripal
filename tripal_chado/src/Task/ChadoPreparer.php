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

      $tripal_dbx = \Drupal::service('tripal.dbx');
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
      $chado_schema = $this->outputSchemas[0]->getSchemaName();

      $this->setProgress(0.1);
      $this->logger->notice("Creating Tripal Materialized Views and Custom Tables...");
      $chado_version = chado_get_version(FALSE, FALSE, $output_schema);
      
      if ($chado_version == '1.3') {
        $this->add_vx_x_custom_tables();
        $this->fix_v1_3_custom_tables();
      }

      $this->setProgress(0.2);
      $this->logger->notice("Loading ontologies...");
      $this->loadOntologies();

      $this->setProgress(0.3);
      $this->logger->notice('Populating materialized view cv_root_mview...');
      // POSTPONED: populate mviews. // SEEMS TO BE MVIEW RELATED AND THUS NOT NEEDED FOR TRIPAL LOADERS
      
      $this->setProgress(0.4);
      $this->logger->notice("Making semantic connections for Chado tables/fields...");
      // $this->populate_chado_semweb_table(); // WE NEED TO DO THIS
      
      $this->setProgress(0.5);
      $this->logger->notice("Map Chado Controlled vocabularies to Tripal Terms...");
      // TODO //  NEXT UP ON THE LIST TO DETERMINE IF WE NEED THIS
      
      $this->setProgress(0.6);
      $this->logger->notice('Populating materialized view db2cv_mview...'); 
      // POSTPONED (mview related)    

      $this->setProgress(0.7);
      $this->logger->notice("Creating default content types...");
      $this->contentTypes();

      $this->setProgress(1);
      $task_success = TRUE;

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
   * Many of the custom tables created for Chado v1.2 are now in Chado v1.3.
   *
   * These tables need not be tracked by Tripal anymore as custom tables and
   * in some cases the Chado version has different columns so we need to
   * adjust them.
   */
  protected function fix_v1_3_custom_tables() {
    
    $connection_chado = \Drupal::service('tripal_chado.database');

    // Update the featuremap_dbxref table by adding an is_current field.
    if (!chado_column_exists('featuremap_dbxref', 'is_current')) {
      $connection_chado->query("ALTER TABLE {featuremap_dbxref} ADD COLUMN is_current boolean DEFAULT true NOT NULL;");
    }
    
    // Remove the previously managed custom tables from the
    // tripal_custom_tables table.
    // \Drupal::database()->select
    $db = \Drupal::database();
    $db->delete('tripal_custom_tables')
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

  /**
   * Add custom tables for any version of Chado.
   *
   * These are tables that Chado uses to manage the site (i.e. temporary
   * loading tables) and not for primary data storage.
   */
  protected function add_vx_x_custom_tables() {
    // Add in custom tables.
    $this->tripal_chado_add_tripal_gff_temp_table();
    $this->tripal_chado_add_tripal_gffcds_temp_table();
    $this->tripal_chado_add_tripal_gffprotein_temp_table();
    $this->tripal_chado_add_tripal_obo_temp_table();

    // Add in materialized views.
    // TODO BUT NOT CRITICAL
    // $this->tripal_chado_add_organism_stock_count_mview();
    // $this->tripal_chado_add_library_feature_count_mview();
    // $this->tripal_chado_add_organism_feature_count_mview();
    // $this->tripal_chado_add_analysis_organism_mview();
    // $this->tripal_chado_add_cv_root_mview_mview();
    // $this->tripal_chado_add_db2cv_mview_mview();
  }  

  protected function tripal_chado_add_tripal_gff_temp_table() {
    $schema = [
      'table' => 'tripal_gff_temp',
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
  
  /**
   *
   */
  function tripal_chado_add_tripal_gffcds_temp_table() {
    $schema = [
      'table' => 'tripal_gffcds_temp',
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
  
  /**
   *
   */
  function tripal_chado_add_tripal_gffprotein_temp_table() {
    $schema = [
      'table' => 'tripal_gffprotein_temp',
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
  
  /**
   * Creates a temporary table to store obo details while loading an obo file
   *
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
  
  
  /**
   * Creates a materialized view that stores the type & number of stocks per
   * organism
   *
   * @ingroup tripal_stock
   */
  function tripal_chado_add_organism_stock_count_mview() {
    $view_name = 'organism_stock_count';
    $comment = 'Stores the type and number of stocks per organism';
  
    $schema = [
      'description' => $comment,
      'table' => $view_name,
      'fields' => [
        'organism_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'genus' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'species' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'common_name' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => FALSE,
        ],
        'num_stocks' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cvterm_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'stock_type' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'organism_stock_count_idx1' => ['organism_id'],
        'organism_stock_count_idx2' => ['cvterm_id'],
        'organism_stock_count_idx3' => ['stock_type'],
      ],
    ];
  
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
  
    chado_add_mview($view_name, 'tripal_stock', $schema, $sql, $comment, FALSE);
  }
  
  
  /**
   * Adds a materialized view keeping track of the type of features associated
   * with each library
   *
   * @ingroup tripal_library
   */
  function tripal_chado_add_library_feature_count_mview() {
    $view_name = 'library_feature_count';
    $comment = 'Provides count of feature by type that are associated with all libraries';
  
    $schema = [
      'table' => $view_name,
      'description' => $comment,
      'fields' => [
        'library_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'name' => [
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ],
        'num_features' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'feature_type' => [
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'library_feature_count_idx1' => ['library_id'],
      ],
    ];
  
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
  
    chado_add_mview($view_name, 'tripal_library', $schema, $sql, $comment, FALSE);
  }
  
  
  /**
   *
   */
  
  
  /**
   * Creates a materialized view that stores the type & number of features per
   * organism
   *
   * @ingroup tripal_feature
   */
  function tripal_chado_add_organism_feature_count_mview() {
    $view_name = 'organism_feature_count';
    $comment = 'Stores the type and number of features per organism';
  
    $schema = [
      'description' => $comment,
      'table' => $view_name,
      'fields' => [
        'organism_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'genus' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'species' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'common_name' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => FALSE,
        ],
        'num_features' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cvterm_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'feature_type' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'organism_feature_count_idx1' => ['organism_id'],
        'organism_feature_count_idx2' => ['cvterm_id'],
        'organism_feature_count_idx3' => ['feature_type'],
      ],
    ];
  
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
  
    chado_add_mview($view_name, 'tripal_feature', $schema, $sql, $comment, FALSE);
  }
  
  
  /**
   * Creates a view showing the link between an organism & it's analysis through
   * associated features.
   *
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
    $schema = [
      'table' => $view_name,
      'description' => $comment,
      'fields' => [
        'analysis_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'organism_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'networkmod_qtl_indx0' => ['analysis_id'],
        'networkmod_qtl_indx1' => ['organism_id'],
      ],
      'foreign keys' => [
        'analysis' => [
          'table' => 'analysis',
          'columns' => [
            'analysis_id' => 'analysis_id',
          ],
        ],
        'organism' => [
          'table' => 'organism',
          'columns' => [
            'organism_id' => 'organism_id',
          ],
        ],
      ],
    ];
  
    // add the view
    chado_add_mview($view_name, 'tripal_analysis', $schema, $sql, $comment, FALSE);
  }
  
  /**
   * Add a materialized view that maps cv to db records.
   *
   * This is needed for viewing cv trees
   *
   */
  function tripal_chado_add_db2cv_mview_mview() {
    $mv_name = 'db2cv_mview';
    $comment = 'A table for quick lookup of the vocabularies and the databases they are associated with.';
    $schema = [
      'table' => $mv_name,
      'description' => $comment,
      'fields' => [
        'cv_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cvname' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'db_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'dbname' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'num_terms' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'cv_id_idx' => ['cv_id'],
        'cvname_idx' => ['cvname'],
        'db_id_idx' => ['db_id'],
        'dbname_idx' => ['db_id'],
      ],
    ];
  
    $sql = "
      SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
        COUNT(CVT.cvterm_id) as num_terms
      FROM cv CV
        INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
        INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN db DB on DB.db_id = DBX.db_id
      WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
      GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
      ORDER BY DB.name
    ";
  
    // Create the MView
    chado_add_mview($mv_name, 'tripal_chado', $schema, $sql, $comment, FALSE);
  }
  
  /**
   * Add a materialized view of root terms for all chado cvs.
   *
   * This is needed for viewing cv trees
   *
   */
  function tripal_chado_add_cv_root_mview_mview() {
    $mv_name = 'cv_root_mview';
    $comment = 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees';
    $schema = [
      'table' => $mv_name,
      'description' => $comment,
      'fields' => [
        'name' => [
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ],
        'cvterm_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cv_id' => [
          'size' => 'big',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cv_name' => [
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'cv_root_mview_indx1' => ['cvterm_id'],
        'cv_root_mview_indx2' => ['cv_id'],
      ],
    ];
  
    $sql = "
      SELECT DISTINCT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name
      FROM cvterm CVT
        LEFT JOIN cvterm_relationship CVTR ON CVT.cvterm_id = CVTR.subject_id
        INNER JOIN cvterm_relationship CVTR2 ON CVT.cvterm_id = CVTR2.object_id
      INNER JOIN cv CV on CV.cv_id = CVT.cv_id
      WHERE CVTR.subject_id is NULL and 
        CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
    ";
  
    // Create the MView
    chado_add_mview($mv_name, 'tripal_chado', $schema, $sql, $comment, FALSE);
  }
  
  

  /**
   * For Chado v1.1 Tripal provides some new custom tables.
   *
   * For Chado v1.2 or greater these tables are not needed as they are part of the
   * schema update.
   */
  protected function add_v1_1_custom_tables() {
    $this->tripal_chado_add_analysisfeatureprop_table();
  }

  /**
   * Create a legacy custom chado table (analysisfeatureprop) to store properties
   * of analysisfeature links.
   */
  protected function tripal_chado_add_analysisfeatureprop_table() {
    // Create analysisfeatureprop table in chado.  This is needed for Chado
    // version 1.11, the table exists in Chado 1.2.
    $connection_chado = \Drupal::service('tripal_chado.database');
    $schema = $connection_chado->schema();
    if (!$schema->tableExists('analysisfeatureprop')) {
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
      $connection_chado->query($sql, []);
    }
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
