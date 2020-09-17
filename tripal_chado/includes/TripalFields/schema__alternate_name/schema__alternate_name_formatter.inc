<?php

class schema__alternate_name_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Synonyms';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__alternate_name'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $list_items = [];

    foreach ($items as $delta => $item) {
      if (empty($item['value'])) {
        continue;
      }
      $list_items[] = $item['value'];
    }
    $list = 'There are no synonyms.';
    if (count($list_items) > 0) {
      $list = [
        'title' => '',
        'items' => $list_items,
        'type' => 'ul',
        'attributes' => [],
      ];
      $list = theme_item_list($list);
    }
    $element[0] = [
      '#type' => 'markup',
      '#markup' => $list,
    ];
  }
}
