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

    // First add base => chado_base join
    $def = array();
    $def['table'] = 'chado_' . $this->table;
    $def['field'] = $this->field;
    $def['left_table'] = $this->table;
    $def['left_field'] = $this->field;
    //$def['handler'] = 'views_handler_join_chado_aggregator';

    $join = new views_join();

    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;

    $this->linker_alias = $this->query->add_relationship('stock_chado_stock', $join, $this->table);

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

    $this->alias = $this->query->add_relationship('chado_stock_node', $join, 'node');
  }
}