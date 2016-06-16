<?php
/**
 * @file
* Contains tripal_views_handler_field_entity_status Filter Handler
*/

/**
 * This Handler provides a generic select list for any chado field that is a string
*  The select list includes all distinct values for that field.
*
* @ingroup tripal_views
*/
class tripal_views_handler_field_entity_default_formatter extends  views_handler_field {

  function render($values) {

    $field_name = $this->field;
    $row_index = $this->view->row_index;
    $result = $entity_id = $this->view->result[$row_index];
    $entity = $result->entity;

    return field_view_field('TripalEntity', $entity, $field_name);
    
  }
}