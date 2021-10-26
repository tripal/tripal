<?php
namespace Drupal\tripal\Services;

/**
 * Provides a Drupal service for easy management of Tripal Vocabularies.
 */
class TripalVocabManager {

  /**
   * A Tripal Logger to report information errors, etc.
   */
  protected $logger;

  /**
   * A single TripalVocab added/updated by the manager.
   */
  protected $vocab;

  /**
   * An array of IDs for the vocabularies last queried.
   */
  protected $vocab_ids;

  /**
   * A single TripalVocabSpace added/updated by the manager.
   */
  protected $idspace;

  /**
   * An array of IDs for the vocabularies last queried.
   */
  protected $idspace_ids;

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
   * Creates a new vocabulary.
   *
   * @param $details
   *   An array of values to set for a new Tripal Vocabulary. The following
   *   keys are supported:
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
   * @param $options
   *   An array of options influencing how the vocabulary is added.
   *   The following keys are supported:
   *    - checkBeforeInsert (true|false): check that the vocabulary doesn't
   *      exist already before trying to insert a new one. Default: true.
   *
   * @return
   *   TRUE if the vocabulary was added, FALSE otherwise.  If the vocabulary
   *   already exists it will be updated and the return value will be TRUE.
   */
  public function addVocabulary($details, $options = []) {
    $options['updateIfExists'] = FALSE;
    return $this->saveVocabulary($details, $options);
  }

  /**
   * Update an existing vocabulary.
   *
   * @param $details
   *   An array of values to upddate for the Tripal Vocabulary. The following
   *   keys are supported:
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
   * @param $options
   *   An array of options influencing how the vocabulary is updated.
   *   No keys are currently supported.
   *
   * @return
   *   TRUE if the vocabulary was added, FALSE otherwise.  If the vocabulary
   *   already exists it will be updated and the return value will be TRUE.
   */
  public function updateVocabulary($details, $options = []) {
    $options['updateIfExists'] = TRUE;
    $options['insertIfNotExists'] = FALSE;
    return $this->saveVocabulary($details, $options);
  }

  /**
   * Saves details to a TripalVocab object (either new or update).
   *
   * @param $details
   *   An array of values to set for a new Tripal Vocabulary. The following
   *   keys are supported:
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
   * @param $options
   *   An array of options influencing how the vocabulary is added.
   *   The following keys are supported:
   *    - checkBeforeInsert (true|false): check that the vocabulary doesn't
   *      exist already before trying to insert a new one. Default: true.
   *    - updateIfExists (true|false): indicates whether to update the existing
   *      term with new values. Default: true.
   *    - insertIfNotExists (true|false): if the vocabulary doesn't already
   *      exist then insert it. Default: TRUE.
   *
   * @return
   *   TRUE if the vocabulary was added, FALSE otherwise.  If the vocabulary
   *   already exists it will be updated and the return value will be TRUE.
   */
  public function saveVocabulary($details, $options = []) {

    // DEPRECATION: Short Name => IDSpace.
    if (array_key_exists('short_name', $details)) {
      if (!array_key_exists('idspace', $details)) {
        $details['idspace'] = $details['short_name'];
      }
    }

    // Check the values we need are available.
    if (!array_key_exists('name', $details)) {
      $this->logger->error('The full vocabulary name must be specified using the "name" key.');
      return FALSE;
    }
    if (!array_key_exists('idspace', $details)) {
      $this->logger->error('The ID Space (i.e. SO) must be specified using the "idspace" key.');
      return FALSE;
    }

    // Set Defaults.
    // -- Options.
    if (!array_key_exists('checkBeforeInsert', $options)) {
      $options['checkBeforeInsert'] = TRUE;
    }
    if (!array_key_exists('updateIfExists', $options)) {
      $options['updateIfExists'] = TRUE;
    }
    if (!array_key_exists('insertIfNotExists', $options)) {
      $options['insertIfNotExists'] = TRUE;
    }
    // -- Values.
    if (!array_key_exists('namespace', $details)) {
      $details['namespace'] = $details['idspace'];
    }
    if (!array_key_exists('description', $details)) {
      $details['description'] = '';
    }
    if (!array_key_exists('url', $details)) {
      $details['url'] = '';
    }
    if (!array_key_exists('urlprefix', $details)) {
      $details['urlprefix'] = '';
    }

    // First, if we are supposed to, check that the vocabulary exists.
    $exists = FALSE;
    if ($options['checkBeforeInsert']) {
      $exists = $this->checkVocabExists($details);
      if ($exists) {
        // If it does exist and we are supposed to update it,
        // then grab the object and set it up to be updated.
        if ($options['updateIfExists'] && $exists === 1) {
          $vocab = $this->getVocabularies($details);
          $idspace = $this->idspace;
        }
        // Otherwise, if we're not supposed to update or more then one
        // match was detected then just say it exists.
        else {
          return TRUE;
        }
      }
      // If it doesn't exist then we need to create an object to set.
      else {
        $vocab = \Drupal\tripal\Entity\TripalVocab::create();
        $idspace = \Drupal\tripal\Entity\TripalVocabSpace::create();
      }
    }
    // If we're not supposed to check then we need an object to set.
    else {
      $vocab = \Drupal\tripal\Entity\TripalVocab::create();
      $idspace = \Drupal\tripal\Entity\TripalVocabSpace::create();
    }

    // If it doesn't exist and we are not supposed to insert
    // then return false.
    if (!$exists AND $options['insertIfNotExists'] === FALSE) {
      return FALSE;
    }

    // Now we have an object, we need to set the values.
    $vocab->setName($details['name']);
    $vocab->setNamespace($details['namespace']);
    $vocab->setUrl($details['url']);
    $vocab->setDescription($details['description']);

    // We save our TripalVocab object:
    // -- First to the database.
    $vocab->save();
    // -- Second to the manager in case they want it later.
    $this->details = $details;
    $this->vocab_ids = [ $vocab->id() ];
    $this->vocab = $vocab;

    // Now onto the IDSpace.
    $idspace->setIDSpace($details['idspace']);
    $idspace->setVocabID($vocab->id());
    $idspace->setURLPrefix($details['urlprefix']);

    // We save our TripalVocabSpace object:
    // -- First to the database.
    $idspace->save();
    // -- Second to the manager in case they want it later.
    $this->idspace_ids = [ $idspace->id() ];
    $this->idspace = $idspace;

    return TRUE;
  }

