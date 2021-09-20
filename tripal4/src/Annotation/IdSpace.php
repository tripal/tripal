<?php

namespace Drupal\tripal4\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a tripal id space item annotation object.
 *
 * @see \Drupal\products\Plugin\ImporterManager
 *
 * @Annotation
 */
class IdSpace extends Plugin {

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
