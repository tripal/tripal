<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class sep__protocol_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Protocol';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sep__protocol'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    if ($items[0]['value']) {

      $protocol_name_term = chado_get_semweb_term('protocol', 'name');
      $protocol_type_term = chado_get_semweb_term('protocol', 'type_id');

      $protocol_name = $items[0]['value'][$protocol_name_term];
      $content = $protocol_name;
      if (array_key_exists('entity_id', $items[0]['value'])) {
        list($entity_type, $entity_id) = explode(':', $items[0]['value']['entity_id']);
        if ($entity_id) {
          $content = l($protocol_name, 'bio_data/' . $entity_id);
        }
      }
      //cardinality for this field is 1
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }


  /**
   * @see TripalFieldFormatter::settingsSummary()
   */
  public function settingsSummary($view_mode) {
    return '';
  }

}