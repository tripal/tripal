<?php

/**
 * The class used for Tripal data type entities
 */
class TripalBundle extends Entity {

  public function __construct($values = [], $entity_type) {
    parent::__construct($values, $entity_type);

    $this->term = tripal_load_term_entity(['term_id' => $this->term_id]);
    $vocab = $this->term->vocab;
    $this->accession = $vocab->vocabulary . ':' . $this->term->accession;
  }
}