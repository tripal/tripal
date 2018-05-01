<?php

/**
 * A class the controller will use for instantiating the TripalTerm entity.
 */
class TripalTerm extends Entity {

  public function __construct($values = []) {
    parent::__construct($values, 'TripalTerm');

    // Get the vocabulary for this term
    $vocab = entity_load('TripalVocab', ['id' => $this->vocab_id]);
    $vocab = reset($vocab);
    $this->vocab = $vocab;

    // Get the term description from the storage backend
    $this->definition = NULL;
    $this->url = NULL;

    $term_details = tripal_get_term_details($vocab->vocabulary, $this->accession);
    if ($term_details) {
      if ($term_details and $term_details['definition']) {
        $this->definition = $term_details['definition'];
      }
      if ($term_details and $term_details['url']) {
        $this->url = $term_details['url'];
      }
    }
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    $vocab = 'TODO';
    return ['path' => '/vocabulary/' . $vocab . '/term/' . $this->id];
  }

  // Getters //
  public function getName() {
    return $this->name;
  }

  public function getAccession() {
    return $this->vocab->vocabulary . ':' . $this->accession;
  }

  public function getDefinition() {
    return $this->definition;
  }

  public function getURL() {
    return $this->url;
  }

  public function getVocab() {
    return $this->vocab;
  }

  public function getID() {
    return $this->id;
  }

}