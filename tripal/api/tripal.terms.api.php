<?php

use \Drupal\tripal\Entity\TripalTerm;

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

  if (empty($vocabulary) OR empty($accession)) {
		// TODO: Uncomment once tripal_report_error is implemented
    // tripal_report_error('tripal_term', TRIPAL_ERROR, "Unable to retrieve details for term due to missing vocabulary and/or accession");
    return FALSE;
  }

	$query = \Drupal::entityQuery('tripal_vocab');
	$query->condition('vocabulary', $vocabulary);
	$vocab_ids = $query->execute();
	if (!is_array($vocab_ids) OR empty($vocab_ids)) {
		return NULL;
	}

	$query = \Drupal::entityQuery('tripal_term');
	$query->condition('accession', $accession);
	$query->condition('vocab_id', $vocab_ids);
	$term_ids = $query->execute();

  if (is_array($term_ids) and count($term_ids) > 0) {
		foreach ($term_ids as $term_id) {

			// First retrieve the term.
			$term = [];
			$term['TripalTerm'] = TripalTerm::load($term_id);
			if (!is_object($term['TripalTerm'])) {
				// TODO: Uncomment once tripal_report_error is implemented
		    // tripal_report_error('tripal_term', TRIPAL_ERROR, "Unable to retrieve details for term due to missing term.");
				return FALSE;
			}

			// Next retrieve term details.
			$term['accession'] = $term['TripalTerm']->getAccession();
			$term['name'] = $term['TripalTerm']->getName();
			$term['definition'] = $term['TripalTerm']->getDefinition();
			// TODO: Add URL once it is available.

			// Finally retrieve the vocabulary and vocab details.
			$term['vocabulary'] = [];
			$term['vocabulary']['TripalVocab'] = $term['TripalTerm']->getVocab();
			if (!is_object($term['vocabulary']['TripalVocab'])) {
				// TODO: Uncomment once tripal_report_error is implemented
		    // tripal_report_error('tripal_term', TRIPAL_ERROR, "Unable to retrieve details for term due to missing vocabulary.");
				return FALSE;
			}
			$term['vocabulary']['name'] = $term['vocabulary']['TripalVocab']->getName();
			$term['vocabulary']['short_name'] = $term['vocabulary']['TripalVocab']->getLabel();
			$term['vocabulary']['description'] = $term['vocabulary']['TripalVocab']->getDescription();
			// TODO: Add URL and URL prefix once they are available.

      return $term;
    }
  }
	else {
		return NULL;
	}
}
