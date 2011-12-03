<?php 
 
/**
 *  @file
 *  This file defines the data array for a given chado table. This array
 *  is merged into a larger array containing definitions of all tables associated
 *  with this module in:
 *  @see tripal_genetic.views.inc --in tripal_genetic_views_data()
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */

/*************************************************************************
 * Purpose: this function returns the portion of the data array 
 *   which describes the genotype table, it's fields and any joins between it and other tables
 * @see tripal_genetic_views_data() --in tripal_genetic.views.inc
 *
 * Table: genotype
 * @code
 * genotype-Copy/Paste Table SQL code here-genotype
 * @endcode
 */
 function retrieve_genotype_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
    $database = 'chado';
  }
   
  //Basic table definition-----------------------------------
  $data['genotype']['table']['group'] = t('Chado Genotype');
  
  $data['genotype']['table']['base'] = array(
    'field' => 'genotype_id',
    'title' => t('Chado Genotype'),
    'help' => t('A genotype is defined by a collection of features, mutations, balancers, deficiencies, haplotype blocks, or engineered constructs.'),
  );
  if($database){
     $data['genotype']['table']['base']['database'] = $database;
  }

  
  //Relationship Definitions---------------------------------

  //Join: feature => feature_genotype => feature
  // This relationship should be described in both directions
  // in the appropriate files (ie: for feature => library 
  // describe in both feature.views.inc and library.views.inc)
  $data['feature']['table']['join']['feature_genotype'] = array(
    'left_field' => 'feature_id',
    'field' => 'feature_id',
  );  
  $data['feature']['table']['join']['genotype'] = array(
    'left_table' => 'feature_genotype',
    'left_field' => 'feature_id',
    'field' => 'feature_id',
  );
  $data['feature_genotype']['table']['join']['genotype'] = array(
    'left_field' => 'genotype_id',
    'field' => 'genotype_id',
  );

  //Join: feature => feature_genotype => feature
  // This relationship should be described in both directions
  // in the appropriate files (ie: for feature => library 
  // describe in both feature.views.inc and library.views.inc)
  $data['genotype']['table']['join']['feature_genotype'] = array(
    'left_field' => 'genotype_id',
    'field' => 'genotype_id',
  );  
  $data['genotype']['table']['join']['feature'] = array(
    'left_table' => 'feature_genotype',
    'left_field' => 'genotype_id',
    'field' => 'genotype_id',
  );
  $data['feature_genotype']['table']['join']['feature'] = array(
    'left_field' => 'feature_id',
    'field' => 'feature_id',
  );
  
   //Join: stock => stock_genotype => stock
  // This relationship should be described in both directions
  // in the appropriate files (ie: for stock => library 
  // describe in both stock.views.inc and library.views.inc)
  $data['stock']['table']['join']['stock_genotype'] = array(
    'left_field' => 'stock_id',
    'field' => 'stock_id',
  );  
  $data['stock']['table']['join']['genotype'] = array(
    'left_table' => 'stock_genotype',
    'left_field' => 'stock_id',
    'field' => 'stock_id',
  );
  $data['stock_genotype']['table']['join']['genotype'] = array(
    'left_field' => 'genotype_id',
    'field' => 'genotype_id',
  );
  
  //Table Field Definitions----------------------------------
      
  //Field: genotype_id (primary key)
  $data['genotype']['genotype_id'] = array(
    'title' => t('Genotype Primary Key'),
    'help' => t('A unique index for every genotype.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  //Field: name (text)  
  $data['genotype']['name'] = array(
    'title' => t('Name'),
    'help' => t('Human-readable Name of the genotype.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  //Field: uniquename (text)  
  $data['genotype']['uniquename'] = array(
    'title' => t('Unique Name'),
    'help' => t('Unique Name of the genotype.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  //Field: description (varchar -255)   
  $data['genotype']['description'] = array(
    'title' => t('Description/Value'),
    'help' => t('A Description of the genotype.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
    
  return $data;
}