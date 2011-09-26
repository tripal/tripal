<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal stock tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * @defgroup tripal_stock_views Stock Views Integration
 * @ingroup views
 * @ingroup tripal_stock
 */

require_once('views/stock.views.inc');
require_once('views/chado_stock.views.inc');
require_once('views/misc_tables.views.inc'); 
/**
 * Implements hook_views_data(): Describe chado/tripal tables & fields to views
 *
 * @return array
 *   A data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_stock_views
 */
function tripal_stock_views_data()  {
  $data = array();

  $data = array_merge($data, retrieve_stock_views_data());
  $data = array_merge($data, retrieve_chado_stock_views_data());
  $data = array_merge($data, retrieve_stock_misc_tables_views_data());
  
  return $data;
}

/**
 * Implements hook_views_handlers()
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return array
 *   An array of handler definitions
 *
 * @ingroup tripal_stock_views
 */
function tripal_stock_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_stock') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_computed_stock_nid' => array(
       'parent' => 'views_handler_field_numeric',
     ),
     'views_handler_field_stockprop_by_type' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
		'views_handler_field_stockprop_all' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
		'views_handler_field_stockrel_by_type' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
		'views_handler_field_stockrel_all' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
		'views_handler_field_stock_dbxref_by_type' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
		'views_handler_field_stock_dbxref_all' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
     'views_handler_filter_stockprop_id' => array(
     	'parent' => 'views_handler_filter',
     ),
     'views_handler_filter_stock_dbxref_id' => array(
     	'parent' => 'views_handler_filter',
     ),
     'views_handler_filter_stock_relationship_id' => array(
     	'parent' => 'views_handler_filter',
     ),
     'views_handler_argument_stockprop_id' => array(
     	'parent' => 'views_handler_argument_string',
     ),
   ),
 );
}

/**
 * Implementation of hook_views_data_alter().
 */
function tripal_stock_views_data_alter(&$data) {

  if( !(is_array($db_url) and array_key_exists('chado',$db_url)) ){

    // Add featuer relationship to node
    $data['node']['stock_chado_nid'] = array(
      'group' => 'Stock',
      'title' => 'Stock Node',
      'help' => 'Links Chado Stock Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_stock',
        'base field' => 'nid'
      ),
    );
  }
  
}

/**
 * Implements hook_views_pre_render
 *
 * Purpose: Intercepts the view after the query has been executed
 *   All the results are stored in $view->result
 *   Looking up the NID here ensures the query is only executed once
 *   for all stocks in the table.
 *
 * @todo add if !<chado/drupal same db> around NID portion
 *
 * @ingroup tripal_stock_views
 */
