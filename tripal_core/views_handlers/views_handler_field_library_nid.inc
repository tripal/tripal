<?php

class views_handler_field_library_nid extends views_handler_field_numeric {
 function construct() {
   parent::construct();
   $this->additional_fields['library_id'] = 'library_id';   
 }

 function query() { 
   $this->add_additional_fields(); 
 }

 function render($values) { 
	return $values->nid;
 } 
} 