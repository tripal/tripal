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
    parent::__construct($configuration, $plugin_id, $plugin_definition);
  }

  /**
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name) {
    
    if (!is_string($name)) {
      return False;
    } 
    
    $manager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $oldname = $this->getDefaultVocabulary();
    if ($oldname) {
      $vocab = $manager->loadCollection($oldname);
      $vocab->removeIdSpace($this->getName());
    }
    if ($name) {
      $vocab = $manager->loadCollection($name);
      if (!$vocab) {
        return False;
      }
      $vocab->addIdSpace($this->getName());
    }
    return TRUE;
  }

}
