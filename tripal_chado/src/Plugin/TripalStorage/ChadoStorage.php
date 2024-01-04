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
use Drupal\Core\Render\Element\Token;

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

    // Setup field debugging.
    $this->field_debugger->printHeader('Insert');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::insertValues');

    // Build the ChadoRecords object.
    $this->records = new ChadoRecords($this->field_debugger, $this->connection);
    $this->buildChadoRecords($values);

    $transaction_chado = $this->connection->startTransaction();
    try {

      // First: Insert the base table records.
      $base_tables = $this->records->getBaseTables();
      foreach ($base_tables as $base_table) {
        $this->records->insertRecords($base_table, $base_table);
      }

      // Second: Insert records from the ancillary tables of
      // each base table.
      foreach ($base_tables as $base_table) {
        $tables = $this->records->getAncillaryTables($base_table);
        foreach ($tables as $table_alias) {
          $this->records->insertRecords($base_table, $table_alias);
        }
      }

      // Now taht we've done the inserts, set the property values.
      $this->setPropValues($values, $this->records);
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
  public function updateValues(&$values) : bool {

    // Setup field debugging.
    $this->field_debugger->printHeader('Update');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::updateValues');

    // Build the ChadoRecords object.
    $this->records = new ChadoRecords($this->field_debugger, $this->connection);
    $this->buildChadoRecords($values);


    $transaction_chado = $this->connection->startTransaction();
    try {

      // Handle base table records first.
      $base_tables = $this->records->getBaseTables();
      foreach ($base_tables as $base_table) {
        $this->records->updateRecords($base_table, $base_table);
      }

      // Next delete all non base records so we can replace them
      // with updates. This is necessary because we may violate unique
      // constraints if we don't e.g. changing the order of records with a
      // rank.
      foreach ($base_tables as $base_table) {
        $tables = $this->records->getAncillaryTables($base_table);
        foreach ($tables as $table_alias) {
          $this->records->deleteRecords($base_table, $table_alias, TRUE);
        }
      }

      // Now insert all new values for the non-base table records.
      foreach ($base_tables as $base_table) {
        $tables = $this->records->getAncillaryTables($base_table);
        foreach ($tables as $table_alias) {
          $this->records->insertRecords($base_table, $table_alias);
        }
      }

      // Now taht we've done the updates, set the property values.
      $this->setPropValues($values, $this->records);
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

    // Setup field debugging.
    $this->field_debugger->printHeader('Load');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::loadValues');

    // Build the ChadoRecords object.
    $this->records = new ChadoRecords($this->field_debugger, $this->connection);
    $this->buildChadoRecords($values);

    $transaction_chado = $this->connection->startTransaction();
    try {

      $base_tables = $this->records->getBaseTables();
      foreach ($base_tables as $base_table) {

        // Do the select for the base tables
        $this->records->selectRecords($base_table, $base_table);

        // Then do the selects for the ancillary tables.
        $tables = $this->records->getAncillaryTables($base_table);
        foreach ($tables as $table_alias) {
          $this->records->selectRecords($base_table, $table_alias);
        }
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
   * @{inheritdoc}
   */
  public function deleteValues($values) : bool {

    $this->field_debugger->printHeader('Delete');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::deleteValues');
    $this->records = new ChadoRecords($this->field_debugger, $this->connection);

    return FALSE;
  }

  /**
   * @{inheritdoc}
   */
  public function findValues($values) {

    // Setup field debugging.
    $this->field_debugger->printHeader('Find');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::findValues');

    // Build the ChadoRecords object.
    $this->records = new ChadoRecords($this->field_debugger, $this->connection);
    $this->buildChadoRecords($values, TRUE);

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
      $base_tables = $this->records->getBaseTables();
      foreach ($base_tables as $base_table) {

        // First we find all matching base records.
        $matches = $this->records->findRecords($base_table, $base_table);

        // Now for each of each matching base record we need to select
        // the anciallry tables.
        foreach ($matches as $match) {

          $tables = $this->records->getAncillaryTables($base_table);
          foreach ($tables as $table_alias) {
            $match->selectRecords($base_table, $table_alias);
          }

          // Clone the value array for this match and set it's properties
          $new_values = $this->cloneValues($values);
          $this->setPropValues($new_values, $match);
          $found_list[] = $new_values;
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
   * Sets the property values using the records returned from Chado.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   *
   * @param ChadoRecords $records
   *   An instance of a ChadoRecords object from which values will be pulled.
   *   We don't use the built in member variable and instead allow it to
   *   be passed in because the findValues() functino can generate copies
   *   of the $records array and use that to set multiple values.
   */
  protected function setPropValues(&$values, ChadoRecords $records) {

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

          // Get the values of properties that can be stored.
          if ($action == 'replace') {
            $replace[] = [$field_name, $delta, $key, $info];
          }
          else if ($action == 'function') {
            $function[] = [$field_name, $delta, $key, $info];
          }
          else {

            // Parse the path.
            $base_table = $storage_plugin_settings['base_table'];
            $path = $prop_storage_settings['path'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : '';
            $table_alias_mapping = array_key_exists('table_alias_mapping', $prop_storage_settings) ? $prop_storage_settings['table_alias_mapping'] : [];
            $path_array = $this->parsePath($base_table, $path, $table_alias_mapping, $as);

            // Get the value column information for this property.
            $base_table = $storage_plugin_settings['base_table'];
            $value_col_info = $this->getPathValueColumn($path_array);
            $table_alias  = $value_col_info['table_alias'];
            $column_alias  = $value_col_info['column_alias'];

            // For values that come from joins, we need to use the root table
            // becuase this is the table that will have the value.
            $my_delta = $delta;
            if($action == 'read_value' and array_key_exists('join', $path_array)) {
              $root_table = $value_col_info['root_table'];
              $root_alias = $value_col_info['root_alias'];
              $table_alias = $root_alias;

              // For values that come from a join on the base table we need
              // to get the value from there but cardinatlity on the base table
              // is always just 1.
              if ($base_table == $root_table) {
                $my_delta = 0;
              }
            }

            // Set the value.
            $value = $records->getColumnValue($base_table, $table_alias, $my_delta, $column_alias);
            $values[$field_name][$delta][$key]['value']->setValue($value);

            //if ($field_name == 'field_note') {
            //  dpm([$field_name, $delta, $key, '--', $base_table, $table_alias, $my_delta, $column_alias, $value]);
            //  dpm($value_col_info);
            //  dpm($values[$field_name][$delta][$key]['value']->getValue());
            //}

          }
        }
      }
    }
    //dpm($this->records->getRecordsArray());


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
   */
  protected function buildChadoRecords($values, bool $is_find = FALSE) {

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

          // Define the context array which will contain all details needed
          // for the buildChadoRecords() methods.
          $base_table = $storage_plugin_settings['base_table'];
          $context = [];
          $context['is_find'] = $is_find;
          $context['base_table'] = $base_table;
          $context['operation'] = array_key_exists('operation', $info) ? $info['operation'] : '=';
          $context['field_name'] = $field_name;
          $context['property_key'] = $key;
          $context['property_settings'] = $prop_storage_settings;
          $context['delta'] = $delta;

          // Get the path array for this field and add any joins if any are needed.
          if (array_key_exists('path', $prop_storage_settings)) {

            // First parse the path
            $path = $prop_storage_settings['path'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : '';
            $table_alias_mapping = array_key_exists('table_alias_mapping', $prop_storage_settings) ? $prop_storage_settings['table_alias_mapping'] : [];
            $path_array = $this->parsePath($base_table, $path, $table_alias_mapping, $as);

            // Add to the context.
            $context['path_string'] = $prop_storage_settings['path'];
            $context['path_array'] = $path_array;

            // We only add joins when the action is 'read_value' because
            // they guarantee a single value (meaning a 1:1 join). For
            // other joins there may be a many to one so we don't want to add
            // those joins off the base table.
            if ($action == 'read_value' and array_key_exists('join', $path_array)) {
              $this->handleJoins($path_array, $context);
            }
          }

          // Now for each action type, set the conditions and fields for
          // selecting chado records based on the other properties supplied.
          switch ($action) {
            case 'store_id':
              $this->handleStoreID($context, $prop_value);
              break;
            case 'store_pkey':
              $this->handleStorePkey($context, $prop_value);
              break;
            case 'store_link':
              $this->handleStoreLink($context, $prop_value);
              break;
            case 'store':
              $this->handleStore($context, $prop_value);
              break;
            case 'read_value':
              $this->handleReadValue($context, $prop_value);
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

    // Set some debugging info.
    $this->field_debugger->summarizeBuiltRecords($this->records);
  }

  /**
   * A helper function for the buildChadoRecords() function.
   *
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
   *   The field/property context provided by the buildChadoRecords() function.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function handleStoreID(array $context, StoragePropertyValue $prop_value) {

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
      'value' => $record_id > 0 ? $record_id : NULL,
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
    $this->records->addColumn($elements, TRUE);

    // Set the field and the condition if we have a record_id.
    if ($record_id > 0) {
      $this->records->addCondition($elements);
    }
  }

  /**
   * A helper function for the buildChadoRecords() function.
   *
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
   *   The field/property context provided by the buildChadoRecords() function.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function handleStorePkey(array $context, StoragePropertyValue $prop_value) {

    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $pkey_id = $prop_value->getValue();
    $elements = [
      'base_table' => $context['base_table'],
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $pkey_id ? $pkey_id : 0,
    ];
    $this->records->addColumn($elements);

    if ($pkey_id) {
      $elements['operation'] = $context['operation'];
      $this->records->addCondition($elements);
    }
  }

  /**
   *
   * A helper function for the buildChadoRecords() function.
   *
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
   *   The field/property context provided by the buildChadoRecords() function.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function handleStoreLink(array $context, StoragePropertyValue $prop_value) {

    $base_table = $context['base_table'];
    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $link_id = $this->records->getRecordID($base_table);
    $elements = [
      'base_table' => $base_table,
      'chado_table' => $value_col_info['chado_table'],
      'table_alias' => $value_col_info['table_alias'],
      'chado_column' => $value_col_info['chado_column'],
      'column_alias' => $value_col_info['column_alias'],
      // Setting the value to NULL and indicating this field contains a link
      // to the base table will cause the value to be set automatically by
      // ChadoRecord once it's available.
      'value' => $link_id ? $link_id : 0,
      'operation' => $context['operation'],
      'delta' => $context['delta'],
    ];
    $this->records->addColumn($elements, TRUE);
    $this->records->addCondition($elements);
  }

  /**
   *
   * A helper function for the buildChadoRecords() function.
   *
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is store.
   *
   * STORE: indicates that the value of this property can be loaded and
   * stored in the Chado table indicated by this property.
   *
   * @param array $context
   *   The field/property context provided by the buildChadoRecords() function.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function handleStore(array $context, StoragePropertyValue $prop_value) {

    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $value = $prop_value->getValue();
    if (is_string($value)) {
      $value = trim($value);
    }
    $elements = [
      'base_table' => $context['base_table'],
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

    $this->records->addColumn($elements);

    // If this is a find operation then we want to add a condition.
    if ($context['is_find'] and !empty($value)) {
      $this->records->addCondition($elements);
    }
  }

  /**
   * A helper function for the buildChadoRecords() function.
   *
   * Add chado record information for a specific ChadoStorageProperty
   * where the action is read_value.
   *
   * READ_VALUE: selecting a single column. This cannot be used for inserting or
   * updating values. Instead we use store actions for that.
   * If reading a value from a non-base table, then the path should
   * be provided.
   *
   * @param array $context
   *   The field/property context provided by the buildChadoRecords() function.
   * @param StoragePropertyValue $prop_value
   *   The value object for the property we are adding records for.
   *   Note: We will always have a StoragePropertyValue for a property even if
   *   the value is not set. This method is expected to check if the value is empty or not.
   */
  protected function handleReadValue(array $context, StoragePropertyValue $prop_value) {

    // Adding of fields via a join are handled by the ChadoRecord::setJoin() functino.
    if (array_key_exists('join', $context['path_array'])) {
      return;
    }
    $value = $prop_value->getValue();
    if (is_string($value)) {
      $value = trim($value);
    }
    $value_col_info = $this->getPathValueColumn($context['path_array']);
    $elements = [
      'base_table' => $value_col_info['base_table'],
      'chado_table' => $value_col_info['root_table'],
      'table_alias' => $value_col_info['root_alias'],
      'chado_column' => $value_col_info['chado_column'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $value ? $value : NULL,
      'operation' => $context['operation'],
    ];
    $this->records->addColumn($elements);

    // If this is a find operation then we want to add a condition on the
    // value.
    if ($context['is_find'] and !empty($value)) {
      $this->records->addCondition($elements);
    }
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
   * @param string $full_path
   *   This argument is used by recursion to build the string path for each level.
   *   It should not be set by the callee.
   * @return array
   *
   */
  protected function parsePath(string $base_table, mixed $path, array $aliases = [], string $as = '', string $full_path = '') {

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

    // The root table is the table at the beginning of the path.
    $root_alias = preg_replace('/^([^.;>]+?)\..*$/', '$1', $full_path);
    $root_table = $root_alias;
    if (array_key_exists($root_alias, $aliases)) {
      $root_table = $aliases[$root_alias];
    }

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
        'base_table'  => $base_table,
        'root_table' => $root_table,
        'root_alias' => $root_alias,
        'chado_table' => $left_table,
        'table_alias' => $left_alias,
        'join' => [
          'base_table'  => $base_table,
          'root_table' => $root_table,
          'root_alias' => $root_alias,
          'path' => $full_path,
          // The path string has no way to specify the type of join so
          // we'll default it to an 'outer' join.
          'type' => 'outer',
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
        $sub_path_arr = $this->parsePath($base_table, $path_arr, $aliases, $as, $full_path);
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

      // Get the table/column at the end.
      list($table_alias, $value_column) = explode(".", $path);
      $chado_table = $table_alias;
      if (array_key_exists($table_alias, $aliases)) {
        $chado_table = $aliases[$table_alias];
      }
      return [
        'path' => $full_path,
        'base_table' => $base_table,
        'root_table' => $root_table,
        'root_alias' => $root_alias,
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
        'base_table' => $base_table,
        'root_table' => $root_table,
        'root_alias' => $root_alias,
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
        'base_table' => $path['base_table'],
        'root_table' => $path['root_table'],
        'root_alias' => $path['root_alias'],
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
   * A helper function for the buildChadoRecords() function.
   *
   * Adds the joins to the ChadoRecord object.
   *
   * @param array $path_array
   *   The join path array
   * @param array $context
   *   The field/property context provided by the buildChadoRecords() function.
   */
  protected function handleJoins(array $path_array, array $context) {

    $elements = [
      'base_table' => $context['base_table'],
      'chado_table' => $path_array['root_table'],
      'table_alias' => $path_array['root_alias'],
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
    $this->records->addJoin($elements);

    // If there is another join then handle that.
    if (array_key_exists('join', $path_array['join'])) {
      $this->handleJoins($path_array['join'], $context);
    }

    // If we've reached the end of the joins we need to add the join columns.
    if (array_key_exists('value_column', $path_array['join'])) {
      $elements = [
        'base_table' => $context['base_table'],
        'chado_table' => $path_array['root_table'],
        'table_alias' => $path_array['root_alias'],
        'delta' => $context['delta'],
        'join_path' => $path_array['join']['path'],
        'chado_column' => $path_array['join']['value_column'],
        'column_alias' =>  $path_array['join']['value_alias'],
        'field_name' => $context['field_name'],
        'property_key' => $context['property_key'],
      ];
      $this->records->addJoinColumn($elements);
    }
  }

  /**
   *
   * {@inheritDoc}
   */
  public function validateValues($values) {

    $this->field_debugger->printHeader('Validate');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::validateValues');

    // Build the ChadoRecord object.
    $this->records = new ChadoRecords($this->field_debugger, $this->connection);
    $this->buildChadoRecords($values);

    // Validate the records.
    $violations = $this->records->validate();

    // Clear out the ChadoRecord object.
    $this->records = NULL;

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
