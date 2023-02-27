<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;


/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "chado_linker__x",
 *   label = @Translation("Chado X"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   default_widget = "chado_linker__x_widget",
 *   default_formatter = "chado_linker__x_formatter"
 * )
 */
class chado_linker__x extends ChadoFieldItemBase {

  public static $id = "chado_linker__x";

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['linker_table'] = '';
    $settings['storage_plugin_settings']['contact_table'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    $linker_table = $settings['linker_table'];
    $contact_table = $settings['contact_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      $record_id_term = 'local:contact';
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Get the base table columns needed for this field.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $link_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    $link_pkey_col = $link_schema_def['primary key'];
    $link_fk_col = array_keys($link_schema_def['foreign keys'][$base_table]['columns'])[0];
    $link_obj_col = 'contact_id';

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'local:contact';
    $link_term = $mapping->getColumnTermId($linker_table, $link_fk_col);
    $object_term = $mapping->getColumnTermId($contact_table, $link_obj_col);
    $value_term = $mapping->getColumnTermId($contact_table, 'name');
//    $value_term = $mapping->getColumnTermId($linker_table, 'value');
//    $rank_term = $mapping->getColumnTermId($linker_table, 'rank');
//    $type_id_term = $mapping->getColumnTermId($linker_table, 'type_id');

    // Create the property types.
    $x = [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col,
      ]),

      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'prop_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $link_pkey_col,
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id',  $link_term, [
        'action' => 'store_link',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $link_fk_col,
      ]),

      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id',  $object_term, [
        'action' => 'store_link',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $link_obj_col,
      ]),

      new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'join',
        'path' => $base_table . '.project_id>' . $linker_table . '.project_id'
          . ';' . $linker_table . '.contact_id>' . $contact_table . '.contact_id',
        'chado_table' => $contact_table,
        'chado_column' => 'name',
        'as' => 'value',
//        'delete_if_empty' => TRUE,
//        'empty_value' => ''
      ]),
//      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'rank', $rank_term,  [
//        'action' => 'store',
//        'chado_table' => $linker_table,
//        'chado_column' => 'rank'
//      ]),
//      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $type_id_term, [
//        'action' => 'store',
//        'chado_table' => $linker_table,
//        'chado_column' => 'type_id'
//      ]),

    ];
    return $x;
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
    $linker_table = $base_table . 'prop';

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    if ($schema->tableExists($linker_table)) {
      $form_state->setValue(['settings', 'storage_plugin_settings', 'linker_table'], $linker_table);
      $form_state->setValue(['settings', 'storage_plugin_settings', 'contact_table'], $contact_table);
    }
    else {
      $form_state->setErrorByName('storage_plugin_settings][base_table',
          'The selected base table does not have an associated property table.');
    }
  }
}
