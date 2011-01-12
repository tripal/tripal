<?php

class views_handler_field_computed_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
	}

	function query() { 
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->nid;
	}
} 