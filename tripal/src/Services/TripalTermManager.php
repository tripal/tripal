<?php
namespace Drupal\tripal\Services;

/**
 * Provides a Drupal service for easy management of Tripal Terms.
 */
class TripalTermManager {

  /**
   * A Tripal Logger to report information errors, etc.
   */
  protected $logger;

  /**
   * A single TripalTerm added/updated by the manager.
   */
  protected $term;

  /**
   * An array of TripalTerm objects to be acted upon.
   */
  protected $term_collection;

  /**
   * An array of IDs for the Terms last queried.
   */
  protected $term_ids;

  /**
   * The last queried details array.
   * Used to check if above cache can be used.
   */
  protected $details;

  /**
   * Set-up the object.
   */
  public function __construct() {
    $this->logger = \Drupal::service('tripal.logger');
  }

  /**
   * Creates a new Term.
   *
   * @param $details
   *   An array of values to set for a new Tripal Term. The following
   *   keys are supported:
   *    - vocabulary:
   *      - name: The full name of the Vocabulary (e.g. sequence).
   *      - short_name: The short name of the Vocabulary (e.g. SO).
   *      - description: A description of the Vocabulary.
   *    - accession: the accession of the term (e.g. 0001645)
   *    - name: the name of the term (e.g. genetic_marker).
   * @param $options
   *   An array of options influencing how the Term is added.
   *   The following keys are supported:
   *    - checkBeforeInsert (true|false): check that the Term doesn't
   *      exist already before trying to insert a new one. Default: true.
   *
   * @return
   *   TRUE if the Term was added, FALSE otherwise.  If the Term
   *   already exists it will be updated and the return value will be TRUE.
   */
  public function addTerm($details, $options = []) {
    $options['updateIfExists'] = FALSE;
    return $this->saveTerm($details, $options);
  }

  /**
   * Update an existing Term.
   *
   * @param $details
   *   An array of values to upddate for the Tripal Term. The following
   *   keys are supported:
   *    - vocabulary:
   *      - name: The full name of the Vocabulary (e.g. sequence).
   *      - short_name: The short name of the Vocabulary (e.g. SO).
   *      - description: A description of the Vocabulary.
   *    - accession: the accession of the term (e.g. 0001645)
   *    - name: the name of the term (e.g. genetic_marker).
   * @param $options
   *   An array of options influencing how the Term is updated.
   *   No keys are currently supported.
   *
   * @return
   *   TRUE if the Term was added, FALSE otherwise.  If the Term
   *   already exists it will be updated and the return value will be TRUE.
   */
  public function updateTerm($details, $options = []) {
    $options['updateIfExists'] = TRUE;
    $options['insertIfNotExists'] = FALSE;
    return $this->saveTerm($details, $options);
  }

