<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;


/**
 * Defines the Tripal field item base class.
 */
abstract class ChadoFieldItemBase extends TripalFieldItemBase {

  /**
    * Indicates if the form should provide a base column select form element.
    *
    * @var bool
    */
  public $display_base_column = FALSE;

  /**
   * Toggle for form display of base column selector.
   *
   * @param boolean $display
   *
   */
  public function display_base_column($display) {
    $this->display_base_column = $display;
  }

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
    $default_base_table = $storage_settings['base_table'] ?? '';
    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
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

    // Optionally provide a column selector for the base table column.
    if ($this->display_base_column) {
      $default_base_column = $storage_settings['base_column'] ?? '';
      $base_column = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_column']);

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

      $base_columns = $this->getTableColumns($base_table, []);
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
    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
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
    $response->addCommand(new ReplaceCommand('#edit-base_column', $form['settings']['storage_plugin_settings']['base_column']));
    return $response;
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
      if ($schema->foreignKeyConstraintExists($table, $object_pkey_col)) {
        // "single-hop" logic, evaluate chado tables for a foreign
        // key to our object table. If it has one, we will consider
        // this a candidate for a base table.
        $base_tables[$table] = $table;

        if ($has_linker_table) {
          // This logic is used for fields using a linker table,
          // i.e. a "double-hop". Here we look in potential linker
          // tables for two foreign keys, one to our object table, and
          // a second to a different table. These different tables
          // become the list of candidate base tables.
          $table_schema_def = $schema->getTableDef($table, ['format' => 'Drupal']);
          if (array_key_exists('foreign keys', $table_schema_def)) {
            foreach ($table_schema_def['foreign keys'] as $foreign_key) {
              if ($foreign_key['table'] != $linked_table) {
                $base_tables[$foreign_key['table']] = $foreign_key['table'];
              }
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
   *  linked table.
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
                $key = $table_name . $delimiter . array_values($foreign_key['columns'])[0];
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
