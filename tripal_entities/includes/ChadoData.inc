<?php
/**
 * The class used for chado data entities.
 */
class ChadoData extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values, 'chado_data');
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'chado_data/' . $this->entity_id);
  }

}