<?php
/**
 * A class the controller will use for instantiating the ChadoData entity.
 */
class ChadoData extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values, 'chado_data');
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'data/' . $this->entity_id);
  }

}