  /**
   * Saves details to a TripalTerm object (either new or update).
   *
   * @param $details
   *   An array of values to set for a new Tripal Term. The following
   *   keys are supported:
   *    - vocabulary:
   *      - name: The full name of the Vocabulary (e.g. sequence).
   *      - short_name: The short name of the Vocabulary (e.g. SO).
   *      - description: A description of the Vocabulary.
   *    - accession: the accession of the term (e.g. 0001645)
   *    - name: the name of the term (e.g. genetic_marker).
   * @param $options
   *   An array of options influencing how the Term is added.
   *   The following keys are supported:
   *    - checkBeforeInsert (true|false): check that the Term doesn't
   *      exist already before trying to insert a new one. Default: true.
   *    - updateIfExists (true|false): indicates whether to update the existing
   *      term with new values. Default: true.
   *    - insertIfNotExists (true|false): if the Term doesn't already
   *      exist then insert it. Default: TRUE.
   *    - insertVocabulary (true|false):  indicates whether to insert the
   *      vocabulary if it doesn't already exist. Default: TRUE.
   *
   * @return
   *   TRUE if the Term was added, FALSE otherwise.  If the Term
   *   already exists it will be updated and the return value will be TRUE.
   */
  public function saveTerm($details, $options = []) {

    // Set Defaults.
    if (!array_key_exists('checkBeforeInsert', $options)) {
      $options['checkBeforeInsert'] = TRUE;
    }
    if (!array_key_exists('updateIfExists', $options)) {
      $options['updateIfExists'] = TRUE;
    }
    if (!array_key_exists('insertIfNotExists', $options)) {
      $options['insertIfNotExists'] = TRUE;
    }
    if (!array_key_exists('insertVocabulary', $options)) {
      $options['insertVocabulary'] = TRUE;
    }

    // Check the values we need are available.
    if (!array_key_exists('vocabulary', $details)) {
      $this->logger->error('The Term Vocabulary must be specified using the "vocabulary" key.');
      return FALSE;
    }
    if (!array_key_exists('name', $details)) {
      $this->logger->error('The full term name must be specified using the "name" key.');
      return FALSE;
    }
    if (!array_key_exists('accession', $details)) {
      $this->logger->error('The term accession must be specified using the "accession" key.');
      return FALSE;
    }
    if (!array_key_exists('definition', $details)) {
      $this->logger->error('The definition of the term must be specified using the "definition" key.');
      return FALSE;
    }

    // First, if we are supposed to, check that the Term exists.
    $exists = FALSE;
    if ($options['checkBeforeInsert']) {
      $exists = $this->checkExists($details);
      if ($exists) {
        // If it does exist and we are supposed to update it,
        // then grab the object and set it up to be updated.
        if ($options['updateIfExists'] && $exists === 1) {
          $term = $this->getTerms($details);
        }
        // Otherwise, if we're not supposed to update or more then one
        // match was detected then just say it exists.
        else {
          return TRUE;
        }
      }
      // If it doesn't exist then we need to create an object to set.
      else {
        $term = \Drupal\tripal\Entity\TripalTerm::create();
      }
    }
    // If we're not supposed to check then we need an object to set.
    else {
      $term = \Drupal\tripal\Entity\TripalTerm::create();
    }

    // If it doesn't exist and we are not supposed to insert
    // then return false.
    if (!$exists AND $options['insertIfNotExists'] === FALSE) {
      return FALSE;
    }

    // Now we need the vocabulary.
    // If they gave it to us, then use it!
    if (array_key_exists('vocab_id', $details['vocabulary'])) {
      $vocab_id = $details['vocabulary']['vocab_id'];
    }
    elseif (array_key_exists('TripalVocab', $details['vocabulary'])) {
      $vocab_id = $details['vocabulary']['TripalVocab']->id();
    }
    // Otherwise, look it up...
    else {
      $vocab = \Drupal::service('tripal.tripalVocab.manager')
        ->getVocabularies($details['vocabulary']);
      if (is_object($vocab)) {
        $vocab_id = $vocab->id();
      }
      // Or even create it.
      elseif ($options['insertVocabulary'] === TRUE) {
        $success = \Drupal::service('tripal.tripalVocab.manager')
          ->saveVocabulary($details['vocabulary']);
        if ($success) {
          $vocab = \Drupal::service('tripal.tripalVocab.manager')
            ->getVocabularies($details['vocabulary']);
          $vocab_id = $vocab->id();
        }
      }
      else {
        $this->logger->error("Unable to retrieve vocabulary then we can't save the term.");
      }
    }

    // Now we need the vocabulary.
    // If they gave it to us, then use it!
    if (array_key_exists('idspace_id', $details['vocabulary'])) {
      $idspace_id = $details['vocabulary']['idspace_id'];
    }
    elseif (array_key_exists('TripalVocabSpace', $details['vocabulary'])) {
      $idspace_id = $details['vocabulary']['TripalVocabSpace']->id();
    }
    // Otherwise, look it up...
    else {
      $idspace = \Drupal::service('tripal.tripalVocab.manager')
        ->getIDSpace($details['vocabulary']);
      if (is_object($idspace)) {
        $idspace_id = $idspace->id();
      }
      // Or even create it.
      elseif ($options['insertVocabulary'] === TRUE) {
        $success = \Drupal::service('tripal.tripalVocab.manager')
          ->saveVocabulary($details['vocabulary']);
        if ($success) {
          $idspace = \Drupal::service('tripal.tripalVocab.manager')
            ->getIDSpace($details['vocabulary']);
          $idspace_id = $idspace->id();
        }
      }
      else {
        $this->logger->error("Unable to retrieve idspace then we can't save the term.");
      }
    }

    // Now we have an object, we need to set the values.
    $term->setVocabID($vocab_id);
    $term->setIDSpaceID($idspace_id);
    $term->setAccession($details['accession']);
    $term->setName($details['name']);
    $term->setDefinition($details['definition']);

    // Finally we save our TripalTerm object:
    // -- First to the database.
    $term->save();
    // -- Second to the manager in case they want it later.
    $this->details = $details;
    $this->term_ids = [ $term->id() ];
    $this->term = $term;

    return TRUE;
  }

