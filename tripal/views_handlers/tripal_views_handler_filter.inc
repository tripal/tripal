<?php

class tripal_views_handler_filter extends views_handler_filter {

  /**
   * Add this filter to the query.
   *
   * Due to the nature of fapi, the value and the operator have an unintended
   * level of indirection. You will find them in $this->operator
   * and $this->value respectively.
   */
  function query() {
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], $this->real_field, $this->value, $this->operator);
  }

}