<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal contact field type.
 *
 * @FieldType(
 *   id = "chado_contact_by_role_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Contacts: Specific Role"),
 *   description = @Translation("Supports linking contacts fullfilling a specific role to the current content type."),
 *   default_widget = "chado_contact_widget_default",
 *   default_formatter = "chado_contact_formatter_default",
 * )
 */
class ChadoContactByRoleTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_contact_by_role_type_default';
  protected static $object_table = 'contact';
  protected static $object_id = 'contact_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // Overrides the default of 'value'
    return 'contact_name';
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
    $storage_settings['storage_plugin_settings']['linking_method'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['linker_fkey_column'] = '';
    $storage_settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $storage_settings;
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

    $schemaObj = \Drupal::service('tripal_chado.database')->schema();
    $mappingObj = \Drupal::entityTypeManager()->getStorage('chado_term_mapping')->load('core_mapping');

    // Get the column, term and any other schema-related details.
    // A) BASE TABLE
    $base_schema_def = $schemaObj->getTableDef($base_table, ['format' => 'Drupal']);
    //    - primary key
    $base_pkey_col = $base_schema_def['primary key'];
    $terms['base_pkey'] = $terms['record_id'];
    // B) OBJECT TABLE (i.e. contact)
    $object_table = self::$object_table;
    $object_schema_def = $schemaObj->getTableDef($object_table, ['format' => 'Drupal']);
    //    - primary key
    $object_pkey_col = $object_schema_def['primary key'];
    $terms['object_pkey'] = $terms['record_id'];
    //    - name
    $terms['name'] = $mappingObj->getColumnTermId($object_table, 'name');
    $max_lengths['name'] = $object_schema_def['fields']['name']['size'];
    //    - description
    $terms['description'] = $mappingObj->getColumnTermId($object_table, 'description');
    $max_lengths['description'] = $object_schema_def['fields']['description']['size'];
    //    - contact type
    $cvterm_schema_def = $schemaObj->getTableDef('cvterm', ['format' => 'Drupal']);
    $terms['contact_type'] = $mappingObj->getColumnTermId('cvterm', 'name');
    $max_lengths['contact_type'] = $cvterm_schema_def['fields']['name']['size'];
    // C) LINKING TABLE.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);
    $linker_schema_def = $schemaObj->getTableDef($linker_table, ['format' => 'Drupal']);
    //    - primary key
    $linker_pkey_col = $linker_schema_def['primary key'];
    $terms['linker_pkey'] = $terms['record_id'];
    //    - left table foreign key
    $linker_left_col = array_keys($linker_schema_def['foreign keys'][$base_table]['columns'])[0];
    $terms['linker_left'] = $mappingObj->getColumnTermId($linker_table, $linker_left_col);
    //    - right table foreign key
    $terms['linker_right'] = $mappingObj->getColumnTermId($linker_table, $linker_fkey_column);
    //    - linking type
    $terms['linker_type_id'] = $mappingObj->getColumnTermId($linker_table, 'type_id');
    if (empty($terms['linker_type_id'])) {
      $terms['linker_type_id'] = $terms['contact_type'];
    }
    //    - rank
    $terms['linker_rank'] = $mappingObj->getColumnTermId($linker_table, 'rank');
    if (empty($terms['linker_rank'])) {
      $terms['linker_rank'] = 'OBCS:0000117';
    }

    // We need to create a table alias for our linker table in order to ensure
    // contact links with other roles are not combined.
    // The type used when creating the linking record will be the same as the
    // type set for the field. As such, we grab that here and use it in our
    // table alias.
    $field_settings = $field_definition->getSettings();
    $term = $field_settings['termIdSpace'] . ':' . $field_settings['termAccession'];
    $table_alias = $linker_table . '_' . preg_replace( '/[^a-z0-9]+/', '', strtolower( $term ) );
    $table_mapping = [$table_alias => $linker_table];

    // FINALLY, THE PROPERTIES!
    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $terms['base_pkey'], [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the Drupal entity ID of the linked chado
    // record, if one exists.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => self::$object_table,
      'fkey' => self::$object_id,
    ]);

    // Define the linker table that links the base table to the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $terms['linker_pkey'], [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => $table_alias . '.' . $linker_pkey_col,
      'table_alias_mapping' => $table_mapping,
    ]);

    // Define the link between the base table and the linker table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link', $terms['linker_left'], [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $table_alias . '.' . $linker_left_col,
      'table_alias_mapping' => $table_mapping,
    ]);

    // Define the link between the linker table and the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $linker_fkey_column, $terms['linker_right'], [
      'action' => 'store',
      'drupal_store' => TRUE,
      'path' => $table_alias . '.' . $linker_fkey_column,
      'table_alias_mapping' => $table_mapping,
      'delete_if_empty' => TRUE,
      'empty_value' => 0,
    ]);

    // Linker type_id
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_type_id', $terms['linker_type_id'], [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.type_id',
      'table_alias_mapping' => $table_mapping,
      'as' => 'linker_type_id',
    ]);

    // Linker Rank
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_rank', $terms['linker_rank'], [
      'action' => 'store',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.rank',
      'table_alias_mapping' => $table_mapping,
      'as' => 'linker_rank',
    ]);

    // The object table, the destination table of the linker table
    // The contact name
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_name', $terms['name'], $max_lengths['name'], [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';name',
      'table_alias_mapping' => $table_mapping,
      'as' => 'contact_name',
    ]);

    // The contact description
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_description', $terms['description'], $max_lengths['description'], [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';description',
      'table_alias_mapping' => $table_mapping,
      'as' => 'contact_description',
    ]);

    // The type of contact
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_type', $terms['contact_type'], $max_lengths['contact_type'], [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $table_alias . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.type_id>cvterm.cvterm_id;name',
      'table_alias_mapping' => $table_mapping,
      'as' => 'contact_type',
    ]);

    return $properties;
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
      if ($prop_value->getKey() == 'linker_type_id') {
        $prop_values[$index]->setValue($term->getInternalId());
      }
    }

    return $prop_values;
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {

    // Get the base table for the content type.
    $has_linker = FALSE;
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    // Get the list of tables linking the base table and our object table.
    $linker_tables = $this->getLinkerTables(self::$object_table, $base_table);
    if (!empty($linker_tables)) {
      $has_linker = TRUE;
    }

    // Ensure that the linker table has a type_id.
    $has_type_id = FALSE;
    $schemaObj = \Drupal::service('tripal_chado.database')->schema();
    foreach ($linker_tables as $item) {

      [$table_name, $contact_fkey] = $item;

      // Check there is a type_id field.
      $has_type_id = $schemaObj->fieldExists($table_name, 'type_id');

      if (!$has_type_id) {
        \Drupal::messenger()->addError('The Contact By Role field requires a type_id in the linking table. This is not present in Chado 1.31 but will likely be added in subsequent versions. For more information, see https://github.com/GMOD/Chado/pull/144.');
      }
    }

    // Only compatible if there is a linker and it has a type_id.
    if ($has_linker AND $has_type_id) {
      return TRUE;
    }
    return FALSE;
  }
}
