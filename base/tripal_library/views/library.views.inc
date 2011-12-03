<?php
	/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the library table, it's fields and any joins between it and other tables
 * @see tripal_library_views_data() --in tripal_library.views.inc
 *
 * @todo Add support for the following tables: library_cvterm, library_pub, library_synonym, libraryprop
 * @todo Add support for multiple libraries listed per feature
 * @todo Add join to node table within if <chado/drupal same db>; also addd if not around nid field
 *
 * BASE TABLE: library
 * @code
 * create table library (
 *    library_id serial not null,
 *    primary key (library_id),
 *    organism_id int not null,
 *    foreign key (organism_id) references organism (organism_id),
 *    name varchar(255),
 *    uniquename text not null,
 *    type_id int not null,
 *    foreign key (type_id) references cvterm (cvterm_id),
 *    is_obsolete int not null default 0,
 *    timeaccessioned timestamp not null default current_timestamp,
 *    timelastmodified timestamp not null default current_timestamp,
 *    constraint library_c1 unique (organism_id,uniquename,type_id)
 * );
 * @endcode
 *
 * @ingroup tripal_library_views
 */
function retrieve_library_views_data() {
	global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     $database = 'chado';
  }
  
  // Basic table definition
	$data['library']['table']['group'] = 'Chado Library';
	$data['library']['table']['base'] = array(
	  'field' => 'library_id',
	  'title' => 'Chado Library',
	  'help' => 'Library existing in the Chado Database',                                                                                                                                                                          
	);
	if($database){
	  $data['library']['table']['base']['database'] = $database;
	}
	
	// Define relationships between this table and others
  $data['library']['table']['join'] = array(
    'library_feature' => array(
      'left_field' => 'library_id',
      'field' => 'library_id',
    ),
    'feature' => array(
      'left_table' => 'library_feature',
      'left_field' => 'library_id',
      'field' => 'library_id',
    ),
  );

  // Describe the joins with the library_feature table
  $data['library_feature']['table']['join'] = array(
    'feature' => array(
      'left_field' => 'feature_id',
      'field' => 'feature_id',
    ),
  );
	
	// Table Field Definitions----------------------
	// Field: library_id (primary key)
	$data['library']['library_id'] = array(
	  'title' => 'Library ID',
	  'help' => 'The primary key of the library table.',
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
  if($database){
		$data['library']['library_nid'] = array(
	  	'title' => 'Node ID',
	  	'help' => 'The node ID for the current library',
	  	'field' => array(
	    	'handler' => 'views_handler_field_computed_library_nid',
	  	),
		);
  } else {
    // Add relationship between chado_library and library
    $data['library']['library_nid'] = array(
      'group' => 'Library',
      'title' => 'Library Node',
      'help' => 'Links Chado Library Fields/Data to the Nodes in the current View.',
      'real field' => 'library_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Library => Chado'),
        'label' => t('Library => Chado'),
        'real field' => 'library_id',
        'base' => 'chado_library',
        'base field' => 'library_id'
      ),
    );

  }
	
	// Field: organism_id (forgeign key)
  //  join between organism table and this one in tripal_organism/views/organism.views.inc
	
	// Field: type_id (forgeign key)
  //  join between cvterm table and this one in tripal_cv/views/cvterm.views.inc
  
	// Field: Name (varchar 255)
	$data['library']['name'] = array(
	  'title' => 'Name',
	  'help' => 'The human-readable name of the current library.',
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
    $data['library']['name']['field']['handler'] = 'views_handler_field_node_optional';
  }
	
	// Field: Unique name (text)
	$data['library']['uniquename'] = array(
	  'title' => 'Unique Name',
	  'help' => 'The unique name of the current library.',
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
  // if joined to the node table add a "Link to Node" option for the field  
  if (!$database) {
    $data['library']['uniquename']['field']['handler'] = 'views_handler_field_node_optional';
  }
	
	// Field: Is obsolete (integer 0/1)
	$data['library']['is_obsolete'] = array(
	  'title' => t('Is Obsolete?'),
	  'help' => t('Indicates whether a given library is obsolete or not.'),
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
	
	// Field: time accessioned (datetime)
	$data['library']['timeaccessioned'] = array(
	  'title' => t('Date Accessioned'),
	  'help' => t('Indicates the date a given library was accessioned (entered into the database).'),
	  'field' => array(
	    'handler' => 'views_handler_field_readable_date',
	    'click sortable' => TRUE,
	  ),
	  'sort' => array(
	    'handler' => 'views_handler_sort_date',
	  ),
	);
	
	// Field: time last modified (datetime)
	$data['library']['timelastmodified'] = array(
	  'title' => t('Date Last Modified'),
	  'help' => t('Indicates the date that a given library was last modified.'),
	  'field' => array(
	    'handler' => 'views_handler_field_readable_date',
	    'click sortable' => TRUE,
	  ),
	  'sort' => array(
	    'handler' => 'views_handler_sort_date',
	  ),
	);
	
	return $data;
	
}