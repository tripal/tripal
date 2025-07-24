<?php

declare(strict_types=1);

namespace Drupal\tripal\TripalField\Attribute;

use Drupal\Core\Field\Attribute\FieldFormatter;
use Drupal\Component\Plugin\Attribute\Plugin;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a FieldFormatter attribute for plugin discovery.
 *
 * Formatters handle the display of field values. They are typically
 * instantiated and invoked by an EntityDisplay object.
 *
 * Additional attribute keys for formatters can be defined in
 * hook_field_formatter_info_alter().
 *
 * @see \Drupal\Core\Field\FormatterPluginManager
 * @see \Drupal\Core\Field\FormatterInterface
 *
 * @ingroup field_formatter
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalFieldFormatter extends FieldFormatter {

  /**
   * Constructs a FieldFormatter attribute.
   *
   * @param string $id
   *   The plugin ID.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $label
   *   (optional) The human-readable name of the formatter type.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $description
   *   (optional) A short description of the formatter type.
   * @param string[] $field_types
   *   (optional) An array of field types the formatter supports.
   * @param int|null $weight
   *   (optional) An integer to determine the weight of this formatter.
   *   Weight is relative to other formatters in the Field UI when selecting a
   *   formatter for a given field instance.
   * @param class-string|null $deriver
   *   (optional) The deriver class.
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
    public readonly ?TranslatableMarkup $description = NULL,
    public readonly array $field_types = [],
    public readonly ?int $weight = NULL,
    public readonly ?string $deriver = NULL,
    public readonly array $valid_tokens = [],
  ) {
dpm("CP201 construct called");//@@@
    parent::__construct($id, $label, $description, $field_types, $weight, $deriver);
  }

}
