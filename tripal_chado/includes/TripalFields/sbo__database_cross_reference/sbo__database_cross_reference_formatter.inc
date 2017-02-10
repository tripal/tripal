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
    $chado_table = $this->instance['settings']['chado_table'];
    $content = '';
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $content = $item['value']['data:1056'] . ':' . $item['value']['data:2091'];
      if ($item['value']['schema:url']) {
        $content = l($content, $item['value']['schema:url'], array('attributes' => array('target' => '_blank')));
      }
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }

    if (count($element) == 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => 'There are no cross references.',
      );
    }
  }
}
