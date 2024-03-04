<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType;

/**
 * Plugin implementation of default Tripal feature field type.
 *
 * @FieldType(
 *   id = "chado_feature_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Feature"),
 *   description = @Translation("Add a Chado feature to the content type."),
 *   default_widget = "chado_feature_widget_default",
 *   default_formatter = "chado_feature_formatter_default",
 * )
 */
class ChadoFeatureTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_feature_type_default';
  protected static $object_table = 'feature';
  protected static $object_id = 'feature_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'feature_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['base_table'] = '';
    $storage_settings['storage_plugin_settings']['linking_method'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['linker_fkey_column'] = '';
    $storage_settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // No default CV Term for this field
    // Gene is SO:0000704
    // mRNA is SO:0000234
    // QTL is SO:0000771
    // Genetic Marker is SO:0001645
    // Heritable Phenotypic Marker is SO:0001500
    // Sequence Variant is SO:0001060
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
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $record_id_term = 'SIO:000729';

    // Base table
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Object table
    $object_table = self::$object_table;
    $object_schema_def = $schema->getTableDef($object_table, ['format' => 'Drupal']);
    $object_pkey_col = $object_schema_def['primary key'];
    $object_pkey_term = $mapping->getColumnTermId($object_table, $object_pkey_col);

    // Columns specific to the object table
    $name_term = $mapping->getColumnTermId($object_table, 'name');
    $name_len = $object_schema_def['fields']['name']['size'];
    $uniquename_term = $mapping->getColumnTermId($object_table, 'uniquename'); // text
    // residues is not implemented in this field since it can be millions of characters long
    $seqlen_term = $mapping->getColumnTermId($object_table, 'seqlen');
    $md5checksum_term = $mapping->getColumnTermId($object_table, 'md5checksum');
    $md5checksum_len = $object_schema_def['fields']['md5checksum']['size'];
    $is_analysis_term = $mapping->getColumnTermId($object_table, 'is_analysis'); // boolean
    $is_obsolete_term = $mapping->getColumnTermId($object_table, 'is_obsolete'); // boolean
    // @todo timeaccessioned, timelastmodified not yet implemented

    // Columns from linked tables
    $dbxref_schema_def = $schema->getTableDef('dbxref', ['format' => 'Drupal']);
    $dbxref_term = $mapping->getColumnTermId('dbxref', 'accession');
    $dbxref_len = $dbxref_schema_def['fields']['accession']['size'];
    $db_schema_def = $schema->getTableDef('db', ['format' => 'Drupal']);
    $db_term = $mapping->getColumnTermId('db', 'name');
    $db_len = $db_schema_def['fields']['name']['size'];
    $cvterm_schema_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $type_term = $mapping->getColumnTermId('cvterm', 'name');
    $type_len = $cvterm_schema_def['fields']['name']['size'];
    $organism_schema_def = $schema->getTableDef('organism', ['format' => 'Drupal']);
    $genus_term = $mapping->getColumnTermId('organism', 'genus');
    $genus_len = $organism_schema_def['fields']['genus']['size'];
    $species_term = $mapping->getColumnTermId('organism', 'species');
    $species_len = $organism_schema_def['fields']['species']['size'];
    $infraspecific_name_term = $mapping->getColumnTermId('organism', 'infraspecific_name');
    $infraspecific_name_len = $organism_schema_def['fields']['infraspecific_name']['size'];
    $abbreviation_term = $mapping->getColumnTermId('organism', 'abbreviation');
    $abbreviation_len = $organism_schema_def['fields']['abbreviation']['size'];
    $common_name_term = $mapping->getColumnTermId('organism', 'common_name');
    $common_name_len = $organism_schema_def['fields']['common_name']['size'];

    // Linker table, when used, requires specifying the linker table and column.
    // For single hop, in the yaml we support using the usual 'base_table'
    // and 'base_column' settings.
    $linker_table = $storage_settings['linker_table'] ?? $base_table;
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? $object_pkey_col;

    $extra_linker_columns = [];
    if ($linker_table != $base_table) {
      $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // the following should be the same as $base_pkey_col @todo make sure it is
      $linker_left_col = array_keys($linker_schema_def['foreign keys'][$base_table]['columns'])[0];
      $linker_left_term = $mapping->getColumnTermId($linker_table, $linker_left_col);
      $linker_fkey_term = $mapping->getColumnTermId($linker_table, $linker_fkey_column);

      // Some but not all linker tables contain rank, type_id, and maybe other columns.
      // These are conditionally added only if they exist in the linker
      // table, and if a term is defined for them.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_fkey_column)) {
          $term = $mapping->getColumnTermId($linker_table, $column);
          if ($term) {
            $extra_linker_columns[$column] = $term;
          }
        }
      }
    }
    else {
      $linker_fkey_term = $mapping->getColumnTermId($base_table, $linker_fkey_column);
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // Base table links directly
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $linker_fkey_column,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);
    }
    // An intermediate linker table is used
    else {
      // Define the linker table that links the base table to the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'path' => $linker_table . '.' . $linker_pkey_col,
      ]);

      // Define the link between the base table and the linker table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link', $linker_left_term, [
        'action' => 'store_link',
        'drupal_store' => FALSE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col,
      ]);

      // Define the link between the linker table and the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $linker_table . '.' . $linker_fkey_column,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);

      // Other columns in the linker table. Set in the widget, but currently not implemented in the formatter.
      // Typically these are type_id and rank, but are not present in all linker tables,
      // so they are added only if present in the linker table.
      foreach ($extra_linker_columns as $column => $term) {
        $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_' . $column, $term, [
          'action' => 'store',
          'drupal_store' => FALSE,
          'path' => $linker_table . '.' . $column,
          'as' => 'linker_' . $column,
        ]);
      }
    }

    // The object table, the destination table of the linker table
    // The feature name
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_name', $name_term, $name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';name',
      'as' => 'feature_name',
    ]);

    // The feature uniquename - not null
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'feature_uniquename', $uniquename_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';uniquename',
      'as' => 'feature_uniquename',
    ]);

    // The feature sequence length
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'feature_seqlen', $seqlen_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';seqlen',
      'as' => 'feature_seqlen',
    ]);

    // The feature md5checksum
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_md5checksum', $md5checksum_term, $md5checksum_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';md5checksum',
      'as' => 'feature_md5checksum',
    ]);

    // Feature is analysis - not null, default=false
    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'feature_is_analysis', $is_analysis_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';is_analysis',
      'as' => 'feature_is_analysis',
    ]);

    // Feature is obsolete - not null, default=false
    $properties[] = new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'feature_is_obsolete', $is_obsolete_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';is_obsolete',
      'as' => 'feature_is_obsolete',
    ]);

    // @todo timeaccessioned, timelastmodified not yet implemented - not null, default CURRENT_TIMESTAMP

    // The type of feature
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_type', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.type_id>cvterm.cvterm_id;name',
      'as' => 'feature_type',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_genus', $genus_term, $genus_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.taxon_id>organism.organism_id;genus',
      'as' => 'feature_genus',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_species', $species_term, $species_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.taxon_id>organism.organism_id;species',
      'as' => 'feature_species',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_infraspecific_type', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.taxon_id>organism.organism_id;organism.type_id>cvterm.cvterm_id;name',
      'as' => 'feature_infraspecific_type',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_infraspecific_name', $infraspecific_name_term, $infraspecific_name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.taxon_id>organism.organism_id;infraspecific_name',
      'as' => 'feature_infraspecific_name',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_abbreviation', $abbreviation_term, $abbreviation_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.taxon_id>organism.organism_id;abbreviation',
      'as' => 'feature_abbreviation',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'feature_common_name', $common_name_term, $common_name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.taxon_id>organism.organism_id;common_name',
      'as' => 'feature_common_name',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'feature_database_accession', $dbxref_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.dbxref_id>dbxref.dbxref_id;accession',
      'as' => 'feature_database_accession',
    ]);

    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'feature_database_name', $db_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id;name',
      'as' => 'feature_database_name',
    ]);

    return $properties;
  }

}
