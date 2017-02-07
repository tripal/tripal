<?php

class sio__references_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'References';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sio__references');


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
    $field_name = $this->field['field_name'];
    $chado_table = $this->instance['settings']['chado_table'];

    // First, organize the values by their types.
    $ordered_items = array();
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
    $list_items = array();
    $headers = array();
    $rows = array();
    ksort($ordered_items);
    foreach ($ordered_items as $type => $children) {
      $rows[] = array(
        array(
          'data' => ucfirst($type) . '(s)',
          'header' => TRUE,
          'width' => '20%',
        ),
        theme_item_list(array(
          'items' => $children,
          'title' => '',
          'type' => 'ul',
          'attributes' => array(),
        )),
      );
    }

    $table = array(
      'header' => array(),
      'rows' => $rows,
      'attributes' => array(
        'id' => 'sio__references-table',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'There are no records in this site to which this publiation refers.',
    );
    $content = theme_table($table);
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}
