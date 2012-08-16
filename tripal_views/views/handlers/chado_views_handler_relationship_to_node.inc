<?php

/**
 * Relationship handler that links a chado table to it's nodes by creating 2 joins.
 *
 * NOTE: This handler assumes if you are linking TABLEA to it's nodes that there is a
 *  table named chado_TABLEA in the drupal schema with at least the following two fields:
 *  nid and TABLEA_id.
 *
 * Definition items:
 * - base: The new base table this relationship will be adding. This does not
 *   have to be a declared base table, but if there are no tables that
 *   utilize this base table, it won't be very effective.
 * - base field: The field to use in the relationship; if left out this will be
 *   assumed to be the primary field.
 * - label: The default label to provide for this relationship, which is
 *   shown in parentheses next to any field/sort/filter/argument that uses
 *   the relationship.
 */
class chado_views_handler_relationship_to_node extends views_handler_relationship {

  function query() {

    $this->ensure_my_table();

    // The base table in chado
    $base_table = $this->definition['base table'];
    $base_field = $this->definition['base field'];

    // The drupal linking table
    $chado_table = 'chado_' . $base_table;

    // Need to remove the incorrectly added node & chado_table join
    unset($this->query->table_queue['node'],
      $this->query->tables['stock']['node'],
      $this->query->table_queue[$chado_table],
      $this->query->tables['stock'][$chado_table]);

    // First add base => chado_base join
    $def = array();
    $def['table'] = $chado_table;
    $def['field'] = $base_field;
    $def['left_table'] = $base_table;
    $def['left_field'] = $base_field;
    //$def['handler'] = 'views_handler_join_chado_aggregator';

    $join = new views_join();

    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;

    $alias = $base_table . '_' . $chado_table;
    $this->linker_alias = $this->query->add_relationship($alias, $join, $this->table);

    // Now add chado_base => node join
    $def = array();
    $def['table'] = 'node';
    $def['field'] = 'nid';
    $def['left_table'] = $this->linker_alias;
    $def['left_field'] = 'nid';
    //$def['handler'] = 'views_handler_join_chado_aggregator';

    $join = new views_join();

    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;

    if ($base_table == $this->view->base_table) {
      $alias = 'node';
    }
    else {
      $alias = $chado_table . '_node';
    }
    $this->alias = $this->query->add_relationship($alias, $join, 'node');
  }
}