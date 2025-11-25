<?php

namespace Drupal\tripal_file\Services;

use Symfony\Component\Yaml\Yaml;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Service for handling tripal_file's rebuild logic.
 */
class TripalFileRebuildService {

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
   * Constructs a new TripalFileRebuildService object.
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
   * Executes the rebuild process for tripal_file.
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

    $config = __DIR__ . '/../../config/other/tripal_file.custom_tables.yml';
    $table_schemas = Yaml::parseFile($config);
    foreach ($table_schemas as $table_name => $table_schema) {
      $args = ['format' => 'drupal', 'source' => 'database'];
      $existing_schema = NULL;
      if ($this->chado_connection->schema()->tableExists($table_name)) {
        $existing_schema = $this->chado_connection->schema()->getTableDef($table_name, $args);
      }
      if ($existing_schema) {
        // If the table already exists, e.g. for a migrated Tripal 3
        // site, then only add missing columns, so that we preserve
        // existing table content.
        $this->migrateTable($table_name, $table_schema, $existing_schema, $chado_schema_name);
      }
      else {
        // Table does not exist, so create it.
        $customTable = new ChadoCustomTable($table_name, $chado_schema_name);
        $force = FALSE;
        $customTable->setTableSchema($table_schema, $force);
        $customTable->setLocked(TRUE);
      }
    }
  }

  /**
   * Updates a table schema of an existing table to match expected.
   *
   * This is to support migrating tables from Tripal 3 which do not have
   * the new type_id columns, and to update unique constraints to include
   * NULLS_NOT_DISTINCT.
   * Adding filename to fileloc table is only applicable to a migrated
   * Tripal 3 site that never ran Tripal File v3 update 7101 and 7102.
   *
   * @param string $table_name
   *   The name of the table being updated.
   * @param array $new_schema
   *   The Tripal 4 table schema.
   * @param array $existing_schema
   *   The Tripal 3 migrated table schema.
   * @param string|null $chado_schema_name
   *   Optional. The chado schema where the custom tables live. If no
   *   schema is specified then the default schema is used.
   *
   * @return void
   *   No return value.
   */
  public function migrateTable(string $table_name, array $new_schema, array $existing_schema, ?string $chado_schema_name = NULL): void {

    $check_columns = ['type_id', 'rank', 'filename'];
    foreach ($check_columns as $column) {
      // Adds the column if it is missing.
      if (array_key_exists($column, $new_schema['fields'])) {
        if (!array_key_exists($column, $existing_schema['fields'])) {

          $transaction_chado = $this->chado_connection->startTransaction();
          try {
            $full_table_name = $chado_schema_name . '.' . $table_name;

            // Add the new column to the table.
            $type = ' ' . $new_schema['fields'][$column]['type'];
            if (($new_schema['fields'][$column]['size'] ?? '') == 'big') {
              $type = ' bigint';
            }
            $constraint = '';
            if ($new_schema['fields'][$column]['not null'] ?? FALSE) {
              $constraint = ' NOT NULL';
            }
            $default = '';
            if (array_key_exists('default', $new_schema['fields'][$column])) {
              $default = ' DEFAULT ' . $new_schema['fields'][$column]['default'];
            }

            $sql = 'ALTER TABLE ' . $full_table_name . ' ADD COLUMN ' . $column . $type . $constraint . $default;
            $this->chado_connection->query($sql, []);

            // Add the foreign key only for the type_id column.
            if ($column == 'type_id') {
              $fkey_name = $table_name . '_' . $column . '_fkey';
              $this->addTypeIdForeignKey($new_schema, $full_table_name, $fkey_name);
            }
          }
          catch (\Exception $e) {
            $transaction_chado->rollback();
            throw new \Exception($e);
          }

          // Clear tripaldbx caches for this modified table.
          $this->clearTripalDbxCaches($table_name);
        }
      }
    }
  }

  /**
   * Adds foreign key for the type_id column.
   *
   * @param array $new_schema
   *   The schema we are updating to.
   * @param string $full_table_name
   *   Table name with chado prefix.
   * @param string $fkey_name
   *   The name of the foreign key to add.
   *
   * @return void
   *   No return value.
   */
  protected function addTypeIdForeignKey(array $new_schema, string $full_table_name, string $fkey_name): void {
    $sql = 'ALTER TABLE ' . $full_table_name
      . ' ADD CONSTRAINT ' . $fkey_name . ' FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id)';
    $this->chado_connection->query($sql, []);

    // Update the unique constraint to include type_id.
    $ukeys = $new_schema['unique keys'] ?? [];
    foreach ($ukeys as $uk_name => $uk_columns) {
      $nnd = '';
      if ($this->nullsNotDistinctSupported() && ($new_schema['nulls not distinct'] ?? FALSE)) {
        $nnd = ' NULLS NOT DISTINCT';
      }

      $sql = 'ALTER TABLE ' . $full_table_name . ' DROP CONSTRAINT IF EXISTS ' . $uk_name;
      $this->chado_connection->query($sql, []);
      $sql = 'ALTER TABLE ' . $full_table_name . ' ADD CONSTRAINT ' . $uk_name . ' UNIQUE'
        . $nnd . ' (' . implode(', ', $uk_columns) . ')';
      $this->chado_connection->query($sql, []);
    }
  }

  /**
   * Clears TripalDbx caches for a table.
   *
   * This is essential after modifying a table schema so that the
   * previous schema is not used by these functions.
   *
   * @param string $table_name
   *   The name of the chado table.
   *
   * @return void
   *   No return value.
   */
  protected function clearTripalDbxCaches(string $table_name): void {
    $this->chado_connection->schema()->getTableDdl($table_name, TRUE);
    $args = ['format' => 'none', 'clear' => TRUE];
    $this->chado_connection->schema()->getTableDef($table_name, $args);
  }

  /**
   * Gets nulls not distinct support status based on postgresql version.
   *
   * NULLS NOT DISTINCT is only supported for unique constraints
   * starting with Postgresql 15. Determine whether the currently
   * running version supports this.
   * Cache result in a class variable so that we only need to do this once.
   *
   * @return bool
   *   TRUE if supported, FALSE if not.
   */
  protected function nullsNotDistinctSupported(): bool {
    if (is_null($this->nulls_not_distinct_supported)) {
      $psql_version = $this->chado_connection->version();
      // Remove distro info, e.g.
      // "13.22 (Debian 13.22-1.pgdg12+1)" -> "13.22".
      $psql_version = preg_replace('/[^\d\.].*$/', '', $psql_version);
      if (version_compare($psql_version, '15.0') < 0) {
        // Version < 15 therefore not supported.
        $this->nulls_not_distinct_supported = FALSE;
      }
      else {
        $this->nulls_not_distinct_supported = TRUE;
      }
    }
    return $this->nulls_not_distinct_supported;
  }

  /**
   * This fixes migrated Tripal 3 EDAM terms with db of 'HTTP:'.
   *
   * The Tripal 3 obo importer incorrectly created and assigned a db
   * of 'HTTP:' to several EDAM typedef records. This function will
   * change the dbxref db_id from 'HTTP:' to 'EDAM'.
   * This bug was fixed for Tripal 4 in PR 2280.
   *
   * @return void
   *   No return value.
   *
   * @see https://github.com/tripal/tripal/pull/2280
   * @see https://github.com/tripal/tripal/issues/2344
   */
  public function applyPr2280Fix() {
    $query = $this->chado_connection->select('1:db', 'db')
      ->fields('db', ['db_id'])
      ->condition('db.name', 'HTTP:', '=');
    $http_db_id = $query->execute()->fetchField();
    if ($http_db_id) {
      $query = $this->chado_connection->select('1:db', 'db')
        ->fields('db', ['db_id'])
        ->condition('db.name', 'EDAM', '=');
      $edam_db_id = $query->execute()->fetchField();
      if ($edam_db_id) {
        $query = $this->chado_connection->update('1:dbxref')
          ->fields(['db_id' => $edam_db_id])
          ->condition('db_id', $http_db_id, '=')
          ->execute();
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
    $config = __DIR__ . '/../../config/other/tripal_file.custom_tables.yml';
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
