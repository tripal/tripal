<?php

class sbo__phenotype_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Phenotype';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sbo__phenotype'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];

    $headers = [];
    $rows = [];
    $has_type = FALSE;
    $has_value = FALSE;
    $has_name = FALSE;

    // First iterate through the phenotypes to see if
    // we have all of the data columns.
    foreach ($items as $delta => $item) {
      $phenotype = $item['value'];
      if (!$phenotype) {
        continue;
      }

      // Get the field values
      if ($phenotype['name']) {
        $has_name = TRUE;
      }
      if ($phenotype['value']) {
        $has_value = TRUE;
      }
      if ($phenotype['type']) {
        $has_type = TRUE;
      }
    }
    foreach ($items as $delta => $item) {
      $row = [];
      $phenotype = $item['value'];
      if ($has_type) {
        $row[] = $phenotype['type'];
      }
      if ($has_name) {
        $phenotype_name = $phenotype['name'];
        // Add a link i there is an entity.
        if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
          list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
          $phenotype_name = l($phenotype_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
        }
        $row[] = $phenotype_name;
      }
      if ($has_value) {
        $row[] = $phenotype['value'];
      }
      $rows[] = $row;
    }
    if ($has_type) {
      $headers[] = 'Phenotype';
    }
    if ($has_name) {
      $headers[] = 'Property';
    }
    if ($has_value) {
      $headers[] = 'Value';
    }
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_linker-table-phenotype-object',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'No phenotypes available',
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
