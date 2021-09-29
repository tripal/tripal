<?php

namespace Drupal\tripal4\Plugin;

use Drupal\tripal4\Plugin\CollectionPluginBase;
use Drupal\tripal4\Plugin\IdSpaceInterface

/**
 * Base class for tripal id space plugins.
 */
abstract class IdSpaceBase extends CollectionPluginBase implements IdSpaceInterface {

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);
  }

}
