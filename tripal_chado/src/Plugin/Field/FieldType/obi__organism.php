<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
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
class obi__organism extends TripalFieldItemBase {

  public static $id = "obi__organism";

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
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];
    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $values = [];
    return $values;
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

    // Get the property term IDs. We'll use these as the property names.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $label_term = 'rdfs:label';
    $genus_term = $mapping->getColumnTermId('organism', 'genus');
    $species_term = $mapping->getColumnTermId('organism', 'species');
    $iftype_term = $mapping->getColumnTermId('organism', 'type_id');
    $ifname_term = $mapping->getColumnTermId('organism', 'infraspecific_name');

    // Create the property types.
    $value = new IntStoragePropertyType($entity_type_id, self::$id, 'value');
    $label = new VarCharStoragePropertyType($entity_type_id, self::$id, $label_term, $label_len,
      // @todo make sure these patterns follow typical token names.
      ['replace' => '<i>[organism_id,genus] [organism_id,species]</i> [organism_id,type_id,name] [organism_id,infraspecific_name']);
    $genus = new VarCharStoragePropertyType($entity_type_id, self::$id, $genus_term, $genus_len,
      ['expand' => 'organism_id,genus']);
    $species = new VarCharStoragePropertyType($entity_type_id, self::$id, $species_term, $species_len,
      ['expand' => 'organism_id,species']);
    $ifname = new VarCharStoragePropertyType($entity_type_id, self::$id, $ifname_term, $ifname_len,
      ['expand' => 'organism_id,infraspecific_name']);
    $iftype = new IntStoragePropertyType($entity_type_id, self::$id, $iftype_term,
      ['expand' => 'organism_id,type_id,name']);

    // Return the list of property types.
    $types = [$value, $label, $genus, $species, $ifname, $iftype];
    $default_types = TripalFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    $types = array_merge($types, $default_types);
    return $types;
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate() {

    $entity = $this->getEntity();
    $entity_type_id = $entity->getEntityTypeId();
    $entity_id = $entity->id();

    // Get the property term IDs. We'll use these as the property names.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $label_term = 'rdfs:label';
    $genus_term = $mapping->getColumnTermId('organism', 'genus');
    $species_term = $mapping->getColumnTermId('organism', 'species');
    $iftype_term = $mapping->getColumnTermId('organism', 'type_id');
    $ifname_term = $mapping->getColumnTermId('organism', 'infraspecific_name');

    // Build the values array.
    $values = [
      new StoragePropertyValue($entity_type_id, self::$id, 'value', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $label_term, $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $genus_term, $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $species_term, $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $iftype_term, $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $ifname_term, $entity_id),
    ];
    $default_values = TripalFieldItemBase::defaultTripalValuesTemplate($entity_type_id, self::$id, $entity_id);
    $values = array_merge($values, $default_values);
    return $values;
  }
}
