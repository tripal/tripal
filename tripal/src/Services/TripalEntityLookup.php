<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Url;
use Drupal\Core\Render\Markup;
use Drupal\field\Entity\FieldStorageConfig;
use \Drupal\tripal\Services\TripalEntityTitle;

class TripalEntityLookup {

  /**
   * This is used by field formatters to get a ready-to-use render
   * array item to link to an entity.
   *
   * @param string $displayed_string
   *   The text that will be displayed as a url link.
   * @param int $entity_id
   *   The primary key value for the Drupal entity. We store -1 in
   *   the Drupal field tables to indicate when there is no Drupal
   *   entity because the TripalEntity class won't save a zero when
   *   drupal_store is TRUE.
   *
   * @return array
   *   If a link is possible, then appropriate render array values to generate the link.
   *   If no link is possible, then appropriate render array values for simple markup.
   */
  public function getRenderableItem($displayed_string, $entity_id) {

    // If an entity_id is provided, then provide a linking render array item.
    if ($entity_id and ($entity_id > 0)) {
      $url_object = Url::fromRoute('entity.tripal_entity.canonical', ['tripal_entity' => $entity_id]);
      $renderable_item = [
        '#type' => 'link',
        '#url' => $url_object,
        '#title' => Markup::create($displayed_string),
      ];
    }
    else {
      // If there is no entity_id, the render array item will just display the passed string.
      $renderable_item = [
        '#markup' => $displayed_string,
      ];
    }

    return $renderable_item;
  }

  /**
   * Retrieve a Drupal entity ID for a record in a given bundle given the bundle's CV term.
   *
   * @param int $record_id
   *   The primary key value for the requested record
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. for gene "SO"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. for gene "0000704"
   * @param string $entity_type
   *   The type of entity, only 'tripal_entity' is supported.
   *
   * @return int|null
   *   The Drupal entity ID, or null if no match found.
   */
  public function getEntityId($record_id, $termIdSpace, $termAccession, $entity_type = 'tripal_entity') {

    // Catch invalid entity type
    if ($entity_type != 'tripal_entity') {
      throw new \Exception("Invalid entity type \"$entity_type\". getEntityId() only supports the entity type \"tripal_entity\"");
    }

    // Perform the lookup steps
    $entity_id = NULL;
    $bundle_id = $this->getBundleFromCvTerm($termIdSpace, $termAccession);
    if ($bundle_id) {
      $base_table = $this->getBundleBaseTable($bundle_id, $entity_type);
      if ($base_table) {
        $entity_ids = $this->getEntityIdFromRecordId($base_table, $record_id, $bundle_id, $entity_type);
        if ($entity_ids) {
          // Here we are just returning the first hit, e.g. analysis published as both
          // analysis and genome assembly. Ideally this will be prevented from happening.
          $entity_id = $entity_ids[0];
        }
      }
    }

    return $entity_id;
  }

  /**
   * Retrieve a Tripal bundle id based on its CV term.
   *
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. "NCIT"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. "C47954"
   *
   * @return string|null
   *   The bundle id, or null if no match found.
   */
  protected function getBundleFromCvTerm($termIdSpace, $termAccession) {
    $bundle_id = NULL;
    $bundles = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['termIdSpace' => $termIdSpace, 'termAccession' => $termAccession]);
    if (sizeof($bundles) == 1) {
      $bundle_id = key($bundles);
    }
    return $bundle_id;
  }

  /**
   * Retrieve the base table for a given bundle.
   *
   * @param string $bundle
   *   The bundle's ID, e.g. "gene"
   * @param string $entity_type
   *   The type of entity, only 'tripal_entity' is supported.
   *
   * @return string|null
   *   The base table name, or null if no match found.
   */
  protected function getBundleBaseTable($bundle, $entity_type) {
    $table = NULL;
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
          $field_storage = FieldStorageConfig::loadByName($entity_type, $field_name);
          if ($field_storage) {
            $storage_plugin_settings = $field_storage->getSettings()['storage_plugin_settings'];
            $base_table = $storage_plugin_settings['base_table'];
            $base_column = $storage_plugin_settings['base_column'] ?? '';
            if ($base_table and $base_column) {
              $table = $base_table;
              break;
            }
          }
        }
      }
    }
    return $table;
  }

  /**
   * Retrieve the pkey for an entity corresponding to a given record in a given table.
   *
   * @param string $base_table
   *   The name of the chado table
   * @param int $record_id
   *   The primary key value for the requested record in the $base_table
   * @param string $bundle_id
   *   The name of the drupal bundle, e.g. for base table 'arraydesign' it is 'array_design'
   * @param string $entity_type
   *   The type of entity, only 'tripal_entity' is supported.
   *
   * @return array
   *   The id for the requested Drupal entity in the tripal_entity table.
   *   Will be an empty array if there was no corresponding entity. This can happen
   *   if the content is not published, or if this is not a content type.
   *   If a given record was published as more than one content type, the
   *   returned array may have more than one entity id. This would happen
   *   in Tripal 3, for example if an analysis is published as both
   *   "analysis", and as "Genome Assembly".
   */
  protected function getEntityIdFromRecordId($base_table, $record_id, $bundle_id, $entity_type) : array {

    $ids = [];
    $required_fields = $this->getRequiredFields($bundle_id, $entity_type);
    if (!$required_fields) {
      // Everything is based on the assumption that there is at least one
      // required field for every content type. If not, we cannot create a link.
      return $ids;
    }

    // We only need to evaluate for one of the required fields,
    // for simplicity just pick the first one.
    $required_field = array_key_first($required_fields);
    $required_base_table = $required_fields[$required_field];

    // Make sure base table matches. It should in all cases, but check just in case.
    if ($base_table != $required_base_table) {
      throw new \Exception("base table for bundle $bundle_id field \"$required_field\" is"
         . " \"$required_base_table\", this does not match passed table \"$base_table\"");
    }

    // These will be the drupal field table name and column name to query.
    $entity_table_name = 'tripal_entity__' . $required_field;
    $entity_column_name = $required_field . '_record_id';

    // Query the appropriate drupal field table for this record_id
    // Ideally there should only be zero or one hit, but in Tripal 3 you
    // could for example publish an analysis as both analysis and Genome Assembly.
    // This function will return all matching entities.
    try {
      $conn = \Drupal::service('database');
      $sql = "SELECT entity_id FROM $entity_table_name WHERE $entity_column_name = :record_id";
      $args = [':record_id' => $record_id];
      $results = $conn->query($sql, $args);
      while ($result = $results->fetchField()) {
        $ids[] = $result;
      }
    }
    catch (\Exception $e) {
      throw new \Exception('Invalid database query: ' . $e->getMessage());
    }

    return $ids;
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
   *   Key is field name, value is base table.
   */
  protected function getRequiredFields($bundle_id, $entity_type) {
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

