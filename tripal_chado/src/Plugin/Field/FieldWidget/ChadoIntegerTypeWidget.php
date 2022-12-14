<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalIntegerTypeWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado integer type widget.
 *
 * @FieldWidget(
 *   id = "chado_integer_type_widget",
 *   label = @Translation("Chado Integer Widget"),
 *   description = @Translation("The default integer type widget."),
 *   field_types = {
 *     "chado_integer_type"
 *   }
 * )
 */
class ChadoIntegerTypeWidget extends TripalIntegerTypeWidget {

}
