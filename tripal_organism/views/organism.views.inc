<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the organism table, it's fields and any joins between it and other tables
 * @see tripal_organism_views_data() --in tripal_organism.views.inc
 *
 * @todo Add support for the following tables: organismprop, organism_dbxref
 * @todo Add join to node table within if <chado/drupal same db>; also addd if not around nid field
 *
 * BASE TABLE: organism
 * @code
 * create table organism (
 *        organism_id serial not null,
 *        primary key (organism_id),
 *        abbreviation varchar(255) null,
 *        genus varchar(255) not null,
 *        species varchar(255) not null,
 *        common_name varchar(255) null,
 *        comment text null,
 *        constraint organism_c1 unique (genus,species)
 * );
 * @endcode
 */
function retrieve_organism_views_data() {

  // Basic table definition
  $data['organism']['table']['group'] = 'Chado Organism';
  $data['organism']['table']['base'] = array(
    'field' => 'organism_id',
    'title' => 'Chado Organism',
    'help' => 'Organisms existing in the Chado Database',                                                                                                                                                                          
    'database' => 'chado'
  );

  // Define relationships between this table and others
  $data['organism']['table']['join'] = array(
    'feature' => array(
      'left_field' => 'organism_id',
      'field' => 'organism_id',
    ),
    'library' => array(
      'left_field' => 'organism_id',
      'field' => 'organism_id',
    ),
    'stock' => array(
      'left_field' => 'organism_id',
      'field' => 'organism_id',
    ),
  ); 

  // Table Field Definitions----------------------
  // Field: organism_id (primary key)
  $data['organism']['organism_id'] = array(
    'title' => 'Organism ID',
    'help' => 'The primary key of the organism.',
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

  // Calculated Field: Node ID
  //  use custom field handler to query drupal for the node ID
  //  this is only needed if chado is in a separate database from drupal
  $data['organism']['nid'] = array(
    'title' => 'Node ID',
    'help' => 'This is the node ID of this organism. It can be used as a link to the node.',
    'field' => array(
      'handler' => 'views_handler_field_computed_nid',
    ),    
  );

  // Field: abbreviation (varchar 255)
  $data['organism']['abbreviation'] = array(
    'title' => 'Abbreviation',
    'help' => 'The abbreviation of the organism name ie: A.thaliana.',
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

  // Field: genus (varchar 255)
  $data['organism']['genus'] = array(
    'title' => 'Genus',
    'help' => 'The genus portion of the organism\'s scientific name',
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

  // Field: species (varchar 255)
  $data['organism']['species'] = array(
    'title' => 'Species',
    'help' => 'The species portion of the organism\'s scientific name',
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

  // Field: common name (varchar 255)
  $data['organism']['common_name'] = array(
    'title' => 'Common Name',
    'help' => 'The common name of the organism.',
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

  // Field: Comment (text)
  $data['organism']['comment'] = array(
    'title' => 'Comment',
    'help' => 'A free-text comment about the organism',
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