<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\TripalStorage\StoragePropertyValue;

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

    // @TODO: remove this hardcoding once the $datastore argument is working.
    $datastore = 'chado_storage';

    // Get the storage plugin used to publish.
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager **/
    $storage_manager = \Drupal::service('tripal.storage');
    $storage = $storage_manager->getInstance(['plugin_id' => $datastore]);

    // Here we'll store the array of searchable properties. This should be the
    // expected input format for the TripalStorageManager::findValues() function.
    $search_values = [];

    // Get the field manager, field definitions for the bundle type, and
    // the field type manager.
    /** @var \Drupal\Core\Entity\EntityFieldManager $field_manager **/
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $bundle);
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    // Iterate over the field definitions for the bundle.
    /** @var \Drupal\Core\Field\BaseFieldDefinition $field_definition **/
    $field_definition = NULL;
    $settings = [];
    foreach ($field_defs as $field_name => $field_definition) {

      if (!empty($field_definition->getTargetBundle())) {
        $storage_definition = $field_definition->getFieldStorageDefinition();
        if ($storage_definition->getSetting('storage_plugin_id') == $datastore) {
          $configuration = [
            'field_definition' => $field_definition,
            'name' => $field_name,
            'parent' => NULL,
          ];
          $instance = $field_type_manager->createInstance($field_definition->getType(), $configuration);
          $prop_types = $instance->tripalTypes($field_definition);
          $field_class = get_class($instance);

          /** @var \Drupal\tripal\TripalStorage\StoragePropertyTypeBase $prop_type **/
          foreach ($prop_types as $prop_type) {
            if ($prop_type->getSearchability() == TRUE) {
              $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
                  $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
              $search_values[$field_name][0][$prop_type->getKey()] = [
                'type' => $prop_type,
                'value' => $prop_value,
                'operation' => '<>',
                'definition' => $field_definition
              ];
            }
          }
        }
      }
    }
   $storage->findValues($search_values);
  }
}
