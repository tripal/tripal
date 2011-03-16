<?php

class views_handler_field_computed_stock_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
		$this->additional_fields['stock_id'] = array('table' => 'stock', 'field' => 'stock_id');
	}

	function query() { 
		$this->ensure_my_table();
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->stock_nid;
	}
} 