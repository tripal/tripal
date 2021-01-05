<?php

namespace Drupal\tripal\Plugin\TripalTermStorage;

use Drupal\Component\Plugin\PluginBase;

/**
 * Base class for TripalTerm Storage plugins.
 */
abstract class TripalTermStorageBase extends PluginBase implements TripalTermStorageInterface {

  /**
   * @{inheritdoc}
   */
  public function getID() {
    // Retrieve the @id property from the annotation and return it.
    return $this->pluginDefinition['id'];
  }

  /**
   * @{inheritdoc}
   */
  public function getLabel() {
    // Retrieve the @label property from the annotation and return it.
    return $this->pluginDefinition['label'];
  }

  /**
   * @{inheritdoc}
   */
  public function getDescription() {
    // Retrieve the @description property from the annotation and return it.
    return $this->pluginDefinition['description'];
  }
}
