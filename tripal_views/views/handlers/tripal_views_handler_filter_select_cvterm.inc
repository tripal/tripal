<?php

/**
 * @file
 * Purpose: This Handler provides a select list for the type field
 *
 *  NOTE: This handler only works when applied to the type_id field in the base_table of
 *  this view.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class tripal_views_handler_filter_select_cvterm extends views_handler_filter_string {

 /**
  * Executed when the field is added
  * Determine which cv to limit the cvterms to
  */
  function init(&$view, $options) {
    parent::init($view, $options);

    if (isset($this->options['show_all'])) {
      $cv_id = variable_get('chado_' . $this->view->base_table . '_cv', NULL);
      if ($cv_id) {
        $results = tripal_core_chado_select('cvterm', array('cvterm_id', 'name'), array('cv_id' => $cv_id));
        if (empty($results)) {
          $results = array();
        }
        foreach ($results as $c) {
          $cvterms[$c->cvterm_id] = $c->name;
        }
      }
      else {
        //get a list of cvs currently used
        if ($this->view->base_table == 'cvterm') {
          $sql = 'SELECT distinct(cv.cv_id) FROM {' . $this->view->base_table . '}'
            .' LEFT JOIN cv cv ON cv.cv_id=cvterm.cv_id';
        }
        else {
          $sql = 'SELECT distinct(cv.cv_id) FROM {' . $this->view->base_table . '}'
            .' LEFT JOIN cvterm cvterm ON cvterm.cvterm_id=' . $this->view->base_table . '.type_id '
            .'LEFT JOIN cv cv ON cv.cv_id=cvterm.cv_id';
        }
        // D7 TODO: Check DBTNG changes work
        $resource = chado_query($sql);
        $cvterms = array();
        foreach ($resource as $r) {
          $results = tripal_core_chado_select('cvterm', array('cvterm_id', 'name'), array('cv_id' => $r->cv_id));
          if (empty($results)) {
            $results = array();
          }
          foreach ($results as $c) {
            $cvterms[$c->cvterm_id] = $c->name;
          }
        }
      }// end of if variable not defined

    }
    else {
      // @coder-ignore: non-drupal schema therefore table prefixing does not apply
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT cvterm_id, name FROM {cvterm} WHERE cvterm_id IN (SELECT distinct(" . $this->field . ") FROM {" . $this->table . "})";
      $resource = chado_query($sql);
      $cvterms = array();
      foreach ($resource as $r) {
        $cvterms[$r->cvterm_id] = $r->name;
      }
    }
    //sort cvterms by name (case insensitive)
    natcasesort($cvterms);

    //add to this handler
    $this->cvterm_options = $cvterms;

  }

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    $form['msg'] = array(
      '#type' => 'item',
      '#value' => '<b>If this filter applies to a table that is aggregated, additionally options may be ignored.</b>'
    );

    parent::options_form($form, $form_state);

    $form['values_form_type'] = array(
      '#type' => 'radios',
      '#title' => t('Filter Type'),
      '#options' => array(
        'textfield' => 'Text Field',
        'select' => 'Drop-Down Box',
      ),
      '#default_value' => ($this->options['values_form_type']) ? $this->options['values_form_type'] : 'select',
    );

    $form['select_multiple'] = array(
      '#type' => 'checkbox',
      '#title' => t('Select Multiple'),
      '#description' => t('Allows more then one option to be selected.'),
      '#default_value' => (isset($this->options['select_multiple'])) ? $this->options['select_multiple'] : FALSE,
    );

    $form['select_optional'] = array(
      '#type' => 'checkbox',
      '#title' => t('Optional'),
      '#description' => t('Adds --Any-- to the available options.'),
      '#default_value' => (isset($this->options['optional'])) ? $this->options['optional'] : TRUE,
    );

    $form['show_all'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show All Terms'),
      '#description' => 'Otherwise only cvterms used in the base table will be used'
    );

  }

  /**
   * Assign our new form elements values to the options array
   */
  function options_submit(&$form, &$form_state) {
    $this->options['values_form_type'] = $form_state['input']['options']['values_form_type'];
    $this->options['select_multiple'] = $form_state['input']['options']['select_multiple'];
    $this->options['select_optional'] = $form_state['input']['options']['select_optional'];
  }

  /**
   * If the table to be filtered is not aggregated uses the parent::query()
   * However, if it is uses postgresql any() function to compare
   */
  function query() {

    // make optional
    // if it is not set or empty then don't restrict the query
    if (!$this->value) {
      return;
    }

    if (is_array($this->value)) {
      if (isset($this->value['All'])) {
        unset($this->value['All']);
      }

      if ($this->operator == '!=') {
        $this->operator = 'NOT IN';
      }
      else {
        $this->operator = 'IN';
      }
    }

    $this->ensure_my_table();
    $field = $this->table_alias . "." . $this->real_field;
    $table = $this->query->get_table_info($this->table);

    if ($this->value) {
      $this->query->add_where($this->options['group'], $field, $this->value, $this->operator);
    }

  }

 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    if (isset($this->options['values_form_type']) AND preg_match('/textfield/', $this->options['values_form_type'])) {
      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => t('%label', array('%label' => $this->options['expose']['label'])),
        '#default_value' => $this->value,
      );
    }
    else {
      // Get Options
      if ($this->options['select_optional']) {
        $options['<select ' . $this->table . '>'] = '--None--';
        $options['All'] = '--Any--';
      }
      $max_length = 40;
      foreach ($this->cvterm_options as $cvterm_id => $cvterm_name) {
        if (drupal_strlen($cvterm_name) > $max_length) {
          $options[$cvterm_id] = drupal_substr($cvterm_name, 0, $max_length) . '...';
        }
        else {
          $options[$cvterm_id] = $cvterm_name;
        }
      }

      if (empty($options)) {
        $options[0] = '';
      }

      //Select List
      $form['value'] = array(
          '#type' => 'select',
          '#title' => t('%label', array('%label' => $this->options['expose']['label'])),
          '#options' => $options,
          '#default_value' => $this->value,
      );

      if ($this->options['select_multiple']) {
        $form['value']['#multiple'] = TRUE;
      }

    }

  }

  /**
   * Ensure the value form gets exposed correctly
   */
  function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);

    if ($this->options['select_multiple']) {
      $form[$this->real_field]['#multiple'] = TRUE;
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
