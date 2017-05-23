<?php

class data__sequence_coordinates_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Sequence Coordinates';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence_coordinates');

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $content = '';
    foreach ($items as $item) {
      if (!empty($item['value'])) {
        $srcfeature = $item['value']['data:3002'];
        $fmin = $item['value']['local:fmin'];
        $fmax = $item['value']['local:fmax'];
        $phase = $item['value']['data:2336'];
        $strand = $item['value']['data:0853'];
        if ($strand == 1) {
          $strand = '+';
        }
        else {
          $strand = '-';
        }
        $content .= $srcfeature . ':' . $fmin . '..' . $fmax . $strand;
      }
    }
    if (!$content) {
      $content = 'This feature is not located on any sequence.';
    }
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}
