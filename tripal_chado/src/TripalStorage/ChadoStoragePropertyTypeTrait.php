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
