<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Plugin implementation of Default Tripal field for sequence data.
 *
 * @FieldType(
 *   id = "chado_source_data_type_default",
 *   label = @Translation("Chado Data Source"),
 *   description = @Translation("The source and version of data used for this analysis"),
 *   default_widget = "chado_source_data_widget_default",
 *   default_formatter = "chado_source_data_formatter_default",
 *   cardinality = 1,
 * )
 */
class ChadoSourceDataTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_source_data_type_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName()  {
    return 'sourcename';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings()  {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'local';
    $settings['termAccession'] = 'source_data';
    $settings['termFixed'] = FALSE;
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
  public static function tripalTypes($field_definition)  {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $record_id_term = $mapping->getColumnTermId('analysis', 'analysis_id');

    $src_uri_term = $mapping->getColumnTermId('analysis', 'sourceuri');
    $src_name_term = $mapping->getColumnTermId('analysis', 'sourcename');
    $src_vers_term = $mapping->getColumnTermId('analysis', 'sourceversion');

    // Get property terms using Chado table columns they map to. Return the properties for this field.
    $properties = [];

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => 'analysis',
      'chado_column' => 'analysis_id',
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'sourceuri', $src_uri_term, 100, [
      'action' => 'store',
      'chado_table' => 'analysis',
      'chado_column' => 'sourceuri',
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'sourcename', $src_name_term, 200, [
      'action' => 'store',
      'chado_table' => 'analysis',
      'chado_column' => 'sourcename',
      'delete_if_empty' => TRUE,
      'empty_value' => '',
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'sourceversion', $src_vers_term, 100, [
      'action' => 'store',
      'chado_table' => 'analysis',
      'chado_column' => 'sourceversion',
    ]);

    return ($properties);
  }
}
