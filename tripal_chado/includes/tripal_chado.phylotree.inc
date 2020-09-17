<?php
/**
 * Prepares a phylogenetic tree for viewing.
 *
 * @param $phylotree
 */
function tripal_phylogeny_prepare_tree_viewer($phylotree)
{

  // If the phylotree is not provided then just return;
  if (!$phylotree) {
    tripal_report_error('tripal_phylotree', TRIPAL_ERROR, 'tripal_phylogeny_prepare_tree_viewer: must provide a $phylotree argument.');
  }

  // Don't prepare for viewing more than once.
  if (property_exists($phylotree, 'prepared_to_view') and
    $phylotree->prepared_to_view == TRUE) {
    return;
  }

  $module_path = drupal_get_path('module', 'tripal_chado');

  drupal_add_js('https://d3js.org/d3.v3.min.js', 'external');

  drupal_add_js("$module_path/theme/js/d3.phylogram.js");
  drupal_add_js("$module_path/theme/js/tripal_phylogeny.js");
  drupal_add_css("$module_path/theme/css/tripal_phylogeny.css");

  drupal_add_library('system', 'ui.dialog');

  // Don't show tick marks for the taxonomy tree.
  $skip_ticks = 0;
  if (!is_null($phylotree->type_id) and ($phylotree->type_id->name == 'taxonomy' or $phylotree->type_id->name == 'Species tree')) {
    $skip_ticks = 1;
  }

  // Get the node colors as set by the administrator.
  $colors = [];
  $color_defaults = variable_get("tripal_phylogeny_org_colors", [
    '1' => [
      'organism' => '',
      'color' => '',
    ],
  ]);
  foreach ($color_defaults as $i => $details) {
    if ($details['organism']) {
      // Strip the [id:xxx] from the name
      $organism_id = preg_replace('/^.+\[id: (\d+)\].*$/', '\1', $details['organism']);
      $colors[$organism_id] = $details['color'];
    }
  }

  drupal_add_js([
    'tripal_chado' => [
      'phylotree_url' => url('phylotree/' . $phylotree->phylotree_id),
      'phylotree_theme_url' => url($module_path . '/theme'),
      'tree_options' => [
        'phylogram_width' => variable_get('tripal_phylogeny_default_phylogram_width', 350),
        'root_node_size' => variable_get('tripal_phylogeny_default_root_node_size', 3),
        'interior_node_size' => variable_get('tripal_phylogeny_default_interior_node_size', 1),
        'leaf_node_size' => variable_get('tripal_phylogeny_default_leaf_node_size', 6),
        'skipTicks' => $skip_ticks,
        'phylogram_scale' => variable_get('tripal_phylogeny_default_phylogram_scale', 1),
      ],
      'org_colors' => $colors,
    ],
  ], 'setting');

  if (!property_exists($phylotree, 'has_nodes')) {
    // If the nodes haven't loaded then set a value so the template can
    // choose not to show the phylogram.
    $values = ['phylotree_id' => $phylotree->phylotree_id];
    $options = ['limit' => 1, 'offset' => 0, 'has_record' => 1];
    $phylotree->has_nodes = chado_select_record('phylonode', ['phylonode_id'], $values, $options);
  }
  if (!property_exists($phylotree, 'has_features')) {
    // If the nodes haven't loaded then set a value so the template can
    // choose not to show the circular dendrogram. The chado_select_record()
    // API call can't do this query so we have to do it manually.
    $sql = "
      SELECT count(*) as num_features
      FROM {phylonode}
      WHERE NOT feature_id IS NULL and phylotree_id = :phylotree_id
      LIMIT 1 OFFSET 0
    ";
    $phylotree->has_features = chado_query($sql, [':phylotree_id' => $phylotree->phylotree_id])->fetchField();
  }

  $phylotree->prepared_to_view = TRUE;
}

/**
 * Get json representation of a phylotree id.
 *
 * This function is meant to be called via AJAX.
 *
 * @param int $phylotree_id
 *   the ID of the phylotree node.
 *
 * @return string json
 *
 * @ingroup tripal_phylogeny
 */
