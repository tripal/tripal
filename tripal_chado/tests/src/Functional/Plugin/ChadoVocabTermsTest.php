<?php 

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Database\Database;
use Drupal\Core\Test\FunctionalTestSetupTrait;
use Drupal\tripal\TripalVocabTerms\TripalTerm;



/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoVocabTerms
 */
class ChadoVocabTermsTest extends ChadoTestBrowserBase {

  /**
   * A helper function to retrieve a Chado cv record.
   *
   * @param $dbname
   *   The name of the database to lookup.
   *
   * @return A database query result.
   */
  protected function getCV($cvname) {
    
    $query = $this->chado->select('1:cv', 'cv')
    ->condition('cv.name', $cvname, '=')
    ->fields('cv', ['name', 'definition']);
    $result = $query->execute();
    if (!$result) {
      return [];
    }
    return $result->fetchAssoc();
  }
  
  /**
   * A helper function to retrieve a Chado db record.
   * 
   * @param string $dbname
   *   The name of the database to lookup.
   *    
   * @return A database query result. 
   */
  protected function getDB($dbname) {
    
    $query = $this->chado->select('1:db', 'db')
      ->condition('db.name', $dbname, '=')
      ->fields('db', ['name', 'urlprefix', 'url', 'description']);
    $result = $query->execute();
    if (!$result) {
      return [];
    }    
    return $result->fetchAssoc();
  }
  
  /**
   * A helper function to retrieve a Chado cvterm record.
   * 
   * @param string $cvname
   * @param string $cvterm_name
   */
  protected function getCVterm($cvname, $cvterm_name) {
    
  }
  
  /**
   * A helper function to update a Chado db record.
   *
   * @param $dbname
   *   The name of the database to lookup.
   *
   * @return A database query result.
   */
  protected function updateRecord($table, $name, $field, $value) {
    $query = $this->chado->update('1:' . $table)
      ->condition('name', $name, '=')
      ->fields([$field => $value]);
    return $query->execute();    
  }
  
  /**
   * A helper function to delete a Chado db record.
   *
   * @param $dbname
   *   The name of the database to lookup.
   *   
   * @return The number of records deleted.
   */
  protected function cleanDB($dbname) {
    $query = $this->chado->delete('1:db')
      ->condition('name', $dbname, '=');
    return $query->execute();    
  }
  
  /**
   * A helper function to delete a Chado cv record.
   *
   * @param $dbname
   *   The name of the database to lookup.
   *
   * @return The number of records deleted.
   */
  protected function cleanCV($cvname) {
    $query = $this->chado->delete('1:cv', 'cv')
      ->condition('name', $cvname, '=');
    return $query->execute();
  }
  
