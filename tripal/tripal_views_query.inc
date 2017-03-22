<?php

class tripal_views_query extends views_plugin_query {

  // The EntityFieldQuery object.
  var $query;

  var $fields;

  var $filters;

  /**
   *
   */
  public function init($base_table = 'tripal_entity', $base_field = 'id', $options) {
    parent::init($base_table, $base_field, $options);

    $this->fields = array();
    $this->where = array();

    // Creqte the TripalFieldQuery object.
    $this->query = new TripalFieldQuery();

    // Make suer we only query on the entities for this bundle type.
    $this->query->entityCondition('entity_type', 'TripalEntity');
    $this->query->entityCondition('bundle', $base_table);
  }
  /**
   *
   */
  public function add_field($table_alias, $field_name, $alias = '', $params = array()) {
    $this->fields[] = array(
      'table_alias' => $table_alias,
      'field_name' => $field_name,
      'alias' => $alias,
      'params' => $params
    );
  }

  /**
   *
   * @param $group
   * @param $field
   * @param $value
   * @param $operator
   */
  public function add_where($group, $field_name, $value = NULL, $operator = NULL) {
    // Remove the preceeding period from the $field_name
    $field_name = preg_replace('/^\./', '', $field_name);

    $this->filters[] = array(
      'group' => $group,
      'field_name' => $field_name,
      'value' => $value,
      'op' => $operator
    );
    if ($value) {
      $this->query->fieldCondition($field_name, $value, $value, $op);
    }
  }

  /**
   *
   * @param  $view
   */
  function execute(&$view) {
    $query = $this->query;

    $start = microtime(TRUE);

    // Execute the count query
    $cquery = clone $query;
    $cquery->count();
    $num_records = $cquery->execute();
    $views->total_rows = count($num_records['TripalEntity']);

    $results = $query->execute();

    // Iterate through the entities that were returned and get the field
    // values that are requested.  Those go in the view->result array.
    $i = 0;
    $view->result = array();
    foreach ($results['TripalEntity'] as $entity_id => $stub) {
      $entities = array($entity_id => $stub);
      $view->result[$i] = new stdClass();
      $view->result[$i]->entity = $stub;
      $view->result[$i]->entity_id = $entity_id;
      foreach ($this->fields as $details) {
        $field_name = $details['field_name'];
        $field = field_info_field($field_name);
        module_invoke($field['storage']['module'], 'field_storage_load', 'TripalEntity',
            $entities, FIELD_LOAD_CURRENT, array($field['id'] => array($entity_id)));
        $view->result[$i]->$field_name = $entities[$entity_id]->$field_name['und'][0]['value'];
      }
      $i++;
    }
    $view->execute_time = microtime(TRUE) - $start;
    $view->current_page = 0;
    dpm($view->query);
    dpm($view->result);
  }

  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   *
   * @return SelectQuery
   *   A SelectQuery object.
   */
  function query($get_count = FALSE) {
  }
}