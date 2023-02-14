<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
#use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Default Tripal field for sequence data.
 *
 * @FieldType(
 *   id = "chado_sequence_default",
 *   label = @Translation("Chado Feature Sequence"),
 *   description = @Translation("A chado feature sequence"),
 *   default_widget = "chado_sequence_widget_default",
 *   default_formatter = "chado_sequence_formatter_default"
 * )
 */
class ChadoSequenceDefault extends ChadoFieldItemBase {

  public static $id = "chado_sequence_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'residues';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'data';
    $settings['termAccession'] = '2044';
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
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the base table columns needed for this field.
    $settings = $field_definition->getSetting('storage_plugin_settings');

    // Get the property terms by using the Chado table columns they map to.

    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $residues_term = $mapping->getColumnTermId('feature', 'residues');
    $seqlen_term = $mapping->getColumnTermId('feature', 'seqlen');
    $md5checksum_term = $mapping->getColumnTermId('feature', 'md5checksum');

    // Return the properties for this field.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => 'feature',
        'chado_column' => 'feature_id'
    ]);
    $properties[] =  new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'residues', $residues_term, [
      'action' => 'store',
      'chado_column' => 'residues',
      'chado_table' => 'feature'
    ]);
    $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'seqlen', $seqlen_term, [
      'action' => 'store',
      'chado_column' => 'seqlen',
      'chado_table' => 'feature'
    ]);
    $properties[] =  new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'md5checksum', $md5checksum_term, [
      'action' => 'store',
      'chado_column' => 'md5checksum',
      'chado_table' => 'feature'
    ]);
    return $properties;
  }
}
