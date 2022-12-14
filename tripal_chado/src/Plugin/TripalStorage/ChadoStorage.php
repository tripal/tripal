<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Symfony\Component\Validator\ConstraintViolation;



/**
 * Chado implementation of the TripalStorageInterface.
 *
 * @TripalStorage(
 *   id = "chado_storage",
 *   label = @Translation("Chado Storage"),
 *   description = @Translation("Interfaces with GMOD Chado for field values."),
 * )
 */
class ChadoStorage extends PluginBase implements TripalStorageInterface {

  /**
   * An associative array that contains all of the property types that
   * have been added to this object. It is indexed by entityType ->
   * fieldName -> key and the value is the
   * Drupal\tripal\TripalStoreage\StoragePropertyValue object.
   *
   * @var array
   */
  protected $property_types = [];

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
   * Retrieve a Chado database connection.
   */
  protected function getChadoConnection() {
    $chado = \Drupal::service('tripal_chado.database');
    $chado->useTripalDbxSchemaFor(get_class());
    return $chado;
  }

	/**
	 * @{inheritdoc}
	 */
  public function addTypes($types) {
    $logger = \Drupal::service('tripal.logger');

    // Index the types by their entity type, field type and key.
    foreach ($types as $index => $type) {
      if (!is_object($type) OR !is_subclass_of($type, 'Drupal\tripal\TripalStorage\StoragePropertyTypeBase')) {
        $logger->error('Type provided must be an object extending StoragePropertyTypeBase. Instead index @index was this: @type',
            ['@index' => $index, '@type' => print_r($type, TRUE)]);
        return FALSE;
      }

      $field_name = $type->getFieldType();
      $entity_type = $type->getEntityType();
      $key = $type->getKey();

      if (!array_key_exists($entity_type, $this->property_types)) {
        $this->property_types[$entity_type] = [];
      }
      if (!array_key_exists($field_name, $this->property_types[$entity_type])) {
        $this->property_types[$entity_type][$field_name] = [];
      }
      if (array_key_exists($key, $this->property_types[$entity_type])) {
        $logger->error('Cannot add a property type, "@prop", as it already exists',
            ['@prop' => $entity_type . '.' . $field_name . '.' . $key]);
        return FALSE;
      }
      $this->property_types[$entity_type][$field_name][$key] = $type;
    }
  }

  /**
   * @{inheritdoc}
   */
  public function getTypes() {
    $types = [];
    foreach ($this->property_types as $field_types) {
      foreach ($field_types as $keys) {
        foreach ($keys as $type) {
          $types[] = $type;
        }
      }
    }
    return $types;
  }

