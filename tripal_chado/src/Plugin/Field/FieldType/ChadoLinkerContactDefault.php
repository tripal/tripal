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
 *   id = "chado_linker_contact_default",
 *   label = @Translation("Chado Contact"),
 *   description = @Translation("Add a linked Chado contact to the content type."),
 *   default_widget = "chado_linker_contact_widget_default",
 *   default_formatter = "chado_linker_contact_formatter_default"
 * )
 */
class ChadoLinkerContactDefault extends ChadoFieldItemBase {

  public static $id = "chado_linker_contact_default";
  public static $object_table = 'contact';
  public static $value_column = 'name';

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = '';
    $settings['storage_plugin_settings']['linker_table'] = '';
    $settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    // CV Term is 'Communication Contact'
    $settings['termIdSpace'] = 'TCONTACT';
    $settings['termAccession'] = '0000018';
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
    $record_id_term = 'TCONTACT:0000018';
    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table or !$linker_table) {
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Get the various table columns needed for this field.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    // Base table
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Object table
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];
    $object_pkey_term = $mapping->getColumnTermId($object_table, $object_pkey_col);  // @@@ same as $link_obj
    $value_term = $mapping->getColumnTermId($object_table, self::$value_column);

    // Linker table
    $link_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    $link_pkey_col = $link_schema_def['primary key'];
    // the following should be the same as $base_pkey_col
    $link_fk_col = array_keys($link_schema_def['foreign keys'][$base_table]['columns'])[0];
    // to-do check here in reverse direction for fk_col???? schema_def has a list of ['referring_tables']
    $link_object_col = $object_pkey_col;
    $link_base_term = $mapping->getColumnTermId($linker_table, $link_fk_col);  // same as $record_id_term? No this is NCIT:C47885
    $link_object_term = $mapping->getColumnTermId($linker_table, $link_object_col);
    // Rank and type_id are added only if they exist in the linker table.
    $rank_term = NULL;
    $type_id_term = NULL;
    $rank_term = $mapping->getColumnTermId($linker_table, 'rank');
    $type_id_term = $mapping->getColumnTermId($linker_table, 'type_id');


//  keys base_pkey_col="project_id" object_pkey_col="contact_id" link_pkey_col="project_contact_id"
//    link_fk_col="project_id" link_object_col="contact_id"
//  terms record_id_term="TCONTACT:0000018" link_base_term="NCIT:C47885"
//    link_object_term="local:contact" object_pkey_term="local:contact" value_term="schema:name"
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_pkey_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col,
    ]);

    // Define the link between the base table and the linker table.
    // Note that type_id and rank are not in all linker tables.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_pkey_id', $record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => $link_pkey_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_id', $link_base_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'left_table' => $base_table,
      'left_table_id' => $base_pkey_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $link_object_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'right_table' => $linker_table,
      'right_table_id' => $link_object_col,
    ]);

    // Define the link between the linker table and the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_object_id', $record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => $link_pkey_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_id', $link_object_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'left_table' => $linker_table,
      'left_table_id' => $linker_object_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $object_pkey_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'right_table' => $object_table,
      'right_table_id' => $object_pkey_col,
    ]);

    // to-do type_id and rank added conditionally only if present in linker table
