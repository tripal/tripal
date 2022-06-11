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
   * Tests if this collection is valid or not.
   *
   * @return bool
   *   True if this collection is valid or false otherwise.
   */
  public function isValid() {
    
    // Instantiate the TripalLogger
    $this->messageLogger = \Drupal::service('tripal.logger');
    
    // Instantiate a TripalDBX connection for Chado.
    $this->chado = \Drupal::service('tripal_chado.database');
    
    // Get the chado definition for the `db` table.
    $this->db_def = $this->chado->schema()->getTableDef('db', ['Source' => 'file']);
    
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
   * Creates this collection. This must only be called once on this new
   * collection instance that has just been created by its collection plugin
   * manager.
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
   * Destroys this collection. This must only be called once when on this
   * existing collection that is being removed from its collection plugin
   * manager.
   */
  public function destroy(){
    // There's no need to destroy anything.
  }  
  
  /**
   * Loads an ID space record from Chado.
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
   * Gets the parent of the given term. The given term must be a valid term for
   * this id space. If the given term is a root of this id space then NULL
   * is returned.
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $child
   *   The given term.
   *
   * @return Drupal\tripal\TripalVocabTerms\TripalTerm|NULL
   *   The parent term or NULL.
   */
  public function getParent($child){
    
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
    
  }
  
  /**
   * Gets the children terms of the given term. The given term must be a valid
   * term for this id space or NULL. If the given term is NULL then the root
   * children of this id space is returned.
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm|NULL $parent
   *   The given term or NULL.
   *
   * @return array
   *   An array of Drupal\tripal\TripalVocabTerms\TripalTerm children objects.
   */
  public function getChildren($parent = NULL){
    
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
    
  }
  
  /**
   * Returns the term in this id space with the given accession. If no such term
   * exists then NULL is returned.
   *
   * @param string $accession
   *   The accession.
   *
   * @return Drupal\tripal\TripalVocabTerms\TripalTerm|NULL
   *   The term or NULL.
   */
  public function getTerm($accession){
    
    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
  }
  
  /**
   * Returns the terms in this id space whose names match the given name with
   * the given options array.
   *
   * The given options array has the following recognized keys:
   *
   * exact(boolean): True to only include exact matches else false to include
   * all substring matches. The default is false.
   *
   * @param string $name
   *   The name.
   *
   * @param array $options
   *   The options array.
   *
   * @return array
   *   Array of matching Drupal\tripal\TripalVocabTerms\TripalTerm instances.
   */
  public function getTerms($name, $options){
    
  }  
  
  /**
   * Returns this id space's default vocabulary name or NULL if no default has
   * been set.
   *
   * @return string
   *   The vocabulary name.
   */
  public function getDefaultVocabulary(){
    return $this->default_vocabulary;    
  }
  
  /**
   * Saves the given term to this id space with the given options array and
   * optional parent. If the given parent is NULL and the given term is new then
   * it is added as a root term of this id space. If the given parent is NULL,
   * the given term already exists, and the appropriate option was given to
   * update the existing term's parent then it is moved to a root term of this
   * id space.
   *
   * The given options array has the following recognized keys:
   *
   * failIfExists(boolean): True to force this method to fail if the given term
   * already exists else false to update the term if it already exists. The
   * default is false.
   *
   * updateParent(boolean): True to update The given term's parent to the one
   * given or false to not update the existing term's parent. If the given term
   * is new this has no effect. The default is false.
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term.
   *
   * @param array $options
   *   The options array.
   *
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm|NULL $parent
   *   The parent term or NULL.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function saveTerm($term, $options, $parent = NULL){
    
  }
  
  /**
   * Removes the term with the given accession from this id space. All children
   * terms are also removed.
   * !!!WARNING!!!
   * If the removed term in this id space is referenced by entities this could
   * break data integrity. This method must be used with extreme caution!
   *
   * @param string $accession
   *   The accession.
   *
   * @return bool
   *   True if the term was removed or false otherwise.
   */
  public function removeTerm($accession) {
    
  }
  
  /**
   * Returns the URL prefix of this id space.
   *
   * @return string
   *   The URL prefix.
   */
  public function getURLPrefix() {
    $db = $this->loadIdSpace();
    if (!$db) {
      return NULL;
    }
    return $db['urlprefix'];    
  }
  
  /**
   * Sets the URL prefix of this id space to the given URL prefix.
   *
   * @param string $prefix
   *   The URL prefix.
   *   
   * @return bool
   *   True if the value was set or false otherwise.
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
   * Returns the description of this id space.
   *
   * @return string
   *   The description.
   */
  public function getDescription() {  
    $db = $this->loadIdSpace();
    if (!$db) {
      return NULL;
    }
    return $db['description'];
    
  }
  
  /**
   * Sets the description of this id space.
   *
   * @param string $description
   *   The description.
   *   
   * @return bool
   *   True if the value was set or false otherwise.
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
}
