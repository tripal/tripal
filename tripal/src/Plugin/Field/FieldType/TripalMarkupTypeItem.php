<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\Component\Utility\Random;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
// Make sure to include the Property type class you are going to create
// in your addTypes() method below.
use Drupal\tripal\TripalStorage\BoolStoragePropertyType;
use Drupal\tripal\TripalStorage\ComputedStoragePropertyType;

/**
 * Plugin implementation of the 'Tripal Markup' field type.
 */
#[TripalFieldType(
  id: 'tripal_markup',
  category: 'tripal',
  label: new TranslatableMarkup('Tripal Markup'),
  description: new TranslatableMarkup('Adds static markup (e.g. instructions) to the form or view display.'),
  default_widget: 'tripal_markup_widget',
  default_formatter: 'tripal_markup_formatter',
)]
class TripalMarkupTypeItem extends TripalFieldItemBase {

  /**
   * The unique identifier of this field.
   *
   * NOTE: must match the id in the Attribute.
   *
   * @var string
   */
  public static string $id = "tripal_markup";

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
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    return [
      new BoolStoragePropertyType($entity_type_id, self::$id, 'has_value', 'owl:hasValue', []),
      new ComputedStoragePropertyType($entity_type_id, self::$id, 'value', 'schema:WebPageElement', [
        'callback' => 'TripalMarkupTypeItem::getMarkupValue',
      ]),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $values = [];

    // Generate a random value to use as a sample.
    $random = new Random();
    $values['value'] = $random->sentences(40);

    return $values;
  }

}
