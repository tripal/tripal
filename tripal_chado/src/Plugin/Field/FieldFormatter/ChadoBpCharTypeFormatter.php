<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalBpCharTypeFormatter;
use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado bpchar type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_bpchar_type_formatter",
 *   label = @Translation("Chado BpChar Type Formatter"),
 *   description = @Translation("The Chado bpchar type formatter."),
 *   field_types = {
 *     "chado_bpchar_type"
 *   }
 * )
 */
class ChadoBpCharTypeFormatter extends DefaultTripalBpCharTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
