<?php

/**
 * @file
 * Provides an application programming interface (API) for working with
 * Tripal controlled vocabularies and their terms.
 */

/**
 * @defgroup tripal_terms_api CV Terms
 * @ingroup tripal_api
 * @{
 * Tripal provides an application programming interface (API) for working with
 * controlled vocabulary terms.  Tripal v4 is highly dependent on controlled
 * vocabularies for identifying all content types and fields attached to those
 * content types. Furthermore, Tripal4 terms are completely database agnositic
 * with basic term details stored in Drupal. Look at the TripalVocab and
 * TripalTerm classes for information on how to integrate these terms with
 * any database backend.
 * @}
 */

/**
 * Add a Tripal Vocabulary.
 *
 * Use this function to add new vocabularies programmaticly.
 * If the vocabulary already exists no new vocabulary is added.
 *
 * @param $details
 *   An array with at least the following keys:
 *    - name: The full name of the vocabulary (e.g. The Sequence Ontology).
 *    - namespace: The namespace of the vocabulary (e.g. sequence).
 *    - idspace: The ID space of the vocabulary (e.g. SO). If there is more
 *         then one IDSpace, use addIDSpace() to add additional ones.
 *    - short_name: The short name of the vocabulary (e.g. SO).
 *         DEPRECATED: replaced by idspace.
 *    - description: A description of the vocabulary.
 *    - url: the URL containing a reference for this vocabulary.
 *    - urlprefix: The URL with tokens referencing a specific term in the
 *         given idspace.
 *
 * @return
 *   TRUE if the vocabulary was added, FALSE otherwise.  If the vocabulary
 *   already exists it will be updated and the return value will be TRUE.
 *
 * @ingroup tripal_terms_api
 */
function tripal_add_vocabulary($details) {
  //return \Drupal::service('tripal.tripalVocab.manager')
  ///  ->addVocabulary($details);
  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return FALSE;
}

/**
 * Retrieves full information about a vocabulary.
 *
 * Vocabularies are stored in a database backend.  Tripal has no requirements
 * for how terms are stored.  By default, the tripal_chado modules provides
 * storage for vocabularies and terms. This function will call the
 * hook_vocab_get_term() function for the database backend that is housing the
 * vocabularies and allow it to return the details about the term.
 *
 * @param $name
 *   The name, namespace or idspace of the vocabulary.
 *
 * @return
 *   An array with at least the following keys:
 *     - name: The full name of the vocabulary (e.g. The Sequence Ontology).
 *     - namespace: The namespace of the vocabulary (e.g. sequence).
 *     - idspace: The ID space of the vocabulary (e.g. SO). If there is more
 *         then one IDSpace, use addIDSpace() to add additional ones.
 *     - description: A description of the vocabulary.
 *     - url: the URL containing a reference for this vocabulary.
 *     - urlprefix: The URL with tokens referencing a specific term in the
 *         given idspace.
 *     - sw_url: The URL for mapping terms via the semantic web.
 *     - num_terms: The number of terms loaded in the vocabulary.
 *     - TripalVocab: The object describing this vocabulary.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabulary_details($name) {

  // $logger = \Drupal::service('tripal.logger');
  // $TripalVocabManager = \Drupal::service('tripal.tripalVocab.manager');
  // $vocabulary = [];
  //
  // if (!is_string($name)) {
  //   $logger->error('You must pass a string to tripal_get_vocabulary_details() to return details. Instead you passed: :var', [':var' => print_r($name, TRUE)]);
  //   return FALSE;
  // }
  //
  // // Retrieve the TripalVocab object if we wern't given it.
  // $object = $TripalVocabManager->getVocabularies(['name' => $name]);
  // // If the name didn't work, maybe they gave us the short name?
  // if (!$object) {
  //   $object = $TripalVocabManager->getVocabularies(['short_name' => $name]);
  // }
  //
  // // If we were unable to retrieve it, let the caller know it doesn't exist.
  // if (!$object) {
  //   $logger->debug('We were unable to retrieve the Tripal Vocabulary with the name :var.', [':var' => $name]);
  //   return FALSE;
  // }
  //
  // if (is_array($object)) {
  //   $result = [];
  //   foreach($object as $vocab) {
  //     $result[] = $vocab->getDetails();
  //   }
  // }
  // elseif (is_object($object)) {
  //   $result = $object->getDetails();
  //   return $result;
  // }
  // else {
  //   return FALSE;
  // }

  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return FALSE;
}

/**
 * Retrieves the list of vocabularies that are available on the site.
 *
 * @return
 *   An array of vocabularies where each entry in the array is compatible
 *   with the array returned by the tripal_get_vocabulary_details()
 *   function.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabularies() {
  // $objects = \Drupal::service('tripal.tripalVocab.manager')
  //   ->getVocabularies([]);
  //
  // $results = [];
  // foreach ($objects as $vocab) {
  //   $results[] = $vocab->getDetails();
  // }
  // return $results;

  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return [];
}

/**
 * Adds a Tripal Term.
 *
 * Use this function to add new terms programmaticly.
 * If the term already exists no new term is added.
 *
 * @param $details
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys
 *       - name: The full name of the vocabulary (e.g. The Sequence Ontology).
 *       - namespace: The namespace of the vocabulary (e.g. sequence).
 *       - idspace: The ID space of the vocabulary (e.g. SO). If there is more
 *         then one IDSpace, use addIDSpace() to add additional ones.
 *       - short_name: The short name of the vocabulary (e.g. SO).
 *         DEPRECATED: replaced by idspace.
 *       - description: A description of the vocabulary.
 *       - url: the URL containing a reference for this vocabulary.
 *       - urlprefix: The URL with tokens referencing a specific term in the
 *         given idspace.
 *     -accession : The name unique ID of the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 *
 * @return
 *   TRUE if the term was added, FALSE otherwise.  If the term already exists
 *   it will be updated and the return value will be TRUE.
 *
 * @ingroup tripal_terms_api
 */
