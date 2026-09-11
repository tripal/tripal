<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\RealStoragePropertyType;

/**
 * Plugin implementation of the 'real' field type.
 */
#[TripalFieldType(
  id: 'tripal_real_type',
  category: 'tripal',
  label: new TranslatableMarkup('Tripal Real Field Type'),
  description: new TranslatableMarkup('A real or floating point number field.'),
  default_widget: 'default_tripal_real_type_widget',
  default_formatter: 'default_tripal_real_type_formatter',
)]
class TripalRealTypeItem extends TripalFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = 'tripal_real_type';

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [];
    return $settings + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = [
      'storage_plugin_id' => 'drupal_sql_storage',
    ];
    return $settings + parent::defaultStorageSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $value['record_id'] = 0;
    $value['value'] = mt_rand(-500000, 500000) / 100;

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSettings();
    $termIdSpace = $storage_settings['termIdSpace'];
    $termAccession = $storage_settings['termAccession'];

    // Use a default term if one is not set.
    $term = 'NCIT:C25712';
    if ($termIdSpace) {
      $term = $termIdSpace . ':' . $termAccession;
    }

    return [
      new RealStoragePropertyType($entity_type_id, self::$id, 'value', $term, [], 'tripal_default_id_space'),
    ];
  }

}
