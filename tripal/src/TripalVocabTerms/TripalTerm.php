<?php

namespace Drupal\tripal\TripalVocabTerms;

/**
 * Defines a vocabulary term object.
 */
class TripalTerm {

  /**
   * Constructs a new term object with the given name, definition, id space,
   * accession, and default vocabulary.
   *
   * @param string $name
   *   The name.
   *
   * @param string $definition
   *   The definition.
   *
   * @param string $idSpace
   *   The id space.
   *
   * @param string $accession
   *   The accession.
   *
   * @param string defaultVocab
   *   The default $vocabulary.
   */
  public function __construct($name, $definition, $idSpace, $accession, $defaultVocab) {
    $this->name = $name;
    $this->definition = $definition;
    $this->idSpace = $idSpace;
    $this->defaultVocab = $defaultVocab;
    $this->accession = $accession;
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
    $terms = []

    $manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach ($manager->getCollectionList() as $name) {
      $idspace = $manager->loadCollection($name);
      $terms[] = $idspace->getTerms($partial,["exact" => FALSE]);
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
   * Sets this term's definition to the given definition.
   *
   * @param string $definition
   *   The definition.
   */
  public function setDefinition($definition) {
    $this->definition = $definition;
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
  public function getDefaultVocab() {
    return $this->defaultVocab;
  }

  /**
   * Returns this term's URL.
   *
   * @return string
   *   The URL.
   */
  public function getURL() {
    $idSpace = $this->getIdSpace();
    $term_url = $idSpace->getURLPrefix();
    $idSpace_name = $idSpace->getName();
    $subbed = False;
    
    // If the URL prefix has replacement tokens then apply those.
    if (preg_match('/\{db\}/', $term_url)) {
      $term_url = preg_replace("/\{db\}/", $idSpace_name, $term_url);
      $subbed = True;
    }
    if (preg_match('/\{db\}/', $term_url)) {
      $term_url = preg_replace("/\{accession\}/", $this->accession, $term_url);
    }
    
    // If no replacement tokens were applied then just add the term 
    // to the end.
    if (!$subbed) {
      $term_url = $term_url . $idSpace_name . ":" . $this->accession;
    }
    
    return $term_url;
  }

  /**
   * Saves this term to its id space.
   * 
   * If the given parent is NULL and this term is new then it is added as a 
   * root term of its id space. If the given parent is NULL but
   * this term already exists, and the appropriate option was given to
   * update this existing term's parent then it is moved to a root term of its
   * id space.
   * 
   * If the parent is not NULL then a relationship term must be provided
   * indicating the type of the relationship with the parent (e.g. `is_a`,
   * `derives_from`, etc.).
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
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm|NULL $relationship
   *   The relationship term or NULL.
   *   
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm|NULL $parent
   *   The parent term or NULL.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function save($options, $parent = NULL, $relationship = NULL) {
    $idspace = $this->getIdSpace();
    return $idspace->saveTerm($this, $options, $parent, $relationship);
  }

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
   * The term id space.
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
   * The default vocabulary.
   *
   * @var string
   */
  private $defaultVocab;

}
