<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalBooleanTypeFormatter;

/**
 * Plugin implementation of default Chado boolean type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_boolean_type_formatter',
  label: new TranslatableMarkup('Chado Boolean Type Formatter'),
  description: new TranslatableMarkup('The Chado boolean type formatter.'),
  field_types: [
    'chado_boolean_type_default',
  ],
)]
class ChadoBooleanFormatterDefault extends DefaultTripalBooleanTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
