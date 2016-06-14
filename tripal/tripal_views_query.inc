<?php
/**
 * @file
 * Views query plugin for Apache Solr Views.
 * Gets its data not from the database, but from a Solr server.
 */

class tripal_views_query extends views_plugin_query {

  public function add_field($table_alias, $field, $alias = '', $params = array()) {
    dpm($field);
    // Make sure an alias is assigned.
    $alias = $alias ? $alias : $field;
    return $alias;
  }

  /**
   * We don't support construction of SQL queries.
   * @param $get_count
   */
  function query($get_count = FALSE) {

  }
  /**
   *
   * @param  $view
   */
  function execute(&$view) {
    // The base table indicates our content type.
    $base_table = $view->base_table;

    dpm($view);
    $row = new stdClass;
    $row->content_type = 10;
    $row->organism__comment = 1000;
    $view->result[] = $row;
    $view->total_rows = 1;
    $view->pager['current_page'] = 0;
  }
}