<?php

namespace Drupal\tripal\TripalBackendPublish\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a TripalBackendPublish annotation object.
 *
 * @Annotation
 */
class TripalBackendPublish extends Plugin {


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
   * A brief description for this plugin.
   *
   * This description will be presented to the site user.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $description;

}
