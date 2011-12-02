<?php

class views_handler_filter_stock_relationship_id extends views_handler_filter {

	function query () {
	  if ($this->value) {
	  
      // get variable position
      if ($this->options['fixed_position'] == 'subject_id') {
        $this->options['variable_position'] = 'object_id';
      } else {
        $this->options['variable_position'] = 'subject_id';
      }
        
	    // determine if just checking presence
	    if (preg_match('/NULL/', $this->operator)) {
          $where = 'stock.stock_id IN ('
            .'SELECT '.$this->options['variable_position'].' FROM stock_relationship '
            .'WHERE '.$this->options['fixed_position'].' '.$this->operator.' AND type_id = '.$this->options['type']
          .')';
          $this->query->add_where($this->options['group'], $where);	      
	    } else {
        
        // determine whether regex was requested
        if ($this->operator == '~') {
          $search_options = array('regex_columns' => array('name','uniquename'));
          $this->operator = '=';
        } elseif ($this->operator == '!~') {
          $search_options = array('regex_columns' => array('name','uniquename'));
          $this->operator = '!=';
        } else {
          $search_options = array();
        }
        
        // get fixed stock id(s)
        $fixed_stock = tripal_core_chado_select('stock',array('stock_id'),array('uniquename' => $this->value), $search_options);
        $fixed_stock_ids = array();
        if ($fixed_stock[0]->stock_id) {
          $fixed_stock_ids[] = $fixed_stock[0]->stock_id;
        } else {
          $fixed_stock = tripal_core_chado_select('stock',array('stock_id'),array('name' => $this->value), $search_options);
          if (sizeof($fixed_stock) > 1) {
            foreach ($fixed_stock as $s) {
              $fixed_stock_ids[] = $s->stock_id;
            }
          } elseif (sizeof($fixed_stock) == 1) {
            $fixed_stock_ids[] = $fixed_stock[0]->stock_id;
          }
        }
        
        // determine operator
        if ($this->operator == '=' && sizeof($fixed_stock_ids) > 1) {
          $this->operator = 'IN';
        } elseif ($this->operator == '!=' && sizeof($fixed_stock_ids) > 1) {
          $this->operator = 'NOT IN';
        }
        
        //generate where
        if (sizeof($fixed_stock_ids) == 1) {
          $where = 'stock.stock_id IN ('
            .'SELECT '.$this->options['variable_position'].' FROM stock_relationship '
            .'WHERE '.$this->options['fixed_position'].' '.$this->operator.' '.$fixed_stock_ids[0].' AND type_id = '.$this->options['type']
          .')';
          $this->query->add_where($this->options['group'], $where);
        } elseif (sizeof($fixed_stock_ids) > 1) {
          $where = 'stock.stock_id IN ('
            .'SELECT '.$this->options['variable_position'].' FROM stock_relationship '
            .'WHERE '.$this->options['fixed_position'].' '.$this->operator.' ('.implode(', ',$fixed_stock_ids).') AND type_id = '.$this->options['type']
          .')';
          $this->query->add_where($this->options['group'], $where);
        } else {
          drupal_set_message('No stock with the name or uniquename '.$this->value.' was found -No filtering done.','error');
        }
      } //end of not just checking presence
    }
	}
	
  function options_form(&$form, &$form_state) {
  	
    if ($this->can_expose()) {
      $this->show_expose_button($form, $form_state);
    }

  	$instructions = 'This filter allows you to enter a stock, the position of that stock in the '
  		.'relationship and relationship type and only stocks with a relationship of the type described '
  		.'will be shown. For example, if you want all stocks where Fred is_paternal_parent_of, then '
  		.'you would enter Fred as the value, select is_paternal_parent_of as the Relationship Type '
  		.'and subject as the Fixed Position.';
  	$form['instructions'] = array(
  		'#type' => 'item',
  		'#value' => t($instructions)
  	);
  	
    $form['op_val_start'] = array('#value' => '<div class="clear-block">');

    // left side
    
    $this->types_form($form, $form_state);
		$form['type']['#prefix'] = '<div class="views-left-50">';
		$form['type']['#suffix'] = '</div>';

    $this->show_operator_form($form, $form_state);
    $form['operator']['#prefix'] = '<div class="views-right-50">';
		$form['operator']['#suffix'] = '</div>';
		
		$this->show_value_form($form, $form_state);
		$form['value']['#prefix'] = '<div class="views-right-50">';
		$form['value']['#suffix'] = '</div>';
		
    // right side


    $this->fixed_position_form($form, $form_state);
    $form['fixed_position']['#prefix'] = '<div class="views-right-50">';
    $form['fixed_position']['#suffix'] = '</div>';

		
    if ($this->can_expose()) {
      $this->show_expose_form($form, $form_state);
    }
    
    $form['op_val_start'] = array('#value' => '</div>');

  }

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

