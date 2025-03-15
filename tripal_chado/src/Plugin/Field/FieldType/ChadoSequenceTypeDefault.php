<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoBpCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of Default Tripal field for sequence data.
 *
 * @FieldType(
 *   id = "chado_sequence_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Sequence Residues"),
 *   description = @Translation("Manages sequence residues for content types storing data in the chado feature table."),
 *   default_widget = "chado_sequence_widget_default",
 *   default_formatter = "chado_sequence_formatter_default",
 *   cardinality = 1,
 * )
 */
class ChadoSequenceTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_sequence_type_default";

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
    $settings['storage_plugin_settings']['base_table'] = 'feature';

    return $settings;
  }

  public static function retrieveSequenceUsingService (): string {
    $sequence="Hello World!";
    // Use params to extract info needed for the service.
/*
    // Call the service to get the sequence.
    $helper = \Drupal::service('tripal_chado.genomic_feature_helper');
    $sequence = $helper->getSeq($entity->id());
*/
    // Can't remember what Chadostorage expects for a return value but I think it's the
    //  value that should be applied to the property.
    return $sequence;
  }

  public static function foo(): string {
    \Drupal::messenger()->addMessage("Now a field should show foo");
    return "foo";
  }
  public static function foo1(): string {
    \Drupal::messenger()->addMessage("Now a field should show foo 1");
    return "foo 1";
  }

  /**
   * {@inheritdoc}
   */
 public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the property terms by using the Chado table columns they map to.
    $residues_term = self::getColumnTermId('feature', 'residues', 'data:2044');
    $seqlen_term = self::getColumnTermId('feature', 'seqlen', 'data:1249');
    $md5checksum_term = self::getColumnTermId('feature', 'md5checksum', 'data:2190');

    // Return the properties for this field.
    $properties = [];


    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'path' => 'feature.feature_id',
    ]);

    $properties[] =  new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'residues', $residues_term, [

      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => '\Drupal\tripal_chado\Plugin\Field\FieldType\ChadoSequenceTypeDefault',
      'function' => "foo1",
    ]);

    $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'seqlen', $seqlen_term, [
      'action' => 'store',
      'path' => 'feature.seqlen',
    ]);

    // Hard-coded as the length of MD5Checksum supported by the chado feature.md5checksum column.
    $md5checksum_len = 32;


    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'derived_sequence','SO:0000001', [
      'action' => 'function',
      'drupal_store' => FALSE,
      'namespace' => '\Drupal\tripal_chado\Plugin\Field\FieldType\ChadoSequenceTypeDefault',
      'function' => "foo1",

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
    if ($base_table == 'feature') {
      $compatible = TRUE;
    }
    return $compatible;
  }

}
