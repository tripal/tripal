<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of Default Tripal field for sequence length.
 */
#[TripalFieldType(
  id: 'chado_sequence_length_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Feature Sequence Length'),
  description: new TranslatableMarkup('A chado feature sequence length'),
  default_widget: 'chado_sequence_length_widget_default',
  default_formatter: 'chado_sequence_length_formatter_default',
)]
class ChadoSequenceLengthTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_sequence_length_type_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'seqlen';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'data';
    $settings['termAccession'] = '1249';
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
    $seqlen_term = self::getColumnTermId('feature', 'seqlen', 'data:1249');

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
      'base_column' => 'seqlen',
      'label' => 'Sequence Length',
      'termIdSpace' => 'data',
      'termAccession' => '1249',
      'description' => 'The size (length) of a sequence, subsequence or region in a sequence, or range(s) of lengths.',
    ];

    // Call the parent discover() with this field's specific options
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);
    return $field_list;
  }

}
