<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal organism tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * @defgroup tripal_organism_views Organism Views Integration
 * @ingroup views
 */

/**
 *
 * @ingroup tripal_organism_views
 */
function tripal_organism_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'organism_listing';
  $view->description = 'A listing of all organism in chado';
  $view->tag = 'chado default';
  $view->base_table = 'organism';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'common_name' => array(
      'label' => 'Common Name',
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
    'genus' => array(
      'label' => 'Genus',
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
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'relationship' => 'none',
    ),
    'species' => array(
      'label' => 'Species',
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
      'id' => 'species',
      'table' => 'organism',
      'field' => 'species',
      'relationship' => 'none',
    ),
    'abbreviation' => array(
      'label' => 'Abbreviation',
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
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'abbreviation',
      'table' => 'organism',
      'field' => 'abbreviation',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'genus' => array(
      'order' => 'ASC',
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'relationship' => 'none',
    ),
    'species' => array(
      'order' => 'ASC',
      'id' => 'species',
      'table' => 'organism',
      'field' => 'species',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_organism content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Organisms');
  $handler->override_option('empty', 'No organisms matched the supplied criteria.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'common_name' => 'common_name',
      'genus' => 'genus',
      'species' => 'species',
      'abbreviation' => 'abbreviation',
    ),
    'info' => array(
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'genus' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'species' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'abbreviation' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/organisms');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Organisms',
    'description' => 'A biological organism.',
    'weight' => '10',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  // Add code specific to a local chado installation
  // NOTE: Edit $handler above to $default_handler for the default display
  if (tripal_core_chado_schema_exists()) {
    // Add nid field
    $fields = $view->get_items('field', 'default');
    $new_fields = array(
      'nid' => array(
        'label' => 'Nid',
        'alter' => array(
          'alter_text' => 0,
          'text' => '',
          'make_link' => 0,
          'path' => '',
          'absolute' => 0,
          'link_class' => '',
          'alt' => '',
          'rel' => '',
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
        'hide_alter_empty' => 1,
        'link_to_node' => 0,
        'exclude' => 1,
        'id' => 'nid',
        'table' => 'node',
        'field' => 'nid',
        'relationship' => 'none',
      )
    );
    $fields = $new_fields + $fields;
    // Adds organism => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Organism to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_organism',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['common_name']['alter']['link_to_node'] = 1;
    $default_handler->override_option('fields', $fields);
    // Only show records with published nodes
    /**
    $filters = $view->get_items('filter', 'default');
    $filters['status'] = array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    );
    $default_handler->override_option('filters', $filters);
    */
  }
  $views[$view->name] = $view;

  return $views;
}
