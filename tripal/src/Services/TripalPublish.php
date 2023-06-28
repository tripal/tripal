<?php

namespace Drupal\tripal\Services;

class TripalPublish {

  /**
   * The Tripal publish object.
   */
  protected $publish = NULL;



  /**
   * The main publish function.
   *
   * Publishes content to Tripal from Chado or another
   * specified datastore that matches the provided
   * filters.
   *
   * @param string $bundle
   *   The name of the bundle type to be published.
   * @param string $datastore
   *   The datastore that content will be published from. Can
   *   be a one of the available Chado instances or a
   *   custom datastore.
   * @param array $filters
   *   Filters that determine which content will be published.
   *
   * @return int
   *   The number of items published, FALSE on failure (for now).
   *
   * @todo The filter and datastore parameters.
   */
  public function publish($bundle, $datastore, $filters) {
    // Get the field info for this content type ($bundle).
    $published = FALSE;

    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $bundle);

    $settings = [];
    /** @var \Drupal\Core\Field\BaseFieldDefinition $field_definition **/
    $field_definition = NULL;
    foreach ($field_defs as $field_name => $field_definition) {
      print_r([$field_name, get_class($field_definition)]);
      print_r($field_definition);


      /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
      $field_type_manager = \Drupal::service('plugin.manager.field.field_type');
      $field = $field_type_manager->createInstance($field_definition['field_type']);
      print_r($field);



      if (!empty($field_definition->getTargetBundle())) {
        $storage_definition = $field_definition->getFieldStorageDefinition();
        //dpm($storage_definition);
        // Only get settings that are Chado fields?
        if ($storage_definition->getSetting('storage_plugin_id') == 'chado_storage') {
          $settings[$field_name] = $storage_definition->getSetting('storage_plugin_settings');
        }
      }
    }
    exit;

    // Get the table definitions (how ids are related and stuff)? E.g. how do we know how an
    // analysis relates to an analysysprop entry - obviously analysis_id but let's be sure.

    // Get the content from the database (Chado for now).
    $connection = \Drupal::service('tripal_chado.database');
    $schema = $connection->schema();

    $base_tables = [];
    $type_tables = [];

    foreach ($settings as $set => $setting) {
      // Use Tripal DBX Schema method to get the table definition.
      $td = $schema->getTableDef($setting['base_table'], []);
dpm($td);
      // Get the record ID (e.g. organism_id, analysis_id) column.
      $settings[$set]['base_table_pk'] = $td['primary key'];

      // Get the record ID of the linked table.
      // @todo are there other types of tables to worry about beyond base and type?
      if (array_key_exists('type_table', $setting)) {
        $td = $schema->getTableDef($setting['type_table'], []);
        $settings[$set]['type_table_pk'] = $td['primary key'];
      }
      // Gather table information into a nice format for creating
      // a database query.
      //   Base tables.
      if (!array_key_exists($setting['base_table'], $base_tables)) {
        $base_tables[$setting['base_table']] = [
          'primary_key' => $settings[$set]['base_table_pk'],
          'columns' => [$setting['base_column']]
        ];
      }
      else {
        $columns = $base_tables[$setting['base_table']]['columns'];
        array_push($columns, $setting['base_column']);
        $base_tables[$setting['base_table']]['columns'] = $columns;
      }
      //    Type tables.
      if (array_key_exists('type_table', $setting)) {
        if (!array_key_exists($setting['type_table'], $type_tables)) {
          // Get the primary key
          $type_tables[$setting['type_table']] = [
            'primary_key' => $settings[$set]['type_table_pk'],
            'columns' => [$setting['type_column']],
            ''
          ];
          // Get the information of the
        }
        else {
          $columns = $type_tables[$setting['type_table']]['columns'];
          array_push($columns, $setting['type_column']);
          $type_tables[$setting['type_table']]['columns'] = $columns;
        }
      }
    }
    // dpm($settings);
    dpm($base_tables);
    dpm($type_tables);
    // Craft a database query using Tripal DBX to fetch records from
    // the datastore.
    $base_table_names = array_keys($base_tables);
    $query = $connection->select($base_tables[$base_table_names[0]], substr($base_table_names[0], 0, 3));

    // Handle any other base tables. Save the first table name for later.
    // @todo How do we want to handle these base tables. In the Analysis example,
    // the extra field doesn't have a distinct way to identify how it relates to the
    // main base table (analysis). Will this ever be an actual case?
    // $first_table_name = array_shift($base_table_names);
    // foreach $

    // // Handle additional type tables.
    // $type_table_names = array_keys($type_tables);
    // foreach ($type_table_names as $type_table_name) {
    //   $query->join()
    // }

    //dpm($base_tables);
    // $query = $connection->select('');
    // select from table_columns=>base_table.base_column as $column


    //@ todo Apply any filters, including automatically filtering out already-published items
    //

    return $published;
  }
}
