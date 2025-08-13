<?php

namespace Drupal\tripal\TripalVocabTerms\Attribute;

use Drupal\Component\Plugin\Attribute\Plugin;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a tripal vocabulartitem attribute object.
 *
 * Plugin Namespace: Drupal\tripal\TripalVocabTerms.
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalVocabulary extends Plugin {

  /**
   * Constructs a Chado Buddy attribute.
   *
   * @param string $id
   *   The plugin ID.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $label
   *   The human-readable name of the plugin.
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
  ) {}

}
