<?php
/**
 * @section
 * Vocabulary Hooks.
 */

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
 * creating a new TripalEntity type.  Tripal will expect that a 'vocabulary' and
 * 'accession' are in the $form_state['storage'] array. The 'vocabulary' and
 * must be the abbreviated uppercase vocabulary for the vocabulary (e.g. 'RO',
 * 'SO', 'PATO', etc.).  The 'accession' must be the unique term ID (or
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
/**
 * Provides a form for importing vocabularies and their terms.
 *
 * Tripal allows for vocabularies to be stored separately from the biological
 * data. This hook allows the default term storage backend to provide an
 * approprite form for importing ontologies (either in OBO or OWL format).
 *
 * @param $form
 * @param $form_state
 *
 */
function hook_vocab_import_form($form, &$form_state) {
  return $form;
}
function hook_vocab_import_form_validate($form, &$form_state) {

}
function hook_vocab_import_form_submit($form, &$form_state) {

}

/**
 * Hook used by the default term storage backend to provide details for a term.
 *
 * This hook is called by the tripal_entity module to retrieve information
 * about the term from the storage backend.  It must return an array with
 * a set of keys.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys
 *       -name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
 *     -accession : The name unique ID of the term.
 *     -url : The URL for the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 *   any other keys may be added as desired. Returns NULL if the term
 *   cannot be found.
 */
function hook_vocab_get_term($vocabulary, $accession) {
  // See the tripal_chado_vocab_get_term() function for an example.

}

/**
 * Hook used by the default term storage backend to add new terms.
 *
 * @param $details
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys
 *       -name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
 *     -accession : The name unique ID of the term.
 *     -url : The URL for the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 * @return
 *   TRUE if the term was added, FALSE otherwise.  If the term already exists
 *   it will be updated and the return value will be TRUE,
 */
function hook_vocab_add_term($details) {
  // See the tripal_chado_vocab_set_term() function for an example.
}



/**
 * Adds a term to the vocabulary storage backend.
 *
 * Use this function to add new terms dynamically to the vocabulary storage
 * backend.  If the term already exists no new term is added.
 *
 * @param $details
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys
 *       -name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
 *     -accession : The name unique ID of the term.
 *     -url : The URL for the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 * @return
 *   TRUE if the term was added, FALSE otherwise.  If the term already exists
 *   it will be updated and the return value will be TRUE,
 */
function tripal_add_term($details) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_add_term';
    if (function_exists($function)) {
      return $function($details);
    }
  }
}

/**
 * Retrieves full information about a vocabulary term.
 *
 * Vocabularies are stored in a database backend.  Tripal has no requirements
 * for how terms are stored.  By default, the tripal_chado modules provides
 * storage for vocabularies and terms. This function will call the
 * hook_vocab_get_term() function for the database backend that is housing the
 * vocabularies and allow it to return the details about the term.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array with at least the following keys:
 *     vocabulary : The short name of the vocabulary (e.g. SO, PATO, foaf).
 *     accession : The name unique ID of the term.
 *     url : The URL for the term.
 *     name : The name of the term.
 *     definition : The term's description.
 *   any other keys may be added as desired. Returns NULL if the term
 *   cannot be found.
 */
function tripal_get_term_details($vocabulary, $accession) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_term';
    if (function_exists($function)) {
      return $function($vocabulary, $accession);
    }
  }
}
