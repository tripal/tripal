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
  public function __construct($name,$definition,$idSpace,$accession,$defaultVocab) {
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
    // TODO
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
   * @return Drupal\tripal\TripalVocabTerms\Interface\TripalIdSpaceInterface
   *   The id space instance.
   */
  public function getIdSpaceObject() {
    // TODO
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
    // TODO
  }

  /**
   * Saves this term to its id space collection's permanent storage. If this
   * term already exists in its id space collection then it is updated with this
   * term instance's data, else it is added as a new term.
   */
  public function save() {
    // TODO
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
