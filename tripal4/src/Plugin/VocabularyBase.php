<?php

namespace Drupal\tripal4\Plugin;

use Drupal\tripal4\Plugin\CollectionPluginBase;
use Drupal\tripal4\Plugin\VocabularyInterface

/**
 * Base class for tripal vocabulary plugins.
 */
abstract class VocabularyBase extends CollectionPluginBase implements VocabularyInterface {

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
