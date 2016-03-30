<?php
/**
 * Implements hook_vocab_storage_info().
 */
function tripal_chado_vocab_storage_info() {
  return array(
    'term_chado_storage' => array(
      'label' => t('Chado storage'),
      'module' => 'tripal_chado',
      'description' => t('Integrates terms stored in the local Chado database with Tripal entities.'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_vocab_get_term().
 */
function tripal_chado_vocab_get_term($namespace, $accession) {
  // Create an empty term array for returning if there is a problem.
  $empty_term = array(
    'namespace'  => $namespace,
    'accession'  => $accession,
    'name'       => '',
    'definition' => 'Term is undefined.',
    'url'  => '',
    // The following are not required for the returned array but we'll
    // add these for convenience later when we look at the TripalTerm
    // objects and these will be there.
    'cvterm'     => NULL,
  );

  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cvterm')) {
    return $empty_term;
  }
  $match = array(
    'dbxref_id' => array(
      'db_id' => array(
        'name' => $namespace,
      ),
      'accession' => $accession,
    ),
  );
  $cvterm = chado_generate_var('cvterm', $match);
  if (!$cvterm) {
    return $empty_term;
  }
  $cvterm = chado_expand_var($cvterm, 'field', 'cvterm.definition');
  $term = array(
    'namespace'  => $cvterm->dbxref_id->db_id->name,
    'accession'  => $cvterm->dbxref_id->accession,
    'name'       => $cvterm->name,
    'url'        => '',
    'definition' => (isset($cvterm->definition)) ? $cvterm->definition : '',
    // The following are not required for the returned array but we'll
    // add these for convenience later when we look at the TripalTerm
    // objects and these will be there.
    'cvterm'     => $cvterm,
  );
  if ($cvterm->dbxref_id->db_id->urlprefix) {
    $term['url'] = $cvterm->dbxref_id->db_id->urlprefix .
      $cvterm->dbxref_id->db_id->name . ':' . $cvterm->dbxref_id->accession;
  }
  return $term;
}

/**
 * Implements hook_vocab_import_form();
 */
function tripal_chado_vocab_import_form($form, &$form_state) {
  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader');
  return tripal_cv_obo_form($form, $form_state);
}
/**
 * Implements hook_vocab_import_form_validate();
 */
function tripal_chado_vocab_import_form_validate($form, &$form_state) {
  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader');
  return tripal_cv_obo_form_validate($form, $form_state);
}
/**
 * Implements hook_vocab_import_form_submit();
 */
function tripal_chado_vocab_import_form_submit($form, &$form_state) {
  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader');
  return tripal_cv_obo_form_submit($form, $form_state);
}
/**
 * Implements hook_vocab_select_term_form().
 */
function tripal_chado_vocab_select_term_form($form, &$form_state) {
  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

  // If no term has been selected yet then provide the auto complete field.
  $form['term_name'] = array(
    '#title'       => t('Content Type'),
    '#type'        => 'textfield',
    '#description' => t("The content type must be the name of a term in
        a controlled vocabulary and the controlled vocabulary should
        already be loaded into Tripal.  For example, to create a content
        type for storing 'genes', use the 'gene' term from the
        Sequence Ontology (SO)."),
    '#required'    => TRUE,
    '#default_value' => $term_name,
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
  );
  $form['select_button'] = array(
    '#type' => 'submit',
    '#value' => t('Lookup Term'),
    '#name' => 'select_cvterm',
    '#ajax' => array(
      'callback' => "tripal_chado_vocab_select_term_form_ajax_callback",
      'wrapper' => "tripal-chado-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  if ($term_name) {
    $form['terms_list'] = array(
      '#type' => 'fieldset',
      '#title' => t('Matching Terms'),
      '#description' => t('Please select the term the best matches the
          content type you want to create. If the same term exists in
          multiple vocabularies you will see more than one option below.')
    );
    $match = array(
      'name' => $term_name,
    );
    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = 0;
    foreach ($terms as $term) {
       $form['terms_list']['term-' . $term->cvterm_id] = array(
         '#type' => 'checkbox',
         '#title' =>  $term->name,
         '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name .
             '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
             '<br><b>Definition:</b>  ' . $term->definition,
       );
       $num_terms++;
    }
    if ($num_terms == 0) {
      $form['terms_list']['none'] = array(
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>'
      );
    }
    // Add in the button for the cases of no terms or too many.
    $form['submit_button'] = array(
      '#type' => 'submit',
      '#value' => t('Use this term'),
      '#name' => 'use_cvterm'
    );
  }

  $form['#prefix'] = '<div id = "tripal-chado-vocab-select-form">';
  $form['#suffix'] = '</div>';

  return $form;
}

/**
 * Implements an AJAX callback for the tripal_chado_vocab_select_term_form.
 */
function tripal_chado_vocab_select_term_form_ajax_callback($form, $form_state) {
  return $form;
}

/**
 * Implements hook_vocab_select_term_form_validate().
 */
function tripal_chado_vocab_select_term_form_validate($form, &$form_state) {

  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='use_cvterm') {

    $cvterm_id = NULL;

    // Make sure we have a cvterm selected
    $num_selected = 0;
    foreach ($form_state['values'] as $key => $value) {
      $matches = array();
      if (preg_match("/^term-(\d+)$/", $key, $matches) and
          $form_state['values']['term-' . $matches[1]]) {
        $cvterm_id = $matches[1];
        $term = chado_generate_var('cvterm', array('cvterm_id' => $cvterm_id));
        $num_selected++;
      }
    }
    if ($num_selected == 0) {
      form_set_error('', 'Please select at least one term.');
    }
    else if ($num_selected > 1) {
      form_set_error('term-' . $cvterm_id, 'Please select only one term from the list below.');
    }
    else {
      $form_state['storage']['namespace'] = $term->dbxref_id->db_id->name;
      $form_state['storage']['accession'] = $term->dbxref_id->accession;
      $form_state['storage']['term_name'] = $term->name;
    }
  }
  // For any other button click it's an AJAX call and we just want to reubild
  // the form.
  else {
    $form_state['rebuild'] = TRUE;
  }
}
