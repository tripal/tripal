<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Url;
use Drupal\field\Entity\FieldStorageConfig;
use \Drupal\tripal\Services\TripalEntityTitle;

class TripalEntityLookup {

  /**
   * The top-level function, used by fields to get a ready-to-use url to link to an entity.
   *
   * @param string $displayed_string
   *   The text that will be displayed as a url link
   * @param integer $record_id
   *   The primary key value for the requested record
   * @param array $item_settings
   *   Contains the following key-value pairs:
   *   'storage_plugin_id' => The id of the TripalStorage plugin, e.g. "chado_storage"
   *   'termIdSpace' => The bundle's CV term namespace e.g. "NCIT"
   *   'termAccession' => The bundle's CV term accession e.g. "C47954"
   *
   * @return array
   *   If a link is possible, then appropriate render array values to generate the link.
   *   If no link is possible, then appropriate render array values for simple markup.
   */
  public function getRenderableItem($displayed_string, $record_id, $item_settings) {

    // Default render array is to just display the passed string.
    $renderable_item = [
      '#markup' => $displayed_string,
    ];

    // If we can generate a link, then update the render array.
    $bundle_id = $this->getBundleFromCvTerm($item_settings['termIdSpace'], $item_settings['termAccession']);
    if ($bundle_id) {
      $base_table = $this->getTableFromCvTerm($item_settings['termIdSpace'], $item_settings['termAccession']);
      if ($base_table) {
        $entity_id = $this->getEntityIdFromRecordId($base_table, $record_id, $bundle_id, 'tripal_entity');
        if ($entity_id) {
          $url_object = Url::fromRoute('entity.tripal_entity.canonical', ['tripal_entity' => $entity_id]);
          $renderable_item = [
            '#type' => 'link',
            '#url' => $url_object,
            '#title' => $displayed_string,
          ];
        }
      }
    }
    return $renderable_item;
  }

  /**
   * Retrieve the base chado table for a given bundle given the bundle's CV term
   *
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. "NCIT"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. "C47954"
   *
   * @return string
   *   The chado table name, or null if no match found.
   */
  public function getTableFromCvTerm($termIdSpace, $termAccession) {
    $chado_table = NULL;
    $entity_type = 'tripal_entity';
    $bundle = $this->getBundleFromCvTerm($termIdSpace, $termAccession);
    if ($bundle) {
      $entityFieldManager = \Drupal::service('entity_field.manager');
      $fields = $entityFieldManager->getFieldDefinitions($entity_type, $bundle);
      $field_list = array_keys($fields);
      $type_name = NULL;
      $base_table = NULL;
      foreach ($field_list as $field_name) {
        // Skip drupal fields. Look for the first tripal field that has a base_column
        // set. Fields from linker tables do not have a base_column.
        if (preg_match('/^'.$bundle.'/', $field_name)) {
          $field_storage = FieldStorageConfig::loadByName('tripal_entity', $field_name);
          if ($field_storage) {
            $storage_plugin_settings = $field_storage->getSettings()['storage_plugin_settings'];
            $base_table = $storage_plugin_settings['base_table'];
            $base_column = $storage_plugin_settings['base_column'] ?? '';
            if ($base_table and $base_column) {
              $chado_table = $base_table;
              break;
            }
          }
        }
      }
    }
    return $chado_table;
  }

  /**
   * Retrieve a Tripal bundle id based on its CV term
   *
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. "NCIT"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. "C47954"
   *
   * @return string
   *   The bundle id, or null if no match found.
   */
  public function getBundleFromCvTerm($termIdSpace, $termAccession) {
    $bundle_id = NULL;
    $bundle_manager = \Drupal::service('entity_type.bundle.info');
    $bundle_list = $bundle_manager->getBundleInfo('tripal_entity');
    foreach ($bundle_list as $id => $properties) {
      // Get each bundle's CV term
      $bundle_info = \Drupal::entityTypeManager()->getStorage('tripal_entity_type')->load($id);
      $bundleIdSpace = $bundle_info->getTermIdSpace();
      $bundleAccession = $bundle_info->getTermAccession();
      // If this is the desired bundle, the values will match
      if (($termIdSpace == $bundleIdSpace) and ($termAccession == $bundleAccession)) {
        $bundle_id = $id;
        break;
      }
    }
    return $bundle_id;
  }

