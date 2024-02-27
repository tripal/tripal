<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Sequence Coordinates widget.
 *
 * @FieldWidget(
 *   id = "chado_sequence_coordinates_widget_default",
 *   label = @Translation("Chado Sequence Coordinates Widget"),
 *   description = @Translation("The default chado sequence coordinates widget."),
 *   field_types = {
 *     "chado_sequence_coordinates_type_default"
 *   }
 * )
 */
class ChadoSequenceCoordinatesWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $item_vals = $items[$delta]->getValue();
    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['record_id'] ?? 0,
    ];

    return $elements;
  }

}
