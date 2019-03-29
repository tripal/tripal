<?php

/**
 * A class the controller will use for instantiating the TripalTerm entity.
 */
class TripalVocab extends Entity {

  public function __construct($values = [], $entity_type) {
    parent::__construct($values, $entity_type);

  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return ['path' => 'vocabulary/' . $this->id];
  }

}