<?php
/**
 * The class used for Chado data type entities
 */
class ChadoDataType extends Entity {

  public $type;
  public $label;

  public function __construct($values = array()) {
    parent::__construct($values, 'chado_data_type');
  }
}