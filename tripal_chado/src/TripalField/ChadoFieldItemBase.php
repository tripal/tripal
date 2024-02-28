<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;


/**
 * Defines the Tripal field item base class.
 */
abstract class ChadoFieldItemBase extends TripalFieldItemBase {

  // delimiter between table name and column name in form select
  protected static $table_column_delimiter = " \u{2192} ";  # right arrow

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
    $complete_form_state = NULL;
    if ($form_state instanceof \Drupal\Core\Form\SubformStateInterface) {
      $complete_form_state = $form_state->getCompleteFormState();
    }

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $tables = $schema->getTables(['type' => 'table', 'status' => 'base']);

    $base_table_disabled = FALSE;
    $is_ajax = \Drupal::request()->isXmlHttpRequest();
    $storage_settings = $this->getSetting('storage_plugin_settings');
    $default_base_table = $storage_settings['base_table'] ?? '';

    if ($default_base_table) {
      $base_table_disabled = TRUE;
      $base_table = $default_base_table;
    }
    else {
      if ($complete_form_state) {
        $base_table = $complete_form_state->getValue(['field_storage', 'subform', 'settings', 'storage_plugin_settings', 'base_table']);
      }
      else {
        $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
      }
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
      '#disabled' => $base_table_disabled,
      '#element_validate' => [[static::class, 'storageSettingsFormValidateBaseTable']],
      '#ajax' => [
        'callback' =>  [static::class, 'storageSettingsFormBaseTableAjaxCallback'],
        'event' => 'change',
        'progress' => [
          'type' => 'throbber',
          'message' => $this->t('Retrieving options that depend on the base table...'),
        ],
        'wrapper' => 'base-table-dependant-elements',
      ]
    ];

    // Provide a container for ajax to update for any additional base_table
    // dependant settings. We provide a wrapper like this since you can only
    // attach a single ajax event to a form element.
    $elements['storage_plugin_settings']['base_table_dependant'] = [
      '#prefix' => '<div id="base-table-dependant-elements">',
      '#suffix' => '</div>',
    ];

    // Optionally provide a column selector for the base table column if
    // the field annotations specify it. In yaml files we don't need to
    // include the 'base_table_dependant' key.
    $plugin_definition = $this->getPluginDefinition();
    if ($plugin_definition['select_base_column'] ?? FALSE) {
      $default_base_column = $storage_settings['base_table_dependant']['base_column']
          ?? $storage_settings['base_column'] ?? '';
      if ($complete_form_state) {
        $base_column = $complete_form_state->getValue(['field_storage', 'subform', 'settings', 'storage_plugin_settings', 'base_column']);
      }
      else {
        $base_column = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_column']);
      }
      // Do not disable selector quite yet if this is just an ajax callback
      $table_column_disabled = ($default_base_column and !$is_ajax);

      $column_types = $plugin_definition['valid_base_column_types'] ?? [];
      $base_columns = $this->getTableColumns($base_table, $column_types);
      $elements['storage_plugin_settings']['base_table_dependant']['base_column'] = [
        '#type' => 'select',
        '#title' => t('Table Column'),
        '#description' => t('Select the column in the base table that contains the field data'),
        '#options' => $base_columns,
        '#default_value' => $default_base_column,
        '#required' => TRUE,
        '#disabled' => $table_column_disabled or $has_data or !$base_table,
      ];
    }

