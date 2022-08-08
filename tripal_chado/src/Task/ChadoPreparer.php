<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalVocabTerms\TripalTerm;

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
   * A connection to the Chado database.
   *
   * @var \Drupal\tripal\TripalDBX\TripalDbxConnection
   */
  protected $chado = NULL;

  /**
   * A connection to the public Drupal database.
   *
   * @var object
   */
  protected $public = NULL;

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

    // Make sure we use the specified Chado schema.
    $schema_name = $this->outputSchemas[0]->getSchemaName();
    $this->chado = \Drupal::service('tripal_chado.database');
    $this->chado->setSchemaName($schema_name);
    $this->public = \Drupal::database();

    try
    {
      $this->setProgress(0.1);
      $this->logger->notice("Creating Tripal Materialized Views and Custom Tables...");
      $chado_version = $this->chado->getVersion();

      if ($chado_version == '1.3') {
        $this->add_vx_x_custom_tables();
        $this->fix_v1_3_custom_tables();
      }

      $this->setProgress(0.2);
      $this->logger->notice("Loading ontologies...");
      $this->addOntologies();
      $this->importOntologies();

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
      $this->createGeneralContentTypes();
      $this->createGenomicContentTypes();
      $this->createGeneticContentTypes();
      $this->createGermplasmContentTypes();
      $this->createExpressionContentTypes();

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

    // Update the featuremap_dbxref table by adding an is_current field.
    if (!chado_column_exists('featuremap_dbxref', 'is_current')) {
      $this->chado->query("ALTER TABLE {featuremap_dbxref} ADD COLUMN is_current boolean DEFAULT true NOT NULL;");
    }

    // Remove the previously managed custom tables from the
    // tripal_custom_tables table.
    // \Drupal::database()->select
    $db = \Drupal::database();
    $table_names = [
      'analysisfeatureprop',
      'featuremap_dbxref',
      'contactprop',
      'featuremapprop',
      'featureposprop',
      'pubauthor_contact',
    ];
    for ($i=0; $i<count($table_names); $i++) {
      $table_name = $table_names[$i];
      $db->delete('tripal_custom_tables')
      ->condition('table_name', $table_name)
      ->execute();
    }
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

    chado_create_custom_table('tripal_gff_temp', $schema, TRUE, NULL,
      FALSE, $this->chado);
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
    chado_create_custom_table('tripal_gffcds_temp', $schema, TRUE, NULL,
      FALSE, $this->chado);
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
    chado_create_custom_table('tripal_gffprotein_temp', $schema, TRUE, NULL,
      FALSE, $this->chado);
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
      $this->chado->query($sql);
      $sql = "CREATE INDEX tripal_obo_temp_idx0 ON {tripal_obo_temp} USING btree (id)";
      $this->chado->query($sql);
      $sql = "CREATE INDEX tripal_obo_temp_idx1 ON {tripal_obo_temp} USING btree (type)";
      $this->chado->query($sql);
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
    chado_add_mview($mv_name, 'tripal_chado', $schema, $sql, $comment, FALSE, $this->chado);
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
    chado_add_mview($mv_name, 'tripal_chado', $schema, $sql, $comment, FALSE, $this->chado);
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
    $schema = $this->chado->schema();
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
      $this->chado->query($sql);
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
   * Gets a controlled vocabulary IDspace object.
   *
   * @param string $name
   *   The name of the IdSpace
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase
   */
  private function getIdSpace($name) {
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idsmanager->loadCollection($name, 'chado_id_space');
    if (!$idSpace) {
      $idSpace = $idsmanager->createCollection($name, 'chado_id_space');
    }
    return $idSpace;
  }

  /**
   * Gets a controlled voabulary object.
   *
   * @param string $name
   *   The name of the vocabulary
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalVocabularyBase
   */
  private function getVocabulary($name) {
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocabulary = $vmanager->loadCollection($name, 'chado_vocabulary');
    if (!$vocabulary) {
      $vocabulary = $vmanager->createCollection($name, 'chado_vocabulary');
    }
    return $vocabulary;
  }

  /**
   * Gets a term by its idSpace and accession
   *
   * @param string $idSpace
   *   The Id space name for the term.
   * @param string $accession
   *   The accession for the term.
   * @return TripalTerm|NULL
   *   A tripal term object.
   */
  private function getTerm($idSpace, $accession, $vocabulary = NULL) {
    $id = $this->getIdSpace($idSpace);
    if ($vocabulary) {
      $id->setDefaultVocabulary($vocabulary);
    }
    return $id->getTerm($accession);
  }

  /**
   * Adds the rdfs vocabulary and IdSpace.
   */
  private function addOntologyRDFS() {
    $vocab = $this->getVocabulary('rdfs');
    $vocab->setLabel('Resource Description Framework Schema');
    $idspace = $this->getIdSpace('rdfs');
    $idspace->setDescription('Resource Description Framework Schema');
    $idspace->setUrlPrefix('http://www.w3.org/2000/01/rdf-schema#{accession}');
    $idspace->setDefaultVocabulary('rdfs');
    $vocab->addIdSpace('rdfs');
    $vocab->setUrl('https://www.w3.org/TR/rdf-schema/');

    $idspace->saveTerm(new TripalTerm([
      'name' => 'comment',
      'accession' => 'comment',
      'idSpace' => 'rdfs',
      'vocabulary' => 'rdfs',
      'definition' => 'A human-readable description of a resource.',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'name' => 'type',
      'accession' => 'type',
      'idSpace' => 'rdfs',
      'vocabulary' => 'rdfs',
      'definition' => 'The type of resource.',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'name' => 'label',
      'accession' => 'label',
      'idSpace' => 'rdfs',
      'vocabulary' => 'rdfs',
      'definition' => 'A human-readable version of a resource\'s name.',
    ]));

  }

  /**
   * Adds the relationship ontology vocabulary and IdSpace.
   */
  private function addOntologyRO() {
    $vocab = $this->getVocabulary('ro');
    $vocab->setLabel('Relationship Ontology (legacy)');
    $idspace = $this->getIdSpace('RO');
    $idspace->setDescription('Relationship Ontology (legacy)');
    $idspace->setURLPrefix("cv/lookup/RO/{accession}	");
    $idspace->setDefaultVocabulary('ro');
    $vocab->addIdSpace('RO');
    $vocab->setUrl('cv/lookup/RO');
  }

  /**
   * Adds the Gene Ontology vocabulary and IdSpace.
   */
  private function addOntologyGO() {
    $cc_vocab = $this->getVocabulary('cellular_component');
    $bp_vocab = $this->getVocabulary('biological_process');
    $mf_vocab = $this->getVocabulary('molecular_function');
    $cc_vocab->setLabel('Gene Ontology Cellular Component Vocabulary');
    $bp_vocab->setLabel('Gene Ontology Biological Process Vocabulary');
    $mf_vocab->setLabel('Gene Ontology Molecular Function Vocabulary');
    $idspace = $this->getIdSpace('GO');
    $idspace->setDescription("The Gene Ontology (GO) knowledgebase is the world’s largest source of information on the functions of genes");
    $idspace->setURLPrefix("http://amigo.geneontology.org/amigo/term/{db}:{accession}");
    $idspace->setDefaultVocabulary('cellular_component');
    $cc_vocab->addIdSpace('GO');
    $bp_vocab->addIdSpace('GO');
    $mf_vocab->addIdSpace('GO');
    $cc_vocab->setURL('http://geneontology.org/');
    $bp_vocab->setURL('http://geneontology.org/');
    $mf_vocab->setURL('http://geneontology.org/');
  }

  /**
   * Adds the Sequence Ontology vocabulary and IdSpace.
   */
  private function addOntologySO() {
    $vocab = $this->getVocabulary('sequence');
    $vocab->setLabel('The Sequence Ontology');
    $idspace = $this->getIdSpace('SO');
    $idspace->setDescription("The Sequence Ontology");
    $idspace->setURLPrefix("http://www.sequenceontology.org/browser/current_svn/term/{db}:{accession}");
    $idspace->setDefaultVocabulary('sequence');
    $vocab->addIdSpace('SO');
    $vocab->setURL('http://www.sequenceontology.org');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000110',
      'name' => 'sequence_feature',
      'idSpace' => 'SO',
      'vocabulary' => 'sequence',
      'definition' => 'Any extent of continuous biological sequence.',
    ]));
    //chado_associate_semweb_term(NULL, 'feature_id', $term);
  }

  /**
   * Adds the Taxonomic Rank Ontology vocabulary and IdSpace.
   */
  private function addOntologyTaxRank() {
    $vocab = $this->getVocabulary('taxonomic_rank');
    $vocab->setLabel('Taxonomic Rank');
    $idspace = $this->getIdSpace('TAXRANK');
    $idspace->setDescription("A vocabulary of taxonomic ranks (species, family, phylum, etc)");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/{db}_{accession}");
    $idspace->setDefaultVocabulary('taxonomic_rank');
    $vocab->addIdSpace('TAXRANK');
    $vocab->setURL('http://www.obofoundry.org/ontology/taxrank.html');

    //$term = chado_get_cvterm(['id' => 'TAXRANK:0000005']);
    //chado_associate_semweb_term('organism', 'genus', $term);

    //$term = chado_get_cvterm(['id' => 'TAXRANK:0000006']);
    //chado_associate_semweb_term('organism', 'species', $term);

    //$term = chado_get_cvterm(['id' => 'TAXRANK:0000045']);
    //chado_associate_semweb_term('organism', 'infraspecific_name', $term);
  }

  /**
   * Adds the Tripal Contact vocabulary and IdSpace.
   */
  private function addOntologyTContact() {
    $vocab = $this->getVocabulary('tripal_contact');
    $vocab->setLabel('Tripal Contact Ontology');
    $idspace = $this->getIdSpace('TCONTACT');
    $idspace->setDescription("Tripal Contact Ontology. A temporary ontology until a more formal appropriate ontology an be identified.");
    $idspace->setURLPrefix("cv/lookup/TCONTACT/{accession}	");
    $idspace->setDefaultVocabulary('tripal_contact');
    $vocab->addIdSpace('TCONTACT');
    $vocab->setURL('cv/lookup/TCONTACT');
  }
  /**
   * Adds the Tripal Pub vocabulary and IdSpace..
   */
  private function addOntologyTPub() {
    $vocab = $this->getVocabulary('tripal_pub');
    $vocab->setLabel('Tripal Publication Ontology');
    $idspace = $this->getIdSpace('TPUB');
    $idspace->setDescription("Tripal Publication Ontology. A temporary ontology until a more formal appropriate ontology an be identified.");
    $idspace->setURLPrefix("cv/lookup/TPUB/{accession}	");
    $idspace->setDefaultVocabulary('tripal_pub');
    $vocab->addIdSpace('TPUB');
    $vocab->setURL('cv/lookup/TPUB');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000002',
      'name' => 'Publication',
      'idSpace' => 'TPUB',
      'vocabulary' => 'tripal_pub',
      'definition' => '',
    ]));

    //$term = chado_get_cvterm(['id' => 'TPUB:0000039']);
    //chado_associate_semweb_term('pub', 'title', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000243']);
    //chado_associate_semweb_term('pub', 'volumetitle', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000042']);
    //chado_associate_semweb_term('pub', 'volume', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000256']);
    //chado_associate_semweb_term('pub', 'series_name', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000043']);
    //chado_associate_semweb_term('pub', 'issue', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000059']);
    //chado_associate_semweb_term('pub', 'pyear', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000044']);
    //chado_associate_semweb_term('pub', 'pages', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000244']);
    //chado_associate_semweb_term('pub', 'publisher', $term);

    //$term = chado_get_cvterm(['id' => 'TPUB:0000245']);
    //chado_associate_semweb_term('pub', 'pubplace', $term);
  }

  /**
   * Adds the friend of a friend database and terms.
   */
  private function addOntologyFOAF() {
    $vocab = $this->getVocabulary('foaf');
    $vocab->setLabel('Friend of a Friend. A dictionary of people-related terms that can be used in structured data).');
    $idspace = $this->getIdSpace('foaf');
    $idspace->setDescription("Friend of a Friend");
    $idspace->setURLPrefix("http://xmlns.com/foaf/spec/#");
    $idspace->setDefaultVocabulary('foaf');
    $vocab->addIdSpace('foaf');
    $vocab->setURL('http://www.foaf-project.org/');
  }

  /**
   * Adds the Hydra vocabulary
   */
  private function addOntologyHydra() {

    $vocab = $this->getVocabulary('hydra');
    $vocab->setLabel('A Vocabulary for Hypermedia-Driven Web APIs.');
    $idspace = $this->getIdSpace('hydra');
    $idspace->setDescription("A Vocabulary for Hypermedia-Driven Web APIs");
    $idspace->setURLPrefix("http://www.w3.org/ns/hydra/core#{accession}");
    $idspace->setDefaultVocabulary('hydra');
    $vocab->addIdSpace('hydra');
    $vocab->setURL('http://www.w3.org/ns/hydra/core');

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Collection',
      'accession' => 'Collection',
      'idSpace' => 'hydra',
      'vocabulary' => 'hydra',
      'definition' => 'A collection holding references to a number of related resources.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'member',
      'accession' => 'member',
      'idSpace' => 'hydra',
      'vocabulary' => 'hydra',
      'definition' => 'A member of the collection',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'name' => 'description',
      'accession' => 'description',
      'idSpace' => 'hydra',
      'vocabulary' => 'hydra',
      'definition' => 'A description.',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'name' => 'totalItems',
      'accession' => 'totalItems',
      'idSpace' => 'hydra',
      'vocabulary' => 'hydra',
      'definition' => 'The total number of items referenced by a collection.',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'name' => 'title',
     'accession' => 'title',
      'idSpace' => 'hydra',
      'vocabulary' => 'hydra',
      'definition' => 'A title, often used along with a description.',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'accession' => 'PartialCollectionView',
      'name' => 'PartialCollectionView',
      'idSpace' => 'hydra',
      'vocabulary' => 'hydra',
      'definition' => 'A PartialCollectionView describes a partial view of a Collection. Multiple PartialCollectionViews can be connected with the the next/previous properties to allow a client to retrieve all members of the collection.',
    ]));
  }

  /**
   * Adds the RDFS database and terms.
   */
  private function addOntologyRDF() {
    $vocab = $this->getVocabulary('rdf');
    $vocab->setLabel('Resource Description Framework');
    $idspace = $this->getIdSpace('rdf');
    $idspace->setDescription("Resource Description Framework");
    $idspace->setURLPrefix("http://www.w3.org/1999/02/22-rdf-syntax-ns#");
    $idspace->setDefaultVocabulary('rdf');
    $vocab->addIdSpace('rdf');
    $vocab->setURL('http://www.w3.org/1999/02/22-rdf-syntax-ns');
  }

  /**
   * Adds the Schema.org database and terms.
   */
  private function addOntologySchema() {

    $vocab = $this->getVocabulary('schema');
    $vocab->setLabel('Schema.org. Schema.org is sponsored by Google, Microsoft, Yahoo and Yandex. The vocabularies are developed by an open community process.');
    $idspace = $this->getIdSpace('schema');
    $idspace->setDescription("Schema.org");
    $idspace->setURLPrefix("https://schema.org/{accession}");
    $idspace->setDefaultVocabulary('schema');
    $vocab->addIdSpace('schema');
    $vocab->setURL('https://schema.org/');


    $idspace->saveTerm(new TripalTerm([
      'accession' => 'name',
      'name' => 'name',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'The name of the item.',
    ]));
    //chado_associate_semweb_term(NULL, 'name', $term);
    //chado_associate_semweb_term('analysis', 'sourcename', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'alternateName',
      'name' => 'alternateName',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'An alias for the item.',
    ]));
    //chado_associate_semweb_term(NULL, 'synonym_id', $term);
    //chado_associate_semweb_term('cvtermsynonym', 'synonym', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'comment',
      'name' => 'comment',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'Comments, typically from users.',
    ]));
    //chado_associate_semweb_term(NULL, 'comment', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'description',
      'name' => 'description',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'A description of the item.',
    ]));
    //chado_associate_semweb_term(NULL, 'description', $term);
    //chado_associate_semweb_term('organism', 'comment', $term);
    //chado_associate_semweb_term('protocol', 'protocoldescription', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'publication',
      'name' => 'publication',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'A publication event associated with the item.',
    ]));
    //chado_associate_semweb_term(NULL, 'pub_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'url',
      'name' => 'url',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'URL of the item.',
    ]));
    //chado_associate_semweb_term('db', 'url', $term);

    // Typically the type_id field is used for distinguishing between records
    // but in the case that it isn't then we need to associate a term with it
    // An entity already has a type so if that type is not dicated by the
    // type_id field then what is in the type_id should therefore be an
    // "additionalType".  Therefore we need to add and map this term to all
    // of the appropriate type_id fields.
    $idspace->saveTerm(new TripalTerm([
      'accession' => 'additionalType',
      'name' => 'additionalType',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'An additional type for the item, typically used for adding more specific types from external vocabularies in microdata syntax. This is a relationship between something and a class that the thing is in.',
    ]));
