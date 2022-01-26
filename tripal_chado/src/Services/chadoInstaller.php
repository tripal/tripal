<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\Services\bulkPgSchemaInstaller;

class chadoInstaller extends bulkPgSchemaInstaller {

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
   * The number of chunk files per version we can install.
   */
  protected $installNumChunks = [
    1.3 => 41,
  ];


  /**
   * Install chado in the specified schema.
   *
   * @param float $version
   *   The version of chado you would like to install.
   */
  public function install($version) {
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
    if (preg_match('/^[a-z][a-z0-9]+$/', $chado_schema) === 0) {
      // Schema name must be a single word containing only lower case letters
      // or numbers and cannot begin with a number.
      $this->logger->error("Schema name must be a single alphanumeric word beginning with a number and all lowercase.");
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
    $init_file = \Drupal::service('extension.list.module')->getPath('tripal_chado') .
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
    $module_path = \Drupal::service('extension.list.module')->getPath('tripal_chado');
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
