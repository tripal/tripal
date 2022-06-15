<?php

namespace Drupal\tripal_chado\Plugin\TripalIdSpace;

use Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;

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
  public function getTerm($accession) {
    
    if (!$this->is_valid) {
      return NULL;
    }
    
    // Get the term record.
    $cvterm = $this->chado->select('1:cvterm', 'CVT')
      ->join('1:dbxterm', 'DBX', 'CVT.dbxref_id = DBX.dbxref_id')
      ->join('1:cv', 'CV', 'CV.cv_id = CVT.cv_id')
      ->join('1:db', 'DB', 'DB.db_id = DBX.db_id')
      ->fields('CVT', ['cvterm_id', 'name', 'definition', 'is_obsolete', 'is_relationship_type'])
      ->condition('DB.name', $this->getName(), '=')
      ->condition('CV.name', $this->getDefaultVocab(), '=')
      ->condition('DBX.accession', $accession, '=')
      ->execute()
      ->fetchObject();
    if (!$cvterm) {
      return NULL;
    }
    $term =  new TripalTerm([
      'name' => $cvterm->name,
      'definition' => $cvterm->definition,
      'accession' => $accession,
      'idSpace' => $this->getName(),
      'vocabulary' => $this->getDefaultVocabulary()
    ]);
    
    // Set the boolean values for the term.
    if ($cvterm->is_obsolete) {
      $term->isObsolete(True);
    }
    if ($cvterm->is_relationship_type) {
      $term->isRelationshipType(True);
    }
    
    // Are there synonyms?
    $synonyms = $this->chado->select('1:cvtermsynonym', 'CVTS')
      ->fields('CVTS', ['synonym', 'type_id'])
      ->condition('CVTS.cvterm_id', $cvterm->cvterm_id, '=')
      ->execute();
    while ($synonym = $synonyms->fetchObject()) {
      $term->addSynonym($synonym->synonym);
    }
    
    // Are there alt IDs?
    $alt_ids = $this->chado->select('1:cvterm_dbxref', 'CVTDBX')
      ->join('1:dbxref', 'DBX', 'CVTDBX.dbxref_id = DBX.dbxref_id')
      ->join('1:db', 'DB', 'DB.db_id = DBX.db_id')
      ->fields('DBX', ['accession'])
      ->fields('DB', ['name'])
      ->condition('CVTDBX.cvterm_id', $cvterm->cvterm_id, '=')
      ->execute();          
    while ($alt_id = $alt_ids->fetchObject()) {
      $term->addAltId($alt_id->name, $alt_id->accession);
    }
    
    // Are there properties?
    $properties = $this->chado->select('1:cvtermprop', 'CVTP')    
      ->join('1:cvterm', 'CVT', 'CVTP.type_id = CVT.cvterm_id')
      ->join('1:dbxref', 'DBX', 'CVT.dbxref_id = DBX.dbxref_id')
      ->join('1:db', 'DB', 'DB.db_id = DBX.db_id')
      ->join('1:cv', 'CV', 'CV.cv_id = CVT.cv_id')
      ->fields('CVT', ['name as cvname'])
      ->fields('CVTP', ['value'])
      ->fields('DBX', ['accession'])
      ->fields('DB', ['name as dbname'])
      ->condition('CVTP.cvterm_id', $cvterm->cvterm_id, '=')
      ->orderBy('CVTP.type_id', 'ASC')
      ->orderBy('CVTP.rank', 'ASC')
      ->execute();
      while ($property = $properties->fetchObject()) {
        $prop_term = new TripalTerm([
          'name' => $property->name,
          'accession' => $property->accession,
          'idSpace' => $property->dbname,
          'vocabulary' => $property->cvname
        ]);
        $term->addProperty($prop_term, $property->value);
      }
      
      // Are there parents?
        
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
  public function saveTerm($term, $options) {
    $accession = $term->getAccession();
    
    $fail_if_exists = False;
    $update_parent = False;
    if (array_key_exists('failIfExists', $options)) {
      $fail_if_exists = $options['failIfExists'];
    }
    if (array_key_exists('updateParent', $options)) {
      $update_parent = $options['updateParent'];
    }
    
    $term_exists = $this->getTerm($accession);
    if (!$term_exists) {
      $this->insertTerm($term);
    }
    if ($term_exists and $fail_if_exists) {
      return False;
    }
    if ($term_exists and !$fail_if_exists) {
      $this->updateTerm($term);
    }
    
    
    if ($update_parent) {
    }       
  }
  
  /**
   * Retrieve a record from the Chado cv table.
   * 
   * @param TripalTerm $term
   *   The TripalTerm object to save.
   *   
   * @return unknown
   *   The cv record in object form.
   */
  protected function getChadoCV(TripalTerm $term) {
    return $this->chado->select('1:cv', 'CV')
      ->fields('CV', ['cv_id', 'name', 'definition'])
      ->condition('name', $term->getVocabulary(), '=')
      ->execute()
      ->fetchObject();    
  }
  
  /**
   * Retrieve a record from the Chado db table.
   * 
   * @param TripalTerm $term
   *   The TripalTerm object to save. 
   * @return object
   *   The db record in object form.
   */
  protected function getChadoDB(TripalTerm $term) {
    return $this->chado->select('1:db', 'DB')
      ->fields('DB', ['db_id', 'name', 'description'])
      ->condition('name', $term->getIdSpace(), '=')
      ->execute()
      ->fetchObject();    
  }
  
  /**
   * Retrieve a record from the Chado dbxref table.
   * 
   * @param TripalTerm $term
   *   The TripalTerm object to save. 
   * @return object
   *   The dbxref record in object form.
   */
  protected function getChadoDBXref(TripalTerm $term) {
    return $this->chado->select('1:dbxref', 'DBX')
      ->fields('DBX', ['dbxref_id'])
      ->condition('db_id', $this->_getChadoDBId($term), '=')
      ->condition('accession', $term->getAccession(), '=')
      ->execute()
      ->fetchObject();
  }
  
  /**
   * Retrieve a record from the Chado cvterm table. 
   * 
   * @param TripalTerm $term
   *   The TripalTerm object to save.
   * @return object
   *   The cvterm record in object form.
   */
  protected function getChadoCVTerm(TripalTerm $term) {
    return $this->chado->select('1:cvterm', 'CVT')
      ->join('1:dbxterm', 'DBX', 'CVT.dbxref_id = DBX.dbxref_id')
      ->join('1:cv', 'CV', 'CV.cv_id = CVT.cv_id')
      ->join('1:db', 'DB', 'DB.db_id = DBX.db_id')
      ->fields('CVT', ['cv_id', 'name', 'definition', 'cv_id'])
      ->fields('DBX', ['accession', 'dbxref_id', 'db_id'])
      ->condition('DB.name', $term->getIdSpace(), '=')
      ->condition('CV.name', $term->getVocabulary(), '=')
      ->condition('DBX.accession', $term->getAccession(), '=')
      ->execute()
      ->fetchObject();
  }

  
  /**
   * Inserts a new term into Chado.
   * 
   * The term should be checked that it does not exist 
   * prior to calling this function.
   *  
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object to update
   *   
   * @return boolean
   *   True if the insert was successful, false otherwise.
   */
  protected function insertTerm(TripalTerm $term) {    
    try {
      $cv = $this->getChadoCV($term);
      $dbxref = $this->getChadoDBXref($term);
      if (!$dbxref) {
        $db = $this->getChadoDB($term);
        $this->chado->insert('1:dbxref')
          ->fields([
            'db_id' => $db->db_id,
            'accession' => $term->getAccession(),
          ])
        ->execute();
        $dbxref = $this->getChadoDBXref($term);
      }
      $this->chado->insert('1:cvterm')
        ->fields([
          'cv_id' => $cv->cv__id,
          'dbxref_id' => $dbxref->dbxref_id,
          'name' => $term->getName(),
          'definition' => $term->getDefinition(),
        ])
        ->execute();
    } 
    catch (Exception $e) {
      $this->messageLogger->error('ChadoIdSpace::insertTerm(). could not insert the cvterm record: @message', 
          ['@message' => $e->getMessage()]);
      return False;
    }
    return True;
  }
  
  /**
   * Updates an existing term in Chado.
   * 
   * The term should be checked that it already exists 
   * prior to execution of this function.
   * 
   * @param Drupal\tripal\TripalVocabTerms\TripalTerm $term
   *   The term object to update
   *   
   * @return boolean
   *   True if the update was successful, false otherwise.
   */
  protected function updateTerm($term) {
    $definition = $term->getDefinition();
    $accession = $term->getAccession();
    $name = $term->getName();
    
    try {
      $cv_id = $this->chado->select('1:cv', 'CV')
        ->fields('CV', ['cv_id'])
        ->condition('name', $this->getDefaultVocabulary(), '=')
        ->execute()
        ->fetchField();
      $cvterm_id = $this->chado->select('1:cvterm', 'CVT')
        ->fields('CVT', ['cvterm_id'])
        ->condition('cv_id', $cv_id, '=')
        ->condition('name', $name, '=')
        ->execute()
        ->fetchField();
      $this->chado->insert('1:cvterm')
        ->fields([
          'name' => $name,
          'definition' => $definition,
        ])
        ->condition('cvterm_id', $cvterm_id, '=')
        ->execute();
    }
    catch (Exception $e) {
      $this->messageLogger->error('ChadoIdSpace: could not update the cvterm record: @message',
          ['@message' => $e->getMessage()]);
      return False;
    }
    return True;
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
      $this->messageLogger->error('ChadoIdSpace: The URL prefix for the vocabulary ID Space must not be longer than @size characters. ' +
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
      $this->messageLogger->error('ChadoIdSpace: The URL prefix could not be updated for the vocabulary ID Space.');
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
      $this->messageLogger->error('ChadoIdSpace: The description could not be updated for the vocabulary ID Space.');
      return False;      
    }
    return True;

  }
  
  /**
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name) {
    $retval = parent::setDefaultVocabulary($name);
    if ($retval === True) {
      $this->default_vocabulary = $name;
    }
    return $retval;
  }
}