  /**
	 * @{inheritdoc}
	 */
  public function removeTypes($types) {

    foreach ($types as $type) {
      $entity_type = $type->getEntityType();
      $field_type = $type->getFieldType();
      $key = $type->getKey();
      if (array_key_exists($entity_type, $this->property_types)) {
        if (array_key_exists($field_type, $this->property_types[$entity_type])) {
          if (array_key_exists($key, $this->property_types[$entity_type])) {
            unset($this->property_types[$entity_type][$field_type][$key]);
          }
        }
      }
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

    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Insert the record.
    $insert = $chado->insert('1:'.$chado_table);
    $insert->fields($record['fields']);
    $record_id = $insert->execute();
    if (!$record_id) {
      throw new \Exception($this->t('Failed to insert a record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Update the record array to include the record id.
    $records[$chado_table][$delta]['conditions'][$pkey] = $record_id;
    return $record_id;
  }

  /**
	 * @{inheritdoc}
	 */
  public function insertValues(&$values) : bool {
    $chado = $this->getChadoConnection();
    $logger = \Drupal::service('tripal.logger');
    $schema = $chado->schema();

    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];

    $transaction_chado = $chado->startTransaction();
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
      if (!empty($cond_value)) {
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
    $chado = $this->getChadoConnection();

    // Don't update if we don't have any conditions set.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
        ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    $update = $chado->update('1:'.$chado_table);
    $update->fields($record['fields']);
    foreach ($record['conditions'] as $chado_column => $cond_value) {
      $update->condition($chado_column, $cond_value);
    }
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
    $chado = $this->getChadoConnection();
    $logger = \Drupal::service('tripal.logger');

    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];
    $base_tables = $build['base_tables'];

    $transaction_chado = $chado->startTransaction();
    try {

      // Handle base table records first.
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          if (!array_key_exists('conditions', $record)) {
            throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to missing conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }

          // If this is the base table then do an update.
          if (in_array($chado_table, array_keys($base_tables))) {
            $this->updateChadoRecord($records, $chado_table, $delta, $record);
            continue;
          }
        }
      }

      // Next delete all non base records so we can replace them
      // with updates. This is necessary because we may violoate unique
      // contraints if we don't e.g. changing the order of records with a
      // rank.
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Skip base table records.
          if (in_array($chado_table, array_keys($base_tables))) {
            continue;
          }

          // Skip records that are already marked for insertion.
          if (!$this->hasValidConditions($record)) {
            continue;
          }

          $this->deleteChadoRecord($records, $chado_table, $delta, $record);
        }
      }

      // Now insert all new values for nor the non-base table records.
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
    $chado = $this->getChadoConnection();

    if (!array_key_exists('conditions', $record)) {
      throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // If we are selecting on the base table and we don't have a proper
    // condition then throw and error.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    // Select the fields in the chado table.
    $select = $chado->select('1:'.$chado_table, 'ct');
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
      if (!empty($value)) {
        $select->condition('ct.'.$chado_column, $value);
      }
    }

    // Execute the query.
    $results = $select->execute();
    if (!$results) {
      throw new \Exception($this->t('Failed to select record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }
    $records[$chado_table][$delta] = $results->fetchAssoc();
  }

  /**
   * @{inheritdoc}
   */
  public function loadValues(&$values) : bool {
    $chado = $this->getChadoConnection();
    $logger = \Drupal::service('tripal.logger');

    $build = $this->buildChadoRecords($values, FALSE);
    $records = $build['records'];
    $base_tables = $build['base_tables'];

    $transaction_chado = $chado->startTransaction();
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
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Don't delete if we don't have any conditions set.
    if (!$this->hasValidConditions($record)) {
      throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
    }

    $delete = $chado->delete('1:'.$chado_table);
    foreach ($record['conditions'] as $chado_column => $cond_value) {
      $delete->condition($chado_column, $cond_value);
    }
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
    $records[$chado_table][$delta]['conditions'][$pkey] = 0;
  }

  /**
   * @{inheritdoc}
   */
  public function deleteValues($values) : bool {

    return FALSE;
  }

  /**
   * @{inheritdoc}
   */
  public function findValues($match) {

  }


  /**
   * Sets the record_id properties after an insert.
   *
  * @param array $values
  *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
  *
  * @param array $records
  *   The set of Chado records.
  */
  protected function setRecordIds(&$values, $records) {
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {
          $definition = $info['definition'];
          $prop_type = $info['type'];

          // Get the field and property storage settings.
          $field_settings = $definition->getSettings();
          $field_storage_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();

          // Get the base table information.
          $base_table = $field_storage_settings['base_table'];
          $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
          $base_pkey = $base_table_def['primary key'];

          // Get the Chado table information. If one is not specified (as
          // in the case of single value fields) then default to the base
          // table.
          $chado_table = $base_table;
          $chado_table_def = $base_table_def;
          $pkey = $base_pkey;
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
            $pkey = $chado_table_def['primary key'];
          }

          // Skip values we don't have tables or deltas for in the record array.
          if (!in_array($chado_table, array_keys($records)) or
              !in_array($delta, array_keys($records[$chado_table]))) {
            continue;
          }

          // If this is the record_id property then set it's value.
          if ($key == 'record_id') {
            $record_id = $records[$chado_table][$delta]['conditions'][$pkey];
            $values[$field_name][$delta][$key]['value']->setValue($record_id);
          }
          
          // If this is a property managing a linked record ID then set it too.
          if (array_key_exists('action', $prop_storage_settings)) {
            if ($prop_storage_settings['action'] == 'link') {
              $linked_record_id = $records[$chado_table][$delta]['conditions'][$pkey];
              $values[$field_name][$delta][$key]['value']->setValue($linked_record_id);
            }
          }
        }
      }
    }
  }

