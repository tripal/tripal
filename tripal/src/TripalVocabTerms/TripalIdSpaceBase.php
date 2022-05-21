<?php

namespace Drupal\tripal\TripalVocabTerms;

use Drupal\tripal\TripalVocabTerms\TripalCollectionPluginBase;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;

/**
 * Base class for tripal id space plugins.
 */
abstract class TripalIdSpaceBase extends TripalCollectionPluginBase implements TripalIdSpaceInterface {

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);
  }

  /**
   * Removes this id space from its previous default vocabulary if one is set
   * and then adds this id space to its new default vocabulary if the given name
   * is not NULL. It is still the responsibility of an implementation to
   * actually save changes to its default vocabulary.
   *
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name) {
    $manager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $oldname = $this->getDefaultVocabulary();
    if ($oldname) {
      $vocab = $manager->getCollection($oldname);
      $vocab->removeIdSpace($this->getName());
    }
    if ($name) {
      $vocab = $manager->getCollection($name);
      $vocab->addIdSpace($this->getName());
    }
  }

}
