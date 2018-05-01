<?php

/**
 * A class the controller will use for instantiating the TripalEntity entity.
 */
class TripalEntity extends Entity {

  public function __construct($values = [], $entity_type) {
    parent::__construct($values, $entity_type);
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return ['path' => 'TripalEntity/' . $this->id];
  }

}