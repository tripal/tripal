<?php

/**
 * @file
 * Purpose: This Handler provides a generic select list for any chado field that is a string
 *  The select list includes all distinct values for that field.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class tripal_views_handler_filter_select_string extends chado_views_handler_filter_string {

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

    $form['max_length'] = array(
      '#type' => 'textfield',
      '#title' => t('Max Width'),
      '#description' => t('Specify the maximum width of the select box'),
      '#default_value' => (isset($this->options['max_length'])) ? $this->options['max_length'] : 40,

    );
    $form['max_length'] = array(
      '#type' => 'markup',
      '#value' => t('<strong><font color="red">Note:</font></strong> If another filter exists for the same table then '.
                    'the values shown in the drop box will only include those from rows that are not filtered.'),

    );

  }

 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    if (preg_match('/textfield/', $this->options['values_form_type'])) {
      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => t('%label', array('%label' => $this->options['label'])),
        '#default_value' => $this->value,
      );

    }
    else {

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
      $sql = "SELECT $this->real_field FROM $this->table $where ORDER BY $this->field ASC";
      $previous_db = tripal_db_set_active('chado');  // use chado database
      $results = db_query($sql);
      tripal_db_set_active($previous_db);  // now use drupal database

      // Build the select box options
      $max_length = $this->options['max_length'];
      if (!$max_length) {
        $max_length = 40;
      }
      if ($this->options['optional']) {
        //$options['<select '.$this->table.'>'] = '--None--';
        $options['All'] = '--Any--';
      }
      while ($r = db_fetch_object($results)) {
        if (drupal_strlen($r->{$this->field}) > $max_length) {
          $options[$r->{$this->field}] = drupal_substr($r->{$this->field}, 0, $max_length) . '...';
        }
        else {
          $options[$r->{$this->field}] = $r->{$this->field};
        }
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
  * Adds this filter to the where clause of the views query
  */
  function query() {

    // make optional
    // if it is not set or empty then don't restrict the query
    if (!$this->value) {
      return;
    }

    $this->ensure_my_table();

    $table = $this->query->get_table_info($this->table);
    if (preg_match('/aggregator/', $table['join']->definition['handler'])) {
      $this->aggregated = TRUE;
    }
    else {
      $this->aggregated = FALSE;
    }

    // filter the aggregates
    if ($this->options['agg']['aggregates_with']) {
      $this::query_restrict_curr_table_records();
    }

    // filter the base table
    if ($this->options['agg']['records_with']) {
      $this::query_restrict_base_records();
    }




  }

  /**
   * This function alters the query by adding the appropriate WHERE
   * to filter the base table to only those with the value in the aggregate array.
   *
   * Note: this function is called only from query()
   */
  function query_restrict_base_records() {
    if (!$this->aggregated) {
      // Not Aggregated ---------------

      $this->ensure_my_table();
      $field = "$this->table_alias.$this->real_field";
      $upper = $this->case_transform();

      if ($this->options['multiple'] AND is_array($this->value)) {
        // Remove any if it's there
        unset($this->value['All']);

        if (sizeof($this->value)) {
          $holders = array();
          foreach ($this->value as $v) {
            if (preg_match('/^[\d\.]+$/', $v)) {
              $holders[] = '%f';
            }
            else {
              $holders[] = "'%s'";
            }
          }
          $where = "$field IN (" . implode(", ", $holders) . ")";
          $this->query->add_where($this->options['group'], $where, $this->value);
        }
      }
      else {

        // Deal with All/Any as value
        if (preg_match('/All/', $this->value)) {
          // Don't do anything
        }
        else {
          $info = $this->operators();
          if (!empty($info[$this->operator]['method'])) {
            $this->{$info[$this->operator]['method']}($field, $upper);
          }
        }
      }

    }
    else {
      // Is Aggregated ----------------

      $this->ensure_my_table();
      $field = "$this->table_alias.$this->real_field";
      $upper = $this->case_transform();

      if ($this->options['multiple'] AND is_array($this->value)) {
        // Remove any if it's there
        unset($this->value['All']);

        if (sizeof($this->value) > 1) {
          $holders = array();
          foreach ($this->value as $v) {
            $holders[] = "'%s'";
          }
          $where = $field .' && ARRAY[' . implode(", ", $holders) . ']';
          $this->query->add_where($this->options['group'], $where, $this->value);

        }
        elseif (sizeof($this->value) == 1) {
          $where = "'%s' = ANY($field)";
          $this->query->add_where($this->options['group'], $where, array_pop($this->value));
        }
      }
      else {

        // Deal with All/Any as value
        if (preg_match('/All/', $this->value)) {
          // Don't do anything
        }
        else {
          $where = "'%s' = ANY($field)";
          $this->query->add_where($this->options['group'], $where, $this->value);
        }
      }

    }
  }

  /**
   * This function alters the query by adding the appropriate WHERE
   * to filter the aggregates shown based on the value. Doesn't affect
   * the number of base table records.
   *
   * Note: this function is called only from query()
   */
  function query_restrict_curr_table_records() {

    if (!$this->aggregated) {
      // Not Aggregated ---------------
      // Warn the admin/user that they have selected that the aggregates should be filtered
      // on a field that isn't aggregated...
      watchdog(
        'tripal_views',
        'You have chosen to filter the aggregates shown for %table %field
          in %view; however, that field is not aggregated (ie: it is part of the base table
          or is a 1:1 relationship to the base table)',
        array(
          '%field' => $this->field,
          '%table' => $this->table,
          '%view' => $this->view->name
        ),
        WATCHDOG_WARNING
      );
      // Do nothing!
    }
    else {
      // Is Aggregated ----------------

      $this->ensure_my_table();
      $field = "$this->table_alias.$this->real_field";
      $upper = $this->case_transform();

      if ($this->options['multiple'] AND is_array($this->value)) {
        // Remove any if it's there
        unset($this->value['All']);

        if (sizeof($this->value) > 1) {
          $holders = array();
          foreach ($this->value as $v) {
            $holders[] = "'%s'";
          }
          $where = $field .' IN (' . implode(", ", $holders) . ')';
          $where = vsprintf($where, $this->value);

          // Add the where to the chado aggregated join object for this table
          // then the views_handler_join_chado_aggregator will add this to the WHERE
          // clause of the sub-query generating the aggregated listing
          $this->query->table_queue[ $this->table ]['join']->filter[] = $where;

        }
        elseif (sizeof($this->value) == 1) {
          $where = "$field = '%s'";
          $where = vsprintf($where, $this->value);

          // Add the where to the chado aggregated join object for this table
          // then the views_handler_join_chado_aggregator will add this to the WHERE
          // clause of the sub-query generating the aggregated listing
          $this->query->table_queue[ $this->table ]['join']->filter[] = $where;
        }
      }
      else {

        // Deal with All/Any as value
        if (preg_match('/All/', $this->value)) {
          // Don't do anything
        }
        else {
          $where = "'%s' = ANY($field)";
          $this->query->add_where($this->options['group'], $where, $this->value);
        }
      }

    }
  }
}
