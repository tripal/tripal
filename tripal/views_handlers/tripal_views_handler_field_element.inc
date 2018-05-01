<?php
/**
 * @file
 *   Views field handler for basic TripalFields fields.
 */

/**
 * Views field handler for basic TripalFields fields.
 */
class tripal_views_handler_field_element extends tripal_views_handler_field {

  /**
   *
   */
  function query() {
    parent::query();
    // We need to add an alias to our TripalFields so that the
    // views can find the results.  With SQL it sets the alias for each
    // field and expects to find that alias in the results array.  Without
    // setting this alias Views can't find our results from our
    // tripal_views_query plugin.
    $this->field_alias = $this->real_field;
  }

  /**
   * Get the value that's supposed to be rendered.
   *
   * This api exists so that other modules can easy set the values of the field
   * without having the need to change the render method as well.
   *
   * @param $values
   *   An object containing all retrieved values.
   * @param $field
   *   Optional name of the field where the value is stored.
   */
  function get_value($values, $field = NULL) {
    $field_name = $this->field_alias;

    if (preg_match('/^(.+?)\.(.*)$/', $field_name, $matches)) {
      $field_name = $matches[1];
      $element_name = $matches[2];
    }
    if (isset($values->{$field_name})) {
      return $values->{$field_name};
    }
  }

  /**
   * Render the field.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  function render($values) {
    $field_name = $this->field_alias;
    $element_name = $field_name;

    if (preg_match('/^(.+?)\.(.*)$/', $field_name, $matches)) {
      $field_name = $matches[1];
      $element_name = $matches[2];
      // Conver the element name back to it's correct format with the colon.
      $element_name = preg_replace('/__/', ':', $element_name);
    }

    // Get the items for this field.
    $items = $this->get_value($values);

    // Is this a nested element? If not make sure we have an array of element
    // names to make it easier to deal with the name below.
    $elements = explode('.', $element_name);
    if (count($elements) == 0) {
      $elements[] = $element_name;
    }

    // Handle single value fields:
    if (count($items) == 0) {
      return '';
    }
    else {
      if (count($items) == 1) {
        $item = $items[0];
        $element = array_shift($elements);
        if (is_array($item['value'][$element])) {
          return $this->_get_element_value($elements, $item['value'][$element]);
        }
        else {
          return $this->sanitize_value($item['value'][$element_name], 'xss');
        }
      }
      else {
        if (count($items) <= 10) {
          $element = array_shift($elements);
          $element_values = [];
          foreach ($items as $index => $item) {
            if (is_array($item['value'][$element])) {
              $element_values[] = $this->_get_element_value($elements, $item['value'][$element]);
            }
            else {
              $element_values[] = $this->sanitize_value($item['value'][$element], 'xss');
            }
          }
          // TODO: theming this way should probably be handled by a sepcial
          // field handler that the user can tweak. But for now we'll just do this.
          return theme_item_list([
            'items' => $element_values,
            'title' => NULL,
            'type' => 'ul',
            'attributes' => [],
          ]);
        }
        else {
          return t('Too many values to show.');
        }
      }
    }
  }

  /**
   * A recursive function for retrieving a nested element value.
   */
  private function _get_element_value($elements, $item_values) {
    $element = array_shift($elements);
    if (is_array($item_values[$element])) {
      return $this->_get_element_value($elements, $item_values[$element]);
    }
    else {
      return $this->sanitize_value($item_values[$element], 'xss');
    }
  }
}
