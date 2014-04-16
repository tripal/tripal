<?php
/**
 * @file
 * Contains tripal_views_handler_filter_select_string Filter Handler
 */

/**
 * This Handler provides a generic select list for any chado field that is a string
 *  The select list includes all distinct values for that field.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_select_string extends views_handler_filter_string {

  /**
   * {@inheritdoc}
   */
  function init(&$view, &$options) {
    parent::init($view, $options);

    // Backwards compatibility
    if (isset($this->options['expose']['values_form_type'])) {
      $this->options['values_form_type'] = $this->options['expose']['values_form_type'];
      unset($this->options['expose']['values_form_type']);
    }
    if (isset($this->options['expose']['select_multiple'])) {
      $this->options['select_multiple'] = $this->options['expose']['select_multiple'];
      unset($this->options['expose']['select_multiple']);
    }
    if (isset($this->options['expose']['select_optional'])) {
      $this->options['select_optional'] = $this->options['expose']['select_optional'];
      unset($this->options['expose']['select_optional']);
    }
    if (isset($this->options['expose']['max_length'])) {
      $this->options['max_length'] = $this->options['expose']['max_length'];
      unset($this->options['expose']['max_length']);
    }
  }

  /**
   * {@inheritdoc}
   */
  function has_extra_options() {
    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['values_form_type'] = array(
      'default' => 'textfield',
      'export' => TRUE
    );
    $options['select_multiple'] = array(
      'default' => FALSE,
      'bool' => TRUE,
      'export' => TRUE
    );
    $options['select_optional'] = array(
      'default' => FALSE,
      'bool' => TRUE,
      'export' => TRUE
    );
    $options['max_length'] = array(
      'default' => 40,
      'export' => TRUE
    );

    return $options;
  }

  /**
   * Provide the options used in the select list.
   * Override this function in extended handlers to easily change option list.
   *
   * @return
   *   An array of options where the key is the value of this field in the database
   */
  function get_select_options() {

    $where_clauses = $this->get_select_option_where();
    $where = '';
    if (!empty($where_clauses)) {
      $where = ' WHERE ' . implode(' AND ', $where_clauses);
    }

    // get the values from the table
    $sql = 'SELECT ' . $this->real_field . ' FROM {' . $this->table . '} ' . $where . ' ORDER BY ' . $this->field . ' ASC';
    $results = chado_query($sql);

    // Build the select box options
    $max_length = (isset($this->options['max_length'])) ? $this->options['max_length'] : 40;
    if (!$max_length) {
      $max_length = 40;
    }
    $options = array();
    if ($this->options['select_optional']) {
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

  /**
   * For the SQL generating the options, determine the WHERE clauses
   *
   * @return
   *   An array of full qualified where clauses (ie: table.myfield = 'fred')
   */
  function get_select_option_where() {
    $where = array();

    // build a where clause that will filter the list in the drop box
    // using fields that are not exposed and that are for the table
    // from whcih the values in the drop box will be slected and
    // we only want to use non-exposed fields because these are not
    // available to the user to edit--they're fixed.
    $where = array();
    $filters = (is_array($this->view->filter)) ? $this->view->filter : array();
    foreach ($filters as $filter_name => $details) {
      // we only want to inclue non-exposed filters
      if ($details->options['exposed'] == FALSE) {

        $value = $details->value;
        if (is_array($details->value) AND isset($details->value['value'])) {
         $value = $details->value['value'];
        }

        if (is_array($value)) {
          // we only want to filter on the table we're getting the list from
          if (strcmp($details->table, $this->table)==0 AND !empty($value)) {
            $where[] = "$details->field IN (" . implode(', ', $value) . ')';
          }

        }
        else {
          // we only want to filter on the table we're getting the list from
          if (strcmp($details->table, $this->table)==0 AND !empty($value)) {
            $where[] = "$details->field $details->operator " . $value;
          }
        }
      }
    }

    return $where;
  }

  /**
   * {@inheritdoc}
   */
  function extra_options_form(&$form, &$form_state) {
    parent::extra_options_form($form, $form_state);

    $form['values_form_type'] = array(
      '#type' => 'radios',
      '#title' => t('Filter Type'),
      '#options' => array(
        'textfield' => 'Text Field',
        'select' => 'Drop-Down Box',
      ),
      '#default_value' => $this->options['values_form_type'],
    );

    $form['select_multiple'] = array(
      '#type' => 'checkbox',
      '#title' => t('Select Multiple'),
      '#description' => t('Allows more then one option to be selected.'),
      '#default_value' => $this->options['select_multiple'],
    );

    $form['select_optional'] = array(
      '#type' => 'checkbox',
      '#title' => t('Optional'),
      '#description' => t('Adds --Any-- to the available options.'),
      '#default_value' => $this->options['select_optional'],
    );

    $form['max_length'] = array(
      '#type' => 'textfield',
      '#title' => t('Max Width'),
      '#description' => t('Specify the maximum width of the select box'),
      '#default_value' => $this->options['max_length'],
    );

    $form['note'] = array(
      '#type' => 'markup',
      '#value' => t('<strong><font color="red">Note:</font></strong> If another filter exists for the same table then '.
                    'the values shown in the drop box will only include those from rows that are not filtered.'),

    );

    return $form;

  }

  /**
   * {@inheritdoc}
   */
  function extra_options_submit($form, &$form_state) {
    $this->options['values_form_type'] = $form_state['values']['options']['values_form_type'];
    $this->options['select_multiple'] = $form_state['values']['options']['select_multiple'];
    $this->options['select_optional'] = $form_state['values']['options']['select_optional'];
    $this->options['max_length'] = $form_state['values']['options']['max_length'];
  }

  /**
   * {@inheritdoc}
   */
  function extra_options_options() {
    $this->options['values_form_type'] = 'textfield';
    $this->options['select_multiple'] = FALSE;
    $this->options['select_optional'] = FALSE;
    $this->options['max_length'] = 40;
  }

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    $this->options['values_form_type'] = (isset($this->options['values_form_type'])) ? $this->options['values_form_type'] : 'textfield';

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
   * {@inheritdoc}
   */
  function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);

    if (isset($this->options['select_multiple'])) {
      if ($this->options['select_multiple']) {

        if (isset($this->options['expose']['identifier'])) {
          $id = $this->options['expose']['identifier'];
        }
        else {
          $id = $this->options['id'];
        }
        $form[$id]['#multiple'] = TRUE;
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  function query() {

    // make optional
    // if it is not set or empty then don't restrict the query
    if (!$this->value) {
      return;
    }

    $this->ensure_my_table();
    $field = $this->table_alias . "." . $this->real_field;
    $table = $this->query->get_table_info($this->table);

    $this->options['values_form_type'] = (isset($this->options['values_form_type'])) ? $this->options['values_form_type'] : 'textfield';
    if (preg_match('/select/', $this->options['values_form_type'])) {
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
      else {
        // don't allow operators other than = and !=
        if ($this->operator != '!=') {
          $this->operator = '=';
        }
      }

      if ($this->value) {
        $this->query->add_where($this->options['group'], $field, $this->value, $this->operator);
      }
    }
    else {
      $info = $this->operators();
      if (!empty($info[$this->operator]['method'])) {
        $this->{$info[$this->operator]['method']}($field);
      }
    }

  }
}