    // Optionally provide a table + column selector for fields using
    // a linking table if the field annotations specify it.
    if ($plugin_definition['object_table'] ?? FALSE) {

      // Base tables presented here are only those that either have foreign
      // keys to our object table, or else have foreign keys through a
      // linker table to our object table. The TRUE parameter to
      // getBaseTables() specifies to include linker tables.
      $elements['storage_plugin_settings']['base_table']['#options']
          = $this->getBaseTables($plugin_definition['object_table'], TRUE);

      $linker_is_disabled = FALSE;
      $linker_tables = [];
      $default_linker_table = $storage_settings['base_table_dependant']['linker_table']
          ?? $storage_settings['linker_table'] ?? '';
      $default_linker_column = $storage_settings['base_table_dependant']['linker_fkey_column']
          ?? $storage_settings['linker_fkey_column'] ?? '';
      $default_linker_table_and_column = $storage_settings['base_table_dependant']['linker_table_and_column']
          ?? $storage_settings['linker_table_and_column'] ?? '';
      if (!$default_linker_table_and_column and $default_linker_table and $default_linker_column) {
        $default_linker_table_and_column = $default_linker_table . self::$table_column_delimiter  . $default_linker_column;
      }
      if ($default_linker_table_and_column) {
        $linker_is_disabled = TRUE;
        // We don't need to retrieve the entire list any more.
        $linker_tables = [$default_linker_table_and_column => $default_linker_table_and_column];
      }
      else {
        $linker_tables = $this->getLinkerTables($plugin_definition['object_table'], $base_table, self::$table_column_delimiter);
      }
      $elements['storage_plugin_settings']['base_table_dependant']['linker_table_and_column'] = [
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
        '#disabled' => $linker_is_disabled or $has_data or !$base_table,
        '#element_validate' => [[static::class, 'storageSettingsFormValidateLinkingMethod']],
      ];
    }

    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
  }

  /**
   * Ajax callback to update any settings that are dependant on the base table
   * being selected.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public static function storageSettingsFormBaseTableAjaxCallback(array $form, FormStateInterface $form_state) {

    // In Drupal 10.2, the field settings and storage settings each become subforms
    // within the same page. We check for the key here in order to still support
    // pre-subform versions of this form.
    if (array_key_exists('field_storage', $form)) {
      // NOTE: AJAX always has the full form, even when subforms are involved ;-p
      // In this case, there are actually keys for the field settings (settings)
      // and field storage (field_storage) forms and then nested below this is
      // a key (subform) containing the actual array of the subform.
      $field_storage_form = $form['field_storage']['subform'];
    }
    else {
      $field_storage_form = $form;
    }

    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#base-table-dependant-elements',
                          $field_storage_form['settings']['storage_plugin_settings']['base_table_dependant']));
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
    $settings = $form_state->getValue('settings');
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
    $linker_table_and_column = $settings['storage_plugin_settings']['base_table_dependant']['linker_table_and_column']
        ?? $settings['storage_plugin_settings']['linker_table_and_column'];
    $parts = self::parse_combined_table_and_column($linker_table_and_column);
    if (count($parts) == 2 and $parts[0] and $parts[1]) {
      $form_state->setValue(['settings', 'storage_plugin_settings', 'linker_table'], $parts[0]);
      $form_state->setValue(['settings', 'storage_plugin_settings', 'linker_fkey_column'], $parts[1]);
    }
    else {
      $form_state->setErrorByName('settings][storage_plugin_settings][linker_table_and_column',
          'The selected linking method is not valid.');
    }
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
    $combined_setting = $storage_settings['base_table_dependant']['linker_table_and_column'] ?? '';
    if ($combined_setting) {
      $parts = self::parse_combined_table_and_column($combined_setting);
      $linker_table = $parts[0];
      $linker_fkey_column = $parts[1];
    }
    else {
      // For single hop, in the yaml we support using the usual 'base_table'
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
  protected function getTableColumns($table_name = '', $column_types = []) {
    $select_list = [];

    if (!$table_name) {
      $select_list[NULL] = '-- Select base table first --';
    }
    else {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();
      $table_schema_def = $schema->getTableDef($table_name, ['format' => 'Drupal']);
      foreach ($table_schema_def['fields'] as $field => $properties) {
        if (!$column_types or in_array($properties['type'], $column_types)) {
          $select_list[$field] = $field;
        }
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
   * Return a list of candidate linking connections given
   * a base table and a linked table. These can either be
   * a column in the base table, or a connection through
   * a linking table that connects the base table to the
   * linked table.
   * In some cases there may be more than one way to link
   * the two tables, so the list generated here can be
   * presented to the site administrator to select the
   * desired linking method.
   *
   * @param string $base_table
   *   The Chado table being used for the current entity (subject).
   * @param string $linked_table
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
  protected function getLinkerTables($linked_table, $base_table, $delimiter = " \u{2192} ") {
    $select_list = [];

    // The base table is needed to generate the list. We will return
    // here again from the ajax callback once that has been selected.
    if (!$base_table) {
      $select_list[NULL] = '-- Select base table first --';
    }
    else {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();

      $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
      $base_pkey_col = $base_schema_def['primary key'];
      $object_schema_def = $schema->getTableDef($linked_table, ['format' => 'Drupal']);
      $object_pkey_col = $object_schema_def['primary key'];

      $all_tables = $schema->getTables(['type' => 'table']);
      foreach (array_keys($all_tables) as $table_name) {
        $table_schema_def = $schema->getTableDef($table_name, ['format' => 'Drupal']);
        if (array_key_exists('foreign keys', $table_schema_def)) {
          foreach ($table_schema_def['foreign keys'] as $foreign_key) {
            if ($foreign_key['table'] == $linked_table) {
              // If the current table is the base table, we have a direct
              // reference to the object table, otherwise it is a linker table,
              // and needs to also have a foreign key to the base table.
              if (($table_name == $base_table)
                  or ($schema->foreignKeyConstraintExists($table_name, $base_pkey_col))) {
                $key = $table_name . $delimiter . array_keys($foreign_key['columns'])[0];
                $select_list[$key] = $key;
              }
            }
          }
        }
      }

      if (count($select_list) == 0) {
        $select_list = [NULL => '-- No link is possible --'];
      }
      // If more than one item was found, prefix the list with a Select message
      elseif (count($select_list) > 1) {
        ksort($select_list);
        $select_list = [NULL => '-- Select --'] + $select_list;
      }
    }
    return $select_list;
  }

}