function tripal_add_term($details) {
  //return \Drupal::service('tripal.tripalTerm.manager')->addTerm($details);

  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return FALSE;
}

/**
 * Retrieves full information about a vocabulary term.
 *
 * @param $vocabulary
 *   The short name of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array with at least the following keys:
 *     - vocabulary : An array containing the following keys:
 *       - name: The full name of the vocabulary (e.g. The Sequence Ontology).
 *       - namespace: The namespace of the vocabulary (e.g. sequence).
 *       - idspace: The ID space of the vocabulary (e.g. SO). If there is more
 *         then one IDSpace, use addIDSpace() to add additional ones.
 *       - short_name: The short name of the vocabulary (e.g. SO).
 *         DEPRECATED: replaced by idspace.
 *       - description: A description of the vocabulary.
 *       - url: the URL containing a reference for this vocabulary.
 *       - urlprefix: The URL with tokens referencing a specific term in the
 *         given idspace.
 *       - TripalVocab: the Tripal vocabulary object.
 *     - accession : The name unique ID of the term.
 *     - url : The URL for the term.
 *     - name : The name of the term.
 *     - definition : The term's description.
 *     - TripalTerm: the Tripal term object.
 *   Returns NULL if the term cannot be found.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_details($vocabulary, $accession) {

  // // Set the details array as expected by the new API.
  // $details = [
  //   'vocabulary' => tripal_get_vocabulary_details($vocabulary),
  //   'accession' => $accession,
  // ];
  // if (array_key_exists('TripalVocab', $details['vocabulary'])) {
  //   $details['vocabulary']['vocab_id'] = $details['vocabulary']['TripalVocab']
  //     ->id();
  // }
  //
  // $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms($details);
  // if (is_array($term)) {
  //   if (sizeof($term) === 1) {
  //     $term = array_pop($term);
  //     return $term->getDetails();
  //   }
  //   else {
  //     return NULL;
  //   }
  // }
  // elseif (is_object($term)) {
  //   return $term->getDetails();
  // }
  // else {
  //   return NULL;
  // }

  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return NULL;
}

/**
 * Retrieves the base terms of the given vocabulary.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   Returns an array of terms where each term is compatible with the
 *   array returned by the tripal_get_term_details() function.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabulary_root_terms($vocabulary) {

  // @UPGRADE
  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return [];
}

/**
 * Retrieves the immediate children of the given term.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   Returns an array of terms where each term is compatible with the
 *   array returned by the tripal_get_term_details() function.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_children($vocabulary, $accession) {

  // Tripal Terms do not have a hierarchy.
  $logger = \Drupal::service('tripal.logger');
  $logger->error('Not Implemented.');
  return [];
}
