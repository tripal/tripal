<?php

/**
 *
 *
 * @ingroup tripal_feature
 * @ingroup views_field_handlers
 */
class views_handler_field_computed_feature_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
		$this->additional_fields['feature_id'] = array('table' => 'feature', 'field' => 'feature_id');
	}

	function query() { 
		$this->ensure_my_table();
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->feature_nid;
	}
} 
