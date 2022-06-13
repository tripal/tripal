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
    $GO = $idsmanager->loadCollection($GO_idspace, "chado_id_space");
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
    
    
    $term = new TripalTerm(
      'biological_process',
      'A biological process represents a specific objective that the organism is genetically programmed to achieve. Biological processes are often described by their outcome or ending state, e.g., the biological process of cell division results in the creation of two daughter cells (a divided cell) from a single parent cell. A biological process is accomplished by a particular set of molecular functions carried out by specific gene products (or macromolecular complexes), often in a highly regulated manner and in a particular temporal sequence.',
      'GO',
      '0008150',
      'biological_process'
    );
    $term->save();
  }
}


