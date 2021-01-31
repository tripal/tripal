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

    // Do we have an empty list? If so, just return.
    if (!$items[0]['value']) {
      return;
    }

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $linker_table = $base_table . '_dbxref';

    $dbname_term = chado_get_semweb_term('db', 'name');
    $accession_term = chado_get_semweb_term('dbxref', 'accession');
    $dburl_term = chado_get_semweb_term('db', 'url');

    // First, organize the values by their databases.
    $ordered_items = [];
    foreach ($items as $delta => $item) {
      $db = $item['value'][$dbname_term];
      $accession = $item['value'][$accession_term];
      // It is possible that a database does not have a url, in which case no link can be generated.
      if (array_key_exists($dburl_term, $item['value']) and $item['value'][$dburl_term]) {
        $url = $item['value'][$dburl_term];

        // This emulates chado_get_dbxref_url() but implemented inline here for better performance.
        $db_count = 0;
        $acc_count = 0;
        $url = preg_replace('/\{db\}/', $db, $url, -1, $acc_count);
        $url = preg_replace('/\{accession\}/', $accession, $url, -1, $acc_count);

        $content = l($accession, $url, ['attributes' => ['target' => '_blank']]);
      }
      else {
        $content = $accession;
      }
      $ordered_items[ucfirst($db)][] = $content;
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
          'data' => ucfirst($type),
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
        'id' => 'sbo__database_cross_reference',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'There are no cross references.',
    ];
    $content = theme_table($table);
    $element[0] = [
      '#type' => 'markup',
      '#markup' => $content,
    ];

  }
}
