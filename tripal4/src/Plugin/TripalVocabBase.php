<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginBase;
use Drupal\tripal4\Plugin\TripalVocabInterface

/**
 * Base class for tripal vocabulary plugins.
 */
abstract class TripalVocabBase extends PluginBase implements TripalVocabInterface {

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);
  }

}
