<?php

/**
 * Purpose: Allows the analysis module to add fields to other module views
 *
 *   For example, a field counting the number of analysis' associted with a given feature would be
 *   added to the feature view as follows:
 *   @code
      $data['feature']['num_analysis''] = array(
        'title' => "Number of analysis'",
        'help' => "Provides a count of the number of analysis' associated with a given feature",
        'field' => array(
          'handler' => 'views_handler_field_chado_count',
          'table_to_query' => 'analysis_feature',
        ),
      ); 
 *   @endcode
 *
 * @ingroup tripal_analysis_views
 */
function retrieve_analysis_misc_tables_views_data() {
  $data = array();
 
  // Table: Feature---------------------------------------------------------------------------------
  // Calculated Field: Number of analysis' (Count -Int)
  // Provides the number of analysis' for a given feature
  $data['feature']['num_analysis'] = array(
    'title' => "Number of analysis'",
    'help' => "Provides a count of the number of analysis' associated with a given feature",
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'analysisfeature',
    ),
  );  
  
  return $data;
}