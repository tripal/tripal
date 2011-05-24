<?php

/**
 * Purpose: This Handler provides a select list for the type field
 *
 *  NOTE: This handler only works when applied to the type_id field in the base_table of
 *  this view.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class views_handler_filter_chado_select_cvterm_name extends views_handler_filter_string {

 /**
	* Executed when the field is added
	* Determine which cv to limit the cvterms to
	*/
  function init(&$view, $options) {
    parent::init($view, $options);
  	
  	$cv_id = variable_get('chado_'.$this->view->base_table.'_cv', null);
    if ($cv_id) {
			$results = tripal_core_chado_select('cvterm',array('cvterm_id','name'), array('cv_id'=>$cv_id));				
			foreach ($results as $c) {
				$cvterms[$c->cvterm_id] = $c->name;
			}    	
    } else {
    	//get a list of cvs currently used
    	if ($this->view->base_table == 'cvterm') {
      	$sql = 'SELECT distinct(cv.cv_id) FROM '.$this->view->base_table
      		.' LEFT JOIN cv cv ON cv.cv_id=cvterm.cv_id';	
    	} else {
      	$sql = 'SELECT distinct(cv.cv_id) FROM '.$this->view->base_table
      		.' LEFT JOIN cvterm cvterm ON cvterm.cvterm_id='.$this->view->base_table.'.type_id '
      		.'LEFT JOIN cv cv ON cv.cv_id=cvterm.cv_id';
      }
    	$previous_db = tripal_db_set_active('chado');
    	$resource = db_query($sql);
    	tripal_db_set_active($previous_db);
    	$cvterms = array();
			while ( $r = db_fetch_object($resource) ) {
				$results = tripal_core_chado_select('cvterm',array('cvterm_id','name'), array('cv_id'=>$r->cv_id));				
				foreach ($results as $c) {
					$cvterms[$c->cvterm_id] = $c->name;
				}
			}
    }// end of if variable not defined
    
    //sort cvterms by name (case insensitive)
    natcasesort($cvterms);
    
    //add to this handler
    $this->cvterm_options = $cvterms;
  }

 /**
  * Defines options for the option forms
  */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['values_form_type'] = array(
      '#type' => 'radios',
      '#title' => 'Filter Type',
      '#options' => array(
        'textfield' => 'Text Field',
        'select' => 'Drop-Down Box',
      ),
      '#default_value' => $this->options['values_form_type'],
    );
  }
  
 /**
  * Alters the query so that WHERE cvterm.cvterm_id=212 is used
  */
  function query() {
    $this->ensure_table;
    
    if (preg_match('/^\d+$/', $this->value)) {
      $where = 'cvterm.cvterm_id=%d';
    } else {
      $where = "cvterm.name" . $this->operator . "'%s'";
    }

    $this->query->add_where($this->options['group'], $where, $this->value);
  }
  
 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    if (preg_match('/select/', $this->options['values_form_type'])) {
      // Get Options
      if ($this->options['exposed']) {    
        $options['All'] = '<Any>';
      }
      $options['<select '.$this->table.'>'] = '<None>';
      $max_length = 40;
      foreach ($this->cvterm_options as $cvterm_id => $cvterm_name) {
        if (strlen($cvterm_name) > $max_length) {
          $options[$cvterm_id] = substr($cvterm_name,0,$max_length) . '...';
        } else {
          $options[$cvterm_id] = $cvterm_name;    
        }
      }
      
      //Select List
      $form['value'] = array(
          '#type' => 'select',
          '#title' => $this->options['label'],
          '#options' => $options,
          '#default_value' => $this->value,
      );
    } else {
      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => $this->options['label'],
        '#default_value' => $this->value,
      );
    }
  }

 /**
  * Ensures the select list gets rendered when the filter is exposed
  */
  function exposed_form(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }

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

  /**
   * This kind of construct makes it relatively easy for a child class
   * to add or remove functionality by overriding this function and
   * adding/removing items from this array.
   */
  function operators() {
    $operators = array(
      '=' => array(
        'title' => t('Is equal to'),
        'short' => t('='),
        'method' => 'op_equal',
        'values' => 1,
      ),
      '!=' => array(
        'title' => t('Is not equal to'),
        'short' => t('!='),
        'method' => 'op_equal',
        'values' => 1,
      ),
      '~' => array(
        'title' => t('Contains'),
        'short' => t('contains'),
        'method' => 'op_contains',
        'values' => 1,
      ),
    );
    
    return $operators;
  }
}