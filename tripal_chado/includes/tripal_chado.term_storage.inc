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
 * Implements hook_vocab_select_term_form().
 */
function tripal_chado_vocab_select_term_form($form, &$form_state) {
  $term_name = '';
  $num_terms = 0;
  $cv_id = '';
  $terms = array();

  // Set defaults using the form state.
  if (array_key_exists('storage', $form_state)) {
    if (array_key_exists('terms', $form_state['storage'])) {
      $terms = $form_state['storage']['terms'];
    }
  }
  $num_terms = count($terms);

  // If no term has been selected yet then provide the auto complete field.
  if ($num_terms == 0) {
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
      '#autocomplete_path' => "admin/tripal/vocab/cvterm/auto_name/$cv_id/",
    );
  }

  // If the term belongs to more than one vocabulary then add additional fields
  // to let the user select the vocabulary.
  if ($num_terms > 1) {
    $form['term_name'] = array(
      '#type' => 'hidden',
      '#value' => $term_name,
    );

    $cvs = array();
    foreach ($terms as $term) {
      $cvs[$term->cv_id->cv_id] = 'Vocabulary: <b>' . $term->cv_id->name . '</b> (' . $term->cv_id->definition . ')<br>' . $term->name . ': ' . $term->definition;
    }
    $form['cv_id'] = array(
      '#type' => 'radios',
      '#title' => t('Select the appropriate vocabulary'),
      '#options' => $cvs,
    );
  }

  // Add in the button for the cases of no terms or too many.
  $form['select_button'] = array(
    '#type' => 'submit',
    '#value' => t('Use this term'),
    '#name' => 'select_cvterm'
  );
  return $form;
}

/**
 * Implements hook_vocab_select_term_form_validate().
 */
function tripal_chado_vocab_select_term_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='select_cvterm') {

    // First, make sure the term is unique. If not then we can't check it.
    $term_name = NULL;
    $cv_id = NULL;
    $cvterm = NULL;
    $term_id = NULL;
    if (array_key_exists('term_name', $form_state['values'])) {
      $term_name = $form_state['input']['term_name'];
    }
    if (array_key_exists('cv_id', $form_state['input'])) {
      $cv_id = $form_state['input']['cv_id'];
    }

    // If a term and $cv_id are provided then we can look for the term using
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
    $form_state['storage']['terms'] = $terms;

    // If we do not have any terms then the term provided by the user does not
    // exists and we need to provide an error message.
    if (count($terms) == 0) {
      form_set_error('term_name', t('The term does not exist in this database.'));
    }
    // If we have more than one term then we need to set an error so that the
    // form can provide a list of vocabularies to select from.
    if (count($terms) > 1) {
      form_set_error('term_name', t('The term is not unique. A list of vocabularies
        that contain this term. Please select the most appropriate vocabulary.'));
    }
    // If we have a unique term then set the namespace, term_id and name.
    if (count($terms) == 1) {
      $form_state['storage']['namespace'] = $terms[0]->dbxref_id->db_id->name;
      $form_state['storage']['term_id']   = $terms[0]->dbxref_id->accession;
      $form_state['storage']['term_name'] = $terms[0]->name;
    }
  }
}
