<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\Attribute\FieldFormatter;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalBooleanTypeFormatter;

#[FieldFormatter(
  id: 'chado_boolean_type_formatter',
  label: new TranslatableMarkup('Chado Boolean Type Formatter'),
  description: new TranslatableMarkup('The Chado boolean type formatter.'),
  field_types: [
    'chado_boolean_type_default',
  ],
)]
/**
 * Plugin implementation of default Chado boolean type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_boolean_type_formatter",
 *   label = @Translation("Chado Boolean Type Formatter"),
 *   description = @Translation("The Chado boolean type formatter."),
 *   field_types = {
 *     "chado_boolean_type_default"
 *   }
 * )
 */
class ChadoBooleanFormatterDefault extends DefaultTripalBooleanTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
