<?php

namespace Drupal\tripal\TripalVocabTerms;

/**
 * Defines a vocabulary term object.
 */
class TripalTerm {

  /**
   * Constructs a new term object.
   *
   * Either use the set functions of this object to provide
   * the necessary values or provide them using the $details argument.
   *
   * Use the isValid() function to make sure the term is valid
   * before saving. A term must have the following values set to be
   * valid:  name, idSpace, vocabulary, and  accession.
   *
   * The $details argument accepts the following keys and values
   *
   * - name: (string) the name of the term
   * - definition: (string) the definitino for the term
   * - is_obsolete: (bool) True if the term is obsolete. Default is False.
   * - is_relationship_type: (bool) True if the term is a relationship type.
   *   Default is False.
   * - idSpace: (array) the ID space name.
   * - vocabulary: (array) the vocabulary name.
   * - parents: (array) an array of parents where each element
   *   in the array is a tuple of two values: a TripalTerm for the parent
   *   and a second TripalTerm for the relationship.
   * - altIDs: (array) an array of alternate term IDs where each
   *   element is a tuple of two values: the ID space name and
   *   the accession of the alternate term.
   * - synonyms: (array)  an array of synonyms where each
   *   element is the synonym, or a list of typles containng the synonym
   *   followed by the a TripalTerm for the synonym type. Usually the synonym
   *   types are terms with a name of: 'exact', 'broad', 'narrow' or 'related'.
   * - properties:(array) an array of properties where each element is a
   *   tuple of two values: a TriaplTerm for the property type and a value.
   *   If the same property is used multiple times with different values,
   *   then the rank will be set in the order that they are provided.
   *
   * @param array|NULL $details
   *   The name.
   */
  public function __construct(array $details = NULL) {

    // Instantiate the TripalLogger.
    $this->messageLogger = \Drupal::service('tripal.logger');

    // Initalize the member variables.
    $this->name = '';
    $this->definition = '';
    $this->accession = '';
    $this->is_obsolete = False;
    $this->is_relationship_type = False;
    $this->idSpace = '';
    $this->vocabulary = '';
    $this->parents = [];
    $this->altIds = [];
    $this->synonyms = [];
    $this->properties = [];
    $this->loaded_attributes = [
      'definition' => False,
      'is_obsolete' => False,
      'is_relationship_type' => False,
      'properties' => False,
      'synonyms' => False,
      'altIds' => False,
      'parents' => False,
    ];
    $this->internalId = NULL;

    if (!is_array($details)) {
      return;
    }

    // Check for problems in the incoming $details argument.
    if (array_key_exists('is_obsolete', $details) and !is_bool($details['is_obsolete'])) {
      $this->messageLogger->error('TripalTerm::__construct(). The is_obsolete value must be boolean.');
    }
    if (array_key_exists('is_relationship_type', $details) and !is_bool($details['is_relationship_type'])) {
      $this->messageLogger->error('TripalTerm::__construct(). The is_relationship_type value must be boolean.');
    }

    // Set the values provided.
    if (array_key_exists('name', $details)) {
      $this->setName($details['name']);
    }
    if (array_key_exists('accession', $details)) {
      $this->setAccession($details['accession']);
    }
    if (array_key_exists('definition', $details)) {
      $this->setDefinition($details['definition']);
    }
    if (array_key_exists('idSpace', $details)) {
      $this->setIdSpace($details['idSpace']);
    }
    if (array_key_exists('vocabulary', $details)) {
      $this->setVocabulary($details['vocabulary']);
    }
    if (array_key_exists('synonyms', $details) and is_array($details['synonyms'])) {
      foreach ($details['synonyms'] as $entry) {
        if (is_array($entry)) {
          if (count($entry) != 2) {
            $this->messageLogger->error('TripalTerm::__construct(). An synonym tuple is not the correct size.');
            continue;
          }
          $this->addSynonym($entry[0], $entry[1]);
        }
        else {
          $this->addSynonym($entry, NULL);
        }
      }
    }
    if (array_key_exists('altIDs', $details) and is_array($details['altIDs'])) {
      foreach ($details['altIDs'] as $tuple) {
        if (count($tuple) != 2) {
          $this->messageLogger->error('TripalTerm::__construct(). An altID tuple is not the correct size.');
          continue;
        }
        $this->addAltId($tuple[0], $tuple[1]);
      }
    }
    if (array_key_exists('parents', $details) and is_array($details['parents'])) {
      foreach ($details['parents'] as $tuple) {
        if (count($tuple) != 2) {
          $this->messageLogger->error('TripalTerm::__construct(). A parents tuple is not the correct size.');
          continue;
        }
        $this->addParent($tuple[0], $tuple[1]);
      }
    }
    if (array_key_exists('properties', $details) and is_array($details['properties'])) {
      foreach ($details['properties'] as $tuple) {
        if (count($tuple) != 2) {
          $this->messageLogger->error('TripalTerm::__construct(). A properties tuple is not the correct size.');
          continue;
        }
        $this->addProperty($tuple[0], $tuple[1]);
      }
    }
    if (array_key_exists('is_obsolete', $details)) {
      $this->setIsObsolete($details['is_obsolete']);
    }
    if (array_key_exists('is_relationship_type', $details)) {
      $this->setIsRelationshipType($details['is_relationship_type']);
    }
  }

