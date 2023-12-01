<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado publication widget.
 *
 * @FieldWidget(
 *   id = "chado_pub_widget_default",
 *   label = @Translation("Chado Publication Widget"),
 *   description = @Translation("The default publication widget."),
 *   field_types = {
 *     "chado_pub_default"
 *   }
 * )
 */
class ChadoPubWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the list of analyses.
    $analyses = [];
    $chado = \Drupal::service('tripal_chado.database');

    // In addition to getting a sorted list of analyses, include
    // the pubprop rdfs:type when it is present, e.g.
    // genome assembly or genome annotation.
    $sql = 'SELECT A.pub_id, A.name, TYPE.value FROM {1:pub} A
      LEFT JOIN (
        SELECT AP.pub_id, AP.value FROM {1:pubprop} AP
        LEFT JOIN {1:cvterm} T ON AP.type_id=T.cvterm_id
        LEFT JOIN {1:cv} CV ON T.cv_id=CV.cv_id
        WHERE T.name=:cvterm
        AND CV.name=:cv
      ) AS TYPE
      ON A.pub_id=TYPE.pub_id
      ORDER BY LOWER(A.name)';
    $results = $chado->query($sql, [':cvterm' => 'type', ':cv' => 'rdfs']);

    while ($pub = $results->fetchObject()) {
      $type_text = $pub->value ? ' (' . $pub->value . ')' : '';
      $analyses[$pub->pub_id] = $pub->name . $type_text;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $pub_id = $item_vals['pub_id'] ?? 0;
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
    $elements['pub_id'] = $element + [
      '#type' => 'select',
      '#options' => $analyses,
      '#default_value' => $pub_id,
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
      if ($value['pub_id'] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no pub_id, this means
          // we need to pass in this record to chado storage to
          // have the linker record be deleted there. To do this,
          // we need to have the correct primitive type for this
          // field, so change from empty string to zero.
          $values[$val_key]['pub_id'] = 0;
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
