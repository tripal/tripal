<?php

class local__source_data_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Data Source';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__source_data');


  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $content = 'The data source is not provided.';
    if ($items[0]['value']) {
      $content = "
        <dl class=\"tripal-dl\">
           <dt>Source Name</dt>
           <dd>: " . $items[0]['value']['schema:name'] . " </dd>
           <dt>Source Version</dt>
           <dd>: " . $items[0]['value']['IAO:0000129'] . " </dd>
           <dt>Source URI</dt>
           <dd>: " . $items[0]['value']['data:1047'] . " </dd>
        </dl>
      ";
    }
    $element[0] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}
