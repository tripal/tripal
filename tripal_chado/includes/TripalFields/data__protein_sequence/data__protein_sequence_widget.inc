<?php

class data__protein_sequence_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Protein Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__protein_sequence');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // TODO: add the form for setting a protein sequence.
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];

  }
}
