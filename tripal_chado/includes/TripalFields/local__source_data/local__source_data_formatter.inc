<?php

class local__source_data_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Data Source';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__source_data'];


  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $sourcename_term = chado_get_semweb_term('analysis', 'sourcename');
    $sourceversion_term = chado_get_semweb_term('analysis', 'sourceversion');
    $sourceuri_term = chado_get_semweb_term('analysis', 'sourceuri');

    $content = 'The data source is not provided.';
    if ($items[0]['value']) {
      $content = "<dl class=\"tripal-dl\">";
      if (!empty($items[0]['value'][$sourcename_term])) {
        $content .= "<dt>Source Name</dt><dd>: " . $items[0]['value'][$sourcename_term] . " </dd>";
      }
      if (!empty($items[0]['value'][$sourceversion_term])) {
        $content .= "<dt>Source Version</dt><dd>: " . $items[0]['value'][$sourceversion_term] . " </dd>";
      }
      if (!empty($items[0]['value'][$sourceuri_term])) {
        $content .= "<dt>Source URI</dt><dd>: " . l($items[0]['value'][$sourceuri_term], $items[0]['value'][$sourceuri_term], ['attributes' => ['target' => '_blank']]) . " </dd>";
      }
      $content .= "</dl>";
    }
    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
