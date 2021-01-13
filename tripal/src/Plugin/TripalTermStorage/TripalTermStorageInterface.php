<?php

namespace Drupal\tripal\Plugin\TripalTermStorage;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\tripal\Entity\TripalVocabSpace;
use Drupal\tripal\Entity\TripalTerm;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for TripalTerm Storage plugins.
 */
interface TripalTermStorageInterface extends PluginInspectionInterface {

  /**
   * Save Tripal Vocabulary details to an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value. Check for update
   * based on whether the 'id' field is set.
   *
   * @param TripalVocab $entity
   *   The TripalVocab entity populated with new values. If this is on insert,
   *   the id will not yet be set.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalVocab.
   *
   * @see TripalVocab::preSave().
   */
  public function preSaveVocab(TripalVocab &$entity, EntityStorageInterface $storage);

  /**
   * Save Tripal Vocabulary IDSpace details to an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value. Check for update
   * based on whether the 'id' field is set.
   *
   * @param TripalVocabSpace $entity
   *   The TripalVocabSpace entity populated with new values. If this is on insert,
   *   the id will not yet be set.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalVocabSpace.
   *
   * @see TripalVocabSpace::preSave().
   */
  public function preSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage);

  /**
   * Save Tripal Term details to an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value. Check for update
   * based on whether the 'id' field is set.
   *
   * @param TripalTerm $entity
   *   The TripalTerm entity populated with new values. If this is on insert,
   *   the id will not yet be set.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalTerm.
   *
   * @see TripalTerm::preSave().
   */
  public function preSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage);

  /**
   * Save Tripal Vocabulary details to an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value.
   *
   * @param TripalVocab $entity
   *   The TripalVocab entity populated with new values.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalVocab.
   * @param bool $update
   *   Indicates whether the entity is being updated or created.
   *
   * @see TripalVocab::postSave().
   */
  public function postSaveVocab(TripalVocab &$entity, EntityStorageInterface $storage, $update);

  /**
   * Save Tripal Vocabulary IDSpace details to an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value.
   *
   * @param TripalVocabSpace $entity
   *   The TripalVocabSpace entity populated with new values.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalVocabSpace.
   * @param bool $update
   *   Indicates whether the entity is being updated or created.
   *
   * @see TripalVocabSpace::postSave().
   */
  public function postSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage, $update);

  /**
   * Save Tripal Term details to an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value.
   *
   * @param TripalTerm $entity
   *   The TripalTerm entity populated with new values.
   * @param EntityStorageInterface $storage
   *   The storage object used to create the TripalTerm.
   * @param bool $update
   *   Indicates whether the entity is being updated or created.
   *
   * @see TripalTerm::postSave().
   */
  public function postSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage, $update);

  /**
   * Delete details for a Tripal Vocabulary from an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value.
   *
   * @param TripalVocab $entity
   *   The entity which is being deleted.
   *
   * @see TripalVocab::delete().
   */
  public function deleteVocab(TripalVocab $entity);

  /**
   * Delete details for a Tripal Vocab IDSpace from an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value.
   *
   * @param TripalVocabSpace $entity
   *   The entity which is being deleted.
   *
   * @see TripalVocabSpace::delete().
   */
  public function deleteVocabSpace(TripalVocabSpace $entity);

  /**
   * Delete details for a Tripal Term from an additional storage backend.
   *
   * To retrieve values use $entity->get('fieldname')->value.
   *
   * @param TripalTerm $entity
   *   The entity which is being deleted.
   *
   * @see TripalTerm::delete().
   */
  public function deleteTerm(TripalTerm $entity);

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
