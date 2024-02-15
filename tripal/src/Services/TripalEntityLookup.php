<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Link;
use Drupal\Core\Url;
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
   * Used by fields to get a ready-to-use url to link to an entity.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage"
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. "NCIT"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. "C47954"
   * @param integer $record_id
   *   The primary key value for the requested record
   * @param string $displayed_string
   *   The text that will be displayed as a url link
   *
   * @return string
   *   The rendered url, or if no match was found, the original $displayed_string.
   */
  public function getFieldUrl($datastore, $termIdSpace, $termAccession, $record_id, $displayed_string) {
    $bundle = $this->getBundleFromCvTerm($termIdSpace, $termAccession);
    if ($bundle) {
      $uri = $this->getEntityURI($datastore, $bundle, $record_id);
      if ($uri) {
        $displayed_string = Link::fromTextAndUrl($displayed_string, Url::fromUri($uri))->toString();
      }
    }
    return $displayed_string;
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
// TEMPORARY will be removed once issue #1783 is resolved @@@
if ($termAccession == 'C47954') {
  $termIdSpace = 'local'; $termAccession = 'contact';
}
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
   * Retrieve a uri for an entity corresponding to a record in a table.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage"
   * @param string $bundle
   *   The id of the entity type (bundle)
   * @param integer $record_id
   *   The primary key value for the requested record
   *
   * @return string
   *   The local uri string for the requested entity.
   *   Will be null if either zero or multiple hits.
   */
  public function getEntityURI($datastore, $bundle, $record_id) {
    $uri = NULL;
    $id = $this->getEntityId($datastore, $bundle, $record_id);
    if ($id) {
      $uri = "internal:/bio_data/$id";
    }

    return $uri;
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
    $title_manager = \Drupal::service('tripal.tripal_entity.title');
    $titles = $title_manager->getEntityTitles($datastore, $bundle, $record_id);

    // This check just prevents errors if record_id happens to be null
    if (count($titles) == 1) {

      // Query the tripal_entity table for a matching title of the same
      // type (i.e. same bundle).
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
