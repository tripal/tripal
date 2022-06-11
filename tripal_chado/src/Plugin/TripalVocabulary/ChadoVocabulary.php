<?php

namespace Drupal\tripal_chado\Plugin\TripalVocabulary;

use Drupal\tripal\TripalVocabTerms\TripalVocabularyBase;

/**
 * Base class for tripal vocabulary plugins.
 * 
 *  @TripalVocabulary(
 *    id = "chado_vocabulary",
 *    label = @Translation("Vocabulary in Chado"),
 *  )
 */
class ChadoVocabulary extends TripalVocabularyBase {
  /**
   * Holds an instance of a BioDB connection to Chado.
   */
  protected $chado = NULL;
      
  /**
   * Holds the list of ID Spaces for this vocabulary.
   */
  protected $id_spaces = [];
  
  /**
   * The definition for the `db` table of Chado.
   */
  protected $db_def = NULL;
  
  /**
   * The definition for the `cv` table of Chado.
   */
  protected $cv_def = NULL;
  
  /**
   * An instance of the TripalLogger.
   */
  protected $messageLogger = NULL;
  
  /**
   * A simple boolean to prevent Chado queries if the vocabulary isn't valid.
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
    
    // Get the chado definition for the `cv` and `db` tables.
    $this->db_def = $this->chado->schema()->getTableDef('db', ['Source' => 'file']);
    $this->cv_def = $this->chado->schema()->getTableDef('cv', ['Source' => 'file']);
    
    // Make sure the name of this collection does not exceeed the allowed size in Chado.
    if (strlen($this->getName()) > $this->cv_def['fields']['name']['size']) {
      $this->messageLogger->error('ChadoVocabulary: The vocabulary name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->cv_def['fields']['name']['size'],
           '@value' => $this->getName()]);
          
    }
    $this->is_valid = True;
    
    return $this->is_valid;
  }
  
  /**
   * Creates this collection. This must only be called once on this new
   * collection instance that has just been created by its collection plugin
   * manager.
   */
  public function create(){
       
    // Check if the record already exists in the database, if it
    // doesn't then insert it.  We don't yet have the definition,
    // but that's okay, the name is all that isrequired to create 
    // a record in the `cv` table.
    $vocab = $this->loadVocab();
    if (!$vocab) {
      $query = $this->chado->insert('1:cv')
        ->fields(['name' => $this->getName()]);
      $query->execute();
    }
    
    // Set the ID spaces that already exist for this vocabulary in Chado.
    $this->id_spaces = $this->loadIDSpaces();
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
   * Loads an Vocbulary record from Chado.
   *
   * This function queries the `db` table of Chado to get the values
   * for the ID space.
   *
   * @return
   *   An associative array containing the columns of the `db1 table
   *   of Chado or NULL if the db could not be found.
   */
  protected function loadVocab() {
    
    // Get the Chado `db` record for this ID space.
    $query = $this->chado->select('1:cv', 'cv')
      ->condition('cv.name', $this->getName(), '=')
      ->fields('cv', ['name', 'definition']);
    $result = $query->execute();
    if ($result) {
      return $result->fetchAssoc();
    }
    return NULL;
  } 
  
  /**
   * Loads the ID Spaces that have been assigned to this vocabulary.
   */
  protected function loadIDSpaces() {
    // TODO we need to use the cv2db mview.
    return [];
  }
    
  /**
   * Returns list of id space collection names that is contained in this vocabulary.
   *
   * @return array
   *   An array of id space collection name strings.
   */
  public function getIdSpaceNames(){
    // Only return the names of the id spaces.
    return array_keys($this->id_spaces);
  }
  
  /**
   * Adds the id space with the given collection name to this vocabulary. The
   * given collection name must be a valid id space collection.
   *
   * @param string $idSpace
   *   The id space collection name.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function addIdSpace($idSpace){
    
    // Get the ID collection for this idSpace and save it for future
    // reference, then add the idSpace to our list.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $id = $idsmanager->loadCollection($idSpace, 'chado_id_space');
    if ($id) {
      $this->id_spaces[$idSpace] = $id;
      return TRUE;
    }
    
    // TODO: we need to update the cv2db mview.
    return FALSE;
  }
  
  /**
   * Removes the id space from this vocabulary with the given collection name.
   *
   * @param string $idSpace
   *   The id space collection name.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function removeIdSpace($idSpace){
    if (array_key_exists($idSpace, $this->id_spaces)) {
      unset($this->id_spaces[$idSpace]);
    }
    
    // TODO: how to handle deltion of an IDspace in Chado? 
    return TRUE;
  }
  
  /**
   * Returns the terms in this vocabulary whose names match the given name.
   * Matches can only be exact or a substring depending on the given flag. The
   * default is to only return exact matches.
   *
   * @param string $name
   *   The name.
   *
   * @param bool $exact
   *   True to only include exact matches else include all substring matches.
   *
   * @return array
   *   Array of matching Drupal\tripal\TripalVocabTerms\TripalTerm instances.
   */
  public function getTerms($name, $exact = True){
    
  }
  
  /**
   * Returns the URL of this vocabulary.
   *
   * @return string
   *   The URL.
   */
  public function getURL(){
    // Don't get a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }    
    
    // All of the ID spaces for the vocabulary should
    // have the same URL, so only query the first corresponding
    // `db` record to get the URL.
    $idSpace = $this->id_spaces[array_keys($this->id_spaces)[0]];
    $query = $this->chado->select('1:db', 'db')
      ->condition('db.name', $idSpace->getName(), '=')
      ->fields('db', ['url']);
    $db = $query->execute();
    if (!$db) {
      return NULL;
    }
    return $db->fetchAssoc()['url'];   
  }
  
  /**
   * Sets the URL of this vocabulary to the given URL.
   *
   * @param string $url
   *   The URL.
   *   
   * @return bool
   *   True if the value was set or false otherwise.
   */
  public function setURL($url){
    
    // Don't set a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return False;
    }
    
    // This value goes to the Chado `db.url` column, so check it's size
    // to make sure it doesn't exceed it.
    if (strlen($this->getURL()) > $this->db_def['fields']['url']['size']) {
      $this->messageLogger->error('ChadoVocabulary: The vocabulary name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->cv_def['fields']['name']['size'],
            '@value' => $this->getName()]);
      return False;
    }
    
    // Update the record in the Chado `db` table for the URL for all ID spaces.
    foreach ($this->id_spaces as $idSpace) {
      $query = $this->chado->update('1:db')
        ->fields(['url' => $url])
        ->condition('name', $idSpace->getName(), '=');
      $num_updated = $query->execute();
      if ($num_updated != 1) {
        $this->logInvalidCondition('ChadoVocabulary: The URL could not be updated for the vocabulary.');
        return False;        
      }
    }
    return True;
  }  
  
  /**
   * Returns the namespace of the vocabulary
   *
   * This should be identical to the name of the collection, and
   * therefore, there is no setter function.
   *
   * @return string $namespace
   *   The namespace of the vocabulary.
   */
  public function getNameSpace() {
    return $this->getName();
  }
  
  
  /**
   * Sets the label for the vocabulary.
   *
   * This is the human readable proper name of the vocabulary.
   *
   * Note that the name of the collection serves as the namespace of the vocabulary.
   *
   * @param string $label
   *   The name of the vocabulary.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function setLabel($label) {
    // Don't set a value for an vocubulary that isn't valid.
    if (!$this->is_valid) {
      return False;
    }
    
    // Note: there's no need to check the size of the label value
    // because the Chado column where this goes (cv.definition) is an
    // unlimited text field.    
    
    // Update the record in the Chado `cv` table.
    $query = $this->chado->update('1:cv')
      ->fields(['definition' => $label])
      ->condition('name', $this->getName(), '=');
    $num_updated = $query->execute();
    if ($num_updated != 1) {
      $this->logInvalidCondition('ChadoVocabulary: The label could not be updated for the vocabulary.');
      return False;      
    }
    return True;
  }
  
  
  /**
   * Returns the label of the vocabulary.
   *
   * This is the human readable proper name of the vocabulary.
   *
   * Note that the name of the collection serves as the namespace of the vocabulary.
   *
   * @return string $label
   *   The name of the vocabulary.
   */
  public function getLabel() {
    $cv = $this->loadVocab();
    if (!$cv) {
      return NULL;
    }
    return $cv['definition'];   
  }
}
