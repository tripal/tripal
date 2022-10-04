<?php

namespace Drupal\tripal_chado\Task;

use Drupal\Core\Config\Entity\ConfigEntityStorage;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
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
      $chado_version = $this->chado->getVersion();
      if ($chado_version != '1.3') {
        throw new TaskException("Cannot prepare. Currently only Chado v1.3 is supported.");
      }

      $this->setProgress(0.1);
      $this->logger->notice("Creating Tripal Custom Tables...");
      $this->createCustomTables();

      $this->setProgress(0.15);
      $this->logger->notice("Creating Tripal Materialized Views...");
      $this->createMviews();

      $this->setProgress(0.2);
      $this->logger->notice("Loading ontologies...");
      $terms_setup = \Drupal::service('tripal_chado.terms_init');
      $terms_setup->installTerms();
      // $this->importOntologies(); @todo uncomment before PR

      $this->setProgress(0.3);
      $this->logger->notice('Populating materialized view cv_root_mview...');
      $this->populateMview_cv_root_mview();

      $this->setProgress(0.4);
      $this->logger->notice('Populating materialized view db2cv_mview...');
      $this->populateMview_db2cv_mview();

      $this->setProgress(0.5);
      $this->logger->notice("Making semantic connections for Chado tables/fields...");
      // $this->populate_chado_semweb_table(); // WE NEED TO DO THIS

      $this->setProgress(0.6);
      $this->logger->notice("Map Chado Controlled vocabularies to Tripal Terms...");
      // TODO //  NEXT UP ON THE LIST TO DETERMINE IF WE NEED THIS

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
   * Create the custom tables this module needs.
   *
   * These are tables that Chado uses to manage the site (i.e. temporary
   * loading tables) and not for primary data storage.
   */
  protected function createCustomTables() {
    $this->createCustomTable_tripal_gff_temp();
    $this->createCustomTable_tripal_gffcds_temp();
    $this->createCustomTable_tripal_gffprotein_temp();
    $this->createCustomTable_tripal_obo_temp();
  }

  /**
   * Creates the tripal_gff_temp table.
   *
   * This table is used by the GFF Importer.
   */
  protected function createCustomTable_tripal_gff_temp() {
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

    $custom_tables = \Drupal::service('tripal_chado.custom_tables');
    $custom_table = $custom_tables->create('tripal_gff_temp', $this->chado->getSchemaName());
    $custom_table->setTableSchema($schema);
    $custom_table->setHidden(True);

    //chado_create_custom_table('tripal_gff_temp', $schema, TRUE, NULL,
    //  FALSE, $this->chado);
  }

  /**
   * Creates the tripal_gffcds_temp table.
   *
   * This table is used by the GFF Importer.
   */
  function createCustomTable_tripal_gffcds_temp() {
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

    $custom_tables = \Drupal::service('tripal_chado.custom_tables');
    $custom_table = $custom_tables->create('tripal_gffcds_temp', $this->chado->getSchemaName());
    $custom_table->setTableSchema($schema);
    $custom_table->setHidden(True);

    //chado_create_custom_table('tripal_gffcds_temp', $schema, TRUE, NULL,
    //  FALSE, $this->chado);
  }

  /**
   * Create the tripal_gffproptein_temp table.
   *
   * This table is used by the GFF Importer.
   */
  function createCustomTable_tripal_gffprotein_temp() {
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

    $custom_tables = \Drupal::service('tripal_chado.custom_tables');
    $custom_table = $custom_tables->create('tripal_gffprotein_temp', $this->chado->getSchemaName());
    $custom_table->setTableSchema($schema);
    $custom_table->setHidden(True);

    //chado_create_custom_table('tripal_gffprotein_temp', $schema, TRUE, NULL,
    //  FALSE, $this->chado);
  }

  /**
   * Create the tripal_obo_temp table.
   *
   * This table is used by the OBO Importer.
   */
  function createCustomTable_tripal_obo_temp() {
    $schema = [
      'table' => 'tripal_obo_temp',
      'fields' => [
        'id' => [
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ],
        'stanza' => [
          'type' => 'text',
          'not null' => TRUE,
        ],
        'type' => [
          'type' => 'varchar',
          'length' => 50,
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'tripal_obo_temp_idx0' => ['id'],
        'tripal_obo_temp_idx1' => ['type'],
      ],
      'unique keys' => [
        'tripal_obo_temp0' => ['id'],
      ],
    ];

    $custom_tables = \Drupal::service('tripal_chado.custom_tables');
    $custom_table = $custom_tables->create('tripal_obo_temp', $this->chado->getSchemaName());
    $custom_table->setTableSchema($schema);
    $custom_table->setHidden(True);
  }

  /**
   * Creates the materialized views used by this module.
   */
  protected function createMviews() {
    $this->createMview_organism_stock_count();
    $this->createMview_library_feature_count();
    $this->createMview_organism_feature_count();
    $this->createMview_analysis_organism();
    $this->createMview_cv_root_mview();
    $this->createMview_db2cv_mview();
  }


  /**
   * Creates a materialized view that stores the type & number of stocks per
   * organism
   *
   * @ingroup tripal_stock
   */
  private function createMview_organism_stock_count() {
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
          INNER JOIN stock S ON O.Organism_id = S.organism_id
          INNER JOIN cvterm CVT ON S.type_id = CVT.cvterm_id
       GROUP BY
          O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
    ";

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create($view_name, $this->chado->getSchemaName());
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
  }

  /**
   * Adds a materialized view keeping track of the type of features associated
   * with each library
   *
   * @ingroup tripal_library
   */
  private function createMview_library_feature_count() {
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

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create($view_name, $this->chado->getSchemaName());
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
  }

  /**
   * Creates a materialized view that stores the type & number of features per
   * organism
   *
   * @ingroup tripal_feature
   */
  private function createMview_organism_feature_count() {
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

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create($view_name, $this->chado->getSchemaName());
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
  }


  /**
   * Creates a view showing the link between an organism & it's analysis through
   * associated features.
   *
   */
  private function createMview_analysis_organism() {
    $view_name = 'analysis_organism';
    $comment = 'This view is for associating an organism (via it\'s associated features) to an analysis.';

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

    // this is the SQL used to identify the organism to which an analsysis
    // has been used.  This is obtained though the analysisfeature -> feature -> organism
    // joins
    $sql = "
      SELECT DISTINCT A.analysis_id, O.organism_id
      FROM analysis A
        INNER JOIN analysisfeature AF ON A.analysis_id = AF.analysis_id
        INNER JOIN feature F ON AF.feature_id = F.feature_id
        INNER JOIN organism O ON O.organism_id = F.organism_id
    ";

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create($view_name, $this->chado->getSchemaName());
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
  }

  /**
   * Add a materialized view that maps cv to db records.
   *
   * This is needed for viewing cv trees
   *
   */
  private function createMview_db2cv_mview() {
    $view_name = 'db2cv_mview';
    $comment = 'A table for quick lookup of the vocabularies and the databases they are associated with.';
    $schema = [
      'table' => $view_name,
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

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create($view_name, $this->chado->getSchemaName());
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
    $mview->setHidden(True);
  }

  /**
   * Add a materialized view of root terms for all chado cvs.
   *
   * This is needed for viewing cv trees
   *
   */
  private function createMview_cv_root_mview() {
    $view_name = 'cv_root_mview';
    $comment = 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees';
    $schema = [
      'table' => $view_name,
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

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create($view_name, $this->chado->getSchemaName());
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
    $mview->setHidden(True);
  }

  /**
   * Populates the cv_root_mview materialized view.
   */
  private function populateMview_cv_root_mview() {
    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->loadByName('cv_root_mview', $this->chado->getSchemaName());
    $mview->populate();
  }

  /**
   * Populates the db2cv materialized view.
   */
  private function populateMview_db2cv_mview() {
    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->loadByName('db2cv_mview', $this->chado->getSchemaName());
    $mview->populate();
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
   * @param array $details
   *   Describes the content type you would like to create.
   *   Should contain the following:
   *    - label: the human-readable label to be used for the content type.
   *    - category: a human-readable category to group like content types together.
   *    - term: a tripal term object which should be associated with the content type.
   */
  private function createContentType($details) {

    $entityType = NULL;
    $bundle = '';

    $term = $details['term'];
    if (!array_key_exists('term', $details) or !$details['term']) {
      $this->logger->error(t('Creation of content type, "@type", failed. No term provided.',
          ['@type' => $details['label']]));
      return NULL;
    }
    if (!$term->isValid()) {
      $this->logger->error(t('Creation of content type, "@type", failed. The provided term, "@term", was not valid.',
          ['@type' => $details['label'], '@term' => $term->getTermId()]));
      return NULL;
    }

    // Check if the type already exists.
    $entityTypes = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['label' => $details['label']]);
    if (!empty($entityTypes)) {
      $this->logger->notice(t('Skipping content type, "@type", as it already exists.',
          ['@type' => $details['label']]));
      $bundle = array_pop(array_keys($entityTypes));
      $entityType = $entityTypes[$bundle];
    }
    else {
      // Get the next bio_data_x index number.
      $cid = 'chado_bio_data_index';
      $cached_val = \Drupal::cache()->get($cid, 0);
      if ($cached_val != 0) {
        $cached_val = $cached_val->data;
      }
      $next_index = $cached_val + 1;
      $bundle = 'bio_data_' . $next_index;
      $details['id'] = $next_index;
      $details['name'] = $bundle;

      $entityType = TripalEntityType::create($details);
      if (is_object($entityType)) {
        $entityType->save();
        $this->logger->notice(t('Content type, "@type", created..',
            ['@type' => $details['label']]));
        \Drupal::cache()->set($cid, $next_index);
      }
      else {
        $this->logger->error(t('Creation of content type, "@type", failed. The provided details were: ',
            ['@type' => $details['label']]) . print_r($details));
      }
    }

    // Create the default view mode for this new content type.
    $storage = \Drupal::entityTypeManager()->getStorage('entity_view_display');
    $view_display = $storage->load('tripal_entity.' . $bundle . '.default');
    if (!$view_display) {
      $view_details = [
        'langcode' => 'en',
        'status' => True,
        'dependencies' => [
          'module' => ['tripal']
        ],
        'targetEntityType' => 'tripal_entity',
        'bundle' => $bundle,
        'mode' => 'default',
        'content' => [],
        'hidden' => [],
      ];
      $view_display = $storage->create($view_details, 'entity_view_display');
      if (!$view_display->save()) {
        $this->logger->error(t('Creation of content type, "@type", default view mode failed. The provided details were: ',
            ['@type' => $details['label']]) . print_r($details));
      }
    }

    // Create the default form mode for this new content type.
    $storage = \Drupal::entityTypeManager()->getStorage('entity_form_display');
    $form_display = $storage->load('tripal_entity.' . $bundle . '.default');
    if (!$form_display) {
      $form_details = [
        'langcode' => 'en',
        'status' => True,
        'dependencies' => [
          'module' => ['tripal']
        ],
        'targetEntityType' => 'tripal_entity',
        'bundle' => $bundle,
        'mode' => 'default',
        'content' => [],
        'hidden' => [],
      ];
      $form_display = $storage->create($form_details, 'entity_view_display');
      if (!$form_display->save()) {
        $this->logger->error(t('Creation of content type, "@type", default form mode failed. The provided details were: ',
            ['@type' => $details['label']]) . print_r($details));
      }
    }
    return $entityType;
  }

  /**
   * Automatically adds single-valued fields for base tables.
   *
   * @param TripalEntityType $entityType
   * @param string $chado_table
   */
  private function addBaseTableSVFields(TripalEntityType $entityType, string $chado_table) {

    // We need the idSpace manager object.
    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    // Get the Chado table information.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);
    $pk = $schema_def['primary key'];
    if (is_array($pk)) {
      $pk = $pk[0];
    }
    $columns = $schema_def['fields'];

    // Get the term to table mapping information for the core chado mapping.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $weight = 10;
    foreach ($columns AS $column => $detail) {
      // Do not add afield for the primary key
      if ($column == $pk) {
        continue;
      }

      $term_id = $mapping->getColumnTermId($chado_table, $column);
      list($idSpace_name, $accession) = explode(':', $term_id);
      $idSpace = $idSpace_manager->loadCollection($idSpace_name);
      if ($idSpace) {
        $term = $idSpace->getTerm($accession);

        $field_name = strtolower($entityType->getName() . '_' . $idSpace_name . '_' . preg_replace('/[^\w]/', '_', $accession));
        $field_name = substr($field_name, 0, 32);

        $field_type = '';
        $storage_settings = [];
        if (strtolower($detail['type']) == 'character varying') {
          $field_type = 'tripal_string_type';
          $storage_settings['max_length'] = $detail['size'];
        }
        if (strtolower($detail['type']) == 'text') {
          $field_type = 'tripal_text_type';
        }
        if (strtolower($detail['type']) == 'bigint' or strtolower($detail['type']) == 'int') {
          // Make sure it's not a foreign key. If so, this will most likely be a complex field.
          $is_fk = FALSE;
          foreach ($schema_def['foreign keys'] as $fktable => $fkdetails) {
            if (array_key_exists($column, $fkdetails['columns'])) {
              $is_fk = TRUE;
            }
          }
          if (!$is_fk) {
            $field_type = 'tripal_integer_type';
          }
        }
        // @todo handle all the different database column types.

        // Is the field required? Ensure we match the database.
        $is_required = FALSE;
        if (isset($detail['not null']) && $detail['not null'] == 1) {
          $is_required = TRUE;
        }

        // Can the database support multiple cardinality?
        // @todo currently we're not automatically determining this.
        $cardinality = 1;

        // If we don't have a suported field type then just skip this
        // columns
        if (!$field_type) {
          continue;
        }

        $field = [
          'name' => $field_name,
          'label' => ucwords($term->getName()),
          'type' => $field_type,
          'description' => $term->getDefinition(),
          'cardinality' => $cardinality,
          'required' => $is_required,
          'storage_settings' => [
            'storage_plugin_id' => 'chado_storage',
            'storage_plugin_settings' => [
              'base_table' => $chado_table,
              'property_settings' => [
                'value' => [
                  'action' => 'store',
                  'chado_table' => $chado_table,
                  'chado_column' => $column,
                ]
              ],
            ],
          ],
          'settings' => [
            'termIdSpace' => $idSpace_name,
            'termAccession' => $term->getAccession(),
          ],
          'display' => [
            'view' => [
              'default' => [
                'region' => 'content',
                'label' => 'above',
                'weight' => $weight,
              ],
            ],
            'form' => [
              'default' => [
                'region' => 'content',
                'weight' => $weight,
              ],
            ],
          ],
        ];

        // Now add in any additional storage settings
        foreach ($storage_settings as $key => $value) {
          $field['storage_settings'][$key] = $value;
        }

        /**
         * @var \Drupal\tripal\Services\TripalFieldsManager $tripal_fields
         */
        $tripal_fields = \Drupal::service('tripal.fields');
        $tripal_fields->addBundleField($entityType->getName(), $field);
        $weight = $weight + 5;
      }
    }
  }

  /**
   * Automatically adds the organism complex value fields for base tables.
   *
   * @param TripalEntityType $entityType
   * @param string $chado_table
   */
  private function addOrganismField(TripalEntityType $entityType, string $chado_table) {

    // We need the idSpace manager object.
    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    // Get the Chado table information.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);

    // Get the term to table mapping information for the core chado mapping.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $weight = 10;
    // This is only for organism foreign keys so no need to add if it's the organism table.
    if ($chado_table == 'organism') {
      return;
    }

    // Now check for the foreign key.
    if (array_key_exists('organism_id', $schema_def['fields'])) {
      $org_id_col = 'organism_id';
    } elseif (array_key_exists('taxon_id', $schema_def['fields'])){
      $org_id_col = 'taxon_id';
    }
    else {
      // there is no foreign key to the organism table for this chado table.
      return;
    }

    // Now retrieve the term mapped to this chado column.
    $term_id = $mapping->getColumnTermId($chado_table, $org_id_col);
    list($idSpace_name, $accession) = explode(':', $term_id);
    $idSpace = $idSpace_manager->loadCollection($idSpace_name);
    if (!is_object($idSpace)) {
      // Unable to find the term we need.
      // No error was originally returned here but we may want to consider adding one.
      return;
    }

    $term = $idSpace->getTerm($accession);

    // Use the same method as Tripal v3 for creating field names.
    $field_name = strtolower($entityType->getName() . '_' . $idSpace_name . '_' . preg_replace('/[^\w]/', '_', $accession));
    $field_name = substr($field_name, 0, 32);
    $field_type = 'obi__organism';

    // Is the field required? Ensure we match the database.
    $is_required = FALSE;
    if (array_key_exists('not null', $schema_def['fields'][$org_id_col]) and
        $schema_def['fields'][$org_id_col]['not null'] == TRUE) {
      $is_required = TRUE;
    }

    $field = [
      'name' => $field_name,
      'label' => ucwords($term->getName()),
      'type' => $field_type,
      'description' => $term->getDefinition(),
      'cardinality' => 1,
      'required' => $is_required,
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings' => [
          'base_table' => $chado_table,
          'property_settings' => [
            'value' => [
              'action' => 'store',
              'chado_table' => $chado_table,
              'chado_column' => $org_id_col,
            ],
          ],
        ],
      ],
      'settings' => [
       'termIdSpace' => $idSpace_name,
       'termAccession' => $term->getAccession(),
      ],
      'display' => [
       'view' => [
         'default' => [
           'region' => 'content',
           'label' => 'above',
           'weight' => $weight,
         ],
       ],
       'form' => [
         'default' => [
           'region' => 'content',
           'weight' => $weight,
         ],
       ],
      ],
    ];
    $tripal_fields = \Drupal::service('tripal.fields');
    $tripal_fields->addBundleField($entityType->getName(), $field);
  }

  /**
   * Automatically adds complex value fields for base tables.
   *
   * @param TripalEntityType $entityType
   * @param string $chado_table
   */
  private function addTypeField(TripalEntityType $entityType, string $chado_table) {

    // We need the idSpace manager object.
    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    // Get the Chado table information.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);

    // Get the term to table mapping information for the core chado mapping.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $weight = 10;
    // We won't add a type field if there isn't a type_id.
    if (!array_key_exists('type_id', $schema_def['fields'])) {
      return;
    }

    $term_id = $mapping->getColumnTermId($chado_table, 'type_id');
    list($idSpace_name, $accession) = explode(':', $term_id);
    $idSpace = $idSpace_manager->loadCollection($idSpace_name);
    if (!is_object($idSpace)) {
      // Unable to find the term we need.
      // No error was originally returned here but we may want to consider adding one.
      return;
    }

    $term = $idSpace->getTerm($accession);

    // Use the same method as Tripal v3 for creating field names.
    $field_name = strtolower($idSpace_name . '__' . preg_replace('/[^\w]/', '_', $term->getName()));
    $field_name = substr($field_name, 0, 32);
    $field_type = 'tripal_integer_type';

    // Is the field required? Ensure we match the database.
    $is_required = FALSE;
    if (array_key_exists('not null', $schema_def['fields']['type_id']) and
        $schema_def['fields']['type_id']['not null'] == TRUE) {
      $is_required = TRUE;
    }

    $field = [
      'name' => $field_name,
      'label' => ucwords($term->getName()),
      'type' => $field_type,
      'description' => $term->getDefinition(),
      'cardinality' => 1,
      'required' => $is_required,
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings' => [
          'base_table' => $chado_table,
          'property_settings' => [
            'value' => [
              'action' => 'store',
              'chado_table' => $chado_table,
              'chado_column' => 'type_id',
            ],
          ],
        ],
      ],
      'settings' => [
        'termIdSpace' => $idSpace_name,
        'termAccession' => $term->getAccession(),
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => $weight,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => $weight,
          ],
        ],
      ],
    ];
    $tripal_fields = \Drupal::service('tripal.fields');
    $tripal_fields->addBundleField($entityType->getName(), $field);
  }


  /**
   * Creates the "General" category of content types.
   */
  private function createGeneralContentTypes() {

    $entity_type = $this->createContentType([
      'label' => 'Organism',
      'term' => $this->getTerm('OBI', '0100026'),
      'category' => 'General',
    ]);
    $this->addBaseTableSVFields($entity_type, 'organism');

    $entity_type = $this->createContentType([
      'label' => 'Analysis',
      'term' => $this->getTerm('operation', '2945', 'EDAM'),
      'category' => 'General',
    ]);
    $this->addBaseTableSVFields($entity_type, 'analysis');

    $entity_type = $this->createContentType([
      'label' => 'Project',
      'term' => $this->getTerm('NCIT', 'C47885'),
      'category' => 'General',
    ]);
    $this->addBaseTableSVFields($entity_type, 'project');

    $entity_type = $this->createContentType([
      'label' => 'Study',
      'term' => $this->getTerm('SIO', '001066'),
      'category' => 'General',
    ]);
    $this->addBaseTableSVFields($entity_type, 'study');

    $entity_type = $this->createContentType([
      'label' => 'Contact',
      'term' => $this->getTerm('local', 'contact'),
      'category' => 'General',
    ]);
    $this->addBaseTableSVFields($entity_type, 'contact');

    $entity_type = $this->createContentType([
      'label' => 'Publication',
      'term' => $this->getTerm('TPUB', '0000002'),
      'category' => 'General',
    ]);
    // @todo we need to handle the pub table specially.
    //$this->addBaseTableSVFields($entity_type, 'pub');

    $entity_type = $this->createContentType([
      'label' => 'Protocol',
      'term' => $this->getTerm('sep', '00101'),
      'category' => 'General',
    ]);
    $this->addBaseTableSVFields($entity_type, 'protocol');
  }

  /**
   * Creates the "Genomic" category of content types.
   */
  private function createGenomicContentTypes() {

    $entity_type = $this->createContentType([
      'label' => 'Gene',
      'term' => $this->getTerm('SO', '0000704'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'feature');
    $this->addOrganismField($entity_type, 'feature');
    $this->addTypeField($entity_type, 'feature');

    $entity_type = $this->createContentType([
      'label' => 'mRNA',
      'term' => $this->getTerm('SO', '0000234'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'feature');

    $entity_type = $this->createContentType([
      'label' => 'Phylogenetic Tree',
      'term' => $this->getTerm('data', '0872', 'EDAM'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'phylotree');

    $entity_type = $this->createContentType([
      'label' => 'Physical Map',
      'term' => $this->getTerm('data', '1280', 'EDAM'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'featuremap');

    $entity_type = $this->createContentType([
      'label' => 'DNA Library',
      'term' => $this->getTerm('NCIT', 'C16223'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'library');

    $entity_type = $this->createContentType([
      'label' => 'Genome Assembly',
      'term' => $this->getTerm('operation', '0525', 'EDAM'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'analysis');

    $entity_type = $this->createContentType([
      'label' => 'Genome Annotation',
      'term' => $this->getTerm('operation', '0362', 'EDAM'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'analysis');

    $entity_type = $this->createContentType([
      'label' => 'Genome Project',
      'term' => $this->getTerm('local', 'Genome Project'),
      'category' => 'Genomic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'project');
  }

  /**
   * Creates the "Genetic" category of content types.
   */
  private function createGeneticContentTypes() {

    $entity_type = $this->createContentType([
      'label' => 'Genetic Map',
      'term' => $this->getTerm('data', '1278', 'EDAM'),
      'category' => 'Genetic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'featuremap');

    $entity_type = $this->createContentType([
      'label' => 'QTL',
      'term' => $this->getTerm('SO', '0000771'),
      'category' => 'Genetic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'feature');

    $entity_type = $this->createContentType([
      'label' => 'Sequence Variant',
      'term' => $this->getTerm('SO', '0001060'),
      'category' => 'Genetic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'feature');

    $entity_type = $this->createContentType([
      'label' => 'Genetic Marker',
      'term' => $this->getTerm('SO', '0001645'),
      'category' => 'Genetic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'feature');

    $entity_type = $this->createContentType([
      'label' => 'Heritable Phenotypic Marker',
      'term' => $this->getTerm('SO', '0001500'),
      'category' => 'Genetic',
    ]);
    $this->addBaseTableSVFields($entity_type, 'feature');
  }

  /**
   * Creates the "Germplasm" category of content types.
   */
  private function createGermplasmContentTypes() {

    // @todo this is commented out in Tripal v3 so
    // leaving it as such here too.
//     $entity_type = $this->createContentType([
//       'label' => 'Phenotypic Trait',
//       'term' => $this->getTerm('NCIT', 'C85496'),
//       'category' => 'Germplasm',
//     ]);
//     $this->addBaseTableSVFields($entity_type, 'cvterm');

    $entity_type = $this->createContentType([
      'label' => 'Germplasm Accession',
      'term' => $this->getTerm('CO_010', '0000044'),
      'category' => 'Germplasm',
    ]);
    $this->addBaseTableSVFields($entity_type, 'stock');

    $entity_type = $this->createContentType([
      'label' => 'Breeding Cross',
      'term' => $this->getTerm('CO_010', '0000255'),
      'category' => 'Germplasm',
    ]);
    $this->addBaseTableSVFields($entity_type, 'stock');

    $entity_type = $this->createContentType([
      'label' => 'Germplasm Variety',
      'term' => $this->getTerm('CO_010', '0000029'),
      'category' => 'Germplasm',
    ]);
    $this->addBaseTableSVFields($entity_type, 'stock');

    $entity_type = $this->createContentType([
      'label' => 'Recombinant Inbred Line',
      'term' => $this->getTerm('CO_010', '0000162'),
      'category' => 'Germplasm',
    ]);
    $this->addBaseTableSVFields($entity_type, 'stock');
  }

  /**
   * Creates the "Expression" category of content types.
   */
  private function createExpressionContentTypes() {

    $entity_type = $this->createContentType([
      'label' => 'Biological Sample',
      'term' => $this->getTerm('sep', '00195'),
      'category' => 'Expression',
    ]);
    $this->addBaseTableSVFields($entity_type, 'biomaterial');

    $entity_type = $this->createContentType([
      'label' => 'Assay',
      'term' => $this->getTerm('OBI', '0000070'),
      'category' => 'Expression',
    ]);
    $this->addBaseTableSVFields($entity_type, 'assaty');

    $entity_type = $this->createContentType([
      'label' => 'Array Design',
      'term' => $this->getTerm('EFO', '0000269'),
      'category' => 'Expression',
    ]);
    $this->addBaseTableSVFields($entity_type, 'arraydesign');
  }
}
