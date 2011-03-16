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
 *   which describes the nd_geolocation table, it's fields and any joins between it and other tables
 * @see tripal_natural_diversity_views_data() --in tripal_natural_diversity.views.inc
 *
 * Table: nd_geolocation
 * @code
 * nd_geolocation-Copy/Paste Table SQL code here-nd_geolocation
 * @endcode
 */
 function retrieve_nd_geolocation_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
    $database = 'chado';
  }
   
  //Basic table definition-----------------------------------
  $data['nd_geolocation']['table']['group'] = t('Chado ND geolocation');
  
  $data['nd_geolocation']['table']['base'] = array(
    'field' => 'nd_geolocation_id',
    'title' => t('Chado Natural Diversity Geolocation'),
    'help' => t('A listing of locations where Natural Diversity Experiments can be performed.'),
  );
  if($database){
     $data['nd_geolocation']['table']['database'] = $database;
  }

  
  //Relationship Definitions---------------------------------
  //Join: nd_geolocation => nd_experiment
  $data['nd_geolocation']['table']['join']['nd_experiment'] = array(
    'left_field' => 'nd_geolocation_id',
    'field' => 'nd_geolocation_id',
  );  
  
  //Table Field Definitions----------------------------------
      
  //Field: nd_geolocation_id (primary key)
  $data['nd_geolocation']['nd_geolocation_id'] = array(
    'title' => t('ND Geolocation Primary Key'),
    'help' => t('A unique index for every nd_geolocation.'),
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

  //Field: description (varchar 255)   
  $data['nd_geolocation']['description'] = array(
    'title' => t('Description'),
    'help' => t('A short description of a given geolocation.'),
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

  //Field: latitude (real)   
  $data['nd_geolocation']['latitude'] = array(
    'title' => t('Latitude'),
    'help' => t('The decimal latitude coordinate of the georeference, using positive and negative sign to indicate N and S, respectively.'),
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

  //Field: longitude (real)   
  $data['nd_geolocation']['longitude'] = array(
    'title' => t('Longitude'),
    'help' => t('The decimal longitude coordinate of the georeference, using positive and negative sign to indicate E and W, respectively.'),
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

  //Field: altitude (real)   
  $data['nd_geolocation']['altitude'] = array(
    'title' => t('Altitude'),
    'help' => t('The altitude (elevation) of the location in meters. If the altitude is only known as a range, this is the average, and altitude_dev will hold half of the width of the range.'),
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

  //Field: geodetic_datum (varchar 255)   
  $data['nd_geolocation']['geodetic_datum'] = array(
    'title' => t('Deodetic Datum'),
    'help' => t('The geodetic system on which the geo-reference coordinates are based. For geo-references measured between 1984 and 2010, this will typically be WGS84.'),
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