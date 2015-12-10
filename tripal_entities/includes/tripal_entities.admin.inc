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
  drupal_set_title('Tripal Content');

  // Retrieve a pages list of all tripal entitles (ie: biological data).
  // This will return the 25 most recently created entities.
  $entities = db_select('tripal_entity', 'td')
    ->fields('td')
    ->orderBy('created', 'DESC')
    ->range(0,25)
    ->execute();

  $headers = array('Title', 'Vocabulary', 'Term', 'Author', 'Status', 'Updated', 'Operations');
  $rows = array();

  // For each entity retrieved add a row to the data listing.
  while ($entity = $entities->fetchObject()) {

    // Get the term
    $term = entity_load('TripalTerm', array('id' => $entity->term_id));
    $term = reset($term);
    $vocab = entity_load('TripalVocab', array('id' => $term->vocab_id));
    $vocab = reset($vocab);

    // Retrieve details about the user who created this data.
    $author = user_load($entity->uid);

    // Add information to the table.
    $rows[] = array(
      l($entity->title, 'bio-data/' . $entity->id),
      $vocab->namespace,
      $term->name,
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
        'data' => t('No Tripal content available.'),
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
 * Types. It requires that a module implmennt the vocabulary storage.  Tripal
 * knows which vocabulary storage methods are available when a module
 * implements the hook_vocab_storage_info() hook.
 *
 */
function tripal_entities_admin_add_type_form($form, &$form_state) {

  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_select_term_form';
    if (function_exists($function)) {
      $form = $function($form, $form_state);
    }
  }
  else {
    tripal_set_message('A storage backend is not enabled for managing
          the vocabulary terms used to create content.  Please enable
          a module that supports storage of vocabualary terms (e.g. tripal_chado)
          and return to create new Tripal content types.', TRIPAL_NOTICE);
  }
  return $form;
}
/**
 * Implements hook_validate() for the tripal_entities_admin_add_type_form.
 *
 */
function tripal_entities_admin_add_type_form_validate($form, &$form_state) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_select_term_form_validate';
    if (function_exists($function)) {
      $function($form, $form_state);
    }
  }
}
/**
 * Implements hook_submit() for the tripal_entities_admin_add_type_form.
 *
 * The storage backend must set the
 *
 */
function tripal_entities_admin_add_type_form_submit($form, &$form_state) {
  $namespace = '';
  $term_id = '';
  if (array_key_exists('storage', $form_state)) {
    $storage = $form_state['storage'];
    $namespace = array_key_exists('namespace', $storage) ? $storage['namespace'] : '';
    $term_id = array_key_exists('term_id', $storage) ? $storage['term_id'] : '';
    $term_name = array_key_exists('term_name', $storage) ? $storage['term_name'] : '';

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $term = tripal_load_term_entity($namespace, $term_id);
    if (!$term) {
      $error = '';
      $success = tripal_create_bundle($namespace, $term_id, $term_name, $error);
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


