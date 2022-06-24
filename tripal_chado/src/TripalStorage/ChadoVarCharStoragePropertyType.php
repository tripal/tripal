<?php

namespace Drupal\tripal_chado\TripalStorage;

use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;

/**
 * Defines the variable character Tripal storage property type.
 */
class ChadoVarCharStoragePropertyType extends VarCharStoragePropertyType {

  use ChadoStoragePropertyTypeTrait;
  
  /**
   * Constructs a new variable character tripal storage property type.
   *
   * @param string entityType
   *   The entity type associated with this property type.
   * @param string fieldType
   *   The field type associated with this property type.
   * @param string key
   *   The key associated with this property type.
   * @param int size
   *   The maximum size of characters for this type.
   * @param array $mapping
   *   An associative array that maps this property to Chado. The following
   *   keys are supported:
   *   - chado_table: The name of the Chado table where the value for this
   *     property can be found.
   *   - chado_column: The name of the column in the Chado table that this 
   *     property type maps to.
   *   - base_table_fk_column: Optional. The name of the foreign key column 
   *     in the base table that links to the chado table.
   *   - chado_table_fk_column: Optional. The name of the foreign key column 
   *     in the chado table that links to the base table. Some tables may link  
   *     to the base table multiple times (e.g. relationship tables) so this 
   *     argument is needed to distinguish which one. If no value is given 
   *     then the first foreign key column to* the base table is used.
   *   - type_column: Optional. The name of the column that limits records by 
   *     type. This is useful for prop tables where a type_id can be used to 
   *     limit values.
   *   - type_id: Optional. The cvterm_id of the type_columns that limits 
   *     records by type. This is useful for prop tables where a type_id can 
   *     be used to limit values.
   */
  public function __construct($entityType, $fieldType, $key, $size = 255, $mapping) {
    parent::__construct($entityType, $fieldType, $key, $size);
    $this->setMapping($mapping);    
    $this->verifyTableColumn();
  }


}
