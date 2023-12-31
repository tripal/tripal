<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Plugin implementation of default Tripal analysis field type.
 *
 * @FieldType(
 *   id = "chado_analysis_type_default",
 *   object_table = "analysis",
 *   label = @Translation("Chado Analysis"),
 *   description = @Translation("Application of analytical methods to existing data of a specific type"),
 *   default_widget = "chado_analysis_widget_default",
 *   default_formatter = "chado_analysis_formatter_default",
 * )
 */
class ChadoAnalysisTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_analysis_type_default';
  // The following needs to match the object_table annotation above
  protected static $object_table = 'analysis';
  protected static $object_id = 'analysis_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'analysis_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['base_table'] = '';
    $storage_settings['storage_plugin_settings']['linking_method'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['linker_fkey_column'] = '';
    $storage_settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'Analysis'
    $field_settings['termIdSpace'] = 'operation';
    $field_settings['termAccession'] = '2945';
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create a variable for easy access to settings.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the various tables and columns needed for this field.
    // We will get the property terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $record_id_term = 'SIO:000729';

    // Base table
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Object table
    $object_table = self::$object_table;
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];
    $object_pkey_term = $mapping->getColumnTermId($object_table, $object_pkey_col);

    // Columns specific to the object table
    $name_term = $mapping->getColumnTermId($object_table, 'name');
    $name_len = $object_schema_def['fields']['name']['size'];
    $description_term = $mapping->getColumnTermId($object_table, 'description'); // text
    $program_term = $mapping->getColumnTermId($object_table, 'program');
    $program_len = $object_schema_def['fields']['program']['size'];
    $programversion_term = $mapping->getColumnTermId($object_table, 'programversion');
    $programversion_len = $object_schema_def['fields']['programversion']['size'];
    $algorithm_term = $mapping->getColumnTermId($object_table, 'algorithm');
    $algorithm_len = $object_schema_def['fields']['algorithm']['size'];
    $sourcename_term = $mapping->getColumnTermId($object_table, 'sourcename');
    $sourcename_len = $object_schema_def['fields']['sourcename']['size'];
    $sourceversion_term = $mapping->getColumnTermId($object_table, 'sourceversion');
    $sourceversion_len = $object_schema_def['fields']['sourceversion']['size'];
    $sourceuri_term = $mapping->getColumnTermId($object_table, 'sourceuri'); // text
    // @todo timeexecuted not yet implemented

    // Linker table, when used, requires specifying the linker table and column.
    // For single hop, in the yaml we support using the usual 'base_table'
    // and 'base_column' settings.
    $linker_table = $storage_settings['linker_table'] ?? $base_table;
    $linker_fkey_col = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? $object_pkey_col;

    $extra_linker_columns = [];
    if ($linker_table != $base_table) {
      $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // the following should be the same as $base_pkey_col @todo make sure it is
      $linker_left_col = array_keys($linker_schema_def['foreign keys'][$base_table]['columns'])[0];
      $linker_left_term = $mapping->getColumnTermId($linker_table, $linker_left_col);
      $linker_fkey_term = $mapping->getColumnTermId($linker_table, $linker_fkey_col);

      // Some but not all linker tables contain rank, type_id, and maybe other columns.
      // These are conditionally added only if they exist in the linker
      // table, and if a term is defined for them.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_fkey_col)) {
          $term = $mapping->getColumnTermId($linker_table, $column);
          if ($term) {
            $extra_linker_columns[$column] = $term;
          }
        }
      }
    }
    else {
      $linker_fkey_term = $mapping->getColumnTermId($base_table, $linker_fkey_col);
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
      //'chado_table' => $base_table,
      //'chado_column' => $base_pkey_col,
    ]);

    // Base table links directly
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $linker_fkey_col,
        //'chado_table' => $base_table,
        //'chado_column' => $linker_fkey_col,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);
    }
    // An intermediate linker table is used
    else {
      // Define the linker table that links the base table to the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_pkey_col,
        //'chado_table' => $linker_table,
        //'chado_column' => $linker_pkey_col,
      ]);

      // Define the link between the base table and the linker table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link', $linker_left_term, [
        'action' => 'store_link',
        'drupal_store' => FALSE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col,
        //'left_table' => $base_table,
        //'left_table_id' => $base_pkey_col,
        //'right_table' => $linker_table,
        //'right_table_id' => $linker_left_col,
      ]);

      // Define the link between the linker table and the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $linker_fkey_col,
        //'chado_table' => $linker_table,
        //'chado_column' => $linker_fkey_col,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);

      // Other columns in the linker table. Set in the widget, but currently not implemented in the formatter.
      // Typically these are type_id and rank, but are not present in all linker tables,
      // so they are added only if present in the linker table.
      foreach ($extra_linker_columns as $column => $term) {
        $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_' . $column, $term, [
          'action' => 'store',
          'drupal_store' => FALSE,
          'path' => $linker_table . '.' . $column,
          //'chado_table' => $linker_table,
          //'chado_column' => $column,
          'as' => 'linker_' . $column,
        ]);
      }
    }

    // The object table, the destination table of the linker table
    // The analysis name
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_name', $name_term, $name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';name',
      //'chado_table' => $object_table,
      //'chado_column' => 'name',
      'as' => 'analysis_name',
    ]);

    // The analysis description
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'analysis_description', $description_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';description',
      //'chado_column' => 'description',
      'as' => 'analysis_description',
    ]);

    // The analysis program - not null
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_program', $program_term, $program_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';program',
      //'chado_column' => 'program',
      'as' => 'analysis_program',
    ]);

    // The analysis program version - not null
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_programversion', $programversion_term, $programversion_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';programversion',
      //'chado_column' => 'programversion',
      'as' => 'analysis_programversion',
    ]);

    // The analysis algorithm
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_algorithm', $algorithm_term, $algorithm_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';algorithm',
      //'chado_column' => 'algorithm',
      'as' => 'analysis_algorithm',
    ]);

    // The analysis sourcename
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_sourcename', $sourcename_term, $sourcename_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col .';sourcename',
      //'chado_column' => 'sourcename',
      'as' => 'analysis_sourcename',
    ]);

    // The analysis sourceversion
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_sourceversion', $sourceversion_term, $sourceversion_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';sourceversion',
      //'chado_column' => 'sourceversion',
      'as' => 'analysis_sourceversion',
    ]);

    // The analysis sourceuri
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'analysis_sourceuri', $sourceuri_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';sourceuri',
      //'chado_column' => 'sourceuri',
      'as' => 'analysis_sourceuri',
    ]);

    // @todo timeexecuted not yet implemented - not null, default CURRENT_TIMESTAMP

    return $properties;
  }

}
