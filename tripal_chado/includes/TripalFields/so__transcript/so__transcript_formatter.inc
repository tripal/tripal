<?php

class so__transcript_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Transcript';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['so__transcript'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $headers = ['Transcript Name', 'Identifier', 'Type', 'Location'];
    $rows = [];
    foreach ($items as $delta => $item) {

      if (!$item['value']) {
        continue;
      }
      $transcript = $item['value'];

      // Get the field values
      $feature_name = $transcript['schema:name'];
      $feature_uname = $transcript['data:0842'];
      $loc = $transcript['SO:0000735'];
      $type = $transcript['rdfs:type'];

      // Add a link if there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $feature_name = l($feature_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
      }
      $rows[] = [$feature_name, $feature_uname, $type, $loc];
    }
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_feature-table-transcripts-object',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'This feature has no transcripts',
    ];
    $content = theme_table($table);

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }
}
