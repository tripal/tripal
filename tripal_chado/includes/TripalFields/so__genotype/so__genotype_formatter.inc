<?php

class so__genotype_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Genotype';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['so__genotype'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];

    $headers = ['Name', 'Description', 'Type'];
    $rows = [];

    foreach ($items as $delta => $item) {
      $genotype = $item['value'];
      if (!$genotype) {
        continue;
      }

      // Get the field values
      $genotype_name = $genotype['schema:name'];
      $description = $genotype['schema:description'];
      $type = $genotype['rdfs:type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $genotype_name = l($genotype_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
      }
      $rows[] = [$genotype_name, $description, $type];
    }

    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_linker-table-genotype-object',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'There are no genotypes available.',
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
