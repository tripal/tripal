<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the feature table, it's fields and any joins between it and other tables
 * @see tripal_feature_views_data() --in tripal_feature.views.inc
 *
 * @todo Add better handler for is_analysis, is_obsolete: something which changes the t/f to a true boolean
 * @todo Add support for the following tables: featureprop, featureloc, featurepos, feature_synonym, feature_relationship
 * @todo Add join to node table within if <chado/drupal same db>; also addd if not around nid field
 *
 * BASE TABLE: feature
 * @code
 *   create table feature (
 *        feature_id serial not null,
 *        primary key (feature_id),
 *        dbxref_id int,
 *        foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
 *        organism_id int not null,
 *        foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
 *        name varchar(255),
 *        uniquename text not null,
 *        residues text,
 *        seqlen int,
 *        md5checksum char(32),
 *        type_id int not null,
 *        foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
 *        is_analysis boolean not null default 'false',
 *        is_obsolete boolean not null default 'false',
 *        timeaccessioned timestamp not null default current_timestamp,
 *        timelastmodified timestamp not null default current_timestamp,
 *        constraint feature_c1 unique (organism_id,uniquename,type_id)
 *   );
 * @endcode
 *
 * @ingroup tripal_feature_views
 */
 function retrieve_feature_views_data() {
	global $db_url;
	$data = array();
	
  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     $database = 'chado';
  }

	
   // Basic table definition
   $data['feature']['table']['group'] = 'Chado Feature';
   $data['feature']['table']['base'] = array(
     'field' => 'feature_id',
     'title' => 'Chado Features',
     'help' => 'Features are Sequence Data Records in Chado.',
   );

	if($database){
		$data['feature']['table']['base']['database'] = $database;
	}
	

  //Relationship Definitions---------------------------------
  //Join: feature => nd_reagent
  $data['feature']['table']['join']['nd_reagent'] = array(
    'left_field' => 'feature_id',
    'field' => 'feature_id',
  );  

  // Table Field Definitions----------------------
  // Field: feature_id (primary key)
  $data['feature']['feature_id'] = array(
    'title' => 'Feature ID',
    'help' => 'The primary key of a feature',
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
  	$data['feature']['feature_nid'] = array(
    	'title' => 'Node ID',
			'help' => 'This is the node ID of this feature. It can be used as a link to the node.',
    	'field' => array(
      	'handler' => 'views_handler_field_computed_feature_nid',
      ),
  	);
  } else {
    // Add relationship between chado_feature and feature
    $data['feature']['feature_nid'] = array(
      'group' => 'Feature',
      'title' => 'Feature Node',
      'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
      'real field' => 'feature_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Feature => Chado'),
        'label' => t('Feature => Chado'),
        'real field' => 'feature_id',
        'base' => 'chado_feature',
        'base field' => 'feature_id'
      ),
    );

  }
  
  // Field: organism_id (forgeign key)
  //  join between organism table and this one in tripal_organism/views/organism.views.inc
  
  // Field: dbxref_id (forgeign key)
  //  join between dbxref table and this one in tripal_db/views/dbxref.views.inc
  
  // Field: type_id (forgeign key)
  //  join between cvterm table and this one in tripal_cv/views/cvterm.views.inc
  
  // Field: name (varchar 255)
  $data['feature']['name'] = array(
    'title' => 'Name',
    'help' => 'The human-readable, non-unique name of a feature.',
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
  // if joined to the node table add a "Link to Node" option for the field  
  if (!$database) {
    $data['feature']['name']['field']['handler'] = 'views_handler_field_node_optional';
  }

  // Field: unique name (text)
  $data['feature']['uniquename'] = array(
    'title' => 'Unique Name',
    'help' => 'The unique name of a feature.',
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
  // if joined to the node table add a "Link to Node" option for the field  
  if (!$database) {
    $data['feature']['uniquename']['field']['handler'] = 'views_handler_field_node_optional';
  }

  // Field: residues (text)
  $data['feature']['residues'] = array(
    'title' => 'Residues',
    'help' => 'The sequence of a feature.',
    'field' => array(
      'handler' => 'views_handler_field_residues',
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

  // Field: sequence length (integer)
  $data['feature']['seqlen'] = array(
    'title' => 'Sequence Length',
    'help' => 'The length of the sequence',
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

  // Field: is analysis (boolean -t/f)
  $data['feature']['is_analysis'] = array(
    'title' => 'Is Analysis',
    'help' => 'A boolean indicating whether this feature was annotated by means of automated analysis.',
    'field' => array(
       'handler' => 'views_handler_field_chado_tf_boolean',
       'click sortable' => TRUE,
       'label' => t('Is Analysis?'),
       'type' => 'yes-no',
    ),
    'filter' => array(
       'handler' => 'views_handler_filter_chado_boolean',
       'label' => t('Is Analysis?'),
       'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Field: is obsolete (boolean -t/f)
  $data['feature']['is_obsolete'] = array(
    'title' => 'Is Obsolete',
    'help' => 'A boolean indicating whether this feature is obsolete.',
    'field' => array(
       'handler' => 'views_handler_field_chado_tf_boolean',
       'click sortable' => TRUE,
       'label' => t('Is Obsolete?'),
       'type' => 'yes-no',
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
  $data['feature']['timeaccessioned'] = array(
    'title' => 'Time Accessioned',
    'help' => 'The date & time when this feature was accessioned (added into the database)',
    'field' => array(
      'handler' => 'views_handler_field_readable_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  // Field: time last modified (datetime)
  $data['feature']['timelastmodified'] = array(
    'title' => 'Time Last Modified',
    'help' => 'The date & time when this feature was last modified.',
    'field' => array(
      'handler' => 'views_handler_field_readable_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  // Calculated Field: Number of Analysis' (Count -Int)
  // Provides the number of analysis' for a given feature
  // @see tripal_analysis/views/misc_tables.views.inc
  
  //Calculated Field: Number of Libraries (Count -Int)
  // Provides the number of libraries for a given feature
  // @see tripal_library/views/misc_tables.views.inc 

	// Calculated Field: feature relationships
	// uses a custom field handler which pulls results from the view
	$data['feature']['relationships'] = array(
    'title' => t('Feature Relationships'),
    'help' => t('Relationships including the current feature.'),
    'field' => array(
      'title' => t('Relationships'),
      'help' => t('Display a given type of relationships including the current feature.'),
      'handler' => 'views_handler_field_chado_rel_by_type',
    ),
  );  
  return $data;
 }
