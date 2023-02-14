<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalTextTypeWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Sequence widget.
 *
 * @FieldWidget(
 *   id = "chado_sequence_widget_default",
 *   label = @Translation("Chado Sequence Widget"),
 *   description = @Translation("The default chado sequence widget."),
 *   field_types = {
 *     "chado_sequence_default"
 *   }
 * )
 */
class ChadoSequenceWidgetDefault extends ChadoWidgetBase {

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

    $elements['residues'] = $element + [
      '#type' => 'textarea',
      '#default_value' => $item_vals['residues'] ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
    ];

    return $elements;
  }


  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    
    // Remove any empty values that aren't mapped to a record id.
    foreach ($values as $val_key => $value) {
      $values[$val_key]['residues'] = preg_replace('/\s/', '', $value['residues']);
      $values[$val_key]['seqlen'] = strlen($values[$val_key]['residues']);
      $values[$val_key]['md5checksum'] = md5($values[$val_key]['residues']);
     
    }
    return $values;
  }

}
