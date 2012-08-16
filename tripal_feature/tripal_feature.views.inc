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
 * @defgroup tripal_feature_views Feature Views Integration
 * @ingroup views
 * @ingroup tripal_feature
 */

/**
 * Implements hook_views_data()
 *
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_feature_views
 */
function tripal_feature_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    $tables = array(
      'feature',
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 9)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE);

        // Add in node relationships if chado is in the same db as drupal
        if (tripal_core_chado_schema_exists()) {
          $integrations = tripal_views_add_node_relationship_to_chado_table_integration($table_integration_array);
          foreach ($integrations as $integration) {
            tripal_views_integration_add_entry($integration);
          }
        }
        else {
          tripal_views_integration_add_entry($table_integration_array);
        }

      }
    }

    $tables = array(
      'feature_cvterm',
      'feature_cvterm_dbxref',
      'feature_cvterm_pub',
      'feature_cvtermprop',
      'feature_dbxref',
      'feature_pub',
      'feature_pubprop',
      'feature_relationship',
      'feature_relationship_pub',
      'feature_relationshipprop',
      'feature_relationshipprop_pub',
      'feature_synonym',
      'featureloc',
      'featureloc_pub',
      'featureprop',
      'featureprop_pub',
      'featuremap',
      'featuremap_pub',
      'featurepos',
      'featurerange'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 9)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
  }

  return $data;
}

/**
 * Implements hook_views_handlers()
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 *
 * @ingroup tripal_feature_views
 */
function tripal_feature_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_feature') . '/views_handlers',
    ),
    'handlers' => array(
    'views_handler_field_residues' => array(
      'parent' => 'views_handler_field',
    ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function tripal_feature_views_data_alter(&$data) {

  if ( !(is_array($db_url) and array_key_exists('chado', $db_url)) ) {

    // Add featuer relationship to node
    $data['node']['feature_chado_nid'] = array(
      'group' => 'Feature',
      'title' => 'Feature Node',
      'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_feature',
        'base field' => 'nid'
      ),
    );
  }

}

/**
 *
 * @ingroup tripal_feature_views
 */
function tripal_feature_views_default_views() {
  $views = array();

  // Main default view
  $view = new view;
  $view->name = 'feature_listing';
  $view->description = 'A listing of chado sequence features.';
  $view->tag = 'chado default';
  $view->base_table = 'feature';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'features_all', 'default');
  $handler->override_option('fields', array(
    'uniquename' => array(
      'label' => 'Unique Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => 'node/[nid]',
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
      'id' => 'uniquename',
      'table' => 'feature',
      'field' => 'uniquename',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'name' => array(
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
      'id' => 'name',
      'table' => 'feature',
      'field' => 'name',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'name_1' => array(
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
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
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
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'seqlen' => array(
      'label' => 'Sequence Length',
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
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'seqlen',
      'table' => 'feature',
      'field' => 'seqlen',
      'relationship' => 'none',
    ),
    'is_obsolete' => array(
      'label' => 'Is Obsolete',
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
      'type' => 'separator',
      'not' => 0,
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'is_obsolete',
      'table' => 'feature',
      'field' => 'is_obsolete',
      'relationship' => 'none',
    ),
    'timeaccessioned' => array(
      'label' => 'Accessioned On',
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
      'date_format' => 'large',
      'custom_date_format' => '',
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'timeaccessioned',
      'table' => 'feature',
      'field' => 'timeaccessioned',
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
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'order' => 'ASC',
      'id' => 'name_1',
      'table' => 'feature',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'common_name' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'common_name_op',
        'identifier' => 'organism',
        'label' => 'Organism Common Name',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'override' => array(
        'button' => 'Override',
      ),
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'type_id' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Type',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'feature',
      'field' => 'type_id',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'name_1' => array(
      'operator' => 'allwords',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_1_op',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'feature',
      'field' => 'name',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'search_results' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'search_results',
      'table' => 'views',
      'field' => 'search_results',
      'relationship' => 'none',
      'apply_button' => 'Show',
      'no_results_text' => 'Click "Show" to see a list of all features matching the entered criteria. If you leave a any of the criteria blank then the features will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all features will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_feature content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Sequence Features');
  $handler->override_option('header', 'Click "Show" to see a list of all features matching the entered criteria. If you leave a any of the criteria blank then the features will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all features will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'There are currently no features sync\'d with drupal.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'accession' => 'accession',
      'accession_link' => 'accession_link',
      'name' => 'name',
      'uniquename' => 'uniquename',
      'name_1' => 'name_1',
      'common_name' => 'common_name',
      'seqlen' => 'seqlen',
      'is_obsolete' => 'is_obsolete',
      'is_analysis' => 'is_analysis',
      'nid' => 'nid',
    ),
    'info' => array(
      'accession' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'accession_link' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name_1' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'seqlen' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'is_obsolete' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'is_analysis' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'nid' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/features');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Features',
    'description' => 'A feature is a biological sequence or a section of a biological sequence, or a collection of such sections. Examples include genes, exons, transcripts, regulatory regions, polypeptides, protein domains, chromosome sequences, sequence variations, cross-genome match regions such as hits and HSPs and so on.',
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
    // Adds feature => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Feature to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_feature',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['name']['alter']['make_link'] = 1;
    $fields['name']['alter']['path'] = 'node/[nid]';
    $default_handler->override_option('fields', $fields);
    // Only show records with published nodes
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
  }
  $views[$view->name] = $view;

  return $views;
}
