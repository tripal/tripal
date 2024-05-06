<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data
 *
 * @FieldFormatter(
 *   id = "chado_synonym_formatter_default",
 *   label = @Translation("Chado Synonym Formatter"),
 *   description = @Translation("A chado synonym formatter"),
 *   field_types = {
 *     "chado_synonym_type_default"
 *   }
 * )
 */
class ChadoSynonymFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $list = [];
    foreach($items as $delta => $item) {
      $value = $item->get('name')->getString();
      $list[$delta] = $value;
    }

    // Also need to make sure to not return markup if the field is empty.
    if (empty($list)) {
      return $elements;
    }

    // If more than one value has been found display all values in an unordered
    // list.
    if (count($list) > 1) {
      $elements[0] = [
        '#theme' => 'item_list',
        '#list_type' => 'ul',
        '#items' => $list,
        '#wrapper_attributes' => ['class' => 'container'],
      ];
      return $elements;
    }

    $elements[0] = [
      "#markup" => $list[0]
    ];
    return $elements;
  }
}
