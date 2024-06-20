<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Form\SubformStateInterface;
use Drupal\tripal\Entity\TripalEntityType;
use Symfony\Component\HttpFoundation\RedirectResponse;


/**
 * Defines the Tripal field item base class.
 */
abstract class ChadoFieldItemBase extends TripalFieldItemBase {

  // The standard record identifier term for all fields.
  protected static $record_id_term = 'SIO:000729';

  // A child class can use these static variables to indicate
  // that it needs a base table column selector in the form.
  protected static $select_base_column = FALSE;
  protected static $valid_base_column_types = [];

  // A child class can use this static variable to indicate
  // that it needs a linker table selector in the form, to this table.
  protected static $object_table = NULL;

  // delimiter between table name and column name in form select
  protected static $table_column_delimiter = " \u{2192} ";  # right arrow

  // Term, namespace, and callback function used for a
  // property for all linking fields to store the Drupal
  // entity ID. The callback function is located in
  // tripal_chado/src/Plugin/TripalStorage/ChadoStorage.php
  protected static $drupal_entity_term = 'schema:ItemPage';
  protected static $chadostorage_namespace = 'Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage';
  protected static $drupal_entity_callback = 'drupalEntityIdLookupCallback';

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

    // Starting with Drupal 10.2, the field storage settings form is a subform
    // within the field settings form. In this case the form_state actually
    // is a sub form state instead of the full form state.
    // There is an ongoing discussion around this which could result in the
    // passed form state going back to a full form state. In order to prevent
    // future breakage because of a core update we'll just check which type of
    // FormStateInterface we've been passed and act accordingly.
    // @See https://www.drupal.org/node/2798261
    if ($form_state instanceof SubformStateInterface) {
      $form_state = $form_state->getCompleteFormState();
    }

    // For most fields, we can specify the base table through the third party setting
    // 'chado_base_table', and then we don't need to select it when manually adding the
    // field to a content type.
    $form_state_storage = $form_state->getStorage();
    $bundle = $form_state_storage['bundle'];
    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    $entity_type_manager = \Drupal::entityTypeManager();
    /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
    $entity_type = $entity_type_manager->getStorage('tripal_entity_type')->load($bundle);
    // If this is not a Chado content type, then $entity_type will be NULL.
    if (!$entity_type) {
      \Drupal::messenger()->addError($this->t(
          'Chado fields cannot be added to non-Chado content types.',
          []));
      $response = new RedirectResponse("/admin/structure/types/manage/" . $bundle . "/fields");
      $response->send();
      return;
    }
    $entity_type_chado_base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');

    // The base table should be selectable by default.
    $base_table_disabled = FALSE;
    $default_base_table = '';

    // If we have a base table defined for the entity type, then we need to see
    // if this field is compatible with the current content type.
    if ($entity_type_chado_base_table) {
      if (!$this->isCompatible($entity_type)) {
        $plugin_definition = $this->getPluginDefinition();
        \Drupal::messenger()->addError($this->t('The selected field, "@field", is not '
            . 'compatible with the "@ctype" content type because Tripal does not know how to link '
            . 'it to the "@base" table of Chado. Only fields with links to "@base" can be '
            . 'added to this content type.',
            ['@field' => $plugin_definition['label'],
             '@ctype' => $entity_type->getLabel(),
             '@base' => $entity_type_chado_base_table]));

        // For Drupal ≤10.1, cleanup the partially created field by removing it.
        $machine_name = $form_state_storage['field_config']->getName();
        $this->removeIncompatibleField($bundle, $machine_name);

        $response = new RedirectResponse("/admin/structure/bio_data/manage/" . $bundle . "/fields/add-field");
        $response->send();
        return;
      }
      // The content type forces the base table so we'll disable the select
      // box for the base table.
      $base_table_disabled = TRUE;
      $base_table = $entity_type_chado_base_table;
      $default_base_table = $entity_type_chado_base_table;
    }
    // If no base table has been set in the content type, then let's see
    // if the field has one hardcoded in it's settings.
    else {
      $storage_settings = $this->getSetting('storage_plugin_settings');
      $storage_settings_base_table = $storage_settings['base_table'] ?? '';
      // If the base table has been saved in the field storage settings
      if ($storage_settings_base_table) {
        $base_table = $storage_settings_base_table;
        $base_table_disabled = TRUE;
        $default_base_table = $storage_settings_base_table;
      }
      else {
        // Base table has been selected in the subform or form depending on Drupal
        // version, and we are in an Ajax callback.
        $base_table = $form_state->getValue(['field_storage', 'subform', 'settings', 'storage_plugin_settings', 'base_table'])
          ?? $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
      }
    }

