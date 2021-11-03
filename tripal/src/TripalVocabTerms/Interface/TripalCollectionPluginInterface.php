<?php

namespace Drupal\tripal\TripalVocabTerms\Interface;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal collection plugins.
 */
interface CollectionPluginInterface extends PluginInspectionInterface {

  /**
   * Creates this collection. This must only be called once on this new
   * collection instance that has just been created by its collection plugin
   * manager.
   */
  public function create();

  /**
   * Destroys this collection. This must only be called once when on this
   * existing collection that is being removed from its collection plugin
   * manager.
   */
  public function destroy();

}
