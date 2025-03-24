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

    $unittype_id_term = self::getColumnTermId('featuremap', 'unittype_id', 'UO:0000000');
    $cv_name_term = self::getColumnTermId('cvterm', 'name', 'schema:name');

    $properties = [];

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => 'featuremap.featuremap_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'unittype_id', $unittype_id_term, [
      'action' => 'store',
      'path' => 'featuremap.unittype_id',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'cv_name', $cv_name_term, $cv_name_len, [
      'action' => 'read_value',
      'path' => 'featuremap.unittype_id>cvterm.cvterm_id;name',
      'as' => 'cv_name'
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
    if ($base_table == 'featuremap') {
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
      'table' => 'cvterm',
      'label' => 'Unit Type',
      'termIdSpace' => 'UO',
      'termAccession' => '0000000',
      'description' => 'A unit of measurement is a standardized quantity of a physical quality.',
    ];

    // Call the parent discover() with this field's specific options
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);
    return $field_list;
  }

}
