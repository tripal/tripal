<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;


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
	 * @{inheritdoc}
	 */
  public function insertValues(&$values) : bool {
    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');
    $records = $this->buildChadoRecords($values);

    $transaction_chado = $chado->startTransaction();
    try {
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          // Insert the record.
          $insert = $chado->insert($chado_table);
          $insert->fields($record['fields']);
          $record_id = $insert->execute();
          if (!$record_id) {
            throw new \Exception($this->t('Failed to insert a record in the Chado "@table" table. Record: @record',
                ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }

          // Update the record array to include the record id.
          $chado = \Drupal::service('tripal_chado.database');
          $schema = $chado->schema();
          $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
          $pkey = $table_def['primary key'];
          $records[$chado_table][$delta]['conditions'][$pkey] = $record_id;
        }
      }
      $this->setRecordIds($values, $records);
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return FALSE;
    }

    // Now set the record Ids of the properties.


    return TRUE;
  }

  /**
   * @{inheritdoc}
   */
  public function updateValues($values) : bool {
    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');
    $records = $this->buildChadoRecords($values);

    $transaction_chado = $chado->startTransaction();
    try {
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          if (!array_key_exists('conditions', $record)) {
            throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to missing conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }

          $update = $chado->update($chado_table);
          $update->fields($record['fields']);
          $num_conditions = 0;
          foreach ($record['conditions'] as $chado_column => $value) {
            if (!empty($value)) {
              $update->condition($chado_column, $value);
              $num_conditions++;
            }
          }
          if ($num_conditions == 0) {
            throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
                ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
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
      }
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return FALSE;
    }
    return TRUE;
  }

  /**
   * @{inheritdoc}
   */
  public function loadValues(&$values) : bool {
    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');
    $records = $this->buildChadoRecords($values);

    $transaction_chado = $chado->startTransaction();
    try {
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          if (!array_key_exists('conditions', $record)) {
            throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }

          $select = $chado->select('1:' . $chado_table, 'ct');
          $select->fields('ct', array_keys($record['fields']));
          $num_conditions = 0;
          foreach ($record['conditions'] as $chado_column => $value) {
            if (!empty($value)) {
              $select->condition($chado_column, $value);
              $num_conditions++;
            }
          }
          if ($num_conditions == 0) {
            throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }
          $results = $select->execute();
          if (!$results) {
            throw new \Exception($this->t('Failed to select record in the Chado "@table" table. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }
          $records[$chado_table][$delta] = $results->fetchAssoc();
        }
      }
      $this->setPropValues($values, $records);

    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return FALSE;
    }
    return TRUE;
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

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {
          $definition = $info['definition'];
          $settings = $definition->getSettings();
          $chado_table = $settings['storage_plugin_settings']['chado_table'];

          if ($key != 'record_id') {
            continue;
          }

          $chado = \Drupal::service('tripal_chado.database');
          $schema = $chado->schema();
          $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
          $pkey = $table_def['primary key'];

          $record_id = $records[$chado_table][$delta]['conditions'][$pkey];
          $values[$field_name][$delta][$key]['value']->setValue($record_id);
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

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {
          $definition = $info['definition'];
          $prop_value = $info['value'];
          $settings = $definition->getSettings();
          $chado_table = $settings['storage_plugin_settings']['chado_table'];
          $chado_column = $settings['storage_plugin_settings']['chado_column'];
          if ($key == 'record_id') {
            continue;
          }

          if (array_key_exists($chado_table, $records)) {
            if (array_key_exists($chado_column, $records[$chado_table][$delta])) {
              $value = $records[$chado_table][$delta][$chado_column];
              $values[$field_name][$delta][$key]['value']->setValue($value);
            }
          }
        }
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
   * @return array
   *   An associative array.
   */
  protected function buildChadoRecords($values) {

    $records = [];
    $logger = \Drupal::service('tripal.logger');

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
          $prop_value = $info['value'];
          $field_type = $prop_value->getFieldType();
          $settings = $definition->getSettings();

          if (!array_key_exists('chado_table', $settings['storage_plugin_settings'])) {
            $logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the "chado_table setting',
              ['@field' => $field_type]));
            continue;
          }
          if (!array_key_exists('chado_column', $settings['storage_plugin_settings'])) {
            $logger->error($this->t('Cannot save record in Chado. The field, "@field", is missing the "chado_column setting',
              ['@field' => $field_type]));
            continue;
          }
          $chado_table = $settings['storage_plugin_settings']['chado_table'];
          $chado_column = $settings['storage_plugin_settings']['chado_column'];

          if ($key == 'record_id') {
            $chado = \Drupal::service('tripal_chado.database');
            $schema = $chado->schema();
            $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
            $pkey = $table_def['primary key'];
            $records[$chado_table][$delta]['conditions'][$pkey] = $prop_value->getValue();
          }
          else {
            $records[$chado_table][$delta]['fields'][$chado_column] = $prop_value->getValue();
          }
        }
      }
    }
    return $records;
  }
}