  /**
   * Check to see if a TripalVocab already exists.
   *
   * @param $details
   *   An array of values to search for the Tripal Vocabulary (Space).
   *   The following keys are supported:
   *    - name: The full name of the vocabulary (e.g. The Sequence Ontology).
   *    - namespace: The namespace of the vocabulary (e.g. sequence).
   *    - idspace: The ID space of the vocabulary (e.g. SO). If there is more
   *         then one IDSpace, use addIDSpace() to add additional ones.
   *    - short_name: The short name of the vocabulary (e.g. SO).
   *         DEPRECATED: replaced by idspace.
   * @param $options
   *   An array of options influencing how the vocabulary is added.
   *   The following keys are supported:
   *    - returnObject (true|false): return the TripalVocab object if it was
   *       found. Default: false.
   *
   * @return
   *   The number of matches if the vocabulary exists, FALSE otherwise.
   *   If 'returnObject' is true then the TripalVocab or an array of
   *   TripalVocab objects are returned.
   */
  public function checkVocabExists($details, $options = []) {

    // Set Defaults.
    if (!array_key_exists('returnObject', $options)) {
      $options['returnObject'] = FALSE;
    }

    // Check the values we need are available.
    if (!is_array($details)) {
      $this->logger->error('You must pass an array to checkVocabExists() to find the object. Instead you passed: :var', [':var' => print_r($details, TRUE)]);
      return FALSE;
    }

    // DEPRECATION: Short Name => IDSpace.
    if (array_key_exists('short_name', $details)) {
      if (!array_key_exists('idspace', $details)) {
        $details['idspace'] = $details['short_name'];
      }
    }

    // TripalVocabSpace.
    // Use the Drupal Entity Query mechanism to look for matches.
    // We will use the IDSpace to grab it's default vocabulary.
    $default_vocab = NULL;
    if (array_key_exists('idspace', $details)) {
      $query = \Drupal::entityQuery('tripal_vocab_space');
      $query->accessCheck(TRUE);
      $query->condition('IDSpace', $details['idspace']);
      $idspace_ids = $query->execute();
      if (sizeof($idspace_ids) === 1) {
        $idspace = $this->loadVocabIDSpaces($idspace_ids);
        $default_vocab = $idspace->getVocabID();
        $this->idspace = $idspace;
        $this->idspace_ids = $idspace_ids;
      }
    }

    // TripalVocab
    // Use the Drupal Entity Query mechanism to look for matches.
    $query = \Drupal::entityQuery('tripal_vocab');
    $query->accessCheck(TRUE);
    if (array_key_exists('name', $details)) {
  	   $query->condition('name', $details['name']);
    }
    if (array_key_exists('namespace', $details)) {
  	   $query->condition('namespace', $details['namespace']);
    }
  	$vocab_ids = $query->execute();
    if ($default_vocab AND !in_array($default_vocab, $vocab_ids)) {
      $vocab_ids[] = $default_vocab;
    }

    // If we have some results, then it exists!
  	if (sizeof($vocab_ids) > 0) {

      $this->details = $details;
      $this->vocab_ids = $vocab_ids;

      // If they want an object, then get them object(s).
      if ($options['returnObject']) {
        return $this->loadVocabularies($vocab_ids, $options);
      }
      // If they don't want objects, then just tell them the number of results.
      else {
        return sizeof($vocab_ids);
      }
    // If we didn't get any results then it doesn't exist...
    // Log some debugging information and return FALSE.
    }
    elseif (empty($vocab_ids)) {
      $this->logger->debug('No TripalVocab results were returned for :params', [':params' => print_r($details, TRUE)]);
    }
    elseif (!is_array($vocab_ids)) {
      $this->logger->debug('Drupal::entityQuery encountered an error when trying to retrieve TripalVocab objects based on the following ', [':params' => print_r($details, TRUE)]);
    }

    return FALSE;
  }

