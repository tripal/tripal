<?php

class schema__publication_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Publication';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('schema__publication');

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
    $chado_table = $this->instance['settings']['chado_table'];
    foreach ($items as $delta => $item) {
      
      $title = isset($item['value']['Title']) ? $item['value']['Title'] : '';
      $citation = isset($item['value']['Citation']) ? $item['value']['Citation'] : '';
      $entity = array_key_exists('entity', $item['value']) ? $item['value']['entity'] : '';
      if ($entity) {
        list($entity_type, $entity_id) = explode(':', $entity);
        $new_title = l($title, 'bio_data/' . $entity_id); 
        $citation = preg_replace("/$title/", $new_title, $citation);
      }
      $list_items[] = $citation;
    }

    krsort($list_items, SORT_NUMERIC);

    $list = array(
      'title' => '',
      'items' => $list_items,
      'type' => 'ol',
      'attributes' => array(),
    );

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_item_list($list),
      );
    }
  }
}
