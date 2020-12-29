<?php

namespace Drupal\tripal\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a TripalTerm Storage item annotation object.
 *
 * @see \Drupal\tripal\Plugin\TripalTermStorageManager
 * @see plugin_api
 *
 * @Annotation
 */
class TripalTermStorage extends Plugin {

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

  /**
   * The description of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $description;

}
