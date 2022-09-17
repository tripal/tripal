<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type widget.
 *
 * @FieldWidget(
 *   id = "chado_linker__prop_widget",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   field_types = {
 *     "chado_linker__prop"
 *   }
 * )
 */
class chado_linker__prop_widget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {


    $element['value'] = [
      '#type' => 'textarea',
      '#default_value' => $items[$delta]->value ?? '',
      '#title' => '',
      '#description' => '',
      '#rows' => '',
      '#required' => TRUE,
    ];

/*     $element[$value_term] = [
      '#type' => 'hidden',
      '#default_value' => $record_id,
    ];
    $element['chado-' . $field_table . '__' . $lfkey_field] = [
      '#type' => 'hidden',
      '#value' => $fk_value,
    ];
    $element['chado-' . $field_table . '__value'] = [
      '#type' => 'textarea',
      '#default_value' => $value,
      '#title' => $instance['label'],
      '#description' => $instance['description'],
      '#rows' => $rows,
      '#required' => $instance['required'],
    ];
    $element['chado-' . $field_table . '__type_id'] = [
      '#type' => 'hidden',
      '#value' => $type_id,
    ];
    $element['chado-' . $field_table . '__rank'] = [
      '#type' => 'hidden',
      '#value' => $rank,
    ]; */

    return $element + parent::formElement($items, $delta, $element, $form, $form_state);
  }
}