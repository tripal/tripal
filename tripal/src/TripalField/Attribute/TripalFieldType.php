<?php

declare(strict_types=1);

namespace Drupal\tripal\TripalField\Attribute;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a TripalFieldType attribute for plugin discovery.
 *
 * This extends the Drupal core FieldType.
 * This does not yet add any additional functionality.
 *
 * @ingroup field_types
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalFieldType extends FieldType {

  /**
   * Constructs a TripalFieldType attribute.
   *
   * @inheritdoc
   */
 public function __construct(
    public readonly string $id,
    public readonly TranslatableMarkup $label,
    public readonly TranslatableMarkup|array|null $description = NULL,
    public readonly string $category = '',
    public readonly int $weight = 0,
    public readonly ?string $default_widget = NULL,
    public readonly ?string $default_formatter = NULL,
    public readonly bool $no_ui = FALSE,
    public readonly ?string $list_class = NULL,
    public readonly ?int $cardinality = NULL,
    public readonly array $constraints = [],
    public readonly array $config_dependencies = [],
    public readonly array $column_groups = [],
    public readonly array $serialized_property_names = [],
    public readonly ?string $deriver = NULL,
    public readonly ?string $module = NULL,
  ) { }

}
