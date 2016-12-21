<?php

class sbo__database_cross_reference_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Database cross reference';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__database_cross_reference');

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
    $chado_table = $this->field['settings']['chado_table'];
    $content = '';
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $content = $item['value']['vocabulary'] . ':' . $item['value']['accession'];
      if ($item['value']['URL']) {
        $content = l($content, $item['value']['URL'], array('attributes' => array('target' => '_blank')));
      }
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }

    if (count($element) == 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => '',
      );
    }
  }
}
