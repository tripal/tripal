<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado arraydesign widget.
 *
 * @FieldWidget(
 *   id = "chado_arraydesign_widget_default",
 *   label = @Translation("Chado ArrayDesign Widget"),
 *   description = @Translation("The default arraydesign widget."),
 *   field_types = {
 *     "chado_arraydesign_default"
 *   }
 * )
 */
class ChadoArrayDesignWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the list of arrar designs.
    $array_designs = [];
    $chado = \Drupal::service('tripal_chado.database');

    $sql = 'SELECT A.arraydesign_id, A.name FROM arraydesign A
      ORDER BY A.name';
    $results = $chado->query($sql, []);

    while ($arraydesign = $results->fetchObject()) {
      $array_designs[$arraydesign->arraydesign_id] = $arraydesign->name;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $arraydesign_id = $item_vals['arraydesign_id'] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['arraydesign_id'] = $element + [
      '#type' => 'select',
      '#options' => $array_designs,
      '#default_value' => $arraydesign_id,
      '#placeholder' => $this->getSetting('placeholder'),
      '#empty_option' => '-- Select --',
    ];

    return $elements;
  }

}
