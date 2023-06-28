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
 *   label = @Translation("Chado analysis local source data Widget default"),
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
<<<<<<< HEAD
=======

    $elements['programversion'] =  [
      '#title' => "Software version",
      '#type' => 'textfield',
      '#default_value' => $item_vals['programversion'] ?? '',
    ];
>>>>>>> b73440286faed0b3cf68dec7fdff3e2bb4528450
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
      $values[$val_key]['programversion'] = $value['programversion'];      
      $values[$val_key]['sourceuri'] = $value['sourceuri'];      
      $values[$val_key]['sourcename'] = $value['sourcename'];      
      $values[$val_key]['sourceversion'] = $value['sourceversion'];
    }
    return $values;
  }

}