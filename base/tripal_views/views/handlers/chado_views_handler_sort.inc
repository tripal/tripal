<?php

class chado_views_handler_sort extends views_handler_sort {

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    
    $form['msg'] = array(
      '#type' => 'item',
      '#value' => '<b>Sorting of aggregated fields only works for PostgreSQL 9.0+. This is due to lack of support at the database level. With lower postgreSQL versions, no sorting is applied.</b>'
    );

    parent::options_form($form, $form_state);
  }
  
  /**
   * Adds the sort to the query only if the field isn't aggregated
   * If the field is aggregated then the sort has to be applied at the join handler level
   */
  function query () {
    
    // Determine if the current field is part of an aggregated table
    $table = $this->query->get_table_info($this->table);
    if (preg_match('/aggregator/',$table['join']->definition['handler'])) {
      $this->aggregated = TRUE;
    } else {
      $this->aggregated = FALSE;
    }
    
    // One day when the aggregated sort will work (ie: Postgresql 9.0+)
    // it will need to be applied in join handler
    // thus tell join handler about the sort
    $table['join']->sort[] = array(
      'table' => $table['alias'],
      'field' => $this->options['field'],
      'order' => $this->options['order']
    );
    
    
    // if not then add the sort
    if (!$this->aggregated) {
      parent::query();
    }
    
  }
  
}