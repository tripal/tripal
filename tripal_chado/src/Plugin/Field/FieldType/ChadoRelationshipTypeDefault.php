<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal relationship field type.
 */
#[TripalFieldType(
  id: 'chado_relationship_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Relationship'),
  description: new TranslatableMarkup('Add a relationship to the content type.'),
  default_widget: 'chado_relationship_widget_default',
  default_formatter: 'chado_relationship_formatter_default',
  cardinality: -1,
)]
class ChadoRelationshipTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_relationship_type_default';
  protected static $termIdSpace = 'SBO';
  protected static $termAccession = '0000374';

  // This is a flag to the ChadoFieldItemBase parent
  // class to provide a column selector in the form
  protected static $select_base_column = TRUE;

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'value';
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
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'Relationship'
    $field_settings['termIdSpace'] = self::$termIdSpace;
    $field_settings['termAccession'] = self::$termAccession;
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create a variable for easy access to settings.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the various tables and columns needed for this field.
    // We will get the property terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Base table
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $base_column = $storage_settings['base_column'];
    $base_column_term = self::getColumnTermId($base_table, $base_column, 'schema:name');

    // Relationship table
    $linker_table = $storage_settings['linker_table'] ?? ($base_table . '_relationship');
    $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    $linker_pkey_col = $linker_schema_def['primary key'];
    // Relationship table column naming is not consistent for nd_reagent and project
    $linker_subject_col = $storage_settings['subject_column'] ?? NULL;
    $linker_object_col = $storage_settings['object_column'] ?? NULL;
    if (!$linker_subject_col || !$linker_object_col) {
      // When this field is added through the UI, these will not have been set yet, so save the settings
      [$linker_subject_col, $linker_object_col] = self::getRelationshipColumns($chado, $base_table, $linker_table);
      $storage_settings['subject_column'] = $linker_subject_col;
      $storage_settings['object_column'] = $linker_object_col;
      $field_definition->setSetting('storage_plugin_settings', $storage_settings);
    }
    $linker_type_col = 'type_id';
    $linker_subject_term = self::getColumnTermId($linker_table, $linker_subject_col, 'local:relationship_subject');
    $linker_object_term = self::getColumnTermId($linker_table, $linker_object_col, 'local:relationship_object');
    $linker_type_term = self::getColumnTermId($linker_table, $linker_type_col, 'schema:additionalType');

    // Columns from linked tables to specify the relationship type
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $type_term = self::getColumnTermId('cvterm', 'name', 'schema:additionalType');
    $type_len = $cvterm_schema_def['fields']['name']['size'];

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the Drupal entity ID of the referenced chado
    // record, if one exists. For this field, this can be either the
    // subject or the object.
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

    // Define the relationship table that links the base table back to itself at another record.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', self::$record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => $linker_table . '.' . $linker_pkey_col,
    ]);

    // Define the link between the base table and the relationship subject.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_id', $linker_subject_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_subject_col,
      'as' => 'subject_id',
    ]);

    // Define the link between the relationship object and the base table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $linker_object_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_object_col,
      'as' => 'object_id',
    ]);

    // The column which will be used for the record name. One will be the hosting record,
    // but we don't know in advance which it is, so store both.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'subject_name', $base_column_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_subject_col . '>' . $base_table . '.' . $base_pkey_col . ';' . $base_column,
      'as' => 'subject_name',
    ]);
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'object_name', $base_column_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_object_col . '>' . $base_table . '.' . $base_pkey_col . ';' . $base_column,
      'as' => 'object_name',
    ]);

    // The type of relationship. This is used as the delete if empty trigger.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $linker_type_term, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_type_col,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'type_name', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_type_col . '>cvterm.cvterm_id;name',
      'as' => 'type_name',
    ]);

    // Some but not all relationship tables contain value or rank columns.
    // These are conditionally added only if they exist in the relationship
    // table.
    if (array_key_exists('value', $linker_schema_def['fields'])) {
      $term = self::getColumnTermId($linker_table, 'value', 'NCIT:C25712');
      $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'relationship_value', $term, [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.value',
        'as' => 'relationship_value',
      ]);
    }
    if (array_key_exists('rank', $linker_schema_def['fields'])) {
      $term = self::getColumnTermId($linker_table, 'rank', 'OBCS:0000117');
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'relationship_rank', $term, [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.rank',
        'as' => 'relationship_rank',
      ]);
    }

    return $properties;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = FALSE;

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    // Relationship tables have a standard naming method
    $relationship_table = $base_table . '_relationship';
    $relationship_schema_def = $schema->getTableDef($relationship_table, ['format' => 'Drupal']);
    if ($relationship_schema_def) {
      $compatible = TRUE;
    }

    return $compatible;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface::discover()
   */
  public static function discover(TripalEntityType $bundle, string $field_id, array $field_types, array $field_instances, array $options = []): array {

    // Initialize with an empty field list.
    $field_list = [];

    // Make sure the base table setting exists.
    $base_table = $bundle->getThirdPartySetting('tripal', 'chado_base_table');
    if ($base_table) {
      /** @var \Drupal\tripal_chado\Database\ChadoConnection $chado **/
      $chado = \Drupal::service('tripal_chado.database');
      // We need to know which column in the base table should be used for an
      // autocomplete. When this field is added through UI this can be selected.
      // We will get this from the title format if possible, otherwise use 'name'.
      $base_column = self::getBaseColumnFromTitleFormat($chado, $bundle, $base_table);

      if ($base_column) {
        // Make sure the relationship table exists in Chado.
        $relationship_table = $base_table . '_relationship';
        if ($chado->schema()->tableExists($relationship_table)) {

          // Lookup actual names of subject_id and object_id columns
          [$subject_column, $object_column] = self::getRelationshipColumns($chado, $base_table, $relationship_table);
          if ($subject_column and $object_column) {

            $field_list[] = [
              'name' => self::generateFieldName($bundle, 'relationship'),
              'content_type' => $bundle->getID(),
              'label' => 'Relationship',
              'type' => self::$id,
              'description' => 'Other records with relationships to this record.',
              'cardinality' => -1,
              'required' => FALSE,
              'storage_settings' => [
                'storage_plugin_id' => 'chado_storage',
                'storage_plugin_settings' => [
                  'base_table' => $base_table,
                  'base_column' => $base_column,
                  'linker_table' => $relationship_table,
                  'subject_column' => $subject_column,
                  'object_column' => $object_column,
                ],
              ],
              'settings' => [
                'termIdSpace' => self::$termIdSpace,
                'termAccession' => self::$termAccession,
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
            // The parent class adds collection plugin IDs
            $field_list = parent::discoverPostprocess($field_list);
          }
        }
      }
    }
    return $field_list;
  }

  /**
   * Finds the name of the base table column to be used for discovery.
   *
   * @param \Drupal\tripal_chado\Database\ChadoConnection $chado
   *   Connection to the chado database
   * @param \Drupal\tripal\Entity\TripalEntityType $bundle
   *   The bundle object
   * @param string $base_table
   *   The name of the chado base table
   * @return string
   *   The name of a column in the base table, or an empty string.
   */
  protected static function getBaseColumnFromTitleFormat(\Drupal\tripal_chado\Database\ChadoConnection $chado,
      \Drupal\tripal\Entity\TripalEntityType $bundle, string $base_table): string {
    $title_format = $bundle->getTitleFormat();
    $bundle_id = $bundle->id();

    // We default to 'name', but if the title format has just a single token
    // from the bundle, then use that. For example, for bundle 'pub' the token
    // is '[pub_title]' so we will use the 'title' column.
    // The notable case of failure for this is for organism.
    $base_column = 'name';
    if (preg_match_all('/\[' . $bundle_id . '_([^\[\]]+)\]/', $title_format, $matches)) {
      // Use the captured pattern
      if (count($matches[1]) == 1) {
        $base_column = $matches[1][0];
      }
    }

    // Validation that the column exists
    if (!$base_column or !$chado->schema()->fieldExists($base_table, $base_column)) {
      $base_column = '';
    }
    return $base_column;
  }

  /**
   * Returns the names of the subject and object columns in the relationship table.
   * This lookup is necessary because column names are not consistent for nd_reagent and project.
   *
   * @param $chado
   *   Connection to Chado database
   * @param string $base_table
   *   The name of the base table, e.g. "project"
   * @param string $relationship_table
   *   The name of the relationship table, e.g. "project_relationship"
   * @return array
   *   Array with two elements, the names of subject and object columns.
   */
  protected static function getRelationshipColumns($chado, string $base_table, string $relationship_table): array {
    $subject_column = '';
    $object_column = '';
    // The subject and object columns will be among the foreign keys to the base table
    $table_schema_def = $chado->schema()->getTableDef($relationship_table, ['format' => 'Drupal']);
    if ($table_schema_def['foreign keys'][$base_table]['columns'] ?? NULL) {
      foreach (array_keys($table_schema_def['foreign keys'][$base_table]['columns']) as $relationship_column) {
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
