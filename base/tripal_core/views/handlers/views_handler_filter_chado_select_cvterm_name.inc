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
  	
  	if ($this->options['show_all']) {
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
    
    } else {
        $sql = "SELECT cvterm_id, name FROM cvterm WHERE cvterm_id IN (SELECT distinct(type_id) FROM %s)";
        $previous_db = tripal_db_set_active('chado');
        $resource = db_query($sql, $this->view->base_table);
        tripal_db_set_active($previous_db);
        $cvterms = array();
        while ( $r = db_fetch_object($resource) ) {
          $cvterms[$r->cvterm_id] = $r->name;
        }        
    }
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

    $form['multiple'] = array(
      '#type' => 'checkbox',
      '#title' => t('Select Multiple'),
      '#description' => t('Allows more then one option to be selected.'),
      '#default_value' => (isset($this->options['multiple'])) ? $this->options['multiple'] : FALSE,
    );

    $form['optional'] = array(
      '#type' => 'checkbox',
      '#title' => t('Optional'),
      '#description' => t('Adds --Any-- to the available options.'),
      '#default_value' => (isset($this->options['optional'])) ? $this->options['optional'] : TRUE,
    );
    
    $form['show_all'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show All Terms',
      '#description' => 'Otherwise only cvterms used in the base table will be used'
    );
  }
  
 /**
  * Alters the query so that WHERE cvterm.cvterm_id=212 is used
  */
  function query() {
    $this->ensure_table;
    
    if ($this->options['multiple']) {
      // Remove any if it's there
      unset($this->value['All']);
      
      if (sizeof($this->value)) {
        $holders = array();
        foreach ($this->value as $v) {
          if (preg_match('/^[\d\.]+$/',$v)) {
            $holders[] = '%d';
          } else {
            $holders[] = "'%s'";
          }
        }
        $where = "cvterm.cvterm_id IN (".implode(", ",$holders).")";
      }
    } else {
      if (preg_match('/^\d+$/', $this->value)) {
        $where = 'cvterm.cvterm_id=%d';
      } else {
        $where = "cvterm.name" . $this->operator . "'%s'";
      }
    }
    
    if ($where) {
      $this->query->add_where($this->options['group'], $where, $this->value);
    }
  }
  
 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    if (preg_match('/select/', $this->options['values_form_type'])) {
      // Get Options
      if ($this->options['optional']) {
        $options['<select '.$this->table.'>'] = '--None--';
        $options['All'] = '--Any--';
      }
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

      if ($this->options['multiple']) {
        $form['value']['#multiple'] = TRUE;
      }
    
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