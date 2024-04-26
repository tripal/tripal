<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of Default Tripal field for sequence data.
 *
 * @FieldType(
 *   id = "chado_sequence_coordinates_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Sequence Coordinates"),
 *   description = @Translation("Locations on reference sequences where the feature is located"),
 *   default_widget = "chado_sequence_coordinates_widget_default",
 *   default_formatter = "chado_sequence_coordinates_formatter_default",
 *   cardinality = 1,
 * )
 */
class ChadoSequenceCoordinatesDefault extends ChadoFieldItemBase {

  public static $id = "chado_sequence_coordinates_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'sequence_coordinates';
  }

  /**
  * {@inheritdoc}
  */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'data';
    $settings['termAccession'] = '2012';
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

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    $field_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $field_settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load( 'core_mapping' );

    $record_id_term = $mapping->getColumnTermId( 'feature', 'feature_id' );
    $ft_uniqname_term = $mapping->getColumnTermId( 'feature', 'name' );

    $srcfeature_id_term = $mapping->getColumnTermId('featureloc', 'srcfeature_id');
    $fmin_term = $mapping->getColumnTermId('featureloc', 'fmin');
    $is_fmin_partial_term = $mapping->getColumnTermId('featureloc', 'is_fmin_partial');
    $fmax_term = $mapping->getColumnTermId('featureloc', 'fmax');
    $is_fmax_partial_term = $mapping->getColumnTermId('featureloc', 'is_fmax_partial');
    $strand_term = $mapping->getColumnTermId('featureloc', 'strand');
    $phase_term = $mapping->getColumnTermId('featureloc', 'phase');
    $residue_info_term = $mapping->getColumnTermId('featureloc', 'residue_info');
    $locgroup_term = $mapping->getColumnTermId('featureloc', 'locgroup');
    $rank_term = $mapping->getColumnTermId('featureloc', 'rank');

    // Get property terms using Chado table columns they map to. Return the properties for this field.
    $properties = [];

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => 'feature.feature_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'featureloc_id', $record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => 'featureloc.featureloc_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'fkey', $record_id_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => 'feature.feature_id>featureloc.feature_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'srcfeature_id', $srcfeature_id_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;srcfeature_id',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'uniquename', $ft_uniqname_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;featureloc.srcfeature_id>feature.feature_id;uniquename',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'fmin', $fmin_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;fmin',
    ]);

    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'is_fmin_partial', $is_fmin_partial_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;is_fmin_partial',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'fmax', $fmax_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;fmax',
    ]);

    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'is_fmax_partial', $is_fmax_partial_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;is_fmax_partial',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'strand', $strand_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;strand',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'phase', $phase_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;phase',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'residue_info', $residue_info_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;residue_info',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'locgroup', $locgroup_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;locgroup',
    ]);

    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'rank', $rank_term, [
      'action' => 'read_value',
      'path' => 'feature.feature_id>featureloc.feature_id;rank',
    ]);

    return($properties);
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

}
