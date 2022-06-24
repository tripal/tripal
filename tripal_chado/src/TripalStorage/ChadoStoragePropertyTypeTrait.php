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
  private $chado_table = NULL;
  
  /**
   * The name of the column in the table of Chado that this property belongs to.
   * @var string
   */
  private $chado_column = NULL;
  
  /** 
  * The name of the foreign key column in the base table that links to the 
  * chado table for this property. 
  */
  private $base_table_fk_column = NULL;
  
  /**
   * The name of the foreign key column in the chado table that links to 
   * the base table. Some tables may link to the base table multiple times 
   * (e.g. relationship tables) so this argument is needed to distinguish 
   * which one. If no value is given then the first foreign key column to 
   * the base table is used.
   */
  private $chado_table_fk_column = NULL;
  
  /**
   * The name of the column that limits records by type. This is useful for
   * prop tables where a type_id can be used to limit values.
   * @var string
   */
  private $type_column = NULL;
  
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
  public function getChadoTable() {
    return $this->chado_table;
  }
  /**
   * Retrieves the name of the column in the Chado table for this property.
   * 
   * @return string
   */
  public function getChadoColumn() {
    return $this->chado_column;
  }
  /**
   * Retrieves the name of the column in the Chado table for this property.
   *
   * @return string
   */
  public function getTypeColumn() {
    return $this->type_columns;
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
  public function getChadoTableFkColumn() {
    return $this->chado_table_fk_column;
  }
  
  /**
   * Retrieves the foreign key column linking the chado table to the base table.
   * @return string
   */
  public function getBaseTableFkColumn() {
    return $this->base_table_fk_column;
  }
  
  /**
   * Sets the member variables provided by this trait.
   * @param array $mapping
   */
  protected function setMapping($mapping) {
    if (array_key_exists('chado_table', $mapping)) {
      $this->chado_table = $mapping['chado_table'];
    }
    if (array_key_exists('chado_column', $mapping)) {
      $this->chado_column = $mapping['chado_column'];
    }
    if (array_key_exists('type_column', $mapping)) {
      $this->type_column = $mapping['type_column'];
    }
    if (array_key_exists('type_id', $mapping)) {
      $this->type_id = $mapping['type_id'];
    }
    if (array_key_exists('base_table_fk_column', $mapping)) {
      $this->base_table_fk_column = $mapping['base_table_fk_column'];
    }
    if (array_key_exists('chado_table_fk_column', $mapping)) {
      $this->chado_table_fk_column = $mapping['chado_table_fk_column'];
    }    
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
    if (!$schema->tableExists($this->chado_table)) {
      $logger->error('The property type, "@type", is invalid because the Chado table, "@table", does not exist.',
          ['@type' => $this->getEntityType(), '@table' => $this->chado_table]);
      return False;
    }
    
    // Make sure the column in the Chado table exists.
    $table_def = $schema->getTableDef($this->chado_table, ['format' => 'drupal']);
    if (!array_key_exists($this->chado_column, $table_def['fields'])) {
      $logger->error('The property type, "@type", is invalid because the Chado column, "@column", does not exist.',
          ['@type' => $this->getEntityType(), '@column' => $this->chado_table . '.' . $this->chado_column]);
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
