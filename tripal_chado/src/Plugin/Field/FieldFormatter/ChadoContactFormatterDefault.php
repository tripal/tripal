<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal contact formatter.
 *
 * @FieldFormatter(
 *   id = "chado_contact_formatter_default",
 *   label = @Translation("Chado contact formatter"),
 *   description = @Translation("A chado contact formatter."),
 *   field_types = {
 *     "chado_contact_default"
 *   }
 * )
 */
class ChadoContactFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $display_contact_type = $items->getSetting('display_contact_type');
    $display_contact_description = $items->getSetting('display_contact_description');
    $elements = [];

    $list = [];
    foreach($items as $delta => $item) {
      $value = $item->get('contact_name')->getString();
      $value_type = $item->get('contact_type')->getString();
      $value_description = $item->get('contact_description')->getString();

      // Change the non-user-friendly 'null' contact, which is specified by chado.
      if ($value == 'null') {
        $value = 'Unknown';
      }
      $list[$delta] = $value;
      if ($value_type and $display_contact_type) {
        $list[$delta] .= ' (' . $value_type . ')';
      }
      if ($value_description and $display_contact_description) {
        $list[$delta] .= ' [' . $value_description . ']';
      }
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
// @todo: add a pager
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