function tripal_phylogeny_ajax_get_tree_json($phylotree_id)
{

  $phylotree = chado_generate_var('phylotree', ['phylotree_id' => $phylotree_id]);

  // For backwards compatibility with Tripal v2 and the legacy modules of
  // Tripal v3 we have two different SQL statements.
  if (module_exists('tripal_phylogeny')) {
    // This SQL gets all of the phylonodes for a given tree as well as the
    // features and organisms with which it is associated.  Each phylonode
    // can be associated with an organism in one of two ways: 1) via a
    // feature linked by the phylonode.feature_id field or 2) via a
    // a record in the phylonde_organism table.  Therefore both types of
    // organism records are returned in the query below, but those
    // retrieved via a FK link on features are prefixed with 'fo_'.
    $sql = "
      SELECT
        n.phylonode_id, n.parent_phylonode_id, n.label AS name, n.distance AS length,
        f.feature_id, f.name AS feature_name,
        cvt.name AS cvterm_name,
        o.organism_id, o.common_name, o.abbreviation, o.genus, o.species,
        fo.organism_id AS fo_organism_id, fo.common_name AS fo_common_name,
        fo.abbreviation AS fo_abbreviation, fo.genus as fo_genus, fo.species AS fo_species,
        cf.nid AS feature_nid,
        fco.nid AS fo_organism_nid,
        co.nid AS organism_nid
      FROM {phylonode} n
        LEFT OUTER JOIN {cvterm} cvt              ON n.type_id = cvt.cvterm_id
        LEFT OUTER JOIN {feature} f               ON n.feature_id = f.feature_id
        LEFT OUTER JOIN [chado_feature] cf        ON cf.feature_id = f.feature_id
        LEFT OUTER JOIN {organism} fo             ON f.organism_id = fo.organism_id
        LEFT OUTER JOIN [chado_organism] fco      ON fco.organism_id = fo.organism_id
        LEFT OUTER JOIN {phylonode_organism} po   ON po.phylonode_id = n.phylonode_id
        LEFT OUTER JOIN {organism} o              ON PO.organism_id = o.organism_id
        LEFT OUTER JOIN [chado_organism] co       ON co.organism_id = o.organism_id
      WHERE n.phylotree_id = :phylotree_id
    ";
  } else {
    $sql = "
      SELECT
        n.phylonode_id, n.parent_phylonode_id, n.label AS name, n.distance AS length,
        f.feature_id, f.name AS feature_name,
        cvt.name AS cvterm_name,
        o.organism_id, o.common_name, o.abbreviation, o.genus, o.species,
        fo.organism_id AS fo_organism_id, fo.common_name AS fo_common_name,
        fo.abbreviation AS fo_abbreviation, fo.genus as fo_genus, fo.species AS fo_species
      FROM {phylonode} n
        LEFT OUTER JOIN {cvterm} cvt              ON n.type_id = cvt.cvterm_id
        LEFT OUTER JOIN {feature} f               ON n.feature_id = f.feature_id
        LEFT OUTER JOIN {organism} fo             ON f.organism_id = fo.organism_id
        LEFT OUTER JOIN {phylonode_organism} po   ON po.phylonode_id = n.phylonode_id
        LEFT OUTER JOIN {organism} o              ON PO.organism_id = o.organism_id
      WHERE n.phylotree_id = :phylotree_id
    ";
  }
  $args = [':phylotree_id' => $phylotree_id];
  $results = chado_query($sql, $args);

  // Fetch all the phylonodes into an assoc array indexed by phylonode_id.
  // Convert from resultset record to array, fixing datatypes. chado_query
  // returns numeric as string and fun stuff like that.
  $phylonodes = [];
  $root_phylonode_ref = NULL;

  if ($results) {
    while ($r = $results->fetchObject()) {
      $phylonode_id = (int)$r->phylonode_id;

      // expect all nodes to have these properties
      $node = [
        'phylonode_id' => $phylonode_id,
        'parent_phylonode_id' => (int)$r->parent_phylonode_id,
        'length' => (double)$r->length,
        'cvterm_name' => $r->cvterm_name,
      ];

      // If the nodes are taxonomic then set an equal distance
      if ($phylotree->type_id->name == 'taxonomy' or $phylotree->type_id->name == 'Speces tree') {
        $node['length'] = 0.001;
      }

      // Other props may exist only for leaf nodes
      if ($r->name) {
        $node['name'] = $r->name;
      }
      // If this node is associated with a feature then add in the details
      if ($r->feature_id) {
        $node['feature_id'] = (int)$r->feature_id;
        $node['feature_name'] = $r->feature_name;
        if (module_exists('tripal_phylogeny')) {
          $node['feature_nid'] = (int)$r->feature_nid;
        } else {
          $entity_id = chado_get_record_entity_by_table('feature', $r->feature_id);
          $node['feature_eid'] = (int)$entity_id;
        }
      }
      // Add in the organism fields when they are available via the
      // phylonode_organism table.
      if ($r->organism_id) {
        $node['organism_id'] = (int)$r->organism_id;
        $node['common_name'] = $r->common_name;
        $node['abbreviation'] = $r->abbreviation;
        $node['genus'] = $r->genus;
        $node['species'] = $r->species;
        if (module_exists('tripal_phylogeny')) {
          $node['organism_nid'] = (int)$r->organism_nid;
        } else {
          $entity_id = chado_get_record_entity_by_table('organism', $r->organism_id);
          $node['organism_eid'] = (int)$entity_id;
        }
        // If the node does not have a name but is linked to an organism
        // then set the name to be that of the genus and species.
        if (!$r->name) {
          $node['name'] = $r->genus . ' ' . $r->species;
        }
      }
      // Add in the organism fields when they are available via the
      // the phylonode.feature_id FK relationship.
      if ($r->fo_organism_id) {
        $node['fo_organism_id'] = (int)$r->fo_organism_id;
        $node['fo_common_name'] = $r->fo_common_name;
        $node['fo_abbreviation'] = $r->fo_abbreviation;
        $node['fo_genus'] = $r->fo_genus;
        $node['fo_species'] = $r->fo_species;
        if (module_exists('tripal_phylogeny')) {
          $node['fo_organism_nid'] = (int)$r->fo_organism_nid;
        } else {
          $entity_id = chado_get_record_entity_by_table('organism', $r->fo_organism_id);
          $node['fo_organism_eid'] = (int)$entity_id;
        }
      }

      // Add this node to the list, organized by ID.
      $phylonodes[$phylonode_id] = $node;
    }

    // Populate the children[] arrays for each node.
    foreach ($phylonodes as $key => &$node) {
      if ($node['parent_phylonode_id'] !== 0) {
        $parent_ref = &$phylonodes[$node['parent_phylonode_id']];
        // Append node reference to children.
        $parent_ref['children'][] = &$node;
      } else {
        $root_phylonode_ref = &$node;
      }
    }
  }

  // dump datastructure as json to browser. drupal sets the mime-type correctly.
  drupal_json_output($root_phylonode_ref);
}


