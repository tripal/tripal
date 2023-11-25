<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado assay widget.
 *
 * @FieldWidget(
 *   id = "chado_assay_widget_default",
 *   label = @Translation("Chado Assay Widget"),
 *   description = @Translation("The default assay widget."),
 *   field_types = {
 *     "chado_assay_default"
 *   }
 * )
 */
class ChadoAssayWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the list of assays.
    $assays = [];
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('assay', 'a');
    $query->fields('a', ['assay_id', 'name']);
    $query->orderBy('name');
    $results = $query->execute();
    while ($assay = $results->fetchObject()) {
      $assays[$assay->assay_id] = $assay->name;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $assay_id = $item_vals['assay_id'] ?? 0;
    // If a linker table is used, values for additional columns that
    // may or may not be present in that table.
    $linker_type_id = $item_vals['linker_type_id'] ?? 1;
    $linker_rank = $item_vals['linker_rank'] ?? $delta;
    $linker_pub_id = $item_vals['linker_pub_id'] ?? 1;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['link'] = [
      '#type' => 'value',
      '#default_value' => $link,
    ];
    $elements['assay_id'] = $element + [
      '#type' => 'select',
      '#options' => $assays,
      '#default_value' => $assay_id,
      '#empty_option' => '-- Select --',
    ];

    // For linker table columns that may or may not be present,
    // it doesn't hurt to always include them, they will be ignored
    // when not needed.
    $elements['linker_type_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_type_id,
    ];
    $elements['linker_rank'] = [
      '#type' => 'value',
      '#default_value' => $linker_rank,
    ];
    // e.g. cell_line_feature has pub_id with not null constraint
    $elements['linker_pub_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_pub_id,
    ];

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    // Handle any empty values.
    foreach ($values as $val_key => $value) {
      if ($value['assay_id'] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no assay_id, this means
          // we need to pass in this record to chado storage to
          // have the linker record be deleted there. To do this,
          // we need to have the correct primitive type for this
          // field, so change from empty string to zero.
          $values[$val_key]['assay_id'] = 0;
        }
        else {
          unset($values[$val_key]);
        }
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      $values[$val_key]['_weight'] = $i;
      $i++;
    }

    return $values;
  }
}
