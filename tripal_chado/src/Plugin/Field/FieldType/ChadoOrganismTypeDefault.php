<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal organism field type.
 *
 * @FieldType(
 *   id = "chado_organism_type_default",
 *   label = @Translation("Chado Organism Reference"),
 *   description = @Translation("A chado organism reference"),
 *   default_widget = "chado_organism_widget_default",
 *   default_formatter = "chado_organism_formatter_default"
 * )
 */
class ChadoOrganismTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_organism_type_default";

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
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the base table columns needed for this field.
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

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

    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $base_fk_col = array_keys($base_schema_def['foreign keys']['organism']['columns'])[0];

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $label_term = 'rdfs:label';
    $org_id_term = $mapping->getColumnTermId($base_table, 'organism_id');
    $genus_term = $mapping->getColumnTermId('organism', 'genus');
    $species_term = $mapping->getColumnTermId('organism', 'species');
    $iftype_term = $mapping->getColumnTermId('organism', 'type_id');
    $ifname_term = $mapping->getColumnTermId('organism', 'infraspecific_name');

    // Return the properties for this field.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
      //'chado_table' => $base_table,
      //'chado_column' => $base_pkey_col
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'organism_id', $org_id_term, [
      'action' => 'store',
      'path' => $base_table . '.' . $base_fk_col,
      //'chado_table' => $base_table,
      //'chado_column' => $base_fk_col,
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'label', $label_term, $label_len, [
      'action' => 'replace',
      'template' => "<i>[genus] [species]</i> [infraspecific_type] [infraspecific_name]",
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'genus', $genus_term, $genus_len, [
      'action' => 'read_value',
      'path' => $base_table . '.organism_id>organism.organism_id;genus',
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'species', $species_term, $species_len, [
      'action' => 'read_value',
      'path' => $base_table . '.organism_id>organism.organism_id;species',
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'infraspecific_name', $ifname_term, $ifname_len, [
      'action' => 'read_value',
      'path' => $base_table . '.organism_id>organism.organism_id;infraspecific_name',
    ]);
    $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'infraspecific_type', $iftype_term, [
      'action' => 'read_value',
      'path' => $base_table . '.organism_id>organism.organism_id;organism.type_id>cvterm.cvterm_id;name',
      'as' => 'infraspecific_type',
      //'chado_column' => 'name',
    ]);
    return $properties;
  }
}
