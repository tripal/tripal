<?php

declare(strict_types=1);

namespace Drupal\tripal\TripalField\Attribute;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a TripalFieldWidget attribute for plugin discovery.
 *
 * This extends the Drupal core FieldWidget attribute.
 * This does not yet add any additional functionality.
 *
 * @ingroup field_widget
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalFieldWidget extends FieldWidget {

  /**
   * Constructs a TripalFieldFormatter attribute.
   *
   * @inheritdoc
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
    public readonly ?TranslatableMarkup $description = NULL,
    public readonly array $field_types = [],
    public readonly bool $multiple_values = FALSE,
    public readonly ?int $weight = NULL,
    public readonly ?string $deriver = NULL,
  ) {}

}
