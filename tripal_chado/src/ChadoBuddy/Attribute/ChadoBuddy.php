<?php

namespace Drupal\tripal_chado\ChadoBuddy\Attribute;

use Drupal\Component\Plugin\Attribute\Plugin;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a Chado Buddy attribute object.
 *
 * Plugin Namespace: Drupal\tripal_chado\ChadoBuddy
 *
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class ChadoBuddy extends Plugin {

  /**
   * Constructs a Chado Buddy attribute.
   *
   * @param string $id
   *   The plugin ID.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $label
   *   The human-readable name of the plugin.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $description
   *   The description of the plugin.
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
    public readonly ?TranslatableMarkup $description = NULL,
  ) {}

}
