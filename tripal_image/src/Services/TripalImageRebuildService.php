<?php

namespace Drupal\tripal_image\Services;

use Symfony\Component\Yaml\Yaml;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Service for handling tripal_image's rebuild logic.
 */
class TripalImageRebuildService {

  /**
   * The chado connection used to query chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * Text to add when creating a unique constraint.
   *
   * This is set depending on whether the Postgres version is 15 or
   * greater. NULL signifies the uninitialized state.
   *
   * @var bool|null
   */
  private ?bool $nulls_not_distinct_supported = NULL;

  /**
   * Constructs a new TripalImageRebuildService object.
   *
   * @param Drupal\tripal_chado\Database\ChadoConnection $chado_connection
   *   The chado connection used to query chado.
   */
  public function __construct(
    ChadoConnection $chado_connection,
  ) {
    $this->chado_connection = $chado_connection;
  }

  /**
   * Executes the rebuild process for tripal_image.
   *
   * This is used to implement hook_rebuild, which is run
   * on cache rebuild.
   */
  public function executeRebuild() {
    $this->createCustomChadoTables();
  }

  /**
   * Creates the custom chado tables used by this module.
   *
   * @param string|null $chado_schema_name
   *   Optional. The chado schema where the custom table will live. If no
   *   schema is specified then the default schema is used.
   *
   * @return void
   *   No return value.
   */
  public function createCustomChadoTables(?string $chado_schema_name = NULL): void {
    if (!$chado_schema_name) {
      $chado_schema_name = $this->chado_connection->getSchemaName();
    }

    $config = __DIR__ . '/../../config/other/tripal_image.custom_tables.yml';
    $table_schemas = Yaml::parseFile($config);
    foreach ($table_schemas as $table_name => $table_schema) {
      $args = ['format' => 'drupal', 'source' => 'database'];
      $existing_schema = NULL;
      if ($this->chado_connection->schema()->tableExists($table_name)) {
        $existing_schema = $this->chado_connection->schema()->getTableDef($table_name, $args);
      }
      if (!$existing_schema) {
        // If the table already exists, do nothing.
        $customTable = new ChadoCustomTable($table_name, $chado_schema_name);
        $force = FALSE;
        $customTable->setTableSchema($table_schema, $force);
        $customTable->setLocked(TRUE);
      }
    }
  }

  /**
   * Deletes the custom chado tables used by this module.
   *
   * This is intended to be called only when this module is uninstalled.
   *
   * @param string|null $chado_schema_name
   *   Optional. The chado schema where the custom tables live. If no
   *   schema is specified then the default schema is used.
   *
   * @return void
   *   No return value.
   */
  public function dropCustomChadoTables(?string $chado_schema_name = NULL): void {
    if (!$chado_schema_name) {
      $chado_schema_name = $this->chado_connection->getSchemaName();
    }
    $config = __DIR__ . '/../../config/other/tripal_image.custom_tables.yml';
    $table_schemas = Yaml::parseFile($config);
    // Drop tables in reverse order of how they were created.
    $table_schemas = array_reverse($table_schemas);
    foreach (array_keys($table_schemas) as $table_name) {
      $customTable = new ChadoCustomTable($table_name, $chado_schema_name);
      $existing_schema = $customTable->getTableSchema();
      if ($existing_schema) {
        $customTable->delete();
      }
    }
  }

}
