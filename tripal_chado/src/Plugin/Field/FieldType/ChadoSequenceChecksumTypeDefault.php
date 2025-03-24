<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoBpCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of Default Tripal field for sequence data.
 *
 * @FieldType(
 *   id = "chado_sequence_checksum_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Feature Sequence Checksum"),
 *   description = @Translation("A chado feature sequence md5 checksum"),
 *   default_widget = "chado_sequence_checksum_widget_default",
 *   default_formatter = "chado_sequence_checksum_formatter_default"
 * )
 */
class ChadoSequenceChecksumTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_sequence_checksum_type_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'md5checksum';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'data';
    $settings['termAccession'] = '2190';
    $settings['fixed_value'] = TRUE;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = 'feature';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the property terms by using the Chado table columns they map to.
    $md5checksum_term = self::getColumnTermId('feature', 'md5checksum', 'data:2190');
    $seqlen_term = self::getColumnTermId('feature', 'seqlen', 'data:1249');

    // Get the length of the database fields so we don't go over the size limit.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $feature_def = $schema->getTableDef('feature', ['format' => 'Drupal']);
    $md5_checksum_len = $feature_def['fields']['md5checksum']['size'];

    // Return the properties for this field.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'path' => 'feature.feature_id',
    ]);
    $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'seqlen', $seqlen_term, [
      'action' => 'read_value',
      'path' => 'feature.seqlen',
    ]);
    $properties[] =  new ChadoBpCharStoragePropertyType($entity_type_id, self::$id, 'md5checksum', $md5checksum_term, $md5_checksum_len, [
      'action' => 'read_value',
      'path' => 'feature.md5checksum',
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
    // This is a "specialty" field for a single content type
    if ($base_table == 'feature') {
      $compatible = TRUE;
    }
    return $compatible;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface::discover()
   */
  public static function discover(TripalEntityType $bundle, string $field_id, array $field_types,
      array $field_instances, array $options = []): array {

    // Specific settings for this field
    $options += [
      'id' => self::$id,
      'base_table' => 'feature',
      'base_column' => 'md5checksum',
      'label' => 'Sequence MD5 Checksum',
      'termIdSpace' => 'data',
      'termAccession' => '2190',
      'description' => 'The 32-character checksum of the sequence, calculated using the MD5 algorithm.',
    ];

    // Call the parent discover() with this field's specific options
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);
    return $field_list;
  }

}