function tripal_stock_views_pre_render	(&$view) {
	if (preg_match('/stock/', $view->base_table)) {
		
		//-----Node IDs---------------------------------------------
		// @see file: tripal_core.views.inc function: tripal_core_add_node_ids_to_view (&$view);
		
		// retrieve the stock_id for each record in the views current page
		$stock_ids = array();
		foreach ($view->result as $row_num => $row) {
			$stock_ids[$row_num] = $row->stock_id;
		}
		
		if (sizeof($stock_ids)) {
		
			//-----Properties------------------------------------------
			$field_names = array_keys($view->field);
			//if any property fields are in the current view
			$property_field_names = preg_grep('/properties/',$field_names);
			if (!empty($property_field_names)) {
				$sql = "SELECT stockprop.*, cvterm.name as type_name FROM stockprop "
					."INNER JOIN cvterm cvterm ON stockprop.type_id=cvterm.cvterm_id "
					."WHERE stockprop.stock_id IN (".implode(',',$stock_ids).")";
				$previous_db = tripal_db_set_active('chado');
				$resource = db_query($sql);
			 tripal_db_set_active($previous_db);
		
				$view->result[$key]->properties = array();
				while ($r = db_fetch_object($resource)) {
					$key = array_search($r->stock_id, $stock_ids);
					$view->result[$key]->properties[] = $r;
				}
			}
		
			//-----Relationships----------------------------------------
			//if any relationship fields are in the current view
			$relationship_field_names = preg_grep('/relationships/', $field_names);
			if (!empty($relationship_field_names)) {
				$sql = "SELECT stock_relationship.*, cvterm.name as type_name, "
					."subject_stock.name as subject_name, object_stock.name as object_name "
					."FROM stock_relationship "
					."LEFT JOIN stock subject_stock ON stock_relationship.subject_id=subject_stock.stock_id "
					."LEFT JOIN stock object_stock ON stock_relationship.object_id=object_stock.stock_id "
					."LEFT JOIN cvterm cvterm ON stock_relationship.type_id = cvterm.cvterm_id "
					."WHERE stock_relationship.subject_id IN (".implode(',',$stock_ids).") "
					."OR stock_relationship.object_id IN (".implode(',',$stock_ids).") ";
				$previous_db = tripal_db_set_active('chado');
				$resource = db_query($sql);
			 tripal_db_set_active($previous_db);
			
				while ($r = db_fetch_object($resource)) {
					if (in_array($r->subject_id, $stock_ids)) {
						$key = array_search($r->subject_id, $stock_ids);
						$r->stock_id = $r->subject_id;
						$view->result[$key]->relationships[] = clone $r;
					}
					if (in_array($r->object_id, $stock_ids)) {
						$key = array_search($r->object_id, $stock_ids);
						$r->stock_id = $r->object_id;
						$view->result[$key]->relationships[] = clone $r;
					}
				}
			}
		
			//-----DB References--------------------------------------------
			//if any dbxref fields are in the current view
			$dbxref_field_names = preg_grep('/dbxref/',$field_names);
			if (!empty($dbxref_field_names)) {
				$sql = "SELECT stock_dbxref.*, dbxref.db_id, db.name as db_name, db.urlprefix, "
					."dbxref.accession, dbxref.version, dbxref.description "
					."FROM stock_dbxref "
					."LEFT JOIN dbxref dbxref ON stock_dbxref.dbxref_id=dbxref.dbxref_id "
					."LEFT JOIN db db ON dbxref.db_id=db.db_id "
					."WHERE stock_dbxref.stock_id IN (".implode(',',$stock_ids).")";
				$previous_db = tripal_db_set_active('chado');
				$resource = db_query($sql);
			 tripal_db_set_active($previous_db);
		
				$view->result[$key]->dbxref = array();
				while ($r = db_fetch_object($resource)) {
					$key = array_search($r->stock_id, $stock_ids);
					$view->result[$key]->dbxref[] = $r;
				}
			}
			
		} //if there are stocks
	} //if we're dealing with a stock view
}

/**
 *
 * @ingroup tripal_stock_views
 */
function tripal_stock_views_default_views() {
  $views = array();
  
  $view = new view;
  $view->name = 'all_stocks';
  $view->description = 'This view lists all stocks by default. There are exposed filters available but no arguments are used.';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'stock';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'uniquename' => array(
      'label' => 'Unique Name',
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
      'id' => 'uniquename',
      'table' => 'stock',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
    'name_2' => array(
      'label' => 'Name',
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
      'id' => 'name_2',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name' => array(
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
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'common_name' => array(
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
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'all_dbxref' => array(
      'label' => 'All Database References',
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
      'type' => 'ul',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_dbxref',
      'table' => 'stock',
      'field' => 'all_dbxref',
      'relationship' => 'none',
    ),
    'all_properties' => array(
      'label' => 'All Properties',
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
      'type' => 'ul',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_properties',
      'table' => 'stock',
      'field' => 'all_properties',
      'relationship' => 'none',
    ),
    'all_relationships' => array(
      'label' => 'All Relationships',
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
      'type' => 'ul',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_relationships',
      'table' => 'stock',
      'field' => 'all_relationships',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'common_name' => array(
      'order' => 'ASC',
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'uniquename' => array(
      'order' => 'ASC',
      'id' => 'uniquename',
      'table' => 'stock',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'common_name' => array(
      'operator' => '=',
      'value' => 'All',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'common_name_op',
        'identifier' => 'organism_common_name',
        'label' => 'Organism',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'name' => array(
      'operator' => '=',
      'value' => 'All',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'name',
        'label' => 'Chado CV Terms: Name',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_stock content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Stocks');
  $handler->override_option('empty', 'There are no stocks that match this criteria. If you think there should be, ensure that all stocks in chado are sync\'d with your website.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 25);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'uniquename' => 'uniquename',
      'name_2' => 'name_2',
      'name' => 'name',
      'common_name' => 'common_name',
      'all_dbxref' => 'all_dbxref',
      'all_properties' => 'all_properties',
      'all_relationships' => 'all_relationships',
    ),
    'info' => array(
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name_2' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'all_dbxref' => array(
        'separator' => '',
      ),
      'all_properties' => array(
        'separator' => '',
      ),
      'all_relationships' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'stocks');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Stocks',
    'description' => 'A full listing of all chado stocks',
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
