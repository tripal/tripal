<?php
/**
 * A class the controller will use for instantiating the TripalTerm entity.
 */
class TripalTerm extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values, 'TripalTerm');

  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    $vocab = 'TODO';
    return array('path' => '/vocabulary/' . $vocab . '/term/' . $this->id);
  }

}