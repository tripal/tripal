<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Plugin\Field\FieldType\TripalMarkupTypeItem;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\TripalField\TripalFormatterBase;

/**
 * Plugin implementation of the 'tripal_markup_formatter' field formatter.
 */
#[TripalFieldFormatter(
  id: 'tripal_markup_formatter',
  label: new TranslatableMarkup('Tripal Markup Page Display'),
  description: new TranslatableMarkup('Displays the static markup provided in the field settings on the page.'),
  field_types: [
    'tripal_markup',
  ],
)]
class TripalMarkupFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();

    return $settings;
  }

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {

    // The value is set int the field settings, so we need to get it from the
    // field definition, not the field item.
    $value = TripalMarkupTypeItem::getMarkupValue($items->getEntity(), $items->getFieldDefinition());

    $element = [
      '#type' => 'processed_text',
      '#text' => $value['value'] ?? '',
      '#format' => $value['format'] ?? '',
    ];

    return $element;
  }

}
