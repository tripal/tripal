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
    $elements['featureloc_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['featureloc_id'] ?? 0,
    ];

    $elements['fmin'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['fmin'] ?? 0,
    ];
    $elements['is_fmin_partial'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['is_fmin_partial'] ?? FALSE,
    ];
    $elements['fmax'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['fmax'] ?? 0,
    ];
    $elements['is_fmax_partial'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['is_fmax_partial'] ?? FALSE,
    ];
    $elements['strand'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['strand'] ?? 0,
    ];
    $elements['phase'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['phase'] ?? 0,
    ];
    $elements['residue_info'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['residue_info'] ?? '',
    ];
    $elements['locgroup'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['locgroup'] ?? 0,
    ];
dpm($elements, "CPW2 elements"); //@@@
    return $elements;
  }

}