  /**
   * Indicates if this term is valid and can be saved.
   *
   * @return boolean
   *   True if valid, False otherwise.
   */
  public function isValid() : bool {
    $is_valid = TRUE;

    if (empty($this->getName())) {
      $is_valid = FALSE;
    }
    if (empty($this->getIdSpace()) OR empty($this->getAccession())) {
      $is_valid = FALSE;
    }
    if (empty($this->getVocabulary())) {
      $is_valid = FALSE;
    }

    return $is_valid;
  }

  /**
   * Sets the ID space for the term.
   *
   * @param string setIdSpace
   *   The name of the ID space.
   */
  public function setIdSpace(string $idSpace) {

    $manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idsp = $manager->loadCollection($idSpace);
    if (!$idsp) {
      $this->messageLogger->error(t('TripalTerm::setIdSpace(). The specified ID space, "@idSpace", does not exist.',
          ['@idSpace' => $idSpace]));
      return;
    }
    $this->idSpace = $idSpace;
  }

  /**
   * Sets the vocabulary for the term.
   *
   * @param string $vocabulary
   *   The name of the vocabulary.
   */
  public function setVocabulary(string $vocabulary) {

    $manager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocab = $manager->loadCollection($vocabulary);
    if (!$vocab) {
      $this->messageLogger->error(T('TripalTerm::setVocabulary(). The specified vocabulary, "@vocab" does not exist.',
          ['@vocab' => $vocabulary]));
      return;
    }
    $this->vocabulary = $vocabulary;
  }

  /**
   * Sets the term's description.
   *
   * @param string $description
   */
  public function setDefinition(string $definition) {
    $this->loaded_attributes['definition'] = True;
    $this->definition = $definition;
  }


  /**
   * Returns a list of valid terms based off matches from the given partial term
   * name. A given max number of terms are returned.
   *
   * @param string $partial
   *   The partial term name.
   *
   * @param int $max
   *   The given max number returned.
   *
   * @return array
   *   An array of valid Drupal\tripal\TripalVocabTerms\Term objects.
   */
  public static function suggestTerms(string $partial, int $max = 10) {
    $terms = [];

    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach ($idsmanager->getCollectionList() as $name) {
      $idspace = $idsmanager->loadCollection($name);
      $terms[] = $idspace->getTerms($partial, ["exact" => FALSE]);
    }

    return $terms;
  }

  /**
   * Tests if the given term is equal to this term.
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm other
   *   The other given term.
   *
   * @return bool
   *   True if equal otherwise false.
   */
  public function isEqual(TripalTerm $other) {
      return $this->idSpace == $other->idSpace && $this->accession == $other->accession;
  }

  /**
   * Returns this term's name.
   *
   * @return string
   *   The name.
   */
  public function getName() {
    return $this->name;
  }

  /**
   * Sets this term's name to the given name.
   *
   * @param string $name
   *   The name.
   */
  public function setName($name) {
    $this->name = $name;
  }

  /**
   * Returns this term's definition.
   *
   * @return string
   *   The definition.
   */
  public function getDefinition() {
    return $this->definition;
  }

  /**
   * Returns this term's id space collection name.
   *
   * @return string
   *   The id space collection name.
   */
  public function getIdSpace() {
    return $this->idSpace;
  }

  /**
   * Returns an instance of this term's id space.
   *
   * @return Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface
   *   The id space instance.
   */
  public function getIdSpaceObject() {
    $manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    return $manager->loadCollection($this->idSpace);
  }

  /**
   * Returns an instance of this term's vocabulary.
   *
   * @return Drupal\tripal\TripalVocabTerms\Interfaces\TripalVocabularyInterface
   *   The vocabulary instance.
   */
  public function getVocabularyObject() {
    $manager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    return $manager->loadCollection($this->vocabulary);
  }

