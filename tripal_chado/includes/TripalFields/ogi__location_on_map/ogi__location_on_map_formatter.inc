<?php

class ogi__location_on_map_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Location on Map';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['ogi__location_on_map'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $rows = [];
    $headers = ['Map', 'Position', 'Map Description'];
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $val = $item['value'];
      $map_name = $item['value']['data:1274']['schema:name'];
      $map_desc = $item['value']['data:1274']['schema:description'];
      $position = $item['value']['SIO:000056'];
      $rows[] = [
        $map_name,
        $position,
        $map_desc,
      ];
    }
    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_feature-table-map-positions',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => [],
      'empty' => 'This are no positions on any maps.',
    ];
    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => theme_table($table),
      ];
    }
  }
}
