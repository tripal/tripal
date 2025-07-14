<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\Services\ChadoPhylotree;

/**
 * Plugin implementation of the 'phylotreevisualization' field type for Chado.
 */
#[FieldType(
  id: 'chado_phylotree_vis_type_default',
  label: new TranslatableMarkup('Chado Phylogenetic Visualization Field Type'),
  description: new TranslatableMarkup('Visualization of a phylogenetic tree.'),
  default_formatter: 'chado_phylotree_vis_formatter_default',
  cardinality: 1,
)]
/**
 * Plugin implementation of the 'phylotreevisualization' field type for Chado.
 *
 * @FieldType(
 *   id = "chado_phylotree_vis_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Phylogenetic Visualization Field Type"),
 *   description = @Translation("Visualization of a phylogenetic tree."),
 *   default_formatter = "chado_phylotree_vis_formatter_default",
 *   cardinality = 1,
 * )
 */
class ChadoPhylotreeVisTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_phylotree_vis_type_default";

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = 'phylotree';
    $settings['storage_plugin_settings']['base_column'] = 'phylotree_id';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'EDAM:Phylogenetic tree rendering'
    $field_settings['termIdSpace'] = 'operation';
    $field_settings['termAccession'] = '0567';
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create a variable for easy access to settings.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'] ?? 'phylotree';
    $base_pkey_col = $storage_settings['base_column'] ?? 'phylotree_id';
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $tree_vis_term = 'operation:0567';

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the json tree representation.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'tree_json', $tree_vis_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => __CLASS__,  // 'Drupal\tripal_chado\Plugin\Field\FieldType\ChadoPhylotreeVisTypeDefault',
      'function' => 'getTreeJson',
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
    if ($base_table == 'phylotree') {
      $compatible = TRUE;
    }

    return $compatible;
  }

  /**
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   */
  public static function getTreeJson(array $context): string {
    $field_name = $context['field_name'];
    $delta = $context['delta'];
    // This will hold each of the tripalTypes values.
    $values = $context['values'][$field_name][$delta];
    // This retrieves the phylotree_id value.
    $record_id = $values['record_id']['value']->getValue();
dpm("CPT5 getTreeJson() callback: field_name=$field_name delta=$delta phylotree_id=$record_id");//@@@

    $chado = \Drupal::service('tripal_chado.database');
    $phylotree = new ChadoPhylotree($chado);
    $json = $phylotree->getTreeJson($record_id);
    return $json;
  }

}