  /**
   * Sets the property values using the recrods returned from Chado.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @param array $records
   *   The set of Chado records.
   */
  protected function setPropValues(&$values, $records) {

    $replace = [];
    $function = [];

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {
          if ($key == 'record_id') {
            continue;
          }

          $prop_type = $info['type'];
          $prop_storage_settings = $prop_type->getStorageSettings();
          $action = $prop_storage_settings['action'];

          // Get the values of properties that can be stored.
          if ($action == 'store') {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_column = $prop_storage_settings['chado_column'];
            if (array_key_exists($chado_table, $records)) {
              if (array_key_exists($delta, $records[$chado_table])) {
                if (array_key_exists($chado_column, $records[$chado_table][$delta])) {
                  $value = $records[$chado_table][$delta][$chado_column];
                  $values[$field_name][$delta][$key]['value']->setValue($value);
                }
              }
            }
          }

          // Get the values of properties that have values added by a join.
          if ($action == 'join') {
            $chado_column = $prop_storage_settings['chado_column'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : $chado_column;
            $value = $records[$chado_table][$delta][$as];
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
      $prop_type = $info['type'];
      $prop_storage_settings = $prop_type->getStorageSettings();
      $value = $prop_storage_settings['template'];

      $matches = [];
      if (preg_match_all('/\[(.+?\:.+?)\]/', $value, $matches)) {
        foreach ($matches[1] as $match) {
          $match_clean = preg_replace('/:/', '_', $match);
          if (array_key_exists($match_clean, $values[$field_name][$delta])) {
            $match_value = $values[$field_name][$delta][$match_clean]['value']->getValue();
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
      $prop_type = $info['type'];
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
   *   When the function returns, any values retreived from the data store
   *   will be set in the StoragePropertyValue object.
   * @param bool $is_store
   *   Set to TRUE if we are building the record array for an insert or an
   *   update.
   * @return array
   *   An associative array.
   */
  protected function buildChadoRecords($values, bool $is_store) {
    $logger = \Drupal::service('tripal.logger');
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $records = [];
    $base_record_ids = [];

    // @debug dpm(array_keys($values), '1st level: field names');

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      // @debug dpm(array_keys($deltas), "2nd level: deltas ($field_name)");
      foreach ($deltas as $delta => $keys) {
        // @debug dpm(array_keys($keys), "3rd level: field key name ($delta)");
        foreach ($keys as $key => $info) {

          // @debug dpm(array_keys($info), "4th level: info key-value pairs ($key)");
          if (!array_key_exists('definition', $info) OR !is_object($info['definition'])) {
            $logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the field definition (i.e. FieldConfig object). There should be a "definition" key in this array: @var',
              ['@field' => $field_name, '@var' => print_r($info, TRUE)]));
            continue;
          }
          if (!array_key_exists('value', $info) OR !is_object($info['value'])) {
            $logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the StoragePropertyValue object.',
              ['@field' => $field_name]));
            continue;
          }

          // @debug ksm($info['definition'], "$key: DEFINITION");
          // @debug ksm($info['type'], "$key: TYPE");
          // @debug ksm($info['value'], "$key: VALUES");
          $definition = $info['definition'];
          $prop_type = $info['type'];
          $prop_value = $info['value'];

          $field_label = $definition->getLabel();
          $field_settings = $definition->getSettings();
          $storage_plugin_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();

          // Check that the chado table is set.
          if (!array_key_exists('base_table', $storage_plugin_settings)) {
            $logger->error($this->t('Cannot store the property, @field.@prop, in Chado. The field is missing the chado base table name.',
                ['@field' => $field_name, '@prop' => $key]));
            continue;
          }

          // Get the base table definitions.
          $base_table = $storage_plugin_settings['base_table'];
          $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
          $base_pkey = $base_table_def['primary key'];

          // Get the Chado table. Use the base table if one is not provided.
          $chado_table = $base_table;
          $chado_table_def = $base_table_def;
          $pkey = $base_pkey;
          $fk_col = NULL;
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
            $pkey = $chado_table_def['primary key'];
            if ($chado_table != $base_table) {
              $fk_col = array_keys($chado_table_def['foreign keys'][$base_table]['columns'])[0];
            }
          }


          // If this is the record ID then keep track of it. This will only
          // be present if we're performing a load or update.
          if ($key == 'record_id') {
            $record_id = $prop_value->getValue();
            $records[$chado_table][$delta]['conditions'][$pkey] = $record_id;
            if ($chado_table == $base_table) {
              $base_record_ids[$base_table] = $record_id;
            }
            continue;
          }

          // Make sure we have an action for this property.
          if (!array_key_exists('action', $prop_storage_settings)) {
            $logger->error($this->t('Cannot store the property, @field.@prop ("@label"), in Chado. The property is missing an action in the property settings: @settings',
                ['@field' => $field_name, '@prop' => $key,
                 '@label' => $field_label, '@settings' => print_r($prop_storage_settings, TRUE)]));
            continue;
          }
          $action = $prop_storage_settings['action'];

          // An action of "store" means that this value can be loaded/stored
          // in the Chado table for the field.
          if ($action == 'store') {
            $chado_column = $prop_storage_settings['chado_column'];
            $value = $prop_value->getValue();
            if (is_string($value)) {
              $value = trim($value);
            }
            // If this column is the foreign key column to the base table then
            // we need to replace it with the record ID. But we can't gurantee
            // that fields come in order. So we'll leave a reminder token to
            // replace it later.
            if ($fk_col == $chado_column) {
              $records[$chado_table][$delta]['fields'][$chado_column] = ['REPLACE_BASE_RECORD_ID', $base_table];
            }
            else {
              $records[$chado_table][$delta]['fields'][$chado_column] = $value;
            }

            // If this field should not allow an empty value that means this
            // entire record should be removed on an update and not inserted.
            $delete_if_empty = array_key_exists('delete_if_empty',$prop_storage_settings) ? $prop_storage_settings['delete_if_empty'] : FALSE;
            if ($delete_if_empty) {
              $records[$chado_table][$delta]['delete_if_empty'][] = $key;
            }
          }
          if ($action == 'link') {            
            // Set the value for the FK link in the records.
            $chado_column = $prop_storage_settings['chado_column'];
            $link_column = $prop_storage_settings['link_column'];
            $records[$chado_table][$delta]['fields'][$link_column] = ['REPLACE_BASE_RECORD_ID', $base_table];
            // Set a condition for the linked record.
            $records[$chado_table][$delta]['conditions'][$chado_column] = $prop_value->getValue();            
          }
          if ($action == 'join') {
            $path = $prop_storage_settings['path'];
            $chado_column = $prop_storage_settings['chado_column'];
            $as = array_key_exists('as', $prop_storage_settings) ? $prop_storage_settings['as'] : $chado_column;
            $path_arr = explode(";", $path);
            $this->addChadoRecordJoins($records, $chado_column, $as, $delta, $path_arr);
          }
          if ($action == 'replace') {
            // Do nothing here for properties that need replacement.
          }
          if ($action == 'function') {
            // Do nothing here for properties that require post-processing
            // with a function.
          }
        }
      }
    }

    // Iterate through the records and set any record IDs for FK relationships.
    foreach ($records as $table_name => $deltas) {
      foreach ($deltas as $delta => $info) {
        foreach ($info['fields'] as $chado_column => $val) {
          if (is_array($val) and $val[0] == 'REPLACE_BASE_RECORD_ID') {

            // If the base record ID is 0 then this is an insert and we
            // don't yet have the base record ID.  So, leave in the message
            // to replace the ID so we can do so later.
            if ($base_record_ids[$val[1]] !== 0) {
              $records[$table_name][$delta]['fields'][$chado_column] = $base_record_ids[$val[1]];
            }
          }
        }
      }
    }

    return [
      'base_tables' => $base_record_ids,
      'records' => $records
    ];
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

    // Get then current number of joins to the right table.
    $num_left = 0;
    if (array_key_exists($left_table,$records[$parent_table][$delta]['joins'])) {
      $num_left = count($records[$parent_table][$delta]['joins'][$left_table]) - 1;
    }
    $num_right = count($records[$parent_table][$delta]['joins'][$right_table]) - 1;

    // Generate aliases for the left and right tables in the join.
    $lalias = $depth == 0 ? 'ct' : 'j' . $left_table . $num_left;
    $ralias = 'j' . $right_table . $num_right;
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();

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
   * @param array $violoations
   *   An array to which any new violoations can be added.
   */
  private function validateRequired($values, $chado_table, $record_id, $record, &$violations) {
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
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
      // Documentation for how to creata violation is here
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
   * @param array $violoations
   *   An array to which any new violoations can be added.
   */
  private function validateUnique($values, $chado_table, $record_id, $record, &$violations) {
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    // Check if we are violoating a unique contraint (if it's an insert)
    if (array_key_exists('unique keys',  $table_def)) {
      $pkey = $table_def['primary key'];

      // Iterate through the unique constraints and see if the record
      // violates it.
      $ukeys = $table_def['unique keys'];
      foreach ($ukeys as $ukey_name => $ukey_cols) {
        $ukey_cols = explode(',', $ukey_cols);
        $query = $chado->select('1:'.$chado_table, 'ct');
        $query->fields('ct');
        foreach ($ukey_cols as $col) {
          $col = trim($col);
          $col_val = NULL;
          if (array_key_exists($col, $record['fields'])) {
            $col_val = $record['fields'][$col];
          }
          // There is an issue with postgreSQL that if the value is allowed
          // to be null but it is in a unique constraint then it will allow
          // it to be inserted because a Null != NULL. So, skip checking
          // these columns in the unique constraint if they are empty.
          if ($table_def['fields'][$col]['not null'] == FALSE and !$col_val) {
            continue;
          }
          $query->condition($col, $col_val);
        }

        // If we have maatching record, check for a unique constraint
        // violation.
        $match = $query->execute()->fetchObject();
        if ($match) {

          // Add a constraint violoation if we have a match and the
          // record_id is 0. This would be an insert but a record already
          // exists. Or, if the record_id isn't the same as the  matched
          // record. This is an update that conflicts with an existing
          // record.
          if (($record_id == 0) or ($record_id != $match->$pkey)) {
            // Documentation for how to creata violation is here
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
   * @param array $violoations
   *   An array to which any new violoations can be added.
   */
  private function validateFKs($values, $chado_table, $record_id, $record, &$violations) {
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_fks = [];
    if (!array_key_exists('foreign keys', $table_def)) {
      return;
    }
    $fkeys = $table_def['foreign keys'];    
    foreach ($fkeys as $fk_table => $info) {
      foreach ($info['columns'] as $lcol => $rcol) {

        // If an FK allows nulls and the value is null then skip this one..
        $col_val = $record['fields'][$lcol];
        if ($table_def['fields'][$lcol]['not null'] == FALSE and !$col_val) {
          continue;
        }

        // Check if the  id is present in the FK talbe.
        $query = $chado->select($fk_table, 'fk');
        $query->fields('fk', [$rcol]);
        $query->condition($rcol, $col_val);
        $fk_id = $query->execute()->fetchField();
        if (!$fk_id) {
          $bad_fks[] = $lcol;
        }
      }
    }

    if (count($bad_fks) > 0) {
      // Documentation for how to creata violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values have a missing linked record in the data store.';
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
   * @param array $violoations
   *   An array to which any new violoations can be added.
   */
  public function validateTypes($values, $chado_table, $record_id, $record, &$violations) {
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_types = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Skip fields without values. If they are required
      // but missing then the validateRequired() function will ccthc those.
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
        if (!is_bool($col_val)) {
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
      // Documentation for how to creata violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values are of the wrong type.';
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
   * @param array $violoations
   *   An array to which any new violoations can be added.
   */
  public function validateSize($values, $chado_table, $record_id, $record, &$violations) {
    $chado = $this->getChadoConnection();
    $schema = $chado->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_sizes = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Skip fields without values. If they are required
      // but missing then the validateRequired() function will ccthc those.
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
      // Documentation for how to creata violation is here
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

    $build = $this->buildChadoRecords($values, TRUE);
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
