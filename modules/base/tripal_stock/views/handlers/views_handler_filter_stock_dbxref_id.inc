<?php

/**
 * Allows stocks to be filtered by associated database reference accession
 *
 * @ingroup tripal_stock
 * @ingroup views_filter_handlers
 */
class views_handler_filter_stock_dbxref_id extends views_handler_filter {

  function init(&$view, $options) {
    parent::init($view, $options);
    $this->db = $this->options['db'];
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

	function query () {
		$this-> db = array_filter($this->db);
		
		if (preg_match('/IS NOT NULL/', $this->options['operator'])) {
			$new_where_sql = "stock.stock_id IN "
			."(SELECT stock_dbxref.stock_id FROM stock_dbxref, dbxref WHERE stock_dbxref.dbxref_id=dbxref.dbxref_id AND dbxref.db_id IN (".implode(', ',$this->db)."))";
			$this->query->add_where($this->options['group'], $new_where_sql);
		} elseif (preg_match('/IS NULL/', $this->options['operator'])) {
			$new_where_sql = "stock.stock_id NOT IN "
			."(SELECT stock_dbxref.stock_id FROM stock_dbxref, dbxref WHERE stock_dbxref.dbxref_id=dbxref.dbxref_id AND dbxref.db_id IN (".implode(', ',$this->db)."))";
			$this->query->add_where($this->options['group'], $new_where_sql);
		} else {
			if (!empty($this->value)) {
				$new_where_sql = "stock.stock_id IN "
					."(SELECT stock_dbxref.stock_id FROM stock_dbxref, dbxref WHERE stock_dbxref.dbxref_id=dbxref.dbxref_id "
					."AND dbxref.db_id IN (".implode(', ',$this->db).") AND dbxref.accession".$this->operator."'".$this->value."')";
				$this->query->add_where($this->options['group'], $new_where_sql);
			}
		}
	}
  
  /////////// Form Parts/////////////////////////
  function types_form(&$form, &$form_state) {
  
    $db_options = tripal_db_get_db_options();
	  ksort($db_options);
    $form['db'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Databases'),
      '#options' => $db_options,
      '#default_value' => $this->db,
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
    );

  }

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
		$form['value'] = array(
  	  '#type' => 'textfield',
    	'#title' => t('Accession'),
      '#default_value' => $this->value,
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

    if (!empty($this->options['expose']['use_type']) && !empty($this->options['expose']['type'])) {
      $type = $this->options['expose']['type'];
      $form[$type] = array(
        '#type' => 'select',
        '#title' => t('Database References'),
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

  }

  function expose_form_right(&$form, &$form_state) {
    if (!empty($form['type']['#type'])) {
      $form['expose']['use_type'] = array(
        '#type' => 'checkbox',
        '#title' => t('Unlock Database Reference'),
        '#description' => t('When checked, the Database Reference will be exposed to the user'),
        '#default_value' => !empty($this->options['expose']['use_type']),
      );
      $form['expose']['type'] = array(
        '#type' => 'textfield',
        '#default_value' => $this->options['expose']['type'],
        '#title' => t('Database Reference identifier'),
        '#size' => 40,
        '#description' => t('This will appear in the URL after the ? to identify this Database Reference.'),
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

