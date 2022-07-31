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

    // Index the types by their entity type, field type and key.
    foreach ($types as $field_name => $prop_types) {
      foreach ($prop_types as $type) {
        $entity_type = $type->getEntityType();
        $key = $type->getKey();

        if (!array_key_exists($entity_type, $this->property_types)) {
          $this->property_types[$entity_type] = [];
        }
        if (!array_key_exists($field_name, $this->property_types[$entity_type])) {
          $this->property_types[$entity_type][$field_name] = [];
        }
        if (array_key_exists($key, $this->property_types[$entity_type])) {
          $logger = \Drupal::service('tripal.logger');
          $logger->error('Cannot add a property type, "@prop", as it already exists',
              ['@prop' => $entity_type . '.' . $field_name . '.' . $key]);
          return False;
        }
        $this->property_types[$entity_type][$field_name][$key] = $type;
      }
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
      if (!array_key_exists($entity_type, $this->property_types)) {
        if (!array_key_exists($field_type, $this->property_types[$entity_type])) {
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
                ['@table' => $chado_table, '@record' => print_r($record, True)]));
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
      return False;
    }

    // Now set the record Ids of the properties.


    return True;
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
              ['@table' => $chado_table, '@record' => print_r($record, True)]));
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
                ['@table' => $chado_table, '@record' => print_r($record, True)]));
          }

          $rows_affected = $update->execute();
          if ($rows_affected == 0) {
            throw new \Exception($this->t('Failed to update record in the Chado "@table" table. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, True)]));
          }
          if ($rows_affected > 1) {
            throw new \Exception($this->t('Incorrectly tried to update multiple records in the Chado "@table" table. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, True)]));
          }
        }
      }
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return False;
    }
    return True;
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
              ['@table' => $chado_table, '@record' => print_r($record, True)]));
          }

          $select = $chado->select($chado_table, 'ct');
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
              ['@table' => $chado_table, '@record' => print_r($record, True)]));
          }
          $results = $select->execute();
          if (!$results) {
            throw new \Exception($this->t('Failed to select record in the Chado "@table" table. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, True)]));
          }
          $records[$chado_table][$delta] = $results->fetchAssoc();
        }
      }
      $this->setPropValues($values, $records);

    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return False;
    }
    return True;
  }

  /**
   * @{inheritdoc}
   */
  public function deleteValues($values) : bool {

    return False;
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
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   * @return array
   *   An associative array.
   */
  protected function buildChadoRecords($values) {

    $records = [];
    $logger = \Drupal::service('tripal.logger');

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {
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