  /**
   * Sets this term's accession.
   *
   * @param string
   *   The accession.
   */
  public function setAccession($accession) {
    return $this->accession = $accession;
  }

  /**
   * Returns this term's accession.
   *
   * @return string
   *   The accession.
   */
  public function getAccession() {
    return $this->accession;
  }

  /**
   * Returns this term's default vocabulary collection name.
   *
   * @return string
   *   The vocabulary collection name.
   */
  public function getVocabulary() {
    return $this->vocabulary;
  }

  /**
   * Returns this term's URL.
   *
   * @return string
   *   The URL.
   */
  public function getURL() {
    $idSpace = $this->getIdSpaceObject();
    $term_url = $idSpace->getURLPrefix();
    $idSpace_name = $idSpace->getName();
    $subbed = False;

    if (!$term_url) {
      $this->messageLogger->warning('TripalTerm::getURL(). The ID space has no URL prefix.');
    }

    // If the URL prefix has replacement tokens then apply those.
    if (preg_match('/\{db\}/', $term_url)) {
      $term_url = preg_replace("/\{db\}/", $idSpace_name, $term_url);
      $subbed = True;
    }
    if (preg_match('/\{accession\}/', $term_url)) {
      $term_url = preg_replace("/\{accession\}/", $this->accession, $term_url);
      $subbed = True;
    }

    // If no replacement tokens were applied then just add the term
    // to the end.
    if (!$subbed) {
      $term_url = $term_url . $idSpace_name . ":" . $this->accession;
    }

    return $term_url;
  }

  /**
   * Saves a term to its ID space data store.
   *
   * If a term is new in the ID space and has no parents then it will
   * be considered a "root" term for the vocabulary. If the term
   * has parents, use the `addParents()` function to add them before
   * calling this function.  If the term is not new and already exists
   * you only need to provide parents if you need to change the parentage.
   * If the `updateParent` option is True then all parents of an existing
   * term will be removed and will be updated to the parents provided.  If
   * `updateParent` is False and no parents are provided then no change
   * is made to the parent relationships.
   *
   * The options array accepts the following recognized keys:
   *
   * failIfExists(boolean): True to force this method to fail if this term
   * already exists else false to update this term if it already exists. The
   * default is false.
   *
   * updateParent(boolean): True to update this term's parent to the one
   * given or false to not update this existing term's parent. If this term
   * is new this has no effect. The default is false.
   *
   * @param array $options
   *   The options array.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function save($options) {

    if(!$this->isValid()) {
      $this->messageLogger->error('TripalTerm::save(). Cannot save the term as it is not currently in a valid state.');
      return False;
    }
    $idspace = $this->getIdSpace();
    return $idspace->saveTerm($this, $options);
  }

  /**
   * Retrieves the  ID for this term.
   *
   * The term ID is the combination of the ID space and the
   * accession (e.g. GO:0008150).
   *
   * @return string
   *   The term ID.
   */
  public function getTermId() {
    return $this->idSpace . ":" . $this->accession;
  }

  /**
   * Adds a parent term
   *
   * A term may have zero or more parents. A term without parents
   * will be considered a root term.  The relationship between the
   * child term and the parent must be specified by another term
   * indicating the relationship (e.g. `is_a`, `derives_from`, etc).
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $parent
   *   The parent term or NULL.
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $relationship
   *   The relationship term or NULL.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function addParent(TripalTerm $parent, TripalTerm $relationship) {
    $this->loaded_attributes['parents'] = True;
    $this->parents[$parent->getTermId()] = [$parent, $relationship];
  }

  /**
   * Returns the parents for this term.
   *
   * @return array
   *   In the array, the key is the term ID for the parent (e.g. GO:0008150).
   *   The value is a tuple that should contain as its first element the
   *   parent term and the second element the relationship term.
   */
  public function getParents() {
    return $this->parents;
  }

  /**
   * Removes a parent from the term.
   *
   * @param string $idSpace
   *   The ID space name of the parent term.
   *
   * @param string $accession
   *   The accession for the parent term.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function removeParent(string $idSpace, string $accession) : bool {
    $term_id = $idSpace . ':' . $accession;
    if (array_key_exists($term_id, $this->parents)) {
      unset($this->parents[$term_id]);
      return True;
    }
    return False;
  }


  /**
   * Adds an alternative term ID for this term.
   *
   * @param string $idSpace
   *   The ID space name of the parent term.
   *
   * @param string $accession
   *   The accession for the parent term.
   */
  public function addAltId(string $idSpace, string $accession) {
    $this->loaded_attributes['altIds'] = True;
    $term_id =  $idSpace . ':' . $accession;
    $this->altIds[$term_id] = 1;
  }

