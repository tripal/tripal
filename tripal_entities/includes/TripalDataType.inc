<?php
/**
 * The class used for Tripal data type entities
 */
class TripalDataType extends Entity {

  public $type;
  public $label;

  public function __construct($values = array()) {
    parent::__construct($values, 'tripal_data_type');
  }
}