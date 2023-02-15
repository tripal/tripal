<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalVarCharTypeFormatter;
use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado varchar type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_varchar_type_formatter",
 *   label = @Translation("Chado VarChar Type Formatter"),
 *   description = @Translation("The Chado varchar type formatter."),
 *   field_types = {
 *     "chado_varchar_type"
 *   }
 * )
 */
class ChadoVarCharTypeFormatter extends DefaultTripalVarCharTypeFormatter {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    return parent::viewElements($items, $langcode);
  }
}
