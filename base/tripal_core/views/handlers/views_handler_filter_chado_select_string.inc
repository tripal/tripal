<?php

/**
 * Purpose: This Handler provides a generic select list for any chado field that is a string
 *  The select list includes all distinct values for that field.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class views_handler_filter_chado_select_string extends views_handler_filter_string {

 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    // Get Options
    if ($this->options['exposed']) {    
      $options['All'] = '<Any>';
    }
    $options['<select '.$this->table.'>'] = '<None>';
    $results = tripal_core_chado_select(
      $this->table,
      array($this->field),
      array()
    );
    $max_length = 40;
    foreach ($results as $r) {
      if (strlen($r->{$this->field}) > $max_length) {
        $options[$r->{$this->field}] = substr($r->{$this->field},0,$max_length) . '...';
      } else {
        $options[$r->{$this->field}] = $r->{$this->field};    
      }
    }
    
    //Select List
    $form['value'] = array(
        '#type' => 'select',
        '#title' => $this->options['label'],
        '#options' => $options,
        '#default_value' => $this->value,
    );
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
  *
  */
  function query() {
    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";
    $upper = $this->case_transform();

    // Deal with All/Any as value
    if (preg_match('/All/', $this->value)) {
      // Don't do anything    
    } else {
      $info = $this->operators();
      if (!empty($info[$this->operator]['method'])) {
        $this->{$info[$this->operator]['method']}($field, $upper);
      }
    }
  }
}