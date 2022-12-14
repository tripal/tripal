<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalStringTypeWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado string type widget.
 *
 * @FieldWidget(
 *   id = "chado_string_type_widget",
 *   label = @Translation("Chado String Widget"),
 *   description = @Translation("The default string type widget."),
 *   field_types = {
 *     "chado_string_type"
 *   }
 * )
 */
class ChadoStringTypeWidget extends TripalStringTypeWidget {

}
