<?php

namespace Drupal\tripal_file\Plugin\Field\FieldType;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal file location field type.
 */
#[FieldType(
  id: 'tripal_file_location_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('File Location'),
  description: new TranslatableMarkup('The location of the file.'),
  default_widget: 'tripal_file_location_widget_default',
  default_formatter: 'tripal_file_location_formatter_default',
)]
class TripalFileLocationTypeDefault extends ChadoFieldItemBase {

  /**
   * The machine name of this field.
   *
   * @var string
   */
  public static $id = 'tripal_file_location_type_default';

  /**
   * The name of the table linked to from the base table.
   *
   * @var string
   */
  protected static $object_table = 'fileloc';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'fileloc_uri';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['base_table'] = 'file';
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'itemLocation'.
    $field_settings['termIdSpace'] = 'schema';
    $field_settings['termAccession'] = 'itemLocation';
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

    // Get the various tables and columns needed for this field. We will
    // get the property terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Base table.
    $base_pkey_col = self::getPrimaryKey($schema, $base_table);

    // The fileloc table.
    $object_table = self::$object_table;
    $object_schema = self::getChadoTableDef($schema, $object_table);
    $object_pkey_col = $object_schema['primary key'];

    // Columns specific to the fileloc table.
    $uri_term = self::getColumnTermId($object_table, 'uri', 'data:1047');
    $rank_term = self::getColumnTermId($object_table, 'rank', 'OBCS:0000117');
    $md5checksum_term = self::getColumnTermId($object_table, 'md5checksum', 'data:2190');
    $md5checksum_len = $object_schema['fields']['md5checksum']['length'];
    $size_term = self::getColumnTermId($object_table, 'size', 'schema:filesize');
    $size_len = $object_schema['fields']['size']['length'];
    $filename_term = self::getColumnTermId($object_table, 'filename', 'data:1050');

    // The fileloc table has a file_id column which is how it
    // links to the file table.
    $object_fkey_col = 'file_id';

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // The link to the fileloc table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'fileloc_id', self::$record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_pkey_col,
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', self::$record_id_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_fkey_col,
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'fileloc_uri', $uri_term, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_fkey_col . ';uri',
      'as' => 'fileloc_uri',
      'delete_if_empty' => TRUE,
      'empty_value' => '',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'fileloc_rank', $rank_term, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_fkey_col . ';rank',
      'as' => 'fileloc_rank',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'fileloc_md5checksum', $md5checksum_term, $md5checksum_len, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_fkey_col . ';md5checksum',
      'as' => 'fileloc_md5checksum',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'fileloc_size', $size_term, $size_len, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_fkey_col . ';size',
      'as' => 'fileloc_size',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'fileloc_filename', $filename_term, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $object_table . '.' . $object_fkey_col . ';filename',
      'as' => 'fileloc_filename',
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = FALSE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    if ($base_table == 'file') {
      $compatible = TRUE;
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
      'label' => 'File Location',
      'termIdSpace' => 'schema',
      'termAccession' => 'itemLocation',
      'description' => 'The location of the file.',
      'cardinality' => -1,
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);
    return $field_list;
  }

}
