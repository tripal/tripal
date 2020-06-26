<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;

class chadoInstaller {

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
   * The DRUPAL-managed database connection.
   */
  private $connection;

  /**
   * The PHP-managed postgreSQL-specific connection.
   * NOTE: required to execute multiple-statement strings.
   */
  private $pgconnection;

  /**
   * The drupal logger for tripal_chado.
   */
  private $logger;

  /**
   * The number of chunk files per version we can install.
   */
  private $installNumChunks = [
    1.3 => 41,
  ];

  /**
   * Constructor: initialize connections.
   */
  public function __construct() {
    $this->connection = \Drupal::database();

    // Initialize the logger.
    $this->logger = \Drupal::logger('tripal_chado');

    // Get the default database and chado schema.
    $databases = $this->connection->getConnectionOptions();
    $dsn = sprintf( 'dbname=%s host=%s port=%s user=%s password=%s',
      $databases['database'],
      $databases['host'],
      $databases['port'],
      $databases['username'],
      $databases['password'] );

    // Open a PHP connection to the database
    // since Drupal restricts us to a single statement per exec.
    $pgconnection = pg_connect($dsn);
    if (!$pgconnection) {
      $this->logger->error(
        "Unable to connect to database using '$dsn' connection string.\n");

      pg_close($pgconnection);
      return FALSE;
    }
    $this->pgconnection = $pgconnection;
  }

  /**
   * Install chado in the specified schema.
   *
   * @param float $version
   *   The version of chado you would like to install.
   * @param string $chado_schema
   *   The name of the schema you would like to install chado in.
   */
  public function install($version, $chado_schema = 'chado') {
    $this->newVersion = $version;
    $this->schemaName = $chado_schema;
    $connection = $this->connection;

    // VALIDATION.
    // Check the version is valid.
    // @todo

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
    $success = $this->applySQL($init_file);
    if ($success) {
      // @upgrade tripal_report_error().
      $this->logger->notice(
        "Install of Chado v1.3 (Step 2 of 2) Successful.\nInstallation Complete\n");
    }
    else {
      // @upgrade tripal_report_error().
      $this->logger->error(
        "Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }

    // 5) Finally set the version.
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
   * Drops the specified schema.
   *
   * @param string $schema_name
   *   The name of the schema you would like to drop.
   * @return bool
   *   Whether or not dropping was successful.
   */
  private function dropSchema($schemaName) {

    // Check if the schema even exists.
    if (chado_dbschema_exists($schemaName)) {

      // Notify the admin and drop the schema.
      // @upgrade tripal_report_error().
      $this->logger->info(
        "Dropping existing Chado ('$schemaName') schema\n");
      $this->connection->query("drop schema $schemaName cascade");

      // Finally, check to see if it was successful.
      if (chado_dbschema_exists($schema_name)) {
        return FALSE;
      }
      else {
        return TRUE;
      }
    }
    // If it doesn't exist then we don't need to drop it!
    else {
      $this->logger->info(
        "Dropping existing Chado ('$schemaName') schema: already dropped.\n");
      return TRUE;
    }
  }

  /**
   * Create the specified schema.
   *
   * NOTE: Also creates the plpgsql language if it doesn't already exist.
   *
   * @param string $schema_name
   *   The name of the schema you would like to create.
   * @return bool
   *   Whether or not the creation was successful.
   */
  private function createSchema($schema_name) {

    // First notify the admin we are creating the schema.
    // @upgrade tripal_report_error().
    $this->logger->info(
      "Creating '$schema_name' schema\n");

    // Next, Create it.
    $this->connection->query("CREATE SCHEMA $schema_name");

    // Before creating the plpgsql language let's check to make sure
    // it doesn't already exists
    $sql = "SELECT COUNT(*) FROM pg_language WHERE lanname = 'plpgsql'";
    $results = $this->connection->query($sql);
    $count = $results->fetchObject();
    if (!$count or $count->count == 0) {
      $this->connection->query("create language plpgsql");
    }

    // Finally, check to see if it was successful.
    if (chado_dbschema_exists($schema_name)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Applies the table definition SQL files.
   *
   * @param float $version
   *   The version of the chado schema to install.
   * @return bool
   *   Whether the install was successful.
   */
  private function applyDefaultSchema($version) {
    $chado_schema = $this->schemaName;
    $numChunks = $this->installNumChunks[$version];

    //   Since the schema SQL file is large we have split it into
    //   multiple chunks. This loop will load each chunk...
    $failed = FALSE;
    $module_path = drupal_get_path('module', 'tripal_chado');
    $path = $module_path . '/chado_schema/parts-v' . $version . '/';
    for ($i = 1; $i <= $numChunks; $i++) {

      $file = $path . 'default_schema-' . $version . '.part' . $i . '.sql';
      $success = $this->applySQL($file);

      if ($success) {
        // @upgrade tripal_report_error().
        print "\tImport of part $i of $numChunks Successful!\n";
      }
      else {
        $failed = TRUE;
        // @upgrade tripal_report_error().
        $this->logger->error(
          "Schema installation part $i of $numChunks Failed...");
          break;
      }
    }

    // Set back to the default connection.
    $drupal_schema = chado_get_schema_name('drupal');
    $this->connection->query("SET search_path = $drupal_schema");

    // Finally report back to the admin how we did.
    if ($failed) {
      // @upgrade tripal_report_error().
      $this->logger->error(
        "Installation (Step 1 of 2) Problems!  Please check output above for errors.");
      return FALSE;
    }
    else {
      // @upgrade tripal_report_error().
      $this->logger->notice(
        "Install of Chado v1.3 (Step 1 of 2) Successful.\nInstallation Complete\n");
      return TRUE;
    }


  }

  /**
   * Applies all statements from an SQL file to the current database.
   *
   * @param string $file
   *   The full path and name of the file containing the SQL statements.
   * @return bool
   *   Whether the application succeeded.
   */
  private function applySQL($sql_file) {
    $chado_schema = $this->schemaName;
    $pgconnection = $this->pgconnection;

    // Retrieve the SQL file and change any search path commands.
    $sql = file_get_contents($sql_file);
    $sql = preg_replace(
      '/(SET\s*search_path\s*=.*)(chado)/',
      '$1' . $chado_schema,
      $sql
    );

    // Apply the SQL to the database.
    $result = pg_query($pgconnection, $sql);
    if (!$result) {
      $this->logger->error(
        "Unable to execute query block.\n");

      pg_close($pgconnection);
      return FALSE;
    }
    return TRUE;
  }
}
