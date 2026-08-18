<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal relationship by role field type.
 */
#[TripalFieldType(
  id: 'chado_relationship_by_role_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Relationship: Specific Role'),
  description: new TranslatableMarkup('Supports linking relationships fullfilling a specific role to the current content type.'),
  default_widget: 'chado_relationship_widget_default',
  default_formatter: 'chado_relationship_formatter_default',
)]
class ChadoRelationshipByRoleTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = 'chado_relationship_by_role_type_default';

  /**
   * Indicate if we should provide a column selector in the add field form.
   *
   * @var bool
   *   If TRUE then provide the select element for the column; if FALSE don't.
   * @see ChadoFieldItemBase
   */
  protected static $select_base_column = TRUE;

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // The property that indicates if this field is empty.
    return 'linker_id';
  }

  /**
   * {@inheritdoc}
   */
  public static function mainDisplayPropertyName() {
    // The property to use in the entity title/url.
    return 'value';
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
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['base_column'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['subject_column'] = '';
    $storage_settings['storage_plugin_settings']['object_column'] = '';
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $value['record_id'] = 0;
    $value['linker_id'] = 0;

    $value['subject_id'] = 0;
    $value['subject_entity_id'] = 0;
    $value['subject_name'] = '';

    $value['object_id'] = 0;
    $value['object_entity_id'] = 0;
    $value['object_name'] = '';

    $value['type_id'] = mt_rand(1, 500);
    $value['type_name'] = '';

    $value['relationship_value'] = '';
    $value['relationship_rank'] = 0;

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];

    if (empty($base_table)) {
      return;
    }

    $terms = [
      'record_id' => self::$record_id_term,
    ];
    $max_lengths = [];

    $chado = \Drupal::service('tripal_chado.database');
    $schemaObj = $chado->schema();
    $mappingObj = \Drupal::entityTypeManager()->getStorage('chado_term_mapping')->load('core_mapping');

    // Get the column, term and any other schema-related details.
    // BASE TABLE.
    $base_schema_def = $schemaObj->getTableDef($base_table, ['format' => 'Drupal']);
    // - primary key
    $base_pkey_col = $base_schema_def['primary key'];
    $terms['base_pkey'] = $terms['record_id'];

    $base_column = $storage_settings['base_column'];
    $terms['base_column'] = $mappingObj->getColumnTermId($base_table, $base_column, 'schema:name');

    // Relationship table.
    $linker_table = $storage_settings['linker_table'] ?? ($base_table . '_relationship');
    $linker_schema_def = $schemaObj->getTableDef($linker_table, ['format' => 'Drupal']);
    $linker_pkey_col = $linker_schema_def['primary key'];
    $terms['linker_pkey'] = $terms['record_id'];
    // Relationship table column naming is not consistent for
    // nd_reagent and project.
    $linker_subject_col = $storage_settings['subject_column'] ?? NULL;
    $linker_object_col = $storage_settings['object_column'] ?? NULL;
    if (!$linker_subject_col || !$linker_object_col) {
      // When this field is added through the UI, these will not have been
      // set yet, so save the settings.
      [$linker_subject_col, $linker_object_col] = self::getRelationshipColumns($chado, $base_table, $linker_table);
      $storage_settings['subject_column'] = $linker_subject_col;
      $storage_settings['object_column'] = $linker_object_col;
      $field_definition->setSetting('storage_plugin_settings', $storage_settings);
    }
    $linker_type_col = 'type_id';
    $terms['linker_subject'] = $mappingObj->getColumnTermId($linker_table, $linker_subject_col, 'local:relationship_subject');
    $terms['linker_object'] = $mappingObj->getColumnTermId($linker_table, $linker_object_col, 'local:relationship_object');
    $terms['linker_type'] = $mappingObj->getColumnTermId($linker_table, $linker_type_col, 'schema:additionalType');

    // Columns from linked tables to specify the relationship type.
    $cvterm_schema_def = $schemaObj->getTableDef('cvterm', ['format' => 'Drupal']);
    $terms['type_name'] = $mappingObj->getColumnTermId('cvterm', 'name', 'schema:additionalType');
    $max_lengths['type_name'] = $cvterm_schema_def['fields']['name']['size'];

    // Value.
    $terms['relationship_value'] = $mappingObj->getColumnTermId($linker_table, 'value') ?: 'NCIT:C25712';

    // Rank.
    $terms['relationship_rank'] = $mappingObj->getColumnTermId($linker_table, 'rank') ?: 'OBCS:0000117';

    // Create a table alias for the linker table in order to ensure
    // relationship links with other roles are not combined. Use the field
    // CV term to scope the alias to this field's role .
    $field_settings = $field_definition->getSettings();
    $term = $field_settings['termIdSpace'] . ':' . $field_settings['termAccession'];
    $table_alias = $linker_table . '_' . preg_replace('/[^a-z0-9]+/', '', strtolower($term));
    $table_mapping = [$table_alias => $linker_table];

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $terms['base_pkey'], [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // Drupal entity IDs for subject/object.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => $base_table,
      'fkey' => 'subject_id',
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => $base_table,
      'fkey' => 'object_id',
    ]);

    // Define the relationship linker pkey using the alias.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $terms['linker_pkey'], [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => $table_alias . '.' . $linker_pkey_col,
      'table_alias_mapping' => $table_mapping,
    ]);

    // Links between base and linker (subject/object) using alias mapping.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_id', $terms['linker_subject'], [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $linker_subject_col,
      'table_alias_mapping' => $table_mapping,
      'as' => 'subject_id',
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $terms['linker_object'], [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $linker_object_col,
      'table_alias_mapping' => $table_mapping,
      'as' => 'object_id',
    ]);

    // Names for subject/object.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'subject_name', $terms['base_column'], [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_subject_col . '>' . $base_table . '.' . $base_pkey_col . ';' . $base_column,
      'table_alias_mapping' => $table_mapping,
      'as' => 'subject_name',
    ]);
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'object_name', $terms['base_column'], [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_object_col . '>' . $base_table . '.' . $base_pkey_col . ';' . $base_column,
      'table_alias_mapping' => $table_mapping,
      'as' => 'object_name',
    ]);

    // Type id and name via alias.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $terms['linker_type'], [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_type_col,
      'table_alias_mapping' => $table_mapping,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'type_name', $terms['type_name'], $max_lengths['type_name'], [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_type_col . '>cvterm.cvterm_id;name',
      'table_alias_mapping' => $table_mapping,
      'as' => 'type_name',
    ]);

    if (array_key_exists('value', $linker_schema_def['fields'])) {
      $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'relationship_value', $terms['relationship_value'], [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $table_alias . '.value',
        'table_alias_mapping' => $table_mapping,
        'as' => 'relationship_value',
      ]);
    }

    if (array_key_exists('rank', $linker_schema_def['fields'])) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'relationship_rank', $terms['relationship_rank'], [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $table_alias . '.rank',
        'table_alias_mapping' => $table_mapping,
        'as' => 'relationship_rank',
      ]);
    }

    return $properties;
  }

  /**
   * We need to set the type_id property value to match the field's term.
   *
   * To do this we'll override the tripalValuesTemplate() and give the
   * `type_id` property a default value.
   *
   * {@inheritdoc}
   *
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
   * {@inheritDoc}
   *
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {

    // Get the base table for the content type.
    $has_linker = FALSE;
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    // Relationship tables have a standard naming method.
    $relationship_table = $base_table . '_relationship';
    $table_exists = $schema->tableExists($relationship_table);
    if ($table_exists) {
      $has_linker = TRUE;
    }

    // Ensure that the relationship table has a type_id.
    $has_type_id = FALSE;

    // Only check for the type_id column if the relationship table exists,
    // otherwise checking for the field would throw a database exception.
    if ($has_linker) {
      $has_type_id = $schema->fieldExists($relationship_table, 'type_id');
      if (!$has_type_id) {
        \Drupal::messenger()->addError('The Relationship By Role field requires a type_id in the linking table. This is not present in Chado 1.31 but will likely be added in subsequent versions.');
      }
    }

    // Only compatible if there is a linker and it has a type_id.
    if ($has_linker and $has_type_id) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Returns the names of the relationship subject and object columns.
   *
   * This lookup is necessary because column names are not consistent
   * for nd_reagent and project.
   *
   * @param Drupal\tripal_chado\Database\ChadoConnection $chado
   *   Connection to Chado database.
   * @param string $base_table
   *   The name of the base table, e.g. "project".
   * @param string $relationship_table
   *   The name of the relationship table, e.g. "project_relationship".
   *
   * @return array
   *   Array with two elements, the names of subject and object columns.
   */
  protected static function getRelationshipColumns(ChadoConnection $chado, string $base_table, string $relationship_table): array {
    $subject_column = '';
    $object_column = '';
    // The subject and object columns will be among the foreign keys
    // to the base table.
    $schema = $chado->schema();
    $foreign_key_def = self::getChadoForeignKeyDef($relationship_table, $base_table, $schema);
    if ($foreign_key_def) {
      foreach (array_keys($foreign_key_def['columns']) as $relationship_column) {
        if (preg_match('/subject/', $relationship_column)) {
          $subject_column = $relationship_column;
        }
        elseif (preg_match('/object/', $relationship_column)) {
          $object_column = $relationship_column;
        }
      }
    }
    return [$subject_column, $object_column];
  }

}
