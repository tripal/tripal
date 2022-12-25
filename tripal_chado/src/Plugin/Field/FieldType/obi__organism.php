<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "obi__organism",
 *   label = @Translation("Chado Organism Reference"),
 *   description = @Translation("A chado organism reference"),
 *   default_widget = "obi__organism_widget",
 *   default_formatter = "obi__organism_formatter"
 * )
 */
class obi__organism extends ChadoFieldItemBase {

  public static $id = "obi__organism";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'label';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'OBI';
    $settings['termAccession'] = '0100026';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the length of the database fields so we don't go over the size limit.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $organism_def = $schema->getTableDef('organism', ['format' => 'Drupal']);
    $cvterm_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $genus_len = $organism_def['fields']['genus']['size'];
    $species_len = $organism_def['fields']['species']['size'];
    $iftype_len = $cvterm_def['fields']['name']['size'];
    $ifname_len = $organism_def['fields']['infraspecific_name']['size'];
    $label_len = $genus_len + $species_len + $iftype_len + $ifname_len;

    // Get the base table columns needed for this field.
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $base_fk_col = array_keys($base_schema_def['foreign keys']['organism']['columns'])[0];

    // Return the properties for this field.
    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'organism_id', [
        'action' => 'store',
        'chado_table' => $base_table,
        'chado_column' => $base_fk_col,
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'label', $label_len, [
        'action' => 'replace',
        'template' => "<i>[genus] [species]</i> [infraspecific_type] [infraspecific_name]",
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'genus', $genus_len, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id',
        'chado_column' => 'genus'
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'species', $species_len, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id',
        'chado_column' => 'species'
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'infraspecific_name', $ifname_len, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id',
        'chado_column' => 'infraspecific_name',
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'infraspecific_type', [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id;organism.type_id>cvterm.cvterm_id',
        'chado_column' => 'name',
        'as' => 'infraspecific_type_name'
      ])
    ];
  }
}
