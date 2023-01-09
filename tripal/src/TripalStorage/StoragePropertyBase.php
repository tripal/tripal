<?php

namespace Drupal\tripal\TripalStorage;

/**
 * Base class for a Tripal storage property type or value.
 */
class StoragePropertyBase {

  /**
   * Constructs a new Tripal storage property base object.
   *
   * @param string entityType
   *   The entity type associated with this storage property base object.
   *
   * @param string fieldType
   *   The field type associated with this storage property base object.
   *
   * @param string key
   *   The key associated with this storage property base object.
   *
   * @param string term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   */
  public function __construct($entityType, $fieldType, $key, $term_id) {
    $this->entityType = $entityType;
    $this->fieldType = $fieldType;

    $matches = [];
    if (preg_match('/^(.+?):(.+)$/', $term_id, $matches)) {
      $this->termIdSpace = $matches[1];
      $this->termAccession = $matches[2];

      $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idspace = $idsmanager->loadCollection($this->termIdSpace);
      if (!$idspace) {
        throw new \Exception('Cannot create a StorageProperty object as IdSpace for the property term is not recognized: ' . $term_id);
      }
      $term = $idspace->getTerm($this->termAccession);
      if (!$term) {
        throw new \Exception('Cannot create a StorageProperty object as accession for the property term is not recognized: ' . $term_id);
      }
    }
    else {
      throw new \Exception('Cannot create a StorageProperty object without a property formatted term: ' . $term_id);
    }

    // Drupal doesn't allow non alphanumeric characters in the key, so
    // remove any.
    $key = preg_replace('/[^\w]/', '_', $key);
    $this->key_ = $key;
  }

  /**
   * Returns the entity type associated with this storage property base
   * object.
   *
   * @return string
   *   The entity type.
   */
  public function getEntityType() {
    return $this->entityType;
  }

  /**
   * Returns the field type associated with this storage property base object.
   *
   * @return string
   *   The field type.
   */
  public function getFieldType() {
    return $this->fieldType;
  }

  /**
   * Returns the key associated with this storage property base object.
   *
   * @return string
   *   The key.
   */
  public function getKey() {
    return $this->key_;
  }

  /**
   * Returns the name of the ID space of the term for this storage property.
   *
   * @return string
   *   The key.
   */
  public function getTermIdSpace() {
    return $this->termIdSpace;
  }

  /**
   * Returns the accession of the term for this storage property.
   *
   * @return string
   *   The key.
   */
  public function getTermAccession() {
    return $this->termAccession;
  }

  /**
   * Gets the CV Term Object associated with this storage property.
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalTerm
   *   The Tripal Controlled Vocabulary Term Object.
   */
  public function getTerm() {
    $manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idspace = $manager->loadCollection($this->termIdSpace);
    return $idspace->getTerm($this->termAccession);
  }

  /**
   * The entity type associated with this storage property base object.
   *
   * @var string
   */
  private $entityType;

  /**
   * The field type associated with this storage property base object.
   *
   * @var string
   */
  private $fieldType;

  /**
   * The field key associated with this storage property base object.
   *
   * @var string
   */
  private $key_;

  /**
   * The Id space name for the CV term that this property is associated with.
   *
   * @var string
   */
  private $termIdSpace;

  /**
   * The accession for the CV term that this property is associated with.
   *
   * @var string
   */
  private $termAccession;
}
