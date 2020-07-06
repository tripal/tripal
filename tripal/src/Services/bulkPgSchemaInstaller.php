<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Database\Database;

class bulkPgSchemaInstaller {


  /**
   * The name of the schema we are interested in applying SQL to.
   */
  protected $schema_name;

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
   * Constructor: initialize connections.
   */
  public function __construct() {
    $this->connection = \Drupal::database();

    // Initialize the logger.
    $this->logger = \Drupal::logger('tripal');

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
   * Drops the specified schema.
   *
   * @param string $schema_name
   *   The name of the schema you would like to drop.
   * @return bool
   *   Whether or not dropping was successful.
   */
  protected function dropSchema($schema_name) {

    // Check if the schema even exists.
    if (chado_dbschema_exists($schema_name)) {

      // Notify the admin and drop the schema.
      // @upgrade tripal_report_error().
      $this->logger->info(
        "Dropping existing Schema: '$schema_name'\n");
      $this->connection->query("drop schema $schema_name cascade");

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
        "Dropping existing schema: '$schema_name' (already dropped).\n");
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
  protected function createSchema($schema_name) {

    // First notify the admin we are creating the schema.
    // @upgrade tripal_report_error().
    $this->logger->info(
      "Creating '$schema_name' schema\n");

    // Next, Create it.
    $this->connection->query("CREATE SCHEMA $schema_name");

    // Finally, check to see if it was successful.
    if (chado_dbschema_exists($schema_name)) {
      return TRUE;
    }
    else {
      return FALSE;
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
  protected function applySQL($sql_file) {
    $schema_name = $this->schemaName;
    $pgconnection = $this->pgconnection;

    // Retrieve the SQL file and change any search path commands.
    $sql = file_get_contents($sql_file);
    $sql = preg_replace(
      '/(SET\s*search_path\s*=.*)(chado)/',
      '$1' . $schema_name,
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
