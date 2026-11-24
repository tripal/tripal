<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;

/**
 * Plugin implementation of the 'Tripal Citation Field Type' field type.
 */
#[TripalFieldType(
  id: 'tripal_citation_type',
  category: 'tripal',
  label: new TranslatableMarkup('Tripal Citation Field Type'),
  description: new TranslatableMarkup('Add a citable publication to a content type.'),
  default_widget: 'default_tripal_citation_type_widget',
  default_formatter: 'default_tripal_citation_type_formatter',
  cardinality: 1,
)]
class TripalCitationTypeItem extends TripalFieldItemBase {

  /**
   * The unique identifier of this field.
   *
   * NOTE: must match the id in the Attribute.
   *
   * @var string
   */
  public static string $id = "tripal_citation_type";

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
      'max_length' => 255,
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
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];
    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Use the field settings to extract the information we need to make
    // generic fields.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSettings();
    $termIdSpace = $storage_settings['termIdSpace'];
    $termAccession = $storage_settings['termAccession'];

    // Use a default term if one is not set.
    $term = 'NCIT:C41196';
    if ($termIdSpace) {
      $term = $termIdSpace . ':' . $termAccession;
    }

    $max_length = $storage_settings['max_length'] ?? 255;

    return [
      new VarCharStoragePropertyType($entity_type_id, self::$id, "value", $term, $max_length),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();
    return $constraints;
  }

}
