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
   * The term accession.
   *
   * @var string
   */
  public $accession;

  /**
   * Constructs a new term object with the given name and accession.
   *
   * @param string name
   *   The name.
   *
   * @param string accession
   *   The accession.
   */
  public function __construct($name,$accession) {
    $this->name = $name;
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
      return $this->name == $other->name && $this->accession == $other->accession;
  }

}
