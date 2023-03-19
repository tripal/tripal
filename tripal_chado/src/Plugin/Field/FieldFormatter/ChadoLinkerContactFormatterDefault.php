<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_linker_contact_formatter_default",
 *   label = @Translation("Chado Contact"),
 *   description = @Translation("Add a linked Chado contact to the content type."),
 *   field_types = {
 *     "chado_linker_contact_default"
 *   }
 * )
 */
class ChadoLinkerContactFormatterDefault extends ChadoFormatterBase {

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

    // Only return markup if the field is not empty.
    if (!empty($list)) {

      // If only one element has been found, don't make into a list.
      if (count($list) == 1) {
        $elements[0] = [
          '#type' => 'markup',
          "#markup" => $list[0]
        ];
      }

      // If more than one value has been found, display all values in an
      // unordered list.
// to-do: add a pager
      elseif (count($list) > 1) {
        $elements[0] = [
          '#theme' => 'item_list',
          '#list_type' => 'ul',
          '#items' => $list,
          '#wrapper_attributes' => ['class' => 'container'],
        ];
      }
    }

    return $elements;
  }
}
