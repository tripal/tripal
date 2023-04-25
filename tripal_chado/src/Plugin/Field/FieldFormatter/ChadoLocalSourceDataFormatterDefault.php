<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\Core\Link;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_local_source_data_formatter_default",
 *   label = @Translation("Chado Local Source Data Formatter"),
 *   description = @Translation("Analysis local source data formatter default"),
 *   field_types = {
 *     "chado_local_source_data_default"
 *   }
 * )
 */
class ChadoLocalSourceDataFormatterDefault extends ChadoFormatterBase {

  /**
  * {@inheritdoc}
  */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoLocalSourceDataFormatter';

    $content = 'The data source is not provided.';

    foreach($items as $delta => $item) {
      $content = "<dl class=\"tripal-dl\">";
      $prog_vers_val = $item->get( 'programversion' )->getString() ;
      if ( !empty( $prog_vers_val ) ) {
        $content .= "<dt>Software version</dt><dd>: " . $prog_vers_val . " </dd>";
      }
      $sourcename_val = $item->get( 'sourcename' )->getString() ;
      if ( !empty( $sourcename_val ) ) {
        $content .= "<dt>Source Name</dt><dd>: " . $sourcename_val . " </dd>";
      }
      $sourceversion_val = $item->get( 'sourceversion' )->getString() ;
      if ( !empty( $sourceversion_val ) ) {
        $content .= "<dt>Source Version</dt><dd>: " . $sourceversion_val . " </dd>";
      }
      $sourceuri_val = $item->get( 'sourceuri' )->getString() ;
      /*
        if (preg_match('/^https?:/i', $sourceuri_val ) ) {
          $sourceuri_val = Link::fromTextAndUrl($sourceuri_val, $sourceuri_val);
        }
      */
      if ( !empty( $sourceuri_val ) ) {
        $content .= "<dt>Source URI</dt><dd>: " . $sourceuri_val . " </dd>";
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
