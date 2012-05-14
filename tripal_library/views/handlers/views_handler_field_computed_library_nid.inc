<?php

class views_handler_field_computed_library_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
		$this->additional_fields['library_id'] = array('table' => 'library', 'field' => 'library_id');
	}

	function query() { 
		$this->ensure_my_table();
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->library_nid;
	}
} 