<?php

/**
 * Provide a data listing for tripal entites (ie: biological data).
 *
 * This function is a callback in a menu item which is set in the
 * TripalEntityUIController class.
 */
function tripal_entities_content_view() {

  // Retrieve our data listing form and render it.
  $form = drupal_get_form('tripal_entities_content_overview_form');
  $output = drupal_render($form);

  // Set the breadcrumb.
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  drupal_set_breadcrumb($breadcrumb);

  return $output;

}

/**
 * Display a listing of Tripal entities.
 *
 * @TODO Filters and bulk operations needed to be added to this form.
 *
 * @param array $form
 * @param array $form_state
 * @return
 *   A form array describing this listing to the Form API.
 */
function tripal_entities_content_overview_form($form, &$form_state) {

  // Set the title to be informative (defaults to content for some reason).
  drupal_set_title('Biological Data');

  // Retrieve a pages list of all tripal entitles (ie: biological data).
  // This will return the 25 most recently created entities.
  $entities = db_select('tripal_entity', 'td')
    ->fields('td')
    ->orderBy('created', 'DESC')//ORDER BY created
    ->range(0,25)
    ->execute();

  $headers = array('Title', 'Vocabulary', 'Term', 'Author', 'Status', 'Updated', 'Operations');
  $rows = array();

  // For each entity retrieved add a row to the data listing.
  while ($entity = $entities->fetchObject()) {

    // Retrieve details about the term this entity is based on.
    $cvterm = tripal_get_cvterm(array('cvterm_id' => $entity->cvterm_id));

    // Retrieve details about the user who created this data.
    $author = user_load($entity->uid);

    // Add information to the table.
    $rows[] = array(
      l($entity->title, 'bio-data/' . $entity->id),
      $cvterm->cv_id->name . ' (' . $cvterm->dbxref_id->db_id->name . ')',
      $cvterm->name,
      l($author->name, 'user/' . $entity->uid),
      $entity->status == 1 ? 'published' : 'unpublished',
      format_date($entity->changed, 'short'),
      l('edit', 'bio-data/' . $entity->id . '/edit') . '&nbsp;&nbsp;' .
      l('delete', 'bio-data/' . $entity->id . '/delete')
    );
  }

  // If there are no entites created yet then add a message to the table to
  // provide guidance to administrators.
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('No biological data available.'),
        'colspan' => 7
      )
    );
  }

  // Render the data listing.
  $table_vars = array(
    'header'     => $headers,
    'rows'       => $rows,
    'attributes' => array(),
    'sticky'     => TRUE,
    'caption'    => '',
    'colgroups'  => array(),
    'empty'      => '',
  );
  $form['results'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', $table_vars),
  );

  return $form;
}

/**
 * Form for creating tripal data types.
 *
 * This form is available on the menu at Admin >> Structure >> Biological Data
 * Types
 *
 * @param array $form
 * @param array $form_state
 *
 * @return
 *    An array describing this form to the Form API.
 */
function tripal_entities_admin_add_type_form($form, &$form_state) {
  $term_name = '';
  $num_terms = 0;
  $cv_id = '';

  // Set defaults using the form state.
  if (array_key_exists('input', $form_state)) {
    if (array_key_exists('term_name', $form_state['input'])) {
      $term_name = $form_state['input']['term_name'];
    }
    if (array_key_exists('cv_id', $form_state['input'])) {
      $cv_id = $form_state['input']['cv_id'];
    }

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

    $terms = tripal_get_cvterm($match, array('return_array' => TRUE));
    $num_terms = count($terms);
  }

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
      '#autocomplete_path' => "admin/tripal/vocab/cvterm/auto_name/$cv_id",
    );
  }
  else {
    $form['term_name'] = array(
      '#type' => 'hidden',
      '#value' => $term_name,
    );
  }

  // If the term belongs to more than one vocabulary then add additional fields
  // to let the user select the vocabulary.
  if ($num_terms > 1) {
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
 * Implements hook_validate() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_admin_add_type_form_validate($form, &$form_state) {

  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='select_cvterm') {

    // First, make sure the term is unique. If not then we can't check it.
    $term_name = NULL;
    $cv_id = NULL;
    $cvterm = NULL;
    if (array_key_exists('term_name', $form_state['values'])) {
      $term_name = $form_state['input']['term_name'];
    }
    if (array_key_exists('cv_id', $form_state['input'])) {
      $cv_id = $form_state['input']['cv_id'];
    }

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
    $terms = tripal_get_cvterm($match, array('return_array' => TRUE));
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
  }
}
/**
 * Implements hook_submit() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_admin_add_type_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#name'] =='select_cvterm') {
    $cvterm = $form_state['storage']['terms'][0];

    $bundle_id = 'bio-data_' . $cvterm->cvterm_id;

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $einfo = entity_get_info('TripalEntity');
    if (!in_array($bundle_id, array_keys($einfo['bundles']))) {
      $error = '';
      $success = tripal_create_entity_type($cvterm, $error);
      if (!$success) {
        drupal_set_message($error, 'error');
        $form_state['redirect'] = "admin/structure/bio-data";
      }
      else {
        drupal_set_message('New biological data type created.  Fields are added automatically to this type.');
        $form_state['redirect'] = "admin/structure/bio-data";
      }
    }
    else {
      drupal_set_message('This type already exists.', 'warning');
    }
  }
}


