<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal RDFS content type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_rdfs_type_formatter",
 *   label = @Translation("Default Content Type Formatter"),
 *   description = @Translation("The default resource content type formatter."),
 *   field_types = [
 *     "tripal_rdfs_type"
 *   ]
 * )
 */
class DefaultTripalRDFSTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
        $elements[$delta] = [
          "#markup" => $item->get("type");
        ];
    }

    return $elements;
  }
}
