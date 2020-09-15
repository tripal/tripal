<?php

/**
 * @file
 * Contains Drupal\tripal_chado\Plugin\Field\FieldWidget\OBIOrganismDefaultWidget.
 */

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a default comment widget.
 *
 * @FieldWidget(
 *   id = "obi__organism_default",
 *   label = @Translation("Organism: Select List"),
 *   field_types = {
 *     "obi__organism"
 *   }
 * )
 */
class OBIOrganismDefaultWidget extends WidgetBase {
  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $settings = $this->getFieldSettings();

    $field_table = $settings['chado_table'];
    $field_column = $settings['chado_column'];

    // Set the linker field appropriately.
    if ($field_table == 'biomaterial') {
      $linker_field = 'chado-biomaterial__taxon_id';
    }
    else {
      $linker_field = 'chado-' . $field_table . '__organism_id';
    }

    $organism_id = 0;
    if (count($items) > 0 and array_key_exists($linker_field, $items[0])) {
      $organism_id = $items[0][$linker_field];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $options = chado_get_organism_select_options(FALSE);
    $widget['record_id'] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $organism_id,
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    ];
    return $widget;
  }
}
