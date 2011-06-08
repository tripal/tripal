<?php

/*************************************************************************
 * tripal_search_unigene_views_data()
 */

function tripal_search_unigene_views_data()  {
	global $db_url;
	$data = array();
	
	// if the chado database is not local to the drupal database
	// then we need to set the database name.  This should always
	// be 'chado'.
	if(is_array($db_url) and array_key_exists('chado', $db_url)) {
		$database = 'chado';
	}
	
 	// Basic table definition
	$data['unigene_for_search']['table']['group'] = 'Tripal Search feature';
	$data['unigene_for_search']['table']['base'] = array(
		'field' => 'unigene_for_search_id',
		'title' => t('Chado Feature with searchable annotations'),
		'help'  => t('A materialized view for feature is generated to enable the search for different annotations'),
	);
 	if ($database) {
 		$data['unigene_for_search']['table']['base']['database'] = $database;
	}
	
	// Table Field Definitions----------------------
	// Field: feature_id (integer)
	$data['unigene_for_search']['unigene_for_search_id'] = array(
		'title' => t('unigene_for_search ID'),
		'help' => t('The primary key of unigene_for_search'),
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
	$data['unigene_for_search']['feature_id'] = array(
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
	$data['unigene_for_search']['feature_type'] = array(
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
	$data['unigene_for_search']['feature_name'] = array(
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
	$data['unigene_for_search']['feature_uniquename'] = array(
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
	$data['unigene_for_search']['feature_is_obsolete'] = array(
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
  $data['unigene_for_search']['organism_common_name'] = array(
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
	$data['unigene_for_search']['feature_seqlen'] = array(
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
	$data['unigene_for_search']['go_term'] = array(
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
	$data['unigene_for_search']['unigene'] = array(
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
	$data['unigene_for_search']['blast_value'] = array(
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
	$data['unigene_for_search']['kegg_value'] = array(
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
	$data['unigene_for_search']['interpro_value'] = array(
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
 * tripal_search_unigene_views_handlers
 */
function tripal_search_unigene_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_search_unigene'),
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

/*************************************************************************
 * tripal_search_unigene_views_default_views
 */
function tripal_search_unigene_views_default_views () {
	$views = array();
	$view = new view;
	$view->name = 'unigene_search';
	$view->description = 'Unigene Search';
	$view->tag = '';
	$view->view_php = '';
	$view->base_table = 'unigene_for_search';
	$view->is_cacheable = FALSE;
	$view->api_version = 2;
	$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
	$handler = $view->new_display('default', 'Defaults', 'default');
	$handler->override_option('fields', array(
  'counter' => array(
    'label' => '#',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'counter_start' => '1',
    'exclude' => 0,
    'id' => 'counter',
    'table' => 'views',
    'field' => 'counter',
    'relationship' => 'none',
	),
  'feature_id' => array(
    'label' => 'feature ID',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => '',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 1,
    'id' => 'feature_id',
    'table' => 'unigene_for_search',
    'field' => 'feature_id',
    'relationship' => 'none',
	),
  'feature_name' => array(
    'label' => 'Sequence',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 1,
      'path' => 'ID[feature_id]',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'feature_name',
    'table' => 'unigene_for_search',
    'field' => 'feature_name',
    'relationship' => 'none',
	),
  'organism_common_name' => array(
    'label' => 'Organism',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'organism_common_name',
    'table' => 'unigene_for_search',
    'field' => 'organism_common_name',
    'relationship' => 'none',
	),
  'feature_seqlen' => array(
    'label' => 'Length',
    'alter' => array(
      'alter_text' => 1,
      'text' => '[feature_seqlen] bp',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => ',',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'feature_seqlen',
    'table' => 'unigene_for_search',
    'field' => 'feature_seqlen',
    'relationship' => 'none',
	),
  'feature_type' => array(
    'label' => 'Type',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'feature_type',
    'table' => 'unigene_for_search',
    'field' => 'feature_type',
    'relationship' => 'none',
	),
  'go_term' => array(
    'label' => 'GO Term',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'go_term',
    'table' => 'unigene_for_search',
    'field' => 'go_term',
    'relationship' => 'none',
	),
  'blast_value' => array(
    'label' => 'Blast',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 1,
      'max_length' => '200',
      'word_boundary' => 0,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'blast_value',
    'table' => 'unigene_for_search',
    'field' => 'blast_value',
    'relationship' => 'none',
	),
  'kegg_value' => array(
    'label' => 'KEGG',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 1,
      'max_length' => '200',
      'word_boundary' => 0,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'kegg_value',
    'table' => 'unigene_for_search',
    'field' => 'kegg_value',
    'relationship' => 'none',
	),
  'interpro_value' => array(
    'label' => 'Interpro',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 1,
      'max_length' => '200',
      'word_boundary' => 0,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
	),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'interpro_value',
    'table' => 'unigene_for_search',
    'field' => 'interpro_value',
    'relationship' => 'none',
	),
	));
	$handler->override_option('arguments', array(
  'null' => array(
    'default_action' => 'default',
    'style_plugin' => 'default_summary',
    'style_options' => array(),
    'wildcard' => 'all',
    'wildcard_substitution' => 'All',
    'title' => '',
    'breadcrumb' => '',
    'default_argument_type' => 'fixed',
    'default_argument' => '',
    'validate_type' => 'php',
    'validate_fail' => 'empty',
    'must_not_be' => 0,
    'id' => 'null',
    'table' => 'views',
    'field' => 'null',
    'validate_user_argument_type' => 'uid',
    'validate_user_roles' => array(
      '2' => 0,
      '3' => 0,
      '5' => 0,
      '4' => 0,
	),
    'relationship' => 'none',
    'default_options_div_prefix' => '',
    'default_argument_fixed' => '',
    'default_argument_user' => 0,
    'default_argument_php' => '',
    'validate_argument_node_type' => array(
      'iframe' => 0,
      'news' => 0,
      'chado_analysis' => 0,
      'chado_analysis_blast' => 0,
      'chado_analysis_interpro' => 0,
      'chado_analysis_kegg' => 0,
      'chado_analysis_unigene' => 0,
      'chado_feature' => 0,
      'chado_library' => 0,
      'chado_organism' => 0,
      'calendar_event' => 0,
      'page' => 0,
      'story' => 0,
	),
    'validate_argument_node_access' => 0,
    'validate_argument_nid_type' => 'nid',
    'validate_argument_vocabulary' => array(
      '17' => 0,
      '13' => 0,
      '14' => 0,
      '15' => 0,
      '16' => 0,
      '9' => 0,
      '10' => 0,
      '11' => 0,
      '12' => 0,
      '5' => 0,
      '6' => 0,
      '7' => 0,
      '8' => 0,
	),
    'validate_argument_type' => 'tid',
    'validate_argument_transform' => 0,
    'validate_user_restrict_roles' => 0,
    'validate_argument_php' => 'if (count($view->exposed_input)) {
return TRUE;
}',
	),
	));
	$handler->override_option('filters', array(
  'feature_name' => array(
    'operator' => 'contains',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'feature_name_op',
      'identifier' => 'feature_name',
      'label' => 'sequence name',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'feature_name',
    'table' => 'unigene_for_search',
    'field' => 'feature_name',
    'relationship' => 'none',
	),
  'feature_seqlen' => array(
    'operator' => '>=',
    'value' => array(
      'value' => '',
      'min' => '',
      'max' => '',
	),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'feature_seqlen_op',
      'identifier' => 'feature_seqlen',
      'label' => 'sequence length (bp)',
      'optional' => 1,
      'remember' => 0,
	),
    'id' => 'feature_seqlen',
    'table' => 'unigene_for_search',
    'field' => 'feature_seqlen',
    'relationship' => 'none',
	),
  'feature_type' => array(
    'operator' => '=',
    'value' => '260',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'feature_type_op',
      'identifier' => 'feature_type',
      'label' => 'type of sequence',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'feature_type',
    'table' => 'unigene_for_search',
    'field' => 'feature_type',
    'relationship' => 'none',
	),
  'go_term' => array(
    'operator' => 'contains',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'go_term_op',
      'identifier' => 'go_term',
      'label' => 'GO term',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'go_term',
    'table' => 'unigene_for_search',
    'field' => 'go_term',
    'relationship' => 'none',
	),
  'organism_common_name' => array(
    'operator' => '=',
    'value' => 'chickpea',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'organism_common_name_op',
      'identifier' => 'organism_common_name',
      'label' => 'organism',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'organism_common_name',
    'table' => 'unigene_for_search',
    'field' => 'organism_common_name',
    'relationship' => 'none',
	),
  'unigene' => array(
    'operator' => '=',
    'value' => 'Cicer arietinum unigene v1',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'unigene_op',
      'identifier' => 'unigene',
      'label' => 'belongs to unigene',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'unigene',
    'table' => 'unigene_for_search',
    'field' => 'unigene',
    'relationship' => 'none',
	),
  'interpro_value' => array(
    'operator' => 'contains',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'interpro_value_op',
      'identifier' => 'interpro_value',
      'label' => 'interpro description',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'interpro_value',
    'table' => 'unigene_for_search',
    'field' => 'interpro_value',
    'relationship' => 'none',
	),
  'blast_value' => array(
    'operator' => 'contains',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'blast_value_op',
      'identifier' => 'blast_value',
      'label' => 'blast description',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'blast_value',
    'table' => 'unigene_for_search',
    'field' => 'blast_value',
    'relationship' => 'none',
	),
  'kegg_value' => array(
    'operator' => 'contains',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'kegg_value_op',
      'identifier' => 'kegg_value',
      'label' => 'kegg description',
      'optional' => 1,
      'remember' => 0,
	),
    'case' => 0,
    'id' => 'kegg_value',
    'table' => 'unigene_for_search',
    'field' => 'kegg_value',
    'relationship' => 'none',
	),
	));
	$handler->override_option('access', array(
  'type' => 'none',
	));
	$handler->override_option('cache', array(
  'type' => 'none',
	));
	$handler->override_option('title', 'Unigene Search');
	$handler->override_option('css_class', 'tripal-search-feature');
	$handler->override_option('empty', '<?php
$view = views_get_current_view();
  if (count($view->exposed_input)) {
    drupal_set_message("Your search returns no results. Please adjust your search criteria and try again.");
} 
?>');
	$handler->override_option('empty_format', '3');
	$handler->override_option('use_pager', '1');
	$handler->override_option('style_plugin', 'table');
	$handler->override_option('style_options', array(
  'grouping' => '',
  'override' => 1,
  'sticky' => 0,
  'order' => 'asc',
  'columns' => array(
    'feature_id' => 'feature_id',
    'feature_name' => 'feature_name',
    'feature_seqlen' => 'feature_seqlen',
    'feature_type' => 'feature_type',
    'go_term' => 'go_term',
    'organism_common_name' => 'organism_common_name',
    'unigene' => 'unigene',
    'blast_value' => 'blast_value',
    'kegg_value' => 'kegg_value',
    'interpro_value' => 'interpro_value',
	),
  'info' => array(
    'feature_id' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'feature_name' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'feature_seqlen' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'feature_type' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'go_term' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'organism_common_name' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'unigene' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'blast_value' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'kegg_value' => array(
      'sortable' => 1,
      'separator' => '',
	),
    'interpro_value' => array(
      'sortable' => 1,
      'separator' => '',
	),
	),
  'default' => '-1',
	));
	$handler = $view->new_display('page', 'Page', 'page_1');
	$handler->override_option('path', 'unigene_search');
	$handler->override_option('menu', array(
  'type' => 'normal',
  'title' => 'Unigene Search',
  'description' => 'Advanced Search for unigenes',
  'weight' => '0',
  'name' => 'primary-links',
	));
	$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
	));
  $views[$view->name] = $view;
  
  return $views;
}
