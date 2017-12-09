<?php
/**
 * The class used for Tripal data type entities
 */
class TripalBundle extends Entity {

  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type);

    $this->term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
    $vocab = $term->vocab;
    $this->accession = $vocab->vocabulary . ':' . $term->accession;
  }
}