<?php

/**
 * Purpose: Allows the library module to add fields to other module views
 *
 *   For example, a field counting the number of libraries associted with a given feature would be
 *   added to the feature view as follows:
 *   @code
      $data['feature']['num_libraries'] = array(
        'title' => "Number of Libraries",
        'help' => "Provides a count of the number of libraries associated with a given feature",
        'field' => array(
          'handler' => 'views_handler_field_chado_count',
          'table_to_query' => 'library_feature',
        ),
      ); 
 *   @endcode
 *
 * @ingroup tripal_library_views
 */
function retrieve_library_misc_tables_views_data() {
  $data = array();
 
  // Table: Feature---------------------------------------------------------------------------------
  // Calculated Field: Number of Libraries (Count -Int)
  // Provides the number of libraries for a given feature
  $data['feature']['num_libraries'] = array(
    'title' => "Number of Libraries",
    'help' => "Provides a count of the number of libraries associated with a given feature",
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'library_feature',
    ),
  ); 

  // Table: Organism--------------------------------------------------------------------------------
  // Calculated Field: Number of Libraries (Count -Int)
  // Provides the number of libraries for a given organism
  $data['organism']['num_libraries'] = array(
    'title' => 'Number of Libraries',
    'help' => 'Provides a count of the number of libraries associated with a given organism',
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'library',
    ),
  );  
  
  return $data;
}