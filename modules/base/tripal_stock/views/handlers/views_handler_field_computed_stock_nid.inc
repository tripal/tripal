<?php

/**
 * Adds the Node ID to for the current stock 
 *
 * This handler is only needed if chado is in a separate database and, therefore, the node tables 
 * cannot be joined to the stock tables
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
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