<?php

namespace Drupal\tripal_chado\Plugin\TripalTermStorage;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\tripal\Entity\TripalVocabSpace;
use Drupal\tripal\Entity\TripalTerm;

use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageBase;
use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageInterface;
use Drupal\Core\Entity\EntityStorageInterface;

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

  }

  /**
   * {@inheritdoc}
   */
  public function preSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage) {

  }

  /**
   * {@inheritdoc}
   */
  public function preSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage) {

  }

  /**
   * {@inheritdoc}
   */
  public function postSaveVocab(TripalVocab &$entity, EntityStorageInterface $storage, $update) {

  }

  /**
   * {@inheritdoc}
   */
  public function postSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage, $update) {

  }

  /**
   * {@inheritdoc}
   */
  public function postSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage, $update) {

  }

  /**
   * {@inheritdoc}
   */
  public function loadVocab($id, TripalVocab &$entity) {
    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function loadVocabSpace($id, TripalVocabSpace &$entity) {
    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function loadTerm($id, TripalTerm &$entity) {
    return $entity;
  }
}
