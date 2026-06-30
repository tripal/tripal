<?php

namespace Drupal\tripal_chado\Services;

use Drupal\tripal_chado\Database\ChadoConnection;
use Symfony\Component\Yaml\Yaml;

/**
 * Service for updating existing chado records from a yaml file.
 */
class ChadoPatchService {

  /**
   * The chado connection used to query chado.
   */
  public ChadoConnection $chado_connection;

  /**
   * Constructs a new ChadoPatchService object.
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
   * Updates chado records using a yaml file.
   *
   * @param string $yaml_file
   *   Specifies what changes to make to chado.
   * @param string|null $chado_schema
   *   The chado schema to update. If not specified, the current
   *   default chado schema is used.
   *
   * @return int|string
   *   If successful, the number of records modified, possibly zero.
   *   If not successful, an error message.
   */
  public function patchChado(string $yaml_file, ?string $chado_schema = NULL): int|string {

    // Load yaml file.
    if (!file_exists($yaml_file)) {
      return "Error, file \"$yaml_file\" does not exist";
    }
    try {
      $file_contents = file_get_contents($yaml_file);
      $yaml_data = Yaml::parse($file_contents);
    }
    catch (\Exception $e) {
      return "Error parsing yaml file \"$yaml_file\": " . $e->getMessage();
    }

    // Make the changes.
    $records_updated = 0;
    $transaction = $this->chado_connection->startTransaction();
    $existing_schema = $this->chado_connection->getSchemaName();
    try {
      if ($chado_schema) {
        $this->chado_connection->setSchemaName($chado_schema);
      }
      foreach ($yaml_data as $record) {
        $records_updated += $this->patchRecord($record);
      }
      if ($existing_schema) {
        $this->chado_connection->setSchemaName($existing_schema);
      }
    }
    catch (\Exception $e) {
      $transaction->rollback();
      $this->chado_connection->setSchemaName($existing_schema);
      return "Error updating chado: " . $e->getMessage();
    }

    return $records_updated;
  }

  /**
   * Updates a single record in chado.
   *
   * @param array $record
   *   Specification of the update to be made. Keys are
   *   - table: name of the chado table
   *   - conditions: one or more elements with:
   *     - column: which column to match on
   *     - value: value to match on
   *   - update_column: column to be updated
   *   - existing_value: optional value for the update_column. If the db
   *     table currently contains anything other than this, no change is made.
   *   - update_value: new value.
   *
   * @return int
   *   Count of number of records updated (0 or 1).
   *
   * @throws \Exception
   *   If more than one record matches, or any other database error.
   */
  protected function patchRecord(array $record): int {
    $status = 0;
    $upsert = $record['upsert'] ?? FALSE;
    $query = $this->chado_connection->select('1:' . $record['table'], 'T');
    foreach ($record['conditions'] as $condition) {
      $query->condition('T.' . $condition['column'], $condition['value'], '=');
    }
    $query->fields('T', [$record['update_column']]);
    $results = $query->execute()->fetchAll();

    // It is unsafe to continue if more than one record is returned.
    if (count($results) > 1) {
      throw new \Exception('Error: Multiple records returned for table "' . $record['table']
        . '" column "' . $record['key_column'] . '" value "' . $record['key_value'] . '".');
    }

    // If no match, and we didn't set upsert, then we just skip
    // this record.
    if (count($results) == 1 || $upsert) {
      // If an existing value was indicated, check for match.
      // If different, do nothing.
      if (isset($record['existing_value'])) {
        $current_value = $results[0]->{$record['update_column']};
        if ($current_value != $record['existing_value']) {
          return $status;
        }
      }
      // Update the record in the database. Status will be the number of
      // records modified.
      if (count($results)) {
        $query = $this->chado_connection->update('1:' . $record['table']);
        foreach ($record['conditions'] as $condition) {
          $query->condition($condition['column'], $condition['value'], '=');
        }
        $query->fields([$record['update_column'] => $record['update_value']]);
      }
      // Record does not exist, so insert it.
      else {
        $query = $this->chado_connection->insert('1:' . $record['table'], 't');
        // When inserting, the conditions become field values.
        $query->fields('t', $record['conditions']);
        $query->addField('t', $record['update_column'], $record['update_value']);
      }
      $status = $query->execute();
    }

    return $status;
  }

}
