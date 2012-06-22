<?php

/**
 * This is a temporary function holding all the old-style views integration
 * needed to link the base tables to their node. This will remain to keep things working
 * until it has been best determined how to do this via the new tripal_views integration
 *
 * It's called att the bottom of tripal_views_views_data()
 */
function tripal_views_TEMPORARY_chado_linking_data($data) {
  global $db_url;

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     // return empty data array b/c if chado is external then no join to the nodetable can be made
     return $data;
  }

  // FEATURE ====================================
  if (module_exists('tripal_feature')) {
    // Basic table definition
    $data['chado_feature']['table'] = array(
      'field' => 'nid',
      'group' => 'Chado Feature'
    );

    $data['chado_feature']['nid'] = array(
      'title' => t('Feature Node ID'),
      'help' => t('The node ID for this feature'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Note: No joins need to be made from $data['feature']['table']

    // Join the chado feature table to feature
    $data['chado_feature']['table']['join']['feature'] = array(
      'left_field' => 'feature_id',
      'field' => 'feature_id',
    );

    // Join the node table to chado feature
    $data['node']['table']['join']['chado_feature'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Join the node table to feature
    $data['node']['table']['join']['feature'] = array(
      'left_table' => 'chado_feature',
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Add relationship between chado_feature and feature
    $data['chado_feature']['feature_nid'] = array(
      'group' => 'Feature',
      'title' => 'Feature Node',
      'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
      'real field' => 'feature_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Feature'),
        'label' => t('Chado => Feature'),
        'real field' => 'feature_id',
        'base' => 'feature',
        'base field' => 'feature_id'
      ),
    );

    // Add node relationship to feature
    $data['chado_feature']['feature_chado_nid'] = array(
      'group' => 'Feature',
      'title' => 'Feature Node',
      'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Node'),
        'label' => t('Chado => Node'),
        'real field' => 'nid',
        'base' => 'node',
        'base field' => 'nid'
      ),
    );
  }

  // ANALYSIS ===================================
  if (module_exists('tripal_analysis')) {
    // Basic table definition
    $data['chado_analysis']['table'] = array(
      'field' => 'nid',
      'group' => 'Chado Analysis'
    );

    $data['chado_analysis']['nid'] = array(
      'title' => t('Analysis Node ID'),
      'help' => t('The node ID for this analysis'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Note: No joins need to be made from $data['analysis']['table']

    // Join the chado analysis table to analysis
    $data['chado_analysis']['table']['join']['analysis'] = array(
      'left_field' => 'analysis_id',
      'field' => 'analysis_id',
    );

    // Join the node table to chado analysis
    $data['node']['table']['join']['chado_analysis'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Join the node table to analysis
    $data['node']['table']['join']['analysis'] = array(
      'left_table' => 'chado_analysis',
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Add relationship between chado_analysis and analysis
    $data['chado_analysis']['analysis_nid'] = array(
      'group' => 'Analysis',
      'title' => 'Analysis Node',
      'help' => 'Links Chado Analysis Fields/Data to the Nodes in the current View.',
      'real field' => 'analysis_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Analysis'),
        'label' => t('Chado => Analysis'),
        'real field' => 'analysis_id',
        'base' => 'analysis',
        'base field' => 'analysis_id'
      ),
    );

    // Add node relationship to analysis
    $data['chado_analysis']['analysis_chado_nid'] = array(
      'group' => 'Analysis',
      'title' => 'Analysis Node',
      'help' => 'Links Chado Analysis Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Node'),
        'label' => t('Chado => Node'),
        'real field' => 'nid',
        'base' => 'node',
        'base field' => 'nid'
      ),
    );
  }

  // ORGANISM ===================================
  if (module_exists('tripal_organism')) {
    // Basic table definition
    $data['chado_organism']['table'] = array(
      'field' => 'nid',
      'group' => 'Chado Organism'
    );

    $data['chado_organism']['nid'] = array(
      'title' => t('Organism Node ID'),
      'help' => t('The node ID for this organism'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Note: No joins need to be made from $data['organism']['table']

    // Join the chado organism table to organism
    $data['chado_organism']['table']['join']['organism'] = array(
      'left_field' => 'organism_id',
      'field' => 'organism_id',
    );

    // Join the node table to chado organism
    $data['node']['table']['join']['chado_organism'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Join the node table to organism
    $data['node']['table']['join']['organism'] = array(
      'left_table' => 'chado_organism',
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Add relationship between chado_organism and organism
    $data['chado_organism']['organism_id'] = array(
      'group' => 'Organism',
      'title' => 'Organism Node',
      'help' => 'Links Chado Organism Fields/Data to the Nodes in the current View.',
      'real field' => 'organism_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Organism'),
        'label' => t('Chado => Organism'),
        'real field' => 'organism_id',
        'base' => 'organism',
        'base field' => 'organism_id'
      ),
    );
  /*
    // Add node relationship to organism
    $data['chado_organism']['nid'] = array(
      'group' => 'Organism',
      'title' => 'Organism Node',
      'help' => 'Links Chado Organism Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Node'),
        'label' => t('Chado => Node'),
        'real field' => 'nid',
        'base' => 'node',
        'base field' => 'nid'
      ),
    );
  */
  }

  // LIBRARY ====================================
  if (module_exists('tripal_library')) {
    // Basic table definition
    $data['chado_library']['table'] = array(
      'field' => 'nid',
      'group' => 'Chado Library'
    );

    $data['chado_library']['nid'] = array(
      'title' => t('Library Node ID'),
      'help' => t('The node ID for this library'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Note: No joins need to be made from $data['library']['table']

    // Join the chado library table to library
    $data['chado_library']['table']['join']['library'] = array(
      'left_field' => 'library_id',
      'field' => 'library_id',
    );

    // Join the node table to chado library
    $data['node']['table']['join']['chado_library'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Join the node table to library
    $data['node']['table']['join']['library'] = array(
      'left_table' => 'chado_library',
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Add relationship between chado_library and library
    $data['chado_library']['library_nid'] = array(
      'group' => 'Library',
      'title' => 'Library Node',
      'help' => 'Links Chado Library Fields/Data to the Nodes in the current View.',
      'real field' => 'library_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Library'),
        'label' => t('Chado => Library'),
        'real field' => 'library_id',
        'base' => 'library',
        'base field' => 'library_id'
      ),
    );

    // Add node relationship to library
    $data['chado_library']['library_chado_nid'] = array(
      'group' => 'Library',
      'title' => 'Library Node',
      'help' => 'Links Chado Library Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Node'),
        'label' => t('Chado => Node'),
        'real field' => 'nid',
        'base' => 'node',
        'base field' => 'nid'
      ),
    );
  }

  // STOCK ======================================
  if (module_exists('tripal_stock')) {
    // Basic table definition
    $data['chado_stock']['table'] = array(
      'field' => 'stock_id',
      'group' => 'Chado Stock',
    );

    $data['chado_stock']['nid'] = array(
      'title' => t('Stock Node ID'),
      'help' => t('The node ID for this analysis'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Note: No joins need to be made from $data['stock']['table']

    // Join the chado stock table to stock
    $data['chado_stock']['table']['join']['stock'] = array(
      'left_field' => 'stock_id',
      'field' => 'stock_id',
    );

    // Join the node table to chado stock
    $data['node']['table']['join']['chado_stock'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Join the node table to stock
    $data['node']['table']['join']['stock'] = array(
      'left_table' => 'chado_stock',
      'left_field' => 'nid',
      'field' => 'nid',
    );

    // Add relationship between chado_stock and stock
    $data['chado_stock']['stock_nid'] = array(
      'group' => 'Stock',
      'title' => 'Stock Node',
      'help' => 'Links Chado Stock Fields/Data to the Nodes in the current View.',
      'real field' => 'stock_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Stock'),
        'label' => t('Chado => Stock'),
        'real field' => 'stock_id',
        'base' => 'stock',
        'base field' => 'stock_id'
      ),
    );

    // Add node relationship to stock
    $data['chado_stock']['stock_chado_nid'] = array(
      'group' => 'Stock',
      'title' => 'Stock Node',
      'help' => 'Links Chado Stock Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Chado => Node'),
        'label' => t('Chado => Node'),
        'real field' => 'nid',
        'base' => 'node',
        'base field' => 'nid'
      ),
    );
  }

  return $data;
}