<?php

/**
 * Allows stocks to be filtered by proeprty values
 *
 * @ingroup tripal_stock
 * @ingroup views_filter_handlers
 */
class views_handler_filter_stockprop_id extends views_handler_filter {

  function init(&$view, $options) {
    parent::init($view, $options);
    $this->type = $this->options['type'];
  }

  function options_form(&$form, &$form_state) {
    if ($this->can_expose()) {
      $this->show_expose_button($form, $form_state);
    }

    $form['op_val_start'] = array('#value' => '<div class="clear-block">');

    $this->types_form($form, $form_state);

    $this->show_operator_form($form, $form_state);
    $form['operator']['#prefix'] = '<div class="views-right-70">';
    $this->show_value_form($form, $form_state);
    $form['op_val_end'] = array('#value' => '</div>');

    if ($this->can_expose()) {
      $this->show_expose_form($form, $form_state);
    }

  }

  function options_validate(&$form, &$form_state) {
    parent::options_validate($form, $form_state);

    if (preg_match('/NULL/',$form_state['values']['options']['operator'])) {
      $value = $form_state['values']['options']['value'];
      if (!empty($value)) {
        drupal_set_message('The Value ('.$value.') will be IGNORED when the Operator is set to "Is Present" or Is Absent".', 'warning');
      }
    } else {
      $value = $form_state['values']['options']['value'];
      if (empty($value)) {
        form_error($form['value'], t('Value required. The value will be used in conjunction with the operator. For example, if the '
				   .'operator="Is equal to" and the value="2010" then only properties with a value of 2010 and the type specified will be displayed.'));
      }
    }

    if (empty($form_state['values']['options']['type'])) {
      drupal_set_message('No Property Type was choosen. As such, any property type whose value '
      			     .$form_state['values']['options']['operator'].' '.$form_state['values']['options']['value'].' will be displayed', 'warning');
    }
  }

	function query () {
		if (preg_match('/IS NOT NULL/', $this->options['operator'])) {
			$new_where_sql = "stock.stock_id IN (SELECT stockprop.stock_id FROM stockprop WHERE stockprop.type_id=".$this->type.")";
			$this->query->add_where($this->options['group'], $new_where_sql);
		} elseif (preg_match('/IS NULL/', $this->options['operator'])) {
			$new_where_sql = "stock.stock_id NOT IN (SELECT stockprop.stock_id FROM stockprop WHERE stockprop.type_id=".$this->type.")";
			$this->query->add_where($this->options['group'], $new_where_sql);
		} elseif ($this->value) {
			$new_where_sql = "stock.stock_id IN (SELECT stockprop.stock_id FROM stockprop WHERE stockprop.type_id=".$this->type." AND stockprop.value".$this->operator."'".$this->value."')";
			$this->query->add_where($this->options['group'], $new_where_sql);
		}
		
	}
	
  function types_form(&$form, &$form_state) {
    $previous_db = tripal_db_set_active('chado');
    $result = db_query("SELECT cvt.cvterm_id as type_id, cvt.name FROM cvterm cvt WHERE cvt.cvterm_id IN (SELECT type_id FROM stockprop)");
    tripal_db_set_active($previous_db);

    $types = array();
    while ($r = db_fetch_object($result)) { $types[$r->type_id] = $r->name; }

    $form['type'] = array(
      '#type' => count($options) < 10 ? 'radios' : 'select',
      '#title' => t('Property Types'),
      '#options' => $types,
      '#default_value' => $this->type,
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
    );

  }

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
	
		if ($this->options['expose']['display_type'] == 'select') {
		
			// Get options
			$previous_db = tripal_db_set_active('chado');
			$resource = db_query("SELECT value FROM stockprop WHERE type_id=".$this->type." ORDER BY value");
		 tripal_db_set_active($previous_db);
			while ($r = db_fetch_object($resource)) {
				$options[$r->value] = $r->value;
			}
			$form['value'] = array(
        '#type' => 'select',
        '#title' => $this->options['label'],
        '#options' => $options,
        '#default_value' => $this->value,
      );
		} else {
			$form['value'] = array(
  	    '#type' => 'textfield',
    	  '#title' => t('Value'),
      	'#default_value' => $this->value,
    	);
    }
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

    if (!empty($this->options['expose']['use_type']) && !empty($this->options['expose']['type'])) {
      $type = $this->options['expose']['type'];
      $form[$type] = array(
        '#type' => 'select',
        '#title' => t('Property Types'),
        '#options' => $this->type_options(),
        '#default_value' => $this->type,  
      );

      if (isset($form[$type]['#title'])) {
        unset($form[$type]['#title']);
      }
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
    
    $form['expose']['display_type'] = array(
    	'#type' => 'radios',
    	'#default_value' => $this->options['expose']['display_type'],
    	'#title' => t('Display Type'),
    	'#description' => t('This will change the form item type of the exposed value form. ie: it can be used to let the user select the property value from a select box rather than a textfield.'),
			'#options' => array(
				'textfield' => 'Text Field',
				'select' => 'Drop Down',
			),
    );
  }

  function expose_form_right(&$form, &$form_state) {
    if (!empty($form['type']['#type'])) {
      $form['expose']['use_type'] = array(
        '#type' => 'checkbox',
        '#title' => t('Unlock Property Type'),
        '#description' => t('When checked, the property type will be exposed to the user'),
        '#default_value' => !empty($this->options['expose']['use_type']),
      );
      $form['expose']['type'] = array(
        '#type' => 'textfield',
        '#default_value' => $this->options['expose']['type'],
        '#title' => t('Property Type identifier'),
        '#size' => 40,
        '#description' => t('This will appear in the URL after the ? to identify this property type.'),
        '#process' => array('views_process_dependency'),
        '#dependency' => array(
          'edit-options-expose-use-type' => array(1)
        ),
      );
    }
    else {
      $form['expose']['type'] = array(
        '#type' => 'value',
        '#value' => '',
      );
    }

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
