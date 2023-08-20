<?php declare(strict_types = 1);

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of the 'chado_example_formatter' field formatter for 'chado_example'.
 *
 * @FieldFormatter(
 *   id = "chado_example_formatter",
 *   label = @Translation("Chado Example Formatter"),
 *   description = @Translation(""),
 *   field_types = {
 *     "chado_example"
 *   }
 * )
 */
class ChadoExampleFormatter extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    // Rather than displaying the primary key value to the page,
    // you should either change it to be a user-sharable value
    // or you should do something with it (e.g. pass to an API or micro-service).
    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get("value")->getString(),
      ];
    }

    return $elements;
  }

}

