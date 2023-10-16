<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\Validator\ConstraintViolation;

use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoFieldDebugger;

/**
 * Chado implementation of the TripalStorageInterface.
 *
 * @TripalStorage(
 *   id = "chado_storage",
 *   label = @Translation("Chado Storage"),
 *   description = @Translation("Interfaces with GMOD Chado for field values."),
 * )
 */
class ChadoStorage extends TripalStorageBase implements TripalStorageInterface {

  /**
   * An associative array that holds the data for mapping an
   * entityTypes to Chado tables.  It is indexed by entityType and the
   * value is the object containing the mapping information.
   *
   * @var array
   */
  protected $type_mapping = [];

  /**
   * An associative array that holds the data for mapping a
   * fieldType key to a Chado table column for a given entity.  It is indexed
   * by entityType -> entityID and the value is the object containing the
   * mapping information.
   *
   * @var array
   */
  protected $id_mapping = [];

  /**
   * The database connection for querying Chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $connection;

  /**
   * A service to provide debugging for fields to developers.
   *
   * @ var Drupal\tripal_chado\Services\ChadoFieldDebugger
   */
  protected $field_debugger;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal.logger'),
      $container->get('tripal_chado.database'),
      $container->get('tripal_chado.field_debugger')
    );
  }

  /**
   * Implements __contruct().
   *
   * Since we have implemented the ContainerFactoryPluginInterface, the constructor
   * will be passed additional parameters added by the create() function. This allows
   * our plugin to use dependency injection without our plugin manager service needing
   * to worry about it.
   *
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param Drupal\tripal\Services\TripalLogger $logger
   * @param Drupal\tripal_chado\Database\ChadoConnection $connection
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, TripalLogger $logger, ChadoConnection $connection, ChadoFieldDebugger $field_debugger) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $logger);

    $this->connection = $connection;
    $this->field_debugger = $field_debugger;
  }

  /**
   * @{inheritdoc}
   */
  public function addFieldDefinition(string $field_name, object $field_definition) {
    parent::addFieldDefinition($field_name, $field_definition);

    // Now check if the field debugger should be enabled for this particular field.
    $settings = $field_definition->getSettings();
    if (array_key_exists('debug', $settings) AND $settings['debug']) {
      $this->field_debugger->addFieldToDebugger($field_name);
      $this->logger->notice('Debugging has been enabled for :name field.',
        [':name' => $field_name],
        ['drupal_set_message' => TRUE, 'logger' => FALSE]
      );
    }
  }

  /**
   * Inserts a single record in a Chado table.
   * @param array $records
   * @param string $chado_table
   * @param integer $delta
   * @param array $record
   * @throws \Exception
   * @return integer
   */
  private function insertChadoRecord(&$records, $chado_table, $delta, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Insert the record.
    $insert = $this->connection->insert('1:' . $chado_table);
    $insert->fields($record['fields']);

    $this->field_debugger->reportQuery($insert, "Insert Query for $chado_table ($delta)");

    $record_id = $insert->execute();

    if (!$record_id) {
      throw new \Exception($this->t('Failed to insert a record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Update the record array to include the record id.
    $records[$chado_table][$delta]['conditions'][$pkey]['value'] = $record_id;
    return $record_id;
  }

  /**
	 * @{inheritdoc}
	 */
  public function insertValues(&$values) : bool {

    $schema = $this->connection->schema();

    $this->field_debugger->printHeader('Insert');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::insertValues');

    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];

    $transaction_chado = $this->connection->startTransaction();
    try {

      // First: Insert the base table records.
      foreach ($build['base_tables'] as $base_table => $record_id) {
        foreach ($records[$base_table] as $delta => $record) {
          $record_id = $this->insertChadoRecord($records, $base_table, $delta, $record);
          $build['base_tables'][$base_table] = $record_id;
        }
      }

      // Second: Insert non base table records.
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table, array_keys($build['base_tables']))) {
            continue;
          }

          // Don't insert any records if any of the columns have field that
          // are marked as "delete if empty".
          if (array_key_exists('delete_if_empty', $record)) {
            $skip_record = FALSE;
            foreach ($record['delete_if_empty'] as $del_key) {
              if ($record['fields'][$del_key] == '') {
                $skip_record = TRUE;
              }
            }
            if ($skip_record) {
              continue;
            }
          }

          // Replace linking fields with values
          foreach ($record['fields'] as $column => $val) {
            if (is_array($val) and $val[0] == 'REPLACE_BASE_RECORD_ID') {
              $base_table = $val[1];
              $records[$chado_table][$delta]['fields'][$column] = $build['base_tables'][$base_table];
              $record['fields'][$column] = $build['base_tables'][$base_table];
            }
          }
          $this->insertChadoRecord($records, $chado_table, $delta, $record);
        }
      }
      $this->setRecordIds($values, $records);
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw new \Exception($e);
    }

    // Now set the record Ids of the properties.
    return TRUE;
  }


  /**
   * Indicates if the record has any valid conditions.
   *
   * For the record to have valid conditions it must first have at least
   * one condition, and the value on which that condition relies is not empty.
   *
   * @param array $records
   * @return boolean
   */
  private function hasValidConditions($record) {
    $num_conditions = 0;
    foreach ($record['conditions'] as $chado_column => $cond_value) {
      if (!empty($cond_value['value'])) {
        $num_conditions++;
      }
    }
    if ($num_conditions == 0) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Indicates if we should keep this record for inserts/updates.
   *
   * @param array $record
   * @return boolean
   */
  private function isEmptyRecord($record) {
    if (array_key_exists('delete_if_empty', $record)) {
      foreach ($record['delete_if_empty'] as $del_key) {
        if ($record['fields'][$del_key] == '') { // @todo use the `empty_value` setting instead of hardcoding the ''
          return TRUE;
        }
      }
    }
    return FALSE;
  }



  /**
   * Updates a single record in a Chado table.
   *
   * @param array $base_tables
   * @param string $chado_table
   * @param integer $delta
   * @param array $record
   * @throws \Exception
   */
  private function updateChadoRecord(&$records, $chado_table, $delta, $record) {

    // Don't update if we don't have any conditions set.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
        ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    $update = $this->connection->update('1:'.$chado_table);
    $update->fields($record['fields']);
    foreach ($record['conditions'] as $chado_column => $cond_value) {
      $update->condition($chado_column, $cond_value['value']);
    }

    $this->field_debugger->reportQuery($update, "Update Query for $chado_table ($delta). Note: aguements may only include the conditional ones, see Drupal Issue #2005626.");

    $rows_affected = $update->execute();
    if ($rows_affected == 0) {
      throw new \Exception($this->t('Failed to update record in the Chado "@table" table. Record: @record',
        ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
    if ($rows_affected > 1) {
      throw new \Exception($this->t('Incorrectly tried to update multiple records in the Chado "@table" table. Record: @record',
        ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
  }


  /**
   * @{inheritdoc}
   */
  public function updateValues(&$values) : bool {

    $this->field_debugger->printHeader('Update');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::updateValues');

    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];

    $base_tables = $build['base_tables'];
    $transaction_chado = $this->connection->startTransaction();
    try {

      // Handle base table records first.
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          // If this is the base table then do an update.
          if (in_array($chado_table, array_keys($base_tables))) {
            if (!array_key_exists('conditions', $record)) {
              throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to missing conditions. Record: @record',
                  ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
            }
            $this->updateChadoRecord($records, $chado_table, $delta, $record);
            continue;
          }
        }
      }

      // Next delete all non base records so we can replace them
      // with updates. This is necessary because we may violate unique
      // constraints if we don't e.g. changing the order of records with a
      // rank.
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table, array_keys($base_tables))) {
            continue;
          }

          // Skip records that don't have a condition set. This means they
          // haven't been inserted before.
          if (!$this->hasValidConditions($record)) {
            continue;
          }
          $this->deleteChadoRecord($records, $chado_table, $delta, $record);
        }
      }

      // Now insert all new values for the non-base table records.
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table, array_keys($base_tables))) {
            continue;
          }
          // Skip records that were supposed to be deleted (and were).
          if ($this->isEmptyRecord($record)) {
            continue;
          }
          $this->insertChadoRecord($records, $chado_table, $delta, $record);
        }
      }
      $this->setRecordIds($values, $records);
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw new \Exception($e);
    }
    return TRUE;
  }

  /**
   * Selects a single record from Chado.
   *
   * @param array $records
   * @param string $chado_table
   * @param integer $delta
   * @param array $record
   *
   * @throws \Exception
   */
  public function selectChadoRecord(&$records, $base_tables, $chado_table, $delta, $record) {

    if (!array_key_exists('conditions', $record)) {
      throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // If we are selecting on the base table and we don't have a proper
    // condition then throw an error.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Select the fields in the chado table.
    $select = $this->connection->select('1:'.$chado_table, 'ct');
    $select->fields('ct', array_keys($record['fields']));

    // Add in any joins.
    if (array_key_exists('joins', $record)) {
      $j_index = 0;
      foreach ($record['joins'] as $rtable => $rjoins) {
        foreach ($rjoins as $jinfo) {
          $lalias = $jinfo['on']['left_alias'];
          $ralias = $jinfo['on']['right_alias'];
          $lcol = $jinfo['on']['left_col'];
          $rcol = $jinfo['on']['right_col'];

          $select->leftJoin('1:' . $rtable, $ralias, $lalias . '.' .  $lcol . '=' .  $ralias . '.' . $rcol);

          foreach ($jinfo['columns'] as $column) {
            $sel_col = $column[0];
            $sel_col_as = $column[1];
            $select->addField($ralias, $sel_col, $sel_col_as);
          }
          $j_index++;
        }
      }
    }

    // Add the select condition
    foreach ($record['conditions'] as $chado_column => $value) {
      if (!empty($value['value'])) {
        $select->condition('ct.'. $chado_column, $value['value'], $value['operation']);
      }
    }

    $this->field_debugger->reportQuery($select, "Select Query for $chado_table ($delta)");

    // Execute the query.
    $results = $select->execute();
    if (!$results) {
      throw new \Exception($this->t('Failed to select record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
    $records[$chado_table][$delta]['fields'] = $results->fetchAssoc();
  }

  /**
   * @{inheritdoc}
   */
  public function loadValues(&$values) : bool {

    $this->field_debugger->printHeader('Load');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::loadValues');

    $build = $this->buildChadoRecords($values, FALSE);
    $records = $build['records'];
    $base_tables = $build['base_tables'];

    $transaction_chado = $this->connection->startTransaction();
    try {
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {
          $this->selectChadoRecord($records, $base_tables, $chado_table, $delta, $record);
        }
      }
      $this->setPropValues($values, $records);
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw new \Exception($e);
    }

    $this->field_debugger->reportValues($values, 'The values after loading is complete.');

    return TRUE;
  }

  /**
   * Deletes a single record in a Chado table.
   *
   * @param array $base_tables
   * @param string $chado_table
   * @param integer $delta
   * @param array $record
   * @throws \Exception
   */
  private function deleteChadoRecord(&$records, $chado_table, $delta, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Don't delete if we don't have any conditions set.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    $delete = $this->connection->delete('1:'.$chado_table);
    foreach ($record['conditions'] as $chado_column => $cond_value) {
      $delete->condition($chado_column, $cond_value['value']);
    }

    $this->field_debugger->reportQuery($delete, "Delete Query for $chado_table ($delta)");

    $rows_affected = $delete->execute();
    if ($rows_affected == 0) {
      throw new \Exception($this->t('Failed to delete a record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
    if ($rows_affected > 1) {
      throw new \Exception($this->t('Incorrectly tried to delete multiple records in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Unset the record Id for this deleted record.
    $records[$chado_table][$delta]['conditions'][$pkey]['value'] = 0;
  }

  /**
   * @{inheritdoc}
   */
  public function deleteValues($values) : bool {

    $this->field_debugger->printHeader('Delete');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::deleteValues');

    return FALSE;
  }

  /**
   * @{inheritdoc}
   */
  public function findValues($match) {

    $this->field_debugger->printHeader('Find');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::findValues');

  }


  /**
   * Sets the Record ID Properties after an insert or update.
   *
   * Specifically, this sets the value of any properties with an action of
   * store_id, store_pkey and store_link. The value of the ID will be pulled
   * from the record conditions for that table.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   *
   * @param array $records
   *   The set of Chado records.
   */
  protected function setRecordIds(&$values, $records) {

    $schema = $this->connection->schema();

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {

      // Now we retrieve the field configuration.
      $definition = $this->getFieldDefinition($field_name);

      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {

          // Retrieve the property type for this value.
          $prop_type = $this->getPropertyType($field_name, $key);

          // Get the field and property storage settings.
          $field_settings = $definition->getSettings();
          $storage_plugin_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();

          // Skip fields that don't have an action. An error was already logged
          // for this in the buildChadoRecords function. Here we just skip.
          if (!array_key_exists('action', $prop_storage_settings)) {
            continue;
          }
          $action = $prop_storage_settings['action'];

          // Get the base table information.
          $base_table = $storage_plugin_settings['base_table'];
          $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
          $base_table_pkey = $base_table_def['primary key'];

          // Get the Chado table information. If one is not specified (as
          // in the case of single value fields) then default to the base
          // table.
          $chado_table = $base_table;
          $chado_table_def = $base_table_def;
          $chado_table_pkey = $base_table_pkey;
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
            $chado_table_pkey = $chado_table_def['primary key'];
          }

          // If this is the record_id property then set its value.
          if ($action == 'store_id') {
            $record_id = $records[$chado_table][0]['conditions'][$base_table_pkey]['value'];
            $values[$field_name][$delta][$key]['value']->setValue($record_id);
          }
          // If this is the linked record_id property then set its value.
          if ($action == 'store_pkey') {
            $record_id = $records[$chado_table][$delta]['conditions'][$chado_table_pkey]['value'];
            $values[$field_name][$delta][$key]['value']->setValue($record_id);
          }
          // If this is a property managing a linked record ID then set it too.
          if ($action == 'store_link') {
            $record_id = $records[$base_table][0]['conditions'][$base_table_pkey]['value'];
            $values[$field_name][$delta][$key]['value']->setValue($record_id);
          }
        }
      }
    }
  }

  /**
   * Sets the property values using the records returned from Chado.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param array $records
   *   The set of Chado records.
   */
  protected function setPropValues(&$values, $records) {

    $schema = $this->connection->schema();

    $replace = [];
    $function = [];

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {

      // Retrieve the field configuration.
      $definition = $this->getFieldDefinition($field_name);

      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {

          // Get the Property type for this value.
          $prop_type = $this->getPropertyType($field_name, $key);

          // Get important settings from the field configuration.
          $field_settings = $definition->getSettings();
          $storage_plugin_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();
          $action = $prop_storage_settings['action'];

          // Record IDs and linked IDs get set in the setRecordIDs() function.
          if ($action == 'store_id') {
            continue;
          }
          if ($action == 'store_pkey') {
            continue;
          }

          // If this is a linked record then the ID should already be in the
          // the conditions of the base table.
          if ($action == 'store_link') {
            $base_table = $storage_plugin_settings['base_table'];
            $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
            $base_table_pkey = $base_table_def['primary key'];
            $link_id = $records[$base_table][0]['conditions'][$base_table_pkey]['value'];
            $values[$field_name][$delta][$key]['value']->setValue($link_id);
          }

          // Get the values of properties that can be stored.
          if ($action == 'store') {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_column = $prop_storage_settings['chado_column'];

            if (array_key_exists($chado_table, $records)) {
              if (array_key_exists($delta, $records[$chado_table])) {
                if (array_key_exists($chado_column, $records[$chado_table][$delta]['fields'])) {
                  $value = $records[$chado_table][$delta]['fields'][$chado_column];
                  $values[$field_name][$delta][$key]['value']->setValue($value);
                }
              }
            }
          }

          // Get the values of properties that just want to read values.
          if (in_array($action, ['read_value', 'join'])) {
            if (array_key_exists('chado_table', $prop_storage_settings)) {
              $chado_table = $prop_storage_settings['chado_table'];
            }
            // Otherwise this is a join + we need the base table.
            // We can use the path to look this up.
            elseif (array_key_exists('path', $prop_storage_settings)) {
              // Examples of the path:
              //   - phylotree.analysis_id>analysis.analysis_id'.
              //   - feature.type_id>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id'
              $path_arr = explode(';', $prop_storage_settings['path']);
              // Now grab the left/right sides of the first part of the path.
              list($left, $right) = explode(">", array_shift($path_arr));
              // Break the left side into table + column.
              list($left_table, $left_col) = explode(".", $left);
              // The base table is the left table of the first part of the path.
              $chado_table = $left_table;
            }
            $chado_column = $prop_storage_settings['chado_column'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : $chado_column;
            $value = $records[$chado_table][$delta]['fields'][$as];
            $values[$field_name][$delta][$key]['value']->setValue($value);
          }

          if ($action == 'replace') {
            $replace[] = [$field_name, $delta, $key, $info];
          }

          if ($action == 'function') {
            $function[] = [$field_name, $delta, $key, $info];
          }
        }
      }
    }

    // Now that we have all stored and loaded values set, let's do any
    // replacements.
    foreach ($replace as $item) {
      $field_name = $item[0];
      $delta = $item[1];
      $key = $item[2];
      $info = $item[3];
      $prop_type = $this->getPropertyType($field_name, $key);
      $prop_storage_settings = $prop_type->getStorageSettings();
      $template = $prop_storage_settings['template'];

      $matches = [];
      $value = $template;
      if (preg_match_all('/\[(.*?)\]/', $template, $matches)) {
        foreach ($matches[1] as $match) {
          if (array_key_exists($match, $values[$field_name][$delta])) {
            $match_value = $values[$field_name][$delta][$match]['value']->getValue() ?? '';
            $value = preg_replace("/\[$match\]/", $match_value, $value);
          }
        }
      }
      if ($value !== NULL && is_string($value)) {
        $values[$field_name][$delta][$key]['value']->setValue(trim($value));
      }
      else {
        $values[$field_name][$delta][$key]['value']->setValue($value);
      }
    }

    // Lastly, let's call any functions.
    foreach ($function as $item) {
      $field_name = $item[0];
      $delta = $item[1];
      $key = $item[2];
      $info = $item[3];
      $prop_type = $this->getPropertyType($field_name, $key);
      $prop_storage_settings = $prop_type->getStorageSettings();
      $namespace = $prop_storage_settings['namespace'];
      $callback = $prop_storage_settings['function'];

      $value = call_user_func($namespace . '\\' . $callback);

      if ($value !== NULL && is_string($value)) {
        $values[$field_name][$delta][$key]['value']->setValue(trim($value));
      }
      else {
        $values[$field_name][$delta][$key]['value']->setValue($value);
      }
    }
  }

  /**
   * Indexes a values array for easy lookup.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. ncbitaxon__common_name).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id and value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *   When the function returns, any values retrieved from the data store
   *   will be set in the StoragePropertyValue object.
   * @param bool $is_store
   *   Set to TRUE if we are building the record array for an insert or an
   *   update.
   * @return array
   *   An associative array.
   */
  protected function buildChadoRecords($values, bool $is_store) {

    $schema = $this->connection->schema();
    $records = [];
    $base_record_ids = [];

    $this->field_debugger->reportValues($values, 'The values submitted to ChadoStorage');

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {

      // Retrieve the field configuration.
      $definition = $this->getFieldDefinition($field_name);
      if (!is_object($definition)) {
        $this->logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the field definition (i.e. FieldConfig object).',
          ['@field' => $field_name]));
        continue;
      }

      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {

          // Ensure we have a value to work with.
          if (!array_key_exists('value', $info) OR !is_object($info['value'])) {
            $this->logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the StoragePropertyValue object.',
              ['@field' => $field_name]));
            continue;
          }
          $prop_value = $info['value'];

          // Retrieve the property type for this value.
          $prop_type = $this->getPropertyType($field_name, $key);

          // Retrieve the operation to be used for searching and if not set, use equals as the default.
          $operation = array_key_exists('operation', $info) ? $info['operation'] : '=';

          // Retrieve important field settings.
          $field_label = $definition->getLabel();
          $field_settings = $definition->getSettings();
          $storage_plugin_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();

          // Make sure we have an action for this property.
          if (!array_key_exists('action', $prop_storage_settings)) {
            $this->logger->error($this->t('Cannot store the property, @field.@prop ("@label"), in Chado. The property is missing an action in the property settings: @settings',
                ['@field' => $field_name, '@prop' => $key,
                 '@label' => $field_label, '@settings' => print_r($prop_storage_settings, TRUE)]));
            continue;
          }
          $action = $prop_storage_settings['action'];

          // Check that the base table for the field is set.
          if (!array_key_exists('base_table', $storage_plugin_settings)) {
            $this->logger->error($this->t('Cannot store the property, @field.@prop, in Chado. The field is missing the chado base table name.',
                ['@field' => $field_name, '@prop' => $key]));
            continue;
          }

          // Get the base table definitions for the field.
          $base_table = $storage_plugin_settings['base_table'];
          $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
          $base_table_pkey = $base_table_def['primary key'];

          // Get the Chado table this specific property works with.
          // Use the base table as a default for properties which do not specify
          // the chado table (e.g. single value fields).
          $chado_table = $base_table;
          $chado_table_def = $base_table_def;
          $chado_table_pkey = $base_table_pkey;
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
            $chado_table_pkey = $chado_table_def['primary key'];
          }
          // Properties with a store_link action use left/right table notation.
          // The left table.left_table_id include the information for the value
          // to be set in this property so use them as equalivalent to the
          // chado table used in other actions. This also allows us to use
          // the base table of the field as a default for this.
          if (array_key_exists('left_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['left_table'];
            $chado_table_pkey = $prop_storage_settings['left_table_id'];
          }

          // Now for each action type, set the conditions and fields for
          // selecting chado records based on the other properties supplied.
          // ----------------------------------------------------------------
          // STORE ID: stores the primary key value for a core table in chado
          // Note: There may be more core tables in properties for this field
          // then just the base table. For example, a field involving a two-join
          // linker table will include two core tables.
          // ................................................................
          if ($action == 'store_id') {
            $record_id = $prop_value->getValue();
            // If the record_id is zero then this is a brand-new value for
            // this property. Let's set it to be replaced in the hopes that
            // some other property has already been inserted and has the ID.
            if ($record_id == 0) {
              $records[$chado_table][0]['conditions'][$chado_table_pkey] = ['value' => ['REPLACE_BASE_RECORD_ID', $base_table], 'operation' => $operation];
              // Now we add the chado table to our array of core tables
              // so that we can replace it with the value for the record later.
              if (!array_key_exists($chado_table, $base_record_ids)) {
                $base_record_ids[$chado_table] = $record_id;
              }
            }
            // However, if the record_id was set when the values were passed in,
            // then we want to set it here and add it to the array of core ids
            // for use later when replacing base record ids.
            else {
              $records[$chado_table][0]['conditions'][$chado_table_pkey] = ['value' => $record_id, 'operation' => $operation];
              $base_record_ids[$chado_table] = $record_id;
            }
          }
          // STORE PKEY: stores the primary key value of a linking table.
          // NOTE: A linking table is not a core table. This is important because
          // during insert and update, the core tables are handled first and then
          // linking tables are handled after.
          // ................................................................
          if ($action == 'store_pkey') {
            $link_record_id = $prop_value->getValue();
            $records[$chado_table][$delta]['conditions'][$chado_table_pkey] = ['value' => $link_record_id, 'operation' => $operation];
          }
          // STORE LINK: performs a join between two tables, one of which is a
          // core table and one of which is a linking table. The value which is saved
          // in this property is the left_table_id indicated in other key/value pairs.
          // ................................................................
          if ($action == 'store_link') {
            // The old implementation of store_link used chado_table/column notation
            // only for the right side of the relationship.
            // This meant we could not reliably determine the left side of the
            // relationship... Confirm this field uses the new method.
            if (array_key_exists('right_table', $prop_storage_settings)) {
              // Using the tables with a store_id, determine which side of this
              // relationship is a base/core table. This will be used for the
              // fields below to ensure the ID is replaced.
              // Start by assuming the left table is the base/core table
              // (e.g. feature.feature_id = featureprop.feature_id).
              $link_base = $chado_table;
              $link_base_id = $chado_table_pkey;
              $linker = $prop_storage_settings['right_table'];
              $linker_id = $prop_storage_settings['right_table_id'];
              // Then check if the right table has a store_id and if so, use it instead.
              // (e.g. analysisfeature.analysis_id = analysis.analysis_id)
              if (array_key_exists($prop_storage_settings['right_table'], $base_record_ids)) {
                $link_base = $prop_storage_settings['right_table'];
                $link_base_id = $prop_storage_settings['right_table_id'];
                $linker = $chado_table;
                $linker_id = $chado_table_pkey;
              }
              // @debug print "We decided it should be BASE $link_base.$link_base_id => LINKER $linker.$linker_id.\n";
              // We want to ensure that the linker table has a field added with
              // the link to replace the ID once it's available.
              $records[$linker] = $records[$linker] ?? [$delta => ['fields' => []]];
              $records[$linker][$delta] = $records[$linker][$delta] ?? ['fields' => []];
              $records[$linker][$delta]['fields'] = $records[$linker][$delta]['fields'] ?? [];
              if (!array_key_exists($linker_id, $records[$linker][$delta]['fields'])) {
                if ($prop_storage_settings['left_table'] !== NULL) {
                  $records[$linker][$delta]['fields'][$linker_id] = ['REPLACE_BASE_RECORD_ID', $link_base];
                  // @debug print "Adding a note to replace $linker.$linker_id with $link_base record_id\n";
                }
              }
            }
            else {
              // Otherwise this field is using the old method for store_link.
              // We will enter backwards compatibility mode here to do our best...
              // It will work to handle CRUD for direct connections (e.g. props)
              // but will cause problems with double-hops and all token replacement.
              $bc_chado_table = $prop_storage_settings['chado_table'];
              $bc_chado_column = $prop_storage_settings['chado_column'];
              $records[$bc_chado_table][$delta]['fields'][$bc_chado_column] = ['REPLACE_BASE_RECORD_ID', $base_table];
              $this->logger->warning(
                'We had to use backwards compatible mode for :name.:key property type with an action of store_link.'
                .' Please update the code for this field to use left/right table notation for the store_link property.'
                .' Backwards compatible mode should allow this field to save/load data but may result in errors with token replacement and publishing.',
                [':name' => $field_name, ':key' => $key]
              );
            }
          }
          // STORE: indicates that the value of this property can be loaded and
          // stored in the Chado table indicated by this property.
          // ................................................................
          if ($action == 'store') {
            $chado_column = $prop_storage_settings['chado_column'];
            $value = $prop_value->getValue();
            if (is_string($value)) {
              $value = trim($value);
            }
            $records[$chado_table][$delta]['fields'][$chado_column] = $value;

            // If this field should not allow an empty value that means this
            // entire record should be removed on an update and not inserted.
            $delete_if_empty = array_key_exists('delete_if_empty',$prop_storage_settings) ? $prop_storage_settings['delete_if_empty'] : FALSE;
            if ($delete_if_empty) {
              $records[$chado_table][$delta]['delete_if_empty'][] = $chado_column;
            }
          }
          // READ_VALUE: selecting a single column. This cannot be used for inserting or
          // updating values. Instead we use store actions for that.
          // If reading a value from a non-base table, then the path should
          // be provided. This also supports the deprecated 'join' action.
          // ................................................................
          if (in_array($action, ['read_value', 'join'])) {
            $chado_column = $prop_storage_settings['chado_column'];
            // If a join is needed to access the column, then the 'path' needs
            // to be defined and the joins need to be added to the query.
            // This will also add the fields to be selected.
            if (array_key_exists('path', $prop_storage_settings)) {
              $path = $prop_storage_settings['path'];
              $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : $chado_column;
              $path_arr = explode(";", $path);
              $this->addChadoRecordJoins($records, $chado_column, $as, $delta, $path_arr);
            }
            // Otherwise, it is a column in a base table. In this case, we
            // only need to ensure the column is added to the fields.
            else {
              // We will only set this if it's not already set.
              // This is to allow another field with a store set for this column
              // to set this value. We actually only do this to ensure it ends up
              // in the query fields.
              if (!array_key_exists('fields', $records[$chado_table][$delta])) {
                $records[$chado_table][$delta]['fields'] = [];
                $records[$chado_table][$delta]['fields'][$chado_column] = NULL;
              }
              elseif (!array_key_exists($chado_column, $records[$chado_table][$delta]['fields'])) {
                $records[$chado_table][$delta]['fields'][$chado_column] = NULL;
              }
            }
          }
          // REPLACE: replace a tokenized string with the values from other
          // properties. As such we do not need to worry about adding this
          // property to the chado queries.
          // ................................................................
          if ($action == 'replace') {
            // Do nothing here for properties that need replacement.
          }
          // FUNCTION: use a function to determine the value of this property.
          // As such, we do not need to add this property to the chado queries.
          // ................................................................
          if ($action == 'function') {
            // Do nothing here for properties that require post-processing
            // with a function.
          }
        }
      }
    }

    // Now we want to iterate through the records and set any record IDs
    // for FK relationships based off the values set in the propertyValues
    // before chado storage was called.
    // Note: We have not yet done any querying ;-p
    // -----------------------------------------------------------------------
    foreach ($records as $table_name => $deltas) {
      foreach ($deltas as $delta => $record) {
        // First for all the fields...
        foreach ($record['fields'] as $chado_column => $val) {
          if (is_array($val) and $val[0] == 'REPLACE_BASE_RECORD_ID') {
            $core_table = $val[1];

            // If the core table is set in the base record ids array and the
            // value is not 0 then we can set this chado field now!
            if (array_key_exists($core_table, $base_record_ids) and $base_record_ids[$core_table] != 0) {
              $records[$table_name][$delta]['fields'][$chado_column] = $base_record_ids[$core_table];
            }
            // If the base record ID is 0 then this is an insert and we
            // don't yet have the base record ID.  So, leave in the message
            // to replace the ID so we can do so later.
            if (array_key_exists($base_table, $base_record_ids) and $base_record_ids[$base_table] != 0) {
              $records[$table_name][$delta]['fields'][$chado_column] = $base_record_ids[$base_table];
            }

          }
        }
        if (!array_key_exists('conditions', $record)) print_r($record);
        foreach ($record['conditions'] as $chado_column => $val) {
          if (is_array($val['value']) and $val['value'][0] == 'REPLACE_BASE_RECORD_ID') {
            $core_table = $val['value'][1];

            // If the core table is set in the base record ids array and the
            // value is not 0 then we can set this condition now!
            if (array_key_exists($core_table, $base_record_ids) and $base_record_ids[$core_table] != 0) {
              $records[$table_name][$delta]['conditions'][$chado_column] = $base_record_ids[$core_table];
            }
            // If the base record ID is 0 then this is an insert and we
            // don't yet have the base record ID.  So, leave in the message
            // to replace the ID so we can do so later.
            if (array_key_exists($base_table, $base_record_ids) and $base_record_ids[$base_table] != 0) {
              $records[$table_name][$delta]['conditions'][$chado_column]['value'] = $base_record_ids[$base_table];
            }

          }
        }
      }
    }

    $this->field_debugger->summarizeBuiltRecords($base_record_ids, $records);

    return [
      'base_tables' => $base_record_ids,
      'records' => $records
    ];
  }

  /**
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is store_id.
   *
   * STORE ID: stores the primary key value for a core table in chado.
   *
   * Note: There may be more core tables in properties for this field
   * then just the base table. For example, a field involving a two-join
   * linker table will include two core tables.
   *
   * @param array $records
   *   The current set of chado records. This method will update this array.
   * @param array $storage_settings
   *   The storage settings for the current property. This is all the information
   *   from the property type.
   * @param array $context
   *   A set of values to provide context. These a pre-computed in the parent method
   *   to reduce code duplication when a task is done for all/many storage properties
   *   regardless of their action.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function buildChadoRecords_store_id(array &$records, array $storage_settings, array &$context, StoragePropertyValue $prop_value) {

    // Get the Chado table this specific property works with.
    // Use the base table as a default for properties which do not specify
    // the chado table (e.g. single value fields).
    $chado_table = $context['base_table'];
    if (array_key_exists('chado_table', $prop_storage_settings)) {
      $chado_table = $prop_storage_settings['chado_table'];
    }
    // Now determine the primary key for the chado table.
    $schema = $this->connection->schema();
    $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $chado_table_pkey = $chado_table_def['primary key'];

    // Get the value if it is set.
    $record_id = $prop_value->getValue();

    // If the record_id is zero then this is a brand-new value for
    // this property. Let's set it to be replaced in the hopes that
    // some other property has already been inserted and has the ID.
    if ($record_id == 0) {
      $records[$chado_table][0]['conditions'][$chado_table_pkey] = [
        'value' => [
          'REPLACE_BASE_RECORD_ID',
          $context['base_table']
        ],
        'operation' => $operation
      ];
      // Now we add the chado table to our array of core tables
      // so that we can replace it with the value for the record later.
      if (!array_key_exists($chado_table, $context['base_record_ids'])) {
        $context['base_record_ids'][$chado_table] = $record_id;
      }
    }
    // However, if the record_id was set when the values were passed in,
    // then we want to set it here and add it to the array of core ids
    // for use later when replacing base record ids.
    else {
      $records[$chado_table][0]['conditions'][$chado_table_pkey] = [
        'value' => $record_id,
        'operation' => $operation
      ];

      $context['base_record_ids'][$chado_table] = $record_id;
    }
  }

  /**
   *
   * @param array $records
   * @param string $base_table
   * @param int $delta
   * @param string $path
   */
  protected function addChadoRecordJoins(array &$records, string $chado_column, string $as,
      int $delta, array $path_arr, $parent_table = NULL, $parent_column = NULL, $depth = 0) {

    // Get the left column and the right table join infor.
    list($left, $right) = explode(">", array_shift($path_arr));
    list($left_table, $left_col) = explode(".", $left);
    list($right_table, $right_col) = explode(".", $right);

    // If the parent_table is not specified then it will be the left table.
    // The only time the parent table is not specified is when this function
    // is first called.
    $parent_table = !$parent_table ? $left_table : $parent_table;
    $parent_column = !$parent_column ? $left_col : $parent_column;


    // Make sure the parent table has a 'joins' array.
    if (!array_key_exists($parent_table, $records) or
        !array_key_exists($delta, $records[$parent_table]) or
        !array_key_exists('joins', $records[$parent_table][$delta])) {
      $records[$parent_table][$delta]['joins'] = [];
    }

    // A parent table may have more than one join to a right table so we
    // initialize the right table with an array.
    if (!array_key_exists($right_table, $records[$parent_table][$delta]['joins'])) {
      $records[$parent_table][$delta]['joins'][$right_table] = [];
    }
    if (!array_key_exists($parent_column, $records[$parent_table][$delta]['joins'][$right_table])) {
      $records[$parent_table][$delta]['joins'][$right_table][$parent_column] = [
        'on' => [],
        'columns' => []
      ];
    }

    // Get the current number of joins to the right table.
    $num_left = 0;
    if (array_key_exists($left_table,$records[$parent_table][$delta]['joins'])) {
      $num_left = count($records[$parent_table][$delta]['joins'][$left_table]) - 1;
    }
    $num_right = count($records[$parent_table][$delta]['joins'][$right_table]) - 1;

    // Generate aliases for the left and right tables in the join.
    $lalias = $depth == 0 ? 'ct' : 'j' . $left_table . $num_left;
    $ralias = 'j' . $right_table . $num_right;
    $schema = $this->connection->schema();

    // Add the join.
    $records[$parent_table][$delta]['joins'][$right_table][$parent_column]['on'] = [
      'left_table' => $left_table,
      'left_col' => $left_col,
      'right_table' => $right_table,
      'right_col' => $right_col,
      'left_alias' => $lalias,
      'right_alias' => $ralias,
    ];

    // We're done recursing if we only have no elements left in the path
    if (count($path_arr) == 0) {
      $records[$parent_table][$delta]['joins'][$right_table][$parent_column]['columns'][] = [$chado_column, $as];
      return;
    }

    // Add the right table back onto the path as the new left table and recurse.
    $depth++;
    $this->addChadoRecordJoins($records, $chado_column, $as, $delta, $path_arr, $parent_table, $parent_column, $depth);
  }


  /**
   * Checks that required fields have values.
   *
   * @param $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   * @param array $violations
   *   An array to which any new violations can be added.
   */
  private function validateRequired($values, $chado_table, $record_id, $record, &$violations) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    $missing = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Don't check the pkey
      if ($col == $pkey) {
        continue;
      }

      // If the field requires a value but doesn't have one then it may be
      // a problem.
      if ($info['not null'] == TRUE and !$col_val) {
        // If the column  has a default value then it's not a problem.
        if (array_key_exists('default', $info)) {
          continue;
        }
        $missing[] = $col;
      }
    }

    if (count($missing) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values are missing. ';
      $params = [];
      foreach ($missing as $col) {
        $message .=  ucfirst($col) . ", ";
      }
      $message = substr($message, 0, -2) . '.';
      $violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }

  /**
   * Checks the unique constraint of the table.
   *
   * @param $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   * @param array $violations
   *   An array to which any new violations can be added.
   */
  private function validateUnique($values, $chado_table, $record_id, $record, &$violations) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    // Check if we are violating a unique constraint (if it's an insert)
    if (array_key_exists('unique keys',  $table_def)) {
      $pkey = $table_def['primary key'];

      // Iterate through the unique constraints and see if the record
      // violates it.
      $ukeys = $table_def['unique keys'];
      foreach ($ukeys as $ukey_name => $ukey_cols) {
        $ukey_cols = explode(',', $ukey_cols);
        $query = $this->connection->select('1:'.$chado_table, 'ct');
        $query->fields('ct');
        foreach ($ukey_cols as $col) {
          $col = trim($col);
          $col_val = NULL;
          if (array_key_exists($col, $record['fields'])) {
            $col_val = $record['fields'][$col];
          }
          // If there is not a NOT NULL constraint on this column,
          // and it is of a string type, then we need to handle
          // empty values specially, since they might be stored
          // as either NULL or as an empty string in the database
          // table. Create a condition that checks for both. For
          // other types, e.g. integer, just check for null.
          if ($table_def['fields'][$col]['not null'] == FALSE and !$col_val) {
            if (in_array($table_def['fields'][$col]['type'],
                ['character', 'character varying', 'text'])) {
              $query->condition($query->orConditionGroup()
                ->condition($col, '', '=')
                ->isNull($col));
            }
            else {
              $query->isNull($col);
            }
          }
          else {
            $query->condition($col, $col_val);
          }
        }

        // If we have matching record, check for a unique constraint
        // violation.
        $match = $query->execute()->fetchObject();
        if ($match) {

          // Add a constraint violation if we have a match and the
          // record_id is 0. This would be an insert but a record already
          // exists. Or, if the record_id isn't the same as the  matched
          // record. This is an update that conflicts with an existing
          // record.
          if (($record_id == 0) or ($record_id != $match->$pkey)) {
            // Documentation for how to create a violation is here
            // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
            $message = 'The item cannot be saved as another already exists with the following values. ';
            $params = [];
            foreach ($ukey_cols as $col) {
              $col = trim($col);
              $col_val = NULL;
              if (array_key_exists($col, $record['fields'])) {
                $col_val = $record['fields'][$col];
              }
              if ($table_def['fields'][$col]['not null'] == FALSE and !$col_val) {
                continue;
              }
              $message .=  ucfirst($col) . ": '@$col'. ";
              $params["@$col"] = $col_val;
            }
            $violations[] = new ConstraintViolation(t($message, $params)->render(),
                $message, $params, '', NULL, '', 1, 0, NULL, '');
          }
        }
      }
    }
  }

  /**
   * Checks that foreign key values exist.
   *
   * @param $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   * @param array $violations
   *   An array to which any new violations can be added.
   */
  private function validateFKs($values, $chado_table, $record_id, $record, &$violations) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_fks = [];
    if (!array_key_exists('foreign keys', $table_def)) {
      return;
    }
    $fkeys = $table_def['foreign keys'];
    foreach ($fkeys as $fk_table => $info) {
      foreach ($info['columns'] as $lcol => $rcol) {

        // If the FK is not set in the record then skip it.
        if (!array_key_exists($lcol, $record['fields'])) {
          continue;
        }

        // If an FK allows nulls and the value is null then skip this one.
        $col_val = $record['fields'][$lcol];
        if ($table_def['fields'][$lcol]['not null'] == FALSE and $col_val === NULL) {
          continue;
        }

        // Check if the id is present in the FK table.
        $query = $this->connection->select($fk_table, 'fk');
        $query->fields('fk', [$rcol]);
        $query->condition($rcol, $col_val);
        $fk_id = $query->execute()->fetchField();
        if (!$fk_id) {
          $bad_fks[] = $lcol;
        }
      }
    }

    if (count($bad_fks) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values have a missing linked record in the data store: ';
      $params = [];
      foreach ($bad_fks as $col) {
        $message .=  ucfirst($col) . ", ";
      }
      $message = substr($message, 0, -1) . '.';
      $violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }

  /**
   * Checks that foreign key values exist.
   *
   * @param $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   * @param array $violations
   *   An array to which any new violations can be added.
   */
  public function validateTypes($values, $chado_table, $record_id, $record, &$violations) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_types = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Skip fields without values. If they are required
      // but missing then the validateRequired() function will check those.
      if (!$col_val) {
        continue;
      }

      if ($info['type'] == 'integer' or
          $info['type'] == 'bigint' or
          $info['type'] == 'smallint' or
          $info['type'] == 'serial') {
        if (!preg_match('/^\d+$/', $col_val)) {
          $bad_types[$col] = 'Integer';
        }
      }
      else if ($info['type'] == 'boolean') {
        if (!is_bool($col_val) and !preg_match('/^[01]$/', $col_val)) {
          $bad_types[$col] = 'Boolean';
        }
      }
      else if ($info['type'] == 'timestamp without time zone' or
               $info['type'] == 'date') {
        if (!is_integer($col_val)) {
          $bad_types[$col] = 'Timestamp';
        }
      }
      else if ($info['type'] == 'character varying' or
               $info['type'] == 'character' or
               $info['type'] == 'text') {
       // Do nothing.
      }
      else if ($info['type'] == 'double precision' or
               $info['type'] == 'real') {
         if (!is_numeric($col_val)) {
           $bad_types[$col] = 'Number';
         }
      }
    }

    if (count($bad_types) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values are of the wrong type: ';
      $params = [];
      foreach ($bad_types as $col => $col_type) {
        $message .=  ucfirst($col) . " should be $col_type. " ;
      }
      $violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }

  /**
   * Checks that size of the value isn't too large
   *
   * @param $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   * @param array $violations
   *   An array to which any new violations can be added.
   */
  public function validateSize($values, $chado_table, $record_id, $record, &$violations) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_sizes = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Skip fields without values. If they are required
      // but missing then the validateRequired() function will check those.
      if (!$col_val) {
        continue;
      }

      // If the column has a size then check it.
      if (array_key_exists('size', $info)) {

        // If this is a string type column.
        if ($info['type'] == 'character varying' or
            $info['type'] == 'character' or
            $info['type'] == 'text') {
          if (strlen($col_val) > $info['size']) {
            $bad_sizes[$col] = $info['size'];
          }
        }
      }
    }

    if (count($bad_sizes) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values are too large. ';
      $params = [];
      foreach ($bad_sizes as $col => $size) {
        $message .=  ucfirst($col) . " should be less than $size characters long. " ;
      }
      $violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }

  /**
   *
   * {@inheritDoc}
   */
  public function validateValues($values) {

    $this->field_debugger->printHeader('Validate');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::validateValues');

    $build = $this->buildChadoRecords($values, TRUE);
    $base_tables = $build['base_tables'];
    $records = $build['records'];
    $violations = [];

    // We only need to validate the base table properties because
    // the linker table values get completely replaced on an update and
    // should not exist for an insert.
    foreach ($build['base_tables'] as $base_table => $record_id) {
      foreach ($records[$base_table] as $delta => $record) {
        $record = $records[$base_table][$delta];
        $this->validateRequired($values, $base_table, $record_id, $record, $violations);
        $this->validateTypes($values, $base_table, $record_id, $record, $violations);
        $this->validateSize($values, $base_table, $record_id, $record, $violations);
        // Don't do the SQL checks if there are previous problems.
        if (count($violations) == 0) {
          $this->validateUnique($values, $base_table, $record_id, $record, $violations);
          $this->validateFKs($values, $base_table, $record_id, $record, $violations);
        }
      }
    }

    return $violations;
  }
}
