<?php

namespace Drupal\tripal\TripalVocabTerms;

use Drupal\tripal\TripalVocabTerms\TripalCollectionPluginBase;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalVocabularyInterface;

/**
 * Base class for tripal vocabulary plugins.
 */
abstract class TripalVocabularyBase extends TripalCollectionPluginBase implements TripalVocabularyInterface {

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);
  }

  /**
   * {@inheritdoc}
   */
  public function getTerms($name,$exact = True) {
    // TODO
  }

}
