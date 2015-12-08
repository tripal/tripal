<?php
/**
 * A class the controller will use for instantiating the TripalTerm entity.
 */
class TripalVocab extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values);

  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'vocabulary/' . $this->id);
  }

}