<?php
/**
 * The class used for Tripal data type entities
 */
class TripalDataType extends Entity {

  public $type;
  public $label;

  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type .'_type');
  }
}