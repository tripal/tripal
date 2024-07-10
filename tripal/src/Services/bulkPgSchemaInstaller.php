<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Database\Database;

class bulkPgSchemaInstaller {


  /**
   * The name of the schema we are interested in applying SQL to.
   */
  protected $schemaName;

  /**
   * The DRUPAL-managed database connection.
   */
  protected $connection;

  /**
   * The PHP-managed postgreSQL-specific connection.
   * NOTE: required to execute multiple-statement strings.
   */
  protected $pgconnection;

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

    // Get the default database.
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
   * Retrieves the PostgreSQL-specific connection to the database.
   *
   * @return object
   *   PostgreSQL connection resource on success, FALSE on failure.
   */
  public function getPgConnection() {
  return $this->pgconnection;
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
   * Drops the specified schema.
   *
   * @param string $schema_name
   *   The name of the schema you would like to drop.
   * @return bool
   *   Whether or not dropping was successful.
   */
  public function dropSchema($schema_name) {

    // Check if the schema even exists.
    if ($this->checkSchema($schema_name)) {

      // Notify the admin and drop the schema.
      // @upgrade tripal_report_error().
      $this->logger->info("Dropping existing Schema: '$schema_name'");
      $this->connection->query("drop schema $schema_name cascade");

      // Finally, check to see if it was successful.
      if ($this->checkSchema($schema_name)) {
        return FALSE;
      }
      else {
        return TRUE;
      }
    }
    // If it doesn't exist then we don't need to drop it!
    else {
      $this->logger->info("Dropping existing schema: '$schema_name' (already dropped).");
      return TRUE;
    }
  }

  /**
   * Create the specified schema.
   *
   * @param string $schema_name
   *   The name of the schema you would like to create.
   * @return bool
   *   Whether or not the creation was successful.
   */
  public function createSchema($schema_name) {

    // First notify the admin we are creating the schema.
    // @upgrade tripal_report_error().
    $this->logger->info("Creating '$schema_name' schema\n");

    // Next, Create it.
    $this->connection->query("CREATE SCHEMA $schema_name");

    // Finally, check to see if it was successful.
    if ($this->checkSchema($schema_name)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Check that the schema is present.
   *
   * @param string $schema_name
   *   The name of the schema you would like to check for existance of.
   * @return bool
   *   Whether or not the schema exists.
   */
  public function checkSchema($schema_name) {
  $sql = "
    SELECT true
    FROM pg_namespace
    WHERE has_schema_privilege(nspname, 'USAGE') AND nspname = :nspname
  ";
  $query = $this->connection->query($sql, [':nspname' => $schema_name]);
  $schema_exists = $query->fetchField();
  return $schema_exists;
  }

  /**
   * Applies all statements from an SQL file to the current database.
   *
   * @param string $file
   *   The full path and name of the file containing the SQL statements.
   * @return bool
   *   Whether the application succeeded.
   */
  public function applySQL($sql_file, $schema_name = FALSE, $append_search_path = FALSE) {
    $pgconnection = $this->pgconnection;

    // Retrieve the SQL file.
    $sql = file_get_contents($sql_file);

    // change any search path commands.
    if ($schema_name) {
      $sql = preg_replace(
          '/(SET\s*search_path\s*=.*)(chado)/',
          '$1' . $schema_name,
          $sql
      );

      // Append search path to the beginning.
      if ($append_search_path) {
        $sql = 'SET search_path = ' . $schema_name . ";\n" . $sql;
      }
    }

    // Apply the SQL to the database.
    $result = pg_query($pgconnection, $sql);
    if (!$result) {
      $this->logger->error("Unable to execute query block.\n");

      pg_close($pgconnection);
      return FALSE;
    }
    return TRUE;
  }
}
