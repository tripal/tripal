<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the stock table, it's fields and any joins between it and other tables
 * @see tripal_stock_views_data() --in tripal_stock.views.inc
 *
 * @todo add better support for is_obsolete -through a field handler
 * @todo Add support for the following tables: stock_cvterm, stock_pub, stock_genotype
 * @todo Add join to node table within if <chado/drupal same db>; also addd if not around nid field
 *
 * BASE TABLE: stock
 * @code
      create table stock (
        stock_id serial not null,
        primary key (stock_id),
        dbxref_id int,
        foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
        organism_id int,
        foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
        name varchar(255),
        uniquename text not null,
        description text,
        type_id int not null,
        foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
        is_obsolete boolean not null default 'false',
        constraint stock_c1 unique (organism_id,uniquename,type_id)
      );
 * @endcode
 *
 * @ingroup tripal_stock_views
 */
function retrieve_stock_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     $database = 'chado';
  }


	// Basic table definition
  $data['stock']['table']['group'] = t('Chado Stock');
  $data['stock']['table']['base'] = array(
    'field' => 'stock_id',
    'title' => t('Chado Stock'),
    'help' => t('Chado Stocks are a record of any material upon which an experiment can be done. For example, a DNA sample, an individual or a population.'),
  );
  if($database){
     $data['stock']['table']['base']['database'] = $database;
  }

	// Define relationships between this table and others
  $data['stock']['table']['join'] = array(
    'organism' => array(
      'left_field' => 'organism_id',
      'field' => 'organism_id',
    ),
  ); 

	// Table Field Definitions----------------------
  // Field: feature_id (primary key)
  $data['stock']['stock_id'] = array(
    'title' => 'Stock ID',
    'help' => 'The primary key of a stock',
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
	if ($database){
  	$data['stock']['stock_nid'] = array(
    	'title' => 'Node ID',
    	'help' => 'This is the node ID of this feature. It can be used as a link to the node.',
    	'field' => array(
       'handler' => 'views_handler_field_computed_stock_nid',
      ),
  	);
  } else {
    // Add relationship between chado_stock and stock
    $data['stock']['stock_nid'] = array(
      'group' => 'Stock',
      'title' => 'Stock Node',
      'help' => 'Links Chado Stock Fields/Data to the Nodes in the current View.',
      'real field' => 'stock_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Stock => Chado'),
        'label' => t('Stock => Chado'),
        'real field' => 'stock_id',
        'base' => 'chado_stock',
        'base field' => 'stock_id'
      ),
    );

  }

  //Field: unique name (text)
  $data['stock']['uniquename'] = array(
    'title' => t('Unique Name'),
    'help' => t('A unique name for this stock.'),
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
    $data['stock']['uniquename']['field']['handler'] = 'views_handler_field_node_optional';
  }
  
  //Field: name (varchar 255)
  $data['stock']['name'] = array(
    'title' => t('Name'),
    'help' => t('The human-readable name of this stock.'),
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
    $data['stock']['name']['field']['handler'] = 'views_handler_field_node_optional';
  }
  
  //Field: description (varchar 255)
  $data['stock']['description'] = array(
    'title' => t('Description'),
    'help' => t('Provides a short description for a given stock.'),
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

  //Field: is_obsolete (boolean t/f)
  $data['stock']['is_obsolete'] = array(
    'title' => t('Is Obsolete?'),
    'help' => t('A yes/no field indicating whether a given stock is obsolete or not.'),
    'field' => array(
      'handler' => 'views_handler_field_chado_tf_boolean',
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

	//Calculated Field: stock properties
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['stock']['properties'] = array(
    'title' => t('Stock Properties'),
    'help' => t('Properties of the current stock.'),
    'field' => array(
      'title' => t('Properties'),
      'help' => t('Display a given type of properties associated with a stock.'),
      'handler' => 'views_handler_field_stockprop_by_type',
    ),
    'filter' => array(
    	'title' => t('Properties'),
    	'help' => t('Filter by a given property type or value.'),
    	'handler' => 'views_handler_filter_stockprop_id',
    ),
    'argument' => array(
    	'title' => t('Properties'),
    	'help' => t('Allow a property to be supplied in the path'),
    	'handler' => 'views_handler_argument_stockprop_id',
    ),
  );

	//Calculated Field: stock properties (ALL)
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['stock']['all_properties'] = array(
  	'title' => t('All Stock Properties'),
  	'help' => t('Properties of the current stock.'),
  	'field' => array(
    	'title' => t('All Properties'),
    	'help' => t('Display all properties associated with a stock.'),
    	'handler' => 'views_handler_field_stockprop_all',
  	),
	);
	
	//Calculated Field: stock relationships
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['stock']['relationships'] = array(
    'title' => t('Stock Relationships'),
    'help' => t('Relationships including the current stock.'),
    'field' => array(
      'title' => t('Relationships'),
      'help' => t('Display a given type of relationships including the current stock.'),
      'handler' => 'views_handler_field_stockrel_by_type',
    ),
    'filter' => array(
    	'handler' => 'views_handler_filter_stock_relationship_id'
    ),
  );

	//Calculated Field: stock relationships (ALL)
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['stock']['all_relationships'] = array(
  	'title' => t('All Stock Relationships'),
  	'help' => t('Relationships including the current stock.'),
  	'field' => array(
    	'title' => t('All Relationships'),
    	'help' => t('Display all relationships including the stock.'),
    	'handler' => 'views_handler_field_stockrel_all',
  	),
	);
	
	//Calculated Field: stock dbxrefs 
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['stock']['dbxref'] = array(
    'title' => t('Stock Database References'),
    'help' => t('Database References associated with the current stock.'),
    'field' => array(
      'title' => t('Database References'),
      'help' => t('Display database references from a given database for the current stock.'),
      'handler' => 'views_handler_field_stock_dbxref_by_type',
    ),
    'filter' => array(
    	'title' => t('Database References'),
    	'help' => t('Filter by a given database reference type and/or value.'),
    	'handler' => 'views_handler_filter_stock_dbxref_id',
    ),
  );

	//Calculated Field: stock dbxrefs (ALL)
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['stock']['all_dbxref'] = array(
  	'title' => t('All Stock Database References'),
  	'help' => t('Database References associated with the current stock.'),
  	'field' => array(
    	'title' => t('All Database References'),
    	'help' => t('Display all database references for the current stock.'),
    	'handler' => 'views_handler_field_stock_dbxref_all',
  	),
	);
  return $data;
}