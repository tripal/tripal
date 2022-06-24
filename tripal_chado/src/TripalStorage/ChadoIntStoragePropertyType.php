<?php

namespace Drupal\tripal_chado\TripalStorage;

use Drupal\tripal\TripalStorage\IntStoragePropertyType;

/**
 * Defines the integer Tripal storage property type.
 */
class ChadoIntStoragePropertyType extends IntStoragePropertyType {
  
  use ChadoStoragePropertyTypeTrait;

  /**
   * Constructs a new integer tripal storage property type.
   *
   * @param string entityType
   *   The entity type associated with this property type.
   * @param string fieldType
   *   The field type associated with this property type.
   * @param string key
   *   The key associated with this property type.
   * @param $chado_table
   *   The name of the Chado table that this property type maps to.
   * @param $chado_column
   *   The name of the column in the Chado table that this property type maps 
   *   to.
   */
  public function __construct($entityType, $fieldType, $key, $chado_table, $chado_column) {
    parent::__construct($entityType, $fieldType, $key);
    $this->chado_table = $chado_table;
    $this->chado_column = $chado_column;    
    $this->verifyTableColumn();
  }
    
}
