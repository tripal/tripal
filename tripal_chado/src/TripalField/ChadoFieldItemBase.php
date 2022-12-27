<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;


/**
 * Defines the Tripal field item base class.
 */
abstract class ChadoFieldItemBase extends TripalFieldItemBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_id'] = 'chado_storage';
    $settings['storage_plugin_settings']['base_table'] = '';
    return $settings;
  }


  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $tables = $schema->getTables(['type' => 'table', 'status' => 'base']);

    $is_disabled = FALSE;
    $storage_settings = $this->getSetting('storage_plugin_settings');
    $default_base_table = array_key_exists('base_table', $storage_settings) ? $storage_settings['base_table'] : '';
    if ($default_base_table) {
      $is_disabled = TRUE;
    }

    // Find base tables.
    $base_tables = [];
    $base_tables[NULL] = '-- Select --';
    foreach (array_keys($tables) as $table) {
      $base_tables[$table] = $table;
    }

    $elements['storage_plugin_settings']['base_table'] = [
      '#type' => 'select',
      '#title' => t('Chado Base Table'),
      '#description' => t('Select the base table in Chado to which this property belongs. ' .
        'For example. If this property is meant to store a feature property then the base ' .
        'table should be "feature".'),
      '#options' => $base_tables,
      '#default_value' => $default_base_table,
      '#required' => TRUE,
      "#disabled" => $is_disabled
    ];

    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
  }
}