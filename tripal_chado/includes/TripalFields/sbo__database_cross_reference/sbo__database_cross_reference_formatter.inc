<?php

class sbo__database_cross_reference_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Cross reference';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sbo__database_cross_reference'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $content = '';

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $linker_table = $base_table . '_dbxref';

    $dbname_term = chado_get_semweb_term('db', 'name');
    $accession_term = chado_get_semweb_term('dbxref', 'accession');
    $dburl_term = chado_get_semweb_term('db', 'url');

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $content = $item['value'][$dbname_term] . ':' . $item['value'][$accession_term];
      if ($item['value'][$dburl_term]) {
        $dbxref = chado_get_dbxref(['dbxref_id' => $item['chado-' . $linker_table . '__dbxref_id']]);
        $url = chado_get_dbxref_url($dbxref);
        $content = l($content, $url, ['attributes' => ['target' => '_blank']]);
      }
      $element[$delta] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }

    if (count($element) == 0) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => 'There are no cross references.',
      ];
    }
  }
}
