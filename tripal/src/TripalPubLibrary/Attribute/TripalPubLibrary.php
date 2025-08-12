<?php

namespace Drupal\tripal\TripalPubLibrary\Attribute;

use Drupal\Component\Plugin\Attribute\Plugin;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a tripal publication importer attribute object.
 *
 * @Annotation
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalPubLibrary extends Plugin {

  /**
   * Constructs a TripalPubLibrary attribute.
   *
   * @param string $id
   *   The plugin ID.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $label
   *   The label of the plugin.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $description
   *   A brief description for this parser.
   *   This description will be presented to the site user.
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
    public readonly ?TranslatableMarkup $description = NULL,
  ) {}

}
