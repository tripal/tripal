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
    // @todo
    return 'ID';
  }

  /**
   * @{inheritdoc}
   */
  public function getLabel() {
    // @todo
    return 'Label';
  }

  /**
   * @{inheritdoc}
   */
  public function getDescription() {
    // @todo
    return 'Description';
  }
}
