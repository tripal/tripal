<?php
/**
 * A class the controller will use for instantiating the TripalData entity.
 */
class TripalData extends Entity {
  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type);
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'data/' . $this->id);
  }

}