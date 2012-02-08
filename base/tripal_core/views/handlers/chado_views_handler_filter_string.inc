<?php

class chado_views_handler_filter_string extends views_handler_filter_string {

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['records_with'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter base table records'),
      '#description' => t('Filters '.$this->view->base_table.' to only those with the value in the aggregate array.'),
      '#default_value' => ($this->options['records_with']) ? $this->options['records_with'] : TRUE,
    );
    
    $form['aggregates_with'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter aggregates displayed'),
      '#description' => t('Filters the aggregates shown based on the value. Doesn\'t affect the number of '.$this->view->base_table.' records.'),
      '#default_value' => ($this->options['aggregates_with']) ? $this->options['aggregates_with'] : TRUE,    
    );
    
  }
  
  /**
   * If the table to be filtered is not aggregated uses the parent::query()
   * However, if it is uses postgresql any() function to compare
   */
  function query () {
    $this->ensure_my_table();
    
    
    $table = $this->query->get_table_info($this->table);
    if (preg_match('/aggregator/',$table['join']->definition['handler'])) {
      $this->aggregated = TRUE;
    } else {
      $this->aggregated = FALSE;
    }

    if (!$this->aggregated) {
      parent::query();
    } else {

      // Only base records with value in the aggregated field
      // This doesn't restrict the items in the aggregate field
      $this->ensure_my_table();
      $field = "$this->table_alias.$this->real_field";
      if ($this->options['records_with']) {
        $where = "'%s' = ANY($field)";
        $this->query->add_where($this->options['group'], $where, $this->value);
      }

      // To restrict the items in the aggregate...
      // Tell the join handler about the filter 
      // so it can be done in the join query
      if ($this->options['aggregates_with']) {
        $table['join']->filter[] = $field ." = '". $this->value."'";
      }    
    }
    
  }
  
}