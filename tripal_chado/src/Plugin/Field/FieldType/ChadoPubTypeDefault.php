<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal publication field type.
 *
 * @FieldType(
 *   id = "chado_pub_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Publication"),
 *   description = @Translation("Associates a publication (e.g. journal article, conference proceedings, book chapter, etc.) with this record."),
 *   default_widget = "chado_pub_widget_default",
 *   default_formatter = "chado_pub_formatter_default",
 * )
 */
class ChadoPubTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_pub_type_default';
  protected static $object_table = 'pub';
  protected static $object_id = 'pub_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'pub_title';
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
    // CV Term is 'publication'
    $field_settings['termIdSpace'] = 'schema';
    $field_settings['termAccession'] = 'publication';
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

    // Columns specific to the object table
    $title_term = $mapping->getColumnTermId($object_table, 'title'); // text
    $volumetitle_term = $mapping->getColumnTermId($object_table, 'volumetitle'); // text
    $volume_term = $mapping->getColumnTermId($object_table, 'volume');
    $volume_len = $object_schema_def['fields']['volume']['size'];
    $series_name_term = $mapping->getColumnTermId($object_table, 'series_name');
    $series_name_len = $object_schema_def['fields']['series_name']['size'];
    $issue_term = $mapping->getColumnTermId($object_table, 'issue');
    $issue_len = $object_schema_def['fields']['issue']['size'];
    $pyear_term = $mapping->getColumnTermId($object_table, 'pyear');
    $pyear_len = $object_schema_def['fields']['pyear']['size'];
    $pages_term = $mapping->getColumnTermId($object_table, 'pages');
    $pages_len = $object_schema_def['fields']['pages']['size'];
    $miniref_term = $mapping->getColumnTermId($object_table, 'miniref');
    $miniref_len = $object_schema_def['fields']['miniref']['size'];
    $uniquename_term = $mapping->getColumnTermId($object_table, 'uniquename'); // text
    $is_obsolete_term = $mapping->getColumnTermId($object_table, 'is_obsolete'); // boolean
    $publisher_term = $mapping->getColumnTermId($object_table, 'publisher');
    $publisher_len = $object_schema_def['fields']['publisher']['size'];
    $pubplace_term = $mapping->getColumnTermId($object_table, 'pubplace');
    $pubplace_len = $object_schema_def['fields']['pubplace']['size'];

    // Cvterm table, to retrieve the name for the publication type
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $type_term = $mapping->getColumnTermId('cvterm', 'name');
    $type_len = $cvterm_schema_def['fields']['name']['size'];

    // Linker table, when used, requires specifying the linker table and column.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);

    $extra_linker_columns = [];
    if ($linker_table != $base_table) {
      $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_pkey_col = $linker_schema_def['primary key'];
      $linker_left_col = array_keys($linker_schema_def['foreign keys'][$base_table]['columns'])[0];
      $linker_left_term = $mapping->getColumnTermId($linker_table, $linker_left_col);
      $linker_fkey_term = $mapping->getColumnTermId($linker_table, $linker_fkey_column);

      // Some but not all linker tables contain rank, type_id, and maybe other columns.
      // These are conditionally added only if they exist in the linker
      // table, and if a term is defined for them.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_fkey_column)) {
          $term = $mapping->getColumnTermId($linker_table, $column);
          if ($term) {
            $extra_linker_columns[$column] = $term;
          }
        }
      }
    }
    else {
      $linker_fkey_term = $mapping->getColumnTermId($base_table, $linker_fkey_column);
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the Drupal entity ID of the linked chado
    // record, if one exists.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => self::$object_table,
      'fkey' => $linker_fkey_column,
    ]);

    // Base table links directly
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $linker_fkey_column,
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
        'path' => $linker_table . '.' . $linker_fkey_column,
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
    // The publication title
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'pub_title', $title_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';title',
      'as' => 'pub_title',
    ]);

    // The publication volumetitle
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'pub_volumetitle', $volumetitle_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';volumetitle',
      'as' => 'pub_volumetitle',
    ]);

    // The publication volume
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_volume', $volume_term, $volume_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';volume',
      'as' => 'pub_volume',
    ]);

    // The publication series (e.g. journal name)
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_series_name', $series_name_term, $series_name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';series_name',
      'as' => 'pub_series_name',
    ]);

    // The publication issue
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_issue', $issue_term, $issue_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';issue',
      'as' => 'pub_issue',
    ]);

    // The publication pyear
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_pyear', $pyear_term, $pyear_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';pyear',
      'as' => 'pub_pyear',
    ]);

    // The publication pages
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_pages', $pages_term, $pages_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';pages',
      'as' => 'pub_pages',
    ]);

    // The publication miniref
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_miniref', $miniref_term, $miniref_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';miniref',
      'as' => 'pub_miniref',
    ]);

    // The publication uniquename - not null
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'pub_uniquename', $uniquename_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';uniquename',
      'as' => 'pub_uniquename',
    ]);

    // The type of publication - not null
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_type', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.type_id>cvterm.cvterm_id;name',
      'as' => 'pub_type',
    ]);

    // Publication is obsolete - default=false
    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'pub_is_obsolete', $is_obsolete_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';is_obsolete',
      'as' => 'pub_is_obsolete',
    ]);

    // The publication publisher
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_publisher', $publisher_term, $publisher_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';publisher',
      'as' => 'pub_publisher',
    ]);

    // The publication pubplace
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'pub_pubplace', $pubplace_term, $pubplace_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';pubplace',
      'as' => 'pub_pubplace',
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = TRUE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $linker_tables = $this->getLinkerTables(self::$object_table, $base_table);
    if (count($linker_tables) < 1) {
      $compatible = FALSE;
    }
    return $compatible;
  }

}
