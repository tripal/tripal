<?php

class chado_linker__dbxref_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $label = 'Cross references';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__dbxref');

  // The list of default settings for this formatter.
  public static $settings = array();
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
