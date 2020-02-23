<?php


/**
 * Extends the EntityFieldQuery to support queries from multiple storage types.
 */
class TripalFieldQuery extends EntityFieldQuery {

  /**
   * Holds a list of fields that should be included in the results
   */
  protected $includes = [];

  /**
   * These member variables keep track of join relationships with the
   * tripal_entity table. This is useful for non Tripal Storage API fields that
   * want to filter based on on other Drupal tables. The most important
   * example for this would be Drupal Views.  These variables are only meant
   * to be used by the tripal_field_storage_query() function as that is the
   * only storage system that should be doing quries on the tripal_entity
   * table itself.
   */
  public $relationships = [];

  public $relationshipConditions = [];

  /**
   * Adds a relationsihp via a table join to the tripal_entity table
   *
   * This is specific for Drupal schema tables and is useful for Views
   * integration when non Tripal Storage API fields are attached to an entity.
   *
   * @param $table
   *   The name of the table to join.
   * @param $alias
   *   The alias for the table.
   * @param $field
   *   The field to join on.
   */
  public function addRelationship($table, $alias, $field) {
    $this->relationships[$alias] = [
      'table' => $table,
      'field' => $field,
    ];
  }

  /**
   * Adds a where statement to a relationship.
   *
   * The relationship is added by the $table
   */
  public function relationshipCondition($table, $field, $value, $op) {

    $table_alias = '';
    // Get the alias for this table.
    foreach ($this->relationships as $alias => $details) {
      if ($details['table'] == $table) {
        $table_alias = $alias;
      }
    }

    if ($table_alias) {
      $this->relationshipConditions[$table_alias] = [
        'field' => $field,
        'value' => $value,
        'op' => $op,
      ];
    }
  }

  /**
   * Overrides the EntityFieldQuery::execute() function.
   */
  public function execute() {
    // Initialize the results array.
    $results = ['first_results' => TRUE];

    // Give a chance for other modules to alter the query.
    drupal_alter('entity_query', $this);
    $this->altered = TRUE;

    // Initialize the pager.
    $this->initializePager();

    // If there are fields then we need to support multiple backends, call
    // the function for each one and merge the results.
    if ($this->fields) {

      // Build the list of all of the different field storage types used
      // for this query.
      foreach ($this->fields as $field) {
        $this->field_storage[$field['storage']['type']] = $field['storage']['module'];
      }

      // Iterate through the field storage types and call each one.
      foreach ($this->field_storage as $storage_type => $storage_module) {
        $callback = $this->queryStorageCallback($storage_module);
        $st_results = call_user_func($callback, $this);
        $results = $this->_intersectResults($results, $st_results);
      }
    }

    // If we have relationships, then handle those.
    if (!empty($this->relationshipConditions)) {
      $st_results = tripal_field_storage_query($this);
      $results = $this->_intersectResults($results, $st_results);

      // Are there any other callbacks that need running other than
      // propertyQuery as we would have run those with the relationships.
      $callback = $this->queryCallback();
      if ($callback and $callback[1] != 'propertyQuery' and
        $callback[1] != 'tripal_field_storage_query') {
        $st_results = call_user_func($callback, $this);
        $results = $this->_intersectResults($results, $st_results);
      }
    }
    // There are no fields or relationships so just use the default 
    // callback for the query.
    else {
      if (!$this->fields) {
        $callback = $this->queryCallback();
        $st_results = call_user_func($callback, $this);
        $results = $this->_intersectResults($results, $st_results);
      }
    }

    if ($this->count == TRUE) {
      if (is_numeric($results)) {
        return $results;
      }
      if (array_key_exists('TripalEntity', $results)) {
        return count($results['TripalEntity']);
      }
      return 0;
    }
    return $results;
  }

  /**
   * Generates an intersection of results from different storage back-ends.
   */
  protected function _intersectResults($current_results, $new_results) {

    // If we currently don't have any results then just allow all through.
    // This is the first result set.
    if (array_key_exists('first_results', $current_results)) {
      return $new_results;
    }

    // set defaults to prevent warnings
    if (empty($new_results)) {
      $new_results['TripalEntity'] = [];
    }
    if (empty($current_results)) {
      $current_results['TripalEntity'] = [];
    }

    // Iterate through all of the new results and only include those that
    // exist in both the current and new.
    $intersection = [];
    foreach ($new_results['TripalEntity'] as $entity_id => $stub) {
      if (array_key_exists($entity_id, $current_results['TripalEntity'])) {
        $intersection[$entity_id] = $stub;
      }
    }
    if (count($intersection) > 0) {
      return ['TripalEntity' => $intersection];
    }
    else {
      return [];
    }
  }

  /**
   * Overides the EntityFieldQuery::queryCallback function.
   */
  public function queryCallback() {

    // Use the override from $this->executeCallback. It can be set either
    // while building the query, or using hook_entity_query_alter().
    if (function_exists($this->executeCallback)) {
      return $this->executeCallback;
    }

    // If there are no field conditions and sorts, and no execute callback
    // then we default to querying entity tables in SQL.
    if (empty($this->fields)) {
      return [
        $this,
        'propertyQuery',
      ];
    }

    // If no override, find the storage engine to be used.
    foreach ($this->fields as $field) {
      if (!isset($storage)) {
        $storage = $field['storage']['module'];
      }
      elseif ($storage != $field['storage']['module']) {
        throw new EntityFieldQueryException(t("Can't handle more than one field storage engine"));
      }
    }
    if ($storage) {

      // Use hook_field_storage_query() from the field storage.
      return $storage . '_field_storage_query';
    }
    else {
      throw new EntityFieldQueryException(t("Field storage engine not found."));
    }
  }

  /**
   * Determines the query callback to use for this entity query.
   *
   * This function is a replacement for queryCallback() from the
   * parent EntityFieldQuery class because that class only allows a single
   * storage type per query.
   *
   * @param $storage
   *   The storage module
   *
   * @throws EntityFieldQueryException
   * @return
   *   A callback that can be used with call_user_func().
   *
   */
  protected function queryStorageCallback($storage) {

    // Use the override from $this->executeCallback. It can be set either
    // while building the query, or using hook_entity_query_alter().
    if (function_exists($this->executeCallback)) {
      return $this->executeCallback;
    }

    // If there are no field conditions and sorts, and no execute callback
    // then we default to querying entity tables in SQL.
    if (empty($this->fields)) {
      return [$this, 'propertyQuery'];
    }

    if ($storage) {
      // Use hook_field_storage_query() from the field storage.
      return $storage . '_field_storage_query';
    }
    else {
      throw new EntityFieldQueryException(t("Field storage engine not found."));
    }
  }

  /**
   * Allows query object to be dumped as a string.
   */
  public function __toString() {
    return print_r($this, TRUE);
  }
}
