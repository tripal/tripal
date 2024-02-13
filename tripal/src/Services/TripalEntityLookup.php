<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\Services\TripalEntityTitle;


class TripalEntityLookup {

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * Stores the bundle (entity type) object.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType $entity_type
   **/
  protected $entity_type = NULL;

  /**
   * The TripalStorage object.
   *
   * @var \Drupal\tripal\TripalStorage\TripalStorageBase $storage
   **/
  protected $storage = NULL;



  /**
   * Retrieve a url for an entity corresponding to a record in a table.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage"
   * @param string $bundle
   *   The id of the entity type (bundle)
   * @param integer $record_id
   *   The primary key value for the requested record
   *
   * @return string
   *   The local url for the requested entity.
   *   Will be null if zero or if multiple hits.
   */
  public function getEntityURL($datastore, $bundle, $record_id) {
    $id = $this->getEntityId($datastore, $bundle, $record_id);
    $url = NULL;
    if ($id) {
      $url = "internal:/bio_data/$id";
    }
    return $url;
  }

  /**
   * Retrieve the pkey for an entity corresponding to a record in a table.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage"
   * @param string $bundle
   *   The id of the entity type (bundle)
   * @param integer $record_id
   *   The primary key value for the requested record
   *
   * @return integer
   *   The id for the requested entity in the tripal_entity table.
   *   Will be null if zero or if multiple hits.
   */
  public function getEntityId($datastore, $bundle, $record_id) {
    $id = NULL;
    $title_service = \Drupal::service('tripal.entity_title');
    $titles = $title_service->getEntityTitles($datastore, $bundle, $record_id);

    // This check just prevents errors if a developer forgets to pass record_id
    if (count($titles) == 1) {

      // Query the tripal_entity table for a matching title of the same type (i.e. bundle).
      $conn = \Drupal::service('database');
      $query = $conn->select('tripal_entity', 'e');
      $query->addField('e', 'id');
      $query->condition('e.type', $bundle, '=');
      $query->condition('e.title', $titles[0], '=');

      // Because there is no unique constraint, we will have to watch
      // for multiple hits. If this happens, we return null.
      $num_hits = $query->countQuery()->execute()->fetchField();
      if ($num_hits == 1) {
        $id = $query->execute()->fetchField();
      }
    }
    return $id;
  }

  /**
   * Replace tokens in the supplied title with the supplied values.
   * Typically the title will have fewer tokens than are supplied in the values.
   *
   * @param string title
   *   The entity title containing token strings, e.g. '[genus] [species] ([common_name])'
   * @param array values
   *   Key value pairs for substitution, e.g. ['name' => 'Gene One']
   */
  private function replaceTokens($title, $values) {
    foreach ($values as $key => $value) {
      $title = preg_replace('/\[$key\]/', $value, $title);
    }
    return trim($title);
  }

}
