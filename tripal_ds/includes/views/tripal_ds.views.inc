<?php



/**
 * Implements hook_views_data().
 */
function tripal_ds_views_data() {

    $data = array();

    $data['tripal_ds']['table']['group'] = t('Tripal Panes');

    $data['tripal_ds']['table']['base'] = array(
        'title' => t('Tripal Panes'),
        'help' => t('Contains Tripal Pane records we want exposed to Views.'),
    );

    $data['tripal_ds']['bundle'] = array(
	   'title' => t('Bundle ID'),
	   'help' => t('This the unque bundle name, eg bio_data_1.'),
		'relationship' => array(
		      'base' => 'tripal_entity', // The name of the table to join with.
		      'base field' => 'bundle', // The name of the field on the joined table.
		      // 'field' => 'nid' -- see hook_views_data_alter(); not needed here.
		      'handler' => 'views_handler_relationship',
		      'label' => t('Default label for the relationship'),
		      'title' => t('Title shown when adding the relationship'),
		      'help' => t('More information on this relationship'),
		    ),    	
		'filter' => array(
      		'handler' => 'views_handler_filter_bundle',
     	),
    	'argument' => array(
      		'handler' => 'views_handler_argument_string',
    	),
    	'field' => array(
      		'handler' => 'views_handler_field',
    	),
  	);

    $data['tripal_ds']['field_name'] = array(
	   'title' => t('Field Name'),
	   'help' => t('This the field name, it is not necessarily unique, like "summary" which might feature across several bundles.'),
	   'field' => array(
      		'handler' => 'views_handler_field',
      		'click sortable' => TRUE, // This is use by the table display plugin.
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

    $data['tripal_ds']['field_label'] = array(
	   'title' => t('Field Label'),
	   'help' => t('The human readable name of the field, like "Data Sequences".'),
	   'field' => array(
      		'handler' => 'views_handler_field',
      		'click sortable' => TRUE, // This is use by the table display plugin.
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

