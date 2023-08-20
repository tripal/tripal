<?php declare(strict_types = 1);

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of the 'chado_example_widget' field widget for 'chado_example'.
 *
 * @FieldWidget(
 *   id = "chado_example_widget",
 *   label = @Translation("Chado Example Widget"),
 *   description = @Translation(""),
 *   field_types = {
 *     "chado_example"
 *   }
 * )
 */
class ChadoExampleWidget extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Grab the values for our properties based on the passed in delta.
    // For fields with a cardinality above 1, this is called one per record
    // with the delta indicating the current record.
    $item_vals = $items[$delta]->getValue();

    // Define your form elements here.=
    $element['value'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['value'] ?? 0,
    ];

    return $element;
  }

}

