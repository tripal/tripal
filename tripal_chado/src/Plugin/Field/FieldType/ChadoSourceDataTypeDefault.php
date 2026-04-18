<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of Default Tripal field for data source.
 */
#[TripalFieldType(
  id: 'chado_source_data_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Data Source'),
  description: new TranslatableMarkup('The source and version of data used for this analysis'),
  default_widget: 'chado_source_data_widget_default',
  default_formatter: 'chado_source_data_formatter_default',
  cardinality: 1,
)]
class ChadoSourceDataTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = "chado_source_data_type_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // The property that indicates if this field is empty.
    return 'sourcename';
  }

  /**
   * {@inheritdoc}
   */
  public static function mainDisplayPropertyName() {
    // The property to use in the entity title/url.
    return 'sourcename';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'local';
    $settings['termAccession'] = 'source_data';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = 'analysis';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $value['record_id'] = 0;
    $value['sourceuri'] = '';
    $value['sourcename'] = '';
    $value['sourceversion'] = '';

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the property terms by using the Chado table columns they map to.
    $src_uri_term = self::getColumnTermId('analysis', 'sourceuri', 'data:1047');
    $src_name_term = self::getColumnTermId('analysis', 'sourcename', 'schema:name');
    $src_vers_term = self::getColumnTermId('analysis', 'sourceversion', 'IAO:0000129');

    // Get property terms using Chado table columns they map to.
    // Return the properties for this field.
    $properties = [];

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => 'analysis.analysis_id',
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'sourceuri', $src_uri_term, 100, [
      'action' => 'store',
      'path' => 'analysis.sourceuri',
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'sourcename', $src_name_term, 200, [
      'action' => 'store',
      'path' => 'analysis.sourcename',
      'delete_if_empty' => TRUE,
      'empty_value' => '',
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'sourceversion', $src_vers_term, 100, [
      'action' => 'store',
      'path' => 'analysis.sourceversion',
    ]);

    return ($properties);
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
    // This is a "specialty" field for a single content type.
    if ($base_table == 'analysis') {
      $compatible = TRUE;
    }
    return $compatible;
  }

}
