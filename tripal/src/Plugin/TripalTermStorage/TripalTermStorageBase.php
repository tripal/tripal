<?php

namespace Drupal\tripal\Plugin\TripalTermStorage;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\tripal\Entity\TripalVocabSpace;
use Drupal\tripal\Entity\TripalTerm;

use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageBase;
use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageInterface;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Component\Plugin\PluginBase;

/**
 * Base class for TripalTerm Storage plugins.
 */
abstract class TripalTermStorageBase extends PluginBase implements TripalTermStorageInterface {

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

  /**
   * {@inheritdoc}
   */
  public function getID() {
    // Retrieve the @id property from the annotation and return it.
    return $this->pluginDefinition['id'];
  }

  /**
   * {@inheritdoc}
   */
  public function getLabel() {
    // Retrieve the @label property from the annotation and return it.
    return $this->pluginDefinition['label'];
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    // Retrieve the @description property from the annotation and return it.
    return $this->pluginDefinition['description'];
  }
}
