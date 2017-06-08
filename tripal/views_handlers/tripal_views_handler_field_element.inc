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

    $value = $this->get_value($values);

    // Handle single value fields:
    if (count($value == 1)) {
      return $this->sanitize_value($value[0]['value'][$element_name], 'xss');
    }
    return '';
  }
}
