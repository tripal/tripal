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

    // Get the list of publications.
    $pubs = [];
    $chado = \Drupal::service('tripal_chado.database');

    // In addition to getting a sorted list of pubs, include
    // the pubprop rdfs:type when it is present, e.g.
    // genome assembly or genome annotation.
    $sql = 'SELECT P.pub_id, P.title FROM {1:pub} P
      ORDER BY LOWER(P.title)';
    $results = $chado->query($sql, []);

    while ($pub = $results->fetchObject()) {
      $pubs[$pub->pub_id] = $pub->title;
      // Change the non-user-friendly 'null' publication.
      if ($pubs[$pub->pub_id] == '') {
        $pubs[$pub->pub_id] = '-- Unknown --';  // This will sort to the top.
      }
    }
    natcasesort($pubs);

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $pub_id = $item_vals['pub_id'] ?? 0;
    // If a linker table is used, values for additional columns that
    // may or may not be present in that table.
    $linker_type_id = $item_vals['linker_type_id'] ?? 1;
    $linker_rank = $item_vals['linker_rank'] ?? $delta;

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
      '#options' => $pubs,
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
