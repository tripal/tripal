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

    include_once('chado_wrapper_functions.inc');

    parent::init($view, $options);

    if ($this->options['show_all']) {
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
        $resource = chado_query($sql);
        $cvterms = array();
        while ( $r = db_fetch_object($resource) ) {
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
      $sql = "SELECT cvterm_id, name FROM {cvterm} WHERE cvterm_id IN (SELECT distinct(%s) FROM {%s})";
      $resource = chado_query($sql, $this->field, $this->table);
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
      '#title' => t('Show All Terms'),
      '#description' => 'Otherwise only cvterms used in the base table will be used'
    );

    $form['agg'] = array(
      '#type' => 'fieldset',
      '#title' => 'Apply to fields that are aggregated'
    );

    $form['agg']['records_with'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter base table records'),
      '#description' => t('Filters %base_table to only those with the value in the aggregate array.', array('%base_table' => $this->view->base_table)),
      '#default_value' => (isset($this->options['agg']['records_with'])) ? $this->options['agg']['records_with'] : TRUE,
    );

    $form['agg']['aggregates_with'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter aggregates displayed'),
      '#description' => t('Filters the aggregates shown based on the value. Doesn\'t affect the number of %base_table records.', array('%base_table' => $this->view->base_table)),
      '#default_value' => (isset($this->options['agg']['aggregates_with'])) ? $this->options['agg']['aggregates_with'] : TRUE,
    );

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

    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";

    $table = $this->query->get_table_info($this->table);
    if (preg_match('/aggregator/', $table['join']->definition['handler'])) {
      $this->aggregated = TRUE;
    }
    else {
      $this->aggregated = FALSE;
    }

    if (!$this->aggregated) {

      if ($this->options['multiple']) {
        // Remove any if it's there
        unset($this->value['All']);

        if (sizeof($this->value)) {
          $holders = array();
          foreach ($this->value as $v) {
            if (preg_match('/^[\d\.]+$/', $v)) {
              $holders[] = '%d';
            }
            else {
              $holders[] = "'%s'";
            }
          }
          $where = $field . " IN (" . implode(", ", $holders) . ")";
        }
      }
      elseif ($this->value != 'All') {
        if (preg_match('/^\d+$/', $this->value)) {
          $where = $field . ' = %d';
        }
        else {
          $where = $field . " " . $this->operator . " '%s'";
        }
      }

      if ($where) {
        $this->query->add_where($this->options['group'], $where, $this->value);
      }
    }
    else {

      // Only base records with value in the aggregated field
      // This doesn't restrict the items in the aggregate field
      $this->ensure_my_table();
      if ($this->options['agg']['records_with']) {
        $where = "'%s' = ANY($field)";
        $this->query->add_where($this->options['group'], $where, $this->value);
      }

      // To restrict the items in the aggregate...
      // Tell the join handler about the filter
      // so it can be done in the join query
      if ($this->options['agg']['aggregates_with']) {

        if (sizeof($this->value) == 1 && is_array($this->value)) {
          $table['join']->filter[] = $field . " " . $this->operator . " '" . array_pop($this->value) . "'";
        }
        elseif (sizeof($this->value) == 1 && is_string($this->value)) {
          $table['join']->filter[] = $field . " " . $this->operator . " '" . $this->value . "'";
        }
        elseif (sizeof($this->value) > 1 && is_array($this->value)) {
          $table['join']->filter[] = $field . " IN (" . implode(',',$this->value) . ")";
        }
      }
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
          '#title' => t('%label', array('%label' => $this->options['label'])),
          '#options' => $options,
          '#default_value' => $this->value,
      );

      if ($this->options['multiple']) {
        $form['value']['#multiple'] = TRUE;
      }

    }
    else {
      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => t('%label', array('%label' => $this->options['label'])),
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
