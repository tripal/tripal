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
class tripal_views_handler_field_entity_status extends  views_handler_field {

  function render($values) {
    $value = $this->get_value($values);
    if ($value == 1) {
      return 'published';
    }
    else {
      return 'unpublished';
    }
  }
}
