<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoFieldDebugger;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal_chado\TripalStorage\ChadoRecords;

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
   * Holds an instance of ChadoRecords.
   *
   * @var \Drupal\tripal_chado\TripalStorage\ChadoRecords
   */
  protected $records = NULL;

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
	 * @{inheritdoc}
	 */
  public function insertValues(&$values) : bool {

    $this->field_debugger->printHeader('Insert');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::insertValues');
    $this->records = new ChadoRecords($this->field_debugger, $values, $this->connection);

    $this->buildChadoRecords($values);
    $base_tables = $this->records->getBaseTables();
    $all_tables = $this->records->getTables();
    $transaction_chado = $this->connection->startTransaction();
    try {

      // First: Insert the base table records.
      foreach ($base_tables as $base_table) {
        $this->records->insertTable($base_table);
      }

      // Second: Insert non base table records.
      foreach ($all_tables as $table_alias) {
        if (in_array($table_alias, $base_tables)) {
          continue;
        }
        $this->records->insertTable($table_alias);
      }
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw new \Exception($e);
    }

    // Now set the record Ids of the properties.
    return TRUE;
  }

  /**
   * @{inheritdoc}
   */
  public function updateValues(&$values) : bool {

    $this->field_debugger->printHeader('Update');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::updateValues');
    $this->records = new ChadoRecords($this->field_debugger, $values, $this->connection);
    $this->buildChadoRecords($values);

    $base_tables = $this->records->getBaseTables();
    $all_tables = $this->records->getTables();
    $transaction_chado = $this->connection->startTransaction();
    try {

      // Handle base table records first.
      foreach ($base_tables as $base_table) {
        $this->records->updateTable($base_table);
      }

      // Next delete all non base records so we can replace them
      // with updates. This is necessary because we may violate unique
      // constraints if we don't e.g. changing the order of records with a
      // rank.
      foreach ($all_tables as $table_alias) {
          // Skip base table records.
        if (in_array($table_alias, array_keys($base_tables))) {
          continue;
        }
        $this->records->deleteTable($table_alias);
      }

      // Now insert all new values for the non-base table records.
      foreach ($all_tables as $table_alias) {
        // Skip base table records.
        if (in_array($table_alias, array_keys($base_tables))) {
          continue;
        }
        $this->records->insertTable($table_alias);
      }
      $this->setRecordIds($values, $this->records);
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw new \Exception($e);
    }
    return TRUE;
  }

  /**
   * @{inheritdoc}
   */
  public function loadValues(&$values) : bool {

    $this->field_debugger->printHeader('Load');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::loadValues');
    $this->records = new ChadoRecords($this->field_debugger, $values, $this->connection);

    $this->buildChadoRecords($values);
    $all_tables = $this->records->getTables();

    $transaction_chado = $this->connection->startTransaction();
    try {
      foreach ($all_tables as $table_alias) {
        $this->records->selectTable($table_alias);
      }
      $this->setPropValues($values, $this->records);
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
    $this->records = new ChadoRecords($this->field_debugger, $values, $this->connection);

    return FALSE;
  }

  /**
   * @{inheritdoc}
   */
  public function findValues($values) {

    $this->field_debugger->printHeader('Find');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::findValues');
    $this->records = new ChadoRecords($this->field_debugger, $values, $this->connection);

    $this->buildChadoRecords($values, TRUE);
    $base_tables = $this->records->getBaseTables();
    $all_tables = $this->records->getTables();

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
      foreach ($base_tables as $base_table) {

        // First we use findChadoRecords() to query chado for all records
        // in the table specified as $base_table. Each match returned here
        // is a query result.
        // @debug print "\t$base_table Record: " . print_r($record, TRUE);
        $matches = $this->findChadoRecords($base_table);
        // Now for each of these query results...
        while ($match = $matches->fetchAssoc()) {
          // @debug print "\t\tWorking on Query Record: " . print_r($match, TRUE);

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

          // Get the value column information for this property.
          $base_table = $storage_plugin_settings['base_table'];
          $path = $prop_storage_settings['path'];
          $path_array = $this->parsePath($path);
          $value_col_info = $this->getPathValueColumn($path_array);
          $chado_table  = $value_col_info['chado_table'];
          $chado_column  = $value_col_info['chado_column'];
          $table_alias  = $value_col_info['table_alias'];
          $column_alias  = $value_col_info['column_alias'];

          // Grab the pkey using the schema definition.
          $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
          $chado_table_pkey = $chado_table_def['primary key'];

          // Now grab the table alias.
          $chado_table_alias = $this->getTableAliasForChadoTable($field_name, $key, $chado_table);

          // For finding we only need to worry about the store_id action.
          if ($is_find) {
            if ($action == 'store_id') {
              $record_id = $records[$table_alias][0]['fields'][$column_alias];
              $values[$field_name][$delta][$key]['value']->setValue($record_id);
              $this->records->setBaseRecord($table_alias, $record_id);
            }
          }
          else {
            $record_id = $records[$table_alias][0]['conditions'][$column_alias]['value'];
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
          if ($action == 'store_link') {
            continue;
          }

          // Get the value column information for this property.
          $path = $prop_storage_settings['path'];
          $path_array = $this->parsePath($path);
          $value_col_info = $this->getPathValueColumn($path_array);
          $chado_table  = $value_col_info['chado_table'];
          $chado_column  = $value_col_info['chado_column'];
          $table_alias  = $value_col_info['table_alias'];
          $column_alias  = $value_col_info['column_alias'];

          // Get the values of properties that can be stored.
          if ($action == 'store') {
            $value = $this->records->getValue($table_alias, $delta, $column_alias);
            $values[$field_name][$delta][$key]['value']->setValue($value);
          }

          // Get the values of properties that just want to read values.
          if ($action == 'read_value') {
            $value = $this->records->getValue($table_alias, $delta, $column_alias);
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
          $context['property_settings'] = $prop_storage_settings;
          $context['delta'] = $delta;


          // Get the path array for this field and add any joins if any are needed.
          if (array_key_exists('path', $prop_storage_settings)) {
            $path_array = $this->parsePath($prop_storage_settings['path'],
                array_key_exists('table_alias_mapping', $prop_storage_settings) ? $prop_storage_settings['table_alias_mapping'] : [],
                array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : '');
            $context['path_string'] = $prop_storage_settings['path'];
            $context['path_array'] = $path_array;
            if (array_key_exists('join', $path_array)) {
              $this->addChadoRecordJoins($path_array, $context);
            }
          }

          // Now for each action type, set the conditions and fields for
          // selecting chado records based on the other properties supplied.
          // ----------------------------------------------------------------
          switch ($action) {
            case 'store_id':
              $this->buildChadoRecords_store_id($context, $prop_value);
              break;
            case 'store_pkey':
              $this->buildChadoRecords_store_pkey($context, $prop_value);
              break;
            case 'store_link':
              $this->buildChadoRecords_store_link($context, $prop_value);
              break;
            case 'store':
              $this->buildChadoRecords_store($context, $prop_value);
              break;
            case 'read_value':
              $this->buildChadoRecords_read_value($context, $prop_value);
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

    // Replace any IDs with their values.
    $this->records->setIDs();

    // Set some debugging info.
    $this->field_debugger->summarizeBuiltRecords($this->records);
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
   * @param array $context
   *   A set of values to provide context. These a pre-computed in the parent method
   *   to reduce code duplication when a task is done for all/many storage properties
   *   regardless of their action.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function buildChadoRecords_store_id(array &$context, StoragePropertyValue $prop_value) {

    $base_table = $context['base_table'];
    $record_id = $prop_value->getValue();
    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $elements = [
      'base_table' =>  $base_table,
      'chado_table' => $value_col_info['chado_table'],
      'table_alias' => $value_col_info['table_alias'],
      'chado_column' => $value_col_info['chado_column'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $record_id,
      'operation' => $context['operation']
    ];

    // The store_id action should only be used for the base table...
    // @todo: I think these checks should go into a field validation test rather than here.
    if ($elements['chado_table'] !== $elements['base_table']) {
      $this->logger->error($this->t('The @field.@key property type uses the '
        . 'store_id action type but is not associated with the base table of the field. '
        . 'Either change the base_table of this field or use store_pkey instead.',
        ['@field' => $context['field_name'], '@key' => $context['property_key']]));
    }

    // Now determine the primary key for the chado table.
    $chado_table_def = $this->connection->schema()->getTableDef($elements['chado_table'], ['format' => 'drupal']);
    $chado_table_pkey = $chado_table_def['primary key'];
    if ($elements['chado_column'] !== $chado_table_pkey) {
      $this->logger->error($this->t('The @field.@key property type uses the '
          . 'store_id action and the column specified in the "path" settings is not '
          . 'the primary key for base table. ',
          ['@field' => $context['field_name'], '@key' => $context['property_key']]));
    }

    // If this is a store_id then we're storing the base table record.
    $this->records->setBaseRecord($elements['chado_table'], $prop_value->getValue());

    // Set the field and the condition if we have a record_id.
    if ($record_id > 0) {
      $this->records->setField($elements);
      $this->records->setCondition($elements);
    }
    // If the record_id is zero then this is a brand-new value for
    // this property. Let's set it to be replaced in the hopes that
    // some other property has already been inserted and has the ID.
    else {
      $elements['value'] =  ['REPLACE_BASE_RECORD_ID', $base_table];
      $this->records->setField($elements);
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
   * @param array $context
   *   A set of values to provide context. These a pre-computed in the parent method
   *   to reduce code duplication when a task is done for all/many storage properties
   *   regardless of their action.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function buildChadoRecords_store_pkey(array &$context, StoragePropertyValue $prop_value) {

    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $elements = [
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      // When we are trying to find a value we need an empty pkey.
      'value' => $context['is_find'] ? NULL : $prop_value->getValue(),
      'operation' => $context['operation']

    ];
    $this->records->setField($elements);

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
   * @param array $context
   *   A set of values to provide context. These a pre-computed in the parent method
   *   to reduce code duplication when a task is done for all/many storage properties
   *   regardless of their action.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function buildChadoRecords_store_link(array &$context, StoragePropertyValue $prop_value) {

    $base_table = $context['base_table'];
    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $elements = [
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      // We want to ensure that the linker table has a field added with
      // the link to replace the ID once it's available.
      'value' => ['REPLACE_BASE_RECORD_ID', $base_table],
      'operation' => $context['operation']
    ];
    $this->records->setField($elements);

    // If this is a find operation then we need to add a condition
    // to the linker table, using the base record id.
    if ($context['is_find']) {
      $base_table = $context['base_table'];
      $elements['value'] = $this->records->getBaseRecordID($base_table);
      $this->records->setCondition($elements);
    }
  }

  /**
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is store.
   *
   * STORE: indicates that the value of this property can be loaded and
   * stored in the Chado table indicated by this property.
   *
   * @param array $context
   *   A set of values to provide context. These a pre-computed in the parent method
   *   to reduce code duplication when a task is done for all/many storage properties
   *   regardless of their action.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function buildChadoRecords_store(array &$context, StoragePropertyValue $prop_value) {

    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $value = $prop_value->getValue();
    if (is_string($value)) {
      $value = trim($value);
    }
    $elements = [
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $value,
      'operation' => $context['operation'],
      'delete_if_empty' => array_key_exists('delete_if_empty', $context['property_settings']) ? $context['property_settings']['delete_if_empty'] : FALSE,
      'empty_value' => array_key_exists('empty_value', $context['property_settings']) ? $context['property_settings']['empty_value'] : '',
    ];

    $this->records->setField($elements);

    // If this is a find operation then we want to add a condition.
    if ($context['is_find'] and !empty($value)) {
      $this->records->setCondition($elements);
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
   * @param array $context
   *   A set of values to provide context. These a pre-computed in the parent method
   *   to reduce code duplication when a task is done for all/many storage properties
   *   regardless of their action.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function buildChadoRecords_read_value(array &$context, StoragePropertyValue $prop_value) {

    // Adding of fields via a join are handled by the ChadoRecord::setJoin() functino.
    if (array_key_exists('join', $context['path_array'])) {
      return;
    }

    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $elements = [
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => NULL,
      'operation' => $context['operation'],
    ];
    $this->records->setField($elements);
  }

  /**
   * Takes a path string for a field property and converts it to an array structure.
   *
   * @param mixed $path
   *   A string continaining the path.  Note: this is a recursive function and on
   *   recursive calls this variable will be n array. Hence, the type is "mixed".*
   * @param array $aliases
   *   Optional. The list of table aliases provdied by the `table_alias_mapping`
   *   argument of a field.  If this variable is an empty array then the function
   *   will use the table name provided in the path.
   * @param string $as
   *   An alias to be used for the Chado table column that contains the value. This
   *   argument will rename the column.
   * @param $full_path
   *   This argument is used by recursion to build the string path for each level.
   *   It should not be set by the callee.
   * @return array
   *
   */
  protected function parsePath(mixed $path, array $aliases = [], string $as = '', $full_path = '') {

    // If the path is a string then split it.
    $path_arr = [];
    if (is_string($path)) {
      // For sanity sake, remove any trailing semicolons that might be there by accident.
      $trimmed_path = trim($path, ';');
      $path_arr = explode(";", $trimmed_path);
    }
    if (is_array($path)) {
      $path_arr = $path;
    }

    // Get the current path in the list.
    $curr_path = array_shift($path_arr);
    $full_path = $full_path ? $full_path . ';' . $curr_path : $curr_path;

    // If the path has a '>' then this is a join.
    if (preg_match('/>/', $curr_path)) {

      // Get the left column and the right table join infor.
      list($left, $right) = explode(">", $curr_path);
      list($left_alias, $left_column) = explode(".", $left);
      list($right_alias, $right_column) = explode(".", $right);

      // Get the true Chado tables from the alias array.  Otherwise use
      // the table provided.  If the developer gave a bad Chado table or
      // didn't provide a proper mapping to an alias, then an SQL error
      // will occur. We don't check it here.
      $left_table = $left_alias;
      $right_table = $right_alias;
      if (array_key_exists($left_alias, $aliases)) {
        $left_table = $aliases[$left_alias];
      }
      if (array_key_exists($right_alias, $aliases)) {
        $right_table = $aliases[$right_alias];
      }

      // Build the return array for the join.
      $ret_array = [
        'chado_table' => $left_table,
        'table_alias' => $left_alias,
        'join' => [
          'path' => $full_path,
          // The path string has no way to specify the type of join so
          // we'll default it to an 'inner' join.
          'type' => 'inner',
          'chado_table' => $right_table,
          'table_alias' => $right_alias,
          'left_column' => $left_column,
          'right_column' => $right_column,
        ],
      ];

      // Before we return, let's check if we have more sub paths to process.
      // if so, then recurse.
      $sub_path_arr = [];
      if (count($path_arr) > 0) {
        $sub_path_arr = $this->parsePath($path_arr, $aliases, $as, $full_path);
      }
      // If there are no more joins, then we need to set the value column to be
      // the same as the last column in tge join.
      else {
        $ret_array['join']['value_column'] = $right_column;
        $ret_array['join']['value_alias'] = $as ? $as : $right_column;
      }

      // If we have a value column in the return value then this means that it hit
      // the end of the join and the value column in which the value is stored
      // will be at the end.  We can just merge that information with the current
      // return array.
      if (array_key_exists('value_column', $sub_path_arr)) {
        $ret_array['join'] = array_merge($ret_array['join'], $sub_path_arr);
      }
      // Otherwise this is another join.
      else if (array_key_exists('chado_table', $sub_path_arr)) {
        $ret_array['join']['join'] = $sub_path_arr['join'];
      }

      return $ret_array;
    }

    // If the path is not a join but has a period then this specifices
    // the table and the column with the value.
    else if (preg_match('/\./', $curr_path)) {
      list($table_alias, $value_column) = explode(".", $path);
      $chado_table = $table_alias;
      if (array_key_exists($table_alias, $aliases)) {
        $chado_table = $aliases[$table_alias];
      }
      return [
        'path' => $full_path,
        'chado_table' => $chado_table,
        'table_alias' => $table_alias,
        'value_column' => $value_column,
        'value_alias' => $as ? $as : $value_column
      ];
    }

    // There is no period in the path so there is no Chado table. We are at the
    // end of the path with joins and we can just return the value column.
    else {
      return [
        'value_column' => $curr_path,
        'value_alias' => $as ? $as : $curr_path
      ];
    }
  }

  /**
   * A helper function to quickly get the value column information from a path.
   *
   * @param array $path
   *   The parsed path of the field property.
   */
  protected function getPathValueColumn(array $path) {

    if (array_key_exists('value_column', $path)) {
      return [
        'chado_table' => $path['chado_table'],
        'table_alias' => $path['table_alias'],
        'chado_column' => $path['value_column'],
        'column_alias' => $path['value_alias'],
      ];
    }
    else if (array_key_exists('join', $path)) {
      return $this->getPathValueColumn($path['join']);
    }
    // We shouldn't get here.
    return NULL;
  }


  /**
   *
   * @param array $path_array
   * @param array $context
   */
  protected function addChadoRecordJoins(array $path_array, array $context) {

    $elements = [
      'chado_table' => $context['base_table'],
      'table_alias' => $context['base_table'],
      'delta' => $context['delta'],
      'left_table' => $path_array['chado_table'],
      'left_alias' => $path_array['table_alias'],
      'left_column' => $path_array['join']['left_column'],
      'right_table' => $path_array['join']['chado_table'],
      'right_alias' => $path_array['join']['table_alias'],
      'right_column' => $path_array['join']['right_column'],
      'join_type' => $path_array['join']['type'],
      'join_path' => $path_array['join']['path'],
    ];
    $this->records->setJoin($elements);

    // If there is another join then handle that.
    if (array_key_exists('join', $path_array['join'])) {
      $this->addChadoRecordJoins($path_array['join'], $context);
    }

    // If we've reached the end of the joins we need to add the join columns.
    if (array_key_exists('value_column', $path_array['join'])) {
      $elements = [
        'chado_table' => $context['base_table'],
        'table_alias' => $context['base_table'],
        'delta' => $context['delta'],
        'join_path' => $path_array['join']['path'],
        'chado_column' => $path_array['join']['value_column'],
        'column_alias' =>  $path_array['join']['value_alias'],
        'field_name' => $context['field_name'],
        'property_key' => $context['property_key'],
      ];
      $this->records->setJoinCols($elements);
    }
  }

  /**
   *
   * {@inheritDoc}
   */
  public function validateValues($values) {

    $this->field_debugger->printHeader('Validate');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::validateValues');

    $this->buildChadoRecords($values);
    $violations = $this->records->validate();

    return $violations;
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
