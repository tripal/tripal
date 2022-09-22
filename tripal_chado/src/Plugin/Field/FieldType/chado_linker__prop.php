<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;


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
class chado_linker__prop extends ChadoFieldItemBase {

  public static $id = "chado_linker__prop";


  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    $property_settings = $settings['property_settings'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return TripalFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    }

    // Get the primary key and FK columns.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $chado_table = $property_settings['value']['chado_table'];
    $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);
    $fk_col = array_keys($schema_def['foreign keys'][$base_table]['columns'])[0];
    $pk_col = $schema_def['primary key'];


    // Get the property term IDs. We'll use these as the property names.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $rank_term = $mapping->getColumnTermId($chado_table, 'rank');
    $type_term = $mapping->getColumnTermId($chado_table, 'type_id');
    $fk_term = $mapping->getColumnTermId($chado_table, $fk_col);

    // Indicate the action to perform for each property.
    $value_settings = $settings['property_settings']['value'];
    $rec_id_settings = [
      'chado_table' => $chado_table,
      'chado_column' => $pk_col,
    ];
    $rank_settings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => 'rank'
    ];
    $type_settings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => 'type_id'
    ];
    $fk_id_settings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => $fk_col,
    ];

    // Create the property types.
    $value = new TextStoragePropertyType($entity_type_id, self::$id, 'value', $value_settings);
    $rank = new IntStoragePropertyType($entity_type_id, self::$id, $rank_term,  $rank_settings);
    $type_id = new IntStoragePropertyType($entity_type_id, self::$id, $type_term,  $type_settings);
    $fk_id = new IntStoragePropertyType($entity_type_id, self::$id, $fk_term,  $fk_id_settings);

    // All Tripal fields have a record_id property and if the chado_table is
    // not specified then it defaults to the base table. We need to store the
    // record_id of the prop table so change the record_id settings.
    $default_types = ChadoFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    $default_types[0]->setStorageSettings($rec_id_settings);

    // Return the list of property types.
    $types = [$value, $rank, $type_id, $fk_id];
    $types = array_merge($types, $default_types);
    return $types;
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate() {

    // Get the field settings.
    $field_definition = $this->getFieldDefinition();
    $settings = $field_definition->getSettings();
    $termIdSpace = $settings['termIdSpace'];
    $termAccession = $settings['termAccession'];

    // Get the Entity information.
    $entity = $this->getEntity();
    $entity_type_id = $entity->getEntityTypeId();
    $entity_id = $entity->id();

    // Get the storage settings.
    $property_settings = $this->getSettings();
    $chado_table = $property_settings['storage_plugin_settings']['property_settings']['value']['chado_table'];
    $base_table = $property_settings['storage_plugin_settings']['base_table'];

    // Get the FK column in the base table that links to the base table.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);
    $fk_col = array_keys($schema_def['foreign keys'][$base_table]['columns'])[0];

    // Get the property term IDs. We'll use these as the property names.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $rank_term = $mapping->getColumnTermId($chado_table, 'rank');
    $type_term = $mapping->getColumnTermId($chado_table, 'type_id');
    $fk_term = $mapping->getColumnTermId($chado_table, $fk_col);

    // Build the values array.
    $value = new StoragePropertyValue($entity_type_id, self::$id, 'value', $entity_id);
    $rank = new StoragePropertyValue($entity_type_id, self::$id, $rank_term, $entity_id);
    $type_id = new StoragePropertyValue($entity_type_id, self::$id, $type_term, $entity_id);
    $fk_id = new StoragePropertyValue($entity_type_id, self::$id, $fk_term, $entity_id);

    // Build the array of values to return.
    $values = [$value, $rank, $type_id, $fk_id];
    $default_values = ChadoFieldItemBase::defaultTripalValuesTemplate($entity_type_id, self::$id, $entity_id);
    $values = array_merge($values, $default_values);
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {

    // We need to set the prop table for this field but we need to know
    // the base table to do that. So we'll add a new validation function so
    // we can get it and set the proper storage settings.
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $elements['storage_plugin_settings']['base_table']['#element_validate'] = [[static::class, 'storageSettingsFormValidate']];
    return $elements;
  }

  /**
   * Form element validation handler
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   */
  public static function storageSettingsFormValidate(array $form, FormStateInterface $form_state) {
    $settings = $form_state->getValue('settings');
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    $base_table = $settings['storage_plugin_settings']['base_table'];
    $prop_table = $base_table . 'prop';

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    if ($schema->tableExists($prop_table)) {
      $form_state->setValue(['settings','storage_plugin_settings','property_settings','value','action'], 'store');
      $form_state->setValue(['settings','storage_plugin_settings','property_settings','value','chado_table'], $prop_table);
      $form_state->setValue(['settings','storage_plugin_settings','property_settings','value','chado_column'], 'value');
    }
    else {
      $form_state->setErrorByName('storage_plugin_settings][base_table',
          'The selected base table does not have an associated property table.');
    }
  }
}