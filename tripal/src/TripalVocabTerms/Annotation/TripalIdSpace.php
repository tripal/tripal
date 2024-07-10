<?php

namespace Drupal\tripal\TripalVocabTerms\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a tripal id space item annotation object.
 *
 * @see \Drupal\products\Plugin\ImporterManager
 *
 * @Annotation
 */
class TripalIdSpace extends Plugin {

  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The label of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

}
