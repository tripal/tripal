<?php

namespace Drupal\tripal\TripalStorage\Attribute;

use Drupal\Component\Plugin\Attribute\Plugin;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a custom plugin annotation for TripalStorage plugins.
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalStorage extends Plugin {

  /**
   * Constructs a TripalFieldFormatter attribute.
   *
   * @param string $id
   *   The plugin ID.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $label
   *   The human-readable name of the tripal storage type.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $description
   *   A short description of the tripal storage type.
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
    public readonly ?TranslatableMarkup $description = NULL,
  ) {}

}
