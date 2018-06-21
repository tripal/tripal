<?php


/**
 * Extends the EntityFieldQuery to support queries from multiple storage types.
 */
class TripalFieldQuery extends EntityFieldQuery {

  /**
   * Holds a list of fields that should be included in the results
   */
  protected $includes = array();

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
    
    $this->relationshipConditions[$table_alias] = [
      'field' => $field,
      'value' => $value,
      'op' => $op,      
    ];
  }

  /**
   * Overrides the EntityFieldQuery::execute() function.
   */
  public function execute() {
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

      // Initialize the results array.
      $results = array();

      // Iterate through the field storage types and call each one.
      foreach ($this->field_storage as $storage_type => $storage_module) {
        // Execute the query using the correct callback.
        $callback = $this->queryStorageCallback($storage_module);

        $st_results = call_user_func($callback, $this);
        // If this is the first storage type to be queries then save these
        // as the current results list.
        if (count($results) == 0) {
          $results = $st_results;
        }
        // If other results already exist then we want to find the intersection
        // of the two and only save those.
        else {
          $intersection = array(
            'TripalEntity' => array(),
          );
          foreach ($st_results['TripalEntity'] as $entity_id => $stub) {
            if (array_key_exists($entity_id, $results['TripalEntity'])) {
              $intersection['TripalEntity'][$entity_id] = $stub;
            }
          }
          $results = $intersection;
        }
      }
    }
    // If there are no fields then default to the original
    // EntityFieldQuery() functionality.
    else {
      $callback = $this->queryCallback();
      $results = call_user_func($callback, $this);
    }

    // If this is a count query then it should return a numeric value.
    if ($results and $this->count) {
      if (!is_numeric($results)) {
        throw new Exception('Query callback function did not provide a numeric count value: ' . $this->queryCallback());
      }
      return $results;
    }
    else {
      return $results;
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
    
    // If we have relationships then we need to join on the tripal_entity
    // table and that always occurs using the tripal_field_storage_query.
    if (!empty($this->relationships)) {
      return 'tripal_field_storage_query';
    }
    // If there are no field conditions and sorts, and no execute callback
    // then we default to querying entity tables in SQL.
    if (empty($this->fields)) {
      return array(
        $this,
        'propertyQuery',
      );
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
    
    // If we have relationships then we need to join on the tripal_entity
    // table and that always occurs using the tripal_field_storage_query.
    if (!empty($this->relationships)) {
      return 'tripal_field_storage_query';
    }
    
    // If there are no field conditions and sorts, and no execute callback
    // then we default to querying entity tables in SQL.
    if (empty($this->fields)) {
      return array($this, 'propertyQuery');
    }

    if ($storage) {
      // Use hook_field_storage_query() from the field storage.
      return $storage . '_field_storage_query';
    }
    else {
      throw new EntityFieldQueryException(t("Field storage engine not found."));
    }
  }
}
