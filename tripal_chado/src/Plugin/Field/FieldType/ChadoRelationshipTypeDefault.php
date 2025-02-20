<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;


/**
 * Plugin implementation of default Tripal relationship field type.
 *
 * @FieldType(
 *   id = "chado_relationship_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Relationship"),
 *   description = @Translation("Add a relationship to the content type."),
 *   default_widget = "chado_relationship_widget_default",
 *   default_formatter = "chado_relationship_formatter_default",
 *   cardinality = -1
 * )
 */
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
    $storage_settings['base_column'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
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

    // Relationship table
    $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    $linker_pkey_col = $linker_schema_def['primary key'];
    // Relationship table column naming follows a predictable pattern
    $linker_subject_col = 'subject_id';
    $linker_object_col = 'object_id';
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
      'fkey' => $linker_subject_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => $base_table,
      'fkey' => $linker_object_col,
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
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // Define the link between the relationship object and the base table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $linker_object_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_object_col,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // The column which will be used for the record name. One will be the hosting record,
    // but we don't know in advance which it is, so store both.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_name', $base_column_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_subject_col . '>' . $base_table . '.' . $base_pkey_col . ';' . $base_column,
      'as' => 'subject_name',
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_name', $base_column_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_object_col . '>' . $base_table . '.' . $base_pkey_col . ';' . $base_column,
      'as' => 'object_name',
    ]);

    // The type of relationship
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $linker_type_term, [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_type_col,
      'empty_value' => 0,
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'type_name', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_type_col . '>cvterm.cvterm_id;name',
      'as' => 'type_name',
    ]);

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
      // We need to know which column in the base table should be used for
      // an autocomplete.
      // @todo maybe check title format, for now hardcoded
      $base_column = 'name';
      if ($base_column) {
        /** @var \Drupal\tripal_chado\Database\ChadoConnection $chado **/
        $chado = \Drupal::service('tripal_chado.database');

        // Make sure the relationship table exists in Chado.
        $relationship_table = $base_table . '_relationship';
        if ($chado->schema()->tableExists($relationship_table)) {
          if ($chado->schema()->fieldExists($base_table, $base_column)) {

            $field_list[] = [
              'name' => self::generateFieldName($bundle, 'relationship'),
              'content_type' => $bundle->getID(),
              'label' => 'Relationship',
              'type' => self::$id,
              'description' => 'Relationships between records.',
              'cardinality' => -1,
              'required' => FALSE,
              'storage_settings' => [
                'storage_plugin_id' => 'chado_storage',
                'storage_plugin_settings' => [
                  'base_table' => $base_table,
                  'base_column' => $base_column,
                  'linker_table' => $relationship_table,
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

}
