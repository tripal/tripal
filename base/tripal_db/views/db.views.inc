<?php

  /**
 * Purpose: this function returns the portion of the data array 
 *   which describes the cv table, it's fields and any joins between it and other tables
 * @see tripal_cv_views_data() --in tripal_cv.views.inc
 *
 *  TABLE: db
 * @code
 * create table db (
 *    db_id serial not null,
 *    primary key (db_id),
 *    name varchar(255) not null,
 *    description varchar(255) null,
 *    urlprefix varchar(255) null,
 *    url varchar(255) null,
 *    constraint db_c1 unique (name)
 * );
 * @endcode
 *
 * @ingroup tripal_db_views
 */
function retrieve_db_views_data() {
  
  // Basic table definition
  $data['db']['table'] = array(
    'field' => 'db_id',
    'group' => 'Chado Database',
    'title' => 'Chado Database',
    'help' => 'Database Records existing in the Chado Database',
    'database' => 'chado'
  );
  
  // Define relationships between this table and others
  $data['db']['table']['join'] = array(
    'dbxref' => array(
      'left_field' => 'db_id',
      'field' => 'db_id',
    ),
    'stock' => array(
    	'left_table' => 'dbxref',
    	'left_field' => 'db_id',
    	'field' => 'db_id',
    ),
    'cvterm' => array(
    	'left_table' => 'dbxref',
    	'left_field' => 'db_id',
    	'field' => 'db_id',
    ),
    'feature' => array(
    	'left_table' => 'dbxref',
    	'left_field' => 'db_id',
    	'field' => 'db_id',
    ),
  );
  
  // Table Field Definitions----------------------
  // Field: db_id (primary key)
  $data['db']['db_id'] = array(
    'title' => t('Database ID'),
    'help' => t('The primary key of the Database.'),
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
  
  // Field: name (varchar 255)
  $data['db']['name'] = array(
    'title' => 'Name',
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_chado_select_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  // Field: description (varchar 255)
  $data['db']['description'] = array(
    'title' => 'Description',
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
  
  // Field: urlprefix (varchar 255)
  $data['db']['urlprefix'] = array(
    'title' => 'URL Prefix',
    'help' => 'The url that when concatenated with the accession of a record in this db produces the url to the record in this database.',
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
  
  // Field: url (varchar 255)
  $data['db']['url'] = array(
    'title' => 'URL',
    'help' => 'The url to the main page of the database',
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