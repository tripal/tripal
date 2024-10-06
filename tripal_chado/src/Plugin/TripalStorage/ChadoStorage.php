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
   * this method is used to determine the parameters to pass to the constructor.
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
   * Implements __construct().
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
   * @param \Drupal\tripal_chado\Services\ChadoFieldDebugger $field_debugger
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
      $this->logger->notice('Debugging has been enabled for @name field.',
        ['@name' => $field_name],
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
    return $this->getStoredTypesFilter(TRUE);
  }

  /**
   *
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::getNonStoredTypes()
   */
  public function getNonStoredTypes() {
    return $this->getStoredTypesFilter(FALSE);
  }

  /**
   * Helper function for getStoredTypes() and getNonStoredTypes().
   *
   * @param bool $required
   *   TRUE to return types that are required.
   *   FALSE to return types that are not required.
   *
   * @return array
   *   Array of \Drupal\tripal\Base\StoragePropertyTypeBase objects.
   */
  protected function getStoredTypesFilter(bool $required) {
    $ret_types = [];
    foreach ($this->property_types as $field_name => $keys) {
      $field_definition = $this->field_definitions[$field_name];
      foreach ($keys as $key => $prop_type) {
        $storage_settings = $prop_type->getStorageSettings();

        // Any field that stores a base record id, a primary key,
        // or a foreign key link is required.
        $is_required = FALSE;
        if (($storage_settings['action'] == 'store_id') or
            ($storage_settings['action'] == 'store_pkey') or
            ($storage_settings['action'] == 'store_link')) {
          $is_required = TRUE;
        }
        // For any other fields that have 'drupal_store' set,
        // it is required too.
        elseif ((array_key_exists('drupal_store', $storage_settings)) and
                ($storage_settings['drupal_store'] === TRUE)) {
          $is_required = TRUE;
        }
        if (($is_required and $required) or (!$is_required and !$required)) {
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
    $this->records = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
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

      // Now that we've done the inserts, set the property values.
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
    $this->records = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
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

      // Now that we've done the updates, set the property values.
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
    $this->records = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
    $this->buildChadoRecords($values);

    $transaction_chado = $this->connection->startTransaction();
    try {

      $base_tables = $this->records->getBaseTables();
      foreach ($base_tables as $base_table) {

        // Do the select for the base tables
        $this->records->selectItems($base_table, $base_table);

        // Then do the selects for the ancillary tables.
        $tables = $this->records->getAncillaryTables($base_table);
        foreach ($tables as $table_alias) {
          $this->records->selectItems($base_table, $table_alias);
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

    // Setup field debugging.
    $this->field_debugger->printHeader('Delete');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::deleteValues');

    // Build the ChadoRecords object.
    $this->records = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
    $this->buildChadoRecords($values, TRUE);

    $transaction_chado = $this->connection->startTransaction();
    try {

      // Iterate through each base table.
      $base_tables = $this->records->getBaseTables();
      foreach ($base_tables as $base_table) {

        // First iterate through the ancillary tables and remove depenedent
        // records.
        $tables = $this->records->getAncillaryTablesWithCond($base_table);
        foreach ($tables as $table) {
          $this->records->deleteRecords($base_table, $table);
        }

        // Second, delete the record in the base talbe.
        $this->records->deleteRecords($base_table, $base_table);
      }

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
  public function findValues($values, $record_ids = []) {

    // Setup field debugging.
    $this->field_debugger->printHeader('Find');
    $this->field_debugger->summarizeChadoStorage($this, 'At the beginning of ChadoStorage::findValues');

    // Build the ChadoRecords object.
    $this->records = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
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
        $entity_matches = $this->records->findRecords($base_table, $base_table, $record_ids);

        // Now for each matching base record we need to select
        // the ancillary tables.
        foreach ($entity_matches as $match) {

          // Clone the value array for this match.
          $new_values = $this->cloneValues($values);

          // Limit base records by iterating through tables with conditions.
          $tables = $this->records->getAncillaryTablesWithCond($base_table);
          foreach ($tables as $table_alias) {


            // Now find any items for this linked table.
            $num_items_found = $match->selectItems($base_table, $table_alias);
            if ($num_items_found == 0) {
              continue;
            }

            // Prepare the values array to receive all the new values. We'll
            // get all the fields for this ancillary table and then
            // reset the values in the new cloned values array for all of
            // those fields.
            $table_fields = $match->getTableFields($base_table, $table_alias);
            foreach ($table_fields as $field_name) {
              for ($i = 0; $i < $num_items_found; $i++) {
                $this->resetValuesItem($new_values, $field_name, $i);
              }
            }
          }

          // Now set the values.
          $this->setPropValues($new_values, $match);

          // Remove any values that are not valid.
          foreach ($new_values as $field_name => $deltas) {
            foreach ($deltas as $delta => $properties) {
              $is_valid = $this->isFieldValid($field_name, $delta, $new_values);
              if (!$is_valid) {
                unset($new_values[$field_name][$delta]);
              }
            }
          }
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
   *   be passed in because the findValues() function can generate copies
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
            // Create a context array to pass information to the callback function.
            $context = [
              'field_name' => $field_name,
              'delta' => $delta,
              'key' => $key,
              'info' => $info,
              'prop_type' => $prop_type,
              'field_settings' => $field_settings,
            ];
            $function[] = $context;
          }
          else {

            // Parse the path.
            $base_table = $storage_plugin_settings['base_table'];
            $path = $prop_storage_settings['path'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : '';
            $table_alias_mapping = array_key_exists('table_alias_mapping', $prop_storage_settings) ? $prop_storage_settings['table_alias_mapping'] : [];
            $path_array = $this->parsePath($field_name, $base_table, $path, $table_alias_mapping, $as);

            // Get the value column information for this property.
            $base_table = $storage_plugin_settings['base_table'];
            $value_col_info = $this->getPathValueColumn($path_array);
            $table_alias  = $value_col_info['table_alias'];
            $column_alias  = $value_col_info['column_alias'];

            // For values that come from joins, we need to use the root table
            // because this is the table that will have the value.
            if ($action == 'read_value' and array_key_exists('join', $path_array)) {
              $root_alias = $value_col_info['root_alias'];
              $table_alias = $root_alias;
            }

            // Anytime we need to pull a value from the base table, the delta
            // should always be zero. There will only ever be one base record.
            // This is needed because all fields use a `record_id` which has
            // a path that is set for the base table.
            $value_delta = $delta;
            if ($table_alias == $base_table) {
              $value_delta = 0;
            }

            $value = $records->getColumnValue($base_table, $table_alias, $value_delta, $column_alias);
            if ($value !== NULL) {
              $values[$field_name][$delta][$key]['value']->setValue($value);
            }
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
    foreach ($function as $context) {
      // Add current values to the context so that a function
      // can access other non-function fields if it needs to.
      $context['values'] = $values;

      // Retrieve the needed keys for the $values array
      $field_name = $context['field_name'];
      $delta = $context['delta'];
      $key = $context['key'];

      // Retrieve the call back function
      $prop_storage_settings = $context['prop_type']->getStorageSettings();
      $namespace = $prop_storage_settings['namespace'];
      $callback_function = $prop_storage_settings['function'];

      // Validate the callback function and then call it to generate a value.
      $value = NULL;
      if (method_exists($namespace, $callback_function)) {
        $value = call_user_func($namespace . '::' . $callback_function, $context);
      }
      else {
        $this->logger->error('Callback function for field @field does not exist: @namespace::@function.',
          ['@field' => $field_name, '@namespace' => $namespace, '@function' => $callback_function]
        );
      }

      if ($value !== NULL && is_string($value)) {
        $values[$field_name][$delta][$key]['value']->setValue(trim($value));
      }
      else {
        $values[$field_name][$delta][$key]['value']->setValue($value);
      }
    }
  }


  /**
   * Checks if a field has all necessary elements to be considered 'found'.
   *
   * The ChadoRecords class will search for all records necessary to
   * populate the values of the fields for a content type. This works well
   * when all conditions are set for the insertValues() and loadValues().
   * However, for the findValues() function there are often no criteria set
   * and we want to find all linked records associated with a base record.
   * All Chado fields will have a `record_id` property and the value of that
   * comes from the base table.  This means that all fields will have at
   * least one property set even if nothing was found. So we need to know
   * if the field has a valid set of property values. If so, we can
   * proceed as if the field was "found" otherwise, we should remove the
   * field values as nothing was found.
   *
   * A field is valid if all of the properties that have an action of 'store'
   * have a non NULL value and if all required properties have a non NULL value.
   *
   * @param string $field_name
   *   The name of the field.
   * @param integer $delta
   *   The field item's delta value.
   * @param array $values
   *  An array of field values.
   * @return boolean
   *   returns TRUE if the field has all necessary elements for inserting
   *   into the Drupal tables for publishing. FALSE otherwise.
   */
  protected function isFieldValid($field_name, $delta, $values) {

    foreach ($values[$field_name][$delta] as $key => $prop_value) {
      /** @var \Drupal\tripal\TripalStorage\StoragePropertyTypeBase $prop_type **/
      $prop_type = $this->getPropertyType($field_name, $key);
      $prop_settings = $prop_type->getStorageSettings();
      $action = $prop_settings['action'];
      $is_store = preg_match('/^store/', $action);
      $value = $prop_value['value']->getValue();
      $is_required = $prop_type->getRequired();
      if ($is_store and $value === NULL) {
        return FALSE;
      }
      if ($is_required and $value === NULL) {
        return FALSE;
      }
    }
    return TRUE;
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
          $context['root_table'] = $base_table;
          $context['root_alias'] = $base_table;
          $context['operation'] = array_key_exists('operation', $info) ? $info['operation'] : '=';
          $context['field_name'] = $field_name;
          $context['property_key'] = $key;
          $context['property_settings'] = $prop_storage_settings;
          $context['delta'] = $delta;
          $context['action'] = $action;


          // Get the path array for this field and add any joins if any are needed.
          if (array_key_exists('path', $prop_storage_settings)) {

            // First parse the path
            $path = $prop_storage_settings['path'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : '';
            $table_alias_mapping = array_key_exists('table_alias_mapping', $prop_storage_settings) ? $prop_storage_settings['table_alias_mapping'] : [];
            $path_array = $this->parsePath($field_name, $base_table, $path, $table_alias_mapping, $as);

            // The path will have the root table. This may or may not be the
            // same as the base table so we should track it.
            $context['root_table'] = $path_array['root_table'];
            $context['root_alias'] = $path_array['root_alias'];

            // We only add joins when the action is 'read_value' because
            // they guarantee a single value (meaning a 1:1 join). For
            // other joins there may be a many to one so we don't want to add
            // those joins off the base table.
            if ($action == 'read_value' and array_key_exists('join', $path_array)) {
              $this->handleJoins($path_array, $context);
            }

            // Add to the context.
            $context['path_string'] = $prop_storage_settings['path'];
            $context['path_array'] = $path_array;
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
      'base_table' => $base_table,
      'root_table' => $context['root_table'],
      'root_alias' => $context['root_alias'],
      'chado_table' => $value_col_info['chado_table'],
      'table_alias' => $value_col_info['table_alias'],
      'chado_column' => $value_col_info['chado_column'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $record_id > 0 ? $record_id : NULL,
      'operation' => $context['operation'],
      'field_name' => $context['field_name'],
      'property_key' => $context['property_key'],
    ];

    // The store_id action should only be used for the base table...
    // @todo: I think these checks should go into a field validation test rather than here.
    if ($elements['chado_table'] !== $elements['base_table']) {
      $this->logger->error($this->t('The @field.@key property type uses the '
        . 'store_id action type but is not associated with the base table of the field. '
        . 'Either change the base_table of this field or use store_pkey instead.  @chado_table != @base_table',
         ['@field' => $context['field_name'],
          '@key' => $context['property_key'],
          '@base_table' => $elements['base_table'],
          '@chado_table' => $elements['chado_table']
        ]));
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
      'root_table' => $context['root_table'],
      'root_alias' => $context['root_alias'],
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $pkey_id ? $pkey_id : NULL,
      'field_name' => $context['field_name'],
      'property_key' => $context['property_key'],
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
      'root_table' => $context['root_table'],
      'root_alias' => $context['root_alias'],
      'chado_table' => $value_col_info['chado_table'],
      'table_alias' => $value_col_info['table_alias'],
      'chado_column' => $value_col_info['chado_column'],
      'column_alias' => $value_col_info['column_alias'],
      // Setting the value to NULL and indicating this field contains a link
      // to the base table will cause the value to be set automatically by
      // ChadoRecord once it's available.
      'value' => $link_id ? $link_id : NULL,
      'operation' => $context['operation'],
      'delta' => $context['delta'],
      'field_name' => $context['field_name'],
      'property_key' => $context['property_key'],
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
      'root_table' => $context['root_table'],
      'root_alias' => $context['root_alias'],
      'chado_table' => $value_col_info['chado_table'],
      'chado_column' => $value_col_info['chado_column'],
      'table_alias' => $value_col_info['table_alias'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $value,
      'operation' => $context['operation'],
      'delete_if_empty' => array_key_exists('delete_if_empty', $context['property_settings']) ? $context['property_settings']['delete_if_empty'] : FALSE,
      'empty_value' => array_key_exists('empty_value', $context['property_settings']) ? $context['property_settings']['empty_value'] : '',
      'field_name' => $context['field_name'],
      'property_key' => $context['property_key'],
    ];

    $this->records->addColumn($elements);

    // If this is a find operation then we want to add a condition.
    if ($context['is_find'] and $value !== NULL) {
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
      'base_table' => $context['base_table'],
      'root_table' => $context['root_table'],
      'root_alias' => $context['root_alias'],
      'chado_table' => $value_col_info['root_table'],
      'table_alias' => $value_col_info['root_alias'],
      'chado_column' => $value_col_info['chado_column'],
      'column_alias' => $value_col_info['column_alias'],
      'delta' => $context['delta'],
      'value' => $value ? $value : NULL,
      'operation' => $context['operation'],
      'field_name' => $context['field_name'],
      'property_key' => $context['property_key'],
    ];
    $this->records->addColumn($elements, FALSE, TRUE);
  }

  /**
   * Takes a path string for a field property and converts it to an array structure.
   *
   * @param string $field_name
   *   The name of the field.
   * @param string $base_table
   *   The name of the base table for thie field.
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
  protected function parsePath(string $field_name, string $base_table, mixed $path,
      array $aliases = [], string $as = '', string $full_path = '') {

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
        $sub_path_arr = $this->parsePath($field_name, $base_table, $path_arr, $aliases, $as, $full_path);
      }
      // If there are no more joins, then we need to set the value column to be
      // the same as the last column in the join.
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

      // If the base table is not the same as the root table then
      // we should add the field name to the colun alias. Otherwise
      // we may have conflicts if mutiple fields use the same alias.
      $value_alias = $as ? $as : $value_column;
      if ($base_table != $root_table) {
        $value_alias = $field_name . '__' . $value_alias;
      }
      return [
        'path' => $full_path,
        'base_table' => $base_table,
        'root_table' => $root_table,
        'root_alias' => $root_alias,
        'chado_table' => $chado_table,
        'table_alias' => $table_alias,
        'value_column' => $value_column,
        'value_alias' => $value_alias
      ];
    }

    // There is no period in the path so there is no Chado table. We are at the
    // end of the path with joins and the value column is not the same as the
    // right join column. We can just return the value column.
    else {
      // If the base table is not the same as the root table then
      // we should add the field name to the colun alias. Otherwise
      // we may have conflicts if mutiple fields use the same alias.
      $value_alias = $as ? $as : $curr_path;
      $value_alias = $field_name . '__' . $value_alias;
      return [
        'base_table' => $base_table,
        'root_table' => $root_table,
        'root_alias' => $root_alias,
        'value_column' => $curr_path,
        'value_alias' => $value_alias
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
  protected function handleJoins(array &$path_array, array $context) {

    $elements = [
      'base_table' => $context['base_table'],
      'root_table' => $context['root_table'],
      'root_alias' => $context['root_alias'],
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
    $path_array['join']['table_alias'] = $elements['right_alias'];

    // If there is another join then handle that.
    if (array_key_exists('join', $path_array['join'])) {
      $join_path = $path_array['join'];
      $this->handleJoins($join_path, $context);
      $path_array['join'] = $join_path;
    }

    // If we've reached the end of the joins we need to add the join columns.
    if (array_key_exists('value_column', $path_array['join'])) {
      $elements = [
        'base_table' => $context['base_table'],
        'root_table' => $context['root_table'],
        'root_alias' => $context['root_alias'],
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
    $this->records = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
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

  /**
   * Returns a list of all pkey_id values for a given base table.
   *
   * @param string $bundle_id
   *   The name of the bundle.
   *
   * @return array
   *   List of pkey_id values in no particular order.
   */
  public function findAllRecordIds(string $bundle_id) {
    $records = [];

    // Retrieve relevant information from the bundle
    $entity_type_manager = \Drupal::entityTypeManager();
    $entity_type = $entity_type_manager->getStorage('tripal_entity_type')->load($bundle_id);
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $type_table = $entity_type->getThirdPartySetting('tripal', 'chado_type_table');
    $type_column = $entity_type->getThirdPartySetting('tripal', 'chado_type_column');
    $termIdSpace = $entity_type->getTermIdSpace();
    $termAccession = $entity_type->getTermAccession();

    // Get the name of the primary key column.
    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
    $pkey_column = $table_def['primary key'];

    // Set up the query
    $query = $this->connection->select('1:' . $base_table, 'BT', []);
    $query->addField('BT', $pkey_column, 'pkey');

    // If there is a type setting, add this as a condition to
    // limit records to only those of this type.
    // For example, only 'gene' SO:0000704 records from the 'feature' table.
    if ($type_table and $type_column) {
      if ($type_table == $base_table) {
        $query->join('1:cvterm', 'T', '"BT".' . $type_column . ' = "T".cvterm_id');
      }
      else {
        $query->join('1:' . $type_table, 'TT', '"BT".' . $pkey_column . ' = "TT".' . $pkey_column);
        $query->join('1:cvterm', 'T', '"TT".' . $type_column . ' = "T".cvterm_id');
      }
      $query->join('1:dbxref', 'X', '"T".dbxref_id = "X".dbxref_id');
      $query->join('1:db', 'DB', '"X".db_id = "DB".db_id');
      $query->condition('X.accession', $termAccession, '=');
      $query->condition('DB.name', $termIdSpace, '=');
    }

    // Retrieve results, i.e. record IDs.
    $results = $query->execute();
    if ($results) {
      while ($pkey_id = $results->fetchField()) {
        $records[] = $pkey_id;
      }
    }
    return $records;
  }

  /**
   * A callback function to allow linking fields to include the Drupal entity ID.
   *
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   * @return int
   *   The Drupal entity ID, or -1 if it doesn't exist.
   *   We use -1 because Tripal preSave will flag a zero for deletion.
   */
  static public function drupalEntityIdLookupCallback($context) {

    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
    $delta = $context['delta'];
    $field_name = $context['field_name'];

    // Get the name of the primary key column of the Chado table that
    // the entity is based on, which is a foreign key for whatever the
    // current content type is. Because this callback handles all fields,
    // it doesn't know what that is, so we need to have that saved in the
    // field properties.
    $prop_storage_settings = $context['prop_type']->getStorageSettings();
    $fkey = $prop_storage_settings['fkey'] ?? NULL;
    if (!$fkey) {
      // Maybe throw an exception here so developers know they forgot the 'fkey'
      return -1;
    }

    $record_id = $context['values'][$field_name][$delta][$fkey]['value'] ?? NULL;
    if (!$record_id) {
      return -1;
    }
    $record_id = $record_id->getValue('value');

    if (!$record_id) {
      return -1;
    }

    // Given the Chado record ID and bundle term, we can lookup the Drupal entity ID.
    $ftable = $prop_storage_settings['ftable'] ?? NULL;
    $entity_id = $lookup_manager->getEntityId(
      $record_id,
      $context['field_settings']['termIdSpace'],
      $context['field_settings']['termAccession'],
      $ftable
    );

    // In the TripalEntity class, the preSave function will flag all falsey
    // property values for deletion when drupal_store is set to TRUE.
    // To get around this, indicate the lack of a Drupal entity with a -1.
    if (!$entity_id) {
      $entity_id = -1;
    }

    return $entity_id;
  }
}
