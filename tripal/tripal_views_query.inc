<?php

class tripal_views_query extends views_plugin_query {

  // The EntityFieldQuery object.
  var $query;

  var $fields;

  /**
   *
   */
  public function init($base_table = 'tripal_entity', $base_field = 'id', $options) {
    parent::init($base_table, $base_field, $options);

    $this->fields = array();

    $this->query = new EntityFieldQuery;
    $this->query->entityCondition('entity_type', 'TripalEntity');
  }
  /**
   *
   */
  public function add_field($table_alias, $field, $alias = '', $params = array()) {
    $this->fields[] = $field;
  }

  /**
   *
   * @param  $view
   */
  function execute(&$view) {
    dpm($view);
  }
}