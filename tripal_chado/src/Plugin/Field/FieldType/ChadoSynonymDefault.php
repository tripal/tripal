<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "chado_feature_synonym_default",
 *   label = @Translation("Chado Synonym"),
 *   description = @Translation("A chado syonym"),
 *   default_widget = "chado_synonym_widget_default",
 *   default_formatter = "chado_synonym_formatter_default"
 * )
 */
class ChadoSynonymDefault extends ChadoFieldItemBase {

  public static $id = "chado_synonym_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['syn_linker_table'] = '';
    $settings['storage_plugin_settings']['syn_linker_fkey_column'] = '';
    return $settings;
  }


  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'schema';
    $settings['termAccession'] = 'alternateName';
    return $settings;
  }


  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
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

    // Check if a corresponding synonym table exists for the
    // base table.
    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
    $linker_table = $base_table . '_synonym';
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $linker_table_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    if (!$linker_table_def) {
      $form_state->setErrorByName('storage_plugin_settings][syn_linker_table',
          'The selected base table cannot support synonyms.');
    }
    else {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();
      $linker_table_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_fkey_column = array_keys($linker_table_def['foreign keys'][$base_table]['columns'])[0];
      $form_state->setvalue(['settings', 'storage_plugin_settings', 'syn_linker_table'], $linker_table);
      $form_state->setvalue(['settings', 'storage_plugin_settings', 'syn_linker_fkey_column'], $linker_fkey_column);
    }
  }


  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the settings for this field.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');

    $base_table = $storage_settings['base_table'];
    $syn_linker_table = $storage_settings['syn_linker_table'];
    $syn_linker_fkey_column = $storage_settings['syn_linker_fkey_column'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table or !$syn_linker_table) {
      $record_id_term = 'SIO:000729';
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Determine the primary key of the base table.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_table_def['primary key'];
    $synonym_table_def = $schema->getTableDef('synonym', ['format' => 'Drupal']);
    $syn_linker_table_def = $schema->getTableDef($syn_linker_table, ['format' => 'Drupal']);
    $syn_linker_table_pkey = $syn_linker_table_def['primary key'];
    $cvterm_table_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);

    // Create variables to store the terms for the properties. We can use terms
    // from Chado tables if appropriate.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';

    // Synonym table fields
    $syn_name_term = $mapping->getColumnTermId('synonym', 'name');
    $syn_name_len = $synonym_table_def['fields']['name']['size'];
    $syn_type_id_term = $mapping->getColumnTermId('synonym','type_id');
    $syn_type_name_len = $cvterm_table_def['fields']['name']['size'];


    // Synonym linker table fields
    $syn_linker_fkey_id_term = $mapping->getColumnTermId($syn_linker_table, $syn_linker_fkey_column);
    $syn_linker_synonym_id_term = $mapping->getColumnTermId($syn_linker_table, 'synonym_id');
    $syn_linker_is_current_term = $mapping->getColumnTermId($syn_linker_table, 'is_current');
    $syn_linker_is_internal_term = $mapping->getColumnTermId($syn_linker_table, 'is_internal');
    $syn_linker_pub_id_term = $mapping->getColumnTermId($syn_linker_table, 'pub_id');

    // Always store the record id of the base record that this field is
    // associated with in Chado.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col
    ]);

    //
    // Properties corresponding to the synonym linker table.
    //
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_pkey_id', $syn_linker_synonym_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'chado_table' => $syn_linker_table,
      'chado_column' => $syn_linker_table_pkey,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_base_fkey_id' , $syn_linker_fkey_id_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'chado_table' => $syn_linker_table,
      'chado_column' => $syn_linker_fkey_column,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_synonym_fkey_id' , $syn_linker_fkey_id_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'chado_table' => $syn_linker_table,
      'chado_column' => 'synonym_id',
    ]);
    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'is_current', $syn_linker_is_current_term, [
      'action' => 'store',
      'chado_table' => $syn_linker_table,
      'drupal_store' => FALSE,
      'chado_column' => 'is_current',
      'empty_value' => TRUE
    ]);
    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'is_internal', $syn_linker_is_internal_term, [
      'action' => 'store',
      'chado_table' => $syn_linker_table,
      'drupal_store' => FALSE,
      'chado_column' => 'is_internal',
      'empty_value' => FALSE
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_pub_id' , $syn_linker_pub_id_term, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'chado_table' => $syn_linker_table,
      'chado_column' => 'pub_id',
    ]);

    //
    // Properties corresponding to the synonym table.
    //
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'name', $syn_name_term, $syn_name_len, [
      'action' => 'join',
      'path' => $syn_linker_table . '.synonym_id>synonym.synonym_id',
      'chado_column' => 'name',
      'as' => 'synonym_name',
      'drupal_store' => FALSE,
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'synonym_type', $syn_type_id_term, $syn_type_name_len, [
      'action' => 'join',
      'path' => $syn_linker_table . '.synonym_id>synonym.synonym_id;synonym.type_id>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'synonym_type',
      'drupal_store' => FALSE,
    ]);


    return $properties;
  }

}