<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalDatetimeTypeFormatter;

/**
 * Plugin implementation of the default Chado datetime type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_datetime_type_formatter',
  label: new TranslatableMarkup('Chado Datetime Type Formatter'),
  description: new TranslatableMarkup('The default datetime type formatter for Chado.'),
  field_types: [
    'chado_datetime_type_default',
  ],
)]
class ChadoDatetimeFormatterDefault extends DefaultTripalDatetimeTypeFormatter {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }

}
