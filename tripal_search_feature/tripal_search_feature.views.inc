<?php

/*************************************************************************
 * tripal_search_feature_views_data()
 */

function tripal_search_feature_views_data()  {
	global $db_url;
	$data = array();
	
	// if the chado database is not local to the drupal database
	// then we need to set the database name.  This should always
	// be 'chado'.
	if(is_array($db_url) and array_key_exists('chado', $db_url)) {
		$database = 'chado';
	}
	
 	// Basic table definition
	$data['feature_for_search']['table']['group'] = 'Tripal Search feature';
	$data['feature_for_search']['table']['base'] = array(
		'field' => 'feature_for_search_id',
		'title' => t('Chado Feature with searchable annotations'),
		'help'  => t('A materialized view for feature is generated to enable the search for different annotations'),
	);
 	if ($database) {
 		$data['feature_for_search']['table']['base']['database'] = $database;
	}
	
	// Table Field Definitions----------------------
	// Field: feature_id (integer)
	$data['feature_for_search']['feature_for_search_id'] = array(
		'title' => t('feature_for_search ID'),
		'help' => t('The primary key of feature_for_search'),
		'field' => array(
			'handler' => 'views_handler_field_numeric',
			'click sortable' => TRUE,
		),
		'sort' => array(
			'handler' => 'views_handler_sort',
		),
		'filter' => array(
			'handler' => 'views_handler_filter_numeric',
		),
	);
			
	// Field: feature_id (integer)
	$data['feature_for_search']['feature_id'] = array(
		'title' => t('feature ID'),
		'help' => t('The primary key of a feature'),
		'field' => array(
			'handler' => 'views_handler_field_numeric',
			'click sortable' => TRUE,
		),
		'sort' => array(
			'handler' => 'views_handler_sort',
		),
		'filter' => array(
			'handler' => 'views_handler_filter_numeric',
		),
	);
	
	// Field: name (character varying(1024))
	$data['feature_for_search']['feature_type'] = array(
		'title' => t('feature_type'),
		'help' => t('Type of the feature'),
		'field' => array(
			'handler' => 'views_handler_field',
			'click sortable' => TRUE,
		),
		'sort' => array(
			'handler' => 'views_handler_sort',
		),
		'filter' => array(
			'handler' => 'views_handler_filter_feature_type_id',
		),
		'argument' => array(
			'handler' => 'views_handler_argument_string',
		),
	);
	
	// Field: name (character varying(255),)
	$data['feature_for_search']['feature_name'] = array(
		'title' => t('feature_name'),
		'help' => t('The optional human-readable common name for a feature, for display purposes.'),
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
	
		// Field: uniquename (text)
	$data['feature_for_search']['feature_uniquename'] = array(
		'title' => t('feature_uniquename'),
		'help' => t('The unique name for a feature; may not be necessarily be particularly human-readable, although this is preferred. This name must be unique for this type of feature within this organism.'),
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
	
	// Field: is_obsolete (boolean)
	$data['feature_for_search']['feature_is_obsolete'] = array(
		'title' => t('feature_is_obsolete'),
		'help' => t('Boolean indicating whether this feature has been obsoleted. Some chado instances may choose to simply remove the feature altogether, others may choose to keep an obsolete row in the table.'),
		'field' => array(
			'handler' => 'views_handler_field',
			'click sortable' => TRUE,
		),
		'sort' => array(
			'handler' => 'views_handler_sort',
		),
		'filter' => array(
			'handler' => 'views_handler_filter_boolean_operator_string',
		),
	);
	
	// Field: common name (varchar 255)
  $data['feature_for_search']['organism_common_name'] = array(
    'title' => 'organism_common_name',
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
  
  	// Field: seqlen (integer)
	$data['feature_for_search']['feature_seqlen'] = array(
		'title' => t('feature_seqlen'),
		'help' => t('The length of the residue feature. See column:residues. This column is partially redundant with the residues column, and also with featureloc. This column is required because the location may be unknown and the residue sequence may not be manifested, yet it may be desirable to store and query the length of the feature. The seqlen should always be manifested where the length of the sequence is known.'),
		'field' => array(
			'handler' => 'views_handler_field_numeric',
			'click sortable' => TRUE,
		),
		'sort' => array(
			'handler' => 'views_handler_sort',
		),
		'filter' => array(
			'handler' => 'views_handler_filter_numeric',
		),
	);
	
		// Field: name (character varying(1024))
	$data['feature_for_search']['go_term'] = array(
		'title' => t('go_term'),
		'help' => t('GO Terms associated with a feature'),
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
		
	// Field: name (character varying(255),)
	$data['feature_for_search']['unigene'] = array(
		'title' => t('unigene'),
		'help' => t('Unigene analysis name.'),
		'field' => array(
			'handler' => 'views_handler_field',
			'click sortable' => TRUE,
		),
		'sort' => array(
			'handler' => 'views_handler_sort',
		),
		'filter' => array(
			'handler' => 'views_handler_filter_analysis_name',
		),
		'argument' => array(
			'handler' => 'views_handler_argument_string',
		),
	);
	
		// Field: value (text,)
	$data['feature_for_search']['blast_value'] = array(
		'title' => t('value'),
		'help' => t(''),
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
	
		// Field: value (text,)
	$data['feature_for_search']['kegg_value'] = array(
		'title' => t('value'),
		'help' => t(''),
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
	
		// Field: value (text,)
	$data['feature_for_search']['interpro_value'] = array(
		'title' => t('value'),
		'help' => t(''),
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

/*************************************************************************
 * tripal_search_feature_views_handlers
 */
function tripal_search_feature_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_search_feature'),
   ),
   'handlers' => array(
   	'views_handler_filter_organism_common_name' => array(
   		'parent' => 'views_handler_filter_string'
   	),
   	'views_handler_filter_analysis_name' => array(
   		'parent' => 'views_handler_filter_string'
   	),
   	'views_handler_filter_feature_type_id' => array(
   		'parent' => 'views_handler_filter_numeric'
   	),
   ),
 );
}