//    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'rank', $rank_term,  [
//      'action' => 'store',
//      'chado_table' => $linker_table,
//      'chado_column' => 'rank'
//    ];
//    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $type_id_term, [
//      'action' => 'store',
//      'chado_table' => $linker_table,
//      'chado_column' => 'type_id'
//    ];

    // Define the object table
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_pkey_id', $object_pkey_term, [
      'action' => 'store_oid',
      'drupal_store' => TRUE,
      'chado_table' => $object_table,
      'chado_column' => $object_pkey_col,
    ]);

    // The displayed value
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
      'action' => 'join',
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $base_pkey_col
        . ';' . $linker_table . '.' . $object_pkey_col . '>' . $object_table . '.' . $object_pkey_col,
      'chado_column' => self::$value_column,
      'as' => 'value',
    ]);

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $storage_settings = $this->getSetting('storage_plugin_settings');
    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
    $object_table = self::$object_table;

    // Base tables presented here are only those that have foreign
    // keys through a linker table to our object table.
    $elements['storage_plugin_settings']['base_table']['#options'] = $this->getBaseTables($object_table);

    // In addition to base_table, we need to also specify the
    // linker_table for this field. Normally there will only be
    // one, but a site or module may have a different custom linker
    // table, so we need to provide a selection mechanism.
    // Add an ajax callback so that when the base table is selected, the
    // linker table select can be populated with candidate linker tables.
    $elements['storage_plugin_settings']['base_table']['#ajax'] = [
      'callback' =>  [$this, 'storageSettingsFormAjaxCallback'],
      'event' => 'change',
      'progress' => [
        'type' => 'throbber',
        'message' => $this->t('Retrieving linker tables...'),
      ],
      'wrapper' => 'edit-linker_table',
    ];

    $linker_is_disabled = FALSE;
    $linker_tables = [];
    $default_linker_table = array_key_exists('linker_table', $storage_settings) ? $storage_settings['linker_table'] : '';
    if ($default_linker_table) {
      $linker_is_disabled = TRUE;
      $linker_tables = [$default_linker_table => $default_linker_table];
    }
    else {
      $linker_tables = $this->getLinkerTables($object_table, $base_table);
    }
    $elements['storage_plugin_settings']['linker_table'] = [
      '#type' => 'select',
      '#title' => t('Chado Linker Table'),
      '#description' => t('Select the table that links the selected base table to the linked table. ' .
        'This is typically a combination of the two table names, but they might be in either order. ' .
        'Generally this select will have only one option, unless a module has added additional custom linker tables. ' .
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
   * Return a list of candidate base tables. This is done
   * by evaluating candidate linker tables that have a foreign
   * key to our $object_table, as well as another foreign key
   * to a different table. These different tables are returned
   * in an alphabetized list ready to use in a form select.
   *
   * @param string $object_table
   *   The Chado table being linked to (object).
   */
  protected function getBaseTables($object_table) {
    $base_tables = [];
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // Start from the primary key of the object, and work
    // back to candidate linked tables.
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];

    // Evaluate chado tables for a foreign key to our object table.
    // If it has one, and if it also has another foreign key to a
    // different table, we will consider this a candidate for
    // a base table.
    $all_tables = $schema->getTables(['type' => 'table']);
    foreach (array_keys($all_tables) as $table) {
      if ($schema->foreignKeyConstraintExists($table, $object_pkey_col)) {
        $table_schema_def = $schema->getTableDef($table, ['format' => 'Drupal']);
        if (array_key_exists('foreign keys', $table_schema_def)) {
          foreach ($table_schema_def['foreign keys'] as $fk) {
            if ($fk['table'] != $object_table) {
              $base_tables[$fk['table']] = $fk['table'];
            }
          }
        }
      }
    }
    // Alphabetize the list presented to the user.
    ksort($base_tables);

    // This should not happen, but provide an indication if it does.
    if (count($base_tables) == 0) {
      $base_tables = [NULL => '-- No base tables available --'];
    }
    // If more than one table was found, prefix the list with a Select message
    elseif (count($base_tables) > 1) {
      $base_tables = [NULL => '-- Select --'] + $base_tables;
    }

    return $base_tables;
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
  protected function getLinkerTables($object_table, $base_table) {
    $linker_tables = [];

    // The base table is needed to generate the list. We will return
    // here again from the ajax callback once that has been selected.
    if (!$base_table) {
      $linker_tables[NULL] = '-- Select base table first --';
    }
    else {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();

      $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
      $base_pkey_col = $base_schema_def['primary key'];
      $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
      $object_pkey_col = $object_schema_def['primary key'];

      $all_tables = $schema->getTables(['type' => 'table']);
      foreach (array_keys($all_tables) as $table) {
        if ($schema->foreignKeyConstraintExists($table, $base_pkey_col) and
            $schema->foreignKeyConstraintExists($table, $object_pkey_col)) {
          $linker_tables[$table] = $table;
        }
      }
      ksort($linker_tables);

      // This should not happen, but provide an indication if it does.
      if (count($linker_tables) == 0) {
        $linker_tables = [NULL => '-- No linker table available --'];
      }
      // If more than one table was found, prefix the list with a Select message
      elseif (count($linker_tables) > 1) {
        $linker_tables = [NULL => '-- Select --'] + $linker_tables;
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
    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-linker_table', $form['settings']['storage_plugin_settings']['linker_table']));
    return $response;
  }

}
