<?php

class views_handler_argument_stockprop_id extends views_handler_argument_string {

	function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);   
    unset($form['glossary']);
    unset($form['limit']);
    unset($form['add_table']);
    unset($form['require_value']);
    
    //get options & display as options
	  $previous_db = db_set_active('chado');
    $result = db_query("SELECT cvt.cvterm_id as type_id, cvt.name FROM cvterm cvt WHERE cvt.cvterm_id IN (SELECT type_id FROM stockprop)");
    db_set_active($previous_db);
    $types = array();
    while ($r = db_fetch_object($result)) { $types[$r->type_id] = $r->name; }
		$form['stockprop_type_id'] = array(
			'#type' => 'radios',
			'#title' => t('Property Types'),
			'#options' => $types,
			'#default_value' => $this->options['stockprop_type_id'],
			'#required' => TRUE,
			'#description' => t('Select the type of property represented by this argument.'),
		);
		
		$operators = array(
      '=' => t('Is equal to'),
      '!=' => t('Is not equal to'),
      '~' => t('Contains'),
      '!~' => t('Does not contain'),
      'IS NOT NULL' => t('Is Present (Not Empty)'),
      'IS NULL' => t('Is Absent (Empty)'),		
		);
		$form['operator'] = array(
			'#type' => 'radios',
			'#title' => 'Operator',
			'#description' => t('Specify how to compare the argument with the property values.'),
			'#options' => $operators,
			'#default_value' => $this->options['operator'],
			'#required' => TRUE,
		);
  }
  
  /**
   * Build the query based upon the formula
   */
	function query() {
    $argument = $this->argument;
    if (!empty($this->options['transform_dash'])) {
      $argument = strtr($argument, '-', ' ');
    }
    
		if (preg_match('/IS NOT NULL/', $this->options['operator'])) {
			$new_where_sql = "stock_id IN (SELECT stock_id FROM stockprop WHERE type_id=".$this->options['stockprop_type_id'].")";
		} elseif (preg_match('/IS NULL/', $this->options['operator'])) {
			$new_where_sql = "stock_id NOT IN (SELECT stock_id FROM stockprop WHERE type_id=".$this->options['stockprop_type_id'].")";
		} else {
			$new_where_sql = "stock_id IN (SELECT stock_id FROM stockprop "
							. "WHERE type_id=".$this->options['stockprop_type_id']." AND value".$this->options['operator']."'".$argument."')";
		}
		$this->query->add_where($this->options['group'], $new_where_sql);
  }

}