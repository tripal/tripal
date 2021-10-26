<?php

namespace Drupal\tripal4\Plugin;

use Drupal\tripal4\Base\CollectionPluginBase;
use Drupal\tripal4\Interface\IdSpaceInterface

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

  /**
   * Removes this id space from its previous default vocabulary if one is set
   * and then adds this id space to its new default vocabulary if the given name
   * is not NULL. It is still the responsibility of an implementation to
   * actually save changes to its default vocabulary.
   *
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name) {
    // TODO
  }

}
