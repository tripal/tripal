<?php
/**
 * @file
 * This file contains the functions used for administration of the module
 *
 */

function tripal_phylogeny_admin_phylotrees_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Phylotrees', 'admin/tripal/extension/tripal_phylogeny');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_phylogeny_admin_phylotree','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Phylotree module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Phylotree View', 'admin/tripal/extension/tripal_phylogeny/views/phylotree/enable').'</li>';
    $output .= '</ul>';
  }
  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_phylogeny
 */
function tripal_phylogeny_admin() {
  $form = array();


  // PHYLOTREE NODE TITLES
  // If your module is using the Chado Node: Title & Path API to allow
  // custom titles for your node type then you need to add the
  // configuration form for this functionality.
  $details = array(
    'module' => 'tripal_phylogeny',
    'content_type' => 'chado_phylotree',
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => array(
      '[phylotree.name]' => 'Tree Name Only',
      // there should always be one options matching the unique constraint.
      '[phylotree.phylotree_id]' => 'The Chado ID for Phylotrees'
    ),
    // the token indicating the unique constraint in the options array
    'unique_option' => '[phylotree.phylotree_id]'
  );

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);


  // PHYLOTREE NODE URL
  // Using the Chado Node: Title & Path API
  $details = array(
    'module' => 'tripal_phylogeny',
    'content_type' => 'chado_phylotree',
    // An array of options to use under "Page URL"
    // the key should be the token and the value should be the human-readable option
    'options' => array(
      '/tree/[phylotree.name]' => 'Tree Name Only',
      // there should always be one options matching the unique constraint.
      '/tree/[phylotree.phylotree_id]' => 'The Chado ID for Phylotrees',
    )
  );
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);

}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_phylogeny_default_plots_form($form, &$form_state) {
  $form = array();

  $form['plot_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Plot Settings'),
    '#description' => t('You can customize settings for each plot'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );

  $form['plot_settings']['phylogram_width'] = array(
    '#type' => 'textfield',
    '#title' => 'Tree Width',
    '#description' => 'Please specify the width in pixels for the phylogram',
    '#default_value' => variable_get('tripal_phylogeny_default_phylogram_width', 350),
    '#element_validate' => array(
      'element_validate_integer_positive'
    ),
    '#size' => 5,
  );

  $form['node_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node Settings'),
    '#description' => t('You can customize settings for the nodes on the trees.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['node_settings']['root_node_size'] = array(
    '#type' => 'textfield',
    '#title' => 'Root Node Size',
    '#description' => 'Please specify a size for the root node size. If set to zero, the node will not appear.',
    '#default_value' => variable_get('tripal_phylogeny_default_root_node_size', 3),
    '#element_validate' => array(
      'element_validate_integer'
    ),
    '#size' => 3,
  );
  $form['node_settings']['interior_node_size'] = array(
    '#type' => 'textfield',
    '#title' => 'Interor Node Size',
    '#description' => 'Please specify a size for the interior node size. If set to zero, the node will not appear.',
    '#default_value' => variable_get('tripal_phylogeny_default_interior_node_size', 0),
    '#element_validate' => array(
      'element_validate_integer'
    ),
    '#size' => 3,
  );
  $form['node_settings']['leaf_node_size'] = array(
    '#type' => 'textfield',
    '#title' => 'Leaf Node Size',
    '#description' => 'Please specify a size for the leaf node size. If set to zero, the node will not appear.',
    '#default_value' => variable_get('tripal_phylogeny_default_leaf_node_size', 6),
    '#element_validate' => array(
      'element_validate_integer'
    ),
    '#size' => 3,
  );

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
  $color_defaults = variable_get("tripal_phylogeny_org_colors", array('1' => array('organism' => '', 'color' => '')));

  $form['node_settings']['desc'] = array(
    '#type' => 'item',
    '#title' => t('Node Colors by Organism'),
    '#markup' => t('If the trees are associated with features (e.g. proteins)
      then the nodes can be color-coded by their organism.  This helps the user
      visualize which nodes belong to each organism.  Please enter the
      name of the organism and it\'s corresponding color in HEX code (e.g. #FF0000 == red).
      Organisms that are not given a color will be gray.'),
  );
  $form['node_settings']['org_table']['num_orgs'] = array(
    '#type' => 'value',
    '#value' => $num_orgs,
  );

  // Iterate through the number of organism colors and add a field for each one.
  for ($i = 0; $i < $num_orgs; $i++) {
    $form['node_settings']['org_table']['organism_' . $i] = array(
      '#type' => 'textfield',
      '#default_value' => array_key_exists($i, $color_defaults) ? $color_defaults[$i]['organism'] : '',
      '#autocomplete_path' => "admin/tripal/legacy/tripal_organism/organism/auto_name",
      '#description' => t('Please enter the name of the organism.'),
      '#size' => 30,
    );
    $form['node_settings']['org_table']['color_' . $i] = array(
      '#type' => 'textfield',
      '#description' => t('Please provide a color in Hex format (e.g. #FF0000).'),
      '#default_value' => array_key_exists($i, $color_defaults) ? $color_defaults[$i]['color'] : '',
      '#suffix' => "<div id=\"color-box-$i\" style=\"width: 30px;\"></div>",
      '#size' => 10,
    );
  }
  $form['node_settings']['org_table']['add'] = array(
    '#type' => 'submit',
    '#name' => 'add',
    '#value' => 'Add',
    '#ajax' => array(
      'callback' => "tripal_phylogeny_default_plots_form_ajax_callback",
      'wrapper'  => 'tripal_phylogeny_default_plots_form',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
  );
  $form['node_settings']['org_table']['remove'] = array(
    '#type' => 'submit',
    '#name' => 'remove',
    '#value' => 'Remove',
    '#ajax' => array(
      'callback' => "tripal_phylogeny_default_plots_form_ajax_callback",
      'wrapper'  => 'tripal_phylogeny_default_plots_form',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
  );
  $form['node_settings']['org_table']['#theme'] = 'tripal_phylogeny_admin_org_color_tables';
  $form['node_settings']['org_table']['#prefix'] = '<div id="tripal_phylogeny_default_plots_form">';
  $form['node_settings']['org_table']['#suffix'] = '</div>';

  $form['submit'] = array(
    '#type' => 'submit',
    '#name' => 'submit',
    '#value' => 'Save Configuration',
  );

  $form['#submit'][] = 'tripal_phylogeny_default_plots_form_submit';

  return $form;
}

/**
 * Validate the phylotree settings forms
 *
 * @ingroup tripal_phylogeny
 */
function tripal_phylogeny_default_plots_form_validate($form, &$form_state) {

}
/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_phylogeny_default_plots_form_submit($form, &$form_state) {
  // Rebuild this form after submission so that any changes are reflected in
  // the flat tables.
  $form_state['rebuild'] = TRUE;

  if ($form_state['clicked_button']['#name'] == 'submit') {
    variable_set('tripal_phylogeny_default_phylogram_width', $form_state['values']['phylogram_width']);

    variable_set('tripal_phylogeny_default_root_node_size', $form_state['values']['root_node_size']);
    variable_set('tripal_phylogeny_default_interior_node_size', $form_state['values']['interior_node_size']);
    variable_set('tripal_phylogeny_default_leaf_node_size', $form_state['values']['leaf_node_size']);

    $num_orgs = $form_state['values']['num_orgs'];
    variable_set("tripal_phylogeny_num_orgs", $num_orgs);
    $colors = array();
    for ($i = 0; $i < $num_orgs ;$i++) {
      $colors[$i] = array(
        'organism' => $form_state['values']['organism_' . $i],
        'color' => $form_state['values']['color_' . $i]
      );
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
function theme_tripal_phylogeny_admin_org_color_tables($variables){
   $fields = $variables['element'];
   $num_orgs = $fields['num_orgs']['#value'];
   $headers = array('Organism', 'Color', '');
   $rows = array();
   for ($i = 0; $i < $num_orgs; $i++) {
     $add_button = ($i == $num_orgs - 1) ? drupal_render($fields['add']) : '';
     $del_button = ($i == $num_orgs - 1 and $i != 0) ? drupal_render($fields['remove']) : '';
     $rows[] = array(
       drupal_render($fields['organism_' . $i]),
       drupal_render($fields['color_' . $i]),
       $add_button . $del_button,
     );
   }
   $table_vars = array(
     'header' => $headers,
     'rows' => $rows,
     'attributes' => array(),
     'sticky' => FALSE,
     'colgroups' => array(),
     'empty' => '',
   );
   $form['orgs']['num_orgs'] = $fields['num_orgs'];
   return theme('table', $table_vars);
}


/**
 * Ajax callback function for the gensas_job_view_panel_form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_phylogeny_default_plots_form_ajax_callback($form, $form_state) {
 // drupal_debug($form['tree_settings']['org_table']['num_orgs']);

  return $form['node_settings']['org_table'];
}