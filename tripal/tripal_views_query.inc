<?php

class tripal_views_query extends views_plugin_query_default {

  private $attach_fields = array();

  /**
   *
   */
  public function init($base_table = 'tripal_entity', $base_field = 'id', $options) {
    parent::init($base_table, $base_field, $options);

    // Always add the TripaEntity id and the bundle name so we can keep
    // track of which rows are in the SQL results
    $this->add_field('tripal_entity', 'id', 'tripal_entity_id');
    $this->add_field('tripal_bundle', 'name', 'tripal_bundle_name');

  }
  /**
   *
   */
  public function add_field($table_alias, $field, $alias = '', $params = array()) {
    // If this is not a TripalField then allow the add_field to work as usual.
    $f = field_info_field($field);
    if (!$f) {
      return parent::add_field($table_alias, $field, $alias, $params);
    }
    else {
      $this->attach_fields[] = $f;
    }
  }

  /**
   *
   * @param  $view
   */
  function execute(&$view) {

    // First execute any SQL that needs to run.
    parent::execute($view);

    // Add in the entity object to the results and any additional fields
    // that have been requested.
    for ($i = 0; $i < count($view->result); $i++) {

      // Build a simple entity stub (it's faster than trying to load the
      // whole things!).
      $entity_id = $view->result[$i]->tripal_entity_id;
      $bundle_name = $view->result[$i]->tripal_bundle_name;
      $ids = array($entity_id, 0, $bundle_name);
      $entity = entity_create_stub_entity('TripalEntity', $ids);

      // Iterate through the field and attach only those that have been
      // requested.
      foreach ($this->attach_fields as $field) {
        // Don't load this field if it doesn't belong to the selected bundle
        if (in_array($bundle_name, $field['bundles']['TripalEntity'])) {
          $entities = array($entity_id => $entity);
          module_invoke($field['storage']['module'], 'field_storage_load', 'TripalEntity',
            $entities, FIELD_LOAD_CURRENT, array($field['id'] => array($entity_id)));
        }
      }
      $view->result[$i]->entity = $entity;
    }
    return;

    // The base table indicates our content type.
    $base_table = $view->base_table;

    // Get the bundle that the view base table represents.
//    $bundle = tripal_load_bundle_entity(array('name' => $view->base_table));

    // The base table for the view is a bundle therefore the first condition
    // must be with the content_type field.
    $query = new TripalFieldQuery();
//    $query->entityCondition('entity_type', 'TripalEntity');
    $query->entityCondition('bundle', $bundle->name);

    // Apply filters
    foreach ($view->filter as $field_name => $handler) {
      if (trim($handler->value)) {
        $query->fieldCondition($field_name, $field_name, $handler->value, $handler->operator);
      }
    }
    // Apply sorting
    foreach ($view->sort as $field_name => $sort) {
      $options = $sort->options;
      $query->fieldOrderBy($field_name, $options['order']);
    }

    $results = $query->execute();

    if (count($results) > 0) {
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
    }

    $view->total_rows = count($view->result);
    $view->pager['current_page'] = 0;
  }
}