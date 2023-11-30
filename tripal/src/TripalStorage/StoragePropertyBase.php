<?php

namespace Drupal\tripal\TripalStorage;


/**
 * Base class for a Tripal storage property type or value.
 */
class StoragePropertyBase {

  /**
   * The ID Space Plugin manager service.
   * Note: Used to confirm the term passed in exists and to retrieve it.
   *
   * @var Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager
   */
  protected $idSpaceService;

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

    // Ideally we would use dependency injection to retrieve the idspace service
    // but that is not available in a custom class like this one. Alternatively,
    // we could pass the service into the constructor but as this class is inherited
    // by many other classes and the constructor is used directly in fields,
    // that would be a lot of work.
    // Our approach: Manage the drupal container ourselves in this constructor.
    // Try to get the current container... It should exist within a bootstrapped
    // site (test or otherwise) and kernel tests.
    // If it doesn't an exception is thrown here.
    $container = \Drupal::getContainer();
    $this->idSpaceService = $container->get('tripal.collection_plugin_manager.idspace');

    // Ensure we have required values.
    if (!$entityType) {
      throw new \Exception('Cannot create a StorageProperty object without specifying the entity type.');
    }
    if (!$fieldType) {
      throw new \Exception('Cannot create a StorageProperty object for entity type "' . $entityType
          . '" without specifying the field that is using it.');
    }
    if (!$key) {
      throw new \Exception('Cannot create a StorageProperty object for entity type "' . $entityType
          . '", field type "' . $fieldType . '" without a key.');
    }

    $matches = [];
    if (preg_match('/^(.+?):(.+)$/', $term_id, $matches)) {
      $this->termIdSpace = $matches[1];
      $this->termAccession = $matches[2];

      $idspace = $this->idSpaceService->loadCollection($this->termIdSpace);
      if (!$idspace) {
        throw new \Exception('Cannot create a StorageProperty object for entity type "' . $entityType
            . '", field type "' . $fieldType . '", key "' . $key
            . '" as IdSpace for the property term is not recognized: "' . $term_id . '"');
      }
      $term = $idspace->getTerm($this->termAccession);
      if (!$term) {
        throw new \Exception('Cannot create a StorageProperty object for entity type "' . $entityType
            . '" field type "' . $fieldType . '", key "' . $key
            . '" as accession for the property term is not recognized: "' . $term_id . '"');
      }
    }
    elseif ($term_id) {
      throw new \Exception('Cannot create a StorageProperty object for entity type "' . $entityType
          . '", field type "' . $fieldType . '", key "' . $key
          . '" without a properly formatted term: "' . $term_id . '"');
    }
    else {
      throw new \Exception('Cannot create a StorageProperty object for entity type "' . $entityType
          . '", field type "' . $fieldType . '", key "' . $key
          . '" as no term was provided');
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
    $idspace = $this->idSpaceService->loadCollection($this->termIdSpace);
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
