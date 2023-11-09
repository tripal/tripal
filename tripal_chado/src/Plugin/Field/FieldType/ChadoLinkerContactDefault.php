<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
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
 *   label = @Translation("Chado Linker Contact"),
 *   description = @Translation("Add a linked Chado contact to the content type."),
 *   default_widget = "chado_linker_contact_widget_default",
 *   default_formatter = "chado_linker_contact_formatter_default"
 * )
 */
class ChadoLinkerContactDefault extends ChadoFieldItemBase {

  public static $id = "chado_linker_contact_default";
  protected static $object_table = 'contact';
  protected static $object_id = 'contact_id';
  protected static $value_column = 'name';

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
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'contact_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    // CV Term is 'Communication Contact'
    $settings['termIdSpace'] = 'NCIT';
    $settings['termAccession'] = 'C47954';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId(); // 'tripal_entity'
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    $object_table = self::$object_table;
    $record_id_term = 'SIO:000729';

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Get the various tables and columns needed for this field.
    // We will get the property terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    // Base table
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Object table
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];
    $object_pkey_term = $mapping->getColumnTermId($object_table, $object_pkey_col);
    $object_type_col = 'type_id';
    $value_term = $mapping->getColumnTermId($object_table, self::$value_column);
    $value_len = $object_schema_def['fields'][self::$value_column]['size'];
    $description_term = $mapping->getColumnTermId($object_table, 'description');
    $description_len = $object_schema_def['fields']['description']['size'];

    // Cvterm table, for the name for the contact type
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $value_type_term = $mapping->getColumnTermId('cvterm', 'name');
    $value_type_len = $cvterm_schema_def['fields']['name']['size'];

    // Site administrator's selection for the connection from the base table to the object table.
    list($linker_table, $linker_right_col) = explode('.', $settings['linker_table']);

    // Linker table, when used
    $extra_linker_columns = [];
    if ($linker_table != $base_table) {
      $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // the following should be the same as $base_pkey_col @todo make sure it is
      $linker_left_col = array_keys($linker_schema_def['foreign keys'][$base_table]['columns'])[0];
      $linker_left_term = $mapping->getColumnTermId($linker_table, $linker_left_col);
      $linker_right_term = $mapping->getColumnTermId($linker_table, $linker_right_col);

      // Some but not all linker tables contain rank and type_id columns,
      // these are conditionally added only if they exist in the linker
      // table, and if a term is defined for them.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_right_col)) {
          $term = $mapping->getColumnTermId($linker_table, $column);
          if ($term) {
            $extra_linker_columns[] = [$column => $term];
          }
        }
      }
    }
    else {
      $linker_right_term = $mapping->getColumnTermId($base_table, $linker_right_col);
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col,
    ]);

    // Base table links directly
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'contact_id', $linker_right_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $linker_right_col,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);
    }
    // An intermediate linker table is used
    else {
      // Define the linker table that links the base table to the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $linker_pkey_col,
      ]);

      // Define the link between the base table and the linker table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link', $linker_left_term, [
        'action' => 'store_link',
        'drupal_store' => FALSE,
        'left_table' => $base_table,
        'left_table_id' => $base_pkey_col,
        'right_table' => $linker_table,
        'right_table_id' => $linker_left_col,
      ]);

      // Define the link between the linker table and the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'contact_id', $linker_right_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $linker_right_col,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);

      // Other columns in the linker table. These are set by the widget.
      // Note that type_id and rank are not present in all linker tables,
      // so they are added only if present in the linker table.
      foreach ($extra_linker_columns as $column => $term) {
        $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $column, $term,  [
          'action' => 'store',
          'chado_table' => $linker_table,
          'chado_column' => $column,
        ]);
      }
    }

    // The object table, the destination table of the linker table.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_name', $value_term, $value_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_right_col . '>' . $object_table . '.' . $object_pkey_col,
      'chado_table' => $object_table,
      'chado_column' => self::$value_column,
      'as' => 'contact_name',
    ]);

    // The contact description.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_description', $description_term, $description_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_right_col . '>' . $object_table . '.' . $object_pkey_col,
      'chado_column' => 'description',
      'as' => 'contact_description',
    ]);

    // The name for the type of contact.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_type', $value_type_term, $value_type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_right_col . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.' . $object_type_col . '>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'contact_type',
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
    // keys through a linker table to our object table, the TRUE
    // parameter to getBaseTables() selects this option.
    $elements['storage_plugin_settings']['base_table']['#options'] = $this->getBaseTables($object_table, TRUE);

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
        'If the base table includes the link as a column, then this will reference the base table. ' .
        'When a linker table is used, the table name is typically a ' .
        'combination of the two table names, but they might be in either order. ' .
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
