<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

/**
 * Chado upgrader.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of an existing Chado schema.
 * $upgrader = \Drupal::service('tripal_chado.upgrader');
 * $upgrader->setParameters([
 *   'output_schemas' => ['chado'],
 * ]);
 * if (!$upgrader->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoUpgrader extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'upgrader';

  /**
   * Default version.
   */
  public const DEFAULT_CHADO_VERSION = '1.3';

  /**
   * Name of the reference schema.
   *
   * This name can be overriden by extending classes.
   */
  public const CHADO_REF_SCHEMA_13 = '_chado_13_template';

  /**
   * Defines a priority order to process some Chado objects to upgrade.
   */
  public const CHADO_OBJECT_PRIORITY_13 = [
    'db',
    'dbxref',
    'cv',
    'cvterm',
    'cvtermpath',
    'pub',
    'synonym',
    'feature',
    'feature_cvterm',
    'feature_dbxref',
    'feature_synonym',
    'featureprop',
    'feature_pub',
    'gffatts',
  ];

  /**
   * Upgrade SQL queries.
   */
  protected $upgradeQueries;

  /**
   * Biological database tool.
   *
   * @var \Drupal\tripal_biodb\Database\BioDbTool
   */
  protected $bioTool;

  /**
   * TripalDbx service object.
   *
   * This provides in-class access to the non-schema specific Tripal DBX API
   * methods.
   *
   * @var object \Drupal\tripal\TripalDBX\TripalDbx
   */
  protected $tripalDbxApi = NULL;

  /**
   * {@inheritdoc}
   */
  public function __construct(
    ?\Drupal\Core\Database\Connection $database = NULL,
    ?\Psr\Log\LoggerInterface $logger = NULL,
    ?\Drupal\tripal_biodb\Lock\SharedLockBackendInterface $locker = NULL,
    ?\Drupal\Core\State\StateInterface $state = NULL
  ) {
    parent::__construct($database, $logger, $locker, $state);
    $this->tripalDbxApi = \Drupal::service('tripal.dbx');
  }

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one output
   * schema and it may include an input schema as reference, a version number,
   * a cleanup flag and an output file name:
   * ```
   * [
   *   'output_schemas' => ['chado'],
   *   'version' => '1.3',
   *   'cleanup' => TRUE,
   *   'filename' => 'upgrade_queries.sql',
   * ]
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      // Select a default version if needed.
      if (empty($this->parameters['version'])) {
        $this->parameters['version'] = static::DEFAULT_CHADO_VERSION;
      }
      // Check the version is valid.
      if ($this->parameters['version'] != static::DEFAULT_CHADO_VERSION) {
        throw new ParameterException(
          "That requested version ("
          . $this->parameters['version']
          . ") is not supported by this upgrader."
        );
      }

      // Check if a reference schema has been specified.
      if (!empty($this->parameters['input_schemas'])) {
        if (1 != count($this->parameters['input_schemas'])) {
          throw new ParameterException(
            "Chado upgrader can take at most one input schemas as reference."
          );
        }
      }
      else {
        // Use default schema name if no name was specified.
        $this->parameters['input_schemas'] = [$this::CHADO_REF_SCHEMA_13];
        // Initialize input schema.
        $this->inputSchemas = $this->prepareSchemas(
          $this->parameters['input_schemas']
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
      $output_schema = $this->outputSchemas[0];

      // Note: schema names have already been validated through BioConnection.
      // Check if the target schema exists.
      if (!$output_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'The schema to upgrade ("'
          . $output_schema->getSchemaName()
          . '") does exists. You can only upgrade existing data. Please check the provided schema name specified as output.'
        );
      }

      // Check file name.
      if (!empty($this->parameters['filename'])) {
        // Check for an absolute path.
        if (0 !== strpos($this->parameters['filename'], DIRECTORY_SEPARATOR)) {
          // Not absolute, use Drupal default path.
          $default_scheme = \Drupal::config('system.file')->get('default_scheme');
          $files_path = \Drupal::service('file_system')->realpath($default_scheme . "://");
          $this->parameters['filename'] = $files_path . '/' . $this->parameters['filename'];
        }
        if (file_exists($this->parameters['filename'])) {
          throw new ParameterException(
            "The file '" . $this->parameters['filename'] . "' already exists. Aborting."
          );
        }
        $fh = fopen($this->parameters['filename'], 'w');
        if (!$fh) {
          throw new ParameterException("Failed to open '" . $this->parameters['filename'] . "' for writting!");
        }
        $this->parameters['fh'] = $fh;
      }
      else {
        $this->parameters['fh'] = FALSE;
      }

      // Cleanup.
      if (!isset($this->parameters['cleanup'])) {
        $this->parameters['cleanup'] = FALSE;
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
   * Upgrade a given chado schema to the specified version.
   *
   * Before using this function, we recommand you backup your database and/or
   * clone your Chado schema first and try to upgrade that clone first. In case
   * of failure during the upgrade process, the upgraded schema may become
   * unusable so you will have to restore a working version. But be careful,
   * even if the upgrade process scceeded, it may have removed some data that
   * did not feet in the reference schema. Therefore, you will have to check the
   * content of the upgraded schema and may have to manully import back removed
   * data (it could be custom tables, columns, functions, whatever that was not
   * present in the official Chado schema version selected).
   *
   * *The upgrade process*
   *
   * First, if no reference input schema is provieded, we create a new Chado
   * template schema (see CHADO_REF_SCHEMA*) to use as a reference for the
   * upgrade process. The structure of the reference schema will be "applied" to
   * the schema to upgrade. In the end, the schema to upgrade will contain the
   * same functions, tables, columns, views, etc. that the reference schema has.
   *
   * After the reference schema is setup (or selected), we process each
   * PostgreSQL object categories and compare the schema to upgrade to the
   * reference one. When changes are required, we store the corresponding SQL
   * queries for each object in the 'upgradeQueries' class member. Cleanup
   * queries are stored in 'upgradeQueries['#cleanup']' in order to remove
   * unnecessary objects.
   *
   * The upgrade process is the following:
   * 1) Prepare table column defaults removal in table definitions (ie. remove
   *    sequences and function dependencies)
   * 2) Prepare functions and aggregate functions removal
   * 3) Prepare views removal
   * 4) Prepare database type upgrade
   * 5) Prepare sequence upgrade
   * 6) Prepare function prototypes (for function inter-dependencies)
   * 7) Prepare table column type upgrade
   *    Columns that match $chado_column_upgrade will be upgraded
   *    using the corresponding queries. Other columns will be updated using
   *    default behavior. Defaults are dropped and will be added later.
   * 8) Prepare sequence association (to table columns)
   * 9) Prepare view upgrade
   * 10) Prepare function upgrade
   * 11) Prepare aggregate function upgrade
   * 12) Prepare table column default upgrade
   * 13) Prepare comment upgrade
   * 14) Prepare data initialization
   * 15) Process upgrade queries
   * 16) Update Tripal integration
   *
   * Note: a couple of PostgreSQL object are not processed as they are not part
   * of Chado schema specifications: collations, domains, triggers, unlogged
   * tables and materialized views (in PostgreSQL sens, Tripal MV could be
   * processed but are removed by default and will need to be recreated).
   *
   * *Parameters*
   *
   * Task parameter array provided to the class constructor includes:
   * - 'output_schemas' array: one output Chado schema that must exist and
   *   contain data (required). This ouput schema is the schema that needs to be
   *   upgraded.
   * - 'input_schemas' array: no input schema or a reference schema name (for
   *   advanced users). See above documentation to understand what the refrence
   *   schema stand for. If the reference schema is not provided, the default
   *   one will be used. If the reference schema does not exist, it will be
   *   created. If it exists, it will be used as is and any provided version
   *   number will be ignored.
   * - 'version' string: a version number (optional, default to
   *   ::DEFAULT_CHADO_VERSION)
   * - 'cleanup' bool: a cleanup flag that tells if existing database objects no
   *   present in the reference schema should be removed (cleanup = TRUE) or not
   *   (cleanup = FALSE).
   *   Warning: if set to TRUE, uncleaned elements may prevent some parts of the
   *   schema to be upgraded and therefore, the upgrade process has more chances
   *   to fail. If you have data that are not part of the official Chado schema
   *   but you want to keep, you may try to set this flag to FALSE to try to
   *   keep them and avoid having to put them back manully in a cleaned upgraded
   *   version.
   *   Default to TRUE (ie. things will be cleaned up).
   * - 'filename' string: when a path to an unexisting file name is provided, NO
   *   upgrade will be performed. Instead, every SQL query part of the upgrade
   *   will be written into that file. This SQL file can be later used to
   *   upgrade the schema manually. However, please note that a reference schema
   *   will be created if needed and the queries have been designed to work
   *   using the provided schema names. Any change on schema names of the schema
   *   to upgrade or the reference schema will lead to issues when the SQL
   *   queries will be run.
   *   The file path can be absolute (starting with a '/') or relative to the
   *   site 'files' directory (or private directory if it the the default).
   *   Default: no file path.
   *
   * Example:
   * ```
   * [
   *   'output_schemas' => ['chado'],
   *   'version' => '1.3',
   *   'cleanup' => TRUE,
   *   'filename' => 'upgrade_queries.sql',
   * ]
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
    // After validation, $this->parameters['fh'] is not empty if there a
    // filename was set or FALSE, $this->parameters['version'] is set to
    // something valid and $this->outputSchemas[0] (to upgrade) and
    // $this->inputSchemas[0] (reference) are both initialized to BioConnection
    // objects.

    // Acquire locks.
    $success = $this->acquireTaskLocks();
    if (!$success) {
      throw new LockException("Unable to acquire all locks for task. See logs for details.");
    }

    try
    {
      $chado_schema = $this->outputSchemas[0];
      $ref_schema = $this->inputSchemas[0];

      // Save task initial data for later progress computation.
      $this->setProgress(0);

      // Note: in most queries, we don't use "{}" for tables as these are often
      // system tables (pg_catalog.pg_* or information_schema tables). Since Drupal
      // uses "{}" to prefix tables in queries, we don't want that for system
      // tables.

      // Make sure the reference schema is available.
      $this->setupReferenceSchema();
      $this->setProgress(0.10);

      // Init query array. We initialize a list of element to have them processed
      // in correct order.
      $this->upgradeQueries = [
        '#start'                => ['START TRANSACTION;'],
        '#cleanup'              => [],
        '#drop_column_defaults' => [],
        '#drop_functions'       => [],
        '#drop_views'           => [],
        '#types'                => [],
        '#sequences'            => [],
        '#priorities'           => [],
        // "#end" will be processed at last even if new elements are added after
        // to upgradeQueries, and its queries will be processed in reverse order.
        '#end'                  => ['COMMIT;'],
      ];

      try {
        // Get Drupal schema name.
        $drupal_schema = $this->tripalDbxApi->getDrupalSchemaName();
        if ($this->parameters['fh']) {
          // Make sure we will work on the given schema when using SQL file.
          $sql_query =
            'SET search_path = '
            . $chado_schema->getQuotedSchemaName()
            . ','
            . $drupal_schema
            . ';'
          ;
          $this->upgradeQueries['#start'][] = $sql_query;
          // And we will go back to Drupal schema in the end.
          $sql_query = "SET search_path = " . $drupal_schema . ";";
          $this->upgradeQueries['#end'][] = $sql_query;
        }

        // Compare schema structures...
        // - Remove column defaults.
        $this->prepareDropColumnDefaults();
        $this->setProgress(0.13);

        // - Remove functions.
        $this->prepareDropFunctions();
        $this->setProgress(0.16);

        // - Drop old views to remove dependencies on tables.
        $this->prepareDropAllViews();
        $this->setProgress(0.19);

        // - Check types.
        $this->prepareUpgradeTypes();
        $this->setProgress(0.22);

        // - Upgrade existing sequences and add missing ones.
        $this->prepareUpgradeSequences();
        $this->setProgress(0.25);

        // - Create prototype functions.
        $this->preparePrototypeFunctions();
        $this->setProgress(0.28);

        // - Tables.
        $this->prepareUpgradeTables();
        $this->setProgress(0.31);

        // - Sequence associations.
        $this->prepareSequenceAssociation();
        $this->setProgress(0.34);

        // - Views.
        $this->prepareUpgradeViews();
        $this->setProgress(0.37);

        // - Upgrade functions (fill function bodies).
        $this->prepareFunctionUpgrade();
        $this->setProgress(0.40);

        // - Upgrade aggregate functions.
        $this->prepareAggregateFunctionUpgrade();
        $this->setProgress(0.43);

        // - Tables defaults.
        $this->prepareUpgradeTableDefauls();
        $this->setProgress(0.46);

        // - Upgrade comments.
        $this->prepareCommentUpgrade();
        $this->setProgress(0.49);

        // - Add missing initialization data.
        $this->reinitSchema();
        $this->setProgress(0.55);

        // - Process upgrades.
        $this->processUpgradeQueries();
        $this->setProgress(0.95);

        if ($this->parameters['fh']) {
          // Do not update if file. Put the query in the SQL file instead.
          $sql_query =
            'UPDATE '
            . $this->connection->prefixTables('{chado_installations}')
            . ' SET version = \''
            . $this->parameters['version']
            . '\', created = \''
            . \Drupal::time()->getRequestTime()
            . '\', updated = \''
            . \Drupal::time()->getRequestTime()
            . '\' WHERE schema_name = \''
            . $chado_schema->getSchemaName()
            . "';\n"
          ;
          fwrite($this->parameters['fh'], $sql_query);
        }
        else {
          // If schema is integrated into Tripal, update version.
          $this->connection->update('chado_installations')
            ->fields([
              'version' => $version,
              'created' => \Drupal::time()->getRequestTime(),
              'updated' => \Drupal::time()->getRequestTime(),
            ])
            ->condition('schema_name', $chado_schema, '=')
            ->execute()
          ;
        }
        // @todo: Test transaction behavior.
      }
      catch (Exception $e) {
        $this->connection->query(
          'ROLLBACK;ROLLBACK;',
          [],
          ['allow_delimiter_in_query' => TRUE,]
        );
        // Rethrow exception.
        throw $e;
      }

      $this->setProgress(1);
      $task_success = TRUE;

      // Release all locks.
      $this->releaseTaskLocks();

      if ($this->parameters['fh']) {
        fclose($this->parameters['fh']);
        $this->parameters['fh'] = FALSE;
        $this->logger->notice(
          'All upgrade SQL queries were recorded into "'
          . $this->parameters['filename']
          . '" instead of being run.'
        );
      }

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
        "Failed to complete Chado installation task.\n"
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
    if (0 >= $progress) {
      $status = 'Upgrade not started yet.';
    }
    elseif (1 > $progress) {
      $status = 'Upgrade in progress';
    }
    else {
      $status = 'Upgrade done.';
    }
    return $status;
  }

  /**
   * Setups the refrence schema.
   */
  protected function setupReferenceSchema() {

    $ref_schema = $this->inputSchemas[0];
    $version = $this->parameters['version'];

    // Check if the schema already exists.
    if ($ref_schema->schema()->schemaExists()) {
      // Yes, check minimal structure.
      if (!$ref_schema->schema()->tableExists('chadoprop')) {
        throw new TaskException(
          'Reference schema ('
          . $ref_schema->getSchemaName()
          . ') does not contain chadoprop table. It seems it is not a complete >=1.3 Chado schema and it should be removed.'
        );
      }
    }
    else {
      // No, create a new reference schema.
      $ref_schema->schema()->createSchema();

      // Apply SQL file containing schema definitions.
      $module_path = \Drupal::service('extension.list.module')->getPath('tripal_chado');
      $file_path =
        $module_path
        . '/chado_schema/chado-only-'
        . $version
        . '.sql';

      // Run SQL file defining Chado schema.
      $success = $ref_schema->executeSqlFile(
        $file_path,
        ['chado' => $ref_schema->getQuotedSchemaName(),]
      );
      if ($success) {
        // Initialize schema with minimal data.
        $file_path =
          $module_path
          . '/chado_schema/initialize-'
          . $version
          . '.sql'
        ;
        $success = $ref_schema->executeSqlFile(
          $file_path,
          ['chado' => $ref_schema->getQuotedSchemaName(),]
        );
      }

      if (!$success) {
        // Failed to instantiate ref schema. Drop any partial ref schema.
        try {
          $ref_schema->schema()->dropSchema();
        }
        catch (Exception $e) {
          // Warn error in logs.
          $this->logger->error(
            'Failed to drop incomplete reference schema "'
            . $ref_schema->getSchemaName()
            . '": '
            . $e->getMessage()
          );
        }
        throw new TaskException(
          'Reference schema "'
          . $ref_schema->getSchemaName()
          . '" for update could not be initialized.'
        );
      }

      // Add version so the UI will detect the correct version of the reference
      // schema.
      $sql_query = "
        INSERT INTO {1:chadoprop} (type_id, value)
        VALUES (
          (
            SELECT cvt.cvterm_id
            FROM {1:cvterm} cvt
              INNER JOIN {1:cv} cv on cvt.cv_id = cv.cv_id
            WHERE cv.name = 'chado_properties' AND cvt.name = 'version'
          ),
          :version
        );
      ";
      $ref_schema->query($sql_query, [':version' => $version]);
    }
  }

  /**
   * Remove table column defaults.
   *
   * Since column defaults may use functions that need to be upgraded, we remove
   * those default in order to drop old functions without removing column
   * content.
   */
  protected function prepareDropColumnDefaults() {

    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get tables that are in current Chado schema and still exist in new
    // version.
    $sql_query = "
      SELECT
        DISTINCT c.relname
      FROM
        pg_class c
        JOIN pg_namespace n ON (
          n.oid = c.relnamespace
          AND n.nspname = :schema
        )
      WHERE
        c.relkind = 'r'
        AND c.relpersistence = 'p'
        AND EXISTS (
          SELECT TRUE
          FROM pg_class c2
            JOIN pg_namespace n2 ON (
              n2.oid = c2.relnamespace
              AND n2.nspname = :ref_schema
            )
          WHERE c2.relname = c.relname
            AND c2.relkind = 'r'
            AND c2.relpersistence = 'p'
        )
    ";
    // Here, we only consider table present in both ref and old schema as other
    // tables should be removed by cleanup.
    $tables = $chado_schema
      ->query(
        $sql_query,
        [
          ':schema' => $chado_schema->getSchemaName(),
          ':ref_schema' => $ref_schema->getSchemaName(),
        ]
      )
      ->fetchCol()
    ;

    foreach ($tables as $table) {
      // Get old table definition.
      $table_definition = $chado_schema->schema()->getTableDef(
        $table,
        [
          'source' => 'database',
          'format' => 'default',
        ]
      );
      foreach ($table_definition['columns'] as $column => $column_def) {
        if (!empty($column_def['default'])) {
          $this->upgradeQueries['#drop_column_defaults'][] =
            "ALTER TABLE "
            . $chado_schema->getQuotedSchemaName()
            . ".$table ALTER COLUMN $column DROP DEFAULT;"
          ;
        }
      }
    }
  }

  /**
   * Drop functions.
   */
  protected function prepareDropFunctions() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get the list of new functions.
    $sql_query = "
      SELECT
        p.proname AS \"proname\",
        replace(
          'DROP '
            || CASE
                WHEN p.prokind = 'a' THEN 'AGGREGATE'
                ELSE 'FUNCTION'
              END
            || ' IF EXISTS " . $chado_schema->getQuotedSchemaName() . ".'
            || quote_ident(p.proname)
            || '('
            ||  pg_get_function_identity_arguments(p.oid)
            || ') CASCADE',
          '" . $ref_schema->getQuotedSchemaName() . ".',
          '" . $chado_schema->getQuotedSchemaName() . ".'
        ) AS \"drop\"
      FROM pg_proc p
        JOIN pg_namespace n ON pronamespace = n.oid
      WHERE
        n.nspname = :ref_schema
        ORDER BY p.prokind ASC
      ;
    ";
    $proto_funcs = $this->connection
      ->query($sql_query, [
        ':ref_schema' => $ref_schema->getSchemaName(),
      ])
      ->fetchAll()
    ;
    foreach ($proto_funcs as $proto_func) {
      $this->upgradeQueries['#drop_functions'][] = $proto_func->drop . ';';
    }
  }

  /**
   * Drop all views of schema to upgrade.
   */
  protected function prepareDropAllViews() {
    $chado_schema = $this->outputSchemas[0];

    // Get views.
    $sql_query = "
      SELECT table_name
      FROM information_schema.views
      WHERE table_schema = :schema
      ORDER BY table_name
    ";
    $views = $this->connection
      ->query($sql_query, [':schema' => $chado_schema->getSchemaName()])
      ->fetchCol()
    ;
    // Drop all views of the schema.
    foreach ($views as $view) {
      $this->upgradeQueries['#drop_views'][] =
        "DROP VIEW IF EXISTS "
        . $chado_schema->getQuotedSchemaName()
        . ".$view CASCADE;"
      ;
    }
  }

  /**
   * Upgrade schema types.
   */
  protected function prepareUpgradeTypes() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get database types.
    $sql_query = "
      SELECT
        c.relkind,
        t.typname,
        t.typcategory,
        CASE
          WHEN t.typcategory='C' THEN
            array_to_string(
              array_agg(
                a.attname
                || ' '
                || pg_catalog.format_type(a.atttypid, a.atttypmod)
                ORDER BY c.relname, a.attnum
              ),
              ', '
            )
          WHEN t.typcategory = 'E' THEN
            REPLACE(
              quote_literal(
                array_to_string(
                  array_agg(e.enumlabel ORDER BY e.enumsortorder),','
                )
              ),
              ',',
              ''','''
            )
          ELSE ''
        END AS \"typdef\"
      FROM pg_type t
        JOIN pg_namespace n ON (n.oid = t.typnamespace)
        LEFT JOIN pg_enum e ON (t.oid = e.enumtypid)
        LEFT JOIN pg_class c ON (c.reltype = t.oid)
        LEFT JOIN pg_attribute a ON (a.attrelid = c.oid)
      WHERE n.nspname = :schema
        AND (c.relkind IS NULL OR c.relkind = 'c')
        AND t.typcategory IN ('C', 'E')
        GROUP BY 1,2,3
        ORDER BY t.typcategory, t.typname;
    ";
    $old_types = $this->connection
      ->query($sql_query, [':schema' => $chado_schema->getSchemaName()])
      ->fetchAllAssoc('typname')
    ;

    $new_types = $this->connection
      ->query($sql_query, [':schema' => $ref_schema->getSchemaName()])
      ->fetchAllAssoc('typname')
    ;

    // Check for missing or changed types.
    foreach ($new_types as $new_type_name => $new_type) {
      if (array_key_exists($new_type_name, $old_types)) {
        // Exists, compare.
        $old_type = $old_types[$new_type_name];
        if (($new_type->typcategory != $old_type->typcategory)
            || ($new_type->typdef != $old_type->typdef)) {
          // Recreate type.
          $this->upgradeQueries['#types'][] =
            "DROP TYPE IF EXISTS "
            . $chado_schema->getQuotedSchemaName()
            . ".$new_type_name CASCADE;";
          $this->upgradeQueries['#types'][] =
            "CREATE TYPE "
            . $chado_schema->getQuotedSchemaName()
            . ".$new_type_name AS "
            . ($new_type->typcategory == 'E' ? 'ENUM ' : '')
            . "("
            . $new_type->typdef
            . ");"
          ;
        }
        else {
          // Same types: remove from $new_types.
          unset($new_types[$new_type_name]);
        }
        // Processed: remove from $old_types.
        unset($old_types[$new_type_name]);
      }
      else {
        // Does not exist, add it.
        $this->upgradeQueries['#types'][] =
          "CREATE TYPE "
          . $chado_schema->getQuotedSchemaName()
          . ".$new_type_name AS "
          . ($new_type->typcategory == 'E' ? 'ENUM ' : '')
          . "("
          . $new_type->typdef
          . ");"
        ;
      }
    }
    // Report type changes.
    if (!empty($old_types)) {
      if ($this->parameters['cleanup']) {
        // Remove old types.
        foreach ($old_types as $old_type_name => $old_type) {
          $this->upgradeQueries['#cleanup'][] =
            "DROP TYPE IF EXISTS "
            . $chado_schema->getQuotedSchemaName()
            . ".$old_type_name CASCADE;"
          ;
        }
        $this->logger->warning(
          t(
            "The following schema types have been removed:\n%types",
            ['%types' => implode(', ', array_keys($old_types))]
          )
        );
      }
      else {
        $this->logger->warning(
          t(
            "The following schema types are not part of the new Chado schema specifications but have been left unchanged. If they are useless, they could be removed:\n%types",
            ['%types' => implode(', ', array_keys($old_types))]
          )
        );
      }
    }
    if (!empty($new_types)) {
      $this->logger->notice(
        t(
          "The following schema types were upgraded:\n%types",
          ['%types' => implode(', ', array_keys($new_types))]
        )
      );
    }
    if (empty($old_types) && empty($new_types)) {
      $this->logger->notice(t("All types were already up-to-date."));
    }
  }

  /**
   * Upgrade schema sequences.
   */
  protected function prepareUpgradeSequences() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get sequences.
    $sql_query = "
      SELECT
        sequence_name,
        data_type,
        start_value,
        minimum_value,
        maximum_value,
        increment,
        cycle_option
      FROM information_schema.sequences
      WHERE sequence_schema = :schema
      ORDER BY 1;
    ";
    $old_seqs = $this->connection
      ->query($sql_query, [':schema' => $chado_schema->getSchemaName(),])
      ->fetchAllAssoc('sequence_name')
    ;
    $new_seqs = $this->connection
      ->query($sql_query, [':schema' => $ref_schema->getSchemaName(),])
      ->fetchAllAssoc('sequence_name')
    ;

    // Check for missing or changed sequences.
    foreach ($new_seqs as $new_seq_name => $new_seq) {
      // Prepare creation/update query.
      $data_type_sql = ' AS ' . $new_seq->data_type;
      $increment_sql = (
        $new_seq->increment
        ? ' INCREMENT BY ' . $new_seq->increment
        : ''
      );
      $min_val_sql = (
        $new_seq->minimum_value
        ? ' MINVALUE ' . $new_seq->minimum_value
        : ' NO MINVALUE'
      );
      $max_val_sql = (
        $new_seq->maximum_value
        ? ' MAXVALUE ' . $new_seq->maximum_value
        : ' NO MAXVALUE'
      );
      $start_sql = (
        ($new_seq->start_value != '')
        ? ' START WITH ' . $new_seq->start_value
        : ''
      );
      // We don't manage sequence CACHE here, not set in Chado schema.
      $cycle_sql = (
        ('YES' == $new_seq->cycle_option)
        ? ' CYCLE'
        : ' NO CYCLE'
      );
      // Owning tables are managed later, once tables are upgraded.
      $create_update_seq_sql_query =
        ' SEQUENCE '
        . $chado_schema->getQuotedSchemaName()
        . '.'
        . $new_seq_name
        . $data_type_sql
        . $increment_sql
        . $min_val_sql
        . $max_val_sql
        . $start_sql
        . $cycle_sql
        . ' OWNED BY NONE;'
      ;

      if (array_key_exists($new_seq_name, $old_seqs)) {
        // Exists, compare.
        $old_seq = $old_seqs[$new_seq_name];

        if (($new_seq->start_value != $old_seq->start_value)
            || ($new_seq->minimum_value != $old_seq->minimum_value)
            || ($new_seq->maximum_value != $old_seq->maximum_value)
            || ($new_seq->increment != $old_seq->increment)
            || ($new_seq->cycle_option != $old_seq->cycle_option)
        ) {

          // Alter sequence.
          $this->upgradeQueries['#sequences'][] =
            'ALTER '
            . $create_update_seq_sql_query
          ;
        }
        else {
          // Same types: remove from $new_seqs.
          unset($new_seqs[$new_seq_name]);
        }
        // Processed: remove from $old_seqs.
        unset($old_seqs[$new_seq_name]);
      }
      else {
        // Does not exist, add it.
        $this->upgradeQueries['#sequences'][] =
          'CREATE '
          . $create_update_seq_sql_query
        ;
      }
    }

    // Report sequence changes.
    if (!empty($old_seqs)) {
      // Remove old sequences.
      if ($this->parameters['cleanup']) {
        foreach ($old_seqs as $old_seq_name => $old_seq) {
          $sql_query =
            "DROP SEQUENCE IF EXISTS "
            . $chado_schema->getQuotedSchemaName()
            . ".$old_seq_name CASCADE;"
          ;
          $this->upgradeQueries['#cleanup'][] = $sql_query;
        }
        $this->logger->warning(
          t(
            "The following sequences have been removed:\n%sequences",
            ['%sequences' => implode(', ', array_keys($old_seqs))]
          )
        );
      }
      else {
        $this->logger->warning(
          t(
            "The following schema sequences are not part of the new Chado schema specifications but have been left unchanged. If they are useless, they could be removed:\n%sequences",
            ['%sequences' => implode(', ', array_keys($old_seqs))]
          )
        );
      }
    }
    if (!empty($new_seqs)) {
      $this->logger->notice(
        t(
          "The following schema sequences were upgraded:\n%sequences",
          ['%sequences' => implode(', ', array_keys($new_seqs))]
        )
      );
    }
    if (empty($old_seqs) && empty($new_seqs)) {
      $this->logger->notice(t("All sequences were already up-to-date."));
    }
  }

  /**
   * Create prototype functions.
   *
   * Replace existing functions with same signature by protoype functions.
   * Prototype functions are functions with an empty body. Those functions will
   * be filled later with the upgraded content. The idea here is to be able to
   * link those functions in other database objects without having to deal with
   * function inter-dependencies (i.e. empty body, so no dependency inside) and
   * keep the same function reference when it will be upgraded.
   */
  protected function preparePrototypeFunctions() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    $sql_query = "
      SELECT
        p.proname AS \"proname\",
        p.proname
          || '('
          ||  pg_get_function_identity_arguments(p.oid)
          || ')'
        AS \"proident\",
        replace(
          'CREATE OR REPLACE FUNCTION "
          . $chado_schema->getQuotedSchemaName()
          . ".'
            || quote_ident(p.proname)
            || '('
            ||  pg_get_function_identity_arguments(p.oid)
            || ') RETURNS '
            || pg_get_function_result(p.oid)
            || ' LANGUAGE plpgsql '
            || CASE
                 WHEN p.provolatile = 'i' THEN ' IMMUTABLE'
                 ELSE ''
               END
            || ' AS \$_\$ BEGIN END# \$_\$#',
          '" . $ref_schema->getQuotedSchemaName() . ".',
          '" . $chado_schema->getQuotedSchemaName() . ".'
        ) AS \"proto\"
      FROM pg_proc p
        JOIN pg_namespace n ON pronamespace = n.oid
      WHERE
        n.nspname = :ref_schema
        AND prokind != 'a'
      ;
    ";
    $proto_funcs = $this->connection
      ->query($sql_query, [
        ':ref_schema' => $ref_schema->getSchemaName(),
      ])
      ->fetchAll()
    ;
    // We use internal PG connection to create functions as function body
    // contains ';' which is forbidden in Drupal DB queries.
    foreach ($proto_funcs as $proto_func) {
      // Drop previous version if exists (as it may have a different return
      // type and cause problems).
      $sql_query = preg_replace(
        '/^.*?FUNCTION\s+((?:[^\.]+\.)?[\w\$\x80-\xFF]+\s*\([^\)]*\)).*$/s',
        'DROP FUNCTION IF EXISTS \1 CASCADE;',
        $proto_func->proto
      );
     $proto_query = str_replace('#', ';', $proto_func->proto);

      $object_id = $proto_func->proident . ' proto';
      if (!isset($this->upgradeQueries[$object_id])) {
        $this->upgradeQueries[$object_id] = [];
      }
      $this->upgradeQueries[$object_id][] = $sql_query;
      $this->upgradeQueries[$object_id][] = $proto_query;
      // $this->dependencies[proto_func->proident] = [];
    }
  }

  /**
   * Upgrade schema tables.
   *
   * Note: Other modules can hook into this functionality by implementing
   *   HOOK_tripal_chado_column_upgrade OR HOOK_tripal_chado_column_upgrade_1_3.
   *   This allows other modules to specify column-specific upgrade procedures.
   *   First level keys are table names, second level keys are column names
   *   and values are array of 2 keys:
   *    'update' = a function to run to process update and return SQL queries
   *    'skip'   = an array of table name as keys and column names to skip as
   *       sub-keys. If no column names are specified, the whole table is skipped.
   *   For example,
   * @code
    function example_tripal_chado_column_upgrade(&$chado_column_upgrade) {
      $chado_column_upgrade = [
      'analysis' => [
        'analysis_id' => [
          'update' => function ($chado_schema, $ref_chado_schema, $cleanup) {
            $sql_queries = [];
            $sql_queries[] =
              "ALTER $ref_chado_schema.analysis ALTER COLUMN analysis_id ...";
            $sql_queries[] =
              "CREATE TABLE $ref_chado_schema.analysis_cvterm ...";
            $sql_queries[] =
              "INSERT INTO $ref_chado_schema.analysis_cvterm ...";
            return $sql_queries;
          },
          'skip' => [
            'analysis' => [
              'analysis_id' => [],
            ],
            'analysis_cvterm' => [],
          ],
        ],
      ],
    ];
    }
   * @endcode
   *
   */
  protected function prepareUpgradeTables() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Column-specific upgrade procedures.
    // See doc-block for further info on structure.
    $chado_column_upgrade = [];

    // Allow Tripal (custom) extensions to alter table upgrade.
    \Drupal::moduleHandler()->alter(
      ['tripal_chado_column_upgrade', 'tripal_chado_column_upgrade_1_3',],
      $chado_column_upgrade
    );

    // Get tables.
    $sql_query = "
      SELECT
        DISTINCT c.relname,
        c.relispartition,
        c.relkind,
        obj_description(c.oid) AS \"comment\"
      FROM pg_class c
      JOIN pg_namespace n ON ( n.oid = c.relnamespace AND n.nspname = :schema )
      WHERE
        c.relkind IN ('r','p')
        AND c.relpersistence = 'p'
      ORDER BY c.relkind DESC, c.relname";
    $old_tables = $this->connection
      ->query($sql_query, [':schema' => $chado_schema->getSchemaName()])
      ->fetchAllAssoc('relname');
    $new_tables = $this->connection
      ->query($sql_query, [':schema' => $ref_schema->getSchemaName()])
      ->fetchAllAssoc('relname');
    $context = [
      'processed_new_tables' => [],
      'new_table_definitions' => [],
      'skip_table_column' => [],
    ];

    // Check for existing tables with columns that can be updated through
    // specific functions (@see hook_tripal_chado_column_upgrade_alter())
    // and add them to the upgradeQueries task list.
    $this->prepareUpgradeTables_existingTables($chado_column_upgrade, $context, $old_tables);

    // Check for missing or changed tables.
    // 1. Add missing tables, upgrade columns on existing table,
    //    remove column defaults, all constraints and indexes.
    $this->prepareUpgradeTables_newTablesStep1($chado_column_upgrade, $context, $new_tables, $old_tables);

    // 2. Adds indexes and table constraints without foreign keys.
    $this->prepareUpgradeTables_newTablesStep2($chado_column_upgrade, $context, $new_tables, $old_tables);

    // 3. Adds foreign key constraints.
    $this->prepareUpgradeTables_newTablesStep3($chado_column_upgrade, $context, $new_tables, $old_tables);

    // Report table changes.
    if (!empty($old_tables)) {
      // Determine what tables were in the old version but not the new one.
      $removed_tables = [];
      foreach ($old_tables as $old_table_name => $old_table) {
        if (!array_key_exists($old_table_name, $new_tables)) {
          $removed_tables[$old_table_name] = $old_table;
        }
      }

      if (!empty($removed_tables)) {
        if ($this->parameters['cleanup']) {
          foreach ($removed_tables as $table_name => $table) {
            $sql_query =
              "DROP TABLE IF EXISTS "
              . $chado_schema->getQuotedSchemaName()
              . ".$table_name CASCADE;"
            ;
            $this->upgradeQueries['#cleanup'][] = $sql_query;
          }
          $this->logger->warning(
            t(
              "The following tables have been removed:\n%tables",
              ['%tables' => implode(', ', array_keys($removed_tables))]
            )
          );
        }
        else {
          $this->logger->warning(
            t(
              "The following tables are not part of the new Chado schema specifications but have been left unchanged. If they are useless, they could be removed:\n%tables",
              ['%tables' => implode(', ', array_keys($removed_tables))]
            )
          );
        }
      }
    }
    if (!empty($new_tables)) {
      $this->logger->notice(
        t(
          "The following schema tables were in the new schema and checked for upgrades:\n%tables",
          ['%tables' => implode(', ', array_keys($new_tables))]
        )
      );
    }
  }

  /**
   * Check for existing tables with columns that can be updated through
   * specific functions (@see hook_tripal_chado_column_upgrade_alter())
   * and add them to the upgradeQueries task list.
   *
   * @param array $chado_column_upgrade
   *   An array describing column-specific upgrade procedures. For the specific
   *   structure of this array see prepareUpgradeTables() above.
   * @param array $context
   *   An array providing a continuous context between methods.
   *   It consists of the following keys:
   *     - processed_new_tables: an array providing a list of processed tables.
   *     - new_table_definitions: an array providing a cached set of table definitions.
   *     - skip_table_column: an array of table columns which should be skipped
   *       since they have already been processed.
   * @param array $tables
   *   An array where the keys are table names in the current schema which is
   *   undergoing an upgrade. The values are objects specifying additional info
   *   about these tables.
   */
  private function prepareUpgradeTables_existingTables(&$chado_column_upgrade, &$context, $tables) {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    foreach (array_keys($tables) as $table_name) {

      // Initialize updateQueries section for this table.
      if (!isset($this->upgradeQueries[$table_name])) {
        $this->upgradeQueries[$table_name] = [];
      }

      // Check column update tasks ($chado_column_upgrade) for specific updates
      // (e.g. column renaming, value alteration, ...).
      if (array_key_exists($table_name, $chado_column_upgrade)) {

        // Get the table definition to use as a reference.
        $table_definition = $chado_schema->schema()->getTableDef(
          $table_name,
          [ 'source' => 'database', 'format' => 'default' ]
        );

        // Using thetable definition, check each column for column-sepcific changes.
        foreach ($table_definition['columns'] as $column => $column_def) {
          if (array_key_exists($column, $chado_column_upgrade[$table_name])) {

            // Init upgrade array.
            if (!isset($this->upgradeQueries[$table_name])) {
              $this->upgradeQueries[$table_name] = [];
            }

            // Get update queries.
            // NOTE: The update key of $chado_column_upgrade for a given table
            // and column should be the name of a function which can be executed
            // to retrieve update SQL queries.
            $function_name = $chado_column_upgrade[$table_name][$column]['update'];
            // Once we confirm that this function exists...
            if (function_exists($function_name)) {
              // We will execute it with the schema name for our current and
              // reference chado instance and the cleanup parameters.
              $upgrade_sql_queries = $function_name(
                $chado_schema->getSchemaName(),
                $ref_schema->getSchemaName(),
                $this->parameters['cleanup']
              );

              // Then the resulting SQL queries can be saved in the update task list.
              $this->upgradeQueries[$table_name][] = $upgrade_sql_queries;
            }

            // Mark column as processed.
            $context['skip_table_column'] += $chado_column_upgrade[$table_name][$column]['skip'];
          }
        }
      }
    }
  }

  /**
   * Step 1: Add missing tables, upgrade columns on existing table,
   * remove column defaults, all constraints and indexes to the upgradeQueries task list.
   *
   * @param array $chado_column_upgrade
   *   An array describing column-specific upgrade procedures. For the specific
   *   structure of this array see prepareUpgradeTables() above.
   * @param array $context
   *   An array providing a continuous context between methods.
   *   It consists of the following keys:
   *     - processed_new_tables: an array providing a list of processed tables.
   *     - new_table_definitions: an array providing a cached set of table definitions.
   *     - skip_table_column: an array of table columns which should be skipped
   *       since they have already been processed.
   * @param array $new_tables
   *   An array where the keys are table names in the current schema which is
   *   undergoing an upgrade. The values are objects specifying additional info
   *   about these tables.
   * @param array $old_tables
   *   An array where the keys are table names in the reference schema.
   *   The values are objects specifying additional info about these tables.
   */
  private function prepareUpgradeTables_newTablesStep1(&$chado_column_upgrade, &$context, $new_tables, $old_tables) {
    $chado_schema = $this->outputSchemas[0];
    $chado_schema_name = $chado_schema->getQuotedSchemaName();
    $ref_schema = $this->inputSchemas[0];
    $ref_schema_name = $ref_schema->getQuotedSchemaName();

    // For each table in the schema to be upgraded...
    foreach ($new_tables as $new_table_name => $new_table) {

      // Initialize an entry in the upgradeQueries task list for the new table
      // if the key is not already set.
      $this->upgradeQueries[$new_table_name] ??= [];

      // Get new table definition.
      // Note: We don't use the cache to retrieve since this is the first loop
      // for the new tables so we know it's not set yet.
      $new_table_definition = $ref_schema->schema()->getTableDef(
        $new_table_name,
        [ 'source' => 'database', 'format' => 'default', ]
      );


      // Check if table should be skipped.
      if (array_key_exists($new_table_name, $context['skip_table_column'])
          && empty($context['skip_table_column'][$new_table_name])) {
        continue;
      }

      // Check if the current table from the reference schema (i.e. new table),
      // is also in the schema to be updated (i.e. old tables).
      if (array_key_exists($new_table_name, $old_tables)) {

        // It is in both schema so now we want to compare them, looking for differences.
        $old_table = $old_tables[$new_table_name];

        // Get old table definition.
        $old_table_definition = $chado_schema->schema()->getTableDef(
          $new_table_name,
          [
            'source' => 'database',
            'format' => 'default',
          ]
        );

        $are_different = FALSE;

        // Start comparison.
        $alter_sql = [];

        // Compare columns between two schema.
        $column_alter_sql = $this->prepareUpgradeTables_newTablesStep1_compareColumns(
          $new_table_definition['columns'],
          $old_table_definition['columns'],
          [
            'ref_schema_name' => $ref_schema_name,
            'chado_schema_name' => $chado_schema_name,
          ]
        );
        $alter_sql = array_merge($alter_sql, $column_alter_sql);

        // Drop or Report remaining old columns.
        $column_drop_sql = $this->prepareUpgradeTables_newTablesStep1_remainingOldColumns(
          $old_table_definition['columns'],
          $new_table_name
        );
        $alter_sql = array_merge($alter_sql, $column_drop_sql);

        // Remove all constraints.
        $old_cstr_def = $old_table_definition['constraints'];
        foreach ($old_cstr_def as $old_constraint_name => $old_constraint_def) {
          $alter_sql[] =
            "DROP CONSTRAINT IF EXISTS $old_constraint_name CASCADE";
        }

        // Alter table.
        if (!empty($alter_sql)) {
          $sql_query =
            "ALTER TABLE " . $chado_schema_name . ".$new_table_name\n  "
            . implode(",\n  ", $alter_sql) . ';'
          ;

          $this->upgradeQueries[$new_table_name][] = $sql_query;
          $context['processed_new_tables'][] = $new_table_name;
        }

        // Remove all old indexes.
        foreach ($old_table_definition['indexes'] as $old_index_name => $old_index_def) {
          $sql_query =
            "DROP INDEX IF EXISTS " . $chado_schema_name
            . ".$old_index_name;"
          ;
          $this->upgradeQueries[$new_table_name][] = $sql_query;
        }

        // Saves table definition.
        $context['new_table_definitions'][$new_table_name] = $new_table_definition;
      }
      else {
        // Does not exist, add it.
        $sql_query =
          "CREATE TABLE "
          . $chado_schema_name
          . ".$new_table_name (LIKE "
          . $ref_schema_name
          . ".$new_table_name EXCLUDING DEFAULTS EXCLUDING CONSTRAINTS EXCLUDING INDEXES INCLUDING COMMENTS);"
        ;
        $this->upgradeQueries[$new_table_name][] = $sql_query;
        $context['processed_new_tables'][] = $new_table_name;

        // Saves table definition.
        $context['new_table_definitions'][$new_table_name] = $new_table_definition;
      }

      // Add comment.
      if (property_exists($new_table, 'comment') AND !empty($new_table->comment)) {
        $sql_query =
          "COMMENT ON TABLE "
          . $chado_schema_name
          . ".$new_table_name IS "
          . $this->connection->quote($new_table->comment)
          . ';';
        $this->upgradeQueries[$new_table_name][] = $sql_query;
      }
    }
  }

  /**
   * Step 1 HELPER: Compare columns for a single table between schema.
   *
   * @param array $new_table_columns
   *   An araay of the columns in a specific table in the reference schema. This
   *   array matches the format of the columns indec returned in the table definition.
   * @param array $old_table_columns
   *  An array of the columns in the same table in the schema to be updated.  This
   *   array matches the format of the columns indec returned in the table definition.
   * @param array $context
   *   An array of elements providing context. Specifically,
   *    - ref_schema_name: the name of the reference schema
   *    - chado_schema_name: the name of the schema to be updated
   * @return array
   *   An array of SQL statements to alter the columns in the schema to be
   *   updated to match the reference schema.
   */
  private function prepareUpgradeTables_newTablesStep1_compareColumns(&$new_table_columns, &$old_table_columns, $context) {

    // Compare columns for each column defined in the reference schema...
    foreach ($new_table_columns as $new_column => $new_column_def) {

      // Replace schema name in the column type if there is one.
      $new_column_type = str_replace(
        $context['ref_schema_name'] . '.',
        $context['chado_schema_name'] . '.',
        $new_column_def['type']
      );

      // Check if column exists in old table.
      if (array_key_exists($new_column, $old_table_columns)) {
        // Column exists, compare.
        $old_column_def = $old_table_columns[$new_column];
        // Data type.
        $old_type = $old_column_def['type'];

        if ($old_type != $new_column_type) {
          $alter_sql[] = "ALTER COLUMN $new_column TYPE $new_column_type";
        }
        // NULL option.
        $old_not_null = $old_column_def['not null'];
        $new_not_null = $new_column_def['not null'];
        if ($old_not_null != $new_not_null) {
          if ($new_not_null) {
            $alter_sql[] = "ALTER COLUMN $new_column SET NOT NULL";
          }
          else {
            $alter_sql[] = "ALTER COLUMN $new_column DROP NOT NULL";
          }
        }
        // No DEFAULT value at the time (added later).
        if (!empty($old_column_def['default'])) {
          $alter_sql[] = "ALTER COLUMN $new_column DROP DEFAULT";
        }
        // Remove processed column from old table data.
        unset($old_table_columns[$new_column]);
      }
      else {
        // Column does not exist, add (without default as it will be added
        // later).
        $new_not_null = $new_column_def['not null'];
        $alter_sql[] =
          "ADD COLUMN $new_column " . $new_column_type
          . ($new_not_null ? ' NOT NULL' : ' NULL' ) ;
      }
    }

    return $alter_sql;
  }

  /**
   * Step 1 HELPER: Drop or Report remaining old columns.
   *
   * @param array $old_col_def
   *   The column array of the old table schema definition.
   * @param string $table_name
   *   The name of the table
   * @return array
   *   An array of drop column SQL commands if we were told to cleanup.
   */
  private function prepareUpgradeTables_newTablesStep1_remainingOldColumns(array $old_col_def, string $table_name) {
    $alter_sql = [];

    if (!empty($old_col_def)) {
      if ($this->parameters['cleanup']) {
        foreach ($old_col_def as $old_column_name => $old_column) {
          $alter_sql[] = "DROP COLUMN $old_column_name";
        }
        $this->logger->notice(
          t(
            "The following columns of table '%table' have been removed:\n%columns",
            [
              '%columns' => implode(', ', array_keys($old_col_def)),
              '%table' => $table_name,
            ]
          )
        );
      }
      else {
        $this->logger->notice(
          t(
            "The following columns of table '%table' should be removed manually if not used:\n%columns",
            [
              '%columns' => implode(', ', array_keys($old_col_def)),
              '%table' => $table_name,
            ]
          )
        );
      }
    }

    return $alter_sql;
  }

  /**
   * Step 2: Adds indexes and table constraints without foreign keys to the
   * upgradeQueries task list.
   *
   * @param array $chado_column_upgrade
   *   An array describing column-specific upgrade procedures. For the specific
   *   structure of this array see prepareUpgradeTables() above.
   * @param array $context
   *   An array providing a continuous context between methods.
   *   It consists of the following keys:
   *     - processed_new_tables: an array providing a list of processed tables.
   *     - new_table_definitions: an array providing a cached set of table definitions.
   *     - skip_table_column: an array of table columns which should be skipped
   *       since they have already been processed.
   * @param array $new_tables
   *   An array where the keys are table names in the current schema which is
   *   undergoing an upgrade. The values are objects specifying additional info
   *   about these tables.
   * @param array $old_tables
   *   An array where the keys are table names in the reference schema.
   *   The values are objects specifying additional info about these tables.
   */
  private function prepareUpgradeTables_newTablesStep2(&$chado_column_upgrade, &$context, $new_tables, $old_tables) {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    foreach ($new_tables as $new_table_name => $new_table) {
      // Check if table should be skipped.
      if (array_key_exists($new_table_name, $context['skip_table_column'])
          && empty($context['skip_table_column'][$new_table_name])) {
        continue;
      }

      $upgq_id = $new_table_name . ' 2nd pass';
      if (!isset($this->upgradeQueries[$upgq_id])) {
        $this->upgradeQueries[$upgq_id] = [];
      }

      $alter_sql = [];
      $new_table_def = $context['new_table_definitions'][$new_table_name];

      // Generate SQL statements to add all contraints for this table.
      // Also keep track of all implicit indecies so they can be skipped later.
      $index_to_skip = [];
      $add_constrain_sql = $this->prepareUpgradeTables_newTablesStep2_addConstraints(
        $new_table_def['constraints'],
        $index_to_skip
      );
      $alter_sql = array_merge($alter_sql, $add_constrain_sql);

      // Alter table.
      if (!empty($alter_sql)) {
        $sql_query =
          "ALTER TABLE "
          . $chado_schema->getQuotedSchemaName()
          . ".$new_table_name\n  "
          . implode(",\n  ", $alter_sql)
          . ';'
        ;
        $this->upgradeQueries[$upgq_id][] = $sql_query;
      }

      // Create new indexes.
      // Queries are added directly to the upgradeQueries queue.
      $this->prepareUpgradeTables_newTablesStep2_addIndicies(
        $new_table_def['indexes'],
        $index_to_skip,
        $upgq_id
      );
    }
  }

  /**
   * Step 2 HELPER: Generate SQL statements to add all contraints for this table.
   *
   * @param array $new_cstr_def
   *    An array of the contraint definitions we want to add.
   * @param array $index_to_skip
   *    An array which starts out empty but allows us to keep track of implicit
   *    indicies so they can be skipped later on in step 2.
   * @return array
   *    An array of SQL statements to be added to the queue.
   */
  private function prepareUpgradeTables_newTablesStep2_addConstraints(array $new_cstr_def, array &$index_to_skip) {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    $alter_sql = [];
    foreach ($new_cstr_def as $new_constraint_name => $new_constraint_def) {

      // Skip foreign keys for now.
      if (preg_match('/(?:^|\s)FOREIGN\s+KEY(?:\s|$)/', $new_constraint_def)) {
        continue;
      }

      $constraint_def = str_replace(
        $ref_schema->getQuotedSchemaName() . '.',
        $chado_schema->getQuotedSchemaName() . '.',
        $new_constraint_def
      );
      $alter_sql[] = "ADD CONSTRAINT $new_constraint_name $constraint_def";

      // Skip implicit indexes.
      if (preg_match('/(?:^|\s)(?:UNIQUE|PRIMARY\s+KEY)(?:\s|$)/', $constraint_def)) {
        $index_to_skip[$new_constraint_name] = TRUE;
      }
    }

    return $alter_sql;
  }

  /**
   * Step 2 HELPER: Create all indicies.
   *
   * @param array $new_index_def
   *    An array of the index definitions we want to add.
   * @param array $index_to_skip
   *    An array of implicit indicies we want to skip.
   * @param string $upgq_id
   *    The key for this table in the upgradeQueries queue.
   */
  private function prepareUpgradeTables_newTablesStep2_addIndicies($new_index_def, $index_to_skip, $upgq_id) {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    foreach ($new_index_def as $new_index_name => $new_index_def) {

      // We always want to add the comment if there is one,
      // so do that first.
      $sql_query =
        "SELECT
          'COMMENT ON INDEX "
          . $chado_schema->getQuotedSchemaName()
          . ".' || quote_ident(c.relname) || ' IS ' || quote_literal(d.description) AS \"comment\"
        FROM pg_class c
          JOIN pg_namespace n ON (n.oid = c.relnamespace)
          JOIN pg_index i ON (i.indexrelid = c.oid)
          JOIN pg_description d ON (d.objoid = c.oid)
        WHERE
          c.reltype = 0
          AND n.nspname = :ref_schema
          AND c.relname = :index_name;";
      $args = [
        ':ref_schema' => $ref_schema->getSchemaName(),
        ':index_name' => $new_index_name,
      ];
      $comment_result = $this->connection->query($sql_query, $args)->fetch();
      if (!empty($comment_result) && !empty($comment_result->comment)) {
        $this->upgradeQueries[$upgq_id][] = $comment_result->comment . ';';
      }

      // Now check if we should skip adding the index query for this index.
      if (isset($index_to_skip[$new_index_name])) {
        continue;
      }

      // Finally add the index query if this index was not skipped.
      $index_def = str_replace(
        $ref_schema->getQuotedSchemaName() . '.',
        $chado_schema->getQuotedSchemaName() . '.',
        $new_index_def['query']
      );
      $this->upgradeQueries[$upgq_id][] = $index_def;
    }
  }

  /**
   * Step 3: Adds foreign key constraints to the upgradeQueries task list.
   *
   * @param array $chado_column_upgrade
   *   An array describing column-specific upgrade procedures. For the specific
   *   structure of this array see prepareUpgradeTables() above.
   * @param array $context
   *   An array providing a continuous context between methods.
   *   It consists of the following keys:
   *     - processed_new_tables: an array providing a list of processed tables.
   *     - new_table_definitions: an array providing a cached set of table definitions.
   *     - skip_table_column: an array of table columns which should be skipped
   *       since they have already been processed.
   * @param array $new_tables
   *   An array where the keys are table names in the current schema which is
   *   undergoing an upgrade. The values are objects specifying additional info
   *   about these tables.
   * @param array $old_tables
   *   An array where the keys are table names in the reference schema.
   *   The values are objects specifying additional info about these tables.
   */
  private function prepareUpgradeTables_newTablesStep3(&$chado_column_upgrade, &$context, $new_tables, $old_tables) {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    foreach ($new_tables as $new_table_name => $new_table) {
      // Check if table should be skipped.
      if (array_key_exists($new_table_name, $context['skip_table_column'])
          && empty($context['skip_table_column'][$new_table_name])) {
        continue;
      }

      $upgq_id = $new_table_name . ' 3rd pass';
      if (!isset($this->upgradeQueries[$upgq_id])) {
        $this->upgradeQueries[$upgq_id] = [];
      }

      $alter_sql = [];
      $new_table_def = $context['new_table_definitions'][$new_table_name];
      $new_cstr_def = $new_table_def['constraints'];
      $index_to_skip = [];

      foreach ($new_cstr_def as $new_constraint_name => $new_constraint_def) {
        // Only foreign keys.
        if (preg_match('/(?:^|\s)FOREIGN\s+KEY(?:\s|$)/', $new_constraint_def)) {
          $constraint_def = str_replace(
            $ref_schema->getQuotedSchemaName() . '.',
            $chado_schema->getQuotedSchemaName() . '.',
            $new_constraint_def
          );
          $alter_sql[] =
            "ADD CONSTRAINT $new_constraint_name $constraint_def"
          ;
        }
      }

      // Alter table.
      if (!empty($alter_sql)) {
        $sql_query =
          "ALTER TABLE "
          . $chado_schema->getQuotedSchemaName()
          . ".$new_table_name\n  "
          . implode(",\n  ", $alter_sql)
          . ';'
        ;
        $this->upgradeQueries[$upgq_id][] = $sql_query;
      }
    }
  }

  /**
   * Associate sequences.
   */
  protected function prepareSequenceAssociation() {

    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get the list of sequences.
    $sql_query = "
    SELECT sequence_name
      FROM information_schema.sequences
     WHERE sequence_schema = :ref_schema
      ;
    ";
    $sequences = $this->connection
      ->query($sql_query, [
        ':ref_schema' => $ref_schema->getSchemaName(),
      ])->fetchCol()
    ;
    foreach ($sequences as $sequence) {
      $sql_query = "
        SELECT
          quote_ident(dc.relname) AS \"relname\",
          quote_ident(a.attname) AS \"attname\"
      FROM pg_class AS c
        JOIN pg_depend AS d ON (c.relfilenode = d.objid)
        JOIN pg_class AS dc ON (d.refobjid = dc.relfilenode)
        JOIN pg_attribute AS a ON (
          a.attnum = d.refobjsubid
          AND a.attrelid = d.refobjid
        )
        JOIN pg_namespace n ON c.relnamespace = n.oid
      WHERE n.nspname = :ref_schema
        AND c.relkind = 'S'
        AND c.relname = :sequence;
      ";
      $relation = $this->connection->query(
        $sql_query,
        [
          ':sequence' => $sequence,
          ':ref_schema' => $ref_schema->getSchemaName(),
        ]
      );
      if ($relation && ($relation = $relation->fetch())) {
        $sql_query =
          'ALTER SEQUENCE '
          . $chado_schema->getQuotedSchemaName()
          . '.'
          . $sequence
          . ' OWNED BY '
          . $chado_schema->getQuotedSchemaName()
          . '.'
          . $relation->relname
          . '.'
          . $relation->attname
          . ';'
        ;
        // Array should have been initialized by table upgrade before.
        $this->upgradeQueries[$relation->relname][] = $sql_query;
      }
    }
  }

  /**
   * Upgrade views.
   */
  protected function prepareUpgradeViews() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get the list of new views.
    $sql_query = "
      SELECT
        table_name,
        regexp_replace(
          view_definition::text,
          :regex_search,
          :regex_replace,
          'gis'
        ) AS \"def\"
      FROM information_schema.views
      WHERE table_schema = :ref_schema
      ;
    ";
    $views = $this->connection
      ->query($sql_query, [
        ':ref_schema' => $ref_schema->getSchemaName(),
        ':regex_search' => '(^|\W)' . $ref_schema->getQuotedSchemaName(). '\.',
        ':regex_replace' => '\1' . $chado_schema->getQuotedSchemaName() . '.',
      ])
      ->fetchAll()
    ;
    foreach ($views as $view) {
      if (!isset($this->upgradeQueries[$view->table_name])) {
        $this->upgradeQueries[$view->table_name] = [];
      }
      $sql_query =
        'CREATE OR REPLACE VIEW '
        . $chado_schema->getQuotedSchemaName()
        . '.'
        . $view->table_name
        . ' AS '
        . $view->def
      ;
      $this->upgradeQueries[$view->table_name][] = $sql_query;
      // Add comment if one.
      $sql_query = "
        SELECT obj_description(c.oid) AS \"comment\"
        FROM pg_class c,
          pg_namespace n
        WHERE n.nspname = :ref_schema
          AND c.relnamespace = n.oid
          AND c.relkind = 'v'
          AND c.relname = :view_name
        ;
      ";
      $comment = $this->connection
        ->query($sql_query, [
          ':ref_schema' => $ref_schema->getSchemaName(),
          ':view_name' => $view->table_name,
        ])
      ;
      if ($comment
          && ($comment = $comment->fetch())
          && !empty($comment->comment)
      ) {
        $sql_query =
          "COMMENT ON VIEW "
          . $chado_schema->getQuotedSchemaName()
          . "."
          . $view->table_name
          . " IS " . $this->connection->quote($comment->comment)
          . ';'
        ;
        $this->upgradeQueries[$view->table_name][] = $sql_query;
      }
    }
  }

  /**
   * Upgrade functions.
   */
  protected function prepareFunctionUpgrade() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get the list of new functions.
    $sql_query = "
        SELECT
          p.oid,
          p.proname,
          p.proname
            || '('
            ||  pg_get_function_identity_arguments(p.oid)
            || ')'
          AS \"proident\",
          regexp_replace(
            regexp_replace(
              pg_get_functiondef(p.oid),
              :regex_search,
              :regex_replace,
              'gis'
            ),
            '" . $ref_schema->getQuotedSchemaName() . "\\.',
            '" . $chado_schema->getQuotedSchemaName() . ".',
            'gis'
          ) AS \"def\"
          FROM pg_proc p
            JOIN pg_namespace n ON p.pronamespace = n.oid
        WHERE
          n.nspname = :ref_schema
          AND p.prokind != 'a'
        ;
    ";
    $funcs = $this->connection
      ->query($sql_query, [
        ':ref_schema' => $ref_schema->getSchemaName(),
        ':regex_search' => '^\s*CREATE\s+FUNCTION',
        ':regex_replace' => 'CREATE OR REPLACE FUNCTION',
      ])
      ->fetchAll()
    ;

    foreach ($funcs as $func) {
      // Update prototype.
      $object_id = $func->proident;
      if (!isset($this->upgradeQueries[$object_id])) {
        $this->upgradeQueries[$object_id] = [];
      }
      $this->upgradeQueries[$object_id][] = $func->def . ';';
    }
    if ($this->parameters['cleanup']) {
      // Get the list of functions not in the official Chado release.
      $sql_query = "
        SELECT
          regexp_replace(
            pg_get_functiondef(p.oid),
            :regex_search,
            :regex_replace,
            'gis'
          ) AS \"func\"
        FROM pg_proc p
          JOIN pg_namespace n ON p.pronamespace = n.oid
        WHERE
          n.nspname = :schema
          AND p.prokind != 'a'
          AND NOT EXISTS (
            SELECT TRUE
            FROM pg_proc pr
              JOIN pg_namespace nr ON pr.pronamespace = nr.oid
            WHERE
              nr.nspname = :ref_schema
              AND pr.prokind != 'a'
              AND pr.proname = p.proname
              AND regexp_replace(
                    pg_get_functiondef(pr.oid),
                    :regex_search,
                    :regex_replace,
                    'gis'
                  )
                = regexp_replace(
                    pg_get_functiondef(p.oid),
                    :regex_search,
                    :regex_replace,
                    'gis'
                  )
          );
      ";
      $old_funcs = $this->connection
        ->query(
          $sql_query,
          [
            ':ref_schema' => $ref_schema->getSchemaName(),
            ':schema' => $chado_schema->getSchemaName(),
            // Extract the function name and its parameters (without the schema
            // name)
            ':regex_search' => '^\s*CREATE\s+(?:OR\s+REPLACE\s+)?FUNCTION\s+(?:[^\.\s]+\.)?([^\)]+\)).*$',
            ':regex_replace' => '\1',
          ]
        )
        ->fetchCol()
      ;
      foreach ($old_funcs as $old_func) {
        $sql_query =
          "DROP FUNCTION IF EXISTS "
          . $chado_schema->getQuotedSchemaName()
          . ".$old_func CASCADE;"
        ;
        $this->upgradeQueries['#cleanup'][] = $sql_query;
      }
      $this->logger->warning(
        t(
          "The following functions have been removed:\n%functions",
          ['%functions' => implode(', ', $old_funcs)]
        )
      );
    }
  }

  /**
   * Upgrade aggregate functions.
   */
  protected function prepareAggregateFunctionUpgrade() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get the list of new aggregate functions.
    $sql_query = "
      SELECT
        p.proname AS \"proname\",
        p.proname
          || '('
          ||  pg_get_function_identity_arguments(p.oid)
          || ')'
        AS \"proident\",
        'DROP AGGREGATE IF EXISTS "
      . $chado_schema->getQuotedSchemaName()
      . ".'
        || p.proname
        || '('
        || format_type(a.aggtranstype, NULL)
        || ')' AS \"drop\",
        'CREATE AGGREGATE "
      . $chado_schema->getQuotedSchemaName()
      . ".'
        || p.proname
        || '('
        || format_type(a.aggtranstype, NULL)
        || ') (sfunc = '
        || regexp_replace(a.aggtransfn::text, '(^|\\W)"
      . $ref_schema->getQuotedSchemaName()
      . "\\.', '\\1"
      . $chado_schema->getQuotedSchemaName()
      . ".', 'gis')
        || ', stype = '
        || format_type(a.aggtranstype, NULL)
        || CASE
             WHEN op.oprname IS NULL THEN ''
             ELSE ', sortop = ' || op.oprname
           END
        || CASE
             WHEN a.agginitval IS NULL THEN ''
             ELSE ', initcond = ''' || a.agginitval || ''''
           END
        || ')' AS \"def\"
      FROM
        pg_proc p
          JOIN pg_namespace n ON p.pronamespace = n.oid
          JOIN pg_aggregate a ON a.aggfnoid = p.oid
          LEFT JOIN pg_operator op ON op.oid = a.aggsortop
      WHERE
        n.nspname = :ref_schema
        AND p.prokind = 'a'
      ;
    ";
    $aggrfuncs = $this->connection
      ->query(
        $sql_query,
        [':ref_schema' => $ref_schema->getSchemaName()]
      )
      ->fetchAll()
    ;
    // Keep track of official aggregate functions.
    $official_aggregate = [];
    foreach ($aggrfuncs as $aggrfunc) {
      // Drop previous version and add a new one.
      $object_id = $aggrfunc->proident;
      if (!isset($this->upgradeQueries[$object_id])) {
        $this->upgradeQueries[$object_id] = [];
      }
      $this->upgradeQueries[$object_id][] = $aggrfunc->drop . ';';
      $this->upgradeQueries[$object_id][] = $aggrfunc->def . ';';
      $official_aggregate[$aggrfunc->drop] = TRUE;
    }

    // Cleanup if needed.
    if ($this->parameters['cleanup']) {
      $sql_query = "
        SELECT
          'DROP AGGREGATE IF EXISTS "
          . $chado_schema->getQuotedSchemaName()
          . ".'
          || p.proname
          || '('
          || format_type(a.aggtranstype, NULL)
          || ')' AS \"drop\"
        FROM
          pg_proc p
            JOIN pg_namespace n ON p.pronamespace = n.oid
            JOIN pg_aggregate a ON a.aggfnoid = p.oid
            LEFT JOIN pg_operator op ON op.oid = a.aggsortop
        WHERE
          n.nspname = :schema
          AND p.prokind = 'a'
        ;
      ";
      $aggrfuncs = $this->connection
        ->query(
          $sql_query,
          [':schema' => $chado_schema->getSchemaName()]
        )
        ->fetchAll()
      ;
      // Drop aggregate functions not met in the reference schema.
      $dropped = [];
      foreach ($aggrfuncs as $aggrfunc) {
        if (!array_key_exists($aggrfunc->drop, $official_aggregate)) {
          $this->upgradeQueries['#cleanup'][] = $aggrfunc->drop . ';';
          $dropped[] = preg_replace(
            '/DROP AGGREGATE IF EXISTS ([^\)]+\))/',
            '\1',
            $aggrfunc->drop
          );
        }
      }
      if (!empty($dropped)) {
        $this->logger->warning(
          t(
            "The following aggregate functions have been removed:\n%agg",
            ['%agg' => implode(', ', $dropped)]
          )
        );
      }
    }
  }

  /**
   * Upgrade table column defaults.
   */
  protected function prepareUpgradeTableDefauls() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    // Get tables.
    $sql_query = "
      SELECT
        DISTINCT c.relname,
        c.relispartition,
        c.relkind,
        obj_description(c.oid) AS \"comment\"
      FROM
        pg_class c
        JOIN pg_namespace n ON (
          n.oid = c.relnamespace
          AND n.nspname = :schema
        )
      WHERE
        c.relkind IN ('r','p')
        AND c.relpersistence = 'p'
      ORDER BY c.relkind DESC, c.relname
    ";
    $new_tables = $this->connection
      ->query($sql_query, [':schema' => $ref_schema->getSchemaName()])
      ->fetchAllAssoc('relname')
    ;

    // Process all tables.
    foreach ($new_tables as $new_table_name => $new_table) {
      $this->upgradeQueries[$new_table_name . ' set default'] = [];

      // Get new table definition.
      $new_table_definition = $ref_schema->schema()->getTableDef(
        $new_table_name,
        [
          'source' => 'database',
          'format' => 'default',
        ]
      );

      $new_column_defs = $new_table_definition['columns'];
      foreach ($new_column_defs as $new_column => $new_column_def) {
        // Replace schema name if there.
        if (isset($new_column_def['default'])) {
          $new_default = str_replace(
            $ref_schema->getQuotedSchemaName() . '.',
            $chado_schema->getQuotedSchemaName() . '.',
            $new_column_def['default']
          );
          $sql_query =
            "ALTER TABLE "
            . $chado_schema->getQuotedSchemaName()
            . ".$new_table_name ALTER COLUMN $new_column SET DEFAULT "
            . $new_default
            . ';';
          $this->upgradeQueries[$new_table_name . ' set default'][] =
            $sql_query
          ;
        }
      }
    }
  }

  /**
   * Upgrade comment.
   */
  protected function prepareCommentUpgrade() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];

    $this->upgradeQueries['#comments'] = [];

    $column_with_comment = [];
    // Find comment on columns.
    $sql_query = "
      SELECT
        n.nspname,
        cs.relname,
        a.attname,
        d.description
      FROM pg_class cs
        JOIN pg_namespace n ON (n.oid = cs.relnamespace)
        JOIN pg_description d ON (d.objoid = cs.oid)
        JOIN pg_attribute a ON (a.attrelid = cs.oid AND d.objsubid = a.attnum)
      WHERE n.nspname = :schema;
    ";
    $column_comments = $this->connection
      ->query($sql_query, [':schema' => $ref_schema->getSchemaName()])
      ->fetchAll()
    ;
    foreach ($column_comments as $column_comment) {
      if ($column_comment->description !== NULL) {
        $this->upgradeQueries['#comments'][] =
          'COMMENT ON COLUMN '
          . $column_comment->relname
          . '.'
          . $column_comment->attname
          . ' IS '
          . $this->connection->quote($column_comment->description)
          . ';'
        ;
        if (!array_key_exists($column_comment->relname, $column_with_comment)) {
          $column_with_comment[$column_comment->relname] = [];
        }
        // Keep track of what is commented.
        $column_with_comment[$column_comment->relname][$column_comment->attname]
          = TRUE;
      }
    }

    // Drop old comments.
    $sql_query = "
      SELECT
        n.nspname,
        cs.relname,
        a.attname,
        d.description
      FROM pg_class cs
        JOIN pg_namespace n ON (n.oid = cs.relnamespace)
        JOIN pg_description d ON (d.objoid = cs.oid)
        JOIN pg_attribute a ON (a.attrelid = cs.oid AND d.objsubid = a.attnum)
      WHERE n.nspname = :schema;
    ";
    $old_column_comments = $this->connection
      ->query($sql_query, [':schema' => $chado_schema->getSchemaName()])
      ->fetchAll()
    ;
    foreach ($old_column_comments as $old_column_comment) {
      $table = $old_column_comment->relname;
      $column = $old_column_comment->attname;
      $no_comment = empty($column_with_comment[$table][$column]);
      if ($no_comment) {
        if ($this->parameters['cleanup']) {
          $this->upgradeQueries['#comments'][] =
            'COMMENT ON COLUMN '
            . $old_column_comment->relname
            . '.'
            . $old_column_comment->attname
            . ' IS NULL;'
          ;
        }
        else {
          $this->logger->warning(
            t(
              'The comment on column %table.%column can be removed.',
              [
                '%table' => $old_column_comment->relname,
                '%column' => $old_column_comment->attname,
              ]
            )
          );
        }
      }
    }
  }

  /**
   * Add missing initialization data.
   */
  protected function reinitSchema() {
    $chado_schema = $this->outputSchemas[0];
    $ref_schema = $this->inputSchemas[0];
    $version = $this->parameters['version'];

    // Get initialization script.
    $module_path = \Drupal::service('extension.list.module')->getPath('tripal_chado');
    $sql_file = $module_path . '/chado_schema/initialize-' . $version . '.sql';
    $sql = file_get_contents($sql_file);
    // Remove any search_path change containing 'chado' as a schema name.
    $sql = preg_replace(
      '/(^|\W)SET\s*search_path\s*=(?:[^;]+,|)\s*chado\s*(,[^;]+|);/im',
      '\1',
      $sql
    );
    $this->upgradeQueries['#init'] = [$sql];
    $this->upgradeQueries['#init'][] = "
      INSERT INTO "
      . $chado_schema->getQuotedSchemaName()
      . ".chadoprop (type_id, value, rank)
      VALUES (
        (
          SELECT cvterm_id
          FROM "
      . $chado_schema->getQuotedSchemaName()
      . ".cvterm CVT
            INNER JOIN "
      . $chado_schema->getQuotedSchemaName()
      . ".cv CV on CVT.cv_id = CV.cv_id
          WHERE CV.name = 'chado_properties' AND CVT.name = 'version'
        ),
        '$version',
        0
      ) ON CONFLICT (type_id, rank) DO UPDATE SET value = '$version';
    ";
  }

  /**
   * Process upgrades.
   *
   * Execute SQL queries or save them into a SQL instead if $filename is set.
   * Queries are ordered according to priorities and what must be run in the
   * end.
   */
  protected function processUpgradeQueries() {

    // Setup DB object upgrade priority according to Chado version.
    $priorities = [];
    switch ($this->parameters['version']) {
      case '1.3':
        $priorities = $this::CHADO_OBJECT_PRIORITY_13;
        break;
    }

    $skip_objects = [];
    $fh = $this->parameters['fh'];

    foreach ($this->upgradeQueries as $object_id => $upgrade_queries) {
      // Skip #end elements that will be processed in the end.
      if ('#end' == $object_id) {
        continue;
      }
      // Process prioritized objects now (remove them from the regular queue and
      // add them to the priorities queue).
      if ('#priorities' == $object_id) {
        foreach ($priorities as $priority) {
          if (array_key_exists($priority, $this->upgradeQueries)) {
            $this->upgradeQueries['#priorities'] = array_merge(
              $this->upgradeQueries['#priorities'],
              $this->upgradeQueries[$priority]
            );
          }
          else {
            throw new TaskException(
              "Failed to prioritize object '$priority': object not found in schema definition!"
            );
          }
          $skip_objects[$priority] = TRUE;
        }
        // Update current variable.
        $upgrade_queries = $this->upgradeQueries['#priorities'];
      }
      // Skip objects already processed (priorities).
      if (array_key_exists($object_id, $skip_objects)) {
        continue;
      }
      if ($fh) {
        foreach ($upgrade_queries as $sql_query) {
          fwrite($fh, $sql_query . "\n");
        }
      }
      else {
        foreach ($upgrade_queries as $sql_query) {
          $this->connection->query(
            $sql_query,
            [],
            ['allow_delimiter_in_query' => TRUE,]
          );
        }
      }
    }
    if ($fh) {
      foreach (array_reverse($this->upgradeQueries['#end']) as $sql_query ) {
        fwrite($fh, $sql_query . "\n");
      }
    }
    else {
      foreach (array_reverse($this->upgradeQueries['#end']) as $sql_query ) {
        $this->connection->query(
          $sql_query,
          [],
          ['allow_delimiter_in_query' => TRUE,]
        );
      }
    }

    // Clear queries.
    $this->upgradeQueries = [];
  }
}

/**
 * Hook to alter tripal_chado_column_upgrade variable.
 *
 * @see prepareUpgradeTables()
 */
function hook_tripal_chado_column_upgrade_alter(&$chado_column_upgrade) {
  $chado_column_upgrade = array_merge(
    $chado_column_upgrade,
    [
      'analysis' => [
        'analysis_id' => [
          'update' => function ($chado_schema, $ref_chado_schema, $cleanup) {
            $sql_queries = [];
            $sql_queries[] =
              "ALTER $ref_chado_schema.analysis ALTER COLUMN analysis_id ...";
            $sql_queries[] =
              "CREATE TABLE $ref_chado_schema.analysis_cvterm ...";
            $sql_queries[] =
              "INSERT INTO $ref_chado_schema.analysis_cvterm ...";
            return $sql_queries;
          },
          'skip' => [
            'analysis' => [
              'analysis_id' => [],
            ],
            'analysis_cvterm' => [],
          ],
        ],
      ],
    ]
  );
}
