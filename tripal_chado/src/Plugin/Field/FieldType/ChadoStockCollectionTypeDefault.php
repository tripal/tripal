<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal stockcollection field type.
 */
#[FieldType(
  id: 'chado_stockcollection_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Stock Collection'),
  description: new TranslatableMarkup('Indicate that the current chado stock is part of the indicated stock collection.'),
  default_widget: 'chado_stockcollection_widget_default',
  default_formatter: 'chado_stockcollection_formatter_default',
)]
class ChadoStockCollectionTypeDefault extends ChadoFieldItemBase {

  /**
   * The id of this field. This must match what is in the attribute above.
   *
   * @var string
   */
  public static $id = 'chado_stockcollection_type_default';

  /**
   * The linked table which is the object of this relationship.
   *
   * @var string
   */
  protected static $object_table = 'stockcollection';

  /**
   * The primary key of the object table.
   *
   * @var string
   */
  protected static $object_id = 'stockcollection_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'.
    return 'stockcollection_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
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
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'OBI';
    $settings['termAccession'] = '0002076';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $value['record_id'] = 0;
    $value['entity_id'] = 0;
    $value['linker_id'] = 0;
    $value['link'] = 0;
    $value[self::$object_id] = 0;

    // Object table properties.
    $value['stockcollection_name'] = '';
    $value['stockcollection_uniquename'] = '';
    $value['stockcollection_type'] = '';

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create a variable for easy access to settings.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $field_id = self::$id;

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the various tables and columns needed for this field. We will get
    // the property terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Base table.
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Object table.
    $object_table = self::$object_table;
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];
    $name_term = self::getColumnTermId($object_table, 'name', 'schema:name');
    $uniquename_term = self::getColumnTermId($object_table, 'uniquename', 'data:0842');
    $stockcollection_type_term = self::getColumnTermId('cvterm', 'name', 'schema:additionalType');

    // Linker table, when used, requires specifying the linker table and column.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);

    $extra_linker_columns = [];

    $linker_fkey_term = self::getColumnTermId($linker_table, $linker_fkey_column, self::$record_id_term);
    $linker_fkey_path = $base_table . '.' . $linker_fkey_column;

    if ($linker_table != $base_table) {
      $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // The following should be the same as $base_pkey_col.
      // @todo make sure it is.
      $linker_left_col = self::getChadoForeignKeyColumn($linker_table, $base_table, $schema);
      $linker_left_term = self::getColumnTermId($linker_table, $linker_left_col, self::$record_id_term);
      $linker_fkey_path = $linker_table . '.' . $linker_fkey_column;

      // Some but not all linker tables contain rank, type_id, and maybe other
      // columns. These are conditionally added only if they exist in the
      // linker table, and if a term is defined for them.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_fkey_column)) {
          $term = self::getColumnTermId($linker_table, $column, 'NCIT:C25712');
          if ($term) {
            $extra_linker_columns[$column] = $term;
          }
        }
      }
    }

    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $stockcollection_type_len = $cvterm_schema_def['fields']['name']['size'];

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, $field_id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the Drupal entity ID of the linked chado
    // record, if one exists.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, $field_id, 'entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => self::$object_table,
      'fkey' => $linker_fkey_column,
    ]);

    // Define the linker table that links the base table to the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, $field_id, 'linker_id', self::$record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => $linker_table . '.' . $linker_pkey_col,
    ]);

    // Define the link between the base table and the linker table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, $field_id, 'link', $linker_left_term, [
      'action' => 'store_link',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col,
    ]);

    // Define the link between the linker table and the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, $field_id, self::$object_id, $linker_fkey_term, [
      'action' => 'store',
      'drupal_store' => TRUE,
      'path' => $linker_fkey_path,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // Other columns in the linker table. Set in the widget, but currently
    // not implemented in the formatter. Typically these are type_id and
    // rank, but are not present in all linker tables, so they are added
    // only if present in the linker table.
    foreach ($extra_linker_columns as $column => $term) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, $field_id, 'linker_' . $column, $term, [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.' . $column,
        'as' => 'linker_' . $column,
      ]);
    }

    // The object table, the destination table of the linker table.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, $field_id, 'stockcollection_name', $name_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';name',
      'as' => 'stockcollection_name',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, $field_id, 'stockcollection_uniquename', $uniquename_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';uniquename',
      'as' => 'stockcollection_uniquename',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, $field_id, 'stockcollection_type', $stockcollection_type_term, $stockcollection_type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
      . ';' . $object_table . '.type_id>cvterm.cvterm_id;name',
      'as' => 'stockcollection_type',
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   *
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

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface::discover()
   */
  public static function discover(
    TripalEntityType $bundle,
    string $field_id,
    array $field_types,
    array $field_instances,
    array $options = [],
  ): array {

    // Specific settings for this field.
    $options += [
      'id' => self::$id,
      'table' => self::$object_table,
      'label' => 'Germplasm Collection',
      'termIdSpace' => 'OBI',
      'termAccession' => '0002076',
      'description' => 'A material entity that has two or more specimens as its parts.',
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);

    return $field_list;
  }

}
