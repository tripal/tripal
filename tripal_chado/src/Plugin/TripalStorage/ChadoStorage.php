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
   * A helper function for deleting records.
   */
  private function deleteRecords(&$records) {
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    foreach ($records as $chado_table => $deltas) {
      foreach ($deltas as $delta => $record) {

        if (!array_key_exists('conditions', $record)) {
          throw new \Exception($this->t('Cannot delete the record in the Chado "@table" table due to missing conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
        }

        $delete = $chado->delete($chado_table);
        $delete->fields($record['fields']);
        foreach ($record['conditions'] as $chado_column => $value) {
          if (!empty($value)) {
            $delete->condition($chado_column, $value);
          }
        }
        $rows_affected = $delete->execute();
        if ($rows_affected == 0) {
          throw new \Exception($this->t('Failed to delete record in the Chado "@table" table. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
        }
        if ($rows_affected > 1) {
          throw new \Exception($this->t('Incorrectly tried to delete multiple records in the Chado "@table" table. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
        }

        // Update the record array to include the record id.
        $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
        $pkey = $table_def['primary key'];
        $records[$chado_table][$delta]['conditions'][$pkey] = -1;
      }
    }
  }

  /**
   * A helper function for inserting records.
   */
  private function insertRecords(&$records) {
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

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
        $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
        $pkey = $table_def['primary key'];
        $records[$chado_table][$delta]['conditions'][$pkey] = $record_id;
      }
    }
  }

  /**
	 * @{inheritdoc}
	 */
  public function insertValues(&$values) : bool {
    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');

    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];

    $transaction_chado = $chado->startTransaction();
    try {
      $this->insertRecords($records);
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
  public function updateValues(&$values) : bool {
    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');
    $build = $this->buildChadoRecords($values, TRUE);
    $records = $build['records'];
    $base_tables = $build['base_tables'];

    $transaction_chado = $chado->startTransaction();
    try {
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          if (!array_key_exists('conditions', $record)) {
            throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to missing conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }

          if (array_key_exists('delete_me', $record)) {
//             $delete_records = [];
//             $delete_records[$chado_table][$delta] = $record;
//             $this->deleteRecords($delete_records);
//             $this->setRecordIds($values, $delete_records);
//             continue;
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
            // If this is table is a base table and we have no conditions then
            // we cannot update. We must fail.  But if this is not the base
            // table then we should be adding a new record to a linking table.
            if (in_array($chado_table, $base_tables)) {
              throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
                  ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
            }
            else {
              $insert_records = [];
              $insert_records[$chado_table][$delta] = $record;
              $this->insertRecords($insert_records);
              $this->setRecordIds($values, $insert_records);
              continue;
            }
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
    $build = $this->buildChadoRecords($values, FALSE);
    $records = $build['records'];
    $base_tables = $build['base_tables'];

    $transaction_chado = $chado->startTransaction();
    try {
      foreach ($records as $chado_table => $deltas) {
        foreach ($deltas as $delta => $record) {

          if (!array_key_exists('conditions', $record)) {
            throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
              ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
          }

          // Select the fields in the chado table.
          $select = $chado->select('1:' . $chado_table, 'ct');
          $select->fields('ct', array_keys($record['fields']));

          // Add in any joins.
          if (array_key_exists('joins', $record)) {
            $j_index = 0;
            foreach ($record['joins'] as $rtable => $jinfo) {
              $lalias = $jinfo['on']['left_alias'];
              $ralias = $jinfo['on']['right_alias'];
              $lcol = $jinfo['on']['left_col'];
              $rcol = $jinfo['on']['right_col'];
              $select->leftJoin($rtable, $ralias, $lalias . '.' .  $lcol . '=' .  $ralias . '.' . $rcol);
              foreach ($jinfo['columns'] as $column) {
                $sel_col = $column[0];
                $sel_col_as = $column[1];
                $select->addField($ralias, $sel_col, $sel_col_as);
              }
              $j_index++;
            }
          }

          // Add the select condition
          $num_conditions = 0;
          foreach ($record['conditions'] as $chado_column => $value) {
            if (!empty($value)) {
              $select->condition('ct.'.$chado_column, $value);
              $num_conditions++;
            }
          }
          if ($num_conditions == 0) {
            // If this is the base table and there is no condiiton for selecting
            // then throw an error. For all other tables don't try to query
            // if we're missing the conditino.
            if (in_array($chado_table, $base_tables)) {
              throw new \Exception($this->t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
                ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
            }
            continue;
          }

          // Execute the query.
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
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // Iterate through the value objects.
    foreach ($values as $field_name => $deltas) {
      foreach ($deltas as $delta => $keys) {
        foreach ($keys as $key => $info) {
          $definition = $info['definition'];
          $prop_type = $info['type'];

          // Get the feild and property storage settings.
          $field_settings = $definition->getSettings();
          $field_storage_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();

          // Get the base table information.
          $base_table = $field_storage_settings['base_table'];
          $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
          $base_pkey = $base_table_def['primary key'];

          // Get the Chado table information. If one is not specified (as
          // in teh case of single value fields) then default to the base
          // table.
          $chado_table = $base_table;
          $chado_table_def = $base_table_def;
          $pkey = $base_pkey;
          if (array_key_exists('chado_table', $prop_storage_settings)) {
            $chado_table = $prop_storage_settings['chado_table'];
            $chado_table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
            $pkey = $chado_table_def['primary key'];
          }

          // Skip values we don't have tables or deltas fo in the record array.
          if (!in_array($chado_table, array_keys($records)) or
              !in_array($delta, array_keys($records[$chado_table]))) {
            continue;
          }

          if ($key == 'record_id') {
            $record_id = $records[$chado_table][$delta]['conditions'][$pkey];
            $values[$field_name][$delta][$key]['value']->setValue($record_id);
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
      $values[$field_name][$delta][$key]['value']->setValue(trim($value));
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
    $chado = \Drupal::service('tripal_chado.database');
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
          $field_storage_settings = $field_settings['storage_plugin_settings'];
          $prop_storage_settings = $prop_type->getStorageSettings();
          $base_table = $field_storage_settings['base_table'];
          $base_table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
          $base_pkey = $base_table_def['primary key'];

          // Get the Chado table. Use the base table if one is not provided.
          // Single value fields won't ever have a chado table specified.
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

          // If this is the record ID then keep track of it.
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
            $value = trim($prop_value->getValue());
            // If this column is the foreign key column to the base table then
            // we need to replace it with the record ID. But we can't gurantee
            // that fields come in order to get that beforehand. So we'll leave
            // a reminder token to replace it when we're all done.
            if ($fk_col == $chado_column) {
              $records[$chado_table][$delta]['fields'][$chado_column] = ['REPLACE_BASE_RECORD_ID', $base_table];
            }
            else {
              $records[$chado_table][$delta]['fields'][$chado_column] = $value;
            }

            // When the record is a linked record we allowe deleting of the
            // linked recrod when a specific property is empty.
            $delete_on_empty = array_key_exists('delete_on_empty', $prop_storage_settings) ? $prop_storage_settings['delete_on_empty'] : FALSE;
            if ($chado_table != $base_table and $delete_on_empty and empty($value)) {
              $records[$chado_table][$delta]['delete_me'] = TRUE;
            }


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
            $records[$table_name][$delta]['fields'][$chado_column] = $base_record_ids[$val[1]];
          }
        }
      }
    }

    return ['base_tables' => $base_record_ids, 'records' => $records];
  }


  /**
   *
   * @param array $records
   * @param string $base_table
   * @param int $delta
   * @param string $path
   */
  protected function addChadoRecordJoins(array &$records, string $chado_column, string $as,
      int $delta, array $path_arr, $parent_table = NULL, $depth = 0) {

    // Get the left column and the right table join infor.
    list($left, $right) = explode(">", array_shift($path_arr));
    list($left_table, $left_col) = explode(".", $left);
    list($right_table, $right_col) = explode(".", $right);

    // We want all joins to be with the parent table record.
    $parent_table = !$parent_table ? $left_table : $parent_table;
    $lalias = $depth == 0 ? 'ct' : 'j' . ($depth - 1);
    $ralias = 'j' . $depth;
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $ltable_def = $schema->getTableDef($left_table, ['format' => 'drupal']);
    $rtable_def = $schema->getTableDef($right_table, ['format' => 'drupal']);

    // @todo check the requested join is valid.

    // Add the join.
    $records[$parent_table][$delta]['joins'][$right_table]['on'] = [
      'left_table' => $left_table,
      'left_col' => $left_col,
      'right_table' => $right_table,
      'right_col' => $right_col,
      'left_alias' => $lalias,
      'right_alias' => $ralias,
    ];

    // We're done recursing if we only have two elements left in the path
    if (count($path_arr)== 0) {
      $records[$parent_table][$delta]['joins'][$right_table]['columns'][] = [$chado_column, $as];
      return;
    }

    // Add the right table back onto the path as the new left table and recurse.
    $depth++;
    $this->addChadoRecordJoins($records, $chado_column, $as, $delta, $path_arr, $parent_table, $depth);
  }
}
