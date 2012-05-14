<?php

class views_handler_field_computed_organism_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
		$this->additional_fields['organism_id'] = array('table' => 'organism', 'field' => 'organism_id');
	}

	function query() { 
		$this->ensure_my_table();
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->organism_nid;
	}
} 