/**
 * @file
 * This file contains the functions used for administration of the module
 *
 */

function tripal_phylogeny_admin_phylotrees_listing()
{
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Storage', 'admin/tripal/storage');
  $breadcrumb[] = l('Chado', 'admin/tripal/storage/chado');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_phylogeny_admin_phylotree', 'default');
  if (isset($view)) {
    $output .= $view;
  } else {
    $output .= '<p>The Phylotree module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Phylotree View', 'admin/tripal/extension/tripal_phylogeny/views/phylotree/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}


/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_phylogeny_default_plots_form($form, &$form_state)
{
  $form = [];

  $form['plot_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Plot Settings'),
    '#description' => t('You can customize settings for each plot'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  ];

  $form['plot_settings']['phylogram_width'] = [
    '#type' => 'textfield',
    '#title' => 'Tree Width',
    '#description' => 'Please specify the width in pixels for the phylogram',
    '#default_value' => variable_get('tripal_phylogeny_default_phylogram_width', 350),
    '#element_validate' => [
      'element_validate_integer_positive',
    ],
    '#size' => 5,
  ];

  $form['plot_settings']['phylogram_scale'] = [
    '#type' => 'select',
    '#title' => t('Phylogram Scale'),
    '#description' => 'Please specify the scale to use.',
    '#default_value' => variable_get('tripal_phylogeny_default_phylogram_scale', 1),
    '#options' => array(
      1 => t('Linear'),
      2 => t('Logarithmic'),
    ),
    '#size' => 2,
  ];


  $form['node_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Node Settings'),
    '#description' => t('You can customize settings for the nodes on the trees.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  ];
  $form['node_settings']['root_node_size'] = [
    '#type' => 'textfield',
    '#title' => 'Root Node Size',
    '#description' => 'Please specify a size for the root node size. If set to zero, the node will not appear.',
    '#default_value' => variable_get('tripal_phylogeny_default_root_node_size', 3),
    '#element_validate' => [
      'element_validate_integer',
    ],
    '#size' => 3,
  ];
  $form['node_settings']['interior_node_size'] = [
    '#type' => 'textfield',
    '#title' => 'Interor Node Size',
    '#description' => 'Please specify a size for the interior node size. If set to zero, the node will not appear.',
    '#default_value' => variable_get('tripal_phylogeny_default_interior_node_size', 0),
    '#element_validate' => [
      'element_validate_integer',
    ],
    '#size' => 3,
  ];
  $form['node_settings']['leaf_node_size'] = [
    '#type' => 'textfield',
    '#title' => 'Leaf Node Size',
    '#description' => 'Please specify a size for the leaf node size. If set to zero, the node will not appear.',
    '#default_value' => variable_get('tripal_phylogeny_default_leaf_node_size', 6),
    '#element_validate' => [
      'element_validate_integer',
    ],
    '#size' => 3,
  ];

  // Get the number of organism colors that already exist. If the site admin
  // has set colors then those settings will be in a Drupal variable which we
  // will retrieve.  Otherwise the num_orgs defaults to 1 and a single
  // set of fields is provided.
  $num_orgs = variable_get("tripal_phylogeny_num_orgs", 1);
  if (array_key_exists('values', $form_state) and array_key_exists('num_orgs', $form_state['values'])) {
    $num_orgs = $form_state['values']['num_orgs'];
  }
  // The default values for each organism color are provided in a d
  // Drupal variable that gets set when the form is set.
  $color_defaults = variable_get("tripal_phylogeny_org_colors", [
    '1' => [
      'organism' => '',
      'color' => '',
    ],
  ]);

  $form['node_settings']['desc'] = [
    '#type' => 'item',
    '#title' => t('Node Colors by Organism'),
    '#markup' => t('If the trees are associated with features (e.g. proteins)
      then the nodes can be color-coded by their organism.  This helps the user
      visualize which nodes belong to each organism.  Please enter the
      name of the organism and it\'s corresponding color in HEX code (e.g. #FF0000 == red).
      Organisms that are not given a color will be gray.'),
  ];
  $form['node_settings']['org_table']['num_orgs'] = [
    '#type' => 'value',
    '#value' => $num_orgs,
  ];

  // Iterate through the number of organism colors and add a field for each one.
  for ($i = 0; $i < $num_orgs; $i++) {
    $form['node_settings']['org_table']['organism_' . $i] = [
      '#type' => 'textfield',
      '#default_value' => array_key_exists($i, $color_defaults) ? $color_defaults[$i]['organism'] : '',
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/organism",
      '#description' => t('Please enter the name of the organism.'),
      '#size' => 30,
    ];
    $form['node_settings']['org_table']['color_' . $i] = [
      '#type' => 'textfield',
      '#description' => t('Please provide a color in Hex format (e.g. #FF0000).'),
      '#default_value' => array_key_exists($i, $color_defaults) ? $color_defaults[$i]['color'] : '',
      '#suffix' => "<div id=\"color-box-$i\" style=\"width: 30px;\"></div>",
      '#size' => 10,
    ];
  }
  $form['node_settings']['org_table']['add'] = [
    '#type' => 'submit',
    '#name' => 'add',
    '#value' => 'Add',
    '#ajax' => [
      'callback' => "tripal_phylogeny_default_plots_form_ajax_callback",
      'wrapper' => 'tripal_phylogeny_default_plots_form',
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];
  $form['node_settings']['org_table']['remove'] = [
    '#type' => 'submit',
    '#name' => 'remove',
    '#value' => 'Remove',
    '#ajax' => [
      'callback' => "tripal_phylogeny_default_plots_form_ajax_callback",
      'wrapper' => 'tripal_phylogeny_default_plots_form',
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];
  $form['node_settings']['org_table']['#theme'] = 'tripal_phylogeny_admin_org_color_tables';
  $form['node_settings']['org_table']['#prefix'] = '<div id="tripal_phylogeny_default_plots_form">';
  $form['node_settings']['org_table']['#suffix'] = '</div>';

  $form['submit'] = [
    '#type' => 'submit',
    '#name' => 'submit',
    '#value' => 'Save Configuration',
  ];

  $form['#submit'][] = 'tripal_phylogeny_default_plots_form_submit';

  return $form;
}

/**
 * Validate the phylotree settings forms
 *
 * @ingroup tripal_phylogeny
 */
function tripal_phylogeny_default_plots_form_validate($form, &$form_state)
{

}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_phylogeny_default_plots_form_submit($form, &$form_state)
{
  // Rebuild this form after submission so that any changes are reflected in
  // the flat tables.
  $form_state['rebuild'] = TRUE;

  if ($form_state['clicked_button']['#name'] == 'submit') {
    variable_set('tripal_phylogeny_default_phylogram_width', $form_state['values']['phylogram_width']);

    variable_set('tripal_phylogeny_default_root_node_size', $form_state['values']['root_node_size']);
    variable_set('tripal_phylogeny_default_interior_node_size', $form_state['values']['interior_node_size']);
    variable_set('tripal_phylogeny_default_leaf_node_size', $form_state['values']['leaf_node_size']);
    variable_set('tripal_phylogeny_default_phylogram_scale', $form_state['values']['phylogram_scale']);

    $num_orgs = $form_state['values']['num_orgs'];
    variable_set("tripal_phylogeny_num_orgs", $num_orgs);
    $colors = [];
    for ($i = 0; $i < $num_orgs; $i++) {
      $colors[$i] = [
        'organism' => $form_state['values']['organism_' . $i],
        'color' => $form_state['values']['color_' . $i],
      ];
    }
    variable_set("tripal_phylogeny_org_colors", $colors);
  }
  if ($form_state['clicked_button']['#name'] == 'add') {
    $form_state['values']['num_orgs']++;
  }
  if ($form_state['clicked_button']['#name'] == 'remove') {
    $form_state['values']['num_orgs']--;
  }
}

/**
 *
 * @param unknown $variables
 */
function theme_tripal_phylogeny_admin_org_color_tables($variables)
{
  $fields = $variables['element'];
  $num_orgs = $fields['num_orgs']['#value'];
  $headers = ['Organism', 'Color', ''];
  $rows = [];
  for ($i = 0; $i < $num_orgs; $i++) {
    $add_button = ($i == $num_orgs - 1) ? drupal_render($fields['add']) : '';
    $del_button = ($i == $num_orgs - 1 and $i != 0) ? drupal_render($fields['remove']) : '';
    $rows[] = [
      drupal_render($fields['organism_' . $i]),
      drupal_render($fields['color_' . $i]),
      $add_button . $del_button,
    ];
  }
  $table_vars = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'colgroups' => [],
    'empty' => '',
  ];
  $form['orgs']['num_orgs'] = $fields['num_orgs'];
  return theme('table', $table_vars);
}


/**
 * Ajax callback function for the gensas_job_view_panel_form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_phylogeny_default_plots_form_ajax_callback($form, $form_state)
{

  return $form['node_settings']['org_table'];
}
