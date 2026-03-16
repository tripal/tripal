<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal project field type.
 */
#[TripalFieldType(
  id: 'chado_project_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Project'),
  description: new TranslatableMarkup('Add a Chado project to the content type.'),
  default_widget: 'chado_project_widget_default',
  default_formatter: 'chado_project_formatter_default',
)]
class ChadoProjectTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = 'chado_project_type_default';

  /**
   * The chado table which is the object of the relationship.
   *
   * Note: this should be in all fields linking a base table to another
   * main chado table (i.e. object table).
   *
   * @var string
   */
  protected static $object_table = 'project';

  /**
   * The foreign key that links the linking table to the object table.
   *
   * Note: this should be in all fields linking a base table to another
   * main chado table (i.e. object table).
   *
   * @var string
   */
  protected static $object_id = 'project_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // The property that indicates if this field is empty.
    return self::$object_id;
  }

  /**
   * {@inheritdoc}
   */
  public static function mainDisplayPropertyName() {
    // The property to use in the entity title/url.
    return 'project_name';
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
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'Project'.
    $field_settings['termIdSpace'] = 'NCIT';
    $field_settings['termAccession'] = 'C47885';
    return $field_settings;
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

    // Do we want to conditionally include type_id and rank?
    $value['linker_type_id'] = mt_rand(1, 500);
    $value['linker_rank'] = 0;

    // Object table properties.
    $value['project_name'] = '';
    $value['project_description'] = '';

    return [$value];
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
    // We will get the terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Base table.
    $base_pkey_col = self::getPrimaryKey($base_table, $schema);

    // Object table.
    $object_table = self::$object_table;
    $object_schema_def = self::getChadoTableDef($object_table, $schema);
    $object_pkey_col = $object_schema_def['primary key'];

    // Columns specific to the object table.
    $name_term = self::getColumnTermId($object_table, 'name', 'schema:name');
    $name_len = $object_schema_def['fields']['name']['size'];
    // Text.
    $description_term = self::getColumnTermId($object_table, 'description', 'schema:description');

    // Linker table, when used, requires specifying the linker table and column.
    // For single hop, in the yaml we support using the usual 'base_table'
    // and 'base_column' settings.
    $linker_table = $storage_settings['linker_table'] ?? $base_table;
    $linker_fkey_column = $storage_settings['linker_fkey_column'] ?? $storage_settings['base_column'] ?? $object_pkey_col;

    $extra_linker_columns = [];
    $linker_fkey_term = self::getColumnTermId($linker_table, $linker_fkey_column, self::$record_id_term);
    $linker_fkey_path = $base_table . '.' . $linker_fkey_column;
    if ($linker_table != $base_table) {
      $linker_schema_def = self::getChadoTableDef($linker_table, $schema);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // The following should be the same as $base_pkey_col.
      // @todo make sure it is.
      $linker_left_col = self::getChadoForeignKeyColumn($linker_table, $base_table, $schema);
      $linker_left_term = self::getColumnTermId($linker_table, $linker_left_col, self::$record_id_term);
      $linker_fkey_path = $linker_table . '.' . $linker_fkey_column;

      // Some but not all linker tables contain rank, type_id, and maybe
      // other columns. These are conditionally added only if they exist in
      // the linker table, and if a term is defined for them.
      // @see https://github.com/GMOD/Chado/issues/140
      // No project linker tables have extra fields so this cannot yet be tested
      // however, the mentioned issue will add a type + rank.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_fkey_column)) {
          $term = self::getColumnTermId($linker_table, $column, 'NCIT:C25712');
          if ($term) {
            $extra_linker_columns[$column] = $term;
          }
        }
      }
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
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

    // Define the linker table that links the base table to the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', self::$record_id_term, [
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
      'path' => $linker_fkey_path,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // Other columns in the linker table.
    // Set in the widget, but currently not implemented in the formatter.
    // Typically these are type_id and rank, but are not present in all
    // linker tables, so they are added only if present in the linker table.
    // @see https://github.com/GMOD/Chado/issues/140
    // No project linker tables have extra fields so this cannot yet be tested
    // however, the mentioned issue will add a type + rank.
    foreach ($extra_linker_columns as $column => $term) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_' . $column, $term, [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.' . $column,
        'as' => 'linker_' . $column,
      ]);
    }

    // The object table, the destination table of the linker table
    // The project name.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'project_name', $name_term, $name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';name',
      'as' => 'project_name',
    ]);

    // The project description.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'project_description', $description_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';description',
      'as' => 'project_description',
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
      'label' => 'Project',
      'termIdSpace' => 'NCIT',
      'termAccession' => 'C47885',
      'description' => 'Any specifically defined piece of work that is undertaken or attempted to meet a single requirement.',
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);

    return $field_list;
  }

}
