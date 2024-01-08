<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal linker property formatter.
 *
 * @FieldFormatter(
 *   id = "chado_linker_property_formatter_default",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   field_types = {
 *     "chado_linker_property_type_default"
 *   }
 * )
 */
class ChadoLinkerPropertyFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $list = [];
    foreach($items as $delta => $item) {
      $value = $item->get('value')->getString();
      // any URLs are made into clickable links
      if (preg_match('/^https?:/i', $value) ) {
        $value = Link::fromTextAndUrl($value, $value);
      }
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