  /**
   * Returns the list of alternate IDs for this term.
   *
   * @return array
   *   An array of term ID strings.
   */
  public function getAltIds() : array {
    return array_keys($this->altIds);
  }

  /**
   * Removes an alternate ID from this term.
   *
   * @param string $idSpace
   *   The ID space name of the parent term.
   *
   * @param string $accession
   *   The accession for the parent term.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function removeAltId(string $idSpace, string $accession) : bool {
    $term_id = $idSpace . ':' . $accession;
    if (array_key_exists($term_id, $this->altIds)) {
      unset($this->altIds[$term_id]);
      return True;
    }
    return False;
  }

  /**
   * Adds a synonym for this term.
   *
   * Some terms may have synonymous names. The synonym type
   * is usually one of the of the following terms: 'exact',
   * 'broad', 'narrow', or 'related'.
   *
   * It is highly encouraged to always provide a type for the
   * synonym.
   *
   *
   * @param string $synonym
   *   The synonym.
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $type
   *   An optional Tripal term indicating the type of synonym.
   */
  public function addSynonym(string $synonym, TripalTerm $type = NULL) {
    $this->loaded_attributes['synonyms'] = True;
    $this->synonyms[$synonym] = $type;
  }

  /**
   * Removes a synonym from this term.
   *
   * @param string $synonym
   *   The synonym.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function removeSynonym(string $synonym) : bool {
    if (array_key_exists($synonym, $this->synonyms)) {
      unset($this->synonyms[$synonym]);
      return True;
    }
    return False;
  }

  /**
   * Returns the list of synonyms for this term.
   *
   * @return array
   *   An associative array where the keys are the synonyms
   *   and the values the synonym type TripalTerm.
   */
  public function getSynonyms() : array {
    return $this->synonyms;
  }

  /**
   * Sets if the term is obsolete or not.
   *
   * @param bool $is_obsolete
   *   True if the term is obsolete, False otherwise.
   */
  public function setIsObsolete(bool $is_obsolete) {
    $this->loaded_attributes['is_obsolete'] = True;
    $this->is_obsolete = $is_obsolete;
  }

  /**
   * Indicates if this term is obsolete or not.
   *
   * @return bool
   *   True if the term is obsolete, False otherwise.
   */
  public function isObsolete() : bool {
    return $this->is_obsolete;
  }

  /**
   * Sets if the term is a relationship type term.
   *
   * @param bool $is_obsolete
   *   True if the term is a relationship type, False otherwise.
   */
  public function setIsRelationshipType(bool $is_relationship_type) {
    $this->loaded_attributes['is_relationship_type'] = True;
    $this->is_relationship_type = $is_relationship_type;
  }

  /**
   * Indicates if the term is a relationship type term.
   *
   * @return bool
   *   True if the term is a relationship type, False otherwise.
   */
  public function isRelationshipType() : bool {
    return $this->is_relationship_type;
  }


  /**
   * Adds a property to this term.
   *
   * @param TripalTerm $term
   *   A term indicating the propery type.
   * @param string $value
   *   The value of the property.
   * @param int|NULL $rank
   *   The rank (or order) of the value. If no rank is specified and if a
   *   property of the same term is already present then the rank will be
   *   incremented for the next value added.
   *
   * @return bool
   *   True if the property was successfully added, False otherwise.
   */
  public function addProperty(TripalTerm $term, string $value, int $rank = NULL) : bool {
    $this->loaded_attributes['properties'] = True;

    // Get the max rank for this property.
    $term_id = $term->getTermId();
    if (!array_key_exists($term_id, $this->properties)) {
      $this->properties[$term_id] = [];
    }
    $max_rank = count($this->properties[$term_id]);

    // Make sure we aren't skipping a rank.
    for ($i = 0; $i < $max_rank; $i++) {
      if (!array_key_exists($i, $this->properties[$term_id])) {
        $this->messageLogger->error('TripalTerm::addProperty. The property term ranks are out of order, cannot add a new property.');
        return False;
      }
    }

    // Make sure the user didn't ask for a rank that exeeds the next one.
    if ($rank != NULL) {
      if ($rank > $max_rank) {
        $this->messageLogger->error('TripalTerm::addProperty. The specified rank is higher than the next max rank.');
        return False;
      }
    }
    else {
      $rank = $max_rank;
    }

    // Set the property.
    $this->properties[$term_id][$rank] = [$term, $value];
    return True;
  }

