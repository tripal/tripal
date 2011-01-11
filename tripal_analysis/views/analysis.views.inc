<?php
	/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the analysis table, it's fields and any joins between it and other tables
 * @see tripal_analysis_views_data() --in tripal_analysis.views.inc
 *
 * @todo Add support for analysisprop table
 * @todo Add support for multiple analysis' listed per feature
 * @todo Add join to node table within if <chado/drupal same db>; also addd if not around nid field
 *
 * BASE TABLE: analysis
 * @code
 *   create table analysis (
 *       analysis_id serial not null,
 *       primary key (analysis_id),
 *       name varchar(255),
 *       description text,
 *       program varchar(255) not null,
 *       programversion varchar(255) not null,
 *       algorithm varchar(255),
 *       sourcename varchar(255),
 *       sourceversion varchar(255),
 *       sourceuri text,
 *       timeexecuted timestamp not null default current_timestamp,
 *       constraint analysis_c1 unique (program,programversion,sourcename)
 *   );
 * @endcode
 */
function retrieve_analysis_views_data() {
  
  // Basic table definition
  $data['analysis']['table']['group'] = 'Chado Analysis';
 	$data['analysis']['table']['base'] = array(
 		'field' => 'analysis_id',
 		'title' => t('Chado Analysis'),
 		'help' => t("An analysis is a particular type of a computational analysis; it may be a blast of one sequence against another, or an all by all blast, or a different kind of analysis altogether. It is a single unit of computation."),
 		'database' => 'chado',
 	);

  // Define relationships between this table and others
  $data['analysis']['table']['join'] = array(
    'analysisfeature' => array(
      'left_field' => 'analysis_id',
      'field' => 'analysis_id',
    ),
    'feature' => array(
      'left_table' => 'analysisfeature',
      'left_field' => 'analysis_id',
      'field' => 'analysis_id',
    ),
  );

  // Describe the joins with the analysis_feature table
  $data['analysisfeature']['table']['join'] = array(
    'feature' => array(
      'left_field' => 'feature_id',
      'field' => 'feature_id',
    ),
  );
	
	// Table Field Definitions----------------------
	// Field: analysis_id (primary key)
	$data['analysis']['analysis_id'] = array(
	  'title' => 'analysis ID',
	  'help' => 'The primary key of the analysis table.',
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
	$data['analysis']['nid'] = array(
	  'title' => 'Node ID',
	  'help' => 'The node ID for the current analysis',
	  'field' => array(
	    'handler' => 'views_handler_field_computed_nid',
	  ),
	);
	
	// Field: name (varchar 255)
 	$data['analysis']['name'] = array(
    'title' => t('Name'),
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

  // Field: description (text)
 	$data['analysis']['description'] = array(
    'title' => t('Description'),
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

  // Field: program (varchar 255)
 	$data['analysis']['program'] = array(
    'title' => t('Program'),
    'help' => t('Program name, e.g. blastx, blastp, sim4, genscan.'),
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

  // Field: program version (varchar 255)
 	$data['analysis']['programversion'] = array(
    'title' => t('Program Version'),
    'help' => t('Version description, e.g. TBLASTX 2.0MP-WashU [09-Nov-2000].'),
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

  // Field: algorithm (varchar 255)
 	$data['analysis']['algorithm'] = array(
    'title' => t('Algorithm'),
    'help' => t('Algorithm name, e.g. blast.'),
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

 	$data['analysis']['sourcename'] = array(
    'title' => t('Source Name'),
    'help' => t('Source name, e.g. cDNA, SwissProt.'),
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

  // Field: source version (varchar 255)
 	$data['analysis']['sourceversion'] = array(
    'title' => t('Source Version'),
    'help' => t('The version of the source.'),
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

  // Field: source URI/URL (text)
 	$data['analysis']['sourceuri'] = array(
    'title' => t('Source URL'),
    'help' => t('This is an optional, permanent URL or URI for the source of the analysis.'),
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

  // Field: time executed (datetime)
 	$data['analysis']['timeexecuted'] = array(
    'title' => 'Time Executed',
    'help' => 'The date & time when this analysis was executed.',
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