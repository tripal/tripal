<?php
/**
 * A class the controller will use for instantiating the TripalTerm entity.
 */
class TripalVocab extends Entity {
  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type);

  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'vocabulary/' . $this->id);
  }

}