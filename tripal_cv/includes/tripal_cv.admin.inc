<?php
/**
 * @file
 * Provides administration of controlled vocabularies & their terms.
 */

/**
 * Provide landing page to the new admin pages
 *
 * @ingroup tripal_cv
 */
function tripal_cv_admin_cv_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Modules', 'admin/tripal/chado');
  $breadcrumb[] = l('Vocabularies', 'admin/tripal/chado/tripal_cv');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $cvs_view = views_embed_view('tripal_cv_admin_cvs','default');
  $cvterms_view = views_embed_view('tripal_cv_admin_cvterms','default');
  if (isset($cvs_view) && isset($cvterms_view)) {
    $output .= $cvs_view;
  }
  else {
    $output .= '<p>The Tripal Controlled Vocabulary module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    if (!isset($cvs_view)) {
      $output .= '<li>'.l('Tripal Vocabularies', 'admin/tripal/chado/tripal_cv/views/cvs/enable').'</li>';
    }
    if (!isset($cvterm_view)) {
      $output .= '<li>'.l('Tripal Vocabulary Terms', 'admin/tripal/chado/tripal_cv/views/cvterms/enable').'</li>';
    }
    $output .= '</ul>';
  }

  return $output;
}

/**
 *
 */
function tripal_cv_admin_set_defaults_form($form, $form_state) {

  $form['instructions'] = array(
    '#markup' => t('Much of the data housed in Chado is typed, meaning that a ' .
        'controlled vocabulary describes what type of data the record is. For example, '.
        'a feature must have a "type" which is typically a term from ' .
        'the Sequence Ontology. Record properties typically have a type as well. '.
        'Tripal allows the administrator to set a default type for each table in '.
        'Chado that requires a type from a vocabulary. By default, autocomplete fields, '.
        'type select boxes and type validation occur using the default vocabularies set below. '),
  );

  // get the list of all tables that use the cvterm table as an FK
  $cvterm_schema = chado_get_schema('cvterm');
  $referring_tables = $cvterm_schema['referring_tables'];

  // get the list of tables that already have default set
  $cv_defaults = db_select('tripal_cv_defaults', 'TCD')
    ->fields('TCD', array('cv_default_id', 'table_name', 'field_name', 'cv_id'))
    ->orderBy('table_name', 'ASC')
    ->execute();

  // get the list of vocabularies
  $cvs = tripal_get_cv_select_options();

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configured Defaults'),
    '#description' => t('The following tables have a default vocabulary'),
    '#tree' => TRUE,
  );
  foreach ($cv_defaults as $cv_default) {
    $cv_default_id = $cv_default->cv_default_id;
    $cv = tripal_get_cv(array('cv_id' => $cv_default->cv_id));

    $form['settings']['existing'][$cv_default_id]["id"] = array(
      '#type' => 'hidden',
      '#value' => $cv_default_id,
    );

    // Display
    $form['settings']['existing'][$cv_default_id]["table_name-display"] = array(
      '#type' => 'markup',
      '#markup' => $cv_default->table_name
    );
    $form['settings']['existing'][$cv_default_id]["field_name-display"] = array(
      '#type' => 'markup',
      '#markup' => $cv_default->field_name
    );

    // Save for use in submit
    $form['settings']['existing'][$cv_default_id]["table_name"] = array(
      '#type' => 'hidden',
      '#value' => $cv_default->table_name
    );
    $form['settings']['existing'][$cv_default_id]["field_name"] = array(
      '#type' => 'hidden',
      '#value' => $cv_default->field_name
    );

    // Selectbox to set the vocabulary
    if (!empty($cv)) {
      $default_cv = $cv_default->cv_id;
    }
    else {
      $cvs[0] = 'NONE SET';
      $default_cv = 0;
    }
    $form['settings']['existing'][$cv_default_id]["vocabulary"] = array(
      '#type' => 'select',
      '#options' => $cvs,
      '#default_value' => $default_cv,
    );

    // Actions
    $view_terms = l('New Vocabulary', 'admin/tripal/chado/tripal_cv/cv/add');
    $add_term = '';
    if (!empty($cv)) {
      $view_terms = l(
        'View Terms',
        'admin/tripal/chado/tripal_cv/cvterms',
        array('query' => array('cv' => $cv->name))
      );

      $add_term = l(
        'Add Term',
        'admin/tripal/chado/tripal_cv/cv/' . $cv->cv_id . '/cvterm/add'
      );
    }
    $form['settings']['existing'][$cv_default_id]["view-terms"] = array(
      '#type' => 'markup',
      '#markup' => $view_terms
    );
    $form['settings']['existing'][$cv_default_id]["add-new-term"] = array(
      '#type' => 'markup',
      '#markup' => $add_term
    );

  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Update Defaults'
  );

  return $form;
}

function tripal_cv_admin_set_defaults_form_submit($form, $form_state) {

  foreach ($form_state['values']['settings']['existing'] as $default_cv) {
    if (!empty($default_cv['vocabulary'])) {
      tripal_set_default_cv(
        $default_cv['table_name'],
        $default_cv['field_name'],
        '', // We are passing in the cv_id so we don't need the name
        $default_cv['vocabulary']
      );
    }
  }

}

/**
 *
 * @param unknown $variables
 */
function theme_tripal_cv_admin_set_defaults_form($variables) {
  $element = $variables['element'];

  $header = array(
    'table_name'      => array('data' => t('Table Name'),         'width' => '20%'),
    'field_name'      => array('data' => t('Field Name'),         'width' => '20%'),
    'vocabulary'      => array('data' => t('Default Vocabulary'), 'width' => '30%'),
    'actions'         => array('data' => t('Actions'),            'width' => '30%'),
  );

  $rows = array();

  foreach ($element['settings']['existing'] as $key => $value) {
    if (is_numeric($key)) {

      $action_links = '<ul class="links inline">';
      if (!empty($value['view-terms'])) {
        $action_links .= '<li>' . drupal_render($value['view-terms']) . '</li>';
      }
      if (!empty($value['add-new-term'])) {
        $action_links .= '<li>' . drupal_render($value['add-new-term']) . '</li>';
      }
      $action_links .= '</li></ul>';

      $rows[] = array(
        drupal_render($value['table_name-display']),
        drupal_render($value['field_name-display']),
        drupal_render($value['vocabulary']),
        $action_links
      );
    }
  }
  $settings_table = theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));
  $element['settings']['existing'] = array(
    '#type' => 'markup',
    '#markup' => $settings_table,
  );

  return drupal_render_children($element);

}