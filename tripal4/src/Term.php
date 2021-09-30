<?php

namespace Drupal\tripal4\Vocabulary;

/**
 * Defines a vocabulary term object.
 */
class Term {

  /**
   * The term name.
   *
   * @var string
   */
  public $name;

  /**
   * The term id space.
   *
   * @var string
   */
  public $idSpace;

  /**
   * The default vocabulary.
   *
   * @var string
   */
  public $defaultVocab;

  /**
   * The term accession.
   *
   * @var string
   */
  public $accession;

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
   * Constructs a new term object with the given name, id space, and accession.
   *
   * @param string name
   *   The name.
   *
   * @param string idSpace
   *   The id space.
   *
   * @param string defaultVocab
   *   The default vocabulary.
   *
   * @param string accession
   *   The accession.
   */
  public function __construct($name,$idSpace,$defaultVocab,$accession) {
    $this->name = $name;
    $this->idSpace = $idSpace;
    $this->defaultVocab = $defaultVocab;
    $this->accession = $accession;
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

}