//     $tables = chado_get_table_names(TRUE);
//     foreach ($tables as $table) {
//       $schema = chado_get_schema($table);
//       // The type_id for the organism is infraspecific type, so don't make
//       // the association for that type.
//       if ($table == 'organism') {
//         continue;
//       }
//       if (in_array("type_id", array_keys($schema['fields']))) {
//         //chado_associate_semweb_term($table, 'type_id', $term);
//       }
//     }

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'ItemPage',
      'name' => 'ItemPage',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'definition' => 'A page devoted to a single item, such as a particular product or hotel.',
    ]));
  }

  /**
   * Adds the Sample processing and separation techniques database and terms.
   */
  private function addOntologySEP() {

    $vocab = $this->getVocabulary('sep');
    $vocab->setLabel('A structured controlled vocabulary for the annotation of sample processing and separation techniques in scientific experiments.');
    $idspace = $this->getIdSpace('sep');
    $idspace->setDescription("Sample processing and separation techniques.");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/{db}_{accession}");
    $idspace->setDefaultVocabulary('sep');
    $vocab->addIdSpace('sep');
    $vocab->setURL('http://psidev.info/index.php?q=node/312');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '00195',
      'name' => 'biological sample',
      'idSpace' => 'sep',
      'vocabulary' => 'sep',
      'definition' => 'A biological sample analysed by a particular technology.',
    ]));
    //chado_associate_semweb_term(NULL, 'biomaterial_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '00101',
      'name' => 'protocol',
      'idSpace' => 'sep',
      'vocabulary' => 'sep',
      'definition' => 'A protocol is a process which is a parameterizable description of a process.',
    ]));
    //chado_associate_semweb_term(NULL, 'protocol_id', $term);
    //chado_associate_semweb_term(NULL, 'nd_protocol_id', $term);
  }

  /**
   * Adds the SemanticScience database and terms.
   */
  private function addOntologySIO() {

    $vocab = $this->getVocabulary('SIO');
    $vocab->setLabel('The Semanticscience Integrated Ontology (SIO) provides a simple, integrated ontology of types and relations for rich description of objects, processes and their attributes.');
    $idspace = $this->getIdSpace('SIO');
    $idspace->setDescription("Semanticscience Integrated Ontology");
    $idspace->setURLPrefix("http://semanticscience.org/resource/{db}_{accession}");
    $idspace->setDefaultVocabulary('SIO');
    $vocab->addIdSpace('SIO');
    $vocab->setURL('http://sio.semanticscience.org/');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '000493',
      'name' => 'clause',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'A clause consists of a subject and a predicate.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => '000631',
      'name' => 'references',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'references is a relation between one entity and the entity that it makes reference to by name, but is not described by it.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => '000056',
      'name' => 'position',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'A measurement of a spatial location relative to a frame of reference or other objects.',
    ]));
    //chado_associate_semweb_term('featurepos', 'mappos', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '001166',
      'name' => 'annotation',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'An annotation is a written explanatory or critical description, or other in-context information (e.g., pattern, motif, link), that has been associated with data or other types of information.',
    ]));
    //chado_associate_semweb_term('feature_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('analysis_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('cell_line_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('environment_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('expression_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('library_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('organism_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('phenotype_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('stock_cvterm', 'cvterm_id', $term);
    //chado_associate_semweb_term('stock_relationship_cvterm', 'cvterm_id', $term);


    $idspace->saveTerm(new TripalTerm([
      'accession' => '000281',
      'name' => 'negation',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'NOT is a logical operator in that has the value true if its operand is false.',
    ]));
    //chado_associate_semweb_term('feature_cvterm', 'is_not', $term);
    //chado_associate_semweb_term('analysis_cvterm', 'is_not', $term);
    //chado_associate_semweb_term('organism_cvterm', 'is_not', $term);
    //chado_associate_semweb_term('stock_cvterm', 'is_not', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '001080',
      'name' => 'vocabulary',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'A vocabulary is a collection of terms.',
    ]));
    //chado_associate_semweb_term('cvterm', 'cv_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '001323',
      'name' => 'email address',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'an email address is an identifier to send mail to particular electronic mailbox.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => '001007',
      'name' => 'assay',
      'vocabulary' => 'SIO',
      'idSpace' => 'SIO',
      'definition' => 'An assay is an investigative (analytic) procedure in ' .
        'laboratory medicine, pharmacology, environmental biology, and ' .
        'molecular biology for qualitatively assessing or quantitatively ' .
        'measuring the presence or amount or the functional activity of a ' .
        'target entity (the analyte) which can be a drug or biochemical ' .
        'substance or a cell in an organism or organic sample.',
    ]));
    //chado_associate_semweb_term(NULL, 'assay_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '010054',
      'name' => 'cell line',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'A cell line is a collection of genetically identifical cells.',
    ]));
    //chado_associate_semweb_term(NULL, 'cell_line_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '001066',
      'name' => 'study',
      'idSpace' => 'SIO',
      'vocabulary' => 'SIO',
      'definition' => 'A study is a process that realizes the steps of a study design.',
    ]));
    //chado_associate_semweb_term(NULL, 'study_id', $term);
  }

  /**
   * Adds the Crop Ontology terms.
   */
  private function addOntologyCO010() {
    $vocab = $this->getVocabulary('germplasm_ontology');
    $vocab->setLabel('GCP germplasm ontology');
    $idspace = $this->getIdSpace('CO_010');
    $idspace->setDescription('Crop Germplasm Ontology');
    $idspace->setUrlPrefix('http://www.cropontology.org/terms/CO_010:{accession}');
    $idspace->setDefaultVocabulary('germplasm_ontology');
    $vocab->addIdSpace('CO_010');
    $vocab->setUrl('http://www.cropontology.org/get-ontology/CO_010');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000044',
      'name' => 'accession',
      'idSpace' => 'CO_010',
      'vocabulary' => 'germplasm_ontology',
      'definition' => '',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000255',
      'name' => 'generated germplasm',
      'idSpace' => 'CO_010',
      'vocabulary' => 'germplasm_ontology',
      'definition' => '',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000029',
      'name' => 'cultivar',
      'idSpace' => 'CO_010',
      'vocabulary' => 'germplasm_ontology',
      'definition' => '',
    ]));
    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000162',
      'name' => '414 inbred line',
      'idSpace' => 'CO_010',
      'vocabulary' => 'germplasm_ontology',
      'definition' => '',
    ]));
  }

  /**
   * Adds the DC database.
   */
  private function addOntologyDC() {
    $vocab = $this->getVocabulary('dc');
    $vocab->setLabel('DCMI Metadata Terms');
    $idspace = $this->getIdSpace('dc');
    $idspace->setDescription('DCMI Metadata Terms');
    $idspace->setUrlPrefix('http://purl.org/dc/terms/{accession}');
    $idspace->setDefaultVocabulary('dc');
    $vocab->addIdSpace('dc');
    $vocab->setUrl('http://purl.org/dc/dcmitype/');

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'Service',
      'name' => 'Service',
      'idSpace' => 'dc',
      'vocabulary' => 'dc',
      'definition' => 'A system that provides one or more functions.',
    ]));
  }

  /**
   * Adds the EDAM database and terms.
   */
  private function addOntologyEDAM() {

    $vocab = $this->getVocabulary('EDAM');
    $vocab->setLabel('Bioscientific data analysis ontology');

    $data_idspace = $this->getIdSpace('data');
    $data_idspace->setDescription("Bioinformatics operations, data types, formats, identifiers and topics.");
    $data_idspace->setURLPrefix("http://edamontology.org/{db}_{accession}");
    $data_idspace->setDefaultVocabulary('EDAM');

    $format_idspace = $this->getIdSpace('format');
    $format_idspace->setDescription('A defined way or layout of representing and structuring data in a computer file, blob, string, message, or elsewhere. The main focus in EDAM lies on formats as means of structuring data exchanged between different tools or resources.');
    $format_idspace->setURLPrefix("http://edamontology.org/{db}_{accession}");
    $format_idspace->setDefaultVocabulary('EDAM');

    $operation_idspace = $this->getIdSpace('operation');
    $operation_idspace->setDescription('A function that processes a set of inputs and results in a set of outputs, or associates arguments (inputs) with values (outputs). Special cases are: a) An operation that consumes no input (has no input arguments).');
    $operation_idspace->setURLPrefix("http://edamontology.org/{db}_{accession}");
    $operation_idspace->setDefaultVocabulary('EDAM');

    $topic_idspace = $this->getIdSpace('topic');
    $topic_idspace->setDescription('A category denoting a rather broad domain or field of interest, of study, application, work, data, or technology. Topics have no clearly defined borders between each other.');
    $topic_idspace->setURLPrefix("http://edamontology.org/{db}_{accession}");
    $topic_idspace->setDefaultVocabulary('foaf');

    $vocab->addIdSpace('data');
    $vocab->addIdSpace('format');
    $vocab->addIdSpace('operation');
    $vocab->addIdSpace('data');
    $vocab->setURL('http://edamontology.org/page');

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1249',
      'name' => 'Sequence length',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'The size (length) of a sequence, subsequence or region in a sequence, or range(s) of lengths.',
    ]));
    //chado_associate_semweb_term('feature', 'seqlen', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2190',
      'name' => 'Sequence checksum',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A fixed-size datum calculated (by using a hash function) for a molecular sequence, typically for purposes of error detection or indexing.',
    ]));
    //chado_associate_semweb_term(NULL, 'md5checksum', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2091',
      'name' => 'Accession',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A persistent (stable) and unique identifier, typically identifying an object (entry) from a database.',
    ]));
    //chado_associate_semweb_term(NULL, 'dbxref_id', $term);
    //chado_associate_semweb_term('dbxref', 'accession', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2044',
      'name' => 'Sequence',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'One or more molecular sequences, possibly with associated annotation.',
    ]));
    //chado_associate_semweb_term('feature', 'residues', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => ':0849',
      'name' => 'Sequence record',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A molecular sequence and associated metadata.',
    ]));

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '0842',
      'name' => 'Identifier',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A text token, number or something else which identifies an entity, but which may not be persistent (stable) or unique (the same identifier may identify multiple things).',
    ]));
    //chado_associate_semweb_term(NULL, 'uniquename', $term);
    //chado_associate_semweb_term('assay', 'arrayidentifier', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2976',
      'name' => 'Protein sequence',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'One or more protein sequences, possibly with associated annotation.',
    ]));

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2968',
      'name' => 'Image',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'Biological or biomedical data has been rendered into an image, typically for display on screen.',
    ]));
    //chado_associate_semweb_term(NULL, 'eimage_id', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1274',
      'name' => 'Map',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A map of (typically one) DNA sequence annotated with positional or non-positional features.',
    ]));
    //chado_associate_semweb_term(NULL, 'featuremap_id', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1278',
      'name' => 'Genetic map',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A map showing the relative positions of genetic markers in a nucleic acid sequence, based on estimation of non-physical distance such as recombination frequencies.',
    ]));
    //chado_associate_semweb_term('featuremap', 'featuremap_id', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1280',
      'name' => 'Physical map',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A map of DNA (linear or circular) annotated with physical features or landmarks such as restriction sites, cloned DNA fragments, genes or genetic markers, along with the physical distances between them. Distance in a physical map is measured in base pairs. A physical map might be ordered relative to a reference map (typically a genetic map) in the process of genome sequencing.',
    ]));
    //chado_associate_semweb_term('featuremap', 'featuremap_id', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2012',
      'name' => 'Sequence coordinates',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A position in a map (for example a genetic map), either a single position (point) or a region / interval.',
    ]));

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1056',
      'name' => 'Database name',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'The name of a biological or bioinformatics database.',
    ]));

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1048',
      'name' => 'Database ID',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'An identifier of a biological or bioinformatics database.',
    ]));
    //chado_associate_semweb_term('db', 'name', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '1047',
      'name' => 'URI',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'The name of a biological or bioinformatics database.',
    ]));
    //chado_associate_semweb_term('analysis', 'sourceuri', $term);
    //chado_associate_semweb_term(NULL, 'uri', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '2336',
      'name' => 'Translation phase specification',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'Phase for translation of DNA (0, 1 or 2) relative to a fragment of the coding sequence.',
    ]));
    //chado_associate_semweb_term('featureloc', 'phase', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '0853',
      'name' => 'DNA sense specification',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'The strand of a DNA sequence (forward or reverse).',
    ]));
    //chado_associate_semweb_term('featureloc', 'strand', $term);
    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '3002',
      'name' => 'Annotation track',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'Annotation of one particular positional feature on a ' .
        'biomolecular (typically genome) sequence, suitable for import and ' .
        'display in a genome browser. Synonym: Sequence annotation track.',
    ]));
    ////chado_associate_semweb_term('featureloc', 'srcfeature_id', $term);

    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '0872',
      'name' => 'Phylogenetic tree',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'The raw data (not just an image) from which a phylogenetic tree is directly generated or plotted, such as topology, lengths (in time or in expected amounts of variance) and a confidence interval for each length.',
    ]));
    //chado_associate_semweb_term(NULL, 'phylotree_id', $term);
    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '3272',
      'name' => 'Species tree',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A phylogenetic tree that reflects phylogeny of the taxa from which the characters (used in calculating the tree) were sampled.',
    ]));
    $data_idspace->saveTerm(new TripalTerm([
      'accession' => '3271',
      'name' => 'Gene tree',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'definition' => 'A phylogenetic tree that is an estimate of the character\'s phylogeny.',
    ]));
    $operation_idspace->saveTerm(new TripalTerm([
      'accession' => '0567',
      'name' => 'Phylogenetic tree visualisation',
      'idSpace' => 'operation',
      'vocabulary' => 'EDAM',
      'definition' => 'A phylogenetic tree that is an estimate of the character\'s phylogeny.',
    ]));
    $operation_idspace->saveTerm(new TripalTerm([
      'accession' => '0564',
      'name' => 'Sequence visualisation',
      'idSpace' => 'operation',
      'vocabulary' => 'EDAM',
      'definition' => 'Visualise, format or render a molecular sequence or sequences such as a sequence alignment, possibly with sequence features or properties shown.',
    ]));
    $operation_idspace->saveTerm(new TripalTerm([
      'accession' => '0525',
      'name' => 'genome assembly',
      'idSpace' => 'operation',
      'vocabulary' => 'EDAM',
      'definition' => '',
    ]));
    $operation_idspace->saveTerm(new TripalTerm([
      'accession' => '0362',
      'name' => 'Genome annotation ',
      'idSpace' => 'operation',
      'vocabulary' => 'EDAM',
      'definition' => '',
    ]));
    $operation_idspace->saveTerm(new TripalTerm([
      'accession' => '2945',
      'name' => 'Analysis',
      'idSpace' => 'operation',
      'vocabulary' => 'EDAM',
      'definition' => 'Apply analytical methods to existing data of a specific type.',
    ]));
    //chado_associate_semweb_term(NULL, 'analysis_id', $term);

  }

  /**
   * Adds the Experimental Factor Ontology and terms.
   */
  private function addOntologyEFO() {
    $vocab = $this->getVocabulary('efo');
    $vocab->setLabel('The Experimental Factor Ontology (EFO) provides a systematic description of many experimental variables available in EBI databases, and for external projects such as the NHGRI GWAS catalogue. It combines parts of several biological ontologies, such as anatomy, disease and chemical compounds. The scope of EFO is to support the annotation, analysis and visualization of data handled by many groups at the EBI and as the core ontology for OpenTargets.org');
    $idspace = $this->getIdSpace('EFO');
    $idspace->setDescription('Experimental Factor Ontology');
    $idspace->setUrlPrefix('http://www.ebi.ac.uk/efo/{db}_{accession}');
    $idspace->setDefaultVocabulary('efo');
    $vocab->addIdSpace('EFO');
    $vocab->setUrl('http://www.ebi.ac.uk/efo/efo.owl');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000548',
      'name' => 'instrument',
      'idSpace' => 'EFO',
      'vocabulary' => 'efo',
      'definition' => 'An instrument is a device which provides a mechanical or electronic function.',
    ]));
    //chado_associate_semweb_term('protocol', 'hardwaredescription', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000269',
      'name' => 'array design',
      'idSpace' => 'EFO',
      'vocabulary' => 'efo',
      'definition' => 'An instrument design which describes the design of the array.',
    ]));
    //chado_associate_semweb_term('assay', 'arraydesign_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0005522',
      'name' => 'substrate type',
      'idSpace' => 'EFO',
      'vocabulary' => 'efo',
      'definition' => 'Controlled terms for descriptors of types of array substrates.',
    ]));
    //chado_associate_semweb_term('arraydesign', 'substratetype_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0001728',
      'name' => 'array manufacturer',
      'idSpace' => 'EFO',
      'vocabulary' => 'efo',
      'definition' => '',
    ]));
    //chado_associate_semweb_term('arraydesign', 'manufacturer_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000269',
      'name' => 'array design',
      'idSpace' => 'EFO',
      'vocabulary' => 'efo',
      'definition' => 'An instrument design which describes the design of the array.',
    ]));
    //chado_associate_semweb_term('element', 'arraydesign_id', $term);
  }

  /**
   * Adds the Eagle-i Resource Ontology database and terms.
   */
  private function addOntologyERO() {
    $vocab = $this->getVocabulary('ero');
    $vocab->setLabel('The Eagle-I Research Resource Ontology models research resources such instruments. protocols, reagents, animal models and biospecimens. It has been developed in the context of the eagle-i project (http://eagle-i.net/).');
    $idspace = $this->getIdSpace('ERO');
    $idspace->setDescription('The Eagle-I Research Resource Ontology');
    $idspace->setUrlPrefix('http://purl.bioontology.org/ontology/ERO/{db}:{accession}');
    $idspace->setDefaultVocabulary('ero');
    $vocab->addIdSpace('ERO');
    $vocab->setUrl('http://purl.bioontology.org/ontology/ERO');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0001716',
      'name' => 'database',
      'idSpace' => 'ERO',
      'vocabulary' => 'ero',
      'definition' => 'A database is an organized collection of data, today typically in digital form.',
    ]));
    //chado_associate_semweb_term(NULL, 'db_id', $term);
    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000387',
      'name' => 'data acquisition',
      'idSpace' => 'ERO',
      'vocabulary' => 'ero',
      'definition' => 'A technique that samples real world physical conditions and conversion of the resulting samples into digital numeric values that can be manipulated by a computer.',
    ]));
    //chado_associate_semweb_term(NULL, 'acquisition_id', $term);
  }

  /**
   * Adds the Information Artifact Ontology database and terms.
   */
  private function addOntologyOBCS() {
    $vocab = $this->getVocabulary('OBCS');
    $vocab->setLabel('Ontology of Biological and Clinical Statistics');
    $idspace = $this->getIdSpace('OBCS');
    $idspace->setDescription("Ontology of Biological and Clinical Statistics");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/{db}_{accession}");
    $idspace->setDefaultVocabulary('OBCS');
    $vocab->addIdSpace('OBCS');
    $vocab->setURL('https://github.com/obcs/obcs');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000117',
      'name' => 'rank order',
      'idSpace' => 'OBCS',
      'vocabulary' => 'OBCS',
      'definition' => 'A data item that represents an arrangement according to a rank, i.e., the position of a particular case relative to other cases on a defined scale.',
    ]));
    //chado_associate_semweb_term(NULL, 'rank', $term);
  }

  /**
   * Adds the Information Artifact Ontology database and terms.
   */
  private function addOntologyOBI() {
    $vocab = $this->getVocabulary('obi');
    $vocab->setLabel('Ontology for Biomedical Investigation. The Ontology for Biomedical Investigations (OBI) is build in a collaborative, international effort and will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data. This ontology arose from the Functional Genomics Investigation Ontology (FuGO) and will contain both terms that are common to all biomedical investigations, including functional genomics investigations and those that are more domain specific');
    $idspace = $this->getIdSpace('OBI');
    $idspace->setDescription("The Ontology for Biomedical Investigation");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/{db}_{accession}");
    $idspace->setDefaultVocabulary('obi');
    $vocab->addIdSpace('OBI');
    $vocab->setURL('http://obi-ontology.org/page/Main_Page');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0100026',
      'name' => 'organism',
      'idSpace' => 'OBI',
      'vocabulary' => 'obi',
      'definition' => 'A material entity that is an individual living system, such as animal, plant, bacteria or virus, that is capable of replicating or reproducing, growth and maintenance in the right environment. An organism may be unicellular or made up, like humans, of many billions of cells divided into specialized tissues and organs.',
    ]));
    //chado_associate_semweb_term(NULL, 'organism_id', $term);
    //chado_associate_semweb_term('biomaterial', 'taxon_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000070',
      'name' => 'assay',
      'idSpace' => 'OBI',
      'vocabulary' => 'obi',
      'definition' => 'A planned process with the objective to produce information about the material entity that is the evaluant, by physically examining it or its proxies.',
    ]));
  }

  /**
   * Adds the Ontology for genetic interval database and terms.
   */
  private function addOntologyOGI() {
    $vocab = $this->getVocabulary('ogi');
    $vocab->setLabel('Ontology for Biomedical Investigation. The Ontology for Biomedical Investigations (OBI) is build in a collaborative, international effort and will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data. This ontology arose from the Functional Genomics Investigation Ontology (FuGO) and will contain both terms that are common to all biomedical investigations, including functional genomics investigations and those that are more domain specific');
    $idspace = $this->getIdSpace('OGI');
    $idspace->setDescription("Ontology for genetic interval");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/{db}_{accession}");
    $idspace->setDefaultVocabulary('ogi');
    $vocab->addIdSpace('OGI');
    $vocab->setURL('http://purl.bioontology.org/ontology/OGI');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000021',
      'name' => 'location on map',
      'idSpace' => 'OGI',
      'vocabulary' => 'ogi',
      'definition' => '',
    ]));
  }

  /**
   * Adds the Information Artifact Ontology database and terms.
   */
  private function addOntologyIAO() {

    $vocab = $this->getVocabulary('IAO');
    $vocab->setLabel('Information Artifact Ontology');
    $idspace = $this->getIdSpace('IAO');
    $idspace->setDescription('Information Artifact Ontology');
    $idspace->setUrlPrefix('http://purl.obolibrary.org/obo/{db}_{accession}');
    $idspace->setDefaultVocabulary('IAO');
    $vocab->addIdSpace('IAO');
    $vocab->setUrl('https://github.com/information-artifact-ontology/IAO/');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000115',
      'name' => 'definition',
      'vocabulary' => 'IAO',
      'idSpace' => 'IAO',
      'definition' => 'The official OBI definition, explaining the meaning of ' .
        'a class or property. Shall be Aristotelian, formalized and normalized. ' .
        'Can be augmented with colloquial definitions.',
    ]));
    //chado_associate_semweb_term(NULL, 'definition', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000129',
      'name' => 'version number',
      'vocabulary' => 'IAO',
      'idSpace' => 'IAO',
      'definition' => 'A version number is an ' .
        'information content entity which is a sequence of characters ' .
        'borne by part of each of a class of manufactured products or its ' .
        'packaging and indicates its order within a set of other products ' .
        'having the same name.',
    ]));
    //chado_associate_semweb_term('analysis', 'programversion', $term);
    //chado_associate_semweb_term('analysis', 'sourceversion', $term);
    //chado_associate_semweb_term(NULL, 'version', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000064',
      'name' => 'algorithm',
      'idSpace' => 'IAO',
      'vocabulary' => 'IAO',
      'definition' => 'An algorithm is a set of instructions for performing a paticular calculation.',
    ]));
    //chado_associate_semweb_term('analysis', 'algorithm', $term);
  }

  private function addOntologyNull() {
    $vocab = $this->getVocabulary('null');
    $vocab->setLabel('No vocabulary');
    $idspace = $this->getIdSpace('null');
    $idspace->setDescription('No database');
    $idspace->setUrlPrefix('cv/lookup/{db}/{accession}');
    $idspace->setDefaultVocabulary('null');
    $vocab->addIdSpace('null');
    $vocab->setUrl('cv/lookup/null');
  }

  /**
   * Adds terms to the 'local' database.
   */
  private function addOntologyLocal() {

    $vocab = $this->getVocabulary('local');
    $vocab->setLabel('Locally created terms');
    $idspace = $this->getIdSpace('local');
    $idspace->setDescription('Terms created for this site');
    $idspace->setUrlPrefix('cv/lookup/{db}/{accession}');
    $idspace->setDefaultVocabulary('local');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('organism_property');
    $vocab->setLabel('A local vocabulary that contains locally defined properties for organisms');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('analysis_property');
    $vocab->setLabel('A local vocabulary that contains locally defined properties for analyses');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('tripal_phylogeny');
    $vocab->setLabel('Terms used by the Tripal phylotree module for phylogenetic and taxonomic trees');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('feature_relationship');
    $vocab->setLabel('A local vocabulary that contains types of relationships between features');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('feature_property');
    $vocab->setLabel('A local vocabulary that contains properties for genomic features');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('contact_property');
    $vocab->setLabel('A local vocabulary that contains properties for contacts. This can be used if the tripal_contact vocabulary (which is default for contacts in Tripal) is not desired.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('contact_type');
    $vocab->setLabel('A local vocabulary that contains types of contacts. This can be used if the tripal_contact vocabulary (which is default for contacts in Tripal) is not desired.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('tripal_contact');
    $vocab->setLabel('A local vocabulary that contains a heirarchical set of terms for describing a contact. It is intended to be used as the default vocabularies in Tripal for contact types and contact properties.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('contact_relationship');
    $vocab->setLabel('A local vocabulary that contains types of relationships between contacts.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('featuremap_units');
    $vocab->setLabel('A local vocabulary that contains map unit types for the unittype_id column of the featuremap table.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('featurepos_property');
    $vocab->setLabel('A local vocabulary that contains terms map properties.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('featuremap_property');
    $vocab->setLabel('A local vocabulary that contains positional types for the feature positions.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('library_property');
    $vocab->setLabel('A local vocabulary that contains properties for libraries.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('library_type');
    $vocab->setLabel('A local vocabulary that contains terms for types of libraries (e.g. BAC, cDNA, FOSMID, etc).');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('project_property');
    $vocab->setLabel('A local vocabulary that contains properties for projects.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('study_property');
    $vocab->setLabel('A local vocabulary that contains properties for studies.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('project_relationship');
    $vocab->setLabel('A local vocabulary that contains Types of relationships between projects');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('tripal_pub');
    $vocab->setLabel('A local vocabulary that contains a heirarchical set of terms for describing a publication. It is intended to be used as the default vocabularies in Tripal for publication types and contact properties.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('pub_type');
    $vocab->setLabel('A local vocabulary that contains types of publications. This can be used if the tripal_pub vocabulary (which is default for publications in Tripal) is not desired.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('pub_property');
    $vocab->setLabel('A local vocabulary that contains properties for publications. This can be used if the tripal_pub vocabulary (which is default for publications in Tripal) is not desired.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('pub_relationship');
    $vocab->setLabel('A local vocabulary that contains types of relationships between publications.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('stock_relationship');
    $vocab->setLabel('A local vocabulary that contains types of relationships between stocks.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('stock_property');
    $vocab->setLabel('A local vocabulary that contains properties for stocks.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('stock_type');
    $vocab->setLabel('A local vocabulary that contains a list of types for stocks.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('tripal_analysis');
    $vocab->setLabel('A local vocabulary that contains terms used for analyses.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('nd_experiment_types');
    $vocab->setLabel('A local vocabulary that contains terms used for the Natural Diverisity module\'s experiment types.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');

    $vocab = $this->getVocabulary('nd_geolocation_property');
    $vocab->setLabel('A local vocabulary that contains terms used for the Natural Diverisity module\'s geolocation property.');
    $vocab->addIdSpace('local');
    $vocab->setUrl('cv/lookup/local');




    $idspace->saveTerm(new TripalTerm([
      'accession' => 'property',
      'name' => 'property',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'definition' => 'A generic term indicating that represents an attribute, quality or characteristic of something.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'timelastmodified',
      'name' => 'time_last_modified',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'definition' => 'The time at which the record was last modified.',
    ]));
    //chado_associate_semweb_term(NULL, 'timelastmodified', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'timeaccessioned',
      'name' => 'time_accessioned',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'definition' => 'The time at which the record was first added.',
    ]));
    //chado_associate_semweb_term(NULL, 'timeaccessioned', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'timeexecuted',
      'name' => 'time_executed',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'definition' => 'The time when the task was executed.',
    ]));
    //chado_associate_semweb_term(NULL, 'timeexecuted', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'infraspecific_type',
      'name' => 'infraspecific_type',
      'idSpace' => 'local',
      'definition' => 'The connector type (e.g. subspecies, varietas, forma, etc.) for the infraspecific name',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term('organism', 'type_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'abbreviation',
      'name' => 'abbreviation',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'definition' => 'A shortened name (or abbreviation) for the item.',
    ]));
    //chado_associate_semweb_term('organism', 'abbreviation', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'expression',
      'name' => 'expression',
      'idSpace' => 'local',
      'definition' => 'Curated expression data',
      'vocabulary' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'is_analysis',
      'name' => 'is_analysis',
      'idSpace' => 'local',
      'definition' => 'Indicates if this feature was predicted computationally using another feature.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term('feature', 'is_analysis', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'is_obsolete',
      'name' => 'is_obsolete',
      'idSpace' => 'local',
      'definition' => 'Indicates if this record is obsolete.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term(NULL, 'is_obsolete', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'is_current',
      'name' => 'is_current',
      'idSpace' => 'local',
      'definition' => 'Indicates if this record is current.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term(NULL, 'is_current', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'is_internal',
      'name' => 'is_internal',
      'idSpace' => 'local',
      'definition' => 'Indicates if this record is internal and not normally available outside of a local setting.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term(NULL, 'is_internal', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'miniref',
      'name' => 'Mini-ref',
      'idSpace' => 'local',
      'definition' => 'A small in-house unique identifier for a publication.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term('pub', 'miniref', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'array_batch_identifier',
      'name' => 'Array Batch Identifier',
      'idSpace' => 'local',
      'definition' => 'A unique identifier for an array batch.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term('assay', 'arraybatchidentifier', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'relationship_subject',
      'name' => 'clause subject',
      'idSpace' => 'local',
      'definition' => 'The subject of a relationship clause.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term(NULL, 'subject_id', $term);
    //chado_associate_semweb_term(NULL, 'subject_reagent_id', $term);
    //chado_associate_semweb_term(NULL, 'subject_project_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'relationship_object',
      'name' => 'clause predicate',
      'idSpace' => 'local',
      'definition' => 'The object of a relationship clause.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term(NULL, 'object_id', $term);
    //chado_associate_semweb_term(NULL, 'object_reagent_id', $term);
    //chado_associate_semweb_term(NULL, 'object_project_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'relationship_type',
      'name' => 'relationship type',
      'idSpace' => 'local',
      'definition' => 'The relationship type.',
      'vocabulary' => 'local',
    ]));
    //chado_associate_semweb_term('acquisition_relationship', 'type_id', $term);
    //chado_associate_semweb_term('biomaterial_relationship', 'type_id', $term);
    //chado_associate_semweb_term('cell_line_relationship', 'type_id', $term);
    //chado_associate_semweb_term('contact_relationship', 'type_id', $term);
    //chado_associate_semweb_term('element_relationship', 'type_id', $term);
    //chado_associate_semweb_term('elementresult_relationship', 'type_id', $term);
    //chado_associate_semweb_term('feature_relationship', 'type_id', $term);
    //chado_associate_semweb_term('nd_reagent_relationship', 'type_id', $term);
    //chado_associate_semweb_term('phylonode_relationship', 'type_id', $term);
    //chado_associate_semweb_term('project_relationship', 'type_id', $term);
    //chado_associate_semweb_term('pub_relationship', 'type_id', $term);
    //chado_associate_semweb_term('quantification_relationship', 'type_id', $term);
    //chado_associate_semweb_term('stock_relationship', 'type_id', $term);
    //chado_associate_semweb_term('cvterm_relationship', 'type_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'rank',
      'name' => 'rank',
      'idSpace' => 'local',
      'definition' => 'A taxonmic rank',
      'vocabulary' => 'organism_property',
    ]));

    $terms = [
      'lineage',
      'genetic_code',
      'genetic_code_name',
      'mitochondrial_genetic_code',
      'mitochondrial_genetic_code_name',
      'division',
      'genbank_common_name',
      'synonym',
      'other_name',
      'equivalent_name',
      'anamorph',
    ];
    foreach ($terms as $term) {
      $idspace->saveTerm(new TripalTerm([
        'name' => $term,
        'accession' => $term,
        'definition' => '',
        'idSpace' => 'local',
        'vocabulary' => 'organism_property',
      ]));
    }

    $idspace->saveTerm(new TripalTerm([
      'name' => 'phylo_leaf',
      'accession' => 'phylo_leaf',
      'definition' => 'A leaf node in a phylogenetic tree.',
      'vocabulary' => 'tripal_phylogeny',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'phylo_root',
      'accession' => 'phylo_root',
      'definition' => 'The root node of a phylogenetic tree.',
      'vocabulary' => 'tripal_phylogeny',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'phylo_interior',
      'accession' => 'phylo_interior',
      'definition' => 'An interior node in a phylogenetic tree.',
      'vocabulary' => 'tripal_phylogeny',
      'idSpace' => 'local',
    ]));

    // Add the terms used to identify nodes in the tree.
    // DEPRECATED: use EDAM's data 'Species tree' term instead.
    $idspace->saveTerm(new TripalTerm([
      'name' => 'taxonomy',
      'accession' => 'taxonomy',
      'definition' => 'A term used to indicate if a phylotree is a taxonomic tree',
      'vocabulary' => 'tripal_phylogeny',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Project Description',
      'accession' => 'Project Description',
      'definition' => 'Description of a project',
      'vocabulary' => 'project_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Project Type',
      'accession' => 'Project Type',
      'definition' => 'A type of project',
      'vocabulary' => 'project_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Genotyping',
      'accession' => 'Genotyping',
      'definition' => 'An experiment where genotypes of individuals are identified.',
      'vocabulary' => 'nd_experiment_types',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Phenotyping',
      'accession' => 'Phenotyping',
      'definition' => 'An experiment where phenotypes of individuals are identified.',
      'vocabulary' => 'nd_experiment_types',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Location',
      'accession' => 'Location',
      'definition' => 'The name of the location.',
      'vocabulary' => 'nd_geolocation_property',
      'idSpace' => 'local',
    ]));


    $idspace->saveTerm(new TripalTerm([
      'accession' => 'library',
      'name' => 'Library',
      'definition' => 'A group of physical entities organized into a collection',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term(NULL, 'library_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'library_description',
      'name' => 'Library Description',
      'definition' => 'Description of a library',
      'vocabulary' => 'library_property',
      'idSpace' => 'local',
    ]));

    // add cvterms for the map unit types
    $idspace->saveTerm(new TripalTerm([
      'accession' => 'cdna_library',
      'name' => 'cdna_library',
      'definition' => 'cDNA library',
      'vocabulary' => 'library_type',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'bac_library',
      'name' => 'bac_library',
      'definition' => 'Bacterial Artifical Chromsome (BAC) library',
      'vocabulary' => 'library_type',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'fosmid_library',
      'name' => 'fosmid_library',
      'definition' => 'Fosmid library',
      'vocabulary' => 'library_type',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'cosmid_library',
      'name' => 'cosmid_library',
      'definition' => 'Cosmid library',
      'vocabulary' => 'library_type',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'yac_library',
      'name' => 'yac_library',
      'definition' => 'Yeast Artificial Chromosome (YAC) library',
      'vocabulary' => 'library_type',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'genomic_library',
      'name' => 'genomic_library',
      'definition' => 'Genomic Library',
      'vocabulary' => 'library_type',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'fasta_definition',
      'accession' => 'fasta_definition',
      'definition' => 'The definition line for a FASTA formatted sequence',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'cM',
      'accession' => 'cM',
      'definition' => 'Centimorgan units',
      'vocabulary' => 'featuremap_units',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'bp',
      'accession' => 'bp',
      'definition' => 'Base pairs units',
      'vocabulary' => 'featuremap_units',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'bin_unit',
      'accession' => 'bin_unit',
      'definition' => 'The bin unit',
      'vocabulary' => 'featuremap_units',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'marker_order',
      'accession' => 'marker_order',
      'definition' => 'Units simply to define marker order.',
      'vocabulary' => 'featuremap_units',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'undefined',
      'accession' => 'undefined',
      'definition' => 'A catch-all for an undefined unit type',
      'vocabulary' => 'featuremap_units',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'start',
      'accession' => 'start',
      'definition' => 'The start coordinate for a map feature.',
      'vocabulary' => 'featurepos_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'stop',
      'accession' => 'stop',
      'definition' => 'The end coordinate for a map feature',
      'vocabulary' => 'featurepos_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Map Dbxref',
      'accession' => 'Map Dbxref',
      'definition' => 'A unique identifer for the map in a remote database.  The ' .
        'format is a database abbreviation and a unique accession separated ' .
        'by a colon.  (e.g. Gramene:tsh1996a)',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Map Type',
      'accession' => 'Map Type',
      'definition' => 'The type of Map (e.g. QTL, Physical, etc.)',
      'vocabulary' => 'featuremap_property',
      'is_relationship' => 0,
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Genome Group',
      'accession' => 'Genome Group',
      'definition' => '',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'URL',
      'accession' => 'URL',
      'definition' => 'A univeral resource locator (URL) reference where the ' .
        'publication can be found.  For maps found online, this would be ' .
        'the web address for the map.',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Population Type',
      'accession' => 'Population Type',
      'definition' => 'A brief description of the population type used to generate ' .
        'the map (e.g. RIL, F2, BC1, etc).',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Population Size',
      'accession' => 'Population Size',
      'definition' => 'The size of the population used to construct the map.',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Methods',
      'accession' => 'Methods',
      'definition' => 'A brief description of the methods used to construct the map.',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Software',
      'accession' => 'Software',
      'definition' => 'The software used to construct the map.',
      'vocabulary' => 'featuremap_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Reference Feature',
      'accession' => 'Reference Feature',
      'definition' => 'A genomic or genetic feature on which other features are mapped.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('featurepos', 'map_feature_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'fmin',
      'name' => 'minimal boundary',
      'definition' => 'The leftmost, minimal boundary in the linear range ' .
        'represented by the feature location. Sometimes this is called ' .
        'start although this is confusing because it does not necessarily ' .
        'represent the 5-prime coordinate.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('featureloc', 'fmin', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'fmax',
      'name' => 'maximal boundary',
      'definition' => 'The rightmost, maximal boundary in the linear range ' .
        'represented by the featureloc. Sometimes this is called end although ' .
        'this is confusing because it does not necessarily represent the ' .
        '3-prime coordinate',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('featureloc', 'fmax', $term);

    $idspace->saveTerm(new TripalTerm([
      'name' => 'analysis_date',
      'accession' => 'analysis_date',
      'definition' => 'The date that an analysis was performed.',
      'vocabulary' => 'tripal_analysis',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'name' => 'analysis_short_name',
      'accession' => 'analysis_short_name',
      'definition' => 'A computer legible (no spaces or special characters) abbreviation for the analysis.',
      'vocabulary' => 'tripal_analysis',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'Analysis Type',
      'name' => 'Analysis Type',
      'definition' => 'The type of analysis that was performed.',
      'vocabulary' => 'analysis_property',
      'idSpace' => 'local',
    ]));

    // Add a term to be used for an inherent 'type_id' for the organism table.
    $idspace->saveTerm(new TripalTerm([
      'accession' => 'analysis',
      'name' => 'analysis',
      'definition' => 'A process as a method of studying the nature of something ' .
        'or of determining its essential features and their relations. ' .
        '(Random House Kernerman Webster\'s College Dictionary, © 2010 K ' .
        'Dictionaries Ltd).',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'source_data',
      'name' => 'source_data',
      'definition' => 'The location where data that is being used come from.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'contact',
      'name' => 'contact',
      'definition' => 'An entity (e.g. individual or organization) through ' .
        'whom a person can gain access to information, favors, ' .
        'influential people, and the like.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('biomaterial', 'biosourceprovider_id', $term);
    //chado_associate_semweb_term(NULL, 'contact_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'relationship',
      'name' => 'relationship',
      'definition' => 'The way in which two things are connected.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'biomaterial',
      'name' => 'biomaterial',
      'definition' => 'A biomaterial represents the MAGE concept of BioSource, BioSample, ' .
        'and LabeledExtract. It is essentially some biological material (tissue, cells, serum) that ' .
        'may have been processed. Processed biomaterials should be traceable back to raw ' .
        'biomaterials via the biomaterialrelationship table.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'array_dimensions',
      'name' => 'array_dimensions',
      'definition' => 'The dimensions of an array.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'array_dimensions', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'element_dimensions',
      'name' => 'element_dimensions',
      'definition' => 'The dimensions of an element.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'element_dimensions', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_of_elements',
      'name' => 'num_of_elements',
      'definition' => 'The number of elements.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_of_elements', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_array_columns',
      'name' => 'num_array_columns',
      'definition' => 'The number of columns in an array.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_array_columns', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_array_rows',
      'name' => 'num_array_rows',
      'definition' => 'The number of rows in an array.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_array_rows', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_grid_columns',
      'name' => 'num_grid_columns',
      'definition' => 'The number of columns in a grid.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_grid_columns', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_grid_rows',
      'name' => 'num_grid_rows',
      'definition' => 'The number of rows in a grid.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_grid_rows', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_sub_columns',
      'name' => 'num_sub_columns',
      'definition' => 'The number of sub columns.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_sub_columns', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'num_sub_rows',
      'name' => 'num_sub_rows',
      'definition' => 'The number of sub rows.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));
    //chado_associate_semweb_term('arraydesign', 'num_sub_rows', $term);

    $idspace->saveTerm(new TripalTerm([
      'name' => 'Study Type',
      'accession' => 'Study Type',
      'definition' => 'A type of study',
      'vocabulary' => 'study_property',
      'idSpace' => 'local',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'Genome Project',
      'name' => 'Genome Project',
      'definition' => 'A project for whole genome analysis that can include assembly and annotation.',
      'vocabulary' => 'local',
      'idSpace' => 'local',
    ]));

  }

  /**
   * Adds the Systems Biology Ontology database and terms.
   */
  private function addOntologySBO() {

    $vocab = $this->getVocabulary('sbo');
    $vocab->setLabel('Systems Biology.  Terms commonly used in Systems Biology, and in particular in computational modeling.');
    $idspace = $this->getIdSpace('SBO');
    $idspace->setDescription("Systems Biology Ontology");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/{db}_{accession}");
    $idspace->setDefaultVocabulary('sbo');
    $vocab->addIdSpace('SBO');
    $vocab->setURL('http://www.ebi.ac.uk/sbo/main/');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000358',
      'name' => 'phenotype',
      'idSpace' => 'SBO',
      'vocabulary' => 'sbo',
      'definition' => 'A biochemical network can generate phenotypes or affects biological processes. Such processes can take place at different levels and are independent of the biochemical network itself.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000554',
      'name' => 'database cross reference',
      'idSpace' => 'SBO',
      'vocabulary' => 'sbo',
      'definition' => 'An annotation which directs one to information contained within a database.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000374',
      'name' => 'relationship',
      'idSpace' => 'SBO',
      'vocabulary' => 'sbo',
      'definition' => 'Connectedness between entities and/or interactions representing their relatedness or influence.',
    ]));
  }

  /**
   * Adds the "Bioinformatics operations, data types, formats, identifiers and
   * topics" database and terms.
   */
  private function addOntologySWO() {

    $vocab = $this->getVocabulary('swo');
    $vocab->setLabel('Bioinformatics operations, data types, formats, identifiers and topics');
    $idspace = $this->getIdSpace('SWO');
    $idspace->setDescription("Bioinformatics operations, data types, formats, identifiers and topics");
    $idspace->setURLPrefix("http://www.ebi.ac.uk/swo/{db}_{accession}");
    $idspace->setDefaultVocabulary('swo');
    $vocab->addIdSpace('SWO');
    $vocab->setURL('http://purl.obolibrary.org/obo/swo');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000001',
      'name' => 'software',
      'idSpace' => 'SWO',
      'vocabulary' => 'swo',
      'definition' => 'Computer software, or generally just software, is any ' .
        'set of machine-readable instructions (most often in the form of a ' .
        'computer program) that conform to a given syntax (sometimes ' .
        'referred to as a language) that is interpretable by a given ' .
        'processor and that directs a computer\'s processor to perform ' .
        'specific operations.',
    ]));
    //chado_associate_semweb_term('analysis', 'program', $term);
    //chado_associate_semweb_term('protocol', 'softwaredescription', $term);
  }



  /**
   * Adds the PUbMed Ontology and terms.
   */
  private function addOntologyPMID() {
    $vocab = $this->getVocabulary('PMID');
    $vocab->setLabel('PubMed.');
    $idspace = $this->getIdSpace('PMID');
    $idspace->setDescription("PubMed");
    $idspace->setURLPrefix("http://www.ncbi.nlm.nih.gov/pubmed/{accession}");
    $idspace->setDefaultVocabulary('PMID');
    $vocab->addIdSpace('PMID');
    $vocab->setURL('http://www.ncbi.nlm.nih.gov/pubmed');
  }


  /**
   * Adds the Uni Ontology database, terms and mappings.
   */
  private function addOntologyUO() {
    $vocab = $this->getVocabulary('uo');
    $vocab->setLabel('Units of Measurement Ontology');
    $idspace = $this->getIdSpace('UO');
    $idspace->setDescription("Units of Measurement Ontology");
    $idspace->setURLPrefix("http://purl.obolibrary.org/obo/UO_{accession}");
    $idspace->setDefaultVocabulary('uo');
    $vocab->addIdSpace('UO');
    $vocab->setURL('http://purl.obolibrary.org/obo/uo');

    $idspace->saveTerm(new TripalTerm([
      'accession' => '0000000',
      'name' => 'unit',
      'idSpace' => 'UO',
      'vocabulary' => 'uo',
      'description' => 'A unit of measurement is a standardized quantity of a physical quality.',
    ]));
    //chado_associate_semweb_term('featuremap', 'unittype_id', $term);
  }


  /**
   * Adds the NCIT vocabulary database and terms.
   */
  private function addOntologyNCIT() {
    $vocab = $this->getVocabulary('ncit');
    $vocab->setLabel('NCI Thesaurus OBO Edition');
    $idspace = $this->getIdSpace('NCIT');
    $idspace->setDescription('The NCIt is a reference terminology that includes broad coverage of the cancer domain, including cancer related diseases, findings and abnormalities. NCIt OBO Edition releases should be considered experimental.');
    $idspace->setUrlPrefix('http://purl.obolibrary.org/obo/{db}_{accession}');
    $idspace->setDefaultVocabulary('ncit');
    $vocab->addIdSpace('NCIT');
    $vocab->setUrl('http://purl.obolibrary.org/obo/ncit.owl');

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C25164',
      'name' => 'Date',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'The particular day, month and year an event has happened or will happen.',
    ]));
    //chado_associate_semweb_term('assay', 'assaydate', $term);
    //chado_associate_semweb_term('acquisition', 'acquisitiondate', $term);
    //chado_associate_semweb_term('quantification', 'quantificationdate', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C48036',
      'name' => 'Operator',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A person that operates some apparatus or machine',
    ]));
    //chado_associate_semweb_term(NULL, 'operator_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C45378',
      'name' => 'Technology Platform',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'The specific version (manufacturer, model, etc.) of a technology that is used to carry out a laboratory or computational experiment.',
    ]));
    //chado_associate_semweb_term('arraydesign', 'platformtype_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C25712',
      'name' => 'Value',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A numerical quantity measured or assigned or computed.',
    ]));
    //chado_associate_semweb_term(NULL, 'value', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C44170',
      'name' => 'Channel',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'An independent acquisition scheme, i.e., a route or conduit through which flows data consisting of one particular measurement using one particular parameter.',
    ]));
    //chado_associate_semweb_term(NULL, 'channel_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C48697',
      'name' => 'Controlled Vocabulary',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A set of terms that are selected and defined based on the requirements set out by the user group, usually a set of vocabulary is chosen to promote consistency across data collection projects. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'cv_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C45559',
      'name' => 'Term',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A word or expression used for some particular thing. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'cvterm_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C80488',
      'name' => 'Expression',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A combination of symbols that represents a value. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'expression_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C16977',
      'name' => 'Phenotype',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'The assemblage of traits or outward appearance of an individual. It is the product of interactions between genes and between genes and the environment. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'phenotype_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C16631',
      'name' => 'Genotype',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'The genetic constitution of an organism or cell, as distinct from its expressed features or phenotype. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'genotype_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C25341',
      'name' => 'Location',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A position, site, or point in space where something can be found. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'nd_geolocation_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C802',
      'name' => 'Reagent',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'Any natural or synthetic substance used in a chemical or biological reaction in order to produce, identify, or measure another substance. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'nd_reagent_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C16551',
      'name' => 'Environment',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'The totality of surrounding conditions. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'environment_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C42765',
      'name' => 'Tree Node',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A term that refers to any individual item or entity in a hierarchy or pedigree. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'phylonode_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C15320',
      'name' => 'Study Design',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A plan detailing how a study will be performed in order to represent the phenomenon under examination, to answer the research questions that have been asked, and defining the methods of data analysis. Study design is driven by research hypothesis being posed, study subject/population/sample available, logistics/resources: technology, support, networking, collaborative support, etc. [ NCI ]',
    ]));
    //chado_associate_semweb_term(NULL, 'studydesign_id', $term);

    // The Company term is missing for the Tripal Contact ontology, but is
    // useful for the arraydesign.manufacturer which is an FK to Contact.
    // It seems better to use a term from a curated ontology than to add to
    // Tripal Contact.
    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C54131',
      'name' => 'Company',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'Any formal business entity for profit, which may be a corporation, a partnership, association or individual proprietorship.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C47885',
      'name' => 'Project',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'Any specifically defined piece of work that is undertaken or attempted to meet a single requirement.',
    ]));
    //chado_associate_semweb_term(NULL, 'project_id', $term);

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C16223',
      'name' => 'DNA Library',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A collection of DNA molecules that have been cloned in vectors.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C85496',
      'name' => 'Trait',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'Any genetically determined characteristic.',
    ]));

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'C25693',
      'name' => 'Subgroup',
      'idSpace' => 'NCIT',
      'vocabulary' => 'ncit',
      'definition' => 'A subdivision of a larger group with members often exhibiting similar characteristics. [ NCI ]',
    ]));

  }

  /**
   * Adds the NCBI Taxon vocabulary database and terms.
   */
  private function addOntologyNCBITaxon() {
    $vocab = $this->getVocabulary('ncbitaxon');
    $vocab->setLabel('NCBI organismal classification. An ontology representation of the NCBI organismal taxonomy');
    $idspace = $this->getIdSpace('NCBITaxon');
    $idspace->setDescription('NCBI organismal classification');
    $idspace->setUrlPrefix('https://www.ncbi.nlm.nih.gov/Taxonomy/Browser/wwwtax.cgi?id={accession}');
    $idspace->setDefaultVocabulary('ncbitaxon');
    $vocab->addIdSpace('NCBITaxon');
    $vocab->setUrl('http://www.berkeleybop.org/ontologies/ncbitaxon/');

    $idspace->saveTerm(new TripalTerm([
      'accession' => 'common_name',
      'name' => 'common name',
      'idSpace' => 'NCBITaxon',
      'vocabulary' => 'ncbitaxon',
      'description' => '',
    ]));
    //chado_associate_semweb_term('organism', 'common_name', $term);
  }

  /**
   * Loads default vocabularies and term.
   *
   * These are only what is necessary for creation of default Tripal content
   * types.
   */
  public function addOntologies() {

    $this->addOntologyCO010();
    $this->addOntologyDC();
    $this->addOntologyEDAM();
    $this->addOntologyEFO();
    $this->addOntologyERO();
    $this->addOntologyFOAF();
    $this->addOntologyGO();
    $this->addOntologyHydra();
    $this->addOntologyIAO();
    $this->addOntologyLocal();
    $this->addOntologyNCBITaxon();
    $this->addOntologyNCIT();
    $this->addOntologyNull();
    $this->addOntologyOBCS();
    $this->addOntologyOBI();
    $this->addOntologyOGI();
    $this->addOntologyPMID();
    $this->addOntologyRDF();
    $this->addOntologyRDFS();
    $this->addOntologyRO();
    $this->addOntologySBO();
    $this->addOntologySchema();
    $this->addOntologySEP();
    $this->addOntologySIO();
    $this->addOntologySO();
    $this->addOntologyTaxRank();
    $this->addOntologyTContact();
    $this->addOntologyTPub();
    $this->addOntologyUO();
  }

  /**
   * Imports ontologies into Chado.
   */
  protected function importOntologies() {
    $ontologies = [];
    $ontologies[] = [
      'vocabulary' => $this->getVocabulary('ro'),
      'idSpace' => $this->getIdSpace('RO'),
      'path' => '{tripal_chado}/files/legacy_ro.obo',
      'auto_load' => FALSE,
    ];
    $ontologies[] = [
      'vocabulary' => $this->getVocabulary('cellular_component'),
      'idSpace' => $this->getIdSpace('GO'),
      'path' => 'http://purl.obolibrary.org/obo/go.obo',
      'auto_load' => FALSE,
    ];
    $ontologies[] = [
      'vocabulary' => $this->getVocabulary('sequence'),
      'idSpace' => $this->getIdSpace('SO'),
      'path' => 'http://purl.obolibrary.org/obo/so.obo',
      'auto_load' => TRUE,
    ];
    $ontologies[] = [
      'vocabulary' => $this->getVocabulary('taxonomic_rank'),
      'idSpace' => $this->getIdSpace('TAXRANK'),
      'path' => 'http://purl.obolibrary.org/obo/taxrank.obo',
      'auto_load' => TRUE,
    ];
    $ontologies[] = [
      'vocabulary' => $this->getVocabulary('tripal_contact'),
      'idSpace' => $this->getIdSpace('TCONTACT'),
      'path' => '{tripal_chado}/files/tcontact.obo',
      'auto_load' => TRUE,
    ];
    $ontologies[] = [
      'vocabulary' => $this->getVocabulary('tripal_pub'),
      'idSpace' => $this->getIdSpace('TPUB'),
      'path' => '{tripal_chado}/files/tpub.obo',
      'auto_load' => TRUE,
    ];

    // Iterate through each ontology and install them with the OBO Importer.
    foreach ($ontologies as $ontology) {
      $obo_id = $this->insertOntologyRecord($ontology);
      $schema_name = $this->outputSchemas[0]->getSchemaName();
      if ($ontology['auto_load']) {
        $this->logger->notice("Importing " . $ontology['idSpace']->getDescription());
        $importer_manager = \Drupal::service('tripal.importer');
        $obo_importer = $importer_manager->createInstance('chado_obo_loader');
        $obo_importer->create(['obo_id' => $obo_id, 'schema_name' => $schema_name]);
        $obo_importer->run();
        $obo_importer->postRun();
      }
    }
  }

  /**
   * A helper function for inserting OBO recrods into the `tripal_cv_obo` table.
   *
   * @param array $obo
   *   An associative array with elements needed for each record.
   * @return int
   *   The Id of the inserted OBO record.
   */
  private function insertOntologyRecord($ontology) {

    $name = $ontology['idSpace']->getDescription();

    // Make sure an OBO with the same name doesn't already exist.
    $obo_id = $this->public->select('tripal_cv_obo', 'tco')
      ->fields('tco', ['obo_id'])
      ->condition('name', $name)
      ->execute()
      ->fetchField();

    if ($obo_id) {
      $this->public->update('tripal_cv_obo')
        ->fields([
          'path' => $ontology['path'],
        ])
        ->condition('name', $name)
        ->execute();
    }
    else {
      $this->public->insert('tripal_cv_obo')
        ->fields([
          'name' => $name,
          'path' => $ontology['path'],
        ])
        ->execute();
    }

    return $this->public->select('tripal_cv_obo', 'tco')
      ->fields('tco', ['obo_id'])
      ->condition('name', $name)
      ->execute()
      ->fetchField();
  }

  /**
   * Create a new Content Type.
   *
   */
  private function createContentType($details) {

    // Get the next bio_data_x index number.
    $cid = 'chado_bio_data_index';
    $next_index = \Drupal::cache()->get($cid, 0)->data + 1;
    $details['id'] = $next_index;
    $details['name'] = 'bio_data_' . $next_index;

    $term = $details['term'];
    if (!array_key_exists('term', $details) or !$details['term']) {
      $this->logger->error(t('Creation of content type, "@type", failed. No term provided.',
          ['@type' => $details['label']]));
      return;
    }
    if (!$term->isValid()) {
      $this->logger->error(t('Creation of content type, "@type", failed. The provided term, "@term", was not valid.',
          ['@type' => $details['label'], '@term' => $term->getTermId()]));
      return;
    }

    // Check if the type already exists.
    $entityType = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['label' => $details['label']]);
    if ($entityType) {
      $this->logger->notice(t('Skipping content type, "@type", as it already exists.',
          ['@type' => $details['label']]));
      return;
    }

    $entityType = TripalEntityType::create($details);
    if (is_object($entityType)) {
      $entityType->save();
      $this->logger->notice(t('Content type, "@type", created..',
          ['@type' => $details['label']]));
      \Drupal::cache()->set($cid, $next_index);
    }
    else {
      $this->logger->error(t('Creation of content type, "@type", failed. The provided provided were: ',
          ['@type' => $details['label']]) . print_r($details));
    }
  }

  /**
   * Creates the "General" category of content types.
   */
  private function createGeneralContentTypes() {

    $this->createContentType([
      'label' => 'Organism',
      'term' => $this->getTerm('OBI', '0100026'),
      'category' => 'General',
    ]);

    $this->createContentType([
      'label' => 'Analysis',
      'term' => $this->getTerm('operation', '2945', 'EDAM'),
      'category' => 'General',
    ]);

    $this->createContentType([
      'label' => 'Project',
      'term' => $this->getTerm('NCIT', 'C47885'),
      'category' => 'General',
    ]);

    $this->createContentType([
      'label' => 'Study',
      'term' => $this->getTerm('SIO', '001066'),
      'category' => 'General',
    ]);

    $this->createContentType([
      'label' => 'Contact',
      'term' => $this->getTerm('local', 'contact'),
      'category' => 'General',
    ]);

    $this->createContentType([
      'label' => 'Publication',
      'term' => $this->getTerm('TPUB', '0000002'),
      'category' => 'General',
    ]);

    $this->createContentType([
      'label' => 'Protocol',
      'term' => $this->getTerm('sep', '00101'),
      'category' => 'General',
    ]);
  }

  /**
   * Creates the "Genomic" category of content types.
   */
  private function createGenomicContentTypes() {

    $this->createContentType([
      'label' => 'Gene',
      'term' => $this->getTerm('SO', '0000704'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'mRNA',
      'term' => $this->getTerm('SO', '0000234'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'Phylogenetic Tree',
      'term' => $this->getTerm('data', '0872', 'EDAM'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'Physical Map',
      'term' => $this->getTerm('data', '1280', 'EDAM'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'DNA Library',
      'term' => $this->getTerm('NCIT', 'C16223'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'Genome Assembly',
      'term' => $this->getTerm('operation', '0525', 'EDAM'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'Genome Annotation',
      'term' => $this->getTerm('operation', '0362', 'EDAM'),
      'category' => 'Genomic',
    ]);

    $this->createContentType([
      'label' => 'Genome Project',
      'term' => $this->getTerm('local', 'Genome Project'),
      'category' => 'Genomic',
    ]);
  }

  /**
   * Creates the "Genetic" category of content types.
   */
  private function createGeneticContentTypes() {

    $this->createContentType([
      'label' => 'Genetic Map',
      'term' => $this->getTerm('data', '1278', 'EDAM'),
      'category' => 'Genetic',
    ]);

    $this->createContentType([
      'label' => 'QTL',
      'term' => $this->getTerm('SO', '0000771'),
      'category' => 'Genetic',
    ]);

    $this->createContentType([
      'label' => 'Sequence Variant',
      'term' => $this->getTerm('SO', '0001060'),
      'category' => 'Genetic',
    ]);

    $this->createContentType([
      'label' => 'Genetic Marker',
      'term' => $this->getTerm('SO', '0001645'),
      'category' => 'Genetic',
    ]);

    $this->createContentType([
      'label' => 'Heritable Phenotypic Marker',
      'term' => $this->getTerm('SO', '0001500'),
      'category' => 'Genetic',
    ]);
  }

  /**
   * Creates the "Germplasm" category of content types.
   */
  private function createGermplasmContentTypes() {

    $this->createContentType([
      'label' => 'Phenotypic Trait',
      'term' => $this->getTerm('NCIT', 'C85496'),
      'category' => 'Germplasm',
    ]);

    $this->createContentType([
      'label' => 'Germplasm Accession',
      'term' => $this->getTerm('CO_010', '0000044'),
      'category' => 'Germplasm',
    ]);

    $this->createContentType([
      'label' => 'Breeding Cross',
      'term' => $this->getTerm('CO_010', '0000255'),
      'category' => 'Germplasm',
    ]);

    $this->createContentType([
      'label' => 'Germplasm Variety',
      'term' => $this->getTerm('CO_010', '0000029'),
      'category' => 'Germplasm',
    ]);

    $this->createContentType([
      'label' => 'Recombinant Inbred Line',
      'term' => $this->getTerm('CO_010', '0000162'),
      'category' => 'Germplasm',
    ]);
  }

  /**
   * Creates the "Expression" category of content types.
   */
  private function createExpressionContentTypes() {

    $this->createContentType([
      'label' => 'Biological Sample',
      'term' => $this->getTerm('sep', '00195'),
      'category' => 'Expression',
    ]);

    $this->createContentType([
      'label' => 'Assay',
      'term' => $this->getTerm('OBI', '0000070'),
      'category' => 'Expression',
    ]);

    $this->createContentType([
      'label' => 'Array Design',
      'term' => $this->getTerm('EFO', '0000269'),
      'category' => 'Expression',
    ]);
  }
}
