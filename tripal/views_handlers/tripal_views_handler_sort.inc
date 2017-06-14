<?php


/**
 * Base sort handler that has no options and performs a simple sort.
 *
 * @ingroup views_sort_handlers
 */
class tripal_views_handler_sort extends views_handler_sort {

  /**
   * Called to add the sort to a query.
   */
  function query() {
    $field_name = $this->real_field;
    $this->ensure_my_table();
    // Add the field.
    $this->query->add_orderby($this->table_alias, $this->real_field, $this->options['order']);
  }

}