  /**
   * Retrieve the pkey for an entity corresponding to a record in a table.
   *
   * @param string $base_table
   *   The name of the chado table
   * @param integer $record_id
   *   The primary key value for the requested record in the $base_table
   * @param string $bundle_id
   *   The name of the drupal bundle, e.g. for base table 'arraydesign' it is 'array_design'
   * @param string $entity_type
   *   The type of entity, only 'tripal_entity' is supported.
   *
   * @return integer
   *   The id for the requested entity in the tripal_entity table.
   *   Will be null if zero or if multiple hits.
   */
  public function getEntityIdFromRecordId($base_table, $record_id, $bundle_id, $entity_type = 'tripal_entity') {

    // Catch invalid entity type
    if ($entity_type != 'tripal_entity') {
      throw new \Exception("Invalid entity type \"$entity_type\". getEntityIdFromRecordId() only supports the entity type \"tripal_entity\"");
    }

    $id = NULL;
    $required_fields = $this->getRequiredFields($bundle_id, $entity_type);
    if (!$required_fields) {
      dpm("Temporary warning that there are no required fields for bundle \"$bundle_id\""); //@@@
      return NULL;
    }

    // We only need to evaluate for one of the required fields,
    // for simplicity just pick the first one.
    $required_field = array_key_first($required_fields);
    $required_base_table = $required_fields[$required_field];

    // Make sure base table matches (it should in all cases)
    if ($base_table != $required_base_table) {
      throw new \Exception("base table for bundle $bundle_id field \"$required_field\" is \"$required_base_table\", this does not match passed table \"$base_table\"");
    }

    // This will be the drupal field table and column to query
    $entity_table_name = 'tripal_entity__' . $required_field;
    $entity_column_name = $required_field . '_record_id';

    try {
      // Query the appropriate drupal field table for this record_id
      $conn = \Drupal::service('database');
      $query = $conn->select($entity_table_name, 'e');
      $query->addField('e', 'entity_id');
      $query->condition('e.' . $entity_column_name, $record_id, '=');

      // There should only be zero or one hit, but this exception is here
      // just in case. For zero hits, we return null.
      $num_hits = $query->countQuery()->execute()->fetchField();
      if ($num_hits > 1) {
        throw new \Exception("TripalEntityLookup: Too many hits ($num_hits) for table $entity_table_name column $entity_column_name record $record_id");
      }
      elseif ($num_hits == 1) {
        $id = $query->execute()->fetchField();
      }
      else {
        dpm("Temporary warning that there were no hits to record_id $record_id in table $entity_table_name column $entity_column_name"); //@@@
      }
    }
    catch (\Exception $e) {
      dpm("Temporary warning that db query was invalid: ".$e->getMessage()); //@@@
      return NULL;
      // throw new \Exception('Invalid database query: ' . $e->getMessage());
    }

    return $id;
  }

  /**
   * Retrieve a list of required fields in a given bundle
   *
   * @param string $bundle_id
   *   The name of the drupal bundle, e.g. 'analysis'
   * @param string $entity_type
   *   The type of entity, only 'tripal_entity' is supported.
   *
   * @return array
   *   A list of required fields.
   *   Key is field name, value is chado base table.
   */
  private function getRequiredFields($bundle_id, $entity_type) {
    $field_list = [];
    $cache_id = 'tripal_required_fields';
    // Get cached value if available
    if ($cache = \Drupal::cache()->get($cache_id)) {
      $field_list = $cache->data;
    }
    else {
      // Get and cache values. Look up every bundle so that
      // we only have to cache once. Takes about 1/10 second.
      $entityFieldManager = \Drupal::service('entity_field.manager');
      $bundles = \Drupal::service('entity_type.bundle.info')->getBundleInfo($entity_type);
      foreach ($bundles as $bundle_name => $bundle_info) {
        $fields = $entityFieldManager->getFieldDefinitions($entity_type, $bundle_name);
        foreach ($fields as $field_name => $field_info) {
          $storage_settings = $field_info->getSetting('storage_plugin_settings');
          $base_table = $storage_settings['base_table'] ?? '';
          $base_column = $storage_settings['base_table_dependant']['base_column']
              ?? $storage_settings['base_column'] ?? '';
          $is_required = $field_info->isRequired();
          if ($is_required and $base_table and $base_column) {
            $field_list[$bundle_name][$field_name] = $base_table;
          }
        }
      }
      // Cache the values, specifying expiration in 1 hour.
      \Drupal::cache()->set($cache_id, $field_list, \Drupal::time()->getRequestTime() + (3600));
    }
    return $field_list[$bundle_id];
  }

}
