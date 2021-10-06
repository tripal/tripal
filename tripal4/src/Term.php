<?php

namespace Drupal\tripal4\Vocabulary;

/**
 * Defines a vocabulary term object.
 */
class Term {

  /**
   * Constructs a new term object with the given name, id space, and accession.
   *
   * @param string name
   *   The name.
   *
   * @param string idSpace
   *   The id space.
   *
   * @param string accession
   *   The accession.
   *
   * @param string defaultVocab
   *   The default vocabulary.
   */
  public function __construct($name,$idSpace,$accession,$defaultVocab) {
    $this->name = $name;
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
   *   An array of valid \Drupal\tripal4\Vocabulary\Term objects.
   */
  public static function suggestTerms(string $partial, int $max = 10) {
  }

  /**
   * Tests if the given term is equal to this term.
   *
   * @param \Drupal\tripal4\Vocabulary\Term other
   *   The other given term.
   *
   * @return bool
   *   True if equal otherwise false.
   */
  public function isEqual(Term $other) {
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
   * @return \Drupal\tripal4\Plugin\IdSpaceInterface
   *   The id space instance.
   */
  public function getIdSpaceObject() {
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
   * The term name.
   *
   * @var string
   */
  private $name;

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
