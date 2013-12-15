<?php

/**
 * @file
 * A chado wrapper for the views_handler_field.
 *
 * Handles fields which may be aggregated during the chado join process. This field
 * will render an aggregated field as a pre_rendered list and will dynamically detect
 * whether the field is aggregated or not.
 *
 * D7: still not compatible
 */
class tripal_views_handler_field_aggregate extends chado_views_handler_field {

  function init(&$view, $options) {
    parent::init($view, $options);

    if (!isset($this->chado_table_description)) {
      $this->chado_table_description = tripal_core_get_chado_table_schema($this->table);
      foreach ($this->chado_table_description['foreign keys'] as $defn) {
        if ($defn['table'] != $this->view->base_table) {
          $join_table = tripal_core_get_chado_table_schema($defn['table']);
          foreach ($join_table['fields'] as $fname => $f) {
            $this->chado_table_description['fields'][$defn['table'] . '_' . $fname] = $f;
          }
        }
      }
    }

  }

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['format'] = array(
      '#type' => 'fieldset',
      '#title' => 'Format Output',
      '#description' => t('The following fields specify how a single result of this field will be
        displayed. When there are multiple results of this field due to aggregation, each result
        will be rendered according to the following rules and then all results will be joined
        together based on the "Display Type" indicated.')
    );

    $this->tokens = array();
    $value = array();
    foreach ( array_keys($this->chado_table_description['fields']) as $field ) {
      $t = '[' . $this->options['id'] . '-' . $field . ']';
      $this->tokens[$t] = t($field);
      $value[] = $t . ' == ' . $field;
    }

    $form['format']['format_string'] = array(
      '#type' => 'textfield',
      '#title' => t('Format String'),
      '#description' => 'Use any of the format tokens below to indicate what fields you want displayed.',
      '#default_value' => ($this->options['format']['format_string']) ? $this->options['format']['format_string'] : implode(', ', array_keys($this->tokens)),
    );

    $form['format']['tokens'] = array(
      '#type' => 'item',
      '#title' => 'Format Tokens',
      '#value' => implode("<br />", $value),
    );

  }

  function query() {
    parent::query();

    $this->table_definition = $this->query->get_table_info($this->table);
  }


  function pre_render(&$values) {

    // further check the results to see if this field is a postgresql array
    $this->aggregated = chado_wrapper_is_aggregated_by_result($this, $values);

    if ($this->aggregated) {

      // Split Aggregated Results
      chado_wrapper_split_array_agg_results($this, $values);

      // Now each result is of the following form:
      // stockprop_id::16554,stock_id::12037,type_id::3650,value::Sm2008-P13,rank::1...
      // we need to split it further
      foreach ($values as $k => $v) {
        $this->split_aggregated_result_with_keys($values[$k]->{$this->field_alias});
      }
    }
  }

  // Split a single value in the $values array into an associative array
  // $value will be an array where each value is of the following form:
  // stockprop_id::16554,stock_id::12037,type_id::3650,value::Sm2008-P13,rank::1...
  function split_aggregated_result_with_keys(&$value) {

    foreach ($value as $k => $v) {
      if (!is_array($v)) {
        if (preg_match('/.*::.*/',$v)) {
          $subparts = explode(',', $v);
          $token_values = array();
          foreach ($subparts as $ssk => $ssv) {
            if (preg_match('/(.*)::(.*)/', $ssv, $matches)) {
              $values[ $matches[1] ] = $matches[2];
              $tokens[ '[' . $this->options['id'] . '-' . $matches[1] . ']' ] = $matches[2];
            }
          }

          if ($this->options['format']['format_string']) {
            $value[$k] = str_replace(array_keys($tokens), $tokens, $this->options['format']['format_string']);
          }
          else {
            $value[$k] = $values;
          }
        }
        else {
          if ($this->options['format']['format_string']) {
            $value[$k] = '';
          }
          else {
            $value[$k] = array();
          }
        }
        $value = array_filter($value);
      }
    }
  }

}
