<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalStringTypeFormatter;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * Plugin implementation of default Chado string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_string_type_formatter",
 *   label = @Translation("Chado String Type Formatter"),
 *   description = @Translation("The Chado string type formatter."),
 *   field_types = {
 *     "chado_string_type_default"
 *   }
 * )
 */
class ChadoStringFormatterDefault extends DefaultTripalStringTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
