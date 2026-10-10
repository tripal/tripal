<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalRealTypeFormatter;

/**
 * Plugin implementation of default Chado real type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_real_type_formatter',
  label: new TranslatableMarkup('Chado Real Type Formatter'),
  description: new TranslatableMarkup('The Chado real type formatter.'),
  field_types: [
    'chado_real_type_default',
  ],
)]
class ChadoRealFormatterDefault extends DefaultTripalRealTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
