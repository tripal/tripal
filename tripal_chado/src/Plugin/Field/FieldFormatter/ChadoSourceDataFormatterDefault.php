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
 *     "chado_source_data_default"
 *   }
 * )
 */
class ChadoSourceDataFormatterDefault extends ChadoFormatterBase {

  /**
  * {@inheritdoc}
  */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $content = 'The source data is not provided.';
    foreach($items as $delta => $item) {
        $content = "<dl class=\"tripal-dl\">";
        $sourcename_val = $item->get('sourcename')->getString() ;
        if ( !empty( $sourcename_val ) ) {
          $content .= "<dt>Source Name:</dt><dd>" . $sourcename_val . "</dd>";
        }
        $sourceversion_val = $item->get('sourceversion')->getString() ;
        if ( !empty( $sourceversion_val ) ) {
          $content .= "<dt>Source Version:</dt><dd>" . $sourceversion_val . "</dd>";
        }
        $sourceuri_val = $item->get('sourceuri')->getString() ;
        if ( !empty( $sourceuri_val ) ) {
          $url = $sourceuri_val;
          if (preg_match('|://|', $sourceuri_val)) {
            $url = Link::fromTextAndUrl($sourceuri_val, Url::fromUri($sourceuri_val, []))->toString();
          }
          $content .= "<dt>Source URI:</dt><dd>" . $url . "</dd>";
        }
        $content .= "</dl>";
      }
      $elements[$delta] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
  
      return $elements;
    }  
}