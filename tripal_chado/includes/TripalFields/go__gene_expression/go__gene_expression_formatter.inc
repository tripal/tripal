<?php

class go__gene_expression_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Gene expression';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['go__gene_expression'];


  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];

    $content = '';
    $rows = [];
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      // Iterate through all of the children of the $item['value']. Add each
      // one as an independent row in the table.

      foreach ($item['value'] as $key => $value) {

        // If this key is the name, then we want to link to the entity if one
        // exists.
        if ($key == 'name') {
          if (array_key_exists('entity', $item['value']) and $item['value']['$entity_id']) {
            list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
            $value = l($value, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
          }
        }
        // If this key is the publication then we want to get the citation
        // and link to the pub if an entity exits.
        if ($key == 'publication') {
          $pub = $value['Citation'];
          if (array_key_exists('publication', $item) and array_key_exists('entity', $item['publication'][0])) {
            $entity_id = $item['publication'][0]['entity_id'];
            $title = $item['value']['publication']['Title'];
            $link = l($title, 'bio_data/' . $entity_id);
            $pub = preg_replace("/$title/", $link, $pub);
          }
          $value = $pub;
        }
        // Add the item as a new row.
        $rows[] = [
          [
            'data' => ucfirst(str_replace('_', ' ', $key)),
            'header' => TRUE,
            'width' => '20%',
          ],
          $value,
        ];
      }
    }
    $table = [
      'header' => [],
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_linker-table-expression-object',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'There is no expression data available.',
    ];
    $content = theme_table($table);
    if (count($items) > 0) {
      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }
}
