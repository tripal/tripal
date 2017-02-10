<?php

class schema__alternate_name_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Synonyms';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('schema__alternate_name');

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $list_items = array();
    foreach ($items as $delta => $item) {
      $list_items[] = $item['value'];
    }
    $list = 'There are no synonyms.';
    if (count($list_items) > 1) {
      $list = array(
        'title' => '',
        'items' => $list_items,
        'type' => 'ol',
        'attributes' => array(),
      );
      $list = theme_item_list($list);
    }
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => $list,
    );
  }
}
