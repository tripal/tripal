<?php

/**
 * @file
 * A chado wrapper for the views_handler_filter_date.
 *
 * Handles fields which may be aggregated during the chado join process. There are options
 * to filter the base table based on an aggregated table or just filter the aggregated
 * table (showing blank for that field if there are no records in the aggregated table).
 *
 * @todo make handle aggregation
 */
class chado_views_handler_filter_date extends views_handler_filter_date {

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    $form['msg'] = array(
      '#type' => 'item',
      '#value' => '<b>If this filter applies to a table that is aggregated, additionally options may be ignored.</b>'
    );

    parent::options_form($form, $form_state);

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
   * Called by query if the operator is between
   */
  function op_between($field) {

    // Check whether we have a UNIX timestamp or an ISO Timestamp
    $check = db_fetch_object(db_query("SELECT $this->real_field as val FROM $this->table WHERE $this->real_field IS NOT NULL LIMIT 1"));
    if (preg_match('/^\d+$/', $check->val)) {
      // this is a unix timestamp
      $is_unix = TRUE;
    }
    else {
      // this is an ISO Timestamp
      $is_unix = FALSE;
    }

    if ($this->operator == 'between') {
      $a = intval(strtotime($this->value['min'], 0));
      $b = intval(strtotime($this->value['max'], 0));
    }
    else {
      $a = intval(strtotime($this->value['max'], 0));
      $b = intval(strtotime($this->value['min'], 0));
    }

    if ($this->value['type'] == 'offset') {
      $a = '***CURRENT_TIME***' . sprintf('%+d', $a); // keep sign
      $b = '***CURRENT_TIME***' . sprintf('%+d', $b); // keep sign
    }
    // %s is safe here because strtotime scrubbed the input and we might
    // have a string if using offset.
    if ($is_unix) {
      if ($this->operator == 'between') {
        $this->query->add_where($this->options['group'], "$field >= %s", $a);
        $this->query->add_where($this->options['group'], "$field <= %s", $b);
      }
      else {
        $this->query->add_where($this->options['group'], "$field >= %s OR $field <= %s", array($a, $b));
      }
    }
    else {
      if ($this->operator == 'between') {
        $this->query->add_where($this->options['group'], "CAST(EXTRACT(EPOCH FROM $field) as integer) >= %s", $a);
        $this->query->add_where($this->options['group'], "CAST(EXTRACT(EPOCH FROM $field) as integer) <= %s", $b);
      }
      else {
        $this->query->add_where($this->options['group'], "CAST(EXTRACT(EPOCH FROM $field) as integer) >= %s OR CAST(EXTRACT(EPOCH FROM $field) as integer) <= %s", array($a, $b));
      }
    }
  }

  /**
   * Called by query if the operator is not between or empty
   */
  function op_simple($field) {
    $value = intval(strtotime($this->value['value'], 0));

    // Check whether we have a UNIX timestamp or an ISO Timestamp
    $check = db_fetch_object(db_query("SELECT $this->real_field as val FROM $this->table WHERE $this->real_field IS NOT NULL LIMIT 1"));
    if (preg_match('/^\d+$/', $check->val)) {
      // this is a unix timestamp
      $is_unix = TRUE;
    }
    else {
      // this is an ISO Timestamp
      $is_unix = FALSE;
    }

    if ($is_unix) {
      if (!empty($this->value['type']) && $this->value['type'] == 'offset') {
        $value = '***CURRENT_TIME***' . sprintf('%+d', $value); // keep sign
      }
      $this->query->add_where($this->options['group'], "$field $this->operator %s", $value);
    }
    else {
      if (!empty($this->value['type']) && $this->value['type'] == 'offset') {
        $value = '***CURRENT_TIME***' . sprintf('%+d', $value); // keep sign
      }
      $this->query->add_where($this->options['group'], "CAST(EXTRACT(EPOCH FROM $field) as integer) $this->operator %s", $value);
    }
  }

  /**
   * Validate that the time values convert to something usable.
   *
   * We modify it to
   * - fix a bug in the views handler for single values
   *    $value['value'] didn't exist
   * - fix a pass by reference error
   *    changed form_error to form_set_error
   */
  function validate_valid_time(&$form, $operator, $value) {
    $operators = $this->operators();

    if ($operators[$operator]['values'] == 1) {
      if (!empty($value['value'])) {
        $convert = strtotime($value['value']);
        if (!empty($form) && ($convert == -1 || $convert === FALSE)) {
          form_set_error($form['value'], t('Invalid date format.'));
        }
      }
    }
    elseif ($operators[$operator]['values'] == 2) {
      $min = strtotime($value['min']);
      if ($min == -1 || $min === FALSE) {
        form_set_error($form['min'], t('Invalid date format.'));
      }
      $max = strtotime($value['max']);
      if ($max == -1 || $max === FALSE) {
        form_set_error($form['max'], t('Invalid date format.'));
      }
    }
  }

}