  /**
   * Retrieve Tripal Vocab(s) based on criteria.
   *
   * @param $details
   *   An array of values to set for a new Tripal Vocabulary. The following
   *   keys are supported:
   *    - name: The full name of the vocabulary (e.g. The Sequence Ontology).
   *    - namespace: The namespace of the vocabulary (e.g. sequence).
   *    - idspace: The ID space of the vocabulary (e.g. SO).
   *    - short_name: The short name of the vocabulary (e.g. SO).
   *         DEPRECATED: replaced by idspace.
   * @param $options
   *   An array of options influencing how the vocabulary is added.
   *   The following keys are supported:
   *     - returnArray (true|false): always returns an array even if there is
   *       only one result. Default: FALSE.
   *     - useCache (true|false): allows you to control if the cache is used.
   *       Default: TRUE.
   *
   * @return
   *   If there is a single result then the TripalVocab object is returned.
   *   If there are multiple results then an array of TripalVocab objects
   *   are returned.
   */
  public function getVocabularies($details, $options = []) {

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
      if ($this->vocab) {
        $cache = $this->vocab;
      }
      if ($this->vocab_ids) {
        $cache = $this->loadVocabularies($this->vocab_ids, $options);
      }
    }

    // If there are cached values, then use them.
    if ($cache && $options['useCache']) {
      $result = $cache;
    }
    // Retrieve the results using checkVocabExists().
    else {
      $result = $this->checkVocabExists($details, ['returnObject' => TRUE]);
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
   * Load Vocabularies.
   *
   * @param $vocab_ids
   *   An array of TripalVocab ids to load.
   * @param $options
   *   An array of options influencing how the vocabulary is added.
   *   The following keys are supported:
   *    - returnArray: Ensures an array is always returned.
   *
   * @return
   *   An array of TripalVocab objects.
   */
  public function loadVocabularies($vocab_ids, $options = []) {

    // Set Default Options.
    if (!array_key_exists('returnArray', $options)) {
      $options['returnArray'] = FALSE;
    }

    // Return a single TripalVocab object if there is only one.
    if (sizeof($vocab_ids)) {
      $id = array_pop($vocab_ids);
      $vocab = \Drupal\tripal\Entity\TripalVocab::load($id);
      $this->vocab = $vocab;
      if ($options['returnArray'] === TRUE) {
        return [ $vocab ];
      }
      else {
        return $vocab;
      }
    }
    // Otherwise, return an array of TripalVocab results.
    else {
      $results = [];
      foreach ($vocab_ids as $id) {
        $results[] = \Drupal\tripal\Entity\TripalVocab::load($id);
      }
      return $results;
    }
  }

  /**
   * Retrieve Tripal IDSpace(s) based on criteria.
   *
   * @param $details
   *   An array of values; The following keys are supported:
   *    - idspace: The ID space of the vocabulary (e.g. SO).
   *    - short_name: The short name of the vocabulary (e.g. SO).
   *         DEPRECATED: replaced by idspace.
   * @param $options
   *   An array of options. The following keys are supported:
   *     - returnArray (true|false): always returns an array even if there is
   *       only one result. Default: FALSE.
   *     - useCache (true|false): allows you to control if the cache is used.
   *       Default: TRUE.
   *
   * @return
   *   If there is a single result then the TripalVocabSpace object is returned.
   *   If there are multiple results then an array of TripalVocabSpace objects
   *   are returned.
   */
  public function getIDSpace($details, $options = []) {

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
      if ($this->idspace) {
        $cache = $this->idspace;
      }
      if ($this->idspace_ids) {
        $cache = $this->loadVocabIDSpaces($this->idspace_ids, $options);
      }
    }

    // If there are cached values, then use them.
    if ($cache && $options['useCache']) {
      $result = $cache;
    }
    // Retrieve the results using checkVocabExists().
    else {
      $this->checkVocabExists($details, ['returnObject' => TRUE]);
      $result = $this->idspace;
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
   * Load IDSpaces.
   *
   * @param $idspace_ids
   *   An array of TripalVocabSpace ids to load.
   * @param $options
   *   An array of options... The following keys are supported:
   *    - returnArray: Ensures an array is always returned.
   *
   * @return
   *   An array of TripalVocabSpace objects.
   */
  public function loadVocabIDSpaces($idspace_ids, $options = []) {

    // Set Default Options.
    if (!array_key_exists('returnArray', $options)) {
      $options['returnArray'] = FALSE;
    }

    // Return a single TripalVocabSpace object if there is only one.
    if (sizeof($idspace_ids)) {
      $id = array_pop($idspace_ids);
      $idspace = \Drupal\tripal\Entity\TripalVocabSpace::load($id);
      $this->idspace = $idspace;
      if ($options['returnArray'] === TRUE) {
        return [ $idspace ];
      }
      else {
        return $idspace;
      }
    }
    // Otherwise, return an array of TripalVocab results.
    else {
      $results = [];
      foreach ($idspace_ids as $id) {
        $results[] = \Drupal\tripal\Entity\TripalVocabSpace::load($id);
      }
      return $results;
    }
  }
}
