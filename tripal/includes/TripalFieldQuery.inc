<?php


/**
 * A simple class for querying entities based on field values for fields.
 *
 * This class supports the use of multiple field storage. This class is loosely
 * modeled after the EntityFieldQuery class.
 *
 */
class TripalFieldQuery {


  // A list of the field storage instances for the fields in the filter.
  protected $field_storage = array();
  // The order in which the field storage execute() function should be
  // called.
  protected $fs_order = array();
  // An associative array of the filters to apply.
  protected $conditions = array();

  /**
   *
   * @param $field_name
   *   The name of the field.
   * @param $value
   *   The value to use for filtering.
   * @param $operator
   *   The operation to apply: '=', '<>', '>', '>=', '<', '<=', 'STARTS_WITH',
   *   'CONTAINS': These operators expect $value to be a literal of the same
   *   type as the column. 'IN', 'NOT IN': These operators expect $value to
   *   be an array of literals of the same type as the column.
   */
  public function fieldCondition($field_name, $value, $operator = '=') {
    $field = field_info_field($field_name);
    if ($field) {
      $field_storage_type = $field['storage']['type'];
      $this->conditions[$field_storage_type][] = array(
        'field' => $field,
        'value' => $value,
        'operator' => $operator,
      );
      if (!array_key_exists($field_storage_type, $this->field_storage)) {
        $this->field_storage[$field_storage_type] = $field['storage'];
        $this->fs_order[] = $field_storage_type;
      }
    }
    return $this;
  }

  /**
   * Executes the query and returns results.
   *
   * This function does not actually perform any queries itself but passes
   * on the task to field storage backends for all of the fields in the
   * filter.  Each backend should return a list of entity IDs that match
   * the filters provided. The intersection of this list is returned.
   *
   * @return
   *  An array of associative arrays of stub entities. The result can be
   *  used in the same way that results from the EntityFieldQuery->execute()
   *  function are used.
   */
  public function execute() {
    // Are there any conditions?  If so, then let the field storage
    // systems handle the query. If there are no fields then just pull out
    // the list of entities.
    $entity_ids = array();
    if (count($this->conditions) > 0) {
      // Iterate through each of the field storage types and run their
      // tquery() function.
      foreach ($this->fs_order as $field_storage_type) {
        $storage = $this->field_storage[$field_storage_type];
        $module = $storage['module'];
        $function_name = $module . '_field_storage_tquery';
        $filter_ids = array();
        if (function_exists($function_name)) {
          $filter_ids = $function_name($this->conditions[$field_storage_type]);
        }
        // Take the intersection of IDs in this filter with those in the
        // final $entity_ids;
        if (count($entity_ids) == 0) {
          $entity_ids = $filter_ids;
        }
        else {
          $entity_ids = array_intersect($entity_ids, $filter_ids);
        }
      }
    }
    else {
      $query = db_select('tripal_entity', 'td');
      $query->fields('td', array('id'));
      $query->orderBy('created', 'DESC');
      $query->range(0,25);
      $results = $query->execute();
      while ($entity_id = $results->fetchField()) {
        $entity_ids[] = $entity_id;
      }
    }

    // Generate the entities for the keys.
    $return = array();
    foreach ($entity_ids as $entity_id) {
      $entity = entity_create_stub_entity('TripalEntity', array($entity_id, NULL, NULL));
      $return['TripalEntity'][$entity_id] = $entity;
    }
    return $return;
  }
}
