<?php

namespace Drupal\tripal\TripalStorage;

/**
 * Defines a TripalStorage annotation object.
 *
 * Additional keys for tripal storage plugins can be defined in
 * hook_tripalstorage_info_alter().
 *
 * @see Drupal\tripal\Services\TripalStorageManager
 * @see Drupal\tripal\TripalStorage\TripalStorageInterface
 * @see https://www.drupal.org/node/3044251
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

  /**
   * The name of the tripal storage class.
   *
   * This is not provided manually, it will be added by the discovery mechanism.
   *
   * @var string
   */
  public $class;

}
