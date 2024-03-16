<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of Default Tripal field for unit of measurement.
 *
 * @FieldType(
 *   id = "chado_unit_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Unit"),
 *   description = @Translation("Provide unit of measurement of content, for example, Genetic Map."),
 *   default_widget = "chado_unit_widget_default",
 *   default_formatter = "chado_unit_formatter_default"
 * )
 */

class ChadoUnitTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_unit_type_default";

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

    $field_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $field_settings['base_table'];
    if (!$base_table) {
      return;
    }

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    $cvterm_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $cv_name_len = $cvterm_def['fields']['name']['size'];

    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = $mapping->getColumnTermId('featuremap', 'featuremap_id');
    $unittype_id_term = $mapping->getColumnTermId('featuremap', 'unittype_id');
    $cv_name_term = $mapping->getColumnTermId('cvterm', 'name');

    $properties = [];

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => 'featuremap.featuremap_id',
      //'chado_table' => 'featuremap',
      //'chado_column' => 'featuremap_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'unittype_id', $unittype_id_term, [
      'action' => 'store',
      'path' => 'featuremap.unittype_id',
      //'chado_table' => 'featuremap',
      //'chado_column' => 'unittype_id',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'cv_name', $cv_name_term, $cv_name_len, [
      'action' => 'read_value',
      'path' => 'featuremap.unittype_id>cvterm.cvterm_id;name',
      //'chado_column' => 'name',
      'as' => 'cv_name'
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = TRUE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    // This is a "specialty" field for a single content type
    // If base table is not defined, assume compatible
    if (!$base_table or ($base_table == 'featuremap')) {
      $compatible = TRUE;
    }
    return $compatible;
  }

}