  /**
   * Tests the ChadoIdSpace Class
   *
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testTripalVocabularyClasses() {
    
    // Create Collection managers.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');    
    
    // These are the values we'll use for the ID space and vocaublary.
    $GO_idspace = 'GO';
    $GO_cc_namespace = 'cellular_component';
    $GO_bp_namespace = 'biological_process';
    $GO_mf_namespace = 'molecular_function';
    $GO_description = "The Gene Ontology (GO) knowledgebase is the world’s largest source of information on the functions of genes";
    $GO_cc_label = 'Gene Ontology Cellular Component Vocabulary';
    $GO_bp_label = 'Gene Ontology Biological Process Vocabulary';
    $GO_mf_label = 'Gene Ontology Molecular Function Vocabulary';
    $GO_urlprefix = "http://amigo.geneontology.org/amigo/term/{db}:{accession}";
    $GO_url = 'http://geneontology.org/';
    
    // 
    // Testing ChadoIdSpace Functionality
    // 
    
    // Make sure the IDspace doesn't yet exist.
    $db = $this->getDB($GO_idspace);
    $this->assertEmpty($db, 'The Chado db has a conflicting record.');    
    
    // Create the ID space object and make sure a Chado record got created.
    $GO = $idsmanager->createCollection($GO_idspace, "chado_id_space");
    $db = $this->getDB($GO_idspace);
    $this->assertTrue($db['name'] == $GO_idspace, 'The name was not set correctly by the ChadoIdSpace object.');
    $this->assertEmpty($db['description'], 'The description should not be set by the ChadoIdSpace object just yet.');
    $this->assertEmpty($db['urlprefix'], 'The URL prefix should not be set by the ChadoIdSpace object just yet.');
    $this->assertEmpty($db['url'], 'The URL should not be set by the ChadoIdSpace object just yet.');

    // Set the description to make sure it gets set in Chado.
    $GO->setDescription($GO_description);
    $db = $this->getDB($GO_idspace);
    $this->assertTrue($db['name'] == $GO_idspace, 'The name was not set correctly after updating by the ChadoIdSpace object.');
    $this->assertTrue($db['description'] == $GO_description, 'The description was not set correctly by the ChadoIdSpace object.');
    $this->assertEmpty($db['urlprefix'], 'The URL prefix should not be set by the ChadoIdSpace object just yet.');
    $this->assertEmpty($db['url'], 'The URL should not be set by the ChadoIdSpace object just yet.');
    
    // Set the URL prefix to make sure it gets set in Chado.
    $GO->setURLPrefix($GO_urlprefix);
    $db = $this->getDB($GO_idspace);
    $this->assertTrue($db['name'] == $GO_idspace, 'The name was not set correctly after updating by the ChadoIdSpace object.');
    $this->assertTrue($db['description'] == $GO_description, 'The description was not set correctly after updating by the ChadoIdSpace object.');    
    $this->assertTrue($db['urlprefix'] == $GO_urlprefix, 'The URL prefix was not set correctly by the ChadoIdSpace object.');
    $this->assertEmpty($db['url'], 'The URL should not be set by the ChadoIdSpace object just yet.');
    
    // Make sure the getters work.
    $this->assertTrue($GO->getURLPrefix() == $GO_urlprefix, "The ChadoIdSpace object did not return a correct URL prefix.");
    $this->assertTrue($GO->getDescription() == $GO_description, "The ChadoIdSpace object did not return a correct description.");
    
    // Change the description and URL prefix and make sure it updates
    $GO->setDescription('Changed');
    $GO->setURLPrefix('Changed');
    $this->assertTrue($GO->getDescription() == 'Changed', "The ChadoIdSpace object did not update the description.");
    $this->assertTrue($GO->getURLPrefix() == 'Changed', "The ChadoIdSpace object did not update the URL prefix.");            
    
    // Simulate a change in the `db` record from another source. The getters should pick up the change.
    $this->updateRecord('db', $GO_idspace, 'urlprefix', 'http://replace.me/');
    $this->assertTrue($GO->getURLPrefix() == 'http://replace.me/', "The ChadoIdSpace object did not pick up an update to the URL prefix from an external source.");
    $this->updateRecord('db', $GO_idspace, 'description', 'Replace Me');
    $this->assertTrue($GO->getDescription() == 'Replace Me', "The ChadoIdSpace object did not pick up an update to the description from an external source.");
           
    // Destroy the ID Space and make sure it's gone from Tripal but not Chado.
    $idsmanager->removeCollection($GO_idspace);
    $GO = $idsmanager->loadCollection($GO_idspace);
    $this->assertTrue($GO === NULL, "The ID Space should be removed from Tripal.");
    $db = $this->getDB($GO_idspace);
    $this->assertTrue($db['urlprefix'] == 'http://replace.me/', "The ID Space was removed from Tripal but should not have been removed from Chado.");
        
    // ID Space cleanup
    $this->cleanDB($GO_idspace);
    $db = $this->getDB($GO_idspace);
    $this->assertEmpty($db, 'The db record should have been removed.');    
    
    //
    // Testing ChadoVocabulary Functionality
    //
                  
    // Make sure the IDspace doesn't yet exist.
    $db = $this->getDB($GO_idspace);
    $this->assertEmpty($db, 'The Chado db has a conflicting record.');   
    
    // Make sure the Vocabulary doesn't yet exist.
    $cv = $this->getCV($GO_cc_namespace);
    $this->assertEmpty($cv, 'The Chado cv has a conflicting record.');     
    $cc = $vmanager->createCollection($GO_cc_namespace, "chado_vocabulary");
    $cv = $this->getCV($GO_cc_namespace);
    $this->assertTrue($cv['name'] == $GO_cc_namespace, 'The name was not set correctly by the ChadoVocabulary object.');
    $this->assertEmpty($cv['definition'], 'The definition should not be set by the ChadoVocabulary object just yet.');
    
    // Set the definition to make sure it gets set in Chado.
    $cc->setLabel($GO_cc_label);
    $cv = $this->getCV($GO_cc_namespace);
    $this->assertTrue($cv['name'] == $GO_cc_namespace, 'The name was not set correctly by the ChadoVocabulary object.');
    $this->assertTrue($cv['definition'] == $GO_cc_label, 'The label was not set correctly by the ChadoVocabulary object.');
    
    // Make sure the getter works.
    $this->assertTrue($cc->getLabel() == $GO_cc_label, "The ChadoVocabulary object did not return a correct label.");    
    
    // Simulate a change in the `cv` record from another source. The getters should pick up the change
    $this->updateRecord('cv', $GO_cc_namespace, 'definition', 'Replace Me');
    $this->assertTrue($cc->getLabel() == 'Replace Me', "The ChadoVocabulary object did not pick up an update to the label from an external source.");
           
    // Associate the IDSpace with the vocabulary,
    $GO = $idsmanager->createCollection($GO_idspace, "chado_id_space");    
    $id_spaces = $cc->getIdSpaceNames();
    $this->assertFalse(in_array($GO_idspace, $id_spaces), 'ID spaces should not be set yet in the ChadoVocabulary');
    $cc->addIdSpace($GO_idspace);
    $id_spaces = $cc->getIdSpaceNames();
    $this->assertTrue(in_array($GO_idspace, $id_spaces), 'The ID space is missing from the ChadoVocabulary');
    
    // Add a URL to the vocabulary, it should show up in the 
    // database table for the ID space.
    $db = $this->getDB($GO_idspace);
    $this->assertEmpty($db['url'], 'The URL should not be set by the ChadoVocabulary object just yet.');
    $cc->setURL($GO_url);
    $db = $this->getDB($GO_idspace);
    $this->assertTrue($db['url'] == $GO_url, 'The URL was not set correctly by the ChadoVocabulary object.');
    $this->assertTrue($cc->getURL() == $GO_url, 'The URL was not retrieved by the ChadoVocabulary object.');
            
    // Test adding a URL without an ID space.
    $bp = $vmanager->createCollection($GO_bp_namespace, "chado_vocabulary");
    $bp->setLabel($GO_bp_label);
    $bp->setURL($GO_url);
    $this->assertFalse($bp->getURL() == $GO_url, 'The URL should not be set without an ID Space');
    
    // Test adding a default vocabulary to an ID space.  This should call the
    // addIdSpace() function on the vocabulary as well.
    $GO->setDefaultVocabulary($GO_bp_namespace, "chado_vocabulary");    
    $this->assertTrue($GO->getDefaultVocabulary() == $GO_bp_namespace, 'The default vocabulary was not set correctly by the ChadoIdSpace object.');
    $bp->setURL($GO_url);
    $this->assertTrue($bp->getURL() == $GO_url, 'The URL was not set correctly by the ChadoVocabulary after setting the default vocabulary.');       
    
    //
    // Testing multiple ID spaces per Vocbulary
    //
    $EDAM_data_idspace = 'data';
    $EDAM_format_idspace = 'format';
    $EDAM_operation_idspace = 'operation';
    $EDAM_topic_idspace = 'topic';
    $EDAM_namespace = 'EDAM';
    $EDAM_data_description = "Information, represented in an information artefact.";
    $EDAM_format_description = "A defined way or layout of representing and structuring data";
    $EDAM_operation_description = "A function that processes a set of inputs and results in a set of outputs";
    $EDAM_topic_description = "A category denoting a rather broad domain or field of interest, of study, application, work, data, or technology";    
    $EDAM_label = 'Gene Ontology Cellular Component Vocabulary';
    $EDAM_urlprefix = "http://edamontology.org/{db}_{accession}";
    $EDAM_url = 'http://edamontology.org';    
    $edam = $vmanager->createCollection($EDAM_namespace, "chado_vocabulary");
    $edam_data = $idsmanager->createCollection($EDAM_data_idspace, "chado_id_space");
    $edam_format = $idsmanager->createCollection($EDAM_format_idspace, "chado_id_space");
    $edam_operation = $idsmanager->createCollection($EDAM_operation_idspace, "chado_id_space");
    $edam_topic = $idsmanager->createCollection($EDAM_topic_idspace, "chado_id_space");        
    $edam_data->setDefaultVocabulary($EDAM_namespace, 'chado_vocabulary');
    $edam_format->setDefaultVocabulary($EDAM_namespace, 'chado_vocabulary');
    $edam_operation->setDefaultVocabulary($EDAM_namespace, 'chado_vocabulary');
    $edam_topic->setDefaultVocabulary($EDAM_namespace, 'chado_vocabulary');    
    $edam->setLabel($EDAM_label);
    $edam->setURL($EDAM_url);
    $edam_data->setURLPrefix($EDAM_urlprefix);
    $edam_format->setURLPrefix($EDAM_urlprefix);
    $edam_operation->setURLPrefix($EDAM_urlprefix);
    $edam_topic->setURLPrefix($EDAM_urlprefix);    
    $edam_data->setDescription($EDAM_data_description);
    $edam_format->setDescription($EDAM_format_description);
    $edam_operation->setDescription($EDAM_operation_description);
    $edam_topic->setDescription($EDAM_topic_description);
    
    // Make sure that all of the ID spaces have been aded to the vocabulary.
    $id_spaces = $edam->getIdSpaceNames();
    $this->assertTrue(in_array($EDAM_data_idspace, $id_spaces), "The EDAM data ID space is missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($EDAM_format_idspace, $id_spaces), "The EDAM format ID space is missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($EDAM_operation_idspace, $id_spaces), "The EDAM operation ID space is missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($EDAM_topic_idspace, $id_spaces), "The EDAM topic ID space is missing from the vocabulary ID spaces.");
    
    // Just do a nother check to make sure the vocabularies and ID spaces got setup correctly.
    $this->assertTrue($edam->getLabel() == $EDAM_label, "The EDAM label was not correctly returned.");
    $this->assertTrue($edam->getURL() == $EDAM_url, "The EDAM URL was not correctly returned.");
    $this->assertTrue($edam->getNameSpace() == $EDAM_namespace, "The EDAM namespace was not correctly returned.");
    $this->assertTrue($edam_data->getDefaultVocabulary() == $EDAM_namespace, "The default vocabulary for the EDAM data ID Space is not correct.");
    $this->assertTrue($edam_format->getDefaultVocabulary() == $EDAM_namespace, "The default vocabulary for the EDAM format ID Space is not correct.");
    $this->assertTrue($edam_operation->getDefaultVocabulary() == $EDAM_namespace, "The default vocabulary for the EDAM operation ID Space is not correct.");
    $this->assertTrue($edam_topic->getDefaultVocabulary() == $EDAM_namespace, "The default vocabulary for the EDAM topic ID Space is not correct.");    
    $this->assertTrue($edam_data->getDescription() == $EDAM_data_description, "The EDAM data ID space's description was not correctly returned.");
    $this->assertTrue($edam_format->getDescription() == $EDAM_format_description, "The EDAM format ID space's description was not correctly returned.");
    $this->assertTrue($edam_operation->getDescription() == $EDAM_operation_description, "The EDAM operation ID space's description was not correctly returned.");
    $this->assertTrue($edam_topic->getDescription() == $EDAM_topic_description, "The EDAM topic ID space's description was not correctly returned.");
    $this->assertTrue($edam_data->getURLPrefix() == $EDAM_urlprefix, "The EDAM data ID space's URL PRefix space description was not correctly returned.");
    $this->assertTrue($edam_format->getURLPrefix() == $EDAM_urlprefix, "The EDAM format ID space's URL PRefix space description was not correctly returned.");
    $this->assertTrue($edam_operation->getURLPrefix() == $EDAM_urlprefix, "The EDAM operation ID space's URL PRefix space description was not correctly returned.");    
    $this->assertTrue($edam_topic->getURLPrefix() == $EDAM_urlprefix, "The EDAM topic ID space's URL PRefix space description was not correctly returned.");
    
    // Test removing an ID space
    $edam->removeIdSpace($EDAM_format_idspace);
    $edam->removeIdSpace($EDAM_topic_idspace);
    $id_spaces = $edam->getIdSpaceNames();
    $this->assertTrue(in_array($EDAM_data_idspace, $id_spaces), "The EDAM data ID space is missing from the vocabulary ID spaces.");
    $this->assertFalse(in_array($EDAM_format_idspace, $id_spaces), "The EDAM format ID space is not missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($EDAM_operation_idspace, $id_spaces), "The EDAM operation ID space is missing from the vocabulary ID spaces.");
    $this->assertFalse(in_array($EDAM_topic_idspace, $id_spaces), "The EDAM topic ID space is not missing from the vocabulary ID spaces.");
    
    //
    // Testing TripalTerms
    //
    $GO->setURLPrefix($GO_urlprefix);
    
    // First create a term for the comment property.
    $rdfs_vocab = $vmanager->createCollection("rdfs", "chado_vocabulary");
    $rdfs_vocab->setLabel('Resource Description Framework Schema');
    $rdfs_vocab->setURL('https://www.w3.org/TR/rdf-schema/');
    $rdfs_id = $idsmanager->createCollection('rdfs', "chado_id_space");
    $rdfs_id->setDescription('Resource Description Framework Schema	');
    $rdfs_id->setURLPrefix('http://www.w3.org/2000/01/rdf-schema#{accession}');
    $rdfs_id->setDefaultVocabulary('rdfs', 'chado_vocabulary');
    $comment = new TripalTerm();
    $comment->setName('comment');
    $comment->setIdSpace('rdfs');
    $comment->setVocabulary('rdfs');
    $comment->setAccession('comment');    
    $this->assertTrue($comment->getName() == 'comment', 'The "comment" TripalTerm returned an incorrect name.');
    $this->assertTrue($comment->getAccession() == 'comment', 'The "comment" TripalTerm returned an incorrect accession.');
    $this->assertTrue($comment->getTermId() == 'rdfs:comment', 'The "comment" TripalTerm returned an incorrect term ID.');
    $this->assertTrue($comment->getVocabulary() == 'rdfs', 'The "comment" TripalTerm returned an incorrect vocabulary.');
    $this->assertTrue($comment->getIdSpace() == 'rdfs', 'The "comment" TripalTerm returned an incorrect ID space.');
    $this->assertTrue($comment->getURL() == 'http://www.w3.org/2000/01/rdf-schema#comment', 'The "comment" TripalTerm returned an incorrect URL.');
    
    // Create a parent term using the built-in setters.
    $parent = new TripalTerm();
    $parent->setName('biological_process');
    $parent->setIdSpace('GO');
    $parent->setVocabulary('biological_process');
    $parent->setAccession('0008150');
    $parent_definition = 'A biological process represents a specific objective that the organism is ' .
      'genetically programmed to achieve. Biological processes are often described by their outcome ' .
      'or ending state, e.g., the biological process of cell division results in the creation of two ' . 
      'daughter cells (a divided cell) from a single parent cell. A biological process is accomplished ' . 
      'by a particular set of molecular functions carried out by specific gene products (or ' .
      'macromolecular complexes), often in a highly regulated manner and in a particular temporal sequence.';
    $parent->setDefinition($parent_definition);
    $parent->addAltId('GO', '0000004');
    $parent->addAltId('GO', '0007582');
    $parent->addAltId('GO', '0044699');
    $parent->addSynonym('biological process');
    $parent->addSynonym('physiological process');
    $parent->addSynonym('single organism process');
    $parent->addSynonym('single-organism process');
    $parent_comment = 'Note that, in addition to forming the root of the biological process ontology, ' . 
      'this term is recommended for use for the annotation of gene products whose biological process ' .
      'is unknown. When this term is used for annotation, it indicates that no information was available ' .
      'about the biological process of the gene product annotated as of the date the annotation was made; ' .
      'the evidence code \'no data\' (ND), is used to indicate this.';
    $parent->addProperty($comment, $parent_comment);
    
    // Run a suite of tests on the term.
    $this->assertTrue($parent->getName() == 'biological_process', 'The "biological_process" TripalTerm returned an incorrect name.');
    $this->assertTrue($parent->getAccession() == '0008150', 'The "biological_process" TripalTerm returned an incorrect accession.');
    $this->assertTrue($parent->getTermId() == 'GO:0008150', 'The "biological_process" TripalTerm returned an incorrect term ID.');
    $this->assertTrue($parent->getDefinition() == $parent_definition, 'The "biological process" TripalTerm returned and incorrect definition.');
    $this->assertTrue($parent->getVocabulary() == 'biological_process', 'The "biological_process" TripalTerm returned an incorrect vocabulary.');
    $this->assertTrue($parent->getIdSpace() == 'GO', 'The "biological_process" TripalTerm returned an incorrect ID space.');
    $this->assertTrue($parent->getURL() == 'http://amigo.geneontology.org/amigo/term/GO:0008150', 'The "biological_process" TripalTerm returned an incorrect URL.');
    $this->assertTrue(in_array('biological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('physiological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single-organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $parent->removeSynonym('physiological process');
    $parent->removeSynonym('single-organism process');
    $this->assertTrue(in_array('biological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('physiological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm contains a synonym that should have been removed.');
    $this->assertTrue(in_array('single organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('single-organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm contains a synonym that should ahve been removed.');
    $this->assertTrue(in_array('GO:0000004', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $this->assertTrue(in_array('GO:0007582', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $this->assertTrue(in_array('GO:0044699', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $parent->removeAltId('GO', '0007582');
    $this->assertTrue(in_array('GO:0000004', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $this->assertFalse(in_array('GO:0007582', $parent->getAltIds()), 'The "biological_process" TripalTerm contains an alternative ID that should have been removed.');
    $this->assertTrue(in_array('GO:0044699', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $properties = $parent->getProperties();
    $this->assertTrue(array_key_exists('rdfs:comment', $properties), 'The "biological_process" TripalTerm is missing the comment property.');
    $this->assertTrue($properties['rdfs:comment'][0][1] == $parent_comment, 'The "biological_process" TripalTerm comment property value was not returned correctly.');
    $parent->removeProperty('rdfs', 'comment', 0);
    $properties = $parent->getProperties();
    $this->assertEmpty($properties, 'The "biological_process" TripalTerm should not have any properties');
    
    // Recreate the term using the constructor instead of the setters.   
    $parent = new TripalTerm([
      'name' => 'biological_process',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'accession' => '0008150',
      'definition' => $parent_definition,
      'altIDs' => [
        ['GO', '0000004'],
        ['GO', '0007582'],
        ['GO', '0044699'],
      ],
      'synonyms' => [
        'biological process',
        'physiological process',
        'single organism process',
        'single-organism process',       
      ],
      'properties' => [
        [$comment, $parent_comment],
      ],
    ]);
    
    // Re-run the same tests above on this recreated term.// Run a suite of tests on the term.
    $this->assertTrue($parent->getName() == 'biological_process', 'The "biological_process" TripalTerm returned an incorrect name.');
    $this->assertTrue($parent->getAccession() == '0008150', 'The "biological_process" TripalTerm returned an incorrect accession.');
    $this->assertTrue($parent->getTermId() == 'GO:0008150', 'The "biological_process" TripalTerm returned an incorrect term ID.');
    $this->assertTrue($parent->getDefinition() == $parent_definition, 'The "biological process" TripalTerm returned and incorrect definition.');
    $this->assertTrue($parent->getVocabulary() == 'biological_process', 'The "biological_process" TripalTerm returned an incorrect vocabulary.');
    $this->assertTrue($parent->getIdSpace() == 'GO', 'The "biological_process" TripalTerm returned an incorrect ID space.');
    $this->assertTrue($parent->getURL() == 'http://amigo.geneontology.org/amigo/term/GO:0008150', 'The "biological_process" TripalTerm returned an incorrect URL.');
    $this->assertTrue(in_array('biological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('physiological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single-organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $parent->removeSynonym('physiological process');
    $parent->removeSynonym('single-organism process');
    $this->assertTrue(in_array('biological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('physiological process', $parent->getSynonyms()), 'The "biological_process" TripalTerm contains a synonym that should have been removed.');
    $this->assertTrue(in_array('single organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('single-organism process', $parent->getSynonyms()), 'The "biological_process" TripalTerm contains a synonym that should ahve been removed.');
    $this->assertTrue(in_array('GO:0000004', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $this->assertTrue(in_array('GO:0007582', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $this->assertTrue(in_array('GO:0044699', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $parent->removeAltId('GO', '0007582');
    $this->assertTrue(in_array('GO:0000004', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $this->assertFalse(in_array('GO:0007582', $parent->getAltIds()), 'The "biological_process" TripalTerm contains an alternative ID that should have been removed.');
    $this->assertTrue(in_array('GO:0044699', $parent->getAltIds()), 'The "biological_process" TripalTerm is missing an alternative ID.');
    $properties = $parent->getProperties();
    $this->assertTrue(array_key_exists('rdfs:comment', $properties), 'The "biological_process" TripalTerm is missing the comment property.');
    $this->assertTrue($properties['rdfs:comment'][0][1] == $parent_comment, 'The "biological_process" TripalTerm comment property value was not returned correctly.');
    $parent->removeProperty('rdfs', 'comment', 0);
    $properties = $parent->getProperties();
    $this->assertEmpty($properties, 'The "biological_process" TripalTerm should not have any properties');
    
    // Next create a relationship type term.
    $is_a = new TripalTerm();
    $is_a->setName('is_a');
    $is_a->setIdSpace('GO');
    $is_a->setVocabulary('biological_process');
    $is_a->setAccession('is_a');    
    $is_a->setIsRelationshipType(True);
    $this->assertTrue($is_a->isRelationshipType(), 'The "is_a" TripalTerm failed to indicate it is a relationship term.');
    
    // Next create a child term and set it's parent.
    $child = new TripalTerm();
    $child->setName('biological phase');
    $child->setIdSpace('GO');
    $child->setVocabulary('biological_process');
    $child->setAccession('0044848');    
    $child->setDefinition('A distinct period or stage in a biological process or cycle.');
    $child_comment = 'Note that phases are is_a disjoint from other biological processes. ' . 
      'happens_during relationships can operate between phases and other biological processes ' . 
      'e.g. DNA replication happens_during S phase.';
    $child->addProperty($comment, $child_comment);
    $child->addParent($parent, $is_a);
    
    // Test the parent/child relationship.
    $parents = $child->getParents();
    $this->assertTrue(array_key_exists('GO:0008150', $parents), 'The "biological phase" TripalTerm did not return a parent.');
    $this->assertTrue($parents['GO:0008150'][0]->getTermId() == 'GO:0008150', 'The "biological phase" TripalTerm parent is out of order. The parent term should be first in the tuple.');
    $this->assertTrue($parents['GO:0008150'][1]->getTermId() == 'GO:is_a', 'The "biological phase" TripalTerm parent is out of order. The relationship term should be second in the tuple.');
    $child->removeParent('GO', '0008150');
    $parents = $child->getParents();
    $this->assertEmpty($parents, 'The "biological phase" TripalTerm should not have any parents after they were removed.');
    
    // Recreate the parent relationship using the constructor.
    $child = new TripalTerm([
      'name' => 'biological phase',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'definition' => 'A distinct period or stage in a biological process or cycle.',
      'accession' => '0044848',
      'properties' => [
        [$comment, $child_comment]
      ],
      'parents' => [
        [$parent, $is_a]
      ],
    ]);
    
    // Repeat the tests for the relationships.
    $parents = $child->getParents();
    $this->assertTrue(array_key_exists('GO:0008150', $parents), 'The "biological phase" TripalTerm did not return a parent.');
    $this->assertTrue($parents['GO:0008150'][0]->getTermId() == 'GO:0008150', 'The "biological phase" TripalTerm parent is out of order. The parent term should be first in the tuple.');
    $this->assertTrue($parents['GO:0008150'][1]->getTermId() == 'GO:is_a', 'The "biological phase" TripalTerm parent is out of order. The relationship term should be second in the tuple.');
    $child->removeParent('GO', '0008150');
    $parents = $child->getParents();
    $this->assertEmpty($parents, 'The "biological phase" TripalTerm should not have any parents after they were removed.');
    
    // Make sure the isVald works.
    $dummy = new TripalTerm();
    $this->assertFalse($dummy->isValid(), 'The dummy TripalTerm reports it is valid when it is not (Test 1).');
    $dummy->setName('dummy');
    $this->assertFalse($dummy->isValid(), 'The dummy TripalTerm reports it is valid when it is not (Test 2).');
    $dummy->setIdSpace('GO');
    $this->assertFalse($dummy->isValid(), 'The dummy TripalTerm reports it is valid when it is not (Test 3).');
    $dummy->setVocabulary('biological_process');
    $this->assertFalse($dummy->isValid(), 'The dummy TripalTerm reports it is valid when it is not (Test 4).');
    $dummy->setAccession('dummy');
    $this->assertTrue($dummy->isValid(), 'The dummy TripalTerm reports it is not valid when it is.');
       
    // Test Saving the term
    // @todo test trying to save when the term is invalid
    // @todo make sure all of the term attributes are saved in the appropriate tables.
    
  }
}


