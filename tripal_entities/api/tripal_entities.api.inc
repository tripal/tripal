<?php

/**
 * Retrieves a TripalTerm entity that matches the given arguments.
 *
 * @param $namespace
 *   The namespace for the vocabulary
 * @param $term_id
 *   The ID (accession) of the term in the vocabulary.
 *
 * @return
 *   A TripalTerm entity object or NULL if not found.
 */
function tripal_load_term_entity($namespace, $term_id) {
  $query = db_select('tripal_term', 'tt');
  $query->join('tripal_vocab' ,'tv', 'tv.id = tt.vocab_id');
  $query->fields('tt', array('id', 'term_id'))
    ->fields('tv', array('namespace'))
    ->condition('tv.namespace', $namespace)
    ->condition('tt.term_id', $term_id);
  $term = $query->execute()->fetchObject();

  if ($term) {
    $entity = entity_load('TripalTerm', array($term->id));
    return reset($entity);
  }
  return NULL;
}

/**
 * Retrieves a TripalVocab entity that maches the given arguments.
 *
 * @param $namespace
 *
 * @return
 * A TripalVocab entity object or NULL if not found.
 */
function tripal_load_vocab_entity($namespace) {
  $vocab = db_select('tripal_vocab', 'tv')
    ->fields('tv')
    ->condition('tv.namespace', $namespace)
    ->execute()
    ->fetchObject();

  if ($vocab) {
    $entity = entity_load('TripalVocab', array($vocab->id));
    return reset($entity);
  }
  return NULL;
}

/**
 * Creates a new Tripal Entity type (i.e. bundle).
 *
 * @param $namespace
 *   The abbreviated namespace for the vocabulary (e.g. RO, SO, PATO).
 * @param $term_id
 *   The unique term ID in the vocabulary $namespace (i.e. an accession).
 * @param $term_name
 *   A human-readable name for this term.  This will became the name that
 *   appears for the content type.  In practice, this should be the name
 *   of the term. (E.g. the name for SO:0000704 is gene).
 * @param $error
 *  A string, passed by reference, that is filled with the error message
 *  if the function fails.
 *
 * @return
 *  TRUE if the entity type (bundle) was succesfully created.  FALSE otherwise.
 */
function tripal_create_bundle($namespace, $term_id, $term_name, &$error = '') {

  // First create the TripalVocab if it doesn't already exist.
  $vocab = tripal_load_vocab_entity($namespace);
  if (!$vocab) {
    $vocab = entity_get_controller('TripalVocab')->create(array('namespace' => $namespace));
    $vocab->save();
  }

  // Next create the TripalTerm if it doesn't already exist.
  $term = tripal_load_term_entity($namespace, $term_id);
  if (!$term) {
    $args = array('vocab_id' => $vocab->id, 'term_id' => $term_id, 'name' => $term_name);
    $term = entity_get_controller('TripalTerm')->create($args);
    $term = $term->save();
  }


  // If the bundle doesn't already exist, then add it.
  $bundle_id = 'bio-data_' . $term->id;
  $einfo = entity_get_info('TripalEntity');
  if (!in_array($bundle_id, array_keys($einfo['bundles']))) {
    // Inser the bundle.
    db_insert('tripal_bundle')
      ->fields(array(
        'label' => $term_name,
        'type' => 'TripalEntity',
        'bundle' => $bundle_id,
      ))
      ->execute();
  }

  // Clear the entity cache so that Drupal will read our
  // hook_entity_info() implementation.
  global $language;
  $langcode = $language->language;
  cache_clear_all("entity_info:$langcode", 'cache');
  variable_set('menu_rebuild_needed', TRUE);


  // Allow modules to now add fields to the bundle
  module_invoke_all('add_bundle_fields', 'TripalEntity', $bundle_id, $term);

  return TRUE;
}


/**
 * Allows a module to add key/value pairs about a bundle.
 *
 * If a module needs to associate variables with a particular TripalEntity
 * type (bundle), it can do so by setting the $bundle_data array passed into
 * this function.  This hook is called prior to creation of a new entity type.
 *
 * @param $bundle_data
 *   An array for key/value pairs to be associated with a bundle.
 * @param $bundle_id
 *   The ID for the bundle.
 * @param $cvterm
 *   The CV term object that the bundle represents.
 */
function hook_tripal_bundle_data_alter(&$bundle_data, $bundle_id, $cvterm) {
  // Get the cvterm for this entity type.
  $bundle_id = $entity->bundle;
  $cvterm_id = preg_replace('/bio-data_/', $bundle_id);
  $cvterm = tripal_get_cv(array('cvterm_id' => $cvterm_id));

  // Add any key/value pairs to the $bundle_data array as desired.
}

/**
 * A hook for specifying information about the data store for vocabularies.
 *
 * The storage backend for controlled vocabularies has traditionally been
 * the Chado CV term tables. However, Tripal v3.0 introduces APIs for supporting
 * other backends.  Therefore, this function indicates to Tripal which
 * data stores are capable of providing support for terms.
 *
 * @return
 *   An array describing the storage backends implemented by the module. The
 *   keys are storage backend names. To avoid name clashes, storage
 *   backend names should be prefixed with the name of the module that
 *   exposes them. The values are arrays describing the storage backend,
 *   with the following key/value pairs:
 *
 *   label: The human-readable name of the storage backend.
 *   module:  The name of the module providing the support for this backend.
 *   description: A short description for the storage backend.
 *   settings: An array whose keys are the names of the settings available for
 *     the storage backend, and whose values are the default values for
 *     those settings.
 */
function hook_vocab_storage_info() {
  return array(
    'term_chado_storage' => array(
      'label' => t('Chado storage'),
      'description' => t('Integrates terms stored in the local Chado database with Tripal entities.'),
      'settings' => array(),
    ),
  );
}

/**
 * Creates a form for specifying a term for TripalEntity creation.
 *
 * This hook allows the module that implements a vocabulary storage backend
 * to provide the form necessary to select a term that will then be used for
 * creating a new TripalEntity type.  Tripal will expect that a 'namespace' and
 * 'term_id' are in the $form_state['storage'] array. The 'namespace' and
 * must be the abbreviated uppercase namespace for the vocabulary (e.g. 'RO',
 * 'SO', 'PATO', etc.).  The 'term_id' must be the unique term ID (or
 * accession) for the term in the vocabulary.
 *
 * @param $form
 * @param $form_state
 *
 * @return
 *   A form object.
 */
function hook_vocab_select_term_form(&$form, &$form_state) {

  return $form;
}
/**
 * Validates the hook_vocab_select_term_form().
 *
 * @param $name
 */
function hook_vocab_select_term_form_validate($form, &$form_state) {

}


