<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal Array Design field type.
 */
#[TripalFieldType(
  id: 'chado_array_design_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Array Design'),
  description: new TranslatableMarkup('Add a Chado Array Design to the content type.'),
  default_widget: 'chado_array_design_widget_default',
  default_formatter: 'chado_array_design_formatter_default',
)]
class ChadoArrayDesignTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = 'chado_array_design_type_default';

  /**
   * The chado table which is the object of the relationship.
   *
   * Note: this should be in all fields linking a base table to another
   * main chado table (i.e. object table).
   *
   * @var string
   */
  protected static $object_table = 'arraydesign';

  /**
   * The foreign key that links the linking table to the object table.
   *
   * Note: this should be in all fields linking a base table to another
   * main chado table (i.e. object table).
   *
   * @var string
   */
  protected static $object_id = 'arraydesign_id';

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
    return 'array_design_name';
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
    // CV Term is 'ArrayDesign'.
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
    $value[self::$object_id] = 0;

    // Object table properties.
    $value['array_design_name'] = '';
    $value['array_design_description'] = '';
    $value['array_design_version'] = '';
    $value['array_design_array_dimensions'] = '';
    $value['array_design_element_dimensions'] = '';
    $value['array_design_num_of_elements'] = 0;
    $value['array_design_num_array_columns'] = 0;
    $value['array_design_num_array_rows'] = 0;
    $value['array_design_num_grid_columns'] = 0;
    $value['array_design_num_grid_rows'] = 0;
    $value['array_design_num_sub_columns'] = 0;
    $value['array_design_num_sub_rows'] = 0;
    $value['array_design_database_accession'] = '';
    $value['array_design_database_name'] = '';
    $value['array_design_platformtype'] = '';
    $value['array_design_substratetype'] = '';
    $value['array_design_manufacturer'] = '';
    $value['array_design_protocol'] = '';

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
    // Text.
    $name_term = self::getColumnTermId($object_table, 'name', 'schema:name');
    // Text.
    $description_term = self::getColumnTermId($object_table, 'description', 'schema:description');
    // Text.
    $version_term = self::getColumnTermId($object_table, 'version', 'IAO:0000129');
    // Text.
    $array_dimensions_term = self::getColumnTermId($object_table, 'array_dimensions', 'local:array_dimensions');
    // Text.
    $element_dimensions_term = self::getColumnTermId($object_table, 'element_dimensions', 'local:element_dimensions');
    $num_of_elements_term = self::getColumnTermId($object_table, 'num_of_elements', 'local:num_of_elements');
    $num_array_rows_term = self::getColumnTermId($object_table, 'num_array_rows', 'local:num_array_rows');
    $num_array_columns_term = self::getColumnTermId($object_table, 'num_array_columns', 'local:num_array_columns');
    $num_grid_columns_term = self::getColumnTermId($object_table, 'num_grid_columns', 'local:num_grid_columns');
    $num_grid_rows_term = self::getColumnTermId($object_table, 'num_grid_rows', 'local:num_grid_rows');
    $num_sub_columns_term = self::getColumnTermId($object_table, 'num_sub_columns', 'local:num_sub_columns');
    $num_sub_rows_term = self::getColumnTermId($object_table, 'num_sub_rows', 'local:num_sub_rows');

    // Columns from linked tables
    // both platformtype and substratetype reference the cvterm table.
    $cvterm_schema_def = self::getChadoTableDef('cvterm', $schema);
    $type_term = self::getColumnTermId('cvterm', 'name', 'rdfs:type');
    $type_len = $cvterm_schema_def['fields']['name']['size'];
    $contact_schema_def = self::getChadoTableDef('contact', $schema);
    $manufacturer_term = self::getColumnTermId('contact', 'name', 'EFO:0001728');
    $manufacturer_len = $contact_schema_def['fields']['name']['size'];
    // Text.
    $protocol_term = self::getColumnTermId('protocol', 'name', 'sep:00101');
    $dbxref_term = self::getColumnTermId('dbxref', 'accession', 'data:2091');
    $db_term = self::getColumnTermId('db', 'name', 'schema:name');

    // Linker table, when used, requires specifying the linker table and column.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);
    $linker_fkey_term = self::getColumnTermId($base_table, $linker_fkey_column, self::$record_id_term);

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

    // Base table links directly.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $linker_fkey_column, $linker_fkey_term, [
      'action' => 'store',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $linker_fkey_column,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // The object table, the destination table of the linker table.
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
      'label' => 'Array Design',
      'termIdSpace' => 'EFO',
      'termAccession' => '0000269',
      'description' => 'An instrument design which describes the design of the array.',
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);

    return $field_list;
  }

}
