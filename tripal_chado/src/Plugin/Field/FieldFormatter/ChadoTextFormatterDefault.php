<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalTextTypeFormatter;

/**
 * Plugin implementation of default Chado text type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_text_type_formatter',
  label: new TranslatableMarkup('Chado Text Type Formatter'),
  description: new TranslatableMarkup('The Chado text type formatter.'),
  field_types: [
    'chado_text_type_default',
  ],
)]
class ChadoTextFormatterDefault extends DefaultTripalTextTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
