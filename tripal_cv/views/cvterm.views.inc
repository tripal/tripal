<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the cv table, it's fields and any joins between it and other tables
 * @see tripal_cv_views_data() --in tripal_cv.views.inc
 *
 * BASE TABLE: cvterm
 * @code
 *   create table cvterm (
 *       cvterm_id serial not null,
 *       primary key (cvterm_id),
 *       cv_id int not null,
 *       foreign key (cv_id) references cv (cv_id) on delete cascade INITIALLY DEFERRED,
 *       name varchar(1024) not null,
 *       definition text,
 *       dbxref_id int not null,
 *       foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
 *       is_obsolete int not null default 0,
 *       is_relationshiptype int not null default 0,
 *       constraint cvterm_c1 unique (name,cv_id,is_obsolete),
 *       constraint cvterm_c2 unique (dbxref_id)
 *   );
 * @endcode
 *
 * @ingroup tripal_cv_views
 */
function retrieve_cvterm_views_data() {

  // Basic table definition
  $data['cvterm']['table']['group'] = 'Chado CV Terms';
  $data['cvterm']['table']['base'] = array(
    'field' => 'cvterm_id',
    'title' => 'Chado CV Terms (Controlled Vocabulary)',
    'help' => 'Controlled Vocabularies (CVs) are the main way Chado controls content.',
    'database' => 'chado'
  );

  // Define relationships between this table and others
   $data['cvterm']['table']['join'] = array(
     'feature' => array(
       'left_field' => 'type_id',
       'field' => 'cvterm_id',
     ),
     'library' => array(
       'left_field' => 'type_id',
       'field' => 'cvterm_id',
     ),
     'stock' => array(
       'left_field' => 'type_id',
       'field' => 'cvterm_id',
     ),
     'nd_reagent' => array(
       'left_field' => 'type_id',
       'field' => 'cvterm_id',
     ),
   );

 // Table Field Definitions----------------------
 // Field: cvterm_id (primary key)
 $data['cvterm']['cvterm_id'] = array(
   'title' => t('CV Term ID'),
   'help' => t('The primary kep of controlled vocabulary terms.'),
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
 
 // Field: Name (varchar 1024)
  $data['cvterm']['name'] = array(
     'title' => 'Name',
     'help' => 'The term name',
     'field' => array(
     'handler' => 'views_handler_field',
       'click sortable' => TRUE,
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_chado_select_cvterm_name',
     ),
     'argument' => array(
       'handler' => 'views_handler_argument_string',
     ),
   );

   // Field: Definition (text)
   $data['cvterm']['definition'] = array(
     'title' => 'Definition',
     'help' => 'A definition of this term',
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

   // Field: dbxref_id (foreign key: dbxref)
   //  join between dbxref table and this one in tripal_db/views/dbxref.views.inc
   
   // Field: is_obsolete (integer: 1/0)
   $data['cvterm']['is_obsolete'] = array(
     'title' => 'Is Obsolete',
     'help' => 'Whether this term is obsolete or not.',
     'field' => array(
       'handler' => 'views_handler_field_boolean',
       'click sortable' => TRUE,
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_chado_boolean',
       'label' => t('Is Obsolete?'),
       'type' => 'yes-no',
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
   );

   // Field: is_relationshiptype (integer: 1/0)
   $data['cvterm']['is_relationshiptype'] = array(
     'title' => 'Is Relationship',
     'help' => 'Whether this term describes a relationship or not.',
     'field' => array(
       'handler' => 'views_handler_field_boolean',
       'click sortable' => TRUE,
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_chado_boolean',
       'label' => t('Is Relationship Type?'),
       'type' => 'yes-no',
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
    );
  
  return $data;

}