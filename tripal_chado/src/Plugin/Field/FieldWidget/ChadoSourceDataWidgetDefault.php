<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Data Source widget.
 *
 * @FieldWidget(
 *   id = "chado_source_data_widget_default",
 *   label = @Translation("Chado Data Source Widget Default"),
 *   description = @Translation("The default source data widget which allows curators to manually enter analysis source data information on the content edit page."),
 *   field_types = {
 *     "chado_source_data_type_default"
 *   }
 * )
 */
class ChadoSourceDataWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state)  {

    $item_vals = $items[$delta]->getValue();

    $elements = [];

    $elements['datasourcegroup'] = [
      '#type' => 'details',
      '#title' => t("Data Source"),
      '#description' => t('The source where data was obtained for this analysis.'),
      '#open' => TRUE, // Controls the HTML5 'open' attribute. Defaults to FALSE.
    ];

    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['record_id'] ?? 0,
    ];
    $elements['datasourcegroup']['sourcename'] =  [
      '#title' => t("Name"),
      '#type' => 'textfield',
      '#description' => t('The name of the source where data was obtained for this analysis.'),
      '#default_value' => $item_vals['sourcename'] ?? '',
    ];
    $elements['datasourcegroup']['sourceversion'] = [
      '#title' => t('Version'),
      '#type' => 'textfield',
      '#description' => t('The version number of the data source (if applicable) for this analysis.'),
      '#default_value' => $item_vals['sourceversion'] ?? '',
    ];
    $elements['datasourcegroup']['sourceuri'] =  [
      '#title' => t("URI"),
      '#type' => 'textfield',
      '#description' => t('The URI (e.g. web URL) where the source data can be obtained.'),
      '#default_value' => $item_vals['sourceuri'] ?? '',
    ];

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state)  {

    // Remove any empty values that aren't mapped to a record id.
    foreach ($values as $val_key => $value) {
      $values[$val_key]['sourceuri'] = $value['datasourcegroup']['sourceuri'];
      $values[$val_key]['sourcename'] = $value['datasourcegroup']['sourcename'];
      $values[$val_key]['sourceversion'] = $value['datasourcegroup']['sourceversion'];
    }
    return $values;
  }
}
