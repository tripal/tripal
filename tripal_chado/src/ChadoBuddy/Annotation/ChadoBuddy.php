<?php

namespace Drupal\tripal_chado\ChadoBuddy\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines chado_buddy annotation object.
 *
 * @Annotation
 */
final class ChadoBuddy extends Plugin {

  /**
   * The plugin ID.
   */
  public readonly string $id;

  /**
   * The human-readable name of the plugin.
   *
   * @ingroup plugin_translatable
   */
  public readonly string $title;

  /**
   * The description of the plugin.
   *
   * @ingroup plugin_translatable
   */
  public readonly string $description;

}
