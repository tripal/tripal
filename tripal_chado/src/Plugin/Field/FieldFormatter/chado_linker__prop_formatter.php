<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_linker__prop_formatter",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   field_types = {
 *     "chado_linker__prop"
 *   }
 * )
 */
class chado_linker__prop_formatter extends TripalFormatterBase {

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
      return;
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
    }
    else {
      $elements[0] = [
        '#type' => 'markup',
        "#markup" => $list[0]
      ];
    }

    return $elements;
  }
}
