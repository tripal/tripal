<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal arraydesign formatter.
 *
 * @FieldFormatter(
 *   id = "chado_arraydesign_formatter_default",
 *   label = @Translation("Chado arraydesign formatter"),
 *   description = @Translation("A chado arraydesign formatter"),
 *   field_types = {
 *     "chado_arraydesign_default"
 *   }
 * )
 */
class ChadoArrayDesignFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('arraydesign_name')->getString()
      ];
    }

    return $elements;
  }
}
