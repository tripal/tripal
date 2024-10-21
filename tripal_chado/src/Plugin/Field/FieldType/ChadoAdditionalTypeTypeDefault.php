<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Plugin implementation of Tripal additional type field type.
 *
 * @FieldType(
 *   id = "chado_additional_type_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Type Reference"),
 *   description = @Translation("A Chado type reference"),
 *   default_widget = "chado_additional_type_widget_default",
 *   default_formatter = "chado_additional_type_formatter_default"
 * )
 */
class ChadoAdditionalTypeTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_additional_type_type_default';

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
    // If this field needs to set a fixed value, set this to TRUE.
    // It indicates to the publishing step to include this field.
    // If not set, then the publishing step may not be able to find matches
    // for this field based on the fixed value.
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
    // Type table and column can also be stored as the select element
    $type_fkey = $storage_settings['type_fkey'] ?? '';
    if ($type_fkey) {
      list($type_table, $type_column) = explode(self::$table_column_delimiter, $type_fkey);
    }

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
    $type_id_term = $mapping->getColumnTermId($type_table, $type_column) ?: 'rdfs:type';
    $name_term = $mapping->getColumnTermId('cvterm', 'name') ?: 'schema:name';
    $idspace_term = 'SIO:000067';
    $accession_term = $mapping->getColumnTermId('dbxref', 'accession') ?: 'data:2091';

    // Always store the record id of the base record that this field is
    // associated with in Chado.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // If the type table and the base table are not the same then we are
    // storing the type in a prop table and we need the pkey for the prop
    // table, the fkey linking to the base table, and we'll set a value
    // of the type name.
    if ($type_table != $base_table) {
      $type_table_def = $schema->getTableDef($type_table, ['format' => 'Drupal']);
      $type_pkey_col = $type_table_def['primary key'];
      $type_fkey_col = array_keys($type_table_def['foreign keys'][$base_table]['columns'])[0];
      $link_term = $mapping->getColumnTermId($type_table, $type_fkey_col) ?: self::$record_id_term;
      $value_term = $mapping->getColumnTermId($type_table, 'value') ?: 'NCIT:C25712';

      // (e.g., analysisprop.analysisprop_id)
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'prop_id', self::$record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'path' => $type_table  . '.' . $type_pkey_col,
      ]);
      // (e.g., analysisprop.feature_id)
      $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link_id', $link_term, [
        'action' => 'store_link',
        'path' => $type_table . '.' . $type_fkey_col,
      ]);
      // (e.g., analysisprop.value)
      $properties[] =  new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'path' => $type_table . '.' . 'value',
      ]);
    }

    // We need to store the numeric cvterm ID for this field.
    // (e.g., feature.type_id or analysisprop.type_id)
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $type_id_term, [
      'action' => 'store',
      'path' => $type_table . '.' . $type_column,
      'empty_value' => 0
    ]);

    // This field needs the term name, idspace and accession for proper
    // display of the type.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'term_name', $name_term, 128, [
      'action' => 'read_value',
      'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id;name',
      'as' => 'term_name'
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'id_space', $idspace_term, 128, [
      'action' => 'read_value',
      'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id;name',
      'as' => 'idSpace'
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'accession', $accession_term, 128, [
      'action' => 'read_value',
      'path' => $type_table. '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;accession',
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

    // Retrieve the base table from the default value set by the parent class.
    $base_table = $elements['storage_plugin_settings']['base_table']['#default_value'];

    // If this is an existing field, retrieve its storage settings.
    $storage_settings = $this->getSetting('storage_plugin_settings');

    $type_table = $storage_settings['type_table'] ?? '';
    $type_column = $storage_settings['type_column'] ?? '';
    $type_fkey = $storage_settings['type_fkey'] ?? '';

    // In the form, table and column will be selected together as a single unit
    $type_select = $type_fkey;
    if ($type_table and $type_column) {
      $type_select = $type_table . self::$table_column_delimiter . $type_column;
    }

    // Change the ajax callback on the base table select (from the parent form) so that
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
    $settings = self::getFormStateSettings($form_state);
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
   * {@inheritDoc}
   */
  public static function storageSettingsFormSubmitBaseTable(array $form, FormStateInterface $form_state) {
    parent::storageSettingsFormSubmitBaseTable($form, $form_state);

    $settings = self::getFormStateSettings($form_state);
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    // This field is used to specify a subset of a chado table for the bundle,
    // e.g. for feature table it could be 'gene'.
    // The first time this field is entered, store third party settings in the
    // entity for the table and column where this term will be stored, so that
    // publish will be able to restrict by the term.
    $type_fkey = $settings['storage_plugin_settings']['type_fkey'] ?? NULL;
    if ($type_fkey) {
      $type_table = $entity_type->getThirdPartySetting('tripal', 'bundle_type_table');
      if (!$type_table) {
        $form_state_storage = $form_state->getStorage();
        $bundle = $form_state_storage['bundle'];
        /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
        $entity_type_manager = \Drupal::entityTypeManager();
        /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
        $entity_type = $entity_type_manager->getStorage('tripal_entity_type')->load($bundle);

        list($type_table, $type_column) = explode(self::$table_column_delimiter, $type_fkey, 2);
        $entity_type->setThirdPartySetting('tripal', 'bundle_type_table', $type_table);
        $entity_type->setThirdPartySetting('tripal', 'bundle_type_column', $type_column);
        $entity_type->save();
      }
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

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = FALSE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');

    /** @var \Drupal\tripal_chado\Database\ChadoConnection $chado **/
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // If the base table has a 'type_id' column, then it is compatible.
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    if (isset($base_table_def['fields']['type_id'])) {
      $compatible = TRUE;
    }

    $prop_def = $schema->getTableDef($base_table . 'prop', ['format' => 'Drupal']);
    // If the property table exists, and has a foreign key to the base table,
    // then this content type is compatible.
    if ($prop_def) {
      if (array_key_exists($base_table, $prop_def['foreign keys'])) {
        $compatible = TRUE;
      }
    }

    return $compatible;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface::discover()
   */
  public static function discover(TripalEntityType $bundle, string $field_id, array $field_definitions) : array {

    /** @var \Drupal\tripal_chado\Database\ChadoConnection $chado **/
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // Initialize with an empty field list.
    $field_list = [];

    // Make sure the base table setting exists.
    $base_table = $bundle->getThirdPartySetting('tripal', 'chado_base_table');
    if (!$base_table) {
      return $field_list;
    }

    // For this field, we need either a "type_id" column in the base table,
    // or else have it specified in a property table. Sometimes we have both.
    $type_table = NULL;
    $type_column = NULL;
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_type_column = 'type_id';
    $base_type_id = $base_table_def['fields'][$base_type_column] ?? NULL;
    $prop_type_id = NULL;
    if ($base_type_id) {
      $type_table = $base_table;
      $type_column = $base_type_column;
    }
    else {
      $prop_table = $base_table . 'prop';
      $prop_type_column = 'type_id';
      if ($chado->schema()->tableExists($prop_table)) {
        $prop_table_def = $schema->getTableDef($prop_table, ['format' => 'Drupal']);
        $prop_type_id = $prop_table_def['fields'][$prop_type_column] ?? NULL;
        if ($prop_type_id) {
          $type_table = $prop_table;
          $type_column = $prop_type_columns;
        }
      }
    }

    // If neither of these two type_ids are present, then this field
    // is not discoverable for the base table.
    if (!$type_table) {
      return $field_list;
    }

    // Create a field entry in the list
    $termIdSpace = $bundle->getTermIdSpace();
    $termAccession = $bundle->getTermAccession();
    $fixed_value = $termIdSpace . ':' . $termAccession;
    $field_list[] = [
      'name' => self::generateFieldName($bundle, 'type', 0),
      'content_type' => $bundle->getID(),
      'label' => 'Type',
      'type' => self::$id,
      'description' => 'This field specifies the controlled vocabulary term'
          . ' for this content type as "' . $fixed_value . '"',
      'cardinality' => 1,
      'required' => TRUE,
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings' => [
          'base_table' => $base_table,
          'type_table' => $type_table,
          'type_column' => $type_column,
        ],
      ],
      'settings' => [
        'termIdSpace' => $termIdSpace,
        'termAccession' => $termAccession,
        'fixed_value' => $fixed_value,
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 10,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 10
          ],
        ],
      ],
    ];

    return $field_list;
  }

}
