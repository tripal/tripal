<?php

class sio__annotation_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Chado Annotation';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__cvterm'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $headers = ['Term', 'Name', 'Definition'];
    $rows = [];

    $field_table = $this->instance['settings']['chado_table'];
    $schema = chado_get_schema($field_table);

    $vocabulary_term = tripal_get_chado_semweb_term('cvterm', 'cv_id');
    $accession_term = tripal_get_chado_semweb_term('dbxref', 'accession');
    $definition_term = tripal_get_chado_semweb_term('cvterm', 'definition');
    $name_term = tripal_get_chado_semweb_term('cvterm', 'name');

    if (array_key_exists('is_not', $schema['fields'])) {
      $negation_term = chado_get_semweb_term($field_table, 'is_not');
    }

    $chado_table = $this->instance['settings']['chado_table'];
    foreach ($items as $delta => $item) {

      if (!empty($item['chado-' . $chado_table . '__cvterm_id'])) {
        $cvterm = chado_generate_var('cvterm', ['cvterm_id' => $item['chado-' . $chado_table . '__cvterm_id']]);
        $dbxref = $cvterm->dbxref_id;

        // Build the accession.
        $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
        if ($dbxref->db_id->urlprefix) {
          $accession = l($accession, chado_get_dbxref_url($dbxref), ['attributes' => ['target' => '_blank']]);
        }

        $row = [
          $accession,
          $item['value'][$name_term],
          $item['value'][$definition_term],
        ];

        if (array_key_exists('is_not', $schema['fields'])) {
          if ($negation_term == FALSE) {
            $row[1] = 'NOT ' . $row[1];
          }
        }

        $rows[] = $row;
      }
    }

    // Theme the results in a talbe.
    $caption = 'This record has the following annotations.';
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => "$chado_table-table-terms",
        'class' => 'tripal-data-table',
      ],
      'caption' => $caption,
      'sticky' => FALSE,
      'colgroups' => [],
      'empty' => 'There are no annotations of this type',
    ];

    if (count($items) > 0) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => theme_table($table),
      ];
    }
  }
}
