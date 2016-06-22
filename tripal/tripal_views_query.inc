<?php
/**
 * @file
 * Views query plugin for Apache Solr Views.
 * Gets its data not from the database, but from a Solr server.
 */

class tripal_views_query extends views_plugin_query {

  public function add_field($table_alias, $field, $alias = '', $params = array()) {
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

    // Get the bundle that the view base table represents.
    $bundle = tripal_load_bundle_entity(array('name' => $view->base_table));

    // The base table for the view is a bundle therefore the first condition
    // must be with the content_type field.
    $query = new TripalFieldQuery();
    $query->fieldCondition('content_type', $bundle->id);
    foreach ($view->filter as $field_name => $handler) {
      $query->fieldCondition($field_name, $handler->value, $handler->operator);
    }

    $results = $query->execute();
    foreach ($results['TripalEntity'] as $entity_id => $stub) {
      // Begin a new row for Views output.
      $row = new stdClass;

      // Get the entity object.
      $entity = tripal_load_entity('TripalEntity', array('id' => $entity_id));
      $entity = reset($entity);

      // Iterate through the fields that are added to the view and attach those
      // to the entity.  After attaching we can get the value and include
      // it in the output results.
      foreach($view->field as $field_name => $handler) {
        $field = field_info_field($field_name);
        field_attach_load($entity->type, array($entity->id => $entity), FIELD_LOAD_CURRENT,
            array('field_id' => $field['id']));
        $items = field_get_items('TripalEntity', $entity, $field_name);
        $value = $items[0]['value'];

        $row->entity = $entity;
        $row->$field_name = $value;
      }

      // Add the row to the results list.
      $view->result[] = $row;

    }

    $view->total_rows = count($view->result);
    $view->pager['current_page'] = 0;
  }
}