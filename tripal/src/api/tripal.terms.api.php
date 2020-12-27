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
 *     -name: The full name of the vocabulary.
 *     -short_name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *     -description: The description of this vocabulary.
 *
 * @return
 *   TRUE if the vocabulary was added, FALSE otherwise.  If the vocabulary
 *   already exists it will be updated and the return value will be TRUE.
 *
 * @ingroup tripal_terms_api
 */
function tripal_add_vocabulary($details) {
  return \Drupal::service('tripal.tripalVocab.manager')
    ->addVocabulary($details);
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
 *   The name or short name of the vocabulary.
 *
 * @return
 *   An array with at least the following keys:
 *     - name: The full name of the vocabulary.
 *     - short_name: The short name abbreviation for the vocabulary.
 *     - description: A brief description of the vocabulary.
 *     - url:  A URL for the online resources for the vocabulary.
 *     - urlprefix: A URL to which the short_name and term
 *       accession can be appended to form a complete URL for a term.  If the
 *       prefix does not support appending then the exact location for the
 *       position of the short_name and the term accession will be
 *       specified with the {db} and {accession} tags respectively.
 *     - sw_url: The URL for mapping terms via the semantic web.
 *     - num_terms: The number of terms loaded in the vocabulary.
 *     - TripalVocab: The object describing this vocabulary.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabulary_details($name) {
  $logger = \Drupal::service('tripal.logger');
  $TripalVocabManager = \Drupal::service('tripal.tripalVocab.manager');
  $vocabulary = [];

  if (!is_string($name)) {
    $logger->error('You must pass a string to tripal_get_vocabulary_details() to return details. Instead you passed: :var', [':var' => print_r($name, TRUE)]);
    return FALSE;
  }

  // Retrieve the TripalVocab object if we wern't given it.
  $object = $TripalVocabManager->getVocabularies(['name' => $name]);
  // If the name didn't work, maybe they gave us the short name?
  if (!$object) {
    $object = $TripalVocabManager->getVocabularies(['short_name' => $name]);
  }

  // If we were unable to retrieve it, let the caller know it doesn't exist.
  if (!$object) {
    $logger->debug('We were unable to retrieve the Tripal Vocabulary with the name :var.', [':var' => $name]);
    return FALSE;
  }

  if (is_array($object)) {
    $result = [];
    foreach($object as $vocab) {
      $result[] = $vocab->getDetails();
    }
  }
  elseif (is_object($object)) {
    $result = $object->getDetails();
    return $result;
  }
  else {
    return FALSE;
  }
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
  $objects = \Drupal::service('tripal.tripalVocab.manager')
    ->getVocabularies([]);

  $results = [];
  foreach ($objects as $vocab) {
    $results[] = $vocab->getDetails();
  }
  return $results;
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
 *       -name: The full name of the vocabulary (e.g. 'sequence').
 *       -short_name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
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
  $logger = \Drupal::service('tripal.logger');

  if (!array_key_exists('vocabulary', $details)) {
    $logger->error('The vocabulary must be specified using the "vocabulary" key.');
    return FALSE;
  }
  if (!array_key_exists('accession', $details)) {
    $logger->error('The term accession must be specified using the "accession" key.');
    return FALSE;
  }
  if (!array_key_exists('name', $details)) {
    $logger->error('The name of the term must be specified using the "name" key.');
    return FALSE;
  }
  if (!array_key_exists('definition', $details)) {
    $logger->error('The definition of the term must be specified using the "definition" key.');
    return FALSE;
  }

  // First, we need the vocabulary.
  // This function will make sure it exists.
  $success = tripal_add_vocabulary($details['vocabulary']);
  if ($success) {
    // Then we retrieve the object.
    $vocab = tripal_get_TripalVocab($details['vocabulary']);
    if (!$vocab) {
      $logger->error('Unable to retrieve TripalVocab even though we just created it.');
      return FALSE;
    }

    // Now we try to retrieve the term in case it already exists.
    $term_exists = tripal_get_term_details(
      $details['vocabulary']['short_name'],
      $details['accession']
    );
    if ($term_exists) {
      $term = $term_exists['TripalTerm'];
    }
    else {
      $term = \Drupal\tripal\Entity\TripalTerm::create();
    }

    $term->setVocabID($vocab->id());
    $term->setAccession($details['accession']);
    $term->setName($details['name']);
    $term->setDefinition($details['definition']);
    $term->save();

    return TRUE;
  }
  else {
    $this->error('Unable to create or retrieve vocabulary which is required to create a term. Parameters: :param', [':param' => print_r($details, TRUE)]);
    return FALSE;
  }

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
 *       - name : The full name of the vocabulary.
 *       - short_name : The short name abbreviation for the vocabulary.
 *       - description : A brief description of the vocabulary.
 *       - url : (optional) A URL for the online resources for the vocabulary.
 *       - urlprefix : (optional) A URL to which the short_name and term
 *         accession can be appended to form a complete URL for a term.  If the
 *         prefix does not support appending then the exact location for the
 *         position of the short_name and the term accession will be
 *         specified with the {db} and {accession} tags respectively.
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
  $logger = \Drupal::service('tripal.logger');

  if (empty($vocabulary)) {
    $logger->error("Unable to retrieve details for term due to missing vocabulary name");
    return FALSE;
  }
  if (empty($accession)) {
    $logger->error("Unable to retrieve details for term due to missing accession");
    return FALSE;
  }

	$vocab = tripal_get_TripalVocab(['short_name' => $vocabulary]);
  if (!$vocab) {
    $logger->debug('Unable to retrieve vocabulary.');
    return FALSE;
  }

	$term['TripalTerm'] = tripal_get_TripalTerm([
    'accession' => $accession,
    'vocab_id' => $vocab->id()
  ]);
	if (!is_object($term['TripalTerm'])) {
		$logger->debug('Unable to find TripalTerm with :accession and :vocab',
      [':accession' => $accession, ':vocab' => $vocabulary]);
		return FALSE;
	}

	// Next retrieve term details.
	$term['accession'] = $term['TripalTerm']->getAccession();
	$term['name'] = $term['TripalTerm']->getName();
	$term['definition'] = $term['TripalTerm']->getDefinition();
	// TODO: Add URL once it is available.

	// Finally retrieve the vocabulary and vocab details.
	$term['vocabulary']['TripalVocab'] = $vocab;
	if (!is_object($term['vocabulary']['TripalVocab'])) {
		$logger->debug("Unable to retrieve details for term due to missing vocabulary.");
		return FALSE;
	}
	$term['vocabulary']['name'] = $vocab->getName();
	$term['vocabulary']['short_name'] = $vocab->getLabel();
	$term['vocabulary']['description'] = $vocab->getDescription();
	// TODO: Add URL and URL prefix once they are available.

  return $term;
}

/**
 * Return the TripalTerm Object.
 *
 * @param array $details
 *   Details which uniquely identify the term to retrieve.
 *   The following keys are supported:
 *     - vocabulary: An array containing the following keys:
 *       - name: The full name of the vocabulary.
 *       - short_name: The short name abbreviation for the vocabulary.
 *       - vocab_id: the id of the TripalVocab object.
 *     - accession: The name unique ID of the term.
 *     - name: The name of the term.
 *
 * @return
 *   The TripalTerm if found and FALSE otherwise.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_TripalTerm($details) {
  $logger = \Drupal::service('tripal.logger');

  if (!is_array($details)) {
    $logger->error('The details must be provided in an array.');
    return FALSE;
  }

  // If the vocabulary is specified we want the ID.
  $vocab_id = NULL;
  if (array_key_exists('vocabulary', $details)) {
    if (array_key_exists('vocab_id', $details['vocabulary'])) {
      $vocab_id = $details['vocabulary']['vocab_id'];
    }
    else {
      $vocab = tripal_get_TripalVocab($details['vocabulary']);
      if ($vocab) {
        $vocab_id = $vocab->id();
      }
      else {
        $logger->error('Unable to retrieve vocabulary from the information provided.');
        return FALSE;
      }
    }
  }

  // Now actually query for the term.
  $query = \Drupal::entityQuery('tripal_term');
  if (array_key_exists('name', $details)) {
	  $query->condition('name', $details['name']);
  }
  if (array_key_exists('accession', $details)) {
	  $query->condition('accession', $details['accession']);
  }
  if ($vocab_id) {
    $query->condition('vocab_id', $vocab_id);
  }
	$term_ids = $query->execute();

	if (sizeof($term_ids) === 1) {
    $id = array_pop($term_ids);
    $term = \Drupal\tripal\Entity\TripalTerm::load($id);
    return $term;
  }
  elseif (empty($term_ids)) {
    $logger->debug('No TripalTerm results were returned for :params', [':params' => print_r($details, TRUE)]);
  }
  elseif (sizeof($term_ids) > 1) {
    $logger->debug('Too many TripalTerm objects were returned for :params', [':params' => print_r($details, TRUE)]);
  }
  elseif (!is_array($term_ids)) {
    $logger->debug('Drupal::entityQuery encountered an error when trying to retrieve TripalTerm objects based on the following ', [':params' => print_r($details, TRUE)]);
  }
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
  return [];
}
