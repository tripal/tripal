<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "chado_linker__prop",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   default_widget = "chado_linker__prop_widget",
 *   default_formatter = "chado_linker__prop_formatter"
 * )
 */
class chado_linker__prop extends TripalFieldItemBase {

  public static $id = "chado_linker__prop";


  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    $chado = \Drupal::service('tripal_chado.database');
    /**
     *
     * @var \Drupal\tripal\TripalDBX\TripalDbxSchema $schema
     */
    $schema = $chado->schema();
    $tables = $schema->getTables(['type' => 'table', 'status' => 'base']);

    // Find base tables.
    $base_tables = [];
    foreach (array_keys($tables) as $table) {
      $base_tables[$table] = $table;
    }

    $elements['base_table'] = [
      '#type' => 'select',
      '#title' => t('Chado Base Table'),
      '#description' => t('Select the base table in Chado to which this property belongs. For example. If this property is meant to store a feature property then the base table should be "feature".'),
      '#options' => $base_tables,
      '#default_value' => '',
    ];

    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes(FieldStorageDefinitionInterface $field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');
    if (!array_key_exists('property_settings', $settings)) {
      return TripalFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    }
    $property_settings = $settings['property_settings'];


    $chado_table = $property_settings['value']['chado_table'];

    // Get the property term IDs. We'll use these as the property names.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $value_term = $mapping->getColumnTermId($chado_table, 'value');
    $rank_term = $mapping->getColumnTermId($chado_table, 'rank');
    $type_term = $mapping->getColumnTermId($chado_table, 'type_id');
    $feature_term = $mapping->getColumnTermId($chado_table, 'feature_id');

    // Indicate the action to perform for each property.
    $value_settings = $settings['property_settings']['value'];
    $rank_settings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => 'genus'
    ];
    $type_settings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => 'species'
    ];
    $feature_settings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => 'name',
    ];

    // Create the property types.
    $value = new TextStoragePropertyType($entity_type_id, self::$id, 'value', $value_settings);
    $rank = new IntStoragePropertyType($entity_type_id, self::$id, $rank_term,  $rank_settings);
    $type_id = new IntStoragePropertyType($entity_type_id, self::$id, $type_term,  $type_settings);
    $feature_id = new IntStoragePropertyType($entity_type_id, self::$id, $feature_term,  $feature_settings);

    // Return the list of property types.
    $types = [$value, $rank, $type_id, $feature_id];
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
    $value_term = $mapping->getColumnTermId($chado_table, 'value');
    $rank_term = $mapping->getColumnTermId($chado_table, 'rank');
    $type_term = $mapping->getColumnTermId($chado_table, 'type_id');
    $feature_term = $mapping->getColumnTermId($chado_table, 'feature_id');

    // Build the values array.
    $values = [
      new StoragePropertyValue($entity_type_id, self::$id, 'value', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $rank_term, $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $type_term, $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, $feature_term, $entity_id),
    ];
    $default_values = TripalFieldItemBase::defaultTripalValuesTemplate($entity_type_id, self::$id, $entity_id);
    $values = array_merge($values, $default_values);
    return $values;
  }

}