<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalIntegerTypeFormatter;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * Plugin implementation of default Chado integer type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_integer_type_formatter",
 *   label = @Translation("Chado Integer Type Formatter"),
 *   description = @Translation("The Chado integer type formatter."),
 *   field_types = {
 *     "chado_integer_type_default"
 *   }
 * )
 */
class ChadoIntegerFormatterDefault extends DefaultTripalIntegerTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
