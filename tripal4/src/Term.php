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
   * The term accession.
   *
   * @var string
   */
  public $accession;

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
   */
  public function __construct($name,$idSpace,$accession) {
    $this->name = $name;
    $this->idSpace = $idSpace;
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
