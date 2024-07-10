<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_source_data_formatter_default",
 *   label = @Translation("Chado Source Data Formatter"),
 *   description = @Translation("The default source data widget which allows curators to manually enter analysis source data information on the content edit page."),
 *   field_types = {
 *     "chado_source_data_type_default"
 *   }
 * )
 */
class ChadoSourceDataFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode)  {
    $elements = [];
    $content = '';

    foreach ($items as $delta => $item) {
      $sourcename_val = $item->get('sourcename')->getString();
      if (!empty($sourcename_val)) {
        $content .= "<dt>Source Name:</dt><dd>" . $sourcename_val . "</dd>";
      }
      $sourceversion_val = $item->get('sourceversion')->getString();
      if (!empty($sourceversion_val)) {
        $content .= "<dt>Source Version:</dt><dd>" . $sourceversion_val . "</dd>";
      }
      $sourceuri_val = $item->get('sourceuri')->getString();
      if (!empty($sourceuri_val)) {
        $url = $sourceuri_val;
        if (preg_match('|://|', $sourceuri_val)) {
          $url = Link::fromTextAndUrl($sourceuri_val, Url::fromUri($sourceuri_val, []))->toString();
        }
        $content .= "<dt>Source URI:</dt><dd>" . $url . "</dd>";
      }
    }

    if ($content) {
      $content = "<dl class=\"tripal-dl\">" . $content . "</dl>";
    } else {
      $content = 'The data source is not provided.';
    }

    // The cardinality of this field is always 1, so only create element for $delta of zero.
    $elements[0] = [
      '#markup' => $content,
    ];

    return $elements;
  }
}
