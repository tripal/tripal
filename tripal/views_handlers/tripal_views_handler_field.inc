<?php
/**
 * @file
 *   Views field handler for basic TripalFields fields.
 */

/**
 * Views field handler for basic TripalFields fields.
 */
class tripal_views_handler_field extends views_handler_field {

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
   * Overrides click_sort().
   */
  function click_sort($order) {
    if (isset($this->field_alias)) {
      // Since fields should always have themselves already added, just
      // add a sort on the field.
      $params = $this->options['group_type'] != 'group' ? ['function' => $this->options['group_type']] : [];
      $this->query->add_orderby($this->table_alias, $this->real_field, $order, $this->field_alias, $params);
    }
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

    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;

    // For some reason the alias isn't being added to the $this->aliases
    // variable when the user clicks the checkbox 'Link this field to the
    // original piece of content'.  That may need to be investigated and is
    // probably a side effect of using the  views_handler_field as a parent
    // class that is expecting something more for SQL. We don't use aliases
    // for fields so the following if statement will fix the problem.
    if (!$alias) {
      $alias = $this->field_alias;
    }
    if (isset($values->{$alias})) {
      return $values->{$alias};
    }
  }

  /**
   * Render the field.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  function render($values) {

    list ($vocabulary, $accession) = explode('__', $this->table_alias);
    $term = tripal_load_term_entity([
      'vocabulary' => $vocabulary,
      'accession' => $accession,
    ]);
    $bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
    $field_name = $this->field_alias;
    $field = field_info_field($field_name);
    $instance = field_info_instance('TripalEntity', $field_name, $bundle->name);
    $entity = $values->entity;

    $items = $this->get_value($values);

    // Handle single value fields:
    if (count($items) == 1) {
      $function = $field['module'] . '_field_formatter_view';
      $value = $function('TripalEntity', $entity, $field, $instance, 'und', $items, $instance['display']['default']);
      return $value;
    }
    return '';
  }
}
