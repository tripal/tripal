<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_phenotype tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */
 
/*************************************************************************
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
function tripal_phenotype_views_data()  {
  $data = array();
  
  return $data;
}

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 */
function tripal_phenotype_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_phenotype') . '/views/handlers',
   ),
   'handlers' => array(

   ),
 );
}