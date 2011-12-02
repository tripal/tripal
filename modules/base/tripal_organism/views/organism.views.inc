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
 *
 * @ingroup tripal_organism_views
 */
function retrieve_organism_views_data() {
	global $db_url;
	$data = array();
	
	// if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     $database = 'chado';
  }

	
  // Basic table definition
  $data['organism']['table']['group'] = 'Chado Organism';
  $data['organism']['table']['base'] = array(
    'field' => 'organism_id',
    'title' => 'Chado Organism',
    'help' => 'Organisms existing in the Chado Database',                                                                                                                                                                          
  );
  if($database){
     $data['organism']['table']['base']['database'] = $database;
  }

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
     'argument' => array(
     	 'handler' => 'views_handler_argument',
     ),
  );

  // Calculated Field: Node ID
  //  use custom field handler to query drupal for the node ID
  //  this is only needed if chado is in a separate database from drupal
  if($database){
	  $data['organism']['organism_nid'] = array(
  	  'title' => 'Node ID',
    	'help' => 'This is the node ID of this organism. It can be used as a link to the node.',
    	'field' => array(
      	'handler' => 'views_handler_field_computed_organism_nid',
    	),    
  	);
  } else {
    // Add relationship between chado_organism and organism
    $data['organism']['organism_id'] = array(
      'group' => 'Organism',
      'title' => 'Organism Node',
      'help' => 'Links Chado Organism Fields/Data to the Nodes in the current View.',
      'real field' => 'organism_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Organism => Chado'),
        'label' => t('Organism => Chado'),
        'real field' => 'organism_id',
        'base' => 'chado_organism',
        'base field' => 'organism_id'
      ),
    );

  }
	
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
       'handler' => 'views_handler_filter_chado_select_string',
     ),
     'argument' => array(
       'handler' => 'views_handler_argument_string',
     ),
  );
  // if joined to the node table add a "Link to Node" option for the field  
  if (!$database) {
    $data['organism']['abbreviation']['field']['handler'] = 'views_handler_field_node_optional';
  }

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
       'handler' => 'views_handler_filter_chado_select_string',
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
       'handler' => 'views_handler_filter_chado_select_string',
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
       'handler' => 'views_handler_filter_organism_common_name',
     ),
     'argument' => array(
       'handler' => 'views_handler_argument_string',
     ),
  );
  // if joined to the node table add a "Link to Node" option for the field  
  if (!$database) {
    $data['organism']['common_name']['field']['handler'] = 'views_handler_field_node_optional';
  }

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
  
  //Calculated Field: Count (Int)
  // Provides the number of features for a given organism
  // @see tripal_feature/views/misc_tables.views.inc

  //Calculated Field: Count (Int)
  // Provides the number of stocks for a given organism
  // @see tripal_stock/views/misc_tables.views.inc

  //Calculated Field: Number of Libraries (Count -Int)
  // Provides the number of libraries for a given organism
  // @see tripal_library/views/misc_tables.views.inc

  return $data;

}
