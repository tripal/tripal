<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Sequence widget.
 *
 * @FieldWidget(
 *   id = "chado_local_source_data_widget_default",
 *   label = @Translation("Chado Local Source Data Widget Default"),
 *   description = @Translation("The default chado local source data widget which allows curators to manually enter analysis source on the content edit page."),
 *   field_types = {
 *     "chado_local_source_data_default"
 *   }
 * )
 */
class ChadoLocalSourceDataWidgetDefault extends ChadoWidgetBase {

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
    $elements['sourceuri'] =  [
      '#title' => "Local Source URI",
      '#type' => 'textfield',
      '#default_value' => $item_vals['sourceuri'] ?? '',
    ];
    $elements['sourcename'] =  [
      '#title' => "Local Source Name",
      '#type' => 'textfield',
      '#default_value' => $item_vals['sourcename'] ?? '',
    ];
    $elements['sourceversion'] = [
      '#type' => 'textfield',
      '#default_value' => $item_vals['sourceversion'] ?? '',
      '#title' => 'Local Source Version',
    ];

    return $elements;
  }


  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Remove any empty values that aren't mapped to a record id.
    foreach ($values as $val_key => $value) {
      $values[$val_key]['sourceuri'] = $value['sourceuri'];
      $values[$val_key]['sourcename'] = $value['sourcename'];
      $values[$val_key]['sourceversion'] = $value['sourceversion'];
    }
    return $values;
  }

}
