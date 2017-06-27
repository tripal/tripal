<?php

class sbo__database_cross_reference_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Cross reference';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__database_cross_reference');

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $chado_table = $this->instance['settings']['chado_table'];
    $content = '';

    $dbname_term = tripal_get_chado_semweb_term('db', 'name');
    $accession_term = tripal_get_chado_semweb_term('dbxref', 'accession');
    $dburl_term = tripal_get_chado_semweb_term('db', 'url');

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $content = $item['value'][$dbname_term] . ':' . $item['value'][$accession_term];
      if ($item['value'][$dburl_term]) {
        $dbxref = tripal_get_dbxref(array('dbxref_id' => $item['chado-feature_dbxref__dbxref_id']));
        $url = tripal_get_dbxref_url($dbxref);
        $content = l($content, $url, array('attributes' => array('target' => '_blank')));
      }
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }

    if (count($element) == 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => 'There are no cross references.',
      );
    }
  }
}
