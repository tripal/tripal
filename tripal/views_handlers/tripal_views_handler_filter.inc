<?php

class tripal_views_handler_filter extends views_handler_filter {

  /**
   * Add this filter to the query.
   */
  function query() {
    $this->ensure_my_table();
    $this->query->add_where(
      $this->options['group'],
      $this->real_field,
      $this->value,
      $this->operator
    );
  }
}