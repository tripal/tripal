<?php

/**
 *
 *
 * @ingroup tripal_analysis
 */
class views_handler_field_computed_analysis_nid extends views_handler_field_numeric {
	function construct() {
		parent::construct();
		drupal_set_message('Computed Analysis NID Handler loaded');
		$this->additional_fields['analysis_id'] = array('table' => 'analysis', 'field' => 'analysis_id');
	}

	function query() { 
		$this->ensure_my_table();
		$this->add_additional_fields(); 
	}

	function render($values) { 
		return $values->analysis_nid;
	}
} 