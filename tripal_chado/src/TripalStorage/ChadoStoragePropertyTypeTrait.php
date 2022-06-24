<?php

namespace Drupal\tripal_chado\TripalStorage;

/**
 * This is a PHP Trait for Chado property storage types.
 *
 * It provides the functions and member variables that are 
 * used for any of the proprty storage types.
 *
 * @group Tripal
 * @group Tripal Chado
 */

trait ChadoStoragePropertyTypeTrait  {
   
   
  /**
   * The name of the table in Chado that this property belongs to.
   * @var string
   */
  private $table = NULL;
  
  /**
   * The name of the column in the table of Chado that this property belongs to.
   * @var string
   */
  private $column = NULL;
  
  /** 
  * The name of the foreign key column in the base table that links to the 
  * chado table for this property. 
  */
  private $base_fk_col = NULL;
  
  /**
   * The name of the foreign key column in the chado table that links to 
   * the base table. Some tables may link to the base table multiple times 
   * (e.g. relationship tables) so this argument is needed to distinguish 
   * which one. If no value is given then the first foreign key column to 
   * the base table is used.
   */
  private $table_fk_col = NULL;
  
  /**
   * The name of the column that limits records by type. This is useful for
   * prop tables where a type_id can be used to limit values.
   * @var string
   */
  private $type_col = NULL;
  
  /**
   * The cvterm_id of the type_columns that limits records by type. This is 
   * useful for prop tables where a type_id can be used to limit values.
   * @var string
   */
  private $type_id = NULL;
  
  /**
   * Indicates if this property type is valid for Chado.
   * 
   * @var boolean
   */
  private $is_valid = False;
  
  
  /**
   * Retrieves the name of the table in Chado for this property.
   *
   * @return string
   */
  public function getTable() {
    return $this->table;
  }
  /**
   * Retrieves the name of the column in the Chado table for this property.
   * 
   * @return string
   */
  public function getColumn() {
    return $this->column;
  }
  /**
   * Retrieves the name of the column in the Chado table for this property.
   *
   * @return string
   */
  public function getTypeColumn() {
    return $this->type_col;
  }
  /**
   * Retrieves the name of the column in the Chado table for this property.
   *
   * @return string
   */
  public function getTypeId() {
    return $this->type_id;
  }
  
  /**
   * Retrieves the foreign key column linking the chado table to the base table.
   * @return string
   */
  public function getTableFkColumn() {
    return $this->table_fk_col;
  }
  
  /**
   * Retrieves the foreign key column linking the chado table to the base table.
   * @return string
   */
  public function getBaseFkColumn() {
    return $this->base_fk_col;
  }
  
  /**
   * Sets the member variables provided by this trait.
   * @param array $mapping
   */
  protected function setMapping() {
    
    $public = \Drupal::database();
    $logger = \Drupal::service('tripal.logger');
    
    $entity_type = $this->getEntityType();
    $field_type = $this->getFieldType();
    $key =  $this->getKey();
    
    // @todo remove the "1:" prefix once issue #217 is fixed.
    $result = $public->select('chado_fields', 'cf')
      ->fields('cf')
      ->condition('entity_type', $entity_type)
      ->condition('field_type', $field_type)
      ->condition('key', $key)
      ->execute();
    if (!$result) {
      $logger->error('The property, "@prop", does not map to Chado',
          ['@prop' => $entity_type . '.' . $field_type . '.' . $key]);
      return;
    }
    $mapping = $result->fetchAssoc();
    
    $this->table = $mapping['table'];
    $this->column = $mapping['column'];
    $this->type_col = $mapping['type_col'];
    $this->type_id = $mapping['type_id'];
    $this->base_fk_col = $mapping['base_fk_col'];
    $this->table_fk_col = $mapping['table_fk_col'];    
  }
  
  /**
   * Verifies that the Chado table and column for this property exist.
   * 
   * @return boolean
   */
  protected function verifyTableColumn() {
    $logger = \Drupal::service('tripal.logger');
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    
    // Make sure the Chado table exists.
    if (!$schema->tableExists($this->table)) {
      $logger->error('The property type, "@type", is invalid because the Chado table, "@table", does not exist.',
          ['@type' => $this->getEntityType(), '@table' => $this->table]);
      return False;
    }
    
    // Make sure the column in the Chado table exists.
    $table_def = $schema->getTableDef($this->table, ['format' => 'drupal']);
    if (!array_key_exists($this->column, $table_def['fields'])) {
      $logger->error('The property type, "@type", is invalid because the Chado column, "@column", does not exist.',
          ['@type' => $this->getEntityType(), '@column' => $this->table . '.' . $this->column]);
      return False;
    }
    $this->is_valid = True;
  }
  
  /**
   * Indicates if this property type is valid.
   * 
   * @todo remove this if the `isValid()` function is added to the
   * StoragePropertyTypeBase class. 
   * 
   * @return boolean
   */
  public function isValid() {
    return $this->is_valid;
  }

}
