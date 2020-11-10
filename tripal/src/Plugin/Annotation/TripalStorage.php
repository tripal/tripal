<?php

namespace Drupal\tripal\Plugin\Annotation;

use Drupal\Component\Annotation\AnnotationBase;

/**
 * Defines a custom plugin annotation for TripalStorage plugins.
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
class TripalStorage extends AnnotationBase {

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
   * {@inheritdoc}
   */
  public function get() {
    return array(
      'id' => $this->id,
      'label' => $this->label,
      'description' => $this->description,
			'class' => $this->class
    );
  }
}
