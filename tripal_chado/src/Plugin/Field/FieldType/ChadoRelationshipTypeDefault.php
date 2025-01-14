<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
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
#    $storage_settings['storage_plugin_settings']['linking_method'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['linker_fkey_column'] = '';
#    $storage_settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'Relationship'
    $field_settings['termIdSpace'] = 'SBO';
    $field_settings['termAccession'] = '0000374';
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
dpm($field_definition, "CP2 field_definition");//@@@

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

    // Relationship tables have a standard naming method
    $linker_table = $base_table . '_relationship';
#    $linker_fkey_column = $storage_settings['linker_fkey_column']
#      ?? $storage_settings['base_column'] ?? $object_pkey_col;

    // Relationship table
    $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    $linker_pkey_col = $linker_schema_def['primary key'];
    // Relationship table column naming follows a predictable pattern
    $linker_subject_col = 'subject_' . $base_table . '_id';
    $linker_object_col = 'object_' . $base_table . '_id';
    $linker_type_col = 'type_id';
    $linker_subject_term = self::getColumnTermId($linker_table, $linker_subject_col, self::$record_id_term);
    $linker_object_term = self::getColumnTermId($linker_table, $linker_object_col, self::$record_id_term);
    $linker_type_term = self::getColumnTermId($linker_table, $linker_type_col, self::$record_id_term);

    // Columns from linked tables to specify the relationship type
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $dbxref_term = self::getColumnTermId('dbxref', 'accession', 'data:2091');
    $db_term = self::getColumnTermId('db', 'name', 'ERO:0001716');
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
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject', $linker_subject_term, [
      'action' => 'store_link',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_subject_col,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // Define the link between the relationship object and the base table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object', $linker_object_term, [
      'action' => 'store',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_object_col,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // The type of relationship
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'relationship_type', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_type_col . '>cvterm.cvterm_id;name',
      'as' => 'relationship_type',
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
    $linker_table = $base_table . '_relationship';
    $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
    if ($linker_schema_def) {
      $compatible = TRUE;
    }

dpm($compatible, "CP1 compatible with table $base_table");//@@@
    return $compatible;
  }

}
