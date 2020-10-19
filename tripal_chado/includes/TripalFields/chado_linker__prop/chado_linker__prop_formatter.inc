<?php

class chado_linker__prop_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Chado Property';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__prop'];


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

    $list = [];
    foreach ($items as $index => $item) {
      $value = $item['value'];
      // any URLs are made into clickable links
      if ( preg_match('/^https?:/i', $value) ) {
        $value = l($value, $value, ['external' => 'TRUE', 'attributes' => ['target' => '_blank']]);
      }
      $list[$index] = $value;
    }

    // Also need to make sure to not return markup if the field is empty.
    if (empty($list)) {
      return;
    }

    // If more than one value has been found display all values in an unordered
    // list.
    if (count($list) > 1) {
      $content = theme_item_list([
        'items' => $list,
        'title' => '',
        'attributes' => [
          'class' => [
            $entity->bundle . '-properties-list',
            'properties-field-list',
          ],
        ],
        'type' => 'ul',
      ]);
    }
    else {
      $content = $list[0];
    }
    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