  /**
   * Check to see if a TripalTerm already exists.
   *
   * @param $details
   *   An array of values to set for a new Tripal Term. The following
   *   keys are supported:
   *    - vocabulary:
   *      - name: The full name of the Vocabulary (e.g. sequence).
   *      - short_name: The short name of the Vocabulary (e.g. SO).
   *      - description: A description of the Vocabulary.
   *    - accession: the accession of the term (e.g. 0001645)
   *    - name: the name of the term (e.g. genetic_marker).
   * @param $options
   *   An array of options influencing how the Term is added.
   *   The following keys are supported:
   *    - returnObject (true|false): return the TripalTerm object if it was
   *       found. Default: false.
   *
   * @return
   *   The number of matches if the Term exists, FALSE otherwise.
   *   If 'returnObject' is true then the TripalTerm or an array of
   *   TripalTerm objects are returned.
   */
  public function checkExists($details, $options = []) {

    // Set Defaults.
    if (!array_key_exists('returnObject', $options)) {
      $options['returnObject'] = FALSE;
    }

    // Check the values we need are available.
    if (!is_array($details)) {
      $this->logger->error('You must pass an array to checkExists() to find the object. Instead you passed: :var', [':var' => print_r($details, TRUE)]);
      return FALSE;
    }

    // Use the Drupal Entity Query mechanism to look for matches.
    $query = \Drupal::entityQuery('tripal_term');
    $query->accessCheck(TRUE);
    if (array_key_exists('name', $details)) {
  	   $query->condition('name', $details['name']);
    }
    if (array_key_exists('accession', $details)) {
  	   $query->condition('accession', $details['accession']);
    }
    if (array_key_exists('vocabulary', $details)) {
      if (array_key_exists('vocab_id', $details['vocabulary'])) {
  	    $query->condition('vocab_id', $details['vocabulary']['vocab_id']);
      }
      else {
        $vocab = \Drupal::service('tripal.tripalVocab.manager')
          ->getVocabularies($details['vocabulary']);
        if (is_object($vocab)) {
          $vocab_id = $vocab->id();
          $query->condition('vocab_id', $vocab_id);
        }
      }
    }
  	$term_ids = $query->execute();

    // If we have some results, then it exists!
  	if (sizeof($term_ids) > 0) {

      $this->details = $details;
      $this->term_ids = $term_ids;

      // If they want an object, then get them object(s).
      if ($options['returnObject']) {
        return $this->loadTerms($term_ids, $options);
      }
      // If they don't want objects, then just tell them the number of results.
      else {
        return sizeof($term_ids);
      }
    // If we didn't get any results then it doesn't exist...
    // Log some debugging information and return FALSE.
    }
    elseif (empty($term_ids)) {
      $this->logger->debug('No TripalTerm results were returned for :params', [':params' => print_r($details, TRUE)]);
    }
    elseif (!is_array($term_ids)) {
      $this->logger->debug('Drupal::entityQuery encountered an error when trying to retrieve TripalTerm objects based on the following ', [':params' => print_r($details, TRUE)]);
    }

    return FALSE;
  }

  /**
   * Retrieve Tripal Term(s) based on criteria.
   *
   * @param $details
   *   An array of values to set for a new Tripal Term. The following
   *   keys are supported:
   *    - vocabulary:
   *      - name: The full name of the Vocabulary (e.g. sequence).
   *      - short_name: The short name of the Vocabulary (e.g. SO).
   *      - description: A description of the Vocabulary.
   *    - accession: the accession of the term (e.g. 0001645)
   *    - name: the name of the term (e.g. genetic_marker).
   * @param $options
   *   An array of options influencing how the Term is added.
   *   The following keys are supported:
   *     - returnArray (true|false): always returns an array even if there is
   *       only one result. Default: FALSE.
   *     - useCache (true|false): allows you to control if the cache is used.
   *       Default: TRUE.
   *
   * @return
   *   If there is a single result then the TripalTerm object is returned.
   *   If there are multiple results then an array of TripalTerm objects
   *   are returned.
   */
  public function getTerms($details, $options = []) {

    // Set Default Options.
    if (!array_key_exists('returnArray', $options)) {
      $options['returnArray'] = FALSE;
    }
    if (!array_key_exists('useCache', $options)) {
      $options['useCache'] = TRUE;
    }

    // Use cache if it's set.
    $cache = FALSE;
    if ($this->details == $details) {
      if ($this->term) {
        $cache = $this->term;
      }
      elseif ($this->term_collection) {
        $cache = $this->term_collection;
      }
      if ($this->term_ids) {
        $cache = $this->loadTerms($this->term_ids, $options);
      }
    }

    // If there are cached values, then use them.
    if ($cache && $options['useCache']) {
      $result = $cache;
    }
    // Retrieve the results using checkExists().
    else {
      $result = $this->checkExists($details, ['returnObject' => TRUE]);
    }

    // If the asker requested an array, then we should give them one.
    if ($options['returnArray']) {
      if (is_object($result)) {
        return [ $result ];
      }
      else {
        return $result;
      }
    }
    // Otherwise, just give them the result.
    else {
      return $result;
    }
  }

  /**
   * Load Terms.
   *
   * @param $term_ids
   *   An array of TripalTerm ids to load.
   * @param $options
   *   An array of options influencing how the Term is added.
   *   The following keys are supported:
   *    - returnArray: Ensures an array is always returned.
   *
   * @return
   *   An array of TripalTerm objects.
   */
  public function loadTerms($term_ids, $options = []) {

    // Set Default Options.
    if (!array_key_exists('returnArray', $options)) {
      $options['returnArray'] = FALSE;
    }

    // Return a single TripalTerm object if there is only one.
    if (sizeof($term_ids)) {
      $id = array_pop($term_ids);
      $term = \Drupal\tripal\Entity\TripalTerm::load($id);
      $this->term = $term;
      if ($options['returnArray'] === TRUE) {
        return [ $term ];
      }
      else {
        return $term;
      }
    }
    // Otherwise, return an array of TripalTerm results.
    else {
      $results = [];
      foreach ($term_ids as $id) {
        $results[] = \Drupal\tripal\Entity\TripalTerm::load($id);
      }
      $this->term_collection = $results;
      return $results;
    }
  }
}
