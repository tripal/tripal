<?php

/*************************************************************************
 * @file: THIS IS A TEMPLATE AND SHOULD NOT BE INCLUDED IN THE MODULE CODE
 *
 *  - simply replace all XXX with the original chado table you want to join to it's drupal nodes.
 *    (ie: If you want to join features to their drupal nodes then XXX=feature)
 * 
 *  NOTE: Creating the table definition file is not enough. You also need to call the 
 *        retrieve_XXX_views_data() function from ../tripal_genetic.views.inc:tripal_genetic_views_data()
 *        by adding the following line:
 *           $data = array_merge($data, retrieve_XXX_views_data());
 *        to the function and including the file directly above the function (blow the function 
 *        header by adding:
 *           require_once('views/XXX.views.inc');
 *
 *  REMOVE THIS COMMENT IN THE COPY!
 */ 
 
/**
 *  @file
 *  This file defines the data array for a given chado table. This array
 *  is merged into a larger array containing definitions of all tables associated
 *  with this module in:
 *  @see tripal_genetic.views.inc --in tripal_genetic_views_data()
 *
 *  Note: All chado tables are joined to their drupal nodes through the chado_XXX linking table. 
 *        This file simply defines this linking table and joins the three tables together.
 *        No modification of XXX.views.inc is needed.
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */
 
/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the chado_XXX drupal table, it's fields and any joins between it 
 *   and other tables
 * @see tripal_genetic_views_data() --in tripal_genetic.views.inc
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 */
function retrieve_chado_XXX_views_data () {
	global $db_url;
  $data = array();
  
  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     // return empty data array b/c if chado is external then no join to the nodetable can be made
     return $data;
  }

  //Basic table definition-----------------------------------
  $data['chado_XXX']['table'] = array(
    'field' => 'nid',
  );
  
  //Relationship Definitions---------------------------------
  // Note: No joins need to be made from $data['XXX']['table']
  
  // Join the chado_XXX table to XXX
  $data['chado_XXX']['table']['join']['XXX'] = array(
  	'left_field' => 'XXX_id',
  	'field' => 'XXX_id',
  );
  
  // Join the node table to chado_XXX
  $data['node']['table']['join']['chado_XXX'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Join the node table to XXX
  $data['node']['table']['join']['XXX'] = array(
  	'left_table' => 'chado_XXX',
  	'left_field' => 'nid',
  	'field' => 'nid',
  );  

	return $data;
}