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
function tripal_cv_admin_set_defaults_form() {
  $form = array();
  
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
    $form['settings']['existing'][$cv_default_id]["table_name"] = array(
      '#type' => 'markup',
      '#markup' => $cv_default->table_name
    );
    $form['settings']['existing'][$cv_default_id]["field_name"] = array(
      '#type' => 'markup',
      '#markup' => $cv_default->field_name
    );
    $form['settings']['existing'][$cv_default_id]["vocabulary"] = array(
      '#type' => 'select',
      '#options' => $cvs,
      '#default_value' => $cv_default->cv_id,
      '#suffix' => '<a target="_blank" href="'. url('admin/tripal/chado/tripal_cv/cvterms') . '?cv=' . $cv->name . '">View terms</a>'
    );
    // remove button
    $form['settings']['existing'][$cv_default_id]['remove'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#name' => "cv_remove-$cv_default_id",
      '#ajax' => array(
        'callback' => "tripal_cv_admin_set_defaults_ajax_update",
        'wrapper' => 'tripal-generic-edit-properties-table',
        'effect'   => 'fade',
        'method'   => 'replace',
        'prevent'  => 'click'
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default node form submit.  In the validate function we validate only the
      // property fields and in the submit we remove the indicated property
      // from the chado_properties array. In order to keep validate errors
      // from the node form validate and Drupal required errors for non-property fields
      // preventing the user from removing properties we set the #limit_validation_errors below
      '#validate' => array('chado_add_node_form_properties_remove_button_validate'),
      '#submit' => array('chado_add_node_form_properties_remove_button_submit'),
      // Limit the validation of the form upon clicking this button to the property_table tree
      // No other fields will be validated (ie: no fields from the main form or any other api
      // added form).
      '#limit_validation_errors' => array(
        array('property_table')  // Validate all fields within $form_state['values']['property_table']
      )
    );
  }
  
  return $form;
}

/**
 * 
 * @param unknown $variables
 */
function theme_tripal_cv_admin_set_defaults_form($variables) {
  $element = $variables['element'];

  $header = array(
    'table_name'      => array('data' => t('Table Name'),      'width' => '20%'),
    'field_name'      => array('data' => t('Field Name'),      'width' => '20%'),
    'vocabulary'      => array('data' => t('Default Vocabulary'), 'width' => '40%'),
    'property_action' => array('data' => t('Actions'),    'width' => '20%'),
  );

  $rows = array();
  
  foreach ($element['settings']['existing'] as $key => $value) {
    if (is_numeric($key)) {
      $rows[] = array(
        drupal_render($value['table_name']),
        drupal_render($value['field_name']),
        drupal_render($value['vocabulary']),
        drupal_render($value['remove']),
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