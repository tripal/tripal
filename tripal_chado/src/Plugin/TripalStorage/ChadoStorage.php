<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\Validator\ConstraintViolation;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoFieldDebugger;
use Drupal\tripal\TripalStorage\StoragePropertyValue;

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
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $connection;

  /**
   * A service to provide debugging for fields to developers.
   *
   * @var \Drupal\tripal_chado\Services\ChadoFieldDebugger
   */
  protected $field_debugger;

  /**
   * A collection of the primary key values for various base tables.
   *
   * This is populated as records are loaded/inserted and then used to later
   * update foreign key values for dependant tables.
   *
   * @var array
   *   key is the table alias and value is the primary key value for the
   *   record with that table alias. If the ID is not yet known then the
   *   value is NULL.
   */
  protected $base_record_ids = [];

  /**
   * A mapping of table alias to chado table name.
   *
   * In most cases the table alias will be the same as the table name. However,
   * especially in cases of chado prop and linking tables, an alias ensures that
   * multiple fields can control a subset of records without interferance or
   * data-swapping between fields.
   *
   * @var array
   *   key is the table alias and value is the official chado table name that
   *   the alias refers to.
   */
  protected $table_alias_mapping = [];

  /**
   * A mapping of the chado table to it's alias. This uses the field and property
   * to ensure the alias is looked up properly.
   */
  protected $reverse_alias_mapping = [];

  /**
   * A mapping of the field property to the alias used in the query.
   * This is specific to properties with an action of read_value who
   * specify a path.
   *
   * @var array
   *  a nested array where mapping the new alias including the right table alias
   *  to the original alias set in the property type. The structure is:
   *   [field name]:
   *     [property key]:
   *       [original column alias]: [new alias]
   */
  protected $join_column_alias = [];

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
   * @param \Drupal\tripal\Services\TripalLogger $logger
   * @param \Drupal\tripal_chado\Database\ChadoConnection $connection
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
   *
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::getStoredTypes()
   */
  public function getStoredTypes() {
    $ret_types = [];
    foreach ($this->property_types as $field_name => $keys) {
      $field_definition = $this->field_definitions[$field_name];
      foreach ($keys as $key => $prop_type) {
        $storage_settings = $prop_type->getStorageSettings();

        // We always need to retreive any field that store a base record id
        // a primery key or a foreign key link.
        if (($storage_settings['action'] == 'store_id') or
            ($storage_settings['action'] == 'store_pkey') or
            ($storage_settings['action'] == 'store_link')) {
          $ret_types[$field_name][$key] = $prop_type;
        }
        // For any other fields that have a 'drupal_store' set we need
        // those too.
        elseif ((array_key_exists('drupal_store', $storage_settings)) and
                ($storage_settings['drupal_store'] === TRUE)) {
          $ret_types[$field_name][$key] = $prop_type;
        }
      }
    }
    return $ret_types;
  }

  /**
   * Inserts a single record in a Chado table.
   *
   * @param array $records
   * @param string $chado_table_alias
   * @param integer $delta
   * @param array $record
   * @throws \Exception
   * @return integer
   */
  private function insertChadoRecord(&$records, $chado_table_alias, $delta, $record) {

    $chado_table = $this->getChadoTableFromAlias($chado_table_alias);

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Insert the record.
    $insert = $this->connection->insert('1:' . $chado_table);
    $insert->fields($record['fields']);

    $this->field_debugger->reportQuery($insert, "Insert Query for $chado_table ($delta)");

    $record_id = $insert->execute();

    if (!$record_id) {
      throw new \Exception($this->t('Failed to insert a record in the Chado "@table" table. Alias: @alias, Record: @record',
          ['@alias' => $chado_table_alias, '@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Update the record array to include the record id.
    $records[$chado_table_alias][$delta]['conditions'][$pkey]['value'] = $record_id;
    return $record_id;
  }

  /**
	 * @{inheritdoc}
	 */
  public function insertValues(&$values) : bool {

    $schema = $this->connection->schema();

    $this->field_debugger->printHeader('Insert');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::insertValues');

    $build = $this->buildChadoRecords($values);
    $records = $build['records'];

    $transaction_chado = $this->connection->startTransaction();
    try {

      // First: Insert the base table records.
      // Note: Assumes there is only a single base table.
      foreach ($this->base_record_ids as $base_table_alias => $record_id) {
        foreach ($records[$base_table_alias] as $delta => $record) {
          $record_id = $this->insertChadoRecord($records, $base_table_alias, $delta, $record);
          $this->base_record_ids[$base_table_alias] = $record_id;
        }
      }

      // Second: Insert non base table records.
      foreach ($records as $chado_table_alias => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table_alias, array_keys($this->base_record_ids))) {
            continue;
          }

          // Don't insert any records if any of the columns have field that
          // are marked as "delete if empty".
          if (array_key_exists('delete_if_empty', $record)) {
            $skip_record = FALSE;
            foreach ($record['delete_if_empty'] as $del_record) {
              if ($record['fields'][$del_record['chado_column']] == $del_record['empty_value']) {
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
              $base_table_alias = $val[1];
              $records[$chado_table_alias][$delta]['fields'][$column] = $this->base_record_ids[$base_table_alias];
              $record['fields'][$column] = $this->base_record_ids[$base_table_alias];
            }
          }
          $this->insertChadoRecord($records, $chado_table_alias, $delta, $record);
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
   *
   * @return boolean
   */
  private function isEmptyRecord($record) {
    if (array_key_exists('delete_if_empty', $record)) {
      foreach ($record['delete_if_empty'] as $del_record) {
        if ($record['fields'][$del_record['chado_column']] == $del_record['empty_value']) {
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

    $this->field_debugger->reportQuery($update, "Update Query for $chado_table ($delta). Note: arguments may only include the conditional ones, see Drupal Issue #2005626.");

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

    $build = $this->buildChadoRecords($values);
    $records = $build['records'];


    $base_tables = $this->base_record_ids;
    $transaction_chado = $this->connection->startTransaction();
    try {

      // Handle base table records first.
      foreach ($records as $chado_table_alias => $deltas) {
        foreach ($deltas as $delta => $record) {

          // If this is the base table then do an update.
          if (in_array($chado_table_alias, array_keys($base_tables))) {
            if (!array_key_exists('conditions', $record)) {
              throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to missing conditions. Record: @record',
                  ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
            }
            $this->updateChadoRecord($records, $chado_table_alias, $delta, $record);
            continue;
          }
        }
      }

      // Next delete all non base records so we can replace them
      // with updates. This is necessary because we may violate unique
      // constraints if we don't e.g. changing the order of records with a
      // rank.
      foreach ($records as $chado_table_alias => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table_alias, array_keys($base_tables))) {
            continue;
          }

          // Skip records that don't have a condition set. This means they
          // haven't been inserted before.
          if (!$this->hasValidConditions($record)) {
            continue;
          }
          $this->deleteChadoRecord($records, $chado_table_alias, $delta, $record);
        }
      }

      // Now insert all new values for the non-base table records.
      foreach ($records as $chado_table_alias => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table_alias, array_keys($base_tables))) {
            continue;
          }
          // Skip records that were supposed to be deleted (and were).
          if ($this->isEmptyRecord($record)) {
            continue;
          }

          $this->insertChadoRecord($records, $chado_table_alias, $delta, $record);
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
   * Queries for multiple records in Chado.
   *
   * @param array $records
   * @param string $chado_table
   * @param integer $delta
   * @param array $record
   *
   * @throws \Exception
   */
  public function findChadoRecords($chado_table_alias, $delta, $record) {

    $chado_table = $this->getChadoTableFromAlias($chado_table_alias);

    // Select the fields in the chado table.
    $select = $this->connection->select('1:' . $chado_table, 'ct');
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
            $sel_col_as = $ralias . '_' . $column[1];
            $field_name = $column[2];
            $property_key = $column[3];
            $this->join_column_alias[$field_name][$property_key][$column[1]] = $sel_col_as;
            $select->addField($ralias, $sel_col, $sel_col_as);
          }
          $j_index++;
        }
      }
    }

    // Add the select condition
    foreach ($record['conditions'] as $chado_column => $value) {
      // If we don't have a primary key for the base table then skip the condition.
      if (is_array($value['value']) and in_array('REPLACE_BASE_RECORD_ID', array_values($value['value']))) {
        continue;
      }
      if (!empty($value)) {
        $select->condition('ct.'.$chado_column, $value['value'], $value['operation']);
      }
    }

    $this->field_debugger->reportQuery($select, "Select Query for $chado_table ($delta)");
    // @debug print "Query in findChadoRecord(): " . strtr((string) $select, $select->arguments());

    // Execute the query.
    $results = $select->execute();
    if (!$results) {
      throw new \Exception($this->t('Failed to select record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
    return $results;
  }

  /**
   * Selects a single record from Chado.
   *
   * @param array $records
   * @param string $chado_table_alias
   * @param integer $delta
   * @param array $record
   *
   * @throws \Exception
   */
  public function selectChadoRecord(&$records, $base_tables, $chado_table_alias, $delta, $record) {

    if (!array_key_exists('conditions', $record)) {
      throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
          ['@table' => $chado_table_alias, '@record' => print_r($record, TRUE)]));
    }

    // If we are selecting on the base table and we don't have a proper
    // condition then throw an error.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
          ['@table' => $chado_table_alias, '@record' => print_r($record, TRUE)]));
    }

    $chado_table = $this->getChadoTableFromAlias($chado_table_alias);

    // Select the fields in the chado table.
    $select = $this->connection->select('1:'.$chado_table, 'ct');
    if (array_key_exists('fields', $record)) {
      $select->fields('ct', array_keys($record['fields']));
    }

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
            $sel_col_as = $ralias . '_' . $column[1];
            $field_name = $column[2];
            $property_key = $column[3];
            $this->join_column_alias[$field_name][$property_key][ $column[1] ] = $sel_col_as;
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
    $records[$chado_table_alias][$delta]['fields'] = $results->fetchAssoc();
  }

  /**
   * @{inheritdoc}
   */
  public function loadValues(&$values) : bool {

    $this->field_debugger->printHeader('Load');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::loadValues');

    $build = $this->buildChadoRecords($values);
    $records = $build['records'];
    $base_tables = $this->base_record_ids;

    $transaction_chado = $this->connection->startTransaction();
    try {
      foreach ($records as $chado_table_alias => $deltas) {
        foreach ($deltas as $delta => $record) {
          $this->selectChadoRecord($records, $base_tables, $chado_table_alias, $delta, $record);
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
  private function deleteChadoRecord(&$records, $chado_table_alias, $delta, $record) {

    $chado_table = $this->getChadoTableFromAlias($chado_table_alias);

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
      // @debug print "\n" . strtr((string) $delete, $delete->arguments()) . "\n";
      throw new \Exception($this->t('Failed to delete a record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
    if ($rows_affected > 1) {
      throw new \Exception($this->t('Incorrectly tried to delete multiple records in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Unset the record Id for this deleted record.
    $records[$chado_table_alias][$delta]['conditions'][$pkey]['value'] = 0;
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
  public function findValues($values) {

    $this->field_debugger->printHeader('Find');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::findValues');

    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];
    print_r($records);
    $base_record_ids = $this->base_record_ids;

    // Start an array to keep track of the results we find.
    // Each element in this array will be a clone of the full $values array
    // passed into this method with it's propertyValue objects set to match
    // the values in chado for a single record.
    $found_list = [];

    // Find all of property values for the base record. We need to
    // search for this first because the properties that have values
    // from linked tables need to know the record_id.
    $transaction_chado = $this->connection->startTransaction();
    try {
      foreach (array_keys($base_record_ids) as $base_table) {
        foreach ($records[$base_table] as $delta => $record) {

          // First we use findChadoRecords() to query chado for all records
          // in the table specified as $base_table. Each match returned here
          // is a query result.
          // @debug print "\t$base_table Record: " . print_r($record, TRUE);
          $matches = $this->findChadoRecords($base_table, $delta, $record);
          // Now for each of these query results...
          while ($match = $matches->fetchAssoc()) {
            // @debug print "\t\tWorking on Query Record: " . print_r($match, TRUE);
            print_r($match);

            // We start by cloning the records array
            // (includes all tables, not just the current $base_table)
            $new_records = $records;
            // and then replace the fields with the match we found.
            $new_records[$base_table][0]['fields'] = $match;
            // We also clone the $values array passed into findValues()
            // including all fields and their propertyValue objects.
            $base_values = $this->cloneValues($values);
            // Now we set the PropertyValue values using the query result.
            $this->setPropValues($base_values, $new_records);
            // Then we set the values of any propertyValue objects
            // with an action of stor_id, store_pkey or store_link.
            $this->setRecordIds($base_values, $new_records, TRUE);
            // Finally, we can add this result to our found list.
            $found_list[] = $base_values;
          }
        }
      }

      // Now that we have the record IDs for the properties in
      // the base table we can query for the properties in linked
      // tables.
      foreach ($found_list as $i => $base_values) {
        $build = $this->buildChadoRecords($base_values, TRUE);
        $base_records = $build['records'];
        foreach ($base_records as $chado_table_alias => $deltas) {

          // Skip base tables as we've already done those.
          if (in_array($chado_table_alias, array_keys($base_record_ids))) {
            continue;
          }

          // Now query for non base records, and add each match to the
          // records array.
          $j = 0;
          foreach ($deltas as $delta => $record) {
            $matches = $this->findChadoRecords($chado_table_alias, $delta, $record);
            while ($match = $matches->fetchAssoc()) {
              $base_records[$chado_table_alias][$j]['fields'] = $match;
              $j++;
            }
          }
          if ($j > 0) {
            $this->setPropValues($found_list[$i], $base_records);
            $this->setRecordIds($found_list[$i], $base_records, TRUE);
          }
        }
      }
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw new \Exception($e);
    }

    exit;
    return $found_list;
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
   * @param array $records
   *   The set of Chado records.
   * @param boolean $is_find
   *  Set to TRUE if the values array was created using the findValues() function.
   *  Record IDs are stored differently for finds.
   */
  protected function setRecordIds(&$values, $records, $is_find = FALSE) {

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

          // Quickly skip any property whose action is not focused on keys.
          if (!in_array($action, ['store_id', 'store_pkey', 'store_link'])) {
            continue;
          }

          // Get the base table information and use it as the default for if
          // a chado_table is not specified (as in the case of single value fields).
          $chado_table = $storage_plugin_settings['base_table'];
          // Now check for if the chado_table is specified as it should be for
          // store_id + store_pkey properties.
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
          }
          // If it is a store_link property then we have to deal with left/right
          // tables so let's do that now.
          // Note: If the action is store_link and the right_table was not set then we
          // want to be backwards compatible with the old store_link approach.
          elseif (array_key_exists('right_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['right_table'];
            if (array_key_exists($prop_storage_settings['right_table'], $this->base_record_ids)) {
              $chado_table = $prop_storage_settings['left_table'];
            }
          }

          // Grab the pkey using the schema definition.
          $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
          $chado_table_pkey = $chado_table_def['primary key'];

          // Now grab the table alias.
          $chado_table_alias = $this->getTableAliasForChadoTable($field_name, $key, $chado_table);

          // We want to do different things when finding records...
          if ($is_find) {
            // For finding we only need to worry about the store_id action.
            if ($action == 'store_id') {
              $record_id = $records[$chado_table_alias][0]['fields'][$chado_table_pkey];
              $values[$field_name][$delta][$key]['value']->setValue($record_id);
              $this->base_record_ids[$chado_table_alias] = $record_id;
            }
            // We are specifically not doing anything for store_pkey and store_link.
          }
          // Otherwise carry on as we used to.
          else {
            // If this is the record_id property then set its value.
            if ($action == 'store_id') {
              $record_id = $records[$chado_table_alias][0]['conditions'][$chado_table_pkey]['value'];
              $values[$field_name][$delta][$key]['value']->setValue($record_id);
            }
            // If this is the linked record_id property then set its value.
            if ($action == 'store_pkey') {
              $record_id = $records[$chado_table_alias][$delta]['conditions'][$chado_table_pkey]['value'];
              $values[$field_name][$delta][$key]['value']->setValue($record_id);
            }
            // If this is a property managing a linked record ID then set it too.
            if ($action == 'store_link') {
              $record_id = $records[$chado_table_alias][0]['conditions'][$chado_table_pkey]['value'];
              $values[$field_name][$delta][$key]['value']->setValue($record_id);
            }
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
          if ($action == 'store_link') {
            continue;
          }

          // Get the values of properties that can be stored.
          if ($action == 'store') {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_table_alias = $this->getTableAliasForChadoTable($field_name, $key, $chado_table);
            $chado_column = $prop_storage_settings['chado_column'];

            if (array_key_exists($chado_table_alias, $records)) {
              if (array_key_exists($delta, $records[$chado_table_alias])) {
                if (array_key_exists($chado_column, $records[$chado_table_alias][$delta]['fields'])) {
                  $value = $records[$chado_table_alias][$delta]['fields'][$chado_column];
                  $values[$field_name][$delta][$key]['value']->setValue($value);
                }
              }
            }
          }

          // Get the values of properties that just want to read values.
          if (in_array($action, ['read_value', 'join'])) {
            // Both variants should have a chado column defined so grab that first.
            $chado_column = $prop_storage_settings['chado_column'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : $chado_column;
            if (array_key_exists('chado_table', $prop_storage_settings)) {
              $chado_table = $prop_storage_settings['chado_table'];
              $chado_table_alias = $this->getTableAliasForChadoTable($field_name, $key, $chado_table);
            }
            // Otherwise this is a join + we need the base table.
            // We can use the path to look this up.
            if (array_key_exists('path', $prop_storage_settings)) {
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
              $chado_table_alias = $left_table;
              // the column alias will actually include the right table alias
              // in order to keep the joins separate.
              // So we will grab that here.
              if (array_key_exists($field_name, $this->join_column_alias)) {
                $as = $this->join_column_alias[$field_name][$key][$as];
              }
              else {
                // If this field does not have a join_alias set
                // but it has a property with a path... then we cannot
                // set it's property. Hopefully it got here via findValues()
                // in which case this it will be set in a later iteration.
                // Just in case it is a problem though we will report it to the
                // field debugger.
                $this->field_debugger->printText("There was no JOIN alias set for $field_name when trying to set property values. If you see this message on a content page then there is a bug somewhere.");
                continue;
              }
            }
            $value = $records[$chado_table_alias][$delta]['fields'][$as];
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
   * @param bool $is_find
   *   Set to TRUE if we are building the record array for finding records.
   * @return array
   *   An associative array.
   */
  protected function buildChadoRecords($values, bool $is_find = FALSE) {
    $records = [];

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
      $storage_plugin_settings = $definition->getSettings()['storage_plugin_settings'];

      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {

          // Ensure we have a value to work with.
          if (!array_key_exists('value', $info) OR !is_object($info['value'])) {
            $this->logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the StoragePropertyValue object.',
              ['@field' => $field_name]));
            continue;
          }

          // Define the context array which will contain all details needed
          // for the buildChadoRecords_*() methods.
          $context = [];
          $context['is_find'] = $is_find;

          // Retrieve the property type for this value.
          $prop_value = $info['value'];
          $prop_type = $this->getPropertyType($field_name, $key);
          $prop_storage_settings = $prop_type->getStorageSettings();

          // Make sure we have an action for this property.
          if (!array_key_exists('action', $prop_storage_settings)) {
            $this->logger->error($this->t('Cannot store the property, @field.@prop ("@label"), in Chado. The property is missing an action in the property settings: @settings',
                ['@field' => $field_name, '@prop' => $key,
                 '@label' => $definition->getLabel(), '@settings' => print_r($prop_storage_settings, TRUE)]));
            continue;
          }
          $action = $prop_storage_settings['action'];

          // Check that the base table for the field is set.
          if (!array_key_exists('base_table', $storage_plugin_settings)) {
            $this->logger->error($this->t('Cannot store the property, @field.@prop, in Chado. The field is missing the chado base table name.',
                ['@field' => $field_name, '@prop' => $key]));
            continue;
          }
          $context['base_table'] = $storage_plugin_settings['base_table'];

          // Get the Chado table this specific property works with.
          // Use the base table as a default for properties which do not specify
          // the chado table (e.g. single value fields).
          /* TO BE REMOVED LATER */
          $chado_table = $context['base_table'];
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
          }

          // Retrieve the operation to be used for searching and if not set, use equals as the default.
          $context['operation'] = array_key_exists('operation', $info) ? $info['operation'] : '=';

          $context['field_name'] = $field_name;
          $context['property_key'] = $key;

          // Now for each action type, set the conditions and fields for
          // selecting chado records based on the other properties supplied.
          // ----------------------------------------------------------------
          switch ($action) {
            case 'store_id':
              $this->buildChadoRecords_store_id($records, $delta, $prop_storage_settings, $context, $prop_value);
              break;
            case 'store_pkey':
              $this->buildChadoRecords_store_pkey($records, $delta, $prop_storage_settings, $context, $prop_value);
              break;
            case 'store_link':
              $this->buildChadoRecords_store_link($records, $delta, $prop_storage_settings, $context, $prop_value);
              break;
            case 'store':
              $this->buildChadoRecords_store($records, $delta, $prop_storage_settings, $context, $prop_value);
              break;
            case 'read_value':
            case 'join':
              $this->buildChadoRecords_read_value($records, $delta, $prop_storage_settings, $context, $prop_value);
              break;
            case 'replace':
              // Do nothing here for properties that need replacement
              // since the values are provided by other properties.
              break;
            case 'function':
              // Do nothing here for properties that require post-processing
              // with a function as determining the value is handled by
              // the function not by chadostorage.
              break;
          }
        }
      }
    }

    // Now we want to iterate through the records and set any record IDs
    // for FK relationships based off the values set in the propertyValues
    // before chado storage was called.
    // Note: We have not yet done any querying ;-p
    // -----------------------------------------------------------------------
    $base_table = $context['base_table'];
    foreach ($records as $table_name => $deltas) {
      foreach ($deltas as $delta => $record) {
        // First for all the fields...
        if (array_key_exists('fields', $record)) {
          foreach ($record['fields'] as $chado_column => $val) {
            if (is_array($val) and $val[0] == 'REPLACE_BASE_RECORD_ID') {
              $core_table = $val[1];

              // If the core table is set in the base record ids array and the
              // value is not 0 then we can set this chado field now!
              if (array_key_exists($core_table, $this->base_record_ids) and $this->base_record_ids[$core_table] != 0) {
                $records[$table_name][$delta]['fields'][$chado_column] = $this->base_record_ids[$core_table];
              }
              // If the base record ID is 0 then this is an insert and we
              // don't yet have the base record ID.  So, leave in the message
              // to replace the ID so we can do so later.
              if (array_key_exists($base_table, $this->base_record_ids) and $this->base_record_ids[$base_table] != 0) {
                $records[$table_name][$delta]['fields'][$chado_column] = $this->base_record_ids[$base_table];
              }
            }
          }
        }
        // All records should have conditions set!
        if (!array_key_exists('conditions', $record)) {
          throw new \Exception('All Chado records built should have a conditions array set by this point. However, the following record does not: ' . print_r($record, TRUE));
        }
        foreach ($record['conditions'] as $chado_column => $val) {
          if (is_array($val['value']) and $val['value'][0] == 'REPLACE_BASE_RECORD_ID') {
            $core_table = $val['value'][1];

            // If the core table is set in the base record ids array and the
            // value is not 0 then we can set this condition now!
            if (array_key_exists($core_table, $this->base_record_ids) and $this->base_record_ids[$core_table] != 0) {
              $records[$table_name][$delta]['conditions'][$chado_column]['value'] = $this->base_record_ids[$core_table];
            }
            // If the base record ID is 0 then this is an insert and we
            // don't yet have the base record ID.  So, leave in the message
            // to replace the ID so we can do so later.
            if (array_key_exists($base_table, $this->base_record_ids) and $this->base_record_ids[$base_table] != 0) {
              $records[$table_name][$delta]['conditions'][$chado_column]['value'] = $this->base_record_ids[$base_table];
            }

          }
        }
      }
    }

    $this->field_debugger->summarizeBuiltRecords($this->base_record_ids, $records);

    return [
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
   * @param int $delta
   *   The position in the values array the current property type stands
   *   and thus the position in the records array it should be.
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
  protected function buildChadoRecords_store_id(array &$records, int $delta, array $storage_settings, array &$context, StoragePropertyValue $prop_value) {

    // Set to TRUE if we are building the record array for finding records.
    $is_find = $context['is_find'];

    // Get the Chado table this specific property works with.
    // Use the base table as a default for properties which do not specify
    // the chado table (e.g. single value fields).
    $chado_table = $context['base_table'];
    if (array_key_exists('chado_table', $storage_settings)) {
      $chado_table = $storage_settings['chado_table'];
    }

    // The store_id action should only be used for the base table...
    if ($chado_table !== $context['base_table']) {
      $this->logger->error($this->t('The @field.@key property type uses the '
        . 'store_id action type but is not associated with the base table of the field. '
        . 'Either change the base_table of this field or use store_pkey instead.',
        ['@field' => $context['field_name'], '@key' => $context['property_key']]));
    }

    // Now determine the primary key for the chado table.
    $chado_table_def = $this->connection->schema()->getTableDef($chado_table, ['format' => 'drupal']);
    $chado_table_pkey = $chado_table_def['primary key'];

    // For store_id action properties, the alias should always match the table name.
    $table_alias = $chado_table;
    $this->setChadoTableAliasMapping($chado_table, $table_alias, $context['field_name'], $context['property_key']);
    // If another alias is provided then we need to trow an exception.
    if (array_key_exists('chado_table_alias', $storage_settings)) {
      throw new \Exception($this->t('The @field.@key property type uses the '
        . 'store_id action type and tries to set a table alias, which are not '
        . 'supported for this type of action.',
        ['@field' => $context['field_name'], '@key' => $context['property_key']]));
    }

    // Get the value if it is set.
    $record_id = $prop_value->getValue();

    // If the record_id is zero then this is a brand-new value for
    // this property. Let's set it to be replaced in the hopes that
    // some other property has already been inserted and has the ID.
    if ($record_id == 0) {
      $records[$table_alias][0]['conditions'][$chado_table_pkey] = [
        'value' => [
          'REPLACE_BASE_RECORD_ID',
          $context['base_table']
        ],
        'operation' => $context['operation']
      ];
      // Now we add the chado table to our array of core tables
      // so that we can replace it with the value for the record later.
      if (!array_key_exists($chado_table, $this->base_record_ids)) {
        $this->base_record_ids[$chado_table] = $record_id;
      }
    }
    // However, if the record_id was set when the values were passed in,
    // then we want to set it here and add it to the array of core ids
    // for use later when replacing base record ids.
    else {
      $records[$table_alias][0]['conditions'][$chado_table_pkey] = [
        'value' => $record_id,
        'operation' => $context['operation']
      ];

      $this->base_record_ids[$table_alias] = $record_id;
    }

    // Additionally, if this is a find record request,
    // then we want to add the value to the 'field' as well.
    if ($is_find) {
      $records[$table_alias][0]['fields'][$chado_table_pkey] = $record_id;
    }
  }

  /**
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is store_pkey.
   *
   * STORE PKEY: stores the primary key value of a linking table.
   *
   * NOTE: A linking table is not a core table. This is important because
   * during insert and update, the core tables are handled first and then
   * linking tables are handled after.
   *
   * @param array $records
   *   The current set of chado records. This method will update this array.
   * @param int $delta
   *   The position in the values array the current property type stands
   *   and thus the position in the records array it should be.
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
  protected function buildChadoRecords_store_pkey(array &$records, int $delta, array $storage_settings, array &$context, StoragePropertyValue $prop_value) {

    // Set to TRUE if we are building the record array for finding records.
    $is_find = $context['is_find'];

    // Get the Chado table this specific property works with.
    // Use the base table as a default for properties which do not specify
    // the chado table (e.g. single value fields).
    $chado_table = $context['base_table'];
    if (array_key_exists('chado_table', $storage_settings)) {
      $chado_table = $storage_settings['chado_table'];
    }
    // Now determine the primary key for the chado table.
    $chado_table_def = $this->connection->schema()->getTableDef($chado_table, ['format' => 'drupal']);
    $chado_table_pkey = $chado_table_def['primary key'];

    // Check if there is a table alias set and if so, then use it.
    $table_alias = $chado_table;
    if (array_key_exists('chado_table_alias', $storage_settings)) {
      $table_alias = $storage_settings['chado_table_alias'];
    }
    $this->setChadoTableAliasMapping($chado_table, $table_alias, $context['field_name'], $context['property_key']);

    $link_record_id = $prop_value->getValue();

    $records[$table_alias][$delta]['conditions'][$chado_table_pkey] = [
      'value' => $link_record_id,
      'operation' => $context['operation']
    ];

    // When we are trying to find a value we need to add the field
    // for the primary key so it can be included in the query.
    if ($is_find) {
      // @todo Stephen had it set to NULL but I wonder if it should be $link_record_id
      $records[$table_alias][$delta]['fields'][$chado_table_pkey] = NULL;
    }

  }

  /**
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is store_link.
   *
   * STORE LINK: performs a join between two tables, one of which is a
   * core table and one of which is a linking table. The value which is saved
   * in this property is the left_table_id indicated in other key/value pairs.
   *
   * NOTE: A JOIN is not added to the query but rather this property stores
   * the id that a join would normally look up. This is much more performant.
   *
   * @param array $records
   *   The current set of chado records. This method will update this array.
   * @param int $delta
   *   The position in the values array the current property type stands
   *   and thus the position in the records array it should be.
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
  protected function buildChadoRecords_store_link(array &$records, int $delta, array $storage_settings, array &$context, StoragePropertyValue $prop_value) {

    // Set to TRUE if we are building the record array for finding records.
    $is_find = $context['is_find'];

    // The old implementation of store_link used chado_table/column notation
    // only for the right side of the relationship.
    // This meant we could not reliably determine the left side of the
    // relationship... Confirm this field uses the new method.
    if (array_key_exists('right_table', $storage_settings)) {
      // Using the tables with a store_id, determine which side of this
      // relationship is a base/core table. This will be used for the
      // fields below to ensure the ID is replaced.
      // Start by assuming the left table is the base/core table
      // (e.g. feature.feature_id = featureprop.feature_id).
      $link_base = $storage_settings['left_table'];
      $link_base_id = $storage_settings['left_table_id'];
      $linker = $storage_settings['right_table'];
      $linker_id = $storage_settings['right_table_id'];
      $linker_alias = (array_key_exists('right_table_alias', $storage_settings)) ? $storage_settings['right_table_alias'] : $linker;
      // Then check if the right table has a store_id and if so, use it instead.
      // (e.g. analysisfeature.analysis_id = analysis.analysis_id)
      if (array_key_exists($storage_settings['right_table'], $this->base_record_ids)) {
        $link_base = $storage_settings['right_table'];
        $link_base_id = $storage_settings['right_table_id'];
        $linker = $storage_settings['left_table'];
        $linker_id = $storage_settings['left_table_id'];
        $linker_alias = (array_key_exists('left_table_alias', $storage_settings)) ? $storage_settings['left_table_alias'] : $linker;
      }

      // If an alias was set then make sure it's added to the context.
      $this->setChadoTableAliasMapping($linker, $linker_alias, $context['field_name'], $context['property_key']);

      // @debug print "We decided it should be BASE $link_base.$link_base_id => LINKER $linker.$linker_id.\n";
      // We want to ensure that the linker table has a field added with
      // the link to replace the ID once it's available.
      $records[$linker_alias] = $records[$linker_alias] ?? [$delta => ['fields' => []]];
      $records[$linker_alias][$delta] = $records[$linker_alias][$delta] ?? ['fields' => []];
      $records[$linker_alias][$delta]['fields'] = $records[$linker_alias][$delta]['fields'] ?? [];
      if (!array_key_exists($linker_id, $records[$linker_alias][$delta]['fields'])) {
        $records[$linker_alias][$delta]['fields'][$linker_id] = ['REPLACE_BASE_RECORD_ID', $link_base];
        // @debug print "Adding a note to replace $linker.$linker_id with $link_base record_id\n";
      }

      // If this is a find operation then we need to add a condition
      // to the linker table, using the base record id.
      if ($is_find) {
        $base_table = $context['base_table'];
        $base_record_id = $this->base_record_ids[$base_table];

        $path = [$link_base . "." . $link_base_id . ">" . $linker . "." . $linker_id];
        $this->addChadoRecordJoins($records, $linker_id, $context['property_key'], $delta, $path, $context['field_name'], $context['property_key']);

        // Add the condition to the linker table so we get the correct record.
        $records[$linker_alias][$delta]['conditions'][$linker_id] = [
          'value' => $base_record_id,
          'operation' => '='
        ];
      }
    }
    else {
      // Otherwise this field is using the old method for store_link.
      // We will enter backwards compatibility mode here to do our best...
      // It will work to handle CRUD for direct connections (e.g. props)
      // but will cause problems with double-hops and all token replacement.
      $bc_chado_table = $storage_settings['chado_table'];
      $bc_chado_column = $storage_settings['chado_column'];
      $records[$bc_chado_table][$delta]['fields'][$bc_chado_column] = ['REPLACE_BASE_RECORD_ID', $context['base_table']];

      // We also need to set the mapping so that the id can be replaced later.
      $this->setChadoTableAliasMapping($bc_chado_table, $bc_chado_table, $context['field_name'], $context['property_key']);

      $this->logger->warning(
        'We had to use backwards compatible mode for :name.:key property type with an action of store_link.'
        .' Please update the code for this field to use left/right table notation for the store_link property.'
        .' Backwards compatible mode should allow this field to save/load data but may result in errors with token replacement and publishing.',
        [':name' => $context['field_name'], ':key' => $context['property_key']]
      );
    }
  }

  /**
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is store.
   *
   * STORE: indicates that the value of this property can be loaded and
   * stored in the Chado table indicated by this property.
   *
   * @param array $records
   *   The current set of chado records. This method will update this array.
   * @param int $delta
   *   The position in the values array the current property type stands
   *   and thus the position in the records array it should be.
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
  protected function buildChadoRecords_store(array &$records, int $delta, array $storage_settings, array &$context, StoragePropertyValue $prop_value) {

    // Set to TRUE if we are building the record array for finding records.
    $is_find = $context['is_find'];

    // Get the Chado table this specific property works with.
    // Use the base table as a default for properties which do not specify
    // the chado table (e.g. single value fields).
    $chado_table = $context['base_table'];
    if (array_key_exists('chado_table', $storage_settings)) {
      $chado_table = $storage_settings['chado_table'];
    }

    // Check if there is a table alias set and if so, then use it.
    $table_alias = $chado_table;
    if (array_key_exists('chado_table_alias', $storage_settings)) {
      $table_alias = $storage_settings['chado_table_alias'];
    }
    $this->setChadoTableAliasMapping($chado_table, $table_alias, $context['field_name'], $context['property_key']);

    // Now grab the column we are interested in.
    $chado_column = $storage_settings['chado_column'];

    // Retrieve the value and clean it up.
    $value = $prop_value->getValue();
    if (is_string($value)) {
      $value = trim($value);
    }

    $records[$table_alias][$delta]['fields'][$chado_column] = $value;

    // If this field should not allow an empty value that means this
    // entire record should be removed on an update and not inserted.
    $delete_if_empty = FALSE;
    $empty_value = '';
    if (array_key_exists('delete_if_empty', $storage_settings)) {
      $delete_if_empty = $storage_settings['delete_if_empty'];
    }
    if (array_key_exists('empty_value', $storage_settings)) {
      $empty_value = $storage_settings['empty_value'];
    }
    if ($delete_if_empty) {
      $records[$table_alias][$delta]['delete_if_empty'][] =
        ['chado_column' => $chado_column, 'empty_value' => $empty_value];
    }

    // If this is a find operation then we want to add a condition
    // for all stored values.
    if ($is_find) {
      if (!empty($value)) {
        $records[$table_alias][$delta]['conditions'][$chado_column] = [
          'value' => $value,
          'operation' => '='
        ];
      }
    }
  }

  /**
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is read_value.
   *
   * READ_VALUE: selecting a single column. This cannot be used for inserting or
   * updating values. Instead we use store actions for that.
   * If reading a value from a non-base table, then the path should
   * be provided.
   *
   * This also supports the deprecated 'join' action.
   *
   * @param array $records
   *   The current set of chado records. This method will update this array.
   * @param int $delta
   *   The position in the values array the current property type stands
   *   and thus the position in the records array it should be.
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
  protected function buildChadoRecords_read_value(array &$records, int $delta, array $storage_settings, array &$context, StoragePropertyValue $prop_value) {

    // Set to TRUE if we are building the record array for finding records.
    $is_find = $context['is_find'];

    // Get the Chado table this specific property works with.
    // Use the base table as a default for properties which do not specify
    // the chado table (e.g. single value fields).
    $chado_table = $context['base_table'];
    if (array_key_exists('chado_table', $storage_settings)) {
      $chado_table = $storage_settings['chado_table'];
    }

    // Check if there is a table alias set and if so, then use it.
    $table_alias = $chado_table;
    if (array_key_exists('chado_table_alias', $storage_settings)) {
      $table_alias = $storage_settings['chado_table_alias'];
    }
    $this->setChadoTableAliasMapping($chado_table, $table_alias, $context['field_name'], $context['property_key']);

    $chado_column = $storage_settings['chado_column'];

    // If a join is needed to access the column, then the 'path' needs
    // to be defined and the joins need to be added to the query.
    // This will also add the fields to be selected.
    if (array_key_exists('path', $storage_settings)) {
      $path = $storage_settings['path'];
      $as = array_key_exists('as', $storage_settings) ? $storage_settings['as'] : $chado_column;
      $path_arr = explode(";", $path);
      $this->addChadoRecordJoins($records, $chado_column, $as, $delta, $path_arr, $context['field_name'], $context['property_key']);
    }
    // Otherwise, it is a column in a base table. In this case, we
    // only need to ensure the column is added to the fields.
    else {
      // We will only set this if it's not already set.
      // This is to allow another field with a store set for this column
      // to set this value. We actually only do this to ensure it ends up
      // in the query fields.
      if (!array_key_exists('fields', $records[$table_alias][$delta])) {
        $records[$table_alias][$delta]['fields'] = [];
        $records[$table_alias][$delta]['fields'][$chado_column] = NULL;
      }
      elseif (!array_key_exists($chado_column, $records[$table_alias][$delta]['fields'])) {
        $records[$table_alias][$delta]['fields'][$chado_column] = NULL;
      }
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
      int $delta, array $path_arr, string $field_name, string $property_key, $parent_table = NULL, $parent_column = NULL, $depth = 0) {

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
      $records[$parent_table][$delta]['joins'][$right_table][$parent_column]['columns'][] = [$chado_column, $as, $field_name, $property_key];
      return;
    }

    // Add the right table back onto the path as the new left table and recurse.
    $depth++;
    $this->addChadoRecordJoins($records, $chado_column, $as, $delta, $path_arr, $field_name, $property_key, $parent_table, $parent_column, $depth);
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
      if ($info['not null'] == TRUE and (!isset($col_val) or ($col_val == ''))) {
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

    $build = $this->buildChadoRecords($values);
    $base_tables = $this->base_record_ids;
    $records = $build['records'];
    $violations = [];

    // We only need to validate the base table properties because
    // the linker table values get completely replaced on an update and
    // should not exist for an insert.
    foreach ($this->base_record_ids as $base_table => $record_id) {
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

  /**
   * Retrieve the chado table name when given the table alias.
   *
   * @param string $table_alias
   *   The table alias for which you would like to look up the mapping.
   * @param array $property_storage
   *   The storage information for the property when available. This is used
   *   to set the table alias mapping if it is not already set.
   *
   * @return string $chado_table
   *   The name of the chado table the alias referrences.
   */
  protected function getChadoTableFromAlias(string $table_alias, array $property_storage = []) {

    // If the mapping has not yet been set then we need to do some
    // detective work to figure it out... Let's do that first
    // and then update the mapping.
    if (!array_key_exists($table_alias, $this->table_alias_mapping)) {
      // If there is a chado table set in the property storage details
      // then we can use it to set the mapping and return it.
      if (array_key_exists('chado_table', $property_storage)) {
        $chado_table = $property_storage['chado_table'];
        $this->table_alias_mapping[$table_alias] = $chado_table;
      }
      // If the action is store_link then this might be the right or left table
      // alias so check those as well.
      elseif (array_key_exists('right_table_alias', $property_storage)) {
        $right_table_alias = $property_storage['right_table_alias'];
        if ($right_table_alias == $table_alias) {
          $chado_table = $proeprty_storage['right_table'];
        }
        $this->table_alias_mapping[$table_alias] = $chado_table;
      }
      elseif (array_key_exists('left_table_alias', $property_storage)) {
        $left_table_alias = $property_storage['left_table_alias'];
        if ($left_table_alias == $table_alias) {
          $chado_table = $proeprty_storage['left_table'];
        }
        $this->table_alias_mapping[$table_alias] = $chado_table;
      }
      // Otherwise, the default table alias is the same as the table name
      // so update the mapping and return the table name.
      else {
        $chado_table = $table_alias;
        $this->table_alias_mapping[$table_alias] = $chado_table;
      }
    }

    return $this->table_alias_mapping[$table_alias];
  }

  /**
   * Retrieves the table alias for a given chado table when the field and property key are known.
   *
   * NOTE: buildChadoRecords() must have been called first!
   *
   * @param string $field_name
   * @param string $property_key
   * @param string $chado_table
   */
  protected function getTableAliasForChadoTable($field_name, $property_key, $chado_table) {

    if (array_key_exists($field_name, $this->reverse_alias_mapping)) {
      if (array_key_exists($property_key, $this->reverse_alias_mapping[$field_name])) {
        if (array_key_exists($chado_table, $this->reverse_alias_mapping[$field_name][$property_key])) {
          return $this->reverse_alias_mapping[$field_name][$property_key][$chado_table];
        }
      }
    }

    $this->logger->warning('ChadoStorage could not find the table alias for the requested chado table. Specifically, we were trying to look up the alias for @table for the field @field, property @property.',
      ['@table' => $chado_table, '@field' => $field_name, '@property' => $property_key]);

    return NULL;
  }

  /**
   * Sets the mapping between chado tables and their alias'
   *
   * @param $chado_table
   * @param $table_alias
   * @param string $field_name
   * @param string $property_key
   */
  protected function setChadoTableAliasMapping($chado_table, $table_alias, $field_name, $property_key) {

    $this->table_alias_mapping[$table_alias] = $chado_table;
    $this->reverse_alias_mapping[$field_name][$property_key][$chado_table] = $table_alias;

  }

  /**
   *
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::publishFrom()
   */
  public function publishForm($form, FormStateInterface &$form_state) {

    $chado_schemas = [];
    $chado = \Drupal::service('tripal_chado.database');
    foreach ($chado->getAvailableInstances() as $schema_name => $details) {
      $chado_schemas[$schema_name] = $schema_name;
    }
    $default_chado = $chado->getSchemaName();

    $storage_form['schema_name'] = [
      '#type' => 'select',
      '#title' => 'Chado Schema Name',
      '#required' => TRUE,
      '#description' => 'Select one of the installed Chado schemas to import into.',
      '#options' => $chado_schemas,
      '#default_value' => $default_chado,
    ];

    return $storage_form;
  }
}
