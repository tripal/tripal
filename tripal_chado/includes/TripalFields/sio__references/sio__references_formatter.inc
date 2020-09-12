<?php

class sio__references_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'References';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sio__references'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $field_name = $this->field['field_name'];
    $chado_table = $this->instance['settings']['chado_table'];

    // Do we have an empty list? If so, just return.
    if (!$items[0]['value']) {
      return;
    }

    // First, organize the values by their types.
    $ordered_items = [];
    foreach ($items as $delta => $item) {
      $type = isset($item['value']['rdfs:type']) ? $item['value']['rdfs:type'] : '';
      $entity = isset($item['value']['entity']) ? $item['value']['entity'] : '';
      $name = isset($item['value']['schema:name']) ? $item['value']['schema:name'] : '';
      $identifier = isset($item['value']['data:0842']) ? $item['value']['data:0842'] : '';
      if ($entity) {
        list($entity_type, $entity_id) = explode(':', $entity);
        $name = l(strip_tags($name), 'bio_data/' . $entity_id);
      }
      $ordered_items[ucfirst($type)][] = $name;
    }

    // Reorder the list so it's compatible with theming a list.
    $list_items = [];
    $headers = [];
    $rows = [];
    ksort($ordered_items);
    foreach ($ordered_items as $type => $children) {
      $rows[] = [
        [
          'data' => ucfirst($type) . '(s)',
          'header' => TRUE,
          'width' => '20%',
        ],
        theme_item_list([
          'items' => $children,
          'title' => '',
          'type' => 'ul',
          'attributes' => [],
        ]),
      ];
    }

    $table = [
      'header' => [],
      'rows' => $rows,
      'attributes' => [
        'id' => 'sio__references-table',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'There are no records in this site to which this publication refers.',
    ];
    $content = theme_table($table);
    $element[0] = [
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
