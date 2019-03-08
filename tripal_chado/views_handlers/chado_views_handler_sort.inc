<?php
/**
 * Views handlers providing sort support for chado fields.
 */

/**
 * Adds support for generic sorting.
 */
class chado_views_handler_sort extends views_handler_sort {

  /**
   * {@inheritdoc}
   */
  function query() {

    // Adds joins to chado_entity and the chado table this field is from.
    $alias = _chado_views_add_table_joins($this);

    // Add the sort.
    $this->query->add_orderby($alias, $this->definition['chado_field'], $this->options['order']);
  }
}
