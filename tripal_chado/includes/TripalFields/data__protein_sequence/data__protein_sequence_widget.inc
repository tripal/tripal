<?php

class data__protein_sequence_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Protein Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__protein_sequence'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // TODO: before we can create a widget we must have a way to
    // save another record whose table matches the same as the base base
    // table with the chado_storage backend getting confused as to what value
    // belongs to which feature record.
  }
}
