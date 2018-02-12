<?php

class sio__annotation_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Chado Annotation';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__cvterm');

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $headers = array('Term', 'Definition');
    $rows = array();

    $field_table = $this->instance['settings']['chado_table'];
    $schema = chado_get_schema($field_table);

    $vocabulary_term = tripal_get_chado_semweb_term('cvterm', 'cv_id');
    $accession_term = tripal_get_chado_semweb_term('dbxref', 'accession');
    $definition_term = tripal_get_chado_semweb_term('cvterm', 'definition');
    if (array_key_exists('is_not', $schema['fields'])) {
      $negation_term = tripal_get_chado_semweb_term($field_table, 'is_not');
    }

    $chado_table = $this->instance['settings']['chado_table'];
    foreach ($items as $delta => $item) {

      if (!empty($item['chado-' . $chado_table . '__cvterm_id'])) {
        $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $item['chado-' . $chado_table . '__cvterm_id']));
        $dbxref = $cvterm->dbxref_id;

        // Build the accession.
        $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
        if ($dbxref->db_id->urlprefix) {
          $accession = l($accession, tripal_get_dbxref_url($dbxref), array('attributes' => array('target' => '_blank')));
        }

        $row = array(
          $accession,
          $item['value'][$definition_term],
        );

        if (array_key_exists('is_not', $schema['fields'])) {
          if ($negation_term == FALSE) {
            $row[1] = 'NOT ' . $row[1];
          }
        }

        $rows[] = $row;
      }
    }

    // Theme the results in a talbe.
    $caption = 'This record is associated with the following annotations.';
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => "$chado_table-table-terms",
        'class' => 'tripal-data-table'
      ),
      'caption' => $caption,
      'sticky' => FALSE,
      'colgroups' => array(),
      'empty' => 'There are no annotations of this type',
    );

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
      );
    }
  }
}
