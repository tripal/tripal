<?php

/**
 * Launchpad for feature administration.
 *
 * @ingroup tripal_feature
 */
function tripal_entities_admin_view() {

  $form = drupal_get_form('tripal_entities_admin_bundles_form');
  $output = drupal_render($form) . "<br>[ Image Place Holder for Data Type Summary ]<br>";

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Biological Data', 'admin/tripal/bundles');
  drupal_set_breadcrumb($breadcrumb);

/*   // Add the view
  $view = views_embed_view('tripal_feature_admin_features','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Feature module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
        . 'administrative interface are disabled. <strong>Click each of the following links to '
          . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>'.l('Features View', 'admin/tripal/chado/tripal_feature/views/features/enable').'</li>';
    $output .= '</ul>';
  }

  // Add a summary chart.
  //-----------------------------------
  $output .= theme('tripal_feature_bar_chart_type_organism_summary');
  drupal_add_js('
    Drupal.behaviors.tripalFeature_moveAdminSummaryChart = {
      attach: function (context, settings) {
        jQuery("#tripal-feature-admin-summary").insertBefore( jQuery(".view-filters") );
    }};
  ', 'inline'); */

  return $output;
}

/**
 *
 */
function tripal_entities_content_view() {
  $form = drupal_get_form('tripal_entities_content_overview_form');
  $output = drupal_render($form);

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  drupal_set_breadcrumb($breadcrumb);

  return $output;

}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function tripal_entities_content_overview_form($form, &$form_state) {
  $form = array();

  $entities = db_select('tripal_entity', 'td')
    ->fields('td')
    ->orderBy('created', 'DESC')//ORDER BY created
    ->range(0,25)
    ->execute();

  $headers = array('Title', 'Vocabulary', 'Term', 'Author', 'Status', 'Updated', 'Operations');
  $rows = array();
  while ($entity = $entities->fetchObject()) {
    $author = user_load($entity->uid);
    $rows[] = array(
      l($entity->title, 'BioData/' . $entity->id),
      $entity->type,
      $entity->bundle,
      l($author->name, 'user/' . $entity->uid),
      $entity->status == 1 ? 'published' : 'unpublished',
      format_date($entity->changed, 'short'),
      l('edit', 'BioData/' . $entity->id . '/edit') . '&nbsp;&nbsp;' .
      l('delete', 'BioData/' . $entity->id . '/delete')
    );
  }

  $table_vars = array(
    'header'     => $headers,
    'rows'       => $rows,
    'attributes' => array(),
    'sticky'     => TRUE,
    'caption'    => '',
    'colgroups'  => array(),
    'empty'      => '',
  );
  $results_table = theme('table', $table_vars);

  $form['results'] = array(
    '#markup' => $results_table
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function tripal_entities_admin_bundles_form($form, &$form_state) {
  $form = array();

  // Set the defaults.
  $cv_id = NULL;
  $term_name = NULL;

  // Set defaults using the form state.
  if (array_key_exists('values', $form_state)) {
    $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : NULL;
    $term_name = array_key_exists('term_name', $form_state['values']) ? $form_state['values']['term_name'] : NULL;
  }

  // Let the user select the vocabulary and tripal_entity but only if they haven't
  // already selected a tripal_entity.
  $sql = "
    SELECT CV.cv_id, CV.name
    FROM {cv} CV
    ORDER BY CV.name
  ";
  $vocabs = chado_query($sql);
  $cvs = array();
  while ($vocab = $vocabs->fetchObject()) {
    $cvs[$vocab->cv_id] = $vocab->name;
  }
  $form['cv_id'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#options' => $cvs,
    '#required' => FALSE,
    '#description' => t('Select a vocabulary to view potential data types in the chart below. Limit the chart to only published data types by selecting the checkbox.'),
    '#default_value' => $cv_id,
    '#ajax' => array(
      'callback' => "tripal_entities_admin_bundles_form_ajax_callback",
      'wrapper' => 'tripal_entities_admin_bundles_form',
      'effect' => 'fade',
      'method' => 'replace'
    )
  );

  $form['refresh_bundles'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh Data Types'),
    '#name' => 'refresh_bundles',
  );

  $form['publish_new_data'] = array(
    '#type' => 'submit',
    '#value' => t('Publish New Data'),
    '#name' => 'publish_new_data',
  );

  $form['#prefix'] = '<div id="tripal_entities_admin_bundle_form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Submit a job to populate the entity tables
 * This operation makes available data types in the database publishable
 */
function tripal_entities_admin_bundles_form_submit($form, $form_state) {
  global $user;
  if ($form_state['clicked_button']['#name'] == 'refresh_bundles') {
    tripal_add_job('Create publishable data types', 'tripal_entity', 'tripal_entities_populate_entity_tables', array(), $user->uid);
  }
  if ($form_state['clicked_button']['#name'] == 'publish_new_data') {
  }
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function tripal_entities_admin_add_type_form($form, &$form_state) {

  $term_name = '';
  $num_terms = 0;
  $cv_id = '';

  // Set defaults using the form state.
  if (array_key_exists('storage', $form_state) and
      array_key_exists('terms', $form_state['storage'])) {
    $terms = $form_state['storage']['terms'];
    $num_terms = count($terms);
  }
  if (array_key_exists('values', $form_state) and
      array_key_exists('term_name', $form_state['values'])) {
    $term_name = $form_state['values']['term_name'];
  }

  // If no term has been selected yet then provide the auto complete field.
  if ($num_terms == 0) {
    $form['term_name'] = array(
      '#title'       => t('Biological Data Type'),
      '#type'        => 'textfield',
      '#description' => t("Please enter the type of data that you want to add.
          Once added, priviledged users can add new records of the selected
          type. As you type, suggestions will be provided."),
      '#required'    => TRUE,
      '#default_value' => $term_name,
      '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/$cv_id",
    );
  }


  // If the term belongs to more than one vocabulary then add additional fields
  // to let the user select the vocabulary.
  if ($num_terms > 1) {
    $cvs = array();
    foreach ($terms as $term) {
      $cvs[$term->cv_id->cv_id] = $term->cv_id->name;
    }
    $form['cv_id'] = array(
      '#type' => 'radios',
      '#title' => t('Select the appropriate vocabulary'),
      '#options' => $cvs,
      '#description' => t('The term belongs to more than one vocabulary. Please
          indicate the proper vocabulary for the term.')
    );
  }

  // Add in the button for the cases of no terms or too many.
  if ($num_terms != 1) {
    $form['select_button'] = array(
      '#type' => 'submit',
      '#value' => t('Use this term'),
      '#name' => 'select_cvterm'
    );
  }

  return $form;
}

/**
 * Implements hook_validate() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_admin_add_type_form_validate($form, &$form_state) {
  // Check if this term and vocabulary is in the tripal_vocabulary usage tables.
  // If not then add it.
  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='select_cvterm') {

    // First, make sure the term is unique. If not then we can't check it.
    $term_name = $form_state['values']['term_name'];
    $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : '';
    $cvterm = NULL;

    // If a term and cv_id are provided then we can look for the term using
    // both and we should find a unique term. If only ther term is provided
    // we can still look for a unique term but there must only be one.
    if ($term_name and !$cv_id) {
      $match = array(
        'name' => $term_name,
      );
    }
    else {
      $match = array(
        'name' => $term_name,
        'cv_id' => $cv_id,
      );
    }
    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));

    // Add the cvterm to the storage element so we don't have to keep
    // looking it up in the submit function or on a rebuild of the form.
    $form_state['storage']['terms'] = $terms;

    // If we only have one term then we found a unique match and we can do
    // some further checking.
    if (count($terms) == 1) {
      $cvterm = $terms[0];
      // Make sure the term is set as published.
      tripal_entities_add_term_usage($cvterm, $form_state);
    }
    // If we do not have any terms then the term provided by the user does not
    // exists and we need to provide an error message.
    if (count($terms) == 0) {
      form_set_error('term_name', t('The term does not exist in this database.'));
    }
    // If we have more than one term then we need to set an error so that the
    // form can provide a list of vocabularies to select from.
    if (count($terms) > 1) {
      form_set_error('', t('The term is not unique. A list of vocabularies has been provided where this term is present. Please select the appropriate one.'));
    }
  }
}
/**
 * Implements hook_submit() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_admin_add_type_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] =='select_cvterm') {
    $cvterm = $form_state['storage']['terms'][0];
    $bundle_id = 'dbxref_' . $cvterm->dbxref_id->dbxref_id;

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $einfo = entity_get_info('BioData');
    if (!in_array($bundle_id, array_keys($einfo['bundles']))) {
      tripal_entities_add_term_usage($cvterm, $form_state);
      tripal_entities_add_bundle($cvterm);
      drupal_set_message('New biological data type created.  Fields are added automatically to this type.');
      $form_state['redirect'] = "admin/structure/BioData";
    }
    else {
      drupal_set_message('This type already exists.', 'warning');
    }
  }
}

/**
 * Adds a vocabulary and term to the Tripal term usage tables.
 *
 * This function is meant to be called only by the
 * tripal_entities_entity_form_validate() function. This code is
 * separated to simplify that function.  Therefore, if errors occur with adding
 * of terms then the form_set_error() is called.
 *
 * @param $cvterm
 */
function tripal_entities_add_term_usage($cvterm, &$form_state) {

  // Before creating the entity we mut add records to the tripal_vocabulary
  // tripal_vocabulary_usage, tripal_term, and tripal_term_usage tables.
  $match = array('cv_id' => $cvterm->cv_id->cv_id);
  $vocab = chado_select_record('tripal_vocabulary', array('*'), $match);
  if (count($vocab) == 0) {
    $values = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'db_id' => $cvterm->dbxref_id->db_id->db_id,
      'publish' => 1,
    );
    $values = chado_insert_record('tripal_vocabulary', $values);
    if (!$values) {
      form_set_error('', 'Could not add vocabulary to tripal_vocabluary table.');
      return FALSE;
    }
    // Convert the values array into an object.
    $vocab = new stdClass();
    $vocab->vocabulary_id = $values['vocabulary_id'];
    $vocab->cv_id = $values['cv_id'];
  }
  else {
    // Make sure the vocabulary is set to publish
    $values = array('publish' => 1);
    chado_update_record('tripal_vocabulary', $match, $values);
    $vocab = $vocab[0];
  }

  // Does this vocabulary have a record in the tripal_vocabulary_usage
  // table? If not then add one.
  $match = array('vocabulary_id' => $vocab->vocabulary_id);
  $vocab_usage = chado_select_record('tripal_vocabulary_usage', array('*'), $match);
  if (count($vocab_usage) == 0) {
    // Look to see if this vocabulary is used as a default for any table. If
    // so then we can use that to populate the tripal_vocabulary_usage table.
    $default = db_select('tripal_cv_defaults', 't')
    ->fields('t')
    ->condition('cv_id', $vocab->cv_id)
    ->execute()
    ->fetchObject();
    if ($default) {
      $values = array(
        'vocabulary_id' => $vocab->vocabulary_id,
        'data_table' => $default->table_name,
        'type_table' => $default->table_name,
        'field' =>  $default->field_name,
      );
      $values = chado_insert_record('tripal_vocabulary_usage', $values);
      if (!$values) {
        form_set_error('', 'Could not add vocabulary to tripal_vocabulary_usage table.');
        return FALSE;
      }
    }
    // If there is no default table then we have an error, and we should
    // set a variable so that the form can help the user deal with the problem.
    else {
      $form_state['storage']['cvterm_has_default'] = FALSE;
      form_set_error('', t('There is no default mapping of this term\'s
          vocabulary to a table in Chado.  Therefore, it is not possible to
          determine how to store data of this type.'));
      return FALSE;
    }
    $vocab_usage = new stdClass();
    $vocab_usage->vocabulary_id = $values['vocabulary_id'];
    $vocab_usage->data_table = $values['data_table'];
    $vocab_usage->type_table = $values['type_table'];
    $vocab_usage->field = $values['field'];
  }
  else {
    $vocab_usage = $vocab_usage[0];
  }

  // Now add the tripal_term record if it doesn't already exist.
  $match = array(
    'vocabulary_id' => $vocab->vocabulary_id,
    'cvterm_id' => $cvterm->cvterm_id,
  );
  $term = chado_select_record('tripal_term', array('*'), $match);
  if (count($term) == 0) {
    $values = array(
      'vocabulary_id' => $vocab->vocabulary_id,
      'cvterm_id' => $cvterm->cvterm_id,
    );
    $values = chado_insert_record('tripal_term', $values);
    if (!$values) {
      form_set_error('', 'Could not add term to tripal_term table..');
      return FALSE;
    }
    $term = new stdClass();
    $term->term_id = $values['term_id'];
  }
  else {
    $values = array('publish' => 1);
    chado_update_record('tripal_term', $match, $values);
    $term = $term[0];
  }

  // Finally, add the tripal_term_usage record if it doesn't already exist.
  $match = array('term_id' => $term->term_id);
  $options = array('has_record' => TRUE);
  if (!chado_select_record('tripal_term_usage', array('*'), $match, $options)) {
    $values = array(
      'term_id' => $term->term_id,
      'data_table' => $vocab_usage->data_table,
      'type_table' => $vocab_usage->type_table,
      'field' =>  $vocab_usage->field,
    );
    $values = chado_insert_record('tripal_term_usage', $values);
    if (!$values) {
      form_set_error('', 'Could not add term to tripal_term table..');
      return FALSE;
    }
  }

  // Clear the entity cache so that Drupal will read our
  // hook_entity_info() implementation which now will have the entities
  // described because we set the publish column to 1 in the tripal_term
  // table.
  global $language;
  $langcode = $language->language;
  cache_clear_all("entity_info:$langcode", 'cache');

  return TRUE;
}
/**
 * Implements hook_chado_field_alter.
 *
 * This function is used when new Chado fields are addd to an Entity.  It
 * allows modules to customize the field, widget types and settings for
 * a field before it is created.
 *
 * @param $field
 */
function hook_chado_field_alter(&$field) {
  // TODO: add example code for how to use this hook.
}
/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function tripal_entities_admin_access_form($form, &$form_state) {
  $form = array();

  return $form;
}
