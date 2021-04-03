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
      $ientity = isset($item['value']['entity']) ? $item['value']['entity'] : '';
      $name = isset($item['value']['schema:name']) ? $item['value']['schema:name'] : '';
      $identifier = isset($item['value']['data:0842']) ? $item['value']['data:0842'] : '';
      if ($ientity) {
        list($entity_type, $entity_id) = explode(':', $ientity);
        $name = l(strip_tags($name), 'bio_data/' . $entity_id);
      }
      $ordered_items[ucfirst($type)][] = $name;
    }

    // Reorder the list so it's compatible with theming a list.
    ksort($ordered_items);

    // Generate the pagers for each type.
    $list_items = [];
    $headers = [];
    $rows = [];
    foreach ($ordered_items as $type => $children) {
      $items_per_page = array_key_exists('items_per_page', $this->instance['settings']) ? $this->instance['settings']['items_per_page'] : 10;
      $total_records = count($children);
      $total_pages = (int) ($total_records / $items_per_page) + 1;
      $pelement = 0;
      $current_page = pager_default_initialize($total_records, $items_per_page, $pelement);
      $pager = theme('pager', [
        'tags' => [],
        'element' => $pelement,
        'parameters' => [],
        'quantity' => 5,
      ]);
      $pager = $this->ajaxifyPager($pager, $entity);
      $page_items = array_chunk($children, $items_per_page);


      $rows[] = [
        [
          'data' => ucfirst($type) . '(s)',
          'header' => TRUE,
          'width' => '20%',
        ],
        theme_item_list([
          'items' => $page_items[$current_page],
          'title' => '',
          'type' => 'ul',
          'attributes' => [],
        ]) . $pager,
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
