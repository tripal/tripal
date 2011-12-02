<?php


/**
* Developed by: Chad N.A Krilow at The University of Saskatchewan
* Contact:cnk046@mail.usask.ca
*/


/**
 * Implements hook_views_data(): Describe tripal tables & fields to views
 *
 * @return array
 *   A data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */ 
function tripal_gbrowse_views_data() {
  
  //array to be filled with views data
  $data = array();

  // Basic table definition
  $data['tripal_gbrowse_instances']['table']['group'] = 'Tripal GBrowse';
  $data['tripal_gbrowse_instances']['table']['base'] = array(
    'field' => 'gbrowse_id',
    'title' => 'Tripal GBrowse',
    'help' => 'A GMOD GBrowse instance registered with Tripal',                                                                                                                                                                          
  );

  // Join the node table to chado stock
  $data['node']['table']['join']['tripal_gbrowse_instances'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );

  // Field: database_name(varchar 255)
  $data['tripal_gbrowse_instances']['database_name'] = array(
    'title' => 'Database Name',
    'help' => 'The specified name of the Database',
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
  
    // Field: database_user(varchar 255)
  $data['tripal_gbrowse_instances']['database_user'] = array(
    'title' => 'Database User',
    'help' => 'The Registered User of the Database',
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
  
  // Field: user_password(varchar 255)
  $data['tripal_gbrowse_instances']['user_password'] = array(
    'title' => 'User Password',
    'help' => 'The user password',
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
  
  // Field: gbrowse_name(varchar 255)
  $data['tripal_gbrowse_instances']['gbrowse_name'] = array(
    'title' => 'GBrowse Instance Name',
    'help' => 'The Name of the Specified GBrowse Instance',
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
  
  // Field: gbrowse_link(varchar 255)
  $data['tripal_gbrowse_instances']['gbrowse_link'] = array(
    'title' => 'GBrowse Instance Link',
    'help' => 'The Link to the GBrowse Instance',
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
    
  // Field: config_file(varchar 255)
  $data['tripal_gbrowse_instances']['config_file'] = array(
    'title' => 'Configuration File',
    'help' => 'The Configuration File Associated with the GBrowse Instance',
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


/**
 * Implements hook_views_default_views(): Describe tripal tables & fields to views
 *
 * This function represents the "Default View" that the user will see when on the drupal site.
 *
 */
function tripal_gbrowse_views_default_views() {
  
  $views = array();
  
  $view = new view;

  $view->name = 'all_tripal_gbrowse_instances';
  $view->description = '';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'tripal_gbrowse_instances';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  
  $handler->override_option('fields', array(
  'gbrowse_name' => array(
    'label' => 'Instance Name',
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
    'id' => 'gbrowse_name',
    'table' => 'tripal_gbrowse_instances',
    'field' => 'gbrowse_name',
    'relationship' => 'none',
    'override' => array(
      'button' => 'Override',
    ),
  ),
  'database_name' => array(
    'label' => 'Database Name',
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
    'id' => 'database_name',
    'table' => 'tripal_gbrowse_instances',
    'field' => 'database_name',
    'relationship' => 'none',
  ),
  'gbrowse_link' => array(
    'label' => 'GBrowse Instance Link',
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
    'id' => 'gbrowse_link',
    'table' => 'tripal_gbrowse_instances',
    'field' => 'gbrowse_link',
    'relationship' => 'none',
  ),
));

$handler->override_option('filters', array(
  'database_name' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'database_name_op',
      'identifier' => 'database_name',
      'label' => 'GBrowse: Database Name',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'database_name',
    'table' => 'tripal_gbrowse_instances',
    'field' => 'database_name',
    'relationship' => 'none',
    'override' => array(
      'button' => 'Override',
    ),
  ),
  'gbrowse_name' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'gbrowse_name_op',
      'identifier' => 'gbrowse_name',
      'label' => 'GBrowse: Instance Name',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'gbrowse_name',
    'table' => 'tripal_gbrowse_instances',
    'field' => 'gbrowse_name',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'database_user' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'database_user_op',
      'identifier' => 'database_user',
      'label' => 'GBrowse: Database User',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'database_user',
    'table' => 'tripal_gbrowse_instances',
    'field' => 'database_user',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
));

  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access tripal_gbrowse content',
  ));

  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  
$handler->override_option('empty', 'No GBrowse instances match these criteria.');

$handler->override_option('empty_format', '1');

$handler->override_option('use_pager', '1');

$handler->override_option('style_plugin', 'table');

  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'gbrowse_name' => 'gbrowse_name',
      'database_name' => 'database_name',
      'gbrowse_link' => 'gbrowse_link',
    ),
    'info' => array(
      'gbrowse_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'database_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'gbrowse_link' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'gbrowse_name',
  ));

$handler = $view->new_display('page', 'Page', 'page_1');

  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'GBrowse Instance Name',
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
			'link_to_node' => 1,
			'exclude' => 0,
			'id' => 'title',
			'table' => 'node',
			'field' => 'title',
			'override' => array(
				'button' => 'Use default',
			),
			'relationship' => 'none',
		),
		'database_name' => array(
			'label' => 'Database Name',
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
			'id' => 'database_name',
			'table' => 'tripal_gbrowse_instances',
			'field' => 'database_name',
			'relationship' => 'none',
		),
		'gbrowse_link' => array(
			'label' => 'GBrowse Instance Link',
			'alter' => array(
				'alter_text' => 0,
				'text' => '',
				'make_link' => 1,
				'path' => '[gbrowse_link]',
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
			'hide_empty' => 1,
			'empty_zero' => 0,
			'exclude' => 0,
			'id' => 'gbrowse_link',
			'table' => 'tripal_gbrowse_instances',
			'field' => 'gbrowse_link',
			'relationship' => 'none',
			'override' => array(
				'button' => 'Use default',
			),
		),
		'changed' => array(
			'label' => 'Updated date',
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
			'date_format' => 'custom',
			'custom_date_format' => 'D, d M Y ',
			'exclude' => 0,
			'id' => 'changed',
			'table' => 'node',
			'field' => 'changed',
			'override' => array(
				'button' => 'Use default',
			),
			'relationship' => 'none',
		),
		'created' => array(
			'label' => 'Post date',
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
			'date_format' => 'custom',
			'custom_date_format' => 'D, d M Y ',
			'exclude' => 0,
			'id' => 'created',
			'table' => 'node',
			'field' => 'created',
			'override' => array(
				'button' => 'Use default',
			),
			'relationship' => 'none',
		),
		'edit_node' => array(
			'label' => 'Edit link',
			'alter' => array(
				'alter_text' => 0,
				'text' => '',
				'make_link' => 1,
				'path' => '[gbrowse_link]',
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
			'hide_empty' => 1,
			'empty_zero' => 0,
			'text' => '',
			'exclude' => 0,
			'id' => 'edit_node',
			'table' => 'node',
			'field' => 'edit_node',
			'override' => array(
				'button' => 'Use default',
			),
			'relationship' => 'none',
		),
	));
	$handler->override_option('path', 'gbrowse_instances');
	$handler->override_option('menu', array(
		'type' => 'normal',
		'title' => 'GBrowse Instances',
		'description' => '',
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