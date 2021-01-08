<?php

namespace Drupal\tripal_chado\Plugin\TripalTermStorage;

use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageBase;

/**
 * TripalTerm Storage plugin: Chado Integration.
 *
 * @ingroup tripal_chado
 *
 * @TripalTermStorage(
 *   id = "chado",
 *   label = @Translation("GMOD Chado Integration"),
 *   description = @Translation("Ensures Tripal Vocabularies are linked with chado cvterms."),
 * )
 */
class TripalTermStorageChado extends TripalTermStorageBase implements TripalTermStorageInterface {

  /**
   * {@inheritdoc}
   */
  public function preSaveVocab(TripalVocab &$entity, EntityStorageInterface $storage) {
    // By default we don't need to do anything.
    // The default entity machinery will save all fields in Drupal storage.
  }

  /**
   * {@inheritdoc}
   */
  public function preSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage) {
    // By default we don't need to do anything.
    // The default entity machinery will save all fields in Drupal storage.
  }

  /**
   * {@inheritdoc}
   */
  public function preSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage) {
    // By default we don't need to do anything.
    // The default entity machinery will save all fields in Drupal storage.
  }

  /**
   * {@inheritdoc}
   */
  public function postSaveVocab(TripalVocab &$entity, EntityStorageInterface $storage, $update) {
    // By default we don't need to do anything.
    // The default entity machinery will save all fields in Drupal storage.
  }

  /**
   * {@inheritdoc}
   */
  public function postSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage, $update) {
    // By default we don't need to do anything.
    // The default entity machinery will save all fields in Drupal storage.
  }

  /**
   * {@inheritdoc}
   */
  public function postSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage, $update) {
    // By default we don't need to do anything.
    // The default entity machinery will save all fields in Drupal storage.
  }

  /**
   * {@inheritdoc}
   */
  public function loadVocab($id, TripalVocab &$entity) {
    // By default we don't need to anything.
    // The default entity machinery will load all fields.
    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function loadVocabSpace($id, TripalVocabSpace &$entity) {
    // By default we don't need to anything.
    // The default entity machinery will load all fields.
    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function loadTerm($id, TripalTerm &$entity) {
    // By default we don't need to anything.
    // The default entity machinery will load all fields.
    return $entity;
  }
}
