<?php


/**
 * Extends the EntityFieldQuery to support queries from multiple storage types.
 */
class TripalFieldQuery extends EntityFieldQuery {

  protected $field_storage = array();

  public function execute() {

    // Give a chance to other modules to alter the query.
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
      $results = call_user_func($this->queryCallback(), $this);
    }

    if ($results and $this->count) {
      return $results;
    }
    else {
      return $results;
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
  public function queryStorageCallback($storage) {
    // Use the override from $this->executeCallback. It can be set either
    // while building the query, or using hook_entity_query_alter().
    if (function_exists($this->executeCallback)) {
      return $this->executeCallback;
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
