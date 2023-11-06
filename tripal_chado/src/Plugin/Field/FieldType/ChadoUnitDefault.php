<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Plugin implementation of Default Tripal field for unit of measurement.
 *
 * @FieldType(
 *   id = "chado_unit_default",
 *   label = @Translation("Chado unit of measurement"),
 *   description = @Translation("Provide unit of measurement of content, for example, Genetic Map."),
 *   default_widget = "chado_unit_widget_default",
 *   default_formatter = "chado_unit_formatter_default"
 * )
 */

class ChadoUnitDefault extends ChadoFieldItemBase {

  public static $id = "chado_unit_default";
  
  /**
   * {@inheritdoc}
  */
  public static function mainPropertyName() {
    return 'unittype_id';
  }

  /**
  * {@inheritdoc}
  */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'UO';
    $settings['termAccession'] = '0000000';
    return $settings;
  }

  /**  
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition)  {

    $entity_type_id = $field_definition->getTargetEntityTypeId();

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    $ftmap_def = $schema->getTableDef('featuremap', ['format' => 'Drupal']);
    $ftmap_name_len = $ftmap_def['fields']['name']['size'];

    $cvterm_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $cv_name_len = $cvterm_def['fields']['name']['size'];

    $field_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $field_settings['base_table'];

    if (!$base_table) {
      $record_id_term = 'data:1280';
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = $mapping->getColumnTermId('featuremap', 'featuremap_id');
    $unittype_id_term = $mapping->getColumnTermId( 'featuremap', 'unittype_id' ) ;    
    $ftmap_name_term = $mapping->getColumnTermId('featuremap', 'name');
    $cv_name_term = $mapping->getColumnTermId('cvterm', 'name');
    $cvterm_id_term = $mapping->getColumnTermId( 'cvterm', 'cvterm_id' ) ;    

    $properties = [];

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => 'featuremap',
      'chado_column' => 'featuremap_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'unittype_id', $unittype_id_term, [
      'action' => 'store',
      'chado_table' => 'featuremap',
      'chado_column' => 'unittype_id',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'cv_name', $cv_name_term, $cv_name_len, [
      'action' => 'read_value',
      'path' => 'featuremap.unittype_id>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'cv_name'
    ]);

    return( $properties );
  }
  
}