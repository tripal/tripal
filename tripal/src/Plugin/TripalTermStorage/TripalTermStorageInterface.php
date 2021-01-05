<?php

namespace Drupal\tripal\Plugin\TripalTermStorage;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for TripalTerm Storage plugins.
 */
interface TripalTermStorageInterface extends PluginInspectionInterface {

  /**
   * Save Tripal Vocabulary details to an additional storage backend.
   *
   * @param array $values
   *   The property values for the TripalVocab object to be created.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalVocab.
   *
   * @see TripalVocab::preCreate().
   */
  public function preCreateVocab(&$values, EntityStorageInterface $storage);

  /**
   * Save Tripal Vocabulary IDSpace details to an additional storage backend.
   *
   * @param array $values
   *   The property values for the TripalVocabSpace object to be created.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalVocabSpace.
   *
   * @see TripalVocabSpace::preCreate().
   */
  public function preCreateVocabSpace(&$values, EntityStorageInterface $storage);

  /**
   * Save Tripal Term details to an additional storage backend.
   *
   * @param array $values
   *   The property values for the TripalTerm object to be created.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalTerm.
   *
   * @see TripalTerm::preCreate().
   */
  public function preCreateTerm(&$values, EntityStorageInterface $storage);

  /**
   * Add details to a Tripal Vocabulary from an additional storage backend.
   *
   * @param int $id
   *   The unique identifier of the Tripal Vocabulary to be loaded.
   * @param TripalVocab $entity
   *   The entity loaded using TripalVocab::load() ready to be added to.
   *
   * @return TripalVocab
   *   The fully loaded Tripal Vocabulary. This should be the $entity parameter
   *    passed in with storage-specific details added and nothing removed.
   *
   * @see TripalVocab::load()
   */
  public function loadVocab($id, TripalVocab &$entity);

  /**
   * Add details to a Tripal Vocabulary IDSpace from an additional storage backend.
   *
   * @param int $id
   *   The unique identifier of the Tripal Vocabulary IDSpace to be loaded.
   * @param TripalVocabSpace $entity
   *   The entity loaded using TripalVocabSpace::load() ready to be added to.
   *
   * @return TripalVocabSpace
   *   The fully loaded Tripal Vocabulary. This should be the $entity parameter
   *    passed in with storage-specific details added and nothing removed.
   *
   * @see TripalVocabSpace::load()
   */
  public function loadVocabSpace($id, TripalVocabSpace &$entity);

  /**
   * Add details to a Tripal Term from an additional storage backend.
   *
   * @param int $id
   *   The unique identifier of the Tripal Term to be loaded.
   * @param TripalTerm $entity
   *   The entity loaded using TripalTerm::load() ready to be added to.
   *
   * @return TripalTerm
   *   The fully loaded Tripal Term. This should be the $entity parameter
   *    passed in with storage-specific details added and nothing removed.
   *
   * @see TripalTerm::load()
   */
  public function loadTerm($id, TripalTerm &$entity);

  /**
   * Retrieve the plugin ID.
   *
   * @return string
   *   Returns the plugin ID.
   */
  public function getID();

  /**
   * Retrieve the plugin label.
   *
   * @return string
   *   Returns the plugin label.
   */
  public function getLabel();

  /**
   * Retrieve a description of the plugin.
   *
   * @return string
   *   Returns the plugin description.
   */
  public function getDescription();

}
