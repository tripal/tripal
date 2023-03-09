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
use Drupal\Core\Ajax\AjaxResponse;
//use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;


/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "chado_linker__x",
 *   label = @Translation("Chado X"),
 *   description = @Translation("Add a linked Chado contact to the content type."),
 *   default_widget = "chado_linker__x_widget",
 *   default_formatter = "chado_linker__x_formatter"
 * )
 */
class chado_linker__x extends ChadoFieldItemBase {

  public static $id = "chado_linker__x";
  public static $object_table = 'contact';

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'local';
    $settings['termAccession'] = 'contact';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = '';
    $settings['storage_plugin_settings']['linker_table'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    // ^@@@ above returns 'tripal_entity'
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    $linker_table = $settings['linker_table'];
    $object_table = self::$object_table;

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table or !$linker_table) {
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
    // to-do check here in reverse direction for fk_col????
    $link_obj_col = 'contact_id';

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'local:contact';
    $link_term = $mapping->getColumnTermId($linker_table, $link_fk_col);
    $object_term = $mapping->getColumnTermId($object_table, $link_obj_col);
    $value_term = $mapping->getColumnTermId($object_table, 'name');
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

      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $link_pkey_col,
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_id', $link_term, [
        'action' => 'store_link',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $link_fk_col,
      ]),
//      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $link_term, [
//        'action' => 'store',
//        'chado_table' => $linker_table,
//        'chado_column' => $link_fk_col,
//      ]),

      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $object_term, [
        'action' => 'store_link',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $link_obj_col,
      ]),
//      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $object_term, [
//        'action' => 'store',
//        'chado_table' => $linker_table,
//        'chado_column' => $link_obj_col,
//      ]),

      new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'join',
        'path' => $base_table . '.project_id>' . $linker_table . '.project_id'
          . ';' . $linker_table . '.contact_id>' . $object_table . '.contact_id',
        'chado_table' => $object_table,
        'chado_column' => 'name',
        'as' => 'value',
        'delete_if_empty' => TRUE,
        'empty_value' => ''
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
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $storage_settings = $this->getSetting('storage_plugin_settings');

    // In addition to base_table, we need to also specify the
    // linker_table for this field.

    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
    $object_table = self::$object_table;

    // Add an ajax callback so that when the base table is selected, the
    // linker table field can be populated with candidate linker tables.
    $elements['storage_plugin_settings']['base_table']['#ajax'] = [
      'callback' =>  [$this, 'storageSettingsFormAjaxCallback'],
      'event' => 'change',
      'progress' => [
        'type' => 'throbber',
        'message' => $this->t('Retrieving linker tables...'),
      ],
      'wrapper' => 'edit-linker_table',
    ];
// maybe have the focus transferred to the linker table. but how?
//    $elements['storage_plugin_settings']['base_table']['#attributes'] = [
//      'data-disable-refocus' => 'true',
//    ];

    $linker_tables = $this->getLinkerTables($base_table, $object_table);
    $linker_is_disabled = FALSE;
    $default_linker_table = array_key_exists('linker_table', $storage_settings) ? $storage_settings['linker_table'] : '';
    if ($default_linker_table) {
      $linker_is_disabled = TRUE;
    }
    $elements['storage_plugin_settings']['linker_table'] = [
      '#type' => 'select',
      '#title' => t('Chado Linker Table'),
      '#description' => t('Select the table that links the selected base table to the linked table. ' .
        'This is typically a combination of the two table names, but they might be in either order. ' .
        'For example to link "feature" to "contact", the linker table would be "feature_contact".'),
      '#options' => $linker_tables,
      '#default_value' => $default_linker_table,
      '#required' => TRUE,
      '#disabled' => $linker_is_disabled,
      '#prefix' => '<div id="edit-linker_table">',
      '#suffix' => '</div>',
    ];

    // Add new validation functions for each selected table so we can
    // check validity and set the proper storage settings.
    $elements['storage_plugin_settings']['base_table']['#element_validate'] = [[static::class, 'storageSettingsFormValidateBaseTable']];
    $elements['storage_plugin_settings']['linker_table']['#element_validate'] = [[static::class, 'storageSettingsFormValidateLinkerTable']];

    return $elements;
  }

  /**
   * Form element validation handler for base table
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   */
  public static function storageSettingsFormValidateBaseTable(array $form, FormStateInterface $form_state) {
    $settings = $form_state->getValue('settings');
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    $base_table = $settings['storage_plugin_settings']['base_table'];
    $object_table = self::$object_table;

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    if ($schema->tableExists($base_table)) {
      $form_state->setValue(['settings', 'storage_plugin_settings', 'base_table'], $base_table);
    }
    else {
      $form_state->setErrorByName('storage_plugin_settings][base_table',
          'The selected base table does not exist in Chado.');
    }
  }

  /**
   * Form element validation handler for linker table
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   */
  public static function storageSettingsFormValidateLinkerTable(array $form, FormStateInterface $form_state) {
    $settings = $form_state->getValue('settings');
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    $base_table = $settings['storage_plugin_settings']['base_table'];
    $linker_table = $settings['storage_plugin_settings']['linker_table'];
    $object_table = self::$object_table;

    // The linker table should contain both the base_table and object_table in its name.
    if (preg_match("/$base_table/", $linker_table) and preg_match("/$object_table/", $linker_table)) {
      $form_state->setValue(['settings', 'storage_plugin_settings', 'linker_table'], $linker_table);
    }
    else {
      $form_state->setErrorByName('storage_plugin_settings][linker_table',
          'The selected linker table is not appropriate for the selected base and linked tables.');
    }
  }

  /**
   * Return a list of candidate linker tables given a
   * base table and a linked table. Core tripal will only
   * ever have zero or one linker tables, but a site may
   * have custom linker tables, so we need a way to allow
   * the site administrator to pick the desired table.
   *
   * @param string $base_table
   *   The Chado table being used for the current entity (subject).
   * @param string $object_table
   *   The Chado table being linked to (object).
   */
  protected function getLinkerTables($base_table, $object_table) {
    $linker_tables = [];
    if (!$base_table) {
      $linker_tables[NULL] = '-- Select base table first --';
    }
    else {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();
      $all_tables = $schema->getTables(['type' => 'table']);

      $linker_tables[NULL] = '-- Select --';
      $ntables = 0;
      foreach (array_keys($all_tables) as $table) {
        if (preg_match("/$base_table/", $table) and preg_match("/$object_table/", $table)) {
          $linker_tables[$table] = $table;
          $ntables++;
        }
      }
      if (!$ntables) {
        $linker_tables = [NULL => '-- No linker table available --'];
      }
      elseif ($ntables == 1) {
        // When there is only one choice, remove the Select option
        array_shift($linker_tables);
      }
    }
    return $linker_tables;
  }

  /**
   * Ajax callback to update the linker table select. The select
   * can't be populated until we know the base table.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function storageSettingsFormAjaxCallback($form, &$form_state) {
    $form_state->setRebuild(TRUE);  // to-do test if this is really needed
    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-linker_table', $form['settings']['storage_plugin_settings']['linker_table']));
    return $response;
  }

}
