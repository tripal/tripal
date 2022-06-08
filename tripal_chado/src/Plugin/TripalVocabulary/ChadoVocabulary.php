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
   * Holds an instance of an ID Space Tripal Colllection Manager. 
   */
  protected $ids = NULL;
  
  
  /**
   * Creates this collection. This must only be called once on this new
   * collection instance that has just been created by its collection plugin
   * manager.
   */
  public function create(){     
    // Instantiate a BioConnection for Chado.
    $this->chado = \Drupal::service('tripal_chado.database');
    
    // We want to use the Chado schema as default in queries.
    $this->chado->useBioSchemaFor($this);    
    
    // Get the ID Space manager object for easy use elsewhere.
    $this->ids = \Drupal::service('tripal.collection_plugin_manager.idspace');
           
  }
  
  /**
   * Destroys this collection. This must only be called once when on this
   * existing collection that is being removed from its collection plugin
   * manager.
   */
  public function destroy(){
    
  }
  
  /**
   * Returns list of id space collection names that is contained in this vocabulary.
   *
   * @return array
   *   An array of id space collection name strings.
   */
  public function getIdSpaceNames(){
    
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
    
    // Get the collection for this ID.
    $id = $this->ids->loadCollection($idSpace, 'chado_id_space');
    
    
    // First make sure that the IDspace doesn't already exist.
    $query = $this->chado->select('db', 'db')
      ->condition('db.name', $id->getName(), '=')
      ->fields('db', ['name']);
    $result = $query->execute();
    $dbname = $result->fetchField();
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
  public function removeIdSpace($idspace){
    
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
  public function getTerms($name,$exact = True){
    
  }
  
  /**
   * Returns the URL of this vocabulary.
   *
   * @return string
   *   The URL.
   */
  public function getURL(){
    
  }
  
  /**
   * Sets the URL of this vocabulary to the given URL.
   *
   * @param string $url
   *   The URL.
   */
  public function setURL($url){
    
  }
  
  /**
   * Returns the description of this vocabulary.
   *
   * @return string
   *   The description.
   */
  public function getDescription(){
    
  }
  
  /**
   * Sets the description of this vocabulary to the given description.
   *
   * @param string $description
   *   The description.
   */
  public function setDescription($description){
    
  }

}
