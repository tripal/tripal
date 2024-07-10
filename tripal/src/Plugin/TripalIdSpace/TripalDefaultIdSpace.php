<?php

namespace Drupal\tripal\Plugin\TripalIdSpace;

use Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;

/**
 * Default Implementation of TripalIdSpaceBase
 *
 *  @TripalIdSpace(
 *    id = "tripal_default_id_space",
 *    label = @Translation("Deafult Tripal IdSpace"),
 *  )
 */
class TripalDefaultIdSpace extends TripalIdSpaceBase {
  /**
   * A simple boolean to prevent queries if the ID space isn't valid.
   *
   * @var bool
   */
  protected $is_valid = FALSE;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    // Instantiate the TripalLogger
    $this->messageLogger = \Drupal::service('tripal.logger');

  }

  /**
   * {@inheritDoc}
   */
  public function getParent($child) {
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }

    $this->messageLogger->warning('The TripalDefaultIdSpace::getParent() function is currently not implemented');
  }

  /**
   * {@inheritDoc}
   */
  public function setURLPrefix($prefix) {
    // Don't set a value for an ID space that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // Make sure the description is not too long.
    if (empty($prefix)) {
      $this->messageLogger->error('TripalDefaultIdSpace: You must provide a urlprefix when calling setURLPrefix().');
      return False;
    }
    if (strlen($prefix) > 255) {
      $this->messageLogger->error('TripalDefaultIdSpace: The urlprefix for the vocabulary ID space must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => 255, '@value' => $prefix]);
      return False;
    }

    // Update the record in the Chado `db` table.
    $conn = \Drupal::service('database');
    $query = $conn->update('tripal_terms_idspaces')
      ->fields(['urlprefix' => $prefix])
      ->condition('name', $this->getName(), '=');
    $num_updated = $query->execute();
    if ($num_updated != 1) {
      $this->messageLogger->error('TripalDefaultIdSpace: The urlprefix could not be updated for the vocabulary ID Space.');
      return False;
    }
    return True;
  }

  /**
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($vocab) {
    $retval = parent::setDefaultVocabulary($vocab);

    // Don't set a value for an ID space that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // Make sure the description is not too long.
    if (empty($vocab)) {
      $this->messageLogger->error('TripalDefaultIdSpace: You must provide a default vocabulary when calling setDefaultVocabulary().');
      return False;
    }
    if (strlen($vocab) > 255) {
      $this->messageLogger->error('TripalDefaultIdSpace: The default vocabulary must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => 255, '@value' => $vocab]);
      return False;
    }

    // Update the record in the Chado `db` table.
    $conn = \Drupal::service('database');
    $update = $conn->update('tripal_terms_idspaces');
    $update = $update->fields(['default_vocab' => $vocab]);
    $update = $update->condition('name', $this->getName(), '=');
    $num_updated = $update->execute();
    if ($num_updated != 1) {
      $this->messageLogger->error('TripalDefaultIdSpace: The default vocabulary could not be updated for the vocabulary ID Space.');
      return False;
    }
    return True;
  }

  /**
   * {@inheritDoc}
   */
  public function recordExists() {
    $db = $this->loadIdSpace();
    if ($db and $db['name'] == $this->getName()) {
      return True;
    }
    return False;
  }

  /**
   * {@inheritDoc}
   */
  public function getTerm($accession, $options = [ ]) {


    if (!$this->is_valid) {
      return NULL;
    }

    $conn = \Drupal::service('database');
    $query = $conn->select('tripal_terms', 'tt');
    $query = $query->fields('tt');
    $query = $query->condition('id_space', $this->getName(), '=');
    $query = $query->condition('accession', $accession, '=');
    $result = $query->execute();
    if (!$result) {
      return NULL;
    }
    $cvterm =  $result->fetchObject();
    if (!$cvterm) {
      return NULL;
    }

    $term =  new TripalTerm([
      'name' => $cvterm->name,
      'definition' => $cvterm->definition,
      'accession' => $accession,
      'idSpace' => $this->getName(),
      'vocabulary' => $cvterm->vocabulary ? $cvterm->vocabulary : $this->getDefaultVocabulary(),
      'is_obsolete' => $cvterm->is_obsolete == 1 ? True : False,
      'is_relationship_type' => $cvterm->is_relationship_type == 1 ? True : False,
    ]);

    // We need an IdSpace manager object to look up synonym an property types.
    /** @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idsmanager */
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    // Add in the synonyms
    $synonyms = unserialize($cvterm->synonyms);
    foreach ($synonyms as $tuple) {
      $synonym_name = $tuple[0];
      $synonym_type = $tuple[1];
      $syn_type_term = NULL;
      if ($synonym_type) {
        list($syn_idspace_name, $syn_accession) = explode(':', $synonym_type);
        /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $syn_idspace */
        $syn_type_idspace = $idsmanager->loadCollection($syn_idspace_name);
        $syn_type_term = $syn_type_idspace->getTerm($syn_accession);
      }
      $term->addSynonym($synonym_name, $syn_type_term);
    }

    // Add in the property.
    $properties = unserialize($cvterm->properties);
    foreach ($properties as $tuple) {
      list($prop_term_id, $rank, $prop_value) = $tuple;
      list($prop_idspace_name, $prop_accession) = explode(':', $prop_term_id);
      /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $syn_idspace */
      $prop_type_idspace = $idsmanager->loadCollection($prop_idspace_name);
      $prop_type_term = $prop_type_idspace->getTerm($prop_accession);
      $term->addProperty($prop_type_term, $prop_value, $rank);
    }

    $altIds = unserialize($cvterm->altIds);
    foreach ($altIds as $altId) {
      list($altId_idspace_name, $altId_accession) = explode(':', $altId);
      $term->addAltId($altId_idspace_name, $altId_accession);
    }

    $parents = unserialize($cvterm->parents);
    foreach ($parents as $tuple) {
      list($parent_term_id, $rel_type_id) = $tuple;
      list($parent_idspace_name, $parent_accession) = explode(':', $parent_term_id);
      list($rel_idspace_name, $rel_accession) = explode(':', $rel_type_id);
      $parent_idspace = $idsmanager->loadCollection($parent_idspace_name);
      $parent_term = $parent_idspace->getTerm($parent_accession);
      $rel_idspace = $idsmanager->loadCollection($rel_idspace_name);
      $rel_term = $rel_idspace->getTerm($rel_accession);
      $term->addParent($parent_term, $rel_term);
    }

    // Set the internal ID.
    $term->setInternalId($cvterm->term_id);

    return $term;

  }

  /**
   * {@inheritDoc}
   */
  public function isValid() {

    // Make sure the name of this ID Space does not exceeed the allowed size in Chado.
    $name = $this->getName();

    if (!empty($name) AND (strlen($name) > 255)) {
      $this->messageLogger->error('TripalDefaultIdSpace: The IdSpace name must not be longer than @size characters. ' +
        'The value provided was: @value', ['@size' => 255, '@value' => $name]);
      $this->is_valid = FALSE;
      return FALSE;
    }

    $this->is_valid = TRUE;
    return TRUE;
  }

  /**
   * {@inheritDoc}
   */
  public function destroy() {
    $this->messageLogger->warning('The TripalDefaultIdSpace::destory() function is currently not implemented');
  }

  /**
   * {@inheritDoc}
   */
  public function removeTerm($accession) {
    $this->messageLogger->warning('The TripalDefaultIdSpace::removeTerm() function is currently not implemented');
  }

  /**
   * {@inheritDoc}
   */
  public function setDescription($description) {

    // Don't set a value for an ID space that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // Make sure the description is not too long.
    if (empty($description)) {
      $this->messageLogger->error('TripalDefaultIdSpace: You must provide a description when calling setDescription().');
      return False;
    }
    if (strlen($description) > 255) {
      $this->messageLogger->error('TripalDefaultIdSpace: The description for the vocabulary ID space must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => 255, '@value' => $description]);
        return False;
    }

    // Update the record in the Chado `db` table.
    $conn = \Drupal::service('database');
    $query = $conn->update('tripal_terms_idspaces')
      ->fields(['description' => $description])
      ->condition('name', $this->getName(), '=');
    $num_updated = $query->execute();
    if ($num_updated != 1) {
      $this->messageLogger->error('TripalDefaultIdSpace: The description could not be updated for the vocabulary ID Space.');
      return False;
    }
    return True;
  }

  /**
   * {@inheritDoc}
   */
  public function getURLPrefix() {
    $db = $this->loadIdSpace();
    return $db['urlprefix'];
  }

  /**
   * {@inheritDoc}
   */
  public function getDescription() {
    $db = $this->loadIdSpace();
    return $db['description'];
  }

  /**
   * {@inheritDoc}
   */
   public function getChildren($parent = NULL) {

     /** @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idsmanager */
     $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');

     // Don't get values for an ID space that isn't valid.
     if (!$this->is_valid) {
       return NULL;
     }

     $children = [];
     $conn = \Drupal::service('database');
     $query = $conn->select('tripal_terms', 'tt');
     $query = $query->fields('tt', ['accession', 'parents']);
     $query = $query->condition('tt.id_space', $this->getName(), '=');
     $query->condition('tt.parents', '%"' . $parent->getTermId() . '"%', 'LIKE');
     $results = $query->execute();
     while ($cvterm = $results->fetchObject()) {
       /** @var \Drupal\tripal\TripalVocabTerms\TripalTerm $term */
       $term = $this->getTerm($cvterm->accession);
       $parents = unserialize($cvterm->parents);
       foreach ($parents as $tuple) {
         if ($tuple[0] == $parent->getTermId()) {
           $rel_type_id = $tuple[1];
           list($rel_idspace_name, $rel_accession) = explode(':', $rel_type_id);
           /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $syn_idspace */
           $rel_idspace = $idsmanager->loadCollection($rel_idspace_name);
           $rel_term = $rel_idspace->getTerm($rel_accession);
           $children[] = [$term, $rel_term];
         }
       }
     }

     return $children;
   }


  /**
   * Loads an ID Space record
   *
   * @return array
   *   An associative array containing the columns of the `db1 table
   *   of Chado or NULL if the db could not be found.
   */
  protected function loadIdSpace() {

    $conn = \Drupal::service('database');

    // Get the Chado `db` record for this ID space.
    $query = $conn->select('tripal_terms_idspaces', 'ids')
      ->condition('ids.name', $this->getName(), '=')
      ->fields('ids', ['name', 'urlprefix', 'description', 'default_vocab']);
    $result = $query->execute();
    if (!$result) {
      return NULL;
    }
    return $result->fetchAssoc();

  }

  /**
   * {@inheritDoc}
   */
  public function createRecord() {
    $conn = \Drupal::service('database');

    // Check if the record already exists in the database, if it
    // doesn't then insert it.  We don't yet have the description,
    // URL prefix, etc but that's okay, the name is all that is
    // required to create a record in the `db` table.
    $db = $this->loadIdSpace();
    if (!$db) {
      $query = $conn->insert('tripal_terms_idspaces');
      $query->fields(['name' => $this->getName()]);
      $query->execute();
    }
  }

  /**
   * {@inheritDoc}
   */
  public function getDefaultVocabulary() {
    $db = $this->loadIdSpace();
    return $db['default_vocab'];
  }


  /**
   * {@inheritDoc}
   */
  public function getTerms($name, $options = [ ]) {
    // The list of terms to return
    $terms = [];

    $conn = \Drupal::service('database');
    $query1 = $conn->select('tripal_terms', 'tt');
    $query1 = $query1->fields('tt', ['accession']);
    $query1 = $query1->condition('tt.id_space', $this->getName(), '=');
    if (array_key_exists('exact', $options) and $options['exact'] === TRUE) {
      $query1->condition('tt.name', $name, '=');
    }
    else {
      $query1->condition('tt.name', $name . '%', 'LIKE');
    }
    $results = $query1->execute();
    while ($cvterm = $results->fetchObject()) {
      /** @var \Drupal\tripal\TripalVocabTerms\TripalTerm $term */
      $term = $this->getTerm($cvterm->accession);
      $terms[$term->getName()][$term->getTermId()] = $term;
    }


    // Now add in any synonyms
    $query2 = $conn->select('tripal_terms', 'tt');
    $query2 = $query2->fields('tt', ['accession', 'synonyms']);
    $query2 = $query2->condition('tt.id_space', $this->getName(), '=');
    if (array_key_exists('exact', $options) and $options['exact'] === TRUE) {
      $query2->condition('tt.synonyms', '%"' . $name . '"%', 'LIKE');
    }
    else {
      $query2->condition('tt.synonyms', '%"' . $name . '%', 'LIKE');
    }
    $results = $query2->execute();
    while ($cvterm = $results->fetchObject()) {
      /** @var \Drupal\tripal\TripalVocabTerms\TripalTerm $term */
      $term = $this->getTerm($cvterm->accession);
      $synonyms = unserialize($cvterm->synonyms);
      foreach ($synonyms as $tuple) {
        if (array_key_exists('exact', $options) and $options['exact'] === TRUE) {
          if ($tuple[0] == $name) {
            $terms[$tuple[0]][$term->getTermId()] = $term;
          }
        }
        else {
          if (preg_match("/^$name/", $tuple[0])) {
            $terms[$tuple[0]][$term->getTermId()] = $term;
          }
        }
      }
    }

    return $terms;
  }

  /**
   * Retrieve a term record from tripal_terms table.
   *
   * This function uses the IdSpace, vocabulary,
   * and accession values to uniquely identify a term.
   *
   * @param TripalTerm $term
   *   The TripalTerm object to save.
   * @return object
   *   The cvterm record in object form.
   */
  protected function findTermRecord(TripalTerm $term) {

    $conn = \Drupal::service('database');
    $query = $conn->select('tripal_terms', 'tt');
    $query = $query->fields('tt');
    $query = $query->condition('tt.id_space', $term->getIdSpace(), '=');
    $query = $query->condition('tt.vocabulary', $term->getVocabulary(), '=');
    $query = $query->condition('tt.accession', $term->getAccession(), '=');
    $result = $query->execute();
    if (!$result) {
      return NULL;
    }
    return $result->fetchObject();
  }

  /**
   * {@inheritDoc}
   */
  public function saveTerm($term, array $options = [ ]) {
    // Don't save terms that aren't valid
    if (!$term->isValid()) {
      $this->messageLogger->error(t('TripalDefaultIdSpace::saveTerm(). The term, "@term" is not valid and cannot be saved. It must include a name, accession, IdSpace and vocabulary.',
          ['@term' => $term->getIdSpace() . ':' . $term->getAccession()]));
      return False;
    }

    // Make sure the idSpace matches.
    if ($this->getName() != $term->getIdSpace()) {
      $this->messageLogger->error(t('TripalDefaultIdSpace::saveTerm(). The term, "@term", does not have the same ID space as this one.',
          ['@term' => $term->getIdSpace() . ':' . $term->getAccession()]));
      return False;
    }

    // Get easy to use boolean variables.
    $fail_if_exists = False;
    if (array_key_exists('failIfExists', $options)) {
      $fail_if_exists = $options['failIfExists'];
    }

    // Does the term exist? If not do an insert, if so, do an update.
    $cvterm = $this->findTermRecord($term);
    if (!$cvterm) {
      if (!$this->insertTerm($term, $options)) {
        return False;
      }
    }
    if ($cvterm and $fail_if_exists) {
      return False;
    }
    if ($cvterm and !$fail_if_exists) {
      if (!$this->updateTerm($term, $cvterm, $options)) {
        return False;
      }
    }

    // Set the internal ID.
    $cvterm = $this->findTermRecord($term);
    $term->setInternalId($cvterm->term_id);

    return True;
  }

  /**
   * Formats the synonyms for saving in the database.
   *
   * @param \Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object
   */
  private function formatTermSynonyms($term) {
    // Convert the synonyms into a list for storing.
    $synonyms = $term->getSynonyms();
    $synonym_list = [];
    foreach ($synonyms as $synonym => $type) {
      $type_id = NULL;
      if ($type) {
        $type_id = $type->getTermId();
      }
      $synonym_list[] = [$synonym, $type_id];
    }
    return $synonym_list;
  }

  /**
   * Formats the parents for saving in the database.
   *
   * @param \Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object
   */
  private function formatTermParents($term) {
    // In the parents array, the key is the term ID for the parent
    // (e.g. GO:0008150). The value is a tuple that should contain as its
    // first element the parent term and the second element the relationship
    // term.
    $parents = $term->getParents();
    $parents_list = [];
    foreach ($parents as $parent_term_id => $tuple) {
      $rel_type_term = $tuple[1];
      $parents_list[] = [$parent_term_id, $rel_type_term->getTermId()];
    }
    return $parents_list;
  }

  /**
   * Formats the synonyms for saving in the database.
   *
   * @param \Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object
   */
  private function formatTermProperties($term) {
    // properties is an associative array where the first level key is the
    // term_id for the property. The second level key is the rank and the
    // value is a tuple with the first element being the TripalTerm for the
    // property type and the second being the propertly value.
    $properties = $term->getProperties();
    $property_list = [];
    foreach ($properties as $term_id => $ranks) {
      foreach ($ranks as $rank => $tuple) {
        $prop_value = $tuple[1];
        $property_list[] = [$term_id, $rank, $prop_value];
      }
    }
    return $property_list;
  }

  /**
   * Inserts a new term.
   *
   * The term should be checked that it does not exist
   * prior to calling this function.
   *
   * @param \Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object to update
   *
   * @param array $options
   *   The options passed to the saveTerm() function.
   *
   * @return boolean
   *   True if the insert was successful, false otherwise.
   */
  protected function insertTerm(TripalTerm $term, array $options) {

    // Instantiate a TripalDBX connection for Chado.
    $conn = \Drupal::service('database');
    try {

      // Add the new term record.
      $insert = $conn->insert('tripal_terms');
      $insert->fields([
        'id_space' => $term->getIdSpace(),
        'vocabulary' => $term->getVocabulary(),
        'name' => $term->getName(),
        'accession' => $term->getAccession(),
        'definition' => $term->getDefinition(),
        'is_obsolete' => $term->isObsolete() ? 1 : 0,
        'is_relationship_type' => $term->isRelationshipType() ? 1 : 0,
        'synonyms' => serialize($this->formatTermSynonyms($term)),
        'properties' => serialize($this->formatTermProperties($term)),
        'altIds' => serialize($term->getAltIds()),
        'parents' => serialize($this->formatTermParents($term)),
      ]);
      $insert->execute();
      $cvterm = $this->findTermRecord($term);
      if (!$cvterm) {
        return False;
      }
    }
    catch (Exception $e) {
      $this->messageLogger->error('TripalDefaultIdSpace::insertTerm(). could not insert the term record: @message',
          ['@message' => $e->getMessage()]);
      return False;
    }
    return True;
  }

  /**
   * Updates an existing term.
   *
   * The term should be checked that it already exists
   * prior to execution of this function.
   *
   * @param \Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object to update
   * @param object $cvterm
   *   The record object for the term to update from the Chado cvterm table.
   * @param array $options
   *   The options passed to the saveTerm() function.
   *
   * @return boolean
   *   True if the update was successful, false otherwise.
   */
  protected function updateTerm(TripalTerm $term, object &$cvterm, array $options) {

    // Instantiate a TripalDBX connection for Chado.
    $conn = \Drupal::service('database');

    try {
      $update = $conn->update('tripal_terms');
      $update->fields([
        'id_space' => $term->getIdSpace(),
        'vocabulary' => $term->getVocabulary(),
        'name' => $term->getName(),
        'accession' => $term->getAccession(),
        'definition' => $term->getDefinition(),
        'is_obsolete' => $term->isObsolete() ? 1 : 0,
        'is_relationship_type' => $term->isRelationshipType() ? 1 : 0,
        'synonyms' => serialize($this->formatTermSynonyms($term)),
        'properties' => serialize($this->formatTermProperties($term)),
        'altIds' => serialize($term->getAltIds()),
        'parents' => serialize($this->formatTermParents($term)),
      ]);
      $update->condition('term_id', $cvterm->term_id);
      $update->execute();
      $cvterm = $this->findTermRecord($term);
    }
    catch (Exception $e) {
      $this->messageLogger->error('ChadoIdSpace: could not update the cvterm record: @message',
          ['@message' => $e->getMessage()]);
      return False;
    }
    return True;
  }
}
