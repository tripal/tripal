<?php

class tripal_views_query extends views_plugin_query {

  // The EntityFieldQuery object.
  var $query;

  var $fields;

  var $filters;


  /**
   * Ensure a table exists in the queue.
   *
   * This function overrides the views_plugin_query version of the function
   * but does nothing other than return the "table" (or bundle) name as
   * we won't be using aliases for bundles.
   *
   * @param $table
   *   The unaliased name of the table to ensure.
   * @param $relationship
   *   The relationship to ensure the table links to. Each relationship will
   *   get a unique instance of the table being added. If not specified, will
   *   be the primary table.
   * @param $join
   *   A views_join object (or derived object) to join the alias in.
   *
   * @return
   *   The alias used to refer to this specific table, or NULL if the table
   *   cannot be ensured.
   */
  public function ensure_table($table, $relationship = NULL, $join = NULL) {
    // Because we are not querying a table, we're querying a TripalFieldQuery
    // object we don't need to ensure the table.
    return $table;
  }
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
   * Add a simple WHERE clause to the query.
   *
   * @param $group
   *   The WHERE group to add these to; groups are used to create AND/OR
   *   sections. Groups cannot be nested. Use 0 as the default group. If the
   *   group does not yet exist it will be created as an AND group.
   * @param $field
   *   The name of the field to check.
   * @param $value
   *   The value to test the field against. In most cases, this is a scalar.
   *   For more complex options, it is an array. The meaning of each element
   *   in the array is dependent on the $operator.
   * @param $operator
   *   The comparison operator, such as =, <, or >=. It also accepts more
   *   complex options such as IN, LIKE, or BETWEEN. Defaults to IN if $value
   *   is an array = otherwise. If $field is a string you have to use 'formula'
   *   here.
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
      foreach ($this->fields as $details) {
        $field_name = $details['field_name'];
        $field = field_info_field($field_name);
        module_invoke($field['storage']['module'], 'field_storage_load', 'TripalEntity',
            $entities, FIELD_LOAD_CURRENT, array($field['id'] => array($entity_id)));
        $view->result[$i]->$field_name = $entities[$entity_id]->$field_name['und'][0]['value'];
      }
      $view->result[$i]->entity_id = $stub->id;
      $i++;
    }
    $view->execute_time = microtime(TRUE) - $start;
    $view->current_page = 0;
    //dpm($view->query);
    //dpm($view->result);
  }

}