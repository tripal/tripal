<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal Array Design field type.
 *
 * @FieldType(
 *   id = "chado_array_design_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Array Design"),
 *   description = @Translation("Add a Chado Array Design to the content type."),
 *   default_widget = "chado_array_design_widget_default",
 *   default_formatter = "chado_array_design_formatter_default",
 * )
 */
class ChadoArrayDesignTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_array_design_type_default';
  protected static $object_table = 'arraydesign';
  protected static $object_id = 'arraydesign_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'array_design_name';
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
    // CV Term is 'ArrayDesign'
    $field_settings['termIdSpace'] = 'NCIT';
    $field_settings['termAccession'] = 'C47885';
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
    $name_term = $mapping->getColumnTermId($object_table, 'name');  // text
    $description_term = $mapping->getColumnTermId($object_table, 'description');  // text
    $version_term = $mapping->getColumnTermId($object_table, 'version');  // text
    $array_dimensions_term = $mapping->getColumnTermId($object_table, 'array_dimensions');  // text
    $element_dimensions_term = $mapping->getColumnTermId($object_table, 'element_dimensions');  // text
    $num_of_elements_term = $mapping->getColumnTermId($object_table, 'num_of_elements');
    $num_array_rows_term = $mapping->getColumnTermId($object_table, 'num_array_rows');
    $num_array_columns_term = $mapping->getColumnTermId($object_table, 'num_array_columns');
    $num_grid_columns_term = $mapping->getColumnTermId($object_table, 'num_grid_columns');
    $num_grid_rows_term = $mapping->getColumnTermId($object_table, 'num_grid_rows');
    $num_sub_columns_term = $mapping->getColumnTermId($object_table, 'num_sub_columns');
    $num_sub_rows_term = $mapping->getColumnTermId($object_table, 'num_sub_rows');

    // Columns from linked tables
    // both platformtype and substratetype reference the cvterm table
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $type_term = $mapping->getColumnTermId('cvterm', 'name');
    $type_len = $cvterm_schema_def['fields']['name']['size'];
    $contact_schema_def = $schema->getTableDef('contact', ['format' => 'Drupal']);
    $manufacturer_term = $mapping->getColumnTermId('contact', 'name');
    $manufacturer_len = $contact_schema_def['fields']['name']['size'];
    $protocol_term = $mapping->getColumnTermId('protocol', 'name');  // text
    // $dbxref_schema_def = $schema->getTableDef('dbxref', ['format' => 'Drupal']);
    $dbxref_term = $mapping->getColumnTermId('dbxref', 'accession');
    // $dbxref_len = $dbxref_schema_def['fields']['accession']['size'];
    $db_schema_def = $schema->getTableDef('db', ['format' => 'Drupal']);
    $db_term = $mapping->getColumnTermId('db', 'name');
    // $db_len = $db_schema_def['fields']['name']['size'];

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

    // This property will store the Drupal entity ID of the linked chado
    // record, if one exists.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'fkey' => self::$object_id,
    ]);

    // Base table links directly
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $linker_fkey_column, $linker_fkey_term, [
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
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $linker_fkey_column, $linker_fkey_term, [
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
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_name', $name_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';name',
      'as' => 'array_design_name',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_description', $description_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';description',
      'as' => 'array_design_description',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_version', $version_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';version',
      'as' => 'array_design_version',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_array_dimensions', $array_dimensions_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';array_dimensions',
      'as' => 'array_design_array_dimensions',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_element_dimensions', $element_dimensions_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';element_dimensions',
      'as' => 'array_design_element_dimensions',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_of_elements', $num_of_elements_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_of_elements',
      'as' => 'array_design_num_of_elements',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_array_columns', $num_array_columns_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_array_columns',
      'as' => 'array_design_num_array_columns',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_array_rows', $num_array_rows_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_array_rows',
      'as' => 'array_design_num_array_rows',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_grid_columns', $num_grid_columns_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_grid_columns',
      'as' => 'array_design_num_grid_columns',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_grid_rows', $num_grid_rows_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_grid_rows',
      'as' => 'array_design_num_grid_rows',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_sub_columns', $num_sub_columns_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_sub_columns',
      'as' => 'array_design_num_sub_columns',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'array_design_num_sub_rows', $num_sub_rows_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';num_sub_rows',
      'as' => 'array_design_num_sub_rows',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_database_accession', $dbxref_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.dbxref_id>dbxref.dbxref_id;accession',
      'as' => 'array_design_database_accession',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_database_name', $db_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id;name',
      'as' => 'array_design_database_name',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'array_design_platformtype', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.platformtype_id>cvterm.cvterm_id;name',
      'as' => 'array_design_platformtype',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'array_design_substratetype', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.substratetype_id>cvterm.cvterm_id;name',
      'as' => 'array_design_substratetype',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'array_design_manufacturer', $manufacturer_term, $manufacturer_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.manufacturer_id>contact.contact_id;name',
      'as' => 'array_design_manufacturer',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'array_design_protocol', $protocol_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.protocol_id>protocol.protocol_id;name',
      'as' => 'array_design_protocol',
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
