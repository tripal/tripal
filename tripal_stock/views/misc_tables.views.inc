<?php

/**
 * Purpose: Allows the stock module to add fields to other module views
 *
 *   For example, a field counting the number of stocks associted with a given organism would be
 *   added to the organism view as follows:
 *   @code
      $data['organism']['num_stocks'] = array(
        'title' => 'Number of stocks',
        'help' => 'Provides a count of the number of stocks associated with a given organism',
        'field' => array(
          'handler' => 'views_handler_field_chado_count',
          'table_to_query' => 'stock',
        ),
      ); 
 *   @endcode
 *
 * @ingroup tripal_stock_views
 */
function retrieve_stock_misc_tables_views_data() {
  $data = array();
 
  // Table: Organism--------------------------------------------------------------------------------
  // Calculated Field: Number of stocks (Count -Int)
  // Provides the number of stocks for a given organism
  $data['organism']['num_stocks'] = array(
    'title' => 'Number of Stocks',
    'help' => 'Provides a count of the number of stocks associated with a given organism',
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'stock',
    ),
  );  
  
  return $data;
}