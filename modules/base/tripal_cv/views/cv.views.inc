<?php

/**
 * Prupose: this function returns a portion of the data array 
 *   which describes the cv table, it's fields and any joins between it and other tables
 * @see tripal_cv_views_data() --in tripal_cv.views.inc
 *
 * TABLE: cv
 * @code
 *   create table cv (
 *       cv_id serial not null,
 *       primary key (cv_id),
 *       name varchar(255) not null,
 *       definition text,
 *       constraint cv_c1 unique (name)
 *   );
 * @endcode
 *
 * @ingroup tripal_cv_views
 */
function retrieve_cv_views_data() {
  $data = array();

  // Basic table definition
  $data['cv']['table'] = array(
    'field' => 'cv_id',
    'title' => 'Chado CV (Controlled Vocabulary)',
    'group' => 'Chado CV',
    'help' => 'Controlled vocabularies existing in the Chado Database',
    'database' => 'chado'
  );

  // Define relationships between this table and others
  $data['cv']['table']['join'] = array(
     'cvterm' => array(
       'left_field' => 'cv_id',
       'field' => 'cv_id',
     ),
  );

  // Table Field Definitions----------------------
  // Field: cv_id (primary key)
  $data['cv']['cv_id'] = array(
    'title' => t('CV ID'),
    'help' => t('The primary key of the controlled vocabulary.'),
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
  
  //Field: name (varchar -255)
  $data['cv']['name'] = array(
    'title' => 'Vocabulary Name',
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

  //Field: definition (text)
  $data['cv']['definition'] = array(
    'title' => 'Vocabulary Definition',
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

  return $data;

}