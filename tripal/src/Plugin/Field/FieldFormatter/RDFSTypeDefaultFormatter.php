<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\Component\Utility\Html;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Plugin\Field\TripalFormatterBase;

/**
 * Plugin implementation of the 'rdfs__type_formatter' formatter.
 *
 * @FieldFormatter(
 *   id = "rdfs__type_formatter",
 *   label = @Translation("Tripal Content Type"),
 *   field_types = {
 *     "rdfs__type"
 *   }
 * )
 */
class RDFSTypeDefaultFormatter extends TripalFormatterBase {

}
