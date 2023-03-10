<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "chado_feature_synonym_default",
 *   label = @Translation("Chado Synonym"),
 *   description = @Translation("A chado syonym"),
 *   default_widget = "chado_synonym_widget_default",
 *   default_formatter = "chado_synonym_formatter_default"
 * )
 */
class ChadoSynonymDefault extends ChadoFieldItemBase {

  public static $id = "chado_synonym_default";

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
#    $settings['storage_plugin_settings']['linked_table'] = '';
    return $settings;
  }


  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    $settings['termIdSpace'] = 'schema';
    $settings['termAccession'] = 'alternateName';
    return $settings;
  }


  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    
    // set linked_table to synonym
#    $linked_table = 'synonym';

    // We need to set the prop table for this field but we need to know
    // the base table to do that. So we'll add a new validation function so
    // we can get it and set the proper storage settings.

    # most common use of alias is with a feature
    $default_base_table = 'feature'; 

    // Find base tables.
    $base_tables = [];
    $base_tables[NULL] = '-- Select --';
    foreach (array('feature','cell_line','library') as $table) {
      $base_tables[$table] = $table;
    }



    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $elements['storage_plugin_settings']['base_table']['#options'] = $base_tables;
    $elements['storage_plugin_settings']['base_table']['#element_validate'] = [[static::class, 'storageSettingsFormValidate']];
    $elements['storage_plugin_settings']['base_table']['#default_value'] = $default_base_table ;


    ### here is where i need to calculate the linker table, and if more than one, set a  dropdown

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
    $settings = $form_state->getValue('settings');
    if (!array_key_exists('storage_plugin_settings', $settings)) {
      return;
    }
    $base_table = $settings['storage_plugin_settings']['base_table'];
#    $linked_table = 'synonym';
#
#    ## maybe this is where i can set the linker table
#
#    $chado = \Drupal::service('tripal_chado.database');
#    $schema = $chado->schema();
#    if ($schema->tableExists($linked_table)) {
#      $form_state->setValue(['settings', 'storage_plugin_settings', 'linked_table'], $linked_table);
#    }
#    else {
#      $form_state->setErrorByName('storage_plugin_settings][linked_table',
#          'The set linked table does not have an associated table.');
#    }
  }



  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    // Get the settings for this field.
    $settings = $field_definition->getSetting('storage_plugin_settings');

    $base_table = $settings['base_table'];
    $linked_table = 'synonym';

    ## want to auto assign feature as basetable if starating from manage fields of gene content type
    ## but dont know how to do this, so I will have to select feature table

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      $record_id_term = 'SIO:000729';
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Determine the primary key of the base table.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    
    // Get the property terms by using the Chado table columns they map to.
    $record_id_term = 'SIO:000729';

    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $synonym_schema_def = $schema->getTableDef('synonym', ['format' => 'Drupal']);
    $pub_schema_def = $schema->getTableDef('pub', ['format' => 'Drupal']);

    $synonym_linker_tables = explode(", ",$synonym_schema_def['referring_tables']);    
    $base_linker_tables = explode(", ",$base_schema_def['referring_tables']);    

    $linker_tables = array_intersect($synonym_linker_tables,$base_linker_tables);
    # if more than one, create a dropdown to get user to select correct linker table
    # else, if only one
    $linker_table = array_shift($linker_tables);
    $linker_schema_def = $schema->getTableDef($linker_table, ['format' => 'Drupal']);

    $settings['storage_plugin_settings']['linker_table'] = $linker_table;
    

    // Get the length of the database fields so we don't go over the size limit.
    $syn_name_term = $mapping->getColumnTermId('synonym', 'name');
    $syn_name_len = $synonym_schema_def['fields']['name']['size'];

    $type_id_term = $mapping->getColumnTermId('synonym','type_id');
    $pub_id_term = $mapping->getColumnTermId($linker_table, 'pub_id');

    ## base_table will be cell_line, feature, or library
    $base_pkey_col = $base_schema_def['primary key']; #feature_id
    $linker_pkey_col = $linker_schema_def['primary key']; #feature_synonym_id
    $base_fk_col = array_keys($linker_schema_def['foreign keys'][$base_table]['columns'])[0];
    $syn_id_term = $mapping->getColumnTermId('synonym', 'synonym_id');


    // Base table     : feature              : cell_line             : library
    // Linker table   : feature_synonymn     : cell_line_synonym     : library_synonym
    // Linked table   : synonym              : synonym               : synonym

    ## record_id is the pkey of the base_table. ie, feature_id, or library_id, or cell_line_id
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
        'action' => 'store_id', ## indicates that the value of this property will hold the record ID (or primary key ID) of the record in the base table of Chado.
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
    ]);
    ## primary key in linker table: feature_synonym
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_pk_id', $record_id_term, [
        'action' => 'store_pkey', # 'store_pkey', ## indicates that the value of this property will hold the primary key ID of a linked table.
        'drupal_store' => TRUE,
        'chado_table' => $linker_table,
        'chado_column' => $linker_pkey_col,
    ]);
    ## feature_id.feature_synonym
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'base_fk_id' , $record_id_term, [
      'action' => 'store_link', 
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => $base_fk_col,
    ]);
   ## type_id.synonym
   ## gets set in widget
   $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'syn_type_id', $type_id_term , [
        'action' => 'store', 
        'chado_table' => 'synonym',
        'chado_column' => 'type_id',
    ]);

    ### primary key in synonym table
    ## synonym_id.synonym
     $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'synonym_pk_id', $syn_id_term, [
        # this is a primary key of itself, but trying store_pkey
        'action' => 'store_id' ,
        'drupal_store' => TRUE,
        'chado_table' => 'synonym',
        'linked_table' => 'synonym', # include linked_table if this is not the base_table, but is the linked table (base >base.linked >linked)
        'chado_column' => 'synonym_id',
    ]);
 
   ## the new or updated synonym name
   $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'name', $syn_name_term, $syn_name_len, [
        'action' => 'store', ## indicates that the value of this property should be stored in the Chado table.
        'chado_table' => 'synonym',
        'chado_column' => 'name',
    ]);
   ## the new or updated synonym name
   ## gets set in widget
   $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'synonym_sgml', $syn_name_term, $syn_name_len, [
        'action' => 'store', ## indicates that the value of this property should be stored in the Chado table.
        'chado_table' => 'synonym',
        'chado_column' => 'synonym_sgml',
    ]);
   
    ## synonym_id.feature_synonym
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'synonym_fk_id', $syn_id_term, [
      'action' => 'store_link',  
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'linked_table' => $linked_table, #synonym
      'chado_column' => 'synonym_id',
    ]);
    ## pub_id.feature_synonyn 
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'pub_id' , $pub_id_term, [
      'action' => 'store',
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => 'pub_id',
    ]);
    
    return $properties;
  }

}
