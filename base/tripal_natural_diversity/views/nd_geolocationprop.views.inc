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
 *   which describes the nd_geolocationprop table, it's fields and any joins between it and other tables
 * @see tripal_natural_diversity_views_data() --in tripal_natural_diversity.views.inc
 *
 * Table: nd_geolocationprop
 * @code
 * nd_geolocationprop-Copy/Paste Table SQL code here-nd_geolocationprop
 * @endcode
 */
 function retrieve_nd_geolocationprop_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
    $database = 'chado';
  }
   
  //Basic table definition-----------------------------------
  $data['nd_geolocationprop']['table'] = array(
    'field' => 'nd_geolocationprop_id',
    'title' => t('Chado ND Geolocation Properties'),
    'help' => t('Properties of Natural Diversity Geolocations.'),
    'group' => t('Chado ND Geolocation Properties'),
  );
  if($database){
     $data['nd_geolocationprop']['table']['database'] = $database;
  }

  
  //Relationship Definitions---------------------------------
  //Join: nd_geolocation => nd_geolocationprop
  $data['nd_geolocationprop']['table']['join']['nd_geolocation'] = array(
    'left_field' => 'nd_geolocation_id',
    'field' => 'nd_geolocation_id',
  );
  
  //Table Field Definitions----------------------------------
      
  //Field: nd_geolocationprop_id (primary key)
  $data['nd_geolocationprop']['field_name'] = array(
    'title' => t('ND Geolocation Property Primary Key'),
    'help' => t('A unique index for every ND Geolocation Property.'),
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
  
  //Field: value (varchar 255)   
  $data['nd_geolocationprop']['value'] = array(
    'title' => t('Property Value'),
    'help' => t('The value of a given property.'),
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

  //Field: rank (int)   
  $data['nd_geolocationprop']['rank'] = array(
    'title' => t('Property Rank'),
    'help' => t('The rank of a given propery.'),
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

  //Calculated Field: type/cvterm name (varchar 255)   
  $data['nd_geolocationprop']['nd_geolocationprop_cvterm_name'] = array(
    'title' => t('Property Type'),
    'help' => t('The type of property.'),
    'field' => array(
      'handler' => 'views_handler_field_cvterm_name',
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