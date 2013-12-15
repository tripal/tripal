<?php

/**
 * @file
 * Purpose: This Handler provides a generic select list for any chado field that is a string
 *  The select list includes all distinct values for that field.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class tripal_views_handler_filter_select_string extends views_handler_filter_string {

  /**
   * Provide the options used in the select list.
   * Override this function in extended handlers to easily change option list.
   *
   * @return
   *   An array of options where the key is the value of this field in the database
   */
  function get_select_options() {

    // build a where clause that will filter the list in the drop box
    // using fields that are not exposed and that are for the table
    // from whcih the values in the drop box will be slected and
    // we only want to use non-exposed fields because these are not
    // available to the user to edit--they're fixed.
    $where = '';
    $filters = (is_array($this->view->filter)) ? $this->view->filter : array();
    foreach ($filters as $filter_name => $details) {
       // we only want to inclue non-exposed filters
       if ($details->options['exposed'] == FALSE) {
          // we only want to filter on the table we're getting the list from
          if (strcmp($details->table, $this->table)==0) {
            $where .= "$details->field $details->operator " . $details->value['value'];
            $where .= ' AND ';
          }
       }
    }
    if ($where) {
       $where = "WHERE $where";
       $where = substr($where, 0, -5); # remove the final ' AND '
    }

    // get the values from the table
    $sql = 'SELECT ' . $this->real_field . ' FROM {' . $this->table . '}' . $where . ' ORDER BY ' . $this->field . ' ASC';
    $results = chado_query($sql);

    // Build the select box options
    $max_length = $this->options['max_length'];
    if (!$max_length) {
      $max_length = 40;
    }
    $options = array();
    if ($this->options['optional']) {
      $options['All'] = '--Any--';
    }
    foreach ($results as $r) {
      if (drupal_strlen($r->{$this->field}) > $max_length) {
        $options[$r->{$this->field}] = drupal_substr($r->{$this->field}, 0, $max_length) . '...';
      }
      else {
        $options[$r->{$this->field}] = $r->{$this->field};
      }
    }

    return $options;
  }

  function options_form(&$form, &$form_state) {
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
      '#default_value' => (isset($this->options['select_optional'])) ? $this->options['select_optional'] : TRUE,
    );

    $form['max_length'] = array(
      '#type' => 'textfield',
      '#title' => t('Max Width'),
      '#description' => t('Specify the maximum width of the select box'),
      '#default_value' => (isset($this->options['max_length'])) ? $this->options['max_length'] : 40,

    );
    $form['note'] = array(
      '#type' => 'markup',
      '#value' => t('<strong><font color="red">Note:</font></strong> If another filter exists for the same table then '.
                    'the values shown in the drop box will only include those from rows that are not filtered.'),

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
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    if (preg_match('/select/', $this->options['values_form_type'])) {

      //Select List
      $form['value'] = array(
          '#type' => 'select',
          '#title' => t('%label', array('%label' => $this->options['expose']['label'])),
          '#options' => $this->get_select_options(),
          '#default_value' => $this->value,
      );

      if ($this->options['select_multiple']) {
        $form['value']['#multiple'] = TRUE;
      }
    }
    else {

      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => t('%label', array('%label' => $this->options['expose']['label'])),
        '#default_value' => $this->value,
      );

    }
  }

  /**
   * Ensure the value form gets exposed correctly
   */
  function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);

    if ($this->options['select_multiple']) {
      $form[$this->options['id']]['#multiple'] = TRUE;
    }
  }

 /**
  * Adds this filter to the where clause of the views query
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
}
