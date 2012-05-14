<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the db table, it's fields and any joins between it and other tables
 * @see tripal_db_views_data() --in tripal_db.views.inc
 *
 * BASE TABLE: dbxref
 * @code
 * create table dbxref (
 *    dbxref_id serial not null,
 *    primary key (dbxref_id),
 *    db_id int not null,
 *    foreign key (db_id) references db (db_id) on delete cascade INITIALLY DEFERRED,
 *    accession varchar(255) not null,
 *    version varchar(255) not null default '',
 *    description text,
 *    constraint dbxref_c1 unique (db_id,accession,version)
 * );
 * @endcode
 *
 * @ingroup tripal_db_views
 */
function retrieve_dbxref_views_data() {

  // Basic table definition
  $data['dbxref']['table']['group'] = 'Chado Database Reference';
  $data['dbxref']['table']['base'] = array(
    'field' => 'dbxref_id',
    'title' => 'Chado Database Reference',
    'help' => 'A Reference to another database. Allows easy linking between becords in one database to those in another.',
    'database' => 'chado'
  );

  // Define relationships between this table and others
  $data['dbxref']['table']['join'] = array(
    'cvterm' => array(
      'left_field' => 'dbxref_id',
      'field' => 'dbxref_id',
    ),
    'feature' => array(
      'left_field' => 'dbxref_id',
      'field' => 'dbxref_id',
    ),
    'stock' => array(
      'left_field' => 'dbxref_id',
      'field' => 'dbxref_id',
    ),  
  );

  // Table Field Definitions----------------------
  // Field: dbxref_id (primary key)
  $data['dbxref']['dbxref_id'] = array(
    'title' => t('Database Reference ID'),
    'help' => t('The primary key of Database References.'),
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
   
  //Field: cv_id (foreign key: cv)
  //  join between cv table and this one in cv.views.inc

  // Field: accession (varchar 255)
  $data['dbxref']['accession'] = array(
    'title' => 'Accession',
    'help' => 'The accession from the database.',
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

  // Field: version (varchar 255)
  $data['dbxref']['version'] = array(
    'title' => 'Version',
    'help' => 'The version of the database reference.',
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

  // Field: description (text)
  $data['dbxref']['description'] = array(
    'title' => 'Description',
    'help' => 'a description of the database reference.',
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Calculated Field: Accession Link
  //  uses custom field handler to pull db urlprefix and concatenate with accession
  //  solves the problem of not being able to add urlprefix to tables which only
  //  join to dbxref table (not db)
  $data['dbxref']['accession_link'] = array(
    'title' => 'Accession Link',
    'help' => 'Provides a link to the record in the external database.',
    'field' => array(
      'handler' => 'views_handler_field_dbxref_accession_link',
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