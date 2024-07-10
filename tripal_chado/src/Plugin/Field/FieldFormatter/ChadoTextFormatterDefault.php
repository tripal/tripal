<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalTextTypeFormatter;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * Plugin implementation of default Chado text type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_text_type_formatter",
 *   label = @Translation("Chado Text Type Formatter"),
 *   description = @Translation("The Chado text type formatter."),
 *   field_types = {
 *     "chado_text_type_default"
 *   }
 * )
 */
class ChadoTextFormatterDefault extends DefaultTripalTextTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
