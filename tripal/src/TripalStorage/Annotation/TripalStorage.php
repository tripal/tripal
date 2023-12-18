<?php

namespace Drupal\tripal\TripalStorage\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a custom plugin annotation for TripalStorage plugins.
 *
 * Additional keys for tripal storage plugins can be defined in
 * hook_tripalstorage_info_alter().
 *
 * @see Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager
 * @see Drupal\tripal\TripalStorage\Interface\TripalStorageInterface
 *
 * @Annotation
 */
class TripalStorage extends Plugin {

  /**
   * The tripal storage type ID.
   *
   * @var string
   */
  public $id;

  /**
   * The human-readable name of the tripal storage type.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

  /**
   * A short description of the tripal storage type.
   *
   * @var \Drupal\Core\Annotation\Translation
   * @ingroup plugin_translatable
   */
  public $description;

}
