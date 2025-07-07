<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of an empty widget for computed sequence.
 * Derived sequences cannot be edited directly in the database.
 *
 * @FieldWidget(
 *   id = "chado_derived_sequence_widget_default",
 *   label = @Translation("Chado Derived Sequence Widget"),
 *   description = @Translation("An empty derived sequence widget for computed sequence that cannot be edited."),
 *   field_types = {
 *     "chado_derived_sequence_type_default"
 *   }
 * )
 */
class ChadoDerivedSequenceWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   * Default sequence is not editable. Therefore, we do not need to
   * add any form elements the settings form.
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    // The derived sequence is not editable, so we do not need to add any
    // form elements.
    return [];
  }


  /**
   * {@inheritDoc}
   *
   * There are no values to validate for the derived sequence widget.
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    // No values to validate for the derived sequence widget.
    // This is a placeholder function to satisfy the interface.
    return $values;
  }

}