    // If we have a base table defined, the select list is just this one
    // table. Otherwise we need to generate a list of possible base tables,
    $base_tables = [];
    if ($base_table_disabled) {
      $base_tables[$base_table] = $base_table;
    }
    else {
      $base_tables[NULL] = '-- Select --';
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();
      $tables = $schema->getTables(['type' => 'table', 'status' => 'base']);
      foreach (array_keys($tables) as $table) {
        $base_tables[$table] = $table;
      }
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
      '#disabled' => $base_table_disabled,
      '#element_validate' => [[static::class, 'storageSettingsFormValidateBaseTable']],
    ];

    // Optionally provide a column selector for the base table column if
    // the field specifies this by setting this flag.
    if (static::$select_base_column) {
      $this->storageSettingsFormBaseColumnSelect($base_table, $has_data, $elements, $form_state);
    }

    // Optionally provide a table + column selector for fields using
    // a linking table if the field specifies this by supplying an object table.
    if (static::$object_table) {
      $this->storageSettingsFormLinkerMethodSelect($base_table, $has_data, $elements, $form_state);
    }
    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
  }

  /**
   * Helper function for storage settings form to provide a base column select element.
   *
   * @param string $base_table
   *   Base table for the field.
   * @param boolean $has_data
   *   Field is disabled if this is true.
   * @param array &$elements
   *   Reference to the render array for the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return array
   *   The updated render array for the form.
   */
  private function storageSettingsFormBaseColumnSelect($base_table, $has_data, array &$elements, FormStateInterface $form_state) {

    $storage_settings = $this->getSetting('storage_plugin_settings');
    $default_base_column = $storage_settings['base_column'] ?? '';

    // Add an ajax callback so that when the base table is selected, the
    // base column select can be populated.
    $elements['storage_plugin_settings']['base_table']['#ajax'] = [
      'callback' =>  [$this, 'storageSettingsFormBaseTableAjaxCallback'],
      'event' => 'change',
      'progress' => [
        'type' => 'throbber',
        'message' => $this->t('Retrieving table columns...'),
      ],
      'wrapper' => 'edit-base_column',
    ];

    $base_columns = $this->getTableColumnSelectOptions($base_table, static::$valid_base_column_types);
    $elements['storage_plugin_settings']['base_column'] = [
      '#type' => 'select',
      '#title' => t('Table Column'),
      '#description' => t('Select the column in the base table that contains the field data'),
      '#options' => $base_columns,
      '#default_value' => $default_base_column,
      '#required' => TRUE,
      '#disabled' => $has_data or !$base_table,
      '#prefix' => '<div id="edit-base_column">',
      '#suffix' => '</div>',
    ];
  }

  /**
   * Helper function for storage settings form to provide a linking method element.
   *
   * @param string $base_table
   *   Base table for the field.
   * @param boolean $has_data
   *   Field is disabled if this is true.
   * @param array &$elements
   *   Reference to the render array for the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return array
   *   The updated render array for the form.
   */
  private function storageSettingsFormLinkerMethodSelect($base_table, $has_data, array &$elements, FormStateInterface $form_state) {

    $storage_settings = $this->getSetting('storage_plugin_settings');
    // Three variables here because the form select returns a single combined value.
    $default_linker_table =  $storage_settings['linker_table'] ?? '';
    $default_linker_column =  $storage_settings['linker_fkey_column'] ?? '';
    $default_linker_table_and_column = $storage_settings['linker_table_and_column'] ?? '';
    if (!$default_linker_table_and_column and $default_linker_table and $default_linker_column) {
      $default_linker_table_and_column = $default_linker_table . self::$table_column_delimiter  . $default_linker_column;
    }

    // Base tables presented in this case are only those that either have
    // foreign keys to our object table, or else have foreign keys through
    // a linker table to our object table. The TRUE parameter to
    // getBaseTables() specifies to include linker tables.
    $elements['storage_plugin_settings']['base_table']['#options']
       = $this->getBaseTables(static::$object_table, TRUE);

    // Add an ajax callback so that when the base table is selected, the
    // linking method select can be populated.
    $elements['storage_plugin_settings']['base_table']['#ajax'] = [
      'callback' =>  [$this, 'storageSettingsFormLinkingMethodAjaxCallback'],
      'event' => 'change',
      'progress' => [
        'type' => 'throbber',
        'message' => $this->t('Retrieving linking methods...'),
      ],
      'wrapper' => 'edit-linker_table',
    ];

    $linker_method_disabled = FALSE;
    $linker_tables = [];

    if ($default_linker_table_and_column) {
      $linker_method_disabled = TRUE;
      // We don't need to retrieve the entire list for this case.
      $linker_tables = [$default_linker_table_and_column => $default_linker_table_and_column];
    }
    else {
      $linker_tables = $this->getLinkerTableSelectOptions(static::$object_table, $base_table, static::$table_column_delimiter);
    }
    $elements['storage_plugin_settings']['linker_table_and_column'] = [
      '#type' => 'select',
      '#title' => t('Linking Method'),
      '#description' => t('Select the table that links the selected base table to the linked table. ' .
        'If the base table includes the link as a column, then this will reference the base table. ' .
        'When a linker table is used, the linking table name is typically a ' .
        'combination of the two table names, but they might be in either order. ' .
        'Generally this select will have only one option, unless a module has added additional custom linker tables. ' .
        'For example to link "feature" to "contact", the linking method would be "feature_contact → contact_id".'),
      '#options' => $linker_tables,
      '#default_value' => $default_linker_table_and_column,
      '#required' => TRUE,
      '#disabled' => $linker_method_disabled or !$base_table,
      '#prefix' => '<div id="edit-linker_table">',
      '#suffix' => '</div>',
      '#element_validate' => [[static::class, 'storageSettingsFormValidateLinkingMethod']],
    ];
  }

  /**
   * Ajax callback to update the base column select. The select
   * can't be populated until we know the base table.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function storageSettingsFormBaseTableAjaxCallback($form, &$form_state) {
    $response = new AjaxResponse();
    $drupal_10_2 = $form_state->getValue(['field_storage']);
    if ($drupal_10_2) {
      $response->addCommand(new ReplaceCommand('#edit-base_column', $form['field_storage']['subform']['settings']['storage_plugin_settings']['base_column']));
    }
    else {
      $response->addCommand(new ReplaceCommand('#edit-base_column', $form['settings']['storage_plugin_settings']['base_column']));
    }
    return $response;
  }

  /**
   * Ajax callback to update the linking method select. The select
   * can't be populated until we know the base table.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function storageSettingsFormLinkingMethodAjaxCallback($form, &$form_state) {
    $response = new AjaxResponse();
    $drupal_10_2 = $form_state->getValue(['field_storage']);
    if ($drupal_10_2) {
      $response->addCommand(new ReplaceCommand('#edit-linker_table', $form['field_storage']['subform']['settings']['storage_plugin_settings']['linker_table_and_column']));
    }
    else {
      $response->addCommand(new ReplaceCommand('#edit-linker_table', $form['settings']['storage_plugin_settings']['linker_table_and_column']));
    }
    return $response;
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
    $settings = self::getFormStateSettings($form_state);
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    $base_table = $settings['storage_plugin_settings']['base_table'];

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    if ($schema->tableExists($base_table)) {
      $form_state->setValue(['settings', 'storage_plugin_settings', 'base_table'], $base_table);
    }
    else {
      $form_state->setErrorByName('settings][storage_plugin_settings][base_table',
          'The selected base table does not exist in Chado.');
    }
  }

  /**
   * A callback function after the field is saved.
   *
   * This function is added to the submit actions for the form via the
   * tripal_chado_form_alter() hook.  It ensures that if the base table
   * for the entity type is not set that this Chado field will set it. This
   * ensures that all future additions of Chado fields will have to check
   * if they are compatible with the base table.
   *
   * @param array $form
   * @param FormStateInterface $form_state
   */
  public static function storageSettingsFormSubmitBaseTable(array $form, FormStateInterface $form_state) {

    $settings = self::getFormStateSettings($form_state);
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }

    // For most fields, we can specify the base table through the third party setting
    // 'chado_base_table', and then we don't need to select it when manually adding the
    // field to a content type.
    $form_state_storage = $form_state->getStorage();
    $bundle = $form_state_storage['bundle'];
    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    $entity_type_manager = \Drupal::entityTypeManager();
    /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
    $entity_type = $entity_type_manager->getStorage('tripal_entity_type')->load($bundle);
    $entity_type_chado_base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');

    // If the entity type is missing the base table then use the one from
    // this field and set it. This should only happen the first time a Chado
    // field is added to the content type.  Once the base table is set for
    // the entity type then all fields added in the future must use this
    // base table.
    if (!$entity_type_chado_base_table) {
      $base_table = $settings['storage_plugin_settings']['base_table'];
      $entity_type->setThirdPartySetting('tripal', 'chado_base_table', $base_table);
      $entity_type->save();
    }
  }

  /**
   * Form element validation handler for linking method (table + column)
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form.
   */
  public static function storageSettingsFormValidateLinkingMethod(array $form, FormStateInterface $form_state) {
    $settings = self::getFormStateSettings($form_state);
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }

    // Convert the combined value from the linking method form select into table and column
    $linker_table_and_column = $settings['storage_plugin_settings']['linker_table_and_column'];
    $parts = explode(self::$table_column_delimiter, $linker_table_and_column);
    if (count($parts) == 2) {
      $form_state->setValue(['settings', 'storage_plugin_settings', 'linker_table'], $parts[0]);
      $form_state->setValue(['settings', 'storage_plugin_settings', 'linker_fkey_column'], $parts[1]);
    }
    else {
      $form_state->setErrorByName('settings][storage_plugin_settings][linker_table_and_column',
          'The selected linking method is not valid.');
    }
  }

  /**
   * Return a list of candidate base tables. We only want to
   * present valid tables to the user, which are those with
   * an appropriate foreign key.
   *
   * @param string $linked_table
   *   The Chado table being linked to via a foreign key.
   * @param bool $has_linker_table
   *   When set to false (default), base tables are only those
   *   tables with a foreign key to $linked_table.
   *   When set to true, also include tables based on two
   *   foreign keys in linker tables, one to the specified
   *   $linked_table, and a second to a different table.
   *
   * @return array
   *   The list of tables is returned in an alphabetized list
   *   ready to use in a form select.
   */
  protected function getBaseTables($linked_table, $has_linker_table = FALSE) {
    $base_tables = [];
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // Start from the primary key of the object table, and work
    // back to candidate base tables.
    $object_schema_def = $schema->getTableDef($linked_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];
    $all_tables = $schema->getTables(['type' => 'table']);
    foreach (array_keys($all_tables) as $table) {
      $table_schema_def = $schema->getTableDef($table, ['format' => 'Drupal']);
      if (array_key_exists('foreign keys', $table_schema_def)) {
        // For "single-hop" logic, we add this table if there is a
        // foreign key to our linked_table.
        $found = FALSE;
        foreach ($table_schema_def['foreign keys'] as $foreign_key) {
          if ($foreign_key['table'] == $linked_table) {
            $base_tables[$table] = $table;
            $found = TRUE;
          }
        }
        // For "double-hop" logic, this may be a linker table,
        // and it needs two foreign keys, one to our linked_table
        // which we detected above, and a second one to another table.
        // This linked-to table is also a candidate base table.
        if ($has_linker_table and $found) {
          foreach ($table_schema_def['foreign keys'] as $foreign_key) {
            if ($foreign_key['table'] != $linked_table) {
              $base_tables[$foreign_key['table']] = $foreign_key['table'];
            }
          }
        }
      }
    }

    // Alphabetize the list presented to the user.
    ksort($base_tables);

    // This is unlikely to happen, but provide an indication if it does.
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
   * Return a list of column names for the indicated table.
   *
   * @param string $table_name
   *   The Chado table of interest.
   *
   * @param array $column_types
   *   If specified, limit to specified column types, e.g.
   *   "character varying", "text", "bigint", etc.
   *
   * @return array
   *   The list of columns is returned in an alphabetized list
   *   ready to use in a form select.
   */
  protected function getTableColumnSelectOptions($table_name = '', $column_types = []) {
    $select_list = [];

    if (!$table_name) {
      $select_list[NULL] = '-- Select base table first --';
    }
    else {
      $column_names = $this->getTableColumns($table_name, $column_types);
      foreach ($column_names as $column_name) {
        $select_list[$column_name] = $column_name;
      }
      if (count($select_list) == 0) {
        $select_list = [NULL => '-- No valid columns available --'];
      }
      // If more than one item was found, prefix the list with a Select message
      elseif (count($select_list) > 1) {
        ksort($select_list);
        $select_list = [NULL => '-- Select --'] + $select_list;
      }
    }

    return $select_list;
  }

  /**
   * Return a list of column names for the indicated table.
   *
   * @param string $table_name
   *   The Chado table of interest.
   *
   * @param array $column_types
   *   If specified, limit to specified column types, e.g.
   *   "character varying", "text", "bigint", etc.
   *
   * @return array
   *   The list of columns
   */
  protected function getTableColumns($table_name, $column_types = []) {
    $table_columns = [];

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $table_schema_def = $schema->getTableDef($table_name, ['format' => 'Drupal']);
    foreach ($table_schema_def['fields'] as $field => $properties) {
      if (!$column_types or in_array($properties['type'], $column_types)) {
        $table_columns[] = $field;
      }
    }
    return $table_columns;
  }

 /**
   * Return a list of candidate linking connections given a base table
   * and a linked table. These can either be a column in the base table,
   * or a connection through a linking table that connects the base
   * table to the linked table.
   * In some cases there may be more than one way to link the two
   * tables, so the list generated here can be presented to the site
   * administrator to select the desired linking method.
   *
   * @param string $base_table
   *   The Chado table being used for the current entity (subject).
   * @param string $object_table
   *   The Chado table being linked to (object).
   * @param string $delimiter
   *   The displayed delimiter between the table and column in the
   *   form select. This defaults to a right arrow.
   *
   * @return array
   *   The list of tables is returned in an alphabetized list
   *   ready to use in a form select. The list elements will be
   *   in the format table.column
   */
  protected function getLinkerTableSelectOptions($object_table, $base_table, $delimiter = " \u{2192} ") {
    $select_list = [];

    // The base table is needed to generate the list. We will return
    // here again from the ajax callback once that has been selected.
    if (!$base_table) {
      $select_list[NULL] = '-- Select base table first --';
    }
    else {
      $linker_tables = $this->getLinkerTables($object_table, $base_table);
      if (count($linker_tables) == 0) {
        $select_list = [NULL => '-- No link is possible --'];
      }
      // If at least one found, convert to options for a form select
      else {
        foreach ($linker_tables as $link) {
          $key = $link[0] . $delimiter . $link[1];
          $select_list[$key] = $key;
        }
        // If more than one item was found, prefix the list with a Select message
        if (count($linker_tables) > 1) {
          ksort($select_list);
          $select_list = [NULL => '-- Select --'] + $select_list;
        }
      }
    }
    return $select_list;
  }

 /**
   * Return a list of candidate linking connections given a base table
   * and a linked table. These can either be a column in the base table,
   * or a connection through a linking table that connects the base
   * table to the linked table.
   *
   * @param string $base_table
   *   The Chado table being used for the current entity (subject).
   * @param string $object_table
   *   The Chado table being linked to (object).
   *
   * @return array
   *   The list of tables and columns.
   */
  protected function getLinkerTables($object_table, $base_table) {
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];

    $all_tables = $schema->getTables(['type' => 'table']);
    $linker_tables = [];
    foreach (array_keys($all_tables) as $table_name) {
      $table_schema_def = $schema->getTableDef($table_name, ['format' => 'Drupal']);
      if (array_key_exists('foreign keys', $table_schema_def)) {
        foreach ($table_schema_def['foreign keys'] as $foreign_key) {
          if ($foreign_key['table'] == $object_table) {
            // If the current table is the base table, we have a direct
            // reference to the object table, otherwise it is a linker table,
            // and needs to also have a foreign key to the base table.
            if (($table_name == $base_table)
                or ($schema->foreignKeyConstraintExists($table_name, $base_pkey_col))) {
              $linker_tables[] = [$table_name, array_keys($foreign_key['columns'])[0]];
            }
          }
        }
      }
    }
    return $linker_tables;
  }

  /**
   * Retrieve linker table and column from storage settings, used in a field's tripalTypes() function.
   *
   * @param array $storage_settings
   *   Storage settings for a field
   * @param string $default_table
   *   This will be the base table for the field
   * @param string $default_column
   *   This will be the object pkey column for the field
   *
   * @return array
   *   Returns linker_table and linker_fkey_column
   */
  public static function get_linker_table_and_column($storage_settings, $default_table, $default_column) {
    // The combined setting comes from the field settings form, e.g. "project → contact"
    $combined_setting = $storage_settings['linker_table_and_column'] ?? '';
    if ($combined_setting) {
      $parts = self::parse_combined_table_and_column($combined_setting);
      $linker_table = $parts[0];
      $linker_fkey_column = $parts[1];
    }
    else {
      // For single hop, in the yaml we can support using the usual 'base_table'
      // and 'base_column' settings, these are passed in as the defaults.
      $linker_table = $storage_settings['linker_table'] ?? $default_table;
      $linker_fkey_column = $storage_settings['linker_fkey_column'] ?? $default_column;
    }
    return [$linker_table, $linker_fkey_column];
  }

  /**
   * Parse a combined table + column string into its two parts
   *
   * @param string $table_and_column
   *   Table and column delimited by self::table_column_delimiter, a right
   *    arrow by default, e.g."project → contact"
   *
   * @return array
   *   Will contain 2 elements if valid, empty array if not.
   */
  protected static function parse_combined_table_and_column($table_and_column) {
    $parts = explode(self::$table_column_delimiter, $table_and_column);
    if (count($parts) == 2) {
      return $parts;
    }
    else {
      return [];
    }
  }

  /**
   * Indicates if the field is compabible with the content type.
   *
   * This function should be implemented by all Chado-base fields and
   * indicate if the field is compatible with the specified content
   * type. By default, it returns TRUE.
   *
   * @param TripalEntityType $entity_type
   *
   * @return bool
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    return TRUE;
  }

  /**
   * Needed for Drupal ≤10.1 when an incompatible field was added
   * to a content type, this will clean up the partially added field.
   *
   * @param string $bundle
   *   The bundle name.
   *
   * @param string $machine_name
   *   The field identifier, including the 'field_' prefix.
   */
  protected function removeIncompatibleField(string $bundle, string $machine_name) {
    $entityFieldManager = \Drupal::service('entity_field.manager');
    $fields = $entityFieldManager->getFieldDefinitions('tripal_entity', $bundle);
    if (isset($fields[$machine_name])) {
      $fields[$machine_name]->delete();
    }
  }
}
