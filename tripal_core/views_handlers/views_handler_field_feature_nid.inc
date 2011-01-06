<?php

class views_handler_field_feature_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
		$this->additional_fields['feature'] = 'feature_id';   
	}

	function query() { 
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->nid;
	}
} 