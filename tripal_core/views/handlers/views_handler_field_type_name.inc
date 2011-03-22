<?php

/**
 *
 * @ingroup views_field_handlers
 * @ingroup tripal_core
 */
class views_handler_field_type_name extends views_handler_field {
 function construct() {
   parent::construct();
 }

 function render($values) { 
   return $values->property_type_name;
 } 
} 