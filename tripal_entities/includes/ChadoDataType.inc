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

/**
 * Gets an array of all chado data types, keyed by the type name.
 *
 * @param $type_name
 *   If set, the type with the given name is returned.
 * @return ChadoDataType[]
 *   Depending whether $type isset, an array of chado data types or a single one.
 */
function chado_data_get_types($type_name = NULL) {
  // entity_load will get the Entity controller for our entity and call the load
  // function of that object - we are loading entities by name here.
  $types = entity_load_multiple_by_name('chado_data_type', isset($type_name) ? array($type_name) : FALSE);
  return isset($type_name) ? reset($types) : $types;
}