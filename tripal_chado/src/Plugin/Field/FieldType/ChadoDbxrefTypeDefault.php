<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal dbxref field type.
 *
 * @FieldType(
 *   id = "chado_dbxref_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Database Cross Reference"),
 *   description = @Translation("Add a Chado dbxref to the content type."),
 *   default_widget = "chado_dbxref_widget_default",
 *   default_formatter = "chado_dbxref_formatter_default",
 * )
 */
class ChadoDbxrefTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_dbxref_type_default';
  protected static $object_table = 'dbxref';
  protected static $object_id = 'dbxref_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'dbxref_accession';
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
    // CV Term is 'Database Cross Reference'
    $field_settings['termIdSpace'] = 'SBO';
    $field_settings['termAccession'] = '0000554';
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
    $db_term = $mapping->getColumnTermId($object_table, 'db_id');
    $accession_term = $mapping->getColumnTermId($object_table, 'accession');
    $accession_len = $object_schema_def['fields']['accession']['size'];
    $version_term = $mapping->getColumnTermId($object_table, 'version');
    $version_len = $object_schema_def['fields']['version']['size'];
    $description_term = $mapping->getColumnTermId($object_table, 'description');  // text

    // Columns from linked tables
    $db_schema_def = $schema->getTableDef('db', ['format' => 'Drupal']);
    $db_name_term = $mapping->getColumnTermId('db', 'name');
    $db_name_len = $db_schema_def['fields']['name']['size'];
    $db_description_term = $mapping->getColumnTermId('db', 'description');
    $db_description_len = $db_schema_def['fields']['description']['size'];
    $db_urlprefix_term = $mapping->getColumnTermId('db', 'urlprefix');
    $db_urlprefix_len = $db_schema_def['fields']['urlprefix']['size'];
    $db_url_term = $mapping->getColumnTermId('db', 'url');
    $db_url_len = $db_schema_def['fields']['url']['size'];

    // Linker table, when used, requires specifying the linker table and column.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);

    $extra_linker_columns = [];
    if ($linker_table != $base_table) {
      $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // the following should be the same as $base_pkey_col @todo make sure it is
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
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col . ';' . $linker_pkey_col,
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
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col . ';' . $linker_fkey_column,
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
          'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col . ';' . $column,
          'as' => 'linker_' . $column,
        ]);
      }
    }

    // The object table, the destination table of the linker table
    // The db_id
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'dbxref_db_id', $db_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';db_id',
      'as' => 'dbxref_db_id',
    ]);

    // The dbxref accession
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'dbxref_accession', $accession_term, $accession_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';accession',
      'as' => 'dbxref_accession',
    ]);

    // The dbxref version (often this is not used)
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'dbxref_version', $version_term, $version_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';version',
      'as' => 'dbxref_version',
    ]);

    // The dbxref description (often this is not used)
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'dbxref_description', $description_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';description',
      'as' => 'dbxref_description',
    ]);

    // The remaining values are from the database referenced by this dbxref linked through db_id
    // The database name
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'dbxref_db_name', $db_name_term, $db_name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.db_id>db.db_id;name',
      'as' => 'dbxref_db_name',
    ]);

    // The database description
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'dbxref_db_description', $db_description_term, $db_description_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.db_id>db.db_id;description',
      'as' => 'dbxref_db_description',
    ]);

    // The database url prefix - may contain {db} or {accession} replaceable values
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'dbxref_db_urlprefix', $db_urlprefix_term, $db_urlprefix_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.db_id>db.db_id;urlprefix',
      'as' => 'dbxref_db_urlprefix',
    ]);

    // The database url
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'dbxref_db_url', $db_url_term, $db_url_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.db_id>db.db_id;url',
      'as' => 'dbxref_db_url',
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = FALSE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    if ($base_table) {
      $linker_tables = $this->getLinkerTables(self::$object_table, $base_table);
      if (count($linker_tables) > 0) {
        $compatible = TRUE;
      }
    }
    return $compatible;
  }

}
