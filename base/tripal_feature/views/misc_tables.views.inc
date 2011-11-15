<?php

/**
 * Purpose: Allows the feature module to add fields to other module views
 *
 *   For example, a field counting the number of features associted with a given organism would be
 *   added to the organism view as follows:
 *   @code
      $data['organism']['num_features'] = array(
        'title' => 'Number of features',
        'help' => 'Provides a count of the number of features associated with a given organism',
        'field' => array(
          'handler' => 'views_handler_field_chado_count',
          'table_to_query' => 'feature',
        ),
      ); 
 *   @endcode
 *
 * @ingroup tripal_feature_views
 */
function retrieve_feature_misc_tables_views_data() {
  $data = array();
 
  // Table: Organism--------------------------------------------------------------------------------
  // Calculated Field: Number of features (Count -Int)
  // Provides the number of features for a given organism
  $data['organism']['num_features'] = array(
    'title' => 'Number of Features',
    'help' => 'Provides a count of the number of features associated with a given organism',
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'feature',
    ),
  );  

  // Table: Library---------------------------------------------------------------------------------
  // Calculated Field: Number of features (Count -Int)
  // Provides the number of features for a given organism
  $data['library']['num_features'] = array(
    'title' => 'Number of Features',
    'help' => 'Provides a count of the number of features associated with a given library',
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'library_feature',
    ),
  );   

  // Table: Analysis--------------------------------------------------------------------------------
  // Calculated Field: Number of features (Count -Int)
  // Provides the number of features for a given organism
  $data['analysis']['num_features'] = array(
    'title' => 'Number of Features',
    'help' => 'Provides a count of the number of features associated with a given analysis',
    'field' => array(
      'handler' => 'views_handler_field_chado_count',
      'table_to_query' => 'analysisfeature',
    ),
  ); 
  return $data;
}