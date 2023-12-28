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
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Plugin implementation of Tripal additional type field type.
 *
 * @FieldType(
 *   id = "chado_additional_type_type_default",
 *   label = @Translation("Chado Type Reference"),
 *   description = @Translation("A Chado type reference"),
 *   default_widget = "chado_additional_type_widget_default",
 *   default_formatter = "chado_additional_type_formatter_default"
 * )
 */
class ChadoAdditionalTypeTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_additional_type_type_default';

  // delimiter between table name and column name in form select
  public static $table_column_delimiter = " \u{2192} ";  # right arrow

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'term_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    // If a fixed value is set, then the field will will always use the
    // same value and the user will not be allowed the change it using the
    // widget. This is necessary for content types that correspond to Chado
    // tables with a type_id that should always match the content type (e.g.
    // gene).
    $settings['fixed_value'] = FALSE;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['type_table'] = '';
    $settings['storage_plugin_settings']['type_column'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the Chado table and column this field maps to.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $type_table = $storage_settings['type_table'] ?? '';
    $type_column = $storage_settings['type_column'] ?? '';

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table or !$type_table) {
      return;
    }

    // Get the connecting information about the base table and the
    // table where the type is stored. If the base table has a `type_id`
    // column then the base table and the type table are the same. If we
    // are using a prop table to store the type_id then the type table and
    // base table will be different.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_table_def['primary key'];

    // Create variables to store the terms for the properties. We can use terms
    // from Chado tables if appropriate.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $type_id_term = $mapping->getColumnTermId($type_table, $type_column);
    $name_term = $mapping->getColumnTermId('cvterm', 'name');
    $idspace_term = 'SIO:000067';
    $accession_term = $mapping->getColumnTermId('dbxref', 'accession');

    // Always store the record id of the base record that this field is
    // associated with in Chado.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col
    ]);

    // If the type table and the base table are not the same then we are
    // storing the type in a prop table and we need the pkey for the prop
    // table, the fkey linking to the base table, and we'll set a value
    // of the type name.
    if ($type_table != $base_table) {
      $type_table_def = $schema->getTableDef($type_table, ['format' => 'Drupal']);
      $type_pkey_col = $type_table_def['primary key'];
      $type_fkey_col = array_keys($type_table_def['foreign keys'][$base_table]['columns'])[0];
      $link_term = $mapping->getColumnTermId($type_table, $type_fkey_col);
      $value_term = $mapping->getColumnTermId($type_table, 'value');

      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'prop_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'chado_table' => $type_table,
        'chado_column' => $type_pkey_col,
      ]);
      $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link_id', $link_term, [
        'action' => 'store_link',
        'drupal_store' => TRUE,
        'left_table' => $base_table,
        'left_table_id' => $base_pkey_col,
        'right_table' => $type_table,
        'right_table_id' => $type_fkey_col,
      ]);
      $properties[] =  new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'chado_table' => $type_table,
        'chado_column' => 'value',
      ]);
    }

    // We need to store the numeric cvterm ID for this field.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $type_id_term, [
      'action' => 'store',
      'drupal_store' => TRUE,
      'chado_table' => $type_table,
      'chado_column' => $type_column,
      'empty_value' => 0
    ]);
    // This field needs the term name, idspace and accession for proper
    // display of the type.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'term_name', $name_term, 128, [
      'action' => 'read_value',
      'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'term_name'
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'id_space', $idspace_term, 128, [
      'action' => 'read_value',
      'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id',
      'chado_column' => 'name',
      'as' => 'idSpace'
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'accession', $accession_term, 128, [
      'action' => 'read_value',
      'path' => $type_table. '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id',
      'chado_column' => 'accession',
      'as' => 'accession'
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalField\TripalFieldItemBase::tripalValuesTemplate()
   */
  public function tripalValuesTemplate($field_definition, $default_value = NULL) {
    $prop_values = parent::tripalValuesTemplate($field_definition, $default_value);

    // The type value is an ontology term ID.  This isn't searchable that way
    // in Chado, so we need to override this function and set the default
    // property values if one is provided.
    $matches = [];
    if ($default_value and preg_match('/^(.+?):(.+?)$/', $default_value, $matches)) {

      $termIdSpace = $matches[1];
      $termAccession = $matches[2];

      /** @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idSpace_manager **/
      /** @var \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase $idSpace **/
      /** @var \Drupal\tripal\TripalVocabTerms\TripalTerm $term **/
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($termIdSpace);
      $term = $idSpace->getTerm($termAccession);

      foreach ($prop_values as $index => $prop_value) {
        if ($prop_value->getKey() == 'type_id') {
          $prop_values[$index]->setValue($term->getInternalId());
        }
        if ($prop_value->getKey() == 'accession') {
          $prop_values[$index]->setValue($term->getAccession());
        }
        if ($prop_value->getKey() == 'term_name') {
          $prop_values[$index]->setValue($term->getName());
        }
        if ($prop_value->getKey() == 'id_space') {
          $prop_values[$index]->setValue($term->getIdSpace());
        }
      }
    }

    return $prop_values;
  }


  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);
    $type_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'type_table']);
    $type_column = $form_state->getValue(['settings', 'storage_plugin_settings', 'type_column']);
    // In the form, table and column will be selected together as a single unit
    $type_select = '';
    if ($type_table and $type_column) {
      $type_select = $type_table . self::$table_column_delimiter . $type_column;
    }

    // Add an ajax callback to the base table select (from the parent form) so that
    // when it is selected, the type table select can be populated with candidate tables.
    $elements['storage_plugin_settings']['base_table']['#ajax'] = [
      'callback' =>  [$this, 'storageSettingsFormTypeFKeyAjaxCallback'],
      'event' => 'change',
      'progress' => [
        'type' => 'throbber',
        'message' => $this->t('Retrieving type table names...'),
      ],
    ];

    // Element to select combined table and column for the additional type.
    $elements['storage_plugin_settings']['type_fkey'] = [
      '#type' => 'select',
      '#title' => t('Type Table and Column'),
      '#description' => t('Select the table and column that specifies the type for this field. ' .
        'This can be either from the base table, or from a different table with a foreign ' .
        'key to the base table.'),
      '#options' => $this->getTypeFkeys($base_table),
      '#default_value' => $type_select,
      '#required' => TRUE,
      '#disabled' => !$base_table,
      '#prefix' => '<div id="edit-type_fkey">',
      '#suffix' => '</div>',
      '#element_validate' => [[static::class, 'storageSettingsFormValidate']],
    ];

    return $elements;
  }

  /**
   * Form element validation handler for type table and column
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

    // The type table and column are selected as a single value. Separate
    // them, and validate and store each separately.
    $type_fkey = $settings['storage_plugin_settings']['type_fkey'];
    $parts = explode(self::$table_column_delimiter, $type_fkey);
    if (count($parts) != 2 or !$parts[0] or !$parts[1]) {
      $form_state->setErrorByName('storage_plugin_settings][type_fkey',
          'An invalid table and column was selected');
    }
    else {
      // Store the separated table and column in their respective settings variables
      $form_state->setValue(['settings', 'storage_plugin_settings', 'type_table'], $parts[0]);
      $form_state->setValue(['settings', 'storage_plugin_settings', 'type_column'], $parts[1]);
    }
  }

  /**
   * Return a list of candidate type tables. This is done
   * by returning tables that have a foreign key to our
   * $base_table, and have a column with a foreign key
   * to cvterm. These tables+columns are returned in an
   * alphabetized list ready to use in a form select,
   * with the base table, if present, included at the top.
   *
   * @param string $base_table
   *   The Chado base table being used for this field.
   */
  protected function getTypeFKeys($base_table) {
    $type_fkeys = [];

    // On the initial presentation of the form, the base table
    // is not yet know. We will return here again from the ajax
    // callback once that has been selected.
    if (!$base_table) {
      $type_fkeys[''] = '-- Select base table first --';
    }
    else {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();

      // Get a list of tables with foreign keys to selected $base_table.
      $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
      $fkey_list = preg_split('/[ ,]+/', ($base_schema_def['referring_tables']??''));
      asort($fkey_list);

      // Include the base table at the top of the sorted list
      // since it is the most likely table to select.
      array_unshift($fkey_list, $base_table);

      // For each of these tables, if there is a column with a foreign key to the
      // cvterm table, return table+column, formatted for use in the form select.
      foreach ($fkey_list as $type_table) {
        $type_schema_def = $schema->getTableDef($type_table, ['format' => 'Drupal']);
        if (isset($type_schema_def['foreign keys']['cvterm']['columns'])) {
          foreach ($type_schema_def['foreign keys']['cvterm']['columns'] as $column_name => $table) {
            $fkey = $type_table . self::$table_column_delimiter . $column_name;
            $type_fkeys[$fkey] = $fkey;
          }
        }
      }

      // If no foreign keys were found, we can't use this field
      // for this base table. Add a message.
      if (!count($type_fkeys)) {
        $type_fkeys[''] = '-- No options available for this base table --';
      }
      if (count($type_fkeys) > 1) {
        $type_fkeys = ['' => '- Select table and column -'] + $type_fkeys;
      }
    }
    return $type_fkeys;
  }

  /**
   * Ajax callback to update the type table+column select. This is needed
   * because the select can't be populated until we know the base table.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function storageSettingsFormTypeFKeyAjaxCallback($form, &$form_state) {
    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-type_fkey', $form['settings']['storage_plugin_settings']['type_fkey']));
    return $response;
  }

}
