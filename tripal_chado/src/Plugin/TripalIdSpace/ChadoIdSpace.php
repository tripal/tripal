<?php

namespace Drupal\tripal_chado\Plugin\TripalIdSpace;

use Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Chado Implementation of TripalIdSpaceBase
 *
 *  @TripalIdSpace(
 *    id = "chado_id_space",
 *    label = @Translation("Vocabulary IDSpace in Chado"),
 *  )
 */
class ChadoIdSpace extends TripalIdSpaceBase implements ContainerFactoryPluginInterface {

  /**
   * Holds the default vacabulary name.
   *
   * @var string
   */
  protected $default_vocabulary = NULL;

  /**
   * The definition for the `db` table of Chado.
   *
   * @var array
   */
  protected $db_def = NULL;


  /**
   * An instance of the TripalLogger.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $messageLogger = NULL;

  /**
   * The database connection for querying Chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $connection;

  /**
   * A simple boolean to prevent Chado queries if the ID space isn't valid.
   *
   * @var bool
   */
  protected $is_valid = False;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {

    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal.logger'),
      $container->get('tripal_chado.database')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, TripalLogger $logger, ChadoConnection $connection) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    $this->messageLogger = $logger;
    $this->connection = $connection;

    // Get the chado definition for the `db` table.
    $this->db_def = $this->connection->schema()->getTableDef('db', ['source' => 'file']);
  }


  /**
   * {@inheritdoc}
   */
  public function isValid() {

    // Make sure the name of this ID Space does not exceeed the allowed size in Chado.
    $name = $this->getName();

    if (!empty($name) AND (strlen($name) > $this->db_def['fields']['name']['size'])) {
      $this->messageLogger->error('ChadoIdSpace: The IdSpace name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->db_def['fields']['name']['size'],
           '@value' => $this->getName()]);
      $this->is_valid = FALSE;
      return FALSE;
    }

    $this->is_valid = TRUE;
    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  public function recordExists() {
    $db = $this->loadIdSpace();
    if ($db and $db['name'] == $this->getName()) {
      return True;
    }
    return False;
  }


  /**
   * {@inheritdoc}
   */
  public function createRecord() {

    // Check if the record already exists in the database, if it
    // doesn't then insert it.  We don't yet have the description,
    // URL prefix, etc but that's okay, the name is all that is
    // required to create a record in the `db` table.
    $db = $this->loadIdSpace();
    if (!$db) {
      $query = $this->connection->insert('1:db')
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
    $this->messageLogger->warning('The ChadoIdSpace::destroy() function is currently not implemented');
  }

  /**
   * Loads an ID Space record from Chado.
   *
   * This function queries the `db` table of Chado to get the values
   * for the ID space.
   *
   * @return array
   *   An associative array containing the columns of the `db1 table
   *   of Chado or NULL if the db could not be found.
   */
  protected function loadIdSpace() {

    // Get the Chado `db` record for this ID space.
    $query = $this->connection->select('1:db', 'db')
      ->condition('db.name', $this->getName(), '=')
      ->fields('db', ['name', 'url', 'urlprefix', 'description']);
    $result = $query->execute();
    if (!$result) {
      return NULL;
    }
    return $result->fetchAssoc();

  }

  /**
   * {@inheritdoc}
   */
  public function getParent($child){

    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }
    $this->messageLogger->warning('The ChadoIdSpace::getParent() function is currently not implemented');
  }

  /**
   * {@inheritdoc}
   */
  public function getChildren($parent = NULL){

    // Don't get values for an ID space that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }

    $terms = [];

    $cvterm = $this->getChadoCVTerm($parent);

    $query = $this->connection->select('1:cvterm_relationship', 'CVTR');
    $query->join('1:cvterm', 'CVTSUB', '"CVTR".subject_id = "CVTSUB".cvterm_id');
    $query->join('1:cvterm', 'CVTTYPE', '"CVTR".type_id = "CVTTYPE".cvterm_id');
    $query->join('1:dbxref', 'DBXSUB', '"CVTSUB".dbxref_id = "DBXSUB".dbxref_id');
    $query->join('1:dbxref', 'DBXTYPE', '"CVTTYPE".dbxref_id = "DBXTYPE".dbxref_id');
    $query->join('1:db', 'DBSUB', '"DBSUB".db_id = "DBXSUB".db_id');
    $query->join('1:cv', 'CVSUB', '"CVSUB".cv_id = "CVTSUB".cv_id');
    $query->join('1:db', 'DBTYPE', '"DBTYPE".db_id = "DBXTYPE".db_id');
    $query->join('1:cv', 'CVTYPE', '"CVTYPE".cv_id = "CVTTYPE".cv_id');
    $query->fields('CVTSUB', ['name'])
      ->fields('DBXSUB', ['accession'])
      ->fields('DBSUB', ['name'])
      ->fields('CVSUB', ['name'])
      ->fields('CVTTYPE', ['name'])
      ->fields('DBXTYPE', ['accession'])
      ->fields('DBTYPE', ['name'])
      ->fields('CVTYPE', ['name'])
      ->condition('CVTR.object_id', $cvterm->cvterm_id, '=');
    $children = $query->execute();
    while ($child = $children->fetchObject()) {
      $child_term = new TripalTerm([
        'name' => $child->name,
        'accession' => $child->accession,
        'idSpace' => $child->DBSUB_name,
        'vocabulary' => $child->CVSUB_name
      ]);
      $type_term = new TripalTerm([
        'name' => $child->CVTTYPE_name,
        'accession' => $child->DBXTYPE_accession,
        'idSpace' => $child->DBTYPE_name,
        'vocabulary' => $child->CVTYPE_name
      ]);
      $terms[] = [$child_term, $type_term];
    }
    return $terms;
  }

  /**
   * {@inheritdoc}
   */
  public function getTerm($accession, $options = []) {

    if (!$this->is_valid) {
      return NULL;
    }

    // Get the term record.
    $query = $this->connection->select('1:cvterm', 'CVT');
    $query->join('1:dbxref', 'DBX', '"CVT".dbxref_id = "DBX".dbxref_id');
    $query->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
    $query->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
    $query->fields('CVT', ['cvterm_id', 'name', 'definition', 'is_obsolete', 'is_relationshiptype']);
    $query->fields('CV', ['name']);
    $query->condition('DB.name', $this->getName(), '=');
    $query->condition('DBX.accession', $accession, '=');
    $cvterm = $query->execute()->fetchObject();
    // @debug print "CVTERM looked up by ChadoIdSpace->getTerm() in db: ".$this->connection->getSchemaName().". " . print_r($cvterm, TRUE) . "\n";

    if (!$cvterm) {
      return NULL;
    }
    $term =  new TripalTerm([
      'name' => $cvterm->name,
      'definition' => $cvterm->definition,
      'accession' => $accession,
      'idSpace' => $this->getName(),
      'vocabulary' => $cvterm->CV_name ? $cvterm->CV_name : $this->getDefaultVocabulary(),
      'is_obsolete' => $cvterm->is_obsolete == 1 ? True : False,
      'is_relationship_type' => $cvterm->is_relationshiptype == 1 ? True : False,
    ]);

    // Set the internal ID.
    $term->setInternalId($cvterm->cvterm_id);

    // Set the boolean values for the term.
    if ($cvterm->is_obsolete) {
      $term->isObsolete(True);
    }
    if ($cvterm->is_relationshiptype) {
      $term->isRelationshipType(True);
    }

    // Are there synonyms?
    if (!array_key_exists('includes', $options) or in_array('synonyms', $options['includes'])) {
      $query = $this->connection->select('1:cvtermsynonym', 'CVTS');
      $query->leftJoin('1:cvterm', 'CVT', '"CVT".cvterm_id = "CVTS".type_id');
      $query->leftJoin('1:dbxref', 'DBX', '"DBX".dbxref_id = "CVT".dbxref_id');
      $query->leftJoin('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
      $query->leftJoin('1:db', 'DB', '"DB".db_id = "DBX".db_id');
      $query->fields('CVTS', ['synonym', 'type_id']);
      $query->fields('CVT', ['name', 'definition']);
      $query->fields('DBX', ['accession']);
      $query->fields('CV', ['name']);
      $query->fields('DB', ['name']);
      $query->condition('CVTS.cvterm_id', $cvterm->cvterm_id, '=');
      $synonyms = $query->execute();
      while ($synonym = $synonyms->fetchObject()) {
        $type_term = NULL;
        if ($synonym->type_id) {
          $type_term = new TripalTerm([
            'name' => $synonym->name,
            'definition' => $synonym->definition,
            'accession' => $synonym->accession,
            'idSpace' => $synonym->DB_name,
            'vocabulary' => $synonym->CV_name,
          ]);
        }
        $term->addSynonym($synonym->synonym, $type_term);
      }
    }

    // Are there alt IDs?
    if (!array_key_exists('includes', $options) or in_array('altIds', $options['includes'])) {
      $query = $this->connection->select('1:cvterm_dbxref', 'CVTDBX');
      $query->join('1:dbxref', 'DBX', '"CVTDBX".dbxref_id = "DBX".dbxref_id');
      $query->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
      $query->fields('DBX', ['accession'])
        ->fields('DB', ['name'])
        ->condition('CVTDBX.cvterm_id', $cvterm->cvterm_id, '=');
      $alt_ids = $query->execute();
      while ($alt_id = $alt_ids->fetchObject()) {
        $term->addAltId($alt_id->name, $alt_id->accession);
      }
    }

    // Are there properties?
    if (!array_key_exists('includes', $options) or in_array('properties', $options['includes'])) {
      $query = $this->connection->select('1:cvtermprop', 'CVTP');
      $query->join('1:cvterm', 'CVT', '"CVTP".type_id = "CVT".cvterm_id');
      $query->join('1:dbxref', 'DBX', '"CVT".dbxref_id = "DBX".dbxref_id');
      $query->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
      $query->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
      $query->fields('CVT', ["name"])
        ->fields('CVTP', ['value'])
        ->fields('DBX', ['accession'])
        ->fields('DB', ['name'])
        ->fields('CV', ['name'])
        ->condition('CVTP.cvterm_id', $cvterm->cvterm_id, '=')
        ->orderBy('CVTP.type_id', 'ASC')
        ->orderBy('CVTP.rank', 'ASC');
      $properties = $query->execute();
      while ($property = $properties->fetchObject()) {
        $prop_term = new TripalTerm([
          'name' => $property->name,
          'accession' => $property->accession,
          'idSpace' => $property->DB_name,
          'vocabulary' => $property->CV_name
        ]);
        $term->addProperty($prop_term, $property->value);
      }
    }

    // Are there parents?
    if (!array_key_exists('includes', $options) or in_array('parents', $options['includes'])) {
      $query = $this->connection->select('1:cvterm_relationship', 'CVTR');
      $query->join('1:cvterm', 'CVTOBJ', '"CVTR".object_id = "CVTOBJ".cvterm_id');
      $query->join('1:cvterm', 'CVTTYPE', '"CVTR".type_id = "CVTTYPE".cvterm_id');
      $query->join('1:dbxref', 'DBXOBJ', '"CVTOBJ".dbxref_id = "DBXOBJ".dbxref_id');
      $query->join('1:dbxref', 'DBXTYPE', '"CVTTYPE".dbxref_id = "DBXTYPE".dbxref_id');
      $query->join('1:db', 'DBOBJ', '"DBOBJ".db_id = "DBXOBJ".db_id');
      $query->join('1:cv', 'CVOBJ', '"CVOBJ".cv_id = "CVTOBJ".cv_id');
      $query->join('1:db', 'DBTYPE', '"DBTYPE".db_id = "DBXTYPE".db_id');
      $query->join('1:cv', 'CVTYPE', '"CVTYPE".cv_id = "CVTTYPE".cv_id');
      $query->fields('CVTOBJ', ['name'])
        ->fields('DBXOBJ', ['accession'])
        ->fields('DBOBJ', ['name'])
        ->fields('CVOBJ', ['name'])
        ->fields('CVTTYPE', ['name'])
        ->fields('DBXTYPE', ['accession'])
        ->fields('DBTYPE', ['name'])
        ->fields('CVTYPE', ['name'])
        ->condition('CVTR.subject_id', $cvterm->cvterm_id, '=');
      $parents = $query->execute();
      while ($parent = $parents->fetchObject()) {
        $parent_term = new TripalTerm([
          'name' => $parent->name,
          'accession' => $parent->accession,
          'idSpace' => $parent->DBOBJ_name,
          'vocabulary' => $parent->CVOBJ_name
        ]);
        $type_term = new TripalTerm([
          'name' => $parent->CVTTYPE_name,
          'accession' => $parent->DBXTYPE_accession,
          'idSpace' => $parent->DBTYPE_name,
          'vocabulary' => $parent->CVTYPE_name
        ]);
        $term->addParent($parent_term, $type_term);
      }
    }

    return $term;
  }

  /**
   * {@inheritdoc}
   */
  public function getTerms($name, $options = []) {

    // The list of terms to return
    $terms = [];

    // Build the query for matching via the `cvterm.name` column.
    $query1 = $this->connection->select('1:cvterm', 'CVT');
    $query1->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
    $query1->join('1:dbxref', 'DBX', '"DBX".dbxref_id = "CVT".dbxref_id');
    $query1->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
    $query1->fields('CVT', ['name', 'definition', 'cvterm_id']);
    $query1->fields('DBX', ['accession']);
    $query1->fields('CV', ['name']);
    $query1->condition('DB.name', $this->getName(), '=');
    if (array_key_exists('exact', $options) and $options['exact'] === True) {
      $query1->condition('CVT.name', $name, '=');
    }
    else {
      $query1->condition('CVT.name', $name . '%', 'LIKE');
    }
    $results = $query1->execute();
    while ($cvterm = $results->fetchObject()) {
      $term = new TripalTerm([
        'name' => $cvterm->name,
        'idSpace' => $this->getName(),
        'vocabulary' => $cvterm->CV_name,
        'definition' => $cvterm->definition,
        'accession' => $cvterm->accession
      ]);
      $terms[$cvterm->name][$term->getTermId()] = $term;
    }

    // Build the query for matching via the `cvtermsynonym.synonym` column.
    $query2 = $this->connection->select('1:cvtermsynonym', 'CS');
    $query2->join('1:cvterm', 'CVT', '"CVT".cvterm_id = "CS".cvterm_id');
    $query2->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
    $query2->join('1:dbxref', 'DBX', '"DBX".dbxref_id = "CVT".dbxref_id');
    $query2->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
    $query2->fields('CVT', ['name', 'definition', 'cvterm_id']);
    $query2->fields('DBX', ['accession']);
    $query2->fields('CV', ['name']);
    $query2->fields('CS', ['synonym']);
    $query2->condition('DB.name', $this->getName(), '=');
    if (array_key_exists('exact', $options) and $options['exact'] === True) {
      $query2->condition('CS.synonym', $name, '=');
    }
    else {
      $query2->condition('CS.synonym', $name . '%', 'LIKE');
    }
    $results = $query2->execute();
    while ($cvterm = $results->fetchObject()) {
      $term = new TripalTerm([
        'name' => $cvterm->name,
        'idSpace' => $this->getName(),
        'vocabulary' => $cvterm->CV_name,
        'definition' => $cvterm->definition,
        'accession' => $cvterm->accession
      ]);
      $terms[$cvterm->synonym][$term->getTermId()] = $term;
    }
    return $terms;
  }

  /**
   * {@inheritdoc}
   */
  public function getDefaultVocabulary(){
    return $this->getDefaultVocabCache();
  }

  /**
   * {@inheritdoc}
   */
  public function saveTerm($term, $options = []) {

    // Don't save terms that aren't valid
    if (!$term->isValid()) {
      $this->messageLogger->error(t('ChadoIdSpace::saveTerm(). The term, "@term" is not valid and cannot be saved. It must include a name, accession, IdSpace and vocabulary.',
          ['@term' => $term->getIdSpace() . ':' . $term->getAccession()]));
      return False;
    }

    // Make sure the idSpace matches.
    if ($this->getName() != $term->getIdSpace()) {
      $this->messageLogger->error(t('ChadoIdSpace::saveTerm(). The term, "@term", does not have the same ID space as this one.',
          ['@term' => $term->getIdSpace() . ':' . $term->getAccession()]));
      return False;
    }

    // Get easy to use boolean variables.
    $fail_if_exists = False;
    if (array_key_exists('failIfExists', $options)) {
      $fail_if_exists = $options['failIfExists'];
    }

    // Does the term exist? If not do an insert, if so, do an update.
    $cvterm = $this->getChadoCVTerm($term);
    if (!$cvterm) {
      if (!$this->insertTerm($term, $options)) {
        return False;
      }
    }
    if ($cvterm and $fail_if_exists) {
      return False;
    }
    if ($cvterm and !$fail_if_exists) {
      if (!$this->updateTerm($term, $cvterm, $options)) {
        return False;
      }
    }

    // Set the internal ID.
    $cvterm = $this->getChadoCVTerm($term);
    $term->setInternalId($cvterm->cvterm_id);

    return True;
  }

  /**
   * Retrieve a record from the Chado cv table.
   *
   * @param TripalTerm $term
   *   The TripalTerm object to save.
   *
   * @return object
   *   The cv record in object form.
   */
  protected function getChadoCV(TripalTerm $term) {

    $result = $this->connection->select('1:cv', 'CV')
      ->fields('CV', ['cv_id', 'name', 'definition'])
      ->condition('name', $term->getVocabulary(), '=')
      ->execute();
    if(!$result) {
      return NULL;
    }
    return $result->fetchObject();
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

    $result = $this->connection->select('1:db', 'DB')
      ->fields('DB', ['db_id', 'name', 'description'])
      ->condition('name', $term->getIdSpace(), '=')
      ->execute();
    if(!$result) {
      return NULL;
    }
    return $result->fetchObject();
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

    $db = $this->getChadoDB($term);
    $result = $this->connection->select('1:dbxref', 'DBX')
      ->fields('DBX', ['dbxref_id', 'db_id', 'accession', 'version' ,'description'])
      ->condition('db_id', $db->db_id, '=')
      ->condition('accession', $term->getAccession(), '=')
      ->execute();
    if (!$result) {
      return NULL;
    }
    return $result->fetchObject();
  }

  /**
   * Retreives a record from the Cahdo dbxref table using the term ID.
   *
   * @param string $term_id
   *   The term ID (e.g. GO:0044708).
   */
  protected function getChadoDBXrefbyTermID(string $term_id) {

    list($db, $accession) = explode(':', $term_id);
    $query = $this->connection->select('1:dbxref', 'DBX');
    $query->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
    $result = $query->fields('DBX', ['dbxref_id', 'db_id', 'accession', 'version' ,'description'])
      ->condition('DB.name', $db, '=')
      ->condition('DBX.accession', $accession, '=')
      ->execute();
    if(!$result) {
      return NULL;
    }
    return $result->fetchObject();
  }

  /**
   * Adds a record from the Cahdo dbxref table using the term ID.
   *
   * The database record must already exist.
   *
   * @param string $term_id
   *   The term ID (e.g. GO:0044708).
   *
   * @return object|NULL
   *   The dbxref Object.
   */
  protected function insertChadoDBxrefbyTermID(string $term_id) {

    list($db, $accession) = explode(':', $term_id);
    $result = $this->connection->select('1:db', 'DB')
      ->fields('DB', ['db_id'])
      ->condition('name', $db, '=')
      ->execute();
    if (!$result) {
      return NULL;
    }

    $db_id = $result->fetchField();
    $this->connection->insert('1:dbxref')
      ->fields([
        'db_id' => $db_id,
        'accession' => $accession,
      ])
      ->execute();
    return $this->getChadoDBXrefbyTermID($term_id);
  }

  /**
   * Retrieve a record from the Chado cvterm table.
   *
   * This function uses the db.name (IdSpace), cv.name (vocabulary)
   * and dbxref.accession values to uniquely identify a term in Chado.
   *
   * @param TripalTerm $term
   *   The TripalTerm object to save.
   * @return object
   *   The cvterm record in object form.
   */
  protected function getChadoCVTerm(TripalTerm $term) {

    $query = $this->connection->select('1:cvterm', 'CVT');
    $query->join('1:dbxref', 'DBX', '"DBX".dbxref_id = "CVT".dbxref_id');
    $query->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
    $query->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
    $query->fields('CVT', ['cv_id', 'cvterm_id', 'definition', 'is_obsolete', 'is_relationshiptype'])
      ->fields('DBX', ['dbxref_id', 'accession'])
      ->condition('DB.name', $term->getIdSpace(), '=')
      ->condition('CV.name', $term->getVocabulary(), '=')
      ->condition('DBX.accession', $term->getAccession(), '=');
    $result = $query->execute();
    if (!$result) {
      return NULL;
    }
    return $result->fetchObject();
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
   * @param array $options
   *   The options passed to the saveTerm() function.
   *
   * @return boolean
   *   True if the insert was successful, false otherwise.
   */
  protected function insertTerm(TripalTerm $term, $options) {

    try {
      // The CV and DB records should already exist.
      $cv = $this->getChadoCV($term);
      $db = $this->getChadoDB($term);

      // The DBXref may already exist even though the
      // term does not.  So, check first and if not then
      // add it.
      $dbxref = $this->getChadoDBXref($term);
      if (!$dbxref) {

        $this->connection->insert('1:dbxref')
          ->fields([
            'db_id' => $db->db_id,
            'accession' => $term->getAccession(),
          ])
        ->execute();
        $dbxref = $this->getChadoDBXref($term);
      }

      // Add the CVterm record.
      $this->connection->insert('1:cvterm')
        ->fields([
          'cv_id' => $cv->cv_id,
          'dbxref_id' => $dbxref->dbxref_id,
          'name' => $term->getName(),
          'definition' => $term->getDefinition(),
          'is_obsolete' => $term->isObsolete() ? 1 : 0,
          'is_relationshiptype' => $term->isRelationshipType() ? 1 : 0,
        ])
        ->execute();
      $cvterm = $this->getChadoCVTerm($term);
      if (!$cvterm) {
        return False;
      }

      // Now save the term attributes.
      if (!$this->saveTermAttributes($term, $cvterm, $options)) {
        return False;
      }

    }
    catch (Exception $e) {
      $this->messageLogger->error('ChadoIdSpace::insertTerm(). could not insert the cvterm record: @message',
          ['@message' => $e->getMessage()]);
      return False;
    }
    return True;
  }

  /**
   *
   * @param TripalTerm $term
   * @param object $cvterm
   * @param array $options
   * @return bool
   */
  protected function saveTermAttributes(TripalTerm $term, object $cvterm, array $options) : bool {

    $update_parent = False;
    if (array_key_exists('updateParent', $options)) {
      $update_parent = $options['updateParent'];
    }

    // Add in synonyms.ount($syns->chado->delete('1:cvtermsynonym')->condition('cvterm_id', $cvterm->cvterm_id)->execute();
    $this->connection->delete('1:cvtermsynonym')->condition('cvterm_id', $cvterm->cvterm_id)->execute();
    foreach ($term->getSynonyms() as $synonym => $type_term) {
      $query = $this->connection->insert('1:cvtermsynonym');
      if ($type_term) {
        $type_cvterm = $this->getChadoCVTerm($type_term);
        $query->fields([
          'cvterm_id' => $cvterm->cvterm_id,
          'synonym' => $synonym,
          'type_id' => $type_cvterm->cvterm_id,
        ]);
      }
      else {
        $query->fields([
          'cvterm_id' => $cvterm->cvterm_id,
          'synonym' => $synonym,
        ]);
      }
      $query->execute();
    }

    // Add in the properties
    $this->connection->delete('1:cvtermprop')->condition('cvterm_id', $cvterm->cvterm_id)->execute();
    foreach ($term->getProperties() as $term_id => $properties) {
      foreach  ($properties as $rank => $tuple) {
        $type_term = $this->getChadoCVTerm($tuple[0]);
        if (!$type_term) {
          return False;
        }
        $value = $tuple[1];
        $this->connection->insert('1:cvtermprop')
          ->fields([
            'cvterm_id' => $cvterm->cvterm_id,
            'type_id' => $type_term->cvterm_id,
            'value' => $value,
            'rank' => $rank
          ])
          ->execute();
      }
    }

    // Add in the alternate IDs.
    $this->connection->delete('1:cvterm_dbxref')->condition('cvterm_id', $cvterm->cvterm_id)->execute();
    foreach ($term->getAltIds() as $term_id) {
      $alt_dbxref = $this->getChadoDBXrefbyTermID($term_id);
      if (!$alt_dbxref) {
        $alt_dbxref = $this->insertChadoDBxrefbyTermID($term_id);
        if (!$alt_dbxref) {
          return False;
        }
      }
      $this->connection->insert('1:cvterm_dbxref')
        ->fields([
          'cvterm_id' => $cvterm->cvterm_id,
          'dbxref_id' => $alt_dbxref->dbxref_id,
        ])
        ->execute();
    }

    // Add in the parents.
    $this->connection->delete('1:cvterm_relationship')->condition('subject_id', $cvterm->cvterm_id)->execute();
    foreach ($term->getParents() as $term_id => $tuple) {
      $parent_term = $tuple[0];
      $rel_term = $tuple[1];
      $parent_term = $this->getChadoCVTerm($parent_term);
      if (!$parent_term) {
        return False;
      }
      $rel_cvterm = $this->getChadoCVTerm($rel_term);
      if (!$rel_cvterm) {
        return False;
      }
      $this->connection->insert('1:cvterm_relationship')
        ->fields([
          'type_id' => $rel_cvterm->cvterm_id,
          'subject_id' => $cvterm->cvterm_id,
          'object_id' => $parent_term->cvterm_id,
        ])
        ->execute();

      // Update the parent if requested to do so.
      if ($update_parent) {
        $parent_idSpace = $parent_term->getIdSpaceObject();
        $parent_idSpace->saveTerm($parent_term, ['failIfExists' => $options['failIfExists']]);
      }
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
   * @param object $cvterm
   *   The record object for the term to update from the Chado cvterm table.
   * @param array $options
   *   The options passed to the saveTerm() function.
   *
   * @return boolean
   *   True if the update was successful, false otherwise.
   */
  protected function updateTerm(TripalTerm $term, object &$cvterm, array $options) {

    try {
      $this->connection->update('1:cvterm')
        ->fields([
          'name' => $term->getName(),
          'definition' => $term->getDefinition(),
          'is_obsolete' => $term->isObsolete() ? 1 : 0,
          'is_relationshiptype' => $term->isRelationshipType() ? 1 : 0,
        ])
        ->condition('cvterm_id', $cvterm->cvterm_id)
        ->execute();
      $cvterm = $this->getChadoCVTerm($term);

      $this->saveTermAttributes($term, $cvterm, $options);
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
    $this->messageLogger->warning('The ChadoIdSpace::removeTerm() function is currently not implemented');
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
    if (empty($prefix)) {
      $this->messageLogger->error('ChadoIdSpace: No URL prefix for the vocabulary ID Space was provided when setURLPrefix() was called.');
      return False;
    }
    if (strlen($prefix) > $this->db_def['fields']['urlprefix']['size']) {
      $this->messageLogger->error('ChadoIdSpace: The URL prefix for the vocabulary ID Space must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->db_def['fields']['urlprefix']['size'],
            '@value' => $prefix]);
      return False;
    }

    // Update the record in the Chado `db` table.
    $query = $this->connection->update('1:db')
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
    if (empty($description)) {
      $this->messageLogger->error('ChadoIdSpace: You must provide a description when calling setDescription().',
          ['@size' => $this->db_def['fields']['description']['size'],
           '@value' => $description]);
      return False;
    }
    if (strlen($description) > $this->db_def['fields']['description']['size']) {
      $this->messageLogger->error('ChadoIdSpace: The description for the vocabulary ID space must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->db_def['fields']['description']['size'],
           '@value' => $description]);
      return False;
    }

    // Update the record in the Chado `db` table.
    $query = $this->connection->update('1:db')
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
   * Retrieves from the Drupal cache the default vocabulary for this space.
   *
   * @return string
   *   The deffault vocabulary name.
   */
  protected function getDefaultVocabCache() {
    $cid = 'chado_id_space_' . $this->getName() . '_default_vocab';
    $default_vocab = '';
    if ($cache = \Drupal::cache()->get($cid)) {
      $default_vocab = $cache->data;
    }
    else {
      // If we couldn't find the cached vocabulary name then
      // we should do a lookup. The cache must have been cleared.
      // We'll pick the first entered cv record as the default.
      $query = $this->connection->select('1:db2cv_mview', 'D2C');
      $query->fields('D2C', ['cvname']);
      $query->condition('dbname', $this->getName());
      $query->orderBy('cv_id');
      $results = $query->execute();
      $result = $results->fetchObject();
      if ($result) {
        $default_vocab = $result->cvname;
        $this->setDefaultVocabCache($default_vocab);
      }
    }

    return $default_vocab;
  }

  /**
   * Sets in the Drupal cache the default vocabulary.
   *
   * @param string $vocabulary
   *   The default vocabulary name.
   */
  protected function setDefaultVocabCache($vocabulary) {
    $cid = 'chado_id_space_' . $this->getName() . '_default_vocab';
    \Drupal::cache()->set($cid, $vocabulary);
  }

  /**
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name) {
    $retval = parent::setDefaultVocabulary($name);
    if ($retval === True) {
      $this->default_vocabulary = $name;
    }
    $this->setDefaultVocabCache($name);
    return $retval;
  }
}
