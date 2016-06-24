<?php


/**
 * A simple class for querying entities based on field values for fields.
 *
 * This class supports the use of multiple field storage. This class is loosely
 * modeled after the EntityFieldQuery class.
 *
 */
class TripalFieldQuery {

  protected $options = array();

  /**
   *
   * @param $field_name
   *   The name of the field.  Or, the name of the field with a subfield
   *   concatenated using a '.' as a delimter.
   * @param $value
   *   The value to use for filtering.
   * @param $operator
   *   The operation to apply: '=', '<>', '>', '>=', '<', '<=', 'STARTS_WITH',
   *   'CONTAINS': These operators expect $value to be a literal of the same
   *   type as the column. 'IN', 'NOT IN': These operators expect $value to
   *   be an array of literals of the same type as the column.
   */
  public function fieldCondition($field_name, $value, $operator = '=') {

    // See if there is a subfield as part of the field_name.
    $subfields = explode('.', $field_name);
    if ($subfields > 1) {
      $field = field_info_field($subfields[0]);
    }
    else {
      $field = field_info_field($field_name);
    }

    if ($field) {
      $field_storage_type = $field['storage']['type'];
      $this->options[$field_storage_type]['filters'][] = array(
        'field' => $field,
        'filter' => $field_name,
        'value' => $value,
        'operator' => $operator,
      );
    }
    return $this;
  }

  /**
   * Orders the result set by a given field column.
   *
   * @param $field_name
   * @param $direction
   *
   * @return TripalFieldQuery
   */
  public function fieldOrderBy($field_name, $direction = 'ASC') {

    // See if there is a subfield as part of the field_name.
    $subfields = explode('.', $field_name);
    if ($subfields > 1) {
      $field = field_info_field($subfields[0]);
    }
    else {
      $field = field_info_field($field_name);
    }

    if ($field) {
      $field_storage_type = $field['storage']['type'];
      $this->options[$field_storage_type]['sort'][] = array(
        'field' => $field,
        'orderBy' => $field_name,
        'direction' => $direction,
      );
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
    // Are there any filters?  If so, then let the field storage
    // systems handle the query. If there are no fields then just pull out
    // the list of entities.
    // dpm($this->filters);
    // dpm($this->sort);

    $entity_ids = array();
    if (count($this->options) > 0) {
      // Iterate through each of the field storage types and run their
      // tquery() function.
      foreach ($this->options as $field_storage_type => $option) {
        $filters = array_key_exists('filters', $option) ? $option['filters'] : array();
        $sort = array_key_exists('sort', $option) ? $option['sort'] : array();

        // Get the storage infor for the fields that belong to this type.
        // We can get it from the first field.
        if (count($filters) > 0) {
          $storage = $filters[0]['field']['storage'];
        }
        else {
          $storage = $sort[0]['field']['storage'];
        }
        $module = $storage['module'];
        $function_name = $module . '_field_storage_tquery';
        $filter_ids = array();
        if (function_exists($function_name)) {
          $filter_ids = $function_name($filters, $sort);
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

    // Order the entities by the field
    if (count($this->order) > 0) {

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
