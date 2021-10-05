<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\Services\ChadoManager;

/**
 * Some code and methods were "borrowed" from \Drupal\Core\Test\TestDatabase
 * class.
 */
class ChadoTemporarySchemaGenerator extends ChadoManager {


  /**
   * A random number used to ensure that test fixtures are unique to each test
   * method.
   *
   * @var int
   */
  protected $lockId;

  /**
   * The Chaod test schema name.
   *
   * @var string
   */
  protected $schemaName;

  /**
   * TestDatabase constructor.
   *
   * @param string|null $schema_suffix
   *   If not provided a new test lock is generated.
   * @param bool $create_lock
   *   (optional) Whether or not to create a lock file. Defaults to FALSE. If
   *   the environment variable RUN_TESTS_CONCURRENCY is greater than 1 it will
   *   be overridden to TRUE regardless of its initial value.
   *
   * @throws \InvalidArgumentException
   *   Thrown when $schema_suffix does not match the regular expression.
   */
  public function __construct($schema_suffix = NULL, $create_lock = FALSE) {
    parent::__construct();
    if ($schema_suffix === NULL) {
      $this->lockId = $this
        ->getTestLock($create_lock);
      $this->databasePrefix = 'test' . $this->lockId;
    }
    else {
      $this->databasePrefix = $schema_suffix;

      // It is possible that we're running a test inside a test. In which case
      // $schema_suffix will be something like test12345678test90123456 and the
      // generated lock ID for the running test method would be 90123456.
      preg_match('/test(\\d+)$/', $schema_suffix, $matches);
      if (!isset($matches[1])) {
        throw new \InvalidArgumentException("Invalid database prefix: {$schema_suffix}");
      }
      $this->lockId = $matches[1];
    }
  }

  /**
   * Generate a new test schema from a version template.
   *
   * If the version template does not exist, it will be created first. Then,
   * a new clone of that template will be generated and its schema name
   * returned.
   *
   * @param string $version
   *   Version number. Currently, only '1.3' is supported.
   *   Default: '1.3'
   * @param integer $expiration_timestamp
   *   The time after wich the schema can be safely removed. If not set, it will
   *   be set to current time + 1 day.
   *
   * @return string
   *   The name of the new test schema generated.
   */
  public static function generate(
    string $version = '1.3',
    ?integer $expiration_timestamp = NULL
  ) {
    if ($version != '1.3') {
      throw new \Exception("Invalid or unsupported Chado schema version '$version'.");
    }
    
    if (!self::$test_mode) {
      throw new \Exception("generateTestSchema() called while not in debug mode. Please call 'ChadoSchema::testMode(TRUE);' first.");
    }

    if (empty($expiration_timestamp)) {
      // 86400 sec = 1 day
      $expiration_timestamp = time() + 86400;
    }

    throw new \Exception("Not implemented.");
    // note: an expiration date should be incorporated in order to cleanup old test
    // schemas.
    return;
  }

  /**
   * Install chado in the specified schema.
   *
   * @param float $version
   *   The version of chado you would like to install.
   */
  public function generate($version) {
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
    $schema_issue = \Drupal\tripal_chado\api\ChadoSchema::isInvalidSchemaName($chado_schema);
    if ($schema_issue) {
      // Schema name must be a single word containing only lower case letters
      // or numbers and cannot begin with a number.
      $this->logger->error($schema_issue);
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
    $init_file = drupal_get_path('module', 'tripal_chado') .
      '/chado_schema/initialize-' . $version . '.sql';
    $success = $this->applySQL($init_file, $chado_schema);
    if ($success) {
      // @upgrade tripal_report_error().
      $this->logger->info("Install of Chado v1.3 (Step 2 of 2) Successful.\nInstallation Complete\n");
    }
    else {
      // @upgrade tripal_report_error().
      $this->logger->info("Installation (Step 2 of 2) Problems!  Please check output for errors.");
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
    $module_path = drupal_get_path('module', 'tripal_chado');
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
      $this->logger->info("Install of Chado v1.3 (Step 1 of 2) Successful.\nInstallation Complete\n");
      return TRUE;
    }
  }
  
}
