<?php

class chado_linker__prop_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Chado Property';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__prop');


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

    $content = array();
    // If $items is not empty and it has more than one value all values need to
    // be added to an array for display.
    if ($items) {
      if (count($items) > 1) {
        foreach ($items as $index => $item) {
          $content[$index] = $item['value'];
        }
      }
      else {
        $content = $items[0]['value'];
      }
    }
    // If more than one value has been found display all values in an unordered
    // list.
    if (count($content) > 1) {
      $bullets = '<ul>';
      $bullets .= '<li>';
      $bullets .= implode("</li><li>", $content);
      $bullets .= '</li>';
      $bullets .= '</ul>';
      $element[0] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $bullets,
      );
    }
    // If it's a single value field display without bullets.
    else {
      $element[0] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
    }

  }
}
