<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of Tripal linker property field type.
 *
 * @FieldType(
 *   id = "chado_property_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   default_widget = "chado_property_widget_default",
 *   default_formatter = "chado_property_formatter_default"
 * )
 */
class ChadoPropertyTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_property_type_default";

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['prop_table'] = '';
    return $settings;
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
  public static function tripalTypes($field_definition) {

    // Create variables for easy access to settings.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = array_key_exists('base_table', $settings) ? $settings['base_table'] : NULL;
    $prop_table = array_key_exists('prop_table', $settings) ? $settings['prop_table'] : NULL;

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the base table columns needed for this field.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $prop_schema_def = $schema->getTableDef($prop_table, ['format' => 'Drupal']);
    $prop_pkey_col = $prop_schema_def['primary key'];
    $prop_fk_col = array_keys($prop_schema_def['foreign keys'][$base_table]['columns'])[0];

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $link_term = $mapping->getColumnTermId($prop_table, $prop_fk_col);
    $value_term = $mapping->getColumnTermId($prop_table, 'value');
    $rank_term = $mapping->getColumnTermId($prop_table, 'rank');
    $type_id_term = $mapping->getColumnTermId($prop_table, 'type_id');

    // We need to create a table alias for our prop table in order to ensure
    // values of other property types are not combined.
    // The type used when creating the prop record will be the same as the
    // type set for the field. As such, we grab that here and use it in our
    // table alias.
    $field_settings = $field_definition->getSettings();
    $term = $field_settings['termIdSpace'] . ':' . $field_settings['termAccession'];
    $table_alias = $prop_table . '_' . preg_replace( '/[^a-z0-9]+/', '', strtolower( $term ) );


    // Create the property types.
    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col,
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'prop_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $prop_pkey_col,
        'table_alias_mapping' => [$table_alias => $prop_table],
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id',  $link_term, [
        'action' => 'store_link',
        'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $prop_fk_col,
        'table_alias_mapping' => [$table_alias => $prop_table],
      ]),
      new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $prop_fk_col . ';value',
        'table_alias_mapping' => [$table_alias => $prop_table],
        'delete_if_empty' => TRUE,
        'empty_value' => ''
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'rank', $rank_term,  [
        'action' => 'store',
        'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $prop_fk_col . ';rank',
        'table_alias_mapping' => [$table_alias => $prop_table],
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $type_id_term, [
        'action' => 'store',
        'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $prop_fk_col . ';type_id',
        'table_alias_mapping' => [$table_alias => $prop_table],
      ]),
    ];
  }

  /**
   * We need to set the type_id property value to match the cvterm_id.
   *
   * To do this we'll override the tripalValuesTemplate() and give the
   * `type_id` property a default value.
   *
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalField\TripalFieldItemBase::tripalValuesTemplate()
   */
  public function tripalValuesTemplate($field_definition, $default_value = NULL) {
    $prop_values = parent::tripalValuesTemplate($field_definition, $default_value);

    $settings = $field_definition->getSettings();

    $termIdSpace = $settings['termIdSpace'];
    $termAccession = $settings['termAccession'];

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
    }
    return $prop_values;
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
    $settings = self::getFormStateSettings($form_state);
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    $base_table = $settings['storage_plugin_settings']['base_table'];
    $prop_table = $base_table . 'prop';

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    if ($schema->tableExists($prop_table)) {
      $drupal_10_2 = $form_state->getValue(['field_storage']);
      if ($drupal_10_2) {
        $form_state->setValue(['field_storage', 'subform', 'settings', 'storage_plugin_settings', 'prop_table'], $prop_table);
      }
      else {
        $form_state->setValue(['settings', 'storage_plugin_settings', 'prop_table'], $prop_table);
      }
    }
    else {
      $form_state->setErrorByName('storage_plugin_settings][base_table',
          'The selected base table does not have an associated property table.');
    }
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

    // If the property table exists, and has a foreign key to the base table,
    // then this content type is compatible.
    $prop_def = $schema->getTableDef($base_table . 'prop', ['format' => 'Drupal']);
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
  public static function discover(TripalEntityType $bundle, string $field_id, array $field_definitions) : array{

    /** @var \Drupal\tripal_chado\Database\ChadoConnection $chado **/
    $chado = \Drupal::service('tripal_chado.database');

    // Initialize with an empty field list.
    $field_list = [];

    // Make sure the base table setting exists.
    $base_table = $bundle->getThirdPartySetting('tripal', 'chado_base_table');
    if (!$base_table) {
      return $field_list;
    }

    // Make sure the prop table exists in Chado.
    $prop_table = $base_table . 'prop';
    if (!$chado->schema()->tableExists($prop_table)) {
      return $field_list;
    }

    // Search for all unique types in the prop table.
    $query = $chado->select('1:' . $prop_table, 'pt');
    $query->leftJoin('1:cvterm', 'cvt', 'pt.type_id = cvt.cvterm_id');
    $query->leftJoin('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
    $query->leftJoin('1:db', 'db', 'db.db_id = dbx.db_id');
    $query->leftJoin('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
    $query->addField('cvt', 'cvterm_id');
    $query->addField('cvt', 'name', 'cvterm_name');
    $query->addField('cvt', 'definition');
    $query->addField('dbx', 'accession');
    $query->addField('db', 'name', 'db_name');
    $query->addField('cv', 'name', 'cv_name');
    $results = $query->distinct()->execute()->fetchAll();

    // Create a field entry for each property type.
    foreach ($results as $recprop) {
      $field_list[] = [
        'name' => self::generateFieldName($bundle, $recprop->cvterm_name),
        'content_type' => $bundle->getID(),
        'label' => ucwords($recprop->cvterm_name),
        'type' => self::$id,
        'description' => 'A record property with the following definition: ' . $recprop->definition,
        'cardinality' => -1,
        'required' => False,
        'storage_settings' => [
          'storage_plugin_id' => 'chado_storage',
          'storage_plugin_settings' => [
            'base_table' => $base_table,
            'prop_table' => $prop_table
          ],
        ],
        'settings' => [
          'termIdSpace' => $recprop->cv_name,
          'termAccession' => $recprop->accession,
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
    }

    return $field_list;
  }

}
