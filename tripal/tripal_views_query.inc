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

    // Convert the $base_table into the bundle table.  Because every
    // tripal site will have different bundle tables we have to do the
    // conversion for cross-site compatibility.
    list($vocabulary, $accession) = explode('__', $base_table);
    $term = tripal_load_term_entity(array('vocabulary' => $vocabulary, 'accession' => $accession));
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));

    // Make sure we only query on the entities for this bundle type.
    $this->query->entityCondition('entity_type', 'TripalEntity');
    $this->query->entityCondition('bundle', $bundle->name);
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
    // TODO: support paging.
    $cquery = clone $query;
    $cquery->count();
    //$views->total_rows = $cquery->execute();

    // Get the IDs
    $results = $query->execute();
    $entity_ids = array_keys($results['TripalEntity']);


    // Get the fields to attach to the entity
    $fields = array();
    $field_ids = array();
    foreach ($this->fields as $details) {
      $field_name = $details['field_name'];
      $field = field_info_field($field_name);
      if ($field) {
        $fields[$field_name] = $field;
        $field_ids[] = $field['id'];
      }
    }

    $entities = tripal_load_entity('TripalEntity', $entity_ids, FALSE, $field_ids);
    $i = 0;
    foreach ($entities as $entity_id => $entity) {
      $view->result[$i] = new stdClass();
      foreach ($this->fields as $details) {
        $field_name = $details['field_name'];
        // The entity_id and link fields are not true fields. They are
        // added by the tripal_views_data_tripal_entity() function to provide
        // useful fields to reference entities. If we see these
        // we need to support them here by giving them values.
        if ($field_name == 'entity_id') {
          $view->result[$i]->$field_name = $entity;
          continue;
        }
        if ($field_name == 'link') {
          $view->result[$i]->$field_name = $entity;
          continue;
        }
        if ($field_name == 'edit_link') {
          $view->result[$i]->$field_name = $entity;
          continue;
        }
        if ($field_name == 'delete_link') {
          $view->result[$i]->$field_name = $entity;
          continue;
        }
        if ($field_name == 'status') {
          $view->result[$i]->$field_name = $entity->status;
          continue;
        }
        if (array_key_exists($field_name, $fields)) {
          $items = field_get_items('TripalEntity', $entity, $field_name);
          $view->result[$i]->$field_name = $items;
        }
      }
      $i++;
    }
    $view->execute_time = microtime(TRUE) - $start;
    $view->current_page = 0;
  }

}