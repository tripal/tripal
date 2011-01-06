<?php

class views_handler_field_organism_nid extends views_handler_field_numeric {
 function construct() {
   parent::construct();
   $this->additional_fields['organism_id'] = 'organism_id';   
 }

 function query() { 
   $this->add_additional_fields(); 
 }

 function render($values) { 
	return $values->nid;
 } 
} 