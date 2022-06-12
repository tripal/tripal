<?php

namespace Drupal\tripal_chado\Plugin\TripalIdSpace;

use Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase;

/**
 * Chado Implementation of TripalIdSpaceBase
 * 
 *  @TripalIdSpace(
 *    id = "chado_id_space",
 *    label = @Translation("Vocabulary IDSpace in Chado"),
 *  )
 */
class ChadoIdSpace extends TripalIdSpaceBase {
  
  protected $default_vocabulary = NULL;
  
  /**
   * Holds the TripalDBX instance for accessing Chado.
   */
  protected $chado = NULL;
    
  
  /**
   * The definition for the `db` table of Chado.
   */
  protected $db_def = NULL;
  
  
  /**
   * An instance of the TripalLogger.
   */
  protected $messageLogger = NULL;
  
  /**
   * A simple boolean to prevent Chado queries if the ID space isn't valid.
   */
  protected $is_valid = False;
  
  
  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    
    // Instantiate the TripalLogger
    $this->messageLogger = \Drupal::service('tripal.logger');
    
    // Instantiate a TripalDBX connection for Chado.
    $this->chado = \Drupal::service('tripal_chado.database');
    
    // Get the chado definition for the `db` table.
    $this->db_def = $this->chado->schema()->getTableDef('db', ['Source' => 'file']);
  }
  
  
  /**
   * {@inheritdoc}
   */
  public function isValid() {    
    
    // Make sure the name of this ID Space does not exceeed the allowed size in Chado.
    if (strlen($this->getName()) > $this->db_def['fields']['name']['size']) {
      $this->messageLogger->error('ChadoIdSpace: The IdSpace name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->db_def['fields']['name']['size'],
           '@value' => $this->getName()]);
          return;
    }
    
    $this->is_valid = True;
    
    return $this->is_valid;
  }
  
  
  /**
   * {@inheritdoc}
   */
  public function create() {
    
    // Check if the record already exists in the database, if it
    // doesn't then insert it.  We don't yet have the description,
    // URL prefix, etc but that's okay, the name is all that is
    // required to create a record in the `db` table.
    $db = $this->loadIdSpace();
    if (!$db) {
      $query = $this->chado->insert('1:db')
        ->fields(['name' => $this->getName()]);
      $query->execute();
    }
  }
  
  /**
   * {@inheritdoc}
   */
  public function destroy(){
    // The destroy function is meant to delete the ID space.
    // But, because CVs and DBs are so critical to almost all 
    // data in Chado we don't want to remove the records.  
    // Let's let the collection be deleted as far as 
    // Tripal is concerned but leave the record in Chado.
    // So, do nothing here.
  }  
  
  /**
   * Loads an ID Space record from Chado.
   *
   * This function queries the `db` table of Chado to get the values
   * for the ID space.
   *
   * @return
   *   An associative array containing the columns of the `db1 table
   *   of Chado or NULL if the db could not be found.
   */
  protected function loadIdSpace() {
    
    // Get the Chado `db` record for this ID space.    
    $query = $this->chado->select('1:db', 'db')
      ->condition('db.name', $this->getName(), '=')
      ->fields('db', ['name', 'url', 'urlprefix', 'description']);
    $result = $query->execute();
    if ($result) {
      return $result->fetchAssoc();
    }
    return NULL;
  }     
  
  /**
   * {@inheritdoc}
   */
  public function getParent($child){
    
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
    
  }
  
  /**
   * {@inheritdoc}
   */
  public function getChildren($parent = NULL){
    
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
    
  }
  
  /**
   * {@inheritdoc}
   */
  public function getTerm($accession){
    
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
  }
  
  /**
   * {@inheritdoc}
   */
  public function getTerms($name, $options){
    
  }  
  
  /**
   * {@inheritdoc}
   */
  public function getDefaultVocabulary(){
    return $this->default_vocabulary;    
  }
  
  /**
   * {@inheritdoc}
   */
  public function saveTerm($term, $options, $parent = NULL){
    
  }
  
  /**
   * {@inheritdoc}
   */
  public function removeTerm($accession) {
    
  }
  
  /**
   * {@inheritdoc}
   */
  public function getURLPrefix() {
    $db = $this->loadIdSpace();
    if (!$db) {
      return NULL;
    }
    return $db['urlprefix'];    
  }
  
  /**
   * {@inheritdoc}
   */
  public function setURLPrefix($prefix) {
    
    // Don't set a value for an ID space that isn't valid.
    if (!$this->is_valid) {
      return False;
    }
    
    // Make sure the URL prefix is good.
    if (strlen($this->getURLPrefix()) > $this->db_def['fields']['urlprefix']['size']) {
      $this->logInvalidCondition('ChadoIdSpace: The URL prefix for the vocabulary ID Space must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->db_def['fields']['urlprefix']['size'],
            '@value' => $this->getName()]);
      return False;
    }
    
    // Update the record in the Chado `db` table.
    $query = $this->chado->update('1:db')
      ->fields(['urlprefix' => $prefix])
      ->condition('name', $this->getName(), '=');
    $num_updated = $query->execute();
    if ($num_updated != 1) {
      $this->logInvalidCondition('ChadoIdSpace: The URL prefix could not be updated for the vocabulary ID Space.');
      return False;
    }
    return True;
  }
  
  
  /**
   * {@inheritdoc}
   */
  public function getDescription() {  
    $db = $this->loadIdSpace();
    if (!$db) {
      return NULL;
    }
    return $db['description'];
    
  }
  
  /**
   * {@inheritdoc}
   */
  public function setDescription($description) {
    
    // Don't set a value for an ID space that isn't valid.
    if (!$this->is_valid) {
      return False;
    }
    

    // Make sure the description is not too long.
    if (strlen($this->getDescription()) > $this->db_def['fields']['description']['size']) {
      $this->messageLogger->error('ChadoIdSpace: The description for the vocabulary ID space must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->db_def['fields']['description']['size'],
           '@value' => $this->getName()]);
      return False;
    }
    
    // Update the record in the Chado `db` table.
    $query = $this->chado->update('1:db')
       ->fields(['description' => $description])
       ->condition('name', $this->getName(), '=');
    $num_updated = $query->execute();
    if ($num_updated != 1) {
      $this->logInvalidCondition('ChadoIdSpace: The description could not be updated for the vocabulary ID Space.');
      return False;      
    }
    return True;

  }
  
  /**
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name, $pluginId) {
    $retval = parent::setDefaultVocabulary($name, $pluginId);
    if ($retval === True) {
      $this->default_vocabulary = $name;
    }
    return $retval;
  }
}
