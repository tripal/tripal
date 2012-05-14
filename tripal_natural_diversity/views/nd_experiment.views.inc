<?php
 
/**
 *  @file
 *  This file defines the data array for a given chado table. This array
 *  is merged into a larger array containing definitions of all tables associated
 *  with this module in:
 *  @see tripal_natural_diversity.views.inc --in tripal_natural_diversity_views_data()
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */

/*************************************************************************
 * Purpose: this function returns the portion of the data array 
 *   which describes the nd_experiment table, it's fields and any joins between it and other tables
 * @see tripal_natural_diversity_views_data() --in tripal_natural_diversity.views.inc
 *
 * @todo Add relationship to nd_geolocations.views.inc
 *
 * Table: nd_experiment
 * @code
  CREATE TABLE nd_experiment (
    nd_experiment_id serial PRIMARY KEY NOT NULL,
    nd_geolocation_id integer NOT NULL references nd_geolocation (nd_geolocation_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED 
  );
 * @endcode
 */
 function retrieve_nd_experiment_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
    $database = 'chado';
  }
   
  //Basic table definition-----------------------------------
  $data['nd_experiment']['table']['group'] = t('Chado ND Experiment');
  
  $data['nd_experiment']['table']['base'] = array(
    'field' => 'nd_experiment_id',
    'title' => t('Chado Natural Diversity Experiment'),
    'help' => t('Represents one data point in a natural diversity project.'),
  );
  if($database){
     $data['nd_experiment']['table']['database'] = $database;
  }

  
  //Relationship Definitions---------------------------------
  //Join: nd_experiment => nd_geolocation
  // In nd_experiment.views.inc
  
  //Join: nd_experiment => nd_experiment_contact => contact
  $data['nd_experiment']['table']['join']['nd_experiment_contact'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['contact'] = array(
    'left_table' => 'nd_experiment_contact',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_contact']['table']['join']['contact'] = array(
    'left_field' => 'contact_id',
    'field' => 'contact_id',
  );
  
  //Join: nd_experiment => nd_experiment_dbxref => dbxref
  $data['nd_experiment']['table']['join']['nd_experiment_dbxref'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['dbxref'] = array(
    'left_table' => 'nd_experiment_dbxref',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_dbxref']['table']['join']['dbxref'] = array(
    'left_field' => 'dbxref_id',
    'field' => 'dbxref_id',
  );
  
  //Join: nd_experiment => nd_experiment_genotype => genotype
  $data['nd_experiment']['table']['join']['nd_experiment_genotype'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['genotype'] = array(
    'left_table' => 'nd_experiment_genotype',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_genotype']['table']['join']['genotype'] = array(
    'left_field' => 'genotype_id',
    'field' => 'genotype_id',
  );
  
  //Join: nd_experiment => nd_experiment_phenotype => phenotype
  $data['nd_experiment']['table']['join']['nd_experiment_phenotype'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['phenotype'] = array(
    'left_table' => 'nd_experiment_phenotype',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_phenotype']['table']['join']['phenotype'] = array(
    'left_field' => 'phenotype_id',
    'field' => 'phenotype_id',
  );
  
  //Join: nd_experiment => nd_experiment_project => project
  $data['nd_experiment']['table']['join']['nd_experiment_project'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['project'] = array(
    'left_table' => 'nd_experiment_project',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_project']['table']['join']['project'] = array(
    'left_field' => 'project_id',
    'field' => 'project_id',
  );
  
  //Join: nd_experiment => nd_experiment_pub => pub
  $data['nd_experiment']['table']['join']['nd_experiment_pub'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['pub'] = array(
    'left_table' => 'nd_experiment_pub',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_pub']['table']['join']['pub'] = array(
    'left_field' => 'pub_id',
    'field' => 'pub_id',
  );

  //Join: nd_experiment => nd_experiment_stock => YYY
  $data['nd_experiment']['table']['join']['nd_experiment_stock'] = array(
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );  
  $data['nd_experiment']['table']['join']['stock'] = array(
    'left_table' => 'nd_experiment_stock',
    'left_field' => 'nd_experiment_id',
    'field' => 'nd_experiment_id',
  );
  $data['nd_experiment_stock']['table']['join']['stock'] = array(
    'left_field' => 'stock_id',
    'field' => 'stock_id',
  );

  //Table Field Definitions----------------------------------
      
  //Field: nd_experiment_id (primary key)
  $data['nd_experiment']['nd_experiment_id'] = array(
    'title' => t('ND Experiment Primary Key'),
    'help' => t('A unique index for every nd_experiment.'),
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
    
  return $data;
}