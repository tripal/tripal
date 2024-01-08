<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;

/**
 * Plugin implementation of default Tripal organism field type.
 *
 * @FieldType(
 *   id = "chado_organism_type_default",
 *   object_table = "organism",
 *   label = @Translation("Chado Organism"),
 *   description = @Translation("A chado organism reference"),
 *   default_widget = "chado_organism_widget_default",
 *   default_formatter = "chado_organism_formatter_default",
 * )
 */
class ChadoOrganismTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_organism_type_default';
  // The following needs to match the object_table annotation above
  protected static $object_table = 'organism';
  protected static $object_id = 'organism_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'species';
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
    // CV Term is 'Organism'
    $field_settings['termIdSpace'] = 'OBI';
    $field_settings['termAccession'] = '0100026';
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
    $genus_term = $mapping->getColumnTermId($object_table, 'genus');
    $genus_len = $object_schema_def['fields']['genus']['size'];
    $species_term = $mapping->getColumnTermId($object_table, 'species');
    $species_len = $object_schema_def['fields']['species']['size'];
    $infraspecific_name_term = $mapping->getColumnTermId($object_table, 'infraspecific_name');
    $infraspecific_name_len = $object_schema_def['fields']['infraspecific_name']['size'];
    $abbreviation_term = $mapping->getColumnTermId($object_table, 'abbreviation');
    $abbreviation_len = $object_schema_def['fields']['abbreviation']['size'];
    $common_name_term = $mapping->getColumnTermId($object_table, 'common_name');
    $common_name_len = $object_schema_def['fields']['common_name']['size'];
    $comment_term = $mapping->getColumnTermId($object_table, 'comment');

    // Other columns specific to this object table
    $comment_term = $mapping->getColumnTermId($object_table, 'comment');

    // Object columns that link to another table
    $object_type_col = 'type_id';

    // Cvterm table, to retrieve the name for the organism type
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $infraspecific_type_term = $mapping->getColumnTermId('cvterm', 'name');
    $infraspecific_type_len = $cvterm_schema_def['fields']['name']['size'];

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
    ]);

    // Base table links directly
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $linker_fkey_col,
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
        'path' => $linker_table . '.' . $linker_pkey_col,
      ]);

      // Define the link between the base table and the linker table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link', $linker_left_term, [
        'action' => 'store_link',
        'drupal_store' => FALSE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col,
      ]);

      // Define the link between the linker table and the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $linker_table . '.' . $linker_fkey_col,
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
          'as' => 'linker_' . $column,
        ]);
      }
    }

    // The object table, the destination table of the linker table
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_genus', $genus_term, $genus_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';genus',
      'chado_table' => $object_table,
      'chado_column' => 'genus',
      'as' => 'organism_genus',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_species', $species_term, $species_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col. ';species',
      'chado_table' => $object_table,
      'chado_column' => 'species',
      'as' => 'organism_species',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_infraspecific_type', $infraspecific_type_term, $infraspecific_type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.' . $object_type_col . '>cvterm.cvterm_id;name',
      'as' => 'organism_infraspecific_type',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_infraspecific_name', $infraspecific_name_term, $infraspecific_name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';infraspecific_name',
      'as' => 'organism_infraspecific_name',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_abbreviation', $abbreviation_term, $abbreviation_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';abbreviation',
      'as' => 'organism_abbreviation',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_common_name', $common_name_term, $common_name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';common_name',
      'as' => 'organism_common_name',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'organism_comment', $comment_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_col . '>' . $object_table . '.' . $object_pkey_col . ';comment',
      'as' => 'organism_comment',
    ]);

    return $properties;
  }

}
