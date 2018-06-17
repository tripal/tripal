<?php

class local__gbrowse_image_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'GBrowse View';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__gbrowse_image');

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $content = '';
    if (count($items) > 0) {
      $url_term = chado_get_semweb_term('db', 'url');
      if (is_array($items[0]['value']) and array_key_exists($url_term, $items[0]['value'])) {
        $content = '<img src="' . $items[0]['value'][$url_term] . '" class="tripal-local--gbrowse-image"/>';
      
        $element[0] = array(
          // We create a render array to produce the desired markup,
          '#type' => 'markup',
          '#markup' => $content,
        );
      }
    }
  }
}
