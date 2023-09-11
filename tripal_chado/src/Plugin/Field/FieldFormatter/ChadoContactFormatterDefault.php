<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal contact formatter.
 *
 * @FieldFormatter(
 *   id = "chado_contact_formatter_default",
 *   label = @Translation("Chado contact formatter"),
 *   description = @Translation("A chado contact formatter"),
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
    $elements = [];

    foreach($items as $delta => $item) {
      $contact_name = $item->get('contact_name')->getString();
      // Change the non-user-friendly 'null' contact, which is spedified by chado.
      if ($contact_name == 'null') {
        $contact_name = 'Unknown';
      }
      $elements[$delta] = [
        "#markup" => $contact_name,
      ];
    }

    return $elements;
  }
}