		$form['value'] = array(
			'#type' => 'textfield',
			'#title' => ($this->options['label']) ? $this->options['label'] : t('Stock Name'),
			'#default_value' => $this->value,
			'#size' => 40,
		);
    	
  }

  function type_options () {

    $previous_db = tripal_db_set_active('chado');
    $result = db_query("SELECT cvt.cvterm_id as type_id, cvt.name FROM cvterm cvt WHERE cvt.cvterm_id IN (SELECT type_id FROM stock_relationship)");
    tripal_db_set_active($previous_db);

    $types = array();
    while ($r = db_fetch_object($result)) { $types[$r->type_id] = $r->name; }
    
    return $types;
  }
  
  function types_form(&$form, &$form_state) {
  	
    $form['type'] = array(
      '#type' => count($options) < 10 ? 'radios' : 'select',
      '#title' => t('Relationship Types'),
      '#options' => $this->type_options(),
      '#default_value' => $this->options['type'],
    );

  }
  
  function fixed_position_form (&$form, &$form_state) {
  
  	$form['fixed_position'] = array(
  		'#type' => 'radios',
  		'#title' => 'Position of Stock to Filter on',
  		'#description' => t('Where a relationship consists of a subject type object, this field indicates '
  			.'what position in the relationship remains fixed (is the stock entered as the value).'),
  		'#options' => array(
  			'subject_id' => 'Subject',
  			'object_id' => 'Object'
  		),
  		'#default_value' => ($this->options['fixed_position']) ? $this->options['fixed_position'] : 'subject_id',
  	);
  	
  }

  function operator_options() {
    return array(
      '=' => t('Is equal to'),
      '!=' => t('Is not equal to'),
      '~' => t('Contains'),
      '!~' => t('Does not contain'),
      'IS NOT NULL' => t('Is Present (Not Empty)'),
      'IS NULL' => t('Is Absent (Empty)'),
    );
  }

 /**
  * Render our chunk of the exposed filter form when selecting
  */
  function exposed_form(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }

    if (!empty($this->options['expose']['use_operator']) && !empty($this->options['expose']['operator'])) {
      $operator = $this->options['expose']['operator'];
      $this->operator_form($form, $form_state);
      $form[$operator] = $form['operator'];

      if (isset($form[$operator]['#title'])) {
        unset($form[$operator]['#title']);
      }

      $this->exposed_translate($form[$operator], 'operator');

      unset($form['operator']);
    }

    if (!empty($this->options['expose']['identifier'])) {
      $value = $this->options['expose']['identifier'];
      $this->value_form($form, $form_state);
      $form[$value] = $form['value'];

      if (isset($form[$value]['#title']) && !empty($form[$value]['#type']) && $form[$value]['#type'] != 'checkbox') {
        unset($form[$value]['#title']);
      }

      $this->exposed_translate($form[$value], 'value');

      if (!empty($form['#type']) && ($form['#type'] == 'checkboxes' || ($form['#type'] == 'select' && !empty($form['#multiple'])))) {
        unset($form[$value]['#default_value']);
      }

      if (!empty($form['#type']) && $form['#type'] == 'select' && empty($form['#multiple'])) {
        $form[$value]['#default_value'] = 'All';
      }

      if ($value != 'value') {
        unset($form['value']);
      }
    }
  }

  function expose_form_left(&$form, &$form_state) {
    $form['expose']['label'] = array(
      '#type' => 'textfield',
      '#default_value' => $this->options['expose']['label'],
      '#title' => t('Label'),
      '#size' => 40,
    );

    $form['expose']['identifier'] = array(
      '#type' => 'textfield',
      '#default_value' => $this->options['expose']['identifier'],
      '#title' => t('Filter identifier'),
      '#size' => 40,
      '#description' => t('This will appear in the URL after the ? to identify this filter. Cannot be blank.'),
    );
    
  }

  function expose_form_right(&$form, &$form_state) {

    $form['expose']['identifier'] = array(
      '#type' => 'textfield',
      '#default_value' => $this->options['expose']['identifier'],
      '#title' => t('Filter identifier'),
      '#size' => 40,
      '#description' => t('This will appear in the URL after the ? to identify this filter. Cannot be blank.'),
    );

    if (!empty($form['operator']['#type'])) {
      $form['expose']['use_operator'] = array(
        '#type' => 'checkbox',
        '#title' => t('Unlock operator'),
        '#description' => t('When checked, the operator will be exposed to the user'),
        '#default_value' => !empty($this->options['expose']['use_operator']),
      );
      $form['expose']['operator'] = array(
        '#type' => 'textfield',
        '#default_value' => $this->options['expose']['operator'],
        '#title' => t('Operator identifier'),
        '#size' => 40,
        '#description' => t('This will appear in the URL after the ? to identify this operator.'),
        '#process' => array('views_process_dependency'),
        '#dependency' => array(
          'edit-options-expose-use-operator' => array(1)
        ),
      );
    }
    else {
      $form['expose']['operator'] = array(
        '#type' => 'value',
        '#value' => '',
      );
    }
    
    $form['expose']['optional'] = array(
      '#type' => 'checkbox',
      '#title' => t('Optional'),
      '#description' => t('This exposed filter is optional and will have added options to allow it not to be set.'),
      '#default_value' => $this->options['expose']['optional'],
    );
  }
}