<?php

/**
 *
 */
function tripal_organism_admin_organism_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Organisms', 'admin/tripal/chado/tripal_organism');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_organism_admin_organisms','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Organism module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Organisms View', 'admin/tripal/chado/tripal_organism/views/organisms/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings for chado_orgnism
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin() {

  $form = array();

  $form['nothing'] = array(
    '#markup' => t('There are currently no settings to configure.')
  );
  //get_tripal_organism_admin_form_reindex_set($form);
  //get_tripal_organism_admin_form_taxonomy_set($form);

  return system_settings_form($form);
}

/**
 *
 *
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_taxonomy_set(&$form) {
  $form['taxonify'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assign Drupal Taxonomy to Organism Features')
  );

  // get the list of libraries
  $sql = "SELECT * FROM {Organism} ORDER BY genus,species";
  $org_rset = chado_query($sql);

  // iterate through all of the libraries
  $org_boxes = array();
  foreach ($org_rset as $organism) {
    $org_boxes[$organism->organism_id] = "$organism->genus $organism->species";
  }

  $form['taxonify']['description'] = array(
    '#type' => 'item',
    '#value' => t(
      "Drupal allows for assignment of \"taxonomy\" or catagorical terms to " .
      "nodes. These terms allow for advanced filtering during searching. This option allows " .
      "for setting taxonomy only for features that belong to the selected organisms below.  All " .
      "other features will be unaffected.  To set taxonomy for all features in the site see the Feature Administration page."),
    '#weight' => 1,
  );

  $form['taxonify']['tx-organisms'] = array(
    '#title'       => t('Organisms'),
    '#type'        => t('checkboxes'),
    '#description' => t("Check the organisms whose features you want to reset taxonomy.  Note: this list contains all organisms, even those that may not be synced."),
    '#required'    => FALSE,
    '#prefix'      => '<div id="lib_boxes">',
    '#suffix'      => '</div>',
    '#options'     => $org_boxes,
    '#weight'      => 2
  );
  $form['taxonify']['tx-button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Feature Taxonomy'),
    '#weight'      => 3
  );
}
/**
 *
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_reindex_set(&$form) {
  // define the fieldsets
  $form['reindex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reindex Organism Features')
  );

  // get the list of libraries
  $sql = "SELECT * FROM {Organism} ORDER BY genus,species";
  $org_rset = chado_query($sql);

  // iterate through all of the libraries
  $org_boxes = array();
  foreach ($org_rset as $organism) {
    $org_boxes[$organism->organism_id] = "$organism->genus $organism->species";
  }
  $form['reindex']['description'] = array(
    '#type' => 'item',
    '#value' => t("This option allows for reindexing of only those features that belong to the selected organisms below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
    '#weight' => 1,
  );

  $form['reindex']['re-organisms'] = array(
    '#title'       => t('Organisms'),
    '#type'        => t('checkboxes'),
    '#description' => t("Check the organisms whose features you want to reindex. Note: this list contains all organisms, even those that may not be synced."),
    '#required'    => FALSE,
    '#prefix'      => '<div id="lib_boxes">',
    '#suffix'      => '</div>',
    '#options'     => $org_boxes,
    '#weight' => 2,
  );
  $form['reindex']['re-button'] = array(
    '#type' => 'submit',
    '#value' => t('Reindex Features'),
    '#weight' => 3,
  );
}

/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    $organisms = $form_state['values']['re-organisms'];
    foreach ($organisms as $organism_id) {
      if ($organism_id and preg_match("/^\d+$/i" , $organism_id)) {
        // get the organism info
        $sql = "SELECT * FROM {organism} WHERE organism_id = :organism_id";
        $organism = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();
        $job_args[0] = $organism_id;
        tripal_add_job("Reindex features for organism: $organism->genus " .
         "$organism->species", 'tripal_organism' ,
         'tripal_organism_reindex_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the taxonomy Job if selected
  if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
    $organisms = $form_state['values']['tx-organisms'];
    foreach ($organisms as $organism_id) {
      if ($organism_id and preg_match("/^\d+$/i", $organism_id)) {
        // get the organism info
        $sql = "SELECT * FROM {organism} WHERE organism_id = :organism_id";
        $organism = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();
        $job_args[0] = $organism_id;
        tripal_add_job("Set taxonomy for features in organism: " .
          "$organism->genus $organism->species" , 'tripal_organism',
          'tripal_organism_taxonify_features', $job_args, $user->uid);
      }
    }
  }
}


/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_reindex_features($organism_id = NULL, $job_id = NULL) {
  $i = 0;

  if (!$organism_id) {
    return;
  }

  $sql = "
    SELECT *
    FROM {feature}
    WHERE organism_id = :organism_id
    ORDER BY feature_id";
  $results = chado_query($sql, array(':organism_id' => $organism_id));

  // load into ids array
  $count = 0;
  $ids = array();
  foreach ($results as $id) {
    $ids[$count] = $id->feature_id;
    $count++;
  }

  $interval = intval($count * 0.01);
  foreach ($ids as $feature_id) {
    // update the job status every 1% features
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id , intval(($i/$count)*100));
    }
    $i++;
  }
}
/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_taxonify_features($organism_id = NULL, $job_id = NULL) {
  $i = 0;

  if (!$organism_id) {
    return;
  }

  $sql = "
    SELECT *
    FROM {feature}
    WHERE organism_id = :organism_id
    ORDER BY feature_id
  ";
  $results = chado_query($sql, array(':organism_id' => $organism_id));

  // load into ids array
  $count = 0;
  $ids = array();
  foreach ($results as $id) {
    $ids[$count] = $id->feature_id;
    $count++;
  }

  // make sure our vocabularies are set before proceeding
  tripal_feature_set_vocabulary();

  // use this SQL for getting the nodes
  $nsql = "SELECT * FROM {chado_feature} CF " .
          "  INNER JOIN {node} N ON N.nid = CF.nid " .
          "WHERE feature_id = :feature_id";

  // iterate through the features and set the taxonomy
  $interval = intval($count * 0.01);
  foreach ($ids as $feature_id) {
    // update the job status every 1% features
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id, intval(($i/$count)*100));
    }
    $node = db_query($nsql, array(':feature_id' => $feature_id))->fetchObject();
    tripal_feature_set_taxonomy($node, $feature_id);
    $i++;
  }
}
