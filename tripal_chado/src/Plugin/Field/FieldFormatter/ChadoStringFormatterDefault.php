<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalStringTypeFormatter;

/**
 * Plugin implementation of default Chado string type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_string_type_formatter',
  label: new TranslatableMarkup('Chado String Type Formatter'),
  description: new TranslatableMarkup('The Chado string type formatter.'),
  field_types: [
    'chado_string_type_default',
  ],
)]
class ChadoStringFormatterDefault extends DefaultTripalStringTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }

}