  /**
   * Retrieves the list of properties for this term.
   *
   * @return array
   *  An associative array where the first level key is the term_id for the property.
   *  The second level key is the rank and the value is a tuple with the first element
   *  being the TripalTerm for the property type and the second being the propertly value.
   */
  public function getProperties() : array {
    return $this->properties;
  }

  /**
   * Removes a property from the list of properties.
   *
   * @param string $idSpace
   *   The ID space name of the property term.
   *
   * @param string $accession
   *   The accession for the property term.
   *
   * @param int $rank
   *   The rank of the value to remove.
   *
   * @return bool
   *   True on success or false otherwise.

   */
  public function removeProperty(string $idSpace, string $accession, int $rank) : bool {
     $term_id = $idSpace . ':' . $accession;
     if (array_key_exists($term_id, $this->properties)) {
       if (array_key_exists($rank, $this->properties[$term_id])) {
         unset($this->properties[$term_id][$rank]);
         if (count($this->properties[$term_id]) == 0) {
           unset($this->properties[$term_id]);
         }
         return True;
       }
     }
     $this->messageLogger->error('TripalTerm::removeProperty(). Could not find the property, "@prop", for removal.',
       ['@prop' => $term_id]);
     return False;
  }

  /**
   * Indicates which attributes are loaded.
   *
   * A term may have any number of attributes that may or may not
   * be loaded. The array returned by this function will
   * indicatew which attributes are loaded and which are not. The
   * array keys are the attribute names and the value is a boolean
   * indicating if the attribute has been loaded. If all attributes
   * are true then all all ahve been loaded and the term is complete.
   * If some attributes are False then the term is not complete.
   *
   * @return array
   *   An associative array with keys indicating the attributes
   *   and the value being a boolean where True indicates that the
   *   attribute is laoded and False otherwise.
   */
  public function getLoadedAttributes() {
    return $this->loaded_attributes;
  }

  /**
   * Sets the internal ID of this term to the given internal ID.
   *
   * @param mixed $internalId
   *   The internal ID.
   */
  public function setInternalId($internalId) {
    $this->internalId = $internalId;
  }

  /** Gets the internal ID of this term. The default is NULL.
   *
   * @return mixed
   *   The internal ID.
   */
  public function getInternalId() {
    return $this->internalId;
  }



  /**
   * An associative array listing the parents.
   *
   * The key is the term ID for the parent (e.g. GO:0008150). The value is a
   * tuple that should contain as its first element the parent term
   * and the second element the relationship term.
   *
   * @var array
   */
  private $parents;

  /**
   * The term name.
   *
   * @var string
   */
  private $name;

  /**
   * The term definition.
   *
   * @var string
   */
  private $definition;

  /**
   * The ID space this terms belongs to.
   *
   * @var string
   */
  private $idSpace;

  /**
   * The term accession.
   *
   * @var string
   */
  private $accession;

  /**
   * The vocabulary this term belongs to.
   *
   * @var string
   */
  private $vocabulary;


  /**
   * An array of alternate IDs for this term.
   *
   * For easy lookup, this is an associative array where
   * the key is the term_id and the value is 1.
   * Using the term_id as the key also prevents duplication.
   *
   * @var array
   */
  private $altIds;


  /**
   * An array of synonyms.
   *
   * For easy lookup, this is an associative array where
   * the key is the synonym and the value is the type term.
   * Using the synonym as the key also prevents duplication.
   *
   * @var array
   */
  private $synonyms;


  /**
   * Indicates if the term is obsolete or not.
   *
   * @var bool
   */
  private $is_obsolete;

  /**
   * Indicates if the term is a relationship type.
   *
   * @var bool
   */
  private $is_relationship_type;

  /**
   * A array of properties for this term.
   *
   * The associative array first level key is the
   * term_id for the property, the second level key is the
   * rank and the value is a tuple with the first element
   * being the TripalTerm for the property type and
   * the second being the propertly value.
   *
   * @var array
   */
  private $properties;


  /**
   * An instance of the TripalLogger.
   */
  private $messageLogger = NULL;


  /**
   * An associative array indicating which attributes of the term are loaded.
   */
  private $loaded_attributes;

  /**
   * An internal ID that can be used by specific plugin implementations.
   */
  private $internalId;
}
