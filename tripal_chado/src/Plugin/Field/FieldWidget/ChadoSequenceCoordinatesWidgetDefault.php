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
dpm($item_vals, "CPW1 item_vals"); //@@@
    $elements = [];
    $elements['featureloc_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['featureloc_id'] ?? 0,
    ];
//                                       Table "chado.featureloc"
//     Column      |   Type   | Collation | Nullable |                      Default                      
//-----------------+----------+-----------+----------+---------------------------------------------------
// featureloc_id   | bigint   |           | not null | nextval('featureloc_featureloc_id_seq'::regclass)
// feature_id      | bigint   |           | not null | 
// srcfeature_id   | bigint   |           |          | 
// fmin            | bigint   |           |          | 
// is_fmin_partial | boolean  |           | not null | false
// fmax            | bigint   |           |          | 
// is_fmax_partial | boolean  |           | not null | false
// strand          | smallint |           |          | 
// phase           | integer  |           |          | 
// residue_info    | text     |           |          | 
// locgroup        | integer  |           | not null | 0
// rank            | integer  |           | not null | 0

//    $elements['fmin'] = [
//      '#type' => 'value',
//      '#default_value' => $item_vals['fmin'] ?? 0,
//    ];
    $elements['fmax'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['fmax'] ?? 0,
    ];
//    $elements['strand'] = [
//      '#type' => 'value',
//      '#default_value' => $item_vals['strand'] ?? 0,
//    ];
    $elements['phase'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['phase'] ?? 0,
    ];
dpm($elements, "CPW2 elements"); //@@@
    return $elements;
  }

}
