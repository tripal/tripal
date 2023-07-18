<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal additional type field type.
 *
 * @FieldType(
 *   id = "chado_analysis_default",
 *   label = @Translation("Analysis"),
 *   description = @Translation("Application of analytical methods to existing data of a specific type"),
 *   default_widget = "chado_analysis_widget_default",
 *   default_formatter = "chado_analysis_formatter_default",
 *   cardinality = 1
 * )
 */
class ChadoAnalysisDefault extends ChadoFieldItemBase {

  public static $id = 'chado_analysis_default';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'analysis_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'operation';
    $settings['termAccession'] = '2945';
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

    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $storage_settings = $this->getSetting('storage_plugin_settings');
    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);

    // Only present base tables that have a foreign key to analysis.
    $elements['storage_plugin_settings']['base_table']['#options'] = $this->getBaseTables('analysis');

    // Add a validation to make sure the base table has a foreign
    // key to analysis_id in the chado.analysis table.
    $elements['storage_plugin_settings']['base_table']['#element_validate'] = [[static::class, 'storageSettingsFormValidate']];
    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the Chado table and column this field maps to.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      $record_id_term = 'operation:2945';
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Get the connecting information for the foreign key from the
    // base table to the analysis table.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $analysis_table_def = $schema->getTableDef('analysis', ['format' => 'Drupal']);
    $base_pkey_col = $base_table_def['primary key'];
    $base_fkey_col = 'analysis_id';

    // Create variables to store the terms for the analysis.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $analysis_id_term = $mapping->getColumnTermId('analysis', 'analysis_id');
    $analysis_name_term = $mapping->getColumnTermId('analysis', 'name');
    $analysis_name_length = $analysis_table_def['fields']['name']['size'];

    // Define field properties, this is a simple lookup in the analysis table
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'analysis_id', $analysis_id_term, [
      'action' => 'store',
      'chado_table' => $base_table,
      'chado_column' => $base_fkey_col,
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_name', $analysis_name_term, $analysis_name_length, [
      'action' => 'join',
      'path' => $base_table . '.' . $base_fkey_col . '>analysis.analysis_id',
      'chado_column' => 'name',
      'as' => 'analysis_name',
    ]);

    return $properties;
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

    // Validation confirms that the base table has a foreign key
    // to analysis_id in the chado.analysis table.
    // This validation might be redundant, since we only present
    // valid base tables to the user, but let's play it safe.
    $base_table = $settings['storage_plugin_settings']['base_table'];
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_fkey_col = 'analysis_id';
    $fkeys = $base_table_def['foreign keys']['analysis']['columns'] ?? [];
    if (!in_array($base_fkey_col, $fkeys)) {
      $form_state->setErrorByName('storage_plugin_settings][base_table',
          'The selected base table does not have a foreign key to analysis_id,'
          . ' this field cannot be used on this content type.');
    }
  }

}
