<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\TripalVocabTerms;

use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalVocabularyInterface;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\Services\TripalLogger;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests for the ChadoCVTerm classes.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoVocabTerms
 */
#[Group('Tripal')]
#[Group('Tripal Chado')]
#[Group('Tripal Chado ChadoVocabTerms')]
class ChadoVocabTermsTest extends ChadoTestKernelBase {

  /**
   * Modules to be enabled in the test environment.
   *
   * @var array
   */
  protected static $modules = ['system', 'tripal_chado'];

  /**
   * The current test chado schema.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * The Tripal IDSpace plugin manager.
   *
   * @var Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager
   */
  protected TripalIdSpaceManager $idsmanager;

  /**
   * The TripalVocab plugin manager.
   *
   * @var Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager
   */
  protected TripalVocabularyManager $vmanager;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm']);

    // Get Chado in place.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY);

    // Ensure the db2cv table exists.
    $create_sql = "CREATE TABLE {1:db2cv_mview} (
         cv_id integer NOT NULL,
         cvname character varying(255) NOT NULL,
         db_id integer NOT NULL,
         dbname character varying(255) NOT NULL,
         num_terms integer NOT NULL
       )";
    $this->chado_connection->query($create_sql);

    // We need to mock the logger.
    $mock_logger = $this->getMockBuilder(TripalLogger::class)
      ->onlyMethods(['notice', 'error'])
      ->getMock();
    $mock_logger->method('notice')
      ->willReturnCallback(function ($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $mock_logger->method('error')
      ->willReturnCallback(function ($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $this->container->set('tripal.logger', $mock_logger);

    // Get the plugin managers.
    $this->vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $this->idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');

  }

  /**
   * Update a chado record assuming the name is unique.
   *
   * @param string $table
   *   The name of chado table to update.
   * @param string $name
   *   The value of the [table].name record in chado you want to update.
   * @param string $field
   *   The name of the column in the chado [table] you want to update.
   * @param mixed $value
   *   The value to update [table].[field] to.
   */
  protected function updateRecord(string $table, string $name, string $field, mixed $value): void {

    $query = $this->chado_connection->update('1:' . $table)
      ->condition('name', $name, '=')
      ->fields([$field => $value]);
    $query->execute();
  }

  /**
   * Delete a chado record assuming the name is unique.
   *
   * @param string $table
   *   The name of chado table whose record you want to delete.
   * @param string $name
   *   The value of the [table].name record in chado you want to delete.
   */
  protected function cleanRecord(string $table, string $name): void {

    $query = $this->chado_connection->delete('1:' . $table)
      ->condition('name', $name, '=');
    $query->execute();
  }

  /**
   * Confirms the ID Space collection matches our expectations.
   *
   * @param array $expectations
   *   An array listing our expections. It supports the following:
   *   - name: the name of the ID Space.
   *   - description: the description of the id space.
   *   - chado_record: TRUE OR FALSE depending on if the record should exist.
   * @param mixed $idspace
   *   An ID Space collection instance to be checked.
   * @param string $prefix
   *   Will be prefixed to the message for all our asserts to provide context.
   */
  protected function assertIdSpaceEquals(array $expectations, mixed $idspace, string $prefix = '') {

    $this->assertInstanceOf(TripalIdSpaceInterface::class, $idspace, $prefix . "ID space did not implement the right interface.");
    $this->assertEquals($expectations['name'], $idspace->getName(), $prefix . "ID space name does not match.");
    $this->assertEquals($expectations['description'], $idspace->getDescription(), $prefix . "ID space description does not match.");

    $chado_record = $this->getChadoDbRecord($expectations['name']);
    if (array_key_exists('chado_record', $expectations) && ($expectations['chado_record'] === TRUE)) {
      $this->assertIsObject($chado_record, $prefix . "Chado Record did not exist.");
      $this->assertEquals($expectations['name'], $chado_record->name, $prefix . "The chado record name does not match.");
      $this->assertEquals($expectations['description'], $chado_record->description, $prefix . "The chado record description does not match.");
    }
    else {
      $this->assertFalse($chado_record, $prefix . "Chado Record should not have existed.");
    }
  }

  /**
   * Confirms the Vocab collection matches our expectations.
   *
   * @param array $expectations
   *   An array listing our expections. It supports the following:
   *   - name: the name of the Vocab.
   *   - definition: the definition of the Vocab.
   *   - chado_record: TRUE OR FALSE depending on if the record should exist.
   * @param mixed $vocab
   *   An Vocab collection instance to be checked.
   * @param string $prefix
   *   Will be prefixed to the message for all our asserts to provide context.
   */
  protected function assertVocabEquals(array $expectations, mixed $vocab, string $prefix = '') {

    $this->assertInstanceOf(TripalVocabularyInterface::class, $vocab, $prefix . "Vocab did not implement the right interface.");
    $this->assertEquals($expectations['name'], $vocab->getName(), $prefix . "Vocab name does not match.");
    $this->assertEquals($expectations['definition'], $vocab->getLabel(), $prefix . "Vocab label/definition does not match.");

    $chado_record = $this->getChadoCvRecord($expectations['name']);
    if (array_key_exists('chado_record', $expectations) && ($expectations['chado_record'] === TRUE)) {
      $this->assertIsObject($chado_record, $prefix . "Chado Record did not exist.");
      $this->assertEquals($expectations['name'], $chado_record->name, $prefix . "The Chado Record name does not match.");
      $this->assertEquals($expectations['definition'], $chado_record->definition, $prefix . "The Chado Record definition does not match.");
    }
    else {
      $this->assertFalse($chado_record, $prefix . "Chado Record should not have existed.");
    }
  }

  /**
   * Tests the ChadoIdSpace Class.
   */
  public function testTripalIdSpaceClass() {

    // These are the values we'll use for the ID space and vocabulary.
    $go_idspace = 'GO';
    $go_description = "The Gene Ontology (GO) knowledgebase is the world’s largest source of information on the functions of genes";
    $go_urlprefix = "http://amigo.geneontology.org/amigo/term/{db}:{accession}";

    // Make sure the IDspace doesn't yet exist.
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertEmpty($db, 'The Chado db has a conflicting record.');

    // Create the ID space object and make sure a Chado record got created.
    $go = $this->idsmanager->createCollection($go_idspace, "chado_id_space");
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertTrue($db->name == $go_idspace, 'The name was not set correctly by the ChadoIdSpace object.');
    $this->assertEmpty($db->description, 'The description should not be set by the ChadoIdSpace object just yet.');
    $this->assertEmpty($db->urlprefix, 'The URL prefix should not be set by the ChadoIdSpace object just yet.');
    $this->assertEmpty($db->url, 'The URL should not be set by the ChadoIdSpace object just yet.');

    // Set the description to make sure it gets set in Chado.
    $go->setDescription($go_description);
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertTrue($db->name == $go_idspace, 'The name was not set correctly after updating by the ChadoIdSpace object.');
    $this->assertTrue($db->description == $go_description, 'The description was not set correctly by the ChadoIdSpace object.');
    $this->assertEmpty($db->urlprefix, 'The URL prefix should not be set by the ChadoIdSpace object just yet.');
    $this->assertEmpty($db->url, 'The URL should not be set by the ChadoIdSpace object just yet.');

    // Set the URL prefix to make sure it gets set in Chado.
    $go->setURLPrefix($go_urlprefix);
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertTrue($db->name == $go_idspace, 'The name was not set correctly after updating by the ChadoIdSpace object.');
    $this->assertTrue($db->description == $go_description, 'The description was not set correctly after updating by the ChadoIdSpace object.');
    $this->assertTrue($db->urlprefix == $go_urlprefix, 'The URL prefix was not set correctly by the ChadoIdSpace object.');
    $this->assertEmpty($db->url, 'The URL should not be set by the ChadoIdSpace object just yet.');

    // Make sure the getters work.
    $this->assertTrue($go->getURLPrefix() == $go_urlprefix, "The ChadoIdSpace object did not return a correct URL prefix.");
    $this->assertTrue($go->getDescription() == $go_description, "The ChadoIdSpace object did not return a correct description.");

    // Change the description and URL prefix and make sure it updates.
    $go->setDescription('Changed');
    $go->setURLPrefix('Changed');
    $this->assertTrue($go->getDescription() == 'Changed', "The ChadoIdSpace object did not update the description.");
    $this->assertTrue($go->getURLPrefix() == 'Changed', "The ChadoIdSpace object did not update the URL prefix.");

    // Simulate a change in the `db` record from another source.
    // The getters should pick up the change.
    $this->updateRecord('db', $go_idspace, 'urlprefix', 'http://replace.me/');
    $this->assertTrue($go->getURLPrefix() == 'http://replace.me/', "The ChadoIdSpace object did not pick up an update to the URL prefix from an external source.");
    $this->updateRecord('db', $go_idspace, 'description', 'Replace Me');
    $this->assertTrue($go->getDescription() == 'Replace Me', "The ChadoIdSpace object did not pick up an update to the description from an external source.");

    // Destroy the ID Space and make sure it's gone from Tripal but not Chado.
    $this->idsmanager->removeCollection($go_idspace);
    $go = $this->idsmanager->loadCollection($go_idspace);
    $this->assertTrue($go === NULL, "The ID Space should be removed from Tripal.");
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertNotFalse($db, "The ID Space was removed from Tripal but should not have been removed from Chado.");
    $this->assertEquals($db->urlprefix, 'http://replace.me/', "The ID Space should not have been changed in chado when it was removed from Tripal.");

  }

  /**
   * Tests the ChadoVocab Class.
   */
  public function testTripalVocabClass() {

    // These are the values we'll use for the ID space and vocabulary.
    $go_idspace = 'GO';
    $go_cc_namespace = 'cellular_component';
    $go_bp_namespace = 'biological_process';
    $go_cc_label = 'Gene Ontology Cellular Component Vocabulary';
    $go_bp_label = 'Gene Ontology Biological Process Vocabulary';
    $go_url = 'http://geneontology.org/';

    // Make sure the IDspace doesn't yet exist.
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertEmpty($db, 'The Chado db has a conflicting record.');

    // Make sure the Vocabulary doesn't yet exist.
    $cv = $this->getChadoCvRecord($go_cc_namespace);
    $this->assertEmpty($cv, 'The Chado cv has a conflicting record.');

    // Create the vocab.
    $cc = $this->vmanager->createCollection($go_cc_namespace, "chado_vocabulary");
    $cv = $this->getChadoCvRecord($go_cc_namespace);
    $this->assertTrue($cv->name == $go_cc_namespace, 'The name was not set correctly by the ChadoVocabulary object.');
    $this->assertEmpty($cv->definition, 'The definition should not be set by the ChadoVocabulary object just yet.');

    // Set the definition to make sure it gets set in Chado.
    $cc->setLabel($go_cc_label);
    $cv = $this->getChadoCvRecord($go_cc_namespace);
    $this->assertTrue($cv->name == $go_cc_namespace, 'The name was not set correctly by the ChadoVocabulary object.');
    $this->assertTrue($cv->definition == $go_cc_label, 'The label was not set correctly by the ChadoVocabulary object.');

    // Make sure the getter works.
    $this->assertTrue($cc->getLabel() == $go_cc_label, "The ChadoVocabulary object did not return a correct label.");

    // Simulate a change in the `cv` record from another source.
    // The getters should pick up the change.
    $this->updateRecord('cv', $go_cc_namespace, 'definition', 'Replace Me');
    $this->assertTrue($cc->getLabel() == 'Replace Me', "The ChadoVocabulary object did not pick up an update to the label from an external source.");

    // Associate the IDSpace with the vocabulary,.
    $go = $this->idsmanager->createCollection($go_idspace, "chado_id_space");
    $id_spaces = $cc->getIdSpaceNames();
    $this->assertFalse(in_array($go_idspace, $id_spaces), 'ID spaces should not be set yet in the ChadoVocabulary');
    $cc->addIdSpace($go_idspace);
    $id_spaces = $cc->getIdSpaceNames();
    $this->assertTrue(in_array($go_idspace, $id_spaces), 'The ID space is missing from the ChadoVocabulary');

    // Add a URL to the vocabulary, it should show up in the
    // database table for the ID space.
    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertEmpty($db->url, 'The URL should not be set by the ChadoVocabulary object just yet.');
    $cc->setURL($go_url);

    $db = $this->getChadoDbRecord($go_idspace);
    $this->assertTrue($db->url == $go_url, 'The URL was not set correctly by the ChadoVocabulary object.');
    $this->assertTrue($cc->getURL() == $go_url, 'The URL was not retrieved by the ChadoVocabulary object.');

    // Test adding a URL without an ID space.
    $bp = $this->vmanager->createCollection($go_bp_namespace, "chado_vocabulary");
    $bp->setLabel($go_bp_label);
    $printed_output = '';
    $expected_message = 'ChadoVocabulary: Cannot set the URL when no ID spaces are present for the vocabulary.';
    ob_start();
    $bp->setURL($go_url);
    $printed_output = ob_get_clean();
    $this->assertStringContainsString($expected_message, $printed_output, "We did not get the log message we expected when trying to the the URL on a vocab without an ID Space.");
    // Also test getting it in the same scenario.
    $printed_output = '';
    $expected_message = 'ChadoVocabulary: Cannot get the URL when no ID spaces are present for the vocabulary.';
    ob_start();
    $returned_url = $bp->getURL();
    $printed_output = ob_get_clean();
    $this->assertStringContainsString($expected_message, $printed_output, "We did not get the log message we expected when trying to the the URL on a vocab without an ID Space.");
    $this->assertFalse($returned_url == $go_url, 'The URL should not be set without an ID Space');

    // Test adding a default vocabulary to an ID space.  This should call the
    // addIdSpace() function on the vocabulary as well.
    $go->setDefaultVocabulary($go_bp_namespace);
    $this->assertTrue($go->getDefaultVocabulary() == $go_bp_namespace, 'The default vocabulary was not set correctly by the ChadoIdSpace object.');
    $bp->setURL($go_url);
    $this->assertTrue($bp->getURL() == $go_url, 'The URL was not set correctly by the ChadoVocabulary after setting the default vocabulary.');

    //
    // Testing multiple ID spaces per Vocabulary
    // .
    $edam_data_idspace = 'data';
    $edam_format_idspace = 'format';
    $edam_operation_idspace = 'operation';
    $edam_topic_idspace = 'topic';
    $edam_namespace = 'EDAM';
    $edam_data_description = "Information, represented in an information artefact.";
    $edam_format_description = "A defined way or layout of representing and structuring data";
    $edam_operation_description = "A function that processes a set of inputs and results in a set of outputs";
    $edam_topic_description = "A category denoting a rather broad domain or field of interest, of study, application, work, data, or technology";
    $edam_label = 'Gene Ontology Cellular Component Vocabulary';
    $edam_urlprefix = "http://edamontology.org/{db}_{accession}";
    $edam_url = 'http://edamontology.org';
    $edam = $this->vmanager->createCollection($edam_namespace, "chado_vocabulary");
    $edam_data = $this->idsmanager->createCollection($edam_data_idspace, "chado_id_space");
    $edam_format = $this->idsmanager->createCollection($edam_format_idspace, "chado_id_space");
    $edam_operation = $this->idsmanager->createCollection($edam_operation_idspace, "chado_id_space");
    $edam_topic = $this->idsmanager->createCollection($edam_topic_idspace, "chado_id_space");
    $edam_data->setDefaultVocabulary($edam_namespace);
    $edam_format->setDefaultVocabulary($edam_namespace);
    $edam_operation->setDefaultVocabulary($edam_namespace);
    $edam_topic->setDefaultVocabulary($edam_namespace);
    $edam->setLabel($edam_label);
    $edam->setURL($edam_url);
    $edam_data->setURLPrefix($edam_urlprefix);
    $edam_format->setURLPrefix($edam_urlprefix);
    $edam_operation->setURLPrefix($edam_urlprefix);
    $edam_topic->setURLPrefix($edam_urlprefix);
    $edam_data->setDescription($edam_data_description);
    $edam_format->setDescription($edam_format_description);
    $edam_operation->setDescription($edam_operation_description);
    $edam_topic->setDescription($edam_topic_description);

    // Make sure that all of the ID spaces have been added to the vocabulary.
    $id_spaces = $edam->getIdSpaceNames();
    $this->assertTrue(in_array($edam_data_idspace, $id_spaces), "The EDAM data ID space is missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($edam_format_idspace, $id_spaces), "The EDAM format ID space is missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($edam_operation_idspace, $id_spaces), "The EDAM operation ID space is missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($edam_topic_idspace, $id_spaces), "The EDAM topic ID space is missing from the vocabulary ID spaces.");

    // Just do another check to make sure the vocabularies and ID spaces
    // got setup correctly.
    $this->assertTrue($edam->getLabel() == $edam_label, "The EDAM label was not correctly returned.");
    $this->assertTrue($edam->getURL() == $edam_url, "The EDAM URL was not correctly returned.");
    $this->assertTrue($edam->getNameSpace() == $edam_namespace, "The EDAM namespace was not correctly returned.");
    $this->assertTrue($edam_data->getDefaultVocabulary() == $edam_namespace, "The default vocabulary for the EDAM data ID Space is not correct.");
    $this->assertTrue($edam_format->getDefaultVocabulary() == $edam_namespace, "The default vocabulary for the EDAM format ID Space is not correct.");
    $this->assertTrue($edam_operation->getDefaultVocabulary() == $edam_namespace, "The default vocabulary for the EDAM operation ID Space is not correct.");
    $this->assertTrue($edam_topic->getDefaultVocabulary() == $edam_namespace, "The default vocabulary for the EDAM topic ID Space is not correct.");
    $this->assertTrue($edam_data->getDescription() == $edam_data_description, "The EDAM data ID space's description was not correctly returned.");
    $this->assertTrue($edam_format->getDescription() == $edam_format_description, "The EDAM format ID space's description was not correctly returned.");
    $this->assertTrue($edam_operation->getDescription() == $edam_operation_description, "The EDAM operation ID space's description was not correctly returned.");
    $this->assertTrue($edam_topic->getDescription() == $edam_topic_description, "The EDAM topic ID space's description was not correctly returned.");
    $this->assertTrue($edam_data->getURLPrefix() == $edam_urlprefix, "The EDAM data ID space's URL PRefix space description was not correctly returned.");
    $this->assertTrue($edam_format->getURLPrefix() == $edam_urlprefix, "The EDAM format ID space's URL PRefix space description was not correctly returned.");
    $this->assertTrue($edam_operation->getURLPrefix() == $edam_urlprefix, "The EDAM operation ID space's URL PRefix space description was not correctly returned.");
    $this->assertTrue($edam_topic->getURLPrefix() == $edam_urlprefix, "The EDAM topic ID space's URL PRefix space description was not correctly returned.");

    // Test removing an ID space.
    $edam->removeIdSpace($edam_format_idspace);
    $edam->removeIdSpace($edam_topic_idspace);
    $id_spaces = $edam->getIdSpaceNames();
    $this->assertTrue(in_array($edam_data_idspace, $id_spaces), "The EDAM data ID space is missing from the vocabulary ID spaces.");
    $this->assertFalse(in_array($edam_format_idspace, $id_spaces), "The EDAM format ID space is not missing from the vocabulary ID spaces.");
    $this->assertTrue(in_array($edam_operation_idspace, $id_spaces), "The EDAM operation ID space is missing from the vocabulary ID spaces.");
    $this->assertFalse(in_array($edam_topic_idspace, $id_spaces), "The EDAM topic ID space is not missing from the vocabulary ID spaces.");

  }

  /**
   * Tests the TripalTerm Class.
   */
  public function testTripalTermClass() {

    // Create the GO ID space as we will use it later.
    $go = $this->idsmanager->createCollection('GO', "chado_id_space");
    $go->setURLPrefix("http://amigo.geneontology.org/amigo/term/{db}:{accession}");
    // Same with the biological process Vocabulary.
    $bp = $this->vmanager->createCollection('biological_process', "chado_vocabulary");
    $bp->setLabel('Gene Ontology Biological Process Vocabulary');

    // First create a term for the comment property.
    $rdfs_vocab = $this->vmanager->createCollection("rdfs", "chado_vocabulary");
    $rdfs_vocab->setLabel('Resource Description Framework Schema');
    $rdfs_id = $this->idsmanager->createCollection('rdfs', "chado_id_space");
    $rdfs_id->setDescription('Resource Description Framework Schema	');
    $rdfs_id->setURLPrefix('http://www.w3.org/2000/01/rdf-schema#{accession}');
    $rdfs_id->setDefaultVocabulary('rdfs', 'chado_vocabulary');
    $rdfs_vocab->setURL('https://www.w3.org/TR/rdf-schema/');
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
    $this->assertTrue(in_array('biological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('physiological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single-organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $parent->removeSynonym('physiological process');
    $parent->removeSynonym('single-organism process');
    $this->assertTrue(in_array('biological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('physiological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm contains a synonym that should have been removed.');
    $this->assertTrue(in_array('single organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('single-organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm contains a synonym that should ahve been removed.');
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

    // Re-run the same tests above on this recreated term.
    $this->assertTrue($parent->getName() == 'biological_process', 'The "biological_process" TripalTerm returned an incorrect name.');
    $this->assertTrue($parent->getAccession() == '0008150', 'The "biological_process" TripalTerm returned an incorrect accession.');
    $this->assertTrue($parent->getTermId() == 'GO:0008150', 'The "biological_process" TripalTerm returned an incorrect term ID.');
    $this->assertTrue($parent->getDefinition() == $parent_definition, 'The "biological process" TripalTerm returned and incorrect definition.');
    $this->assertTrue($parent->getVocabulary() == 'biological_process', 'The "biological_process" TripalTerm returned an incorrect vocabulary.');
    $this->assertTrue($parent->getIdSpace() == 'GO', 'The "biological_process" TripalTerm returned an incorrect ID space.');
    $this->assertTrue($parent->getURL() == 'http://amigo.geneontology.org/amigo/term/GO:0008150', 'The "biological_process" TripalTerm returned an incorrect URL.');
    $this->assertTrue(in_array('biological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('physiological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertTrue(in_array('single-organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $parent->removeSynonym('physiological process');
    $parent->removeSynonym('single-organism process');
    $this->assertTrue(in_array('biological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('physiological process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm contains a synonym that should have been removed.');
    $this->assertTrue(in_array('single organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm is missing a synonym.');
    $this->assertFalse(in_array('single-organism process', array_keys($parent->getSynonyms())), 'The "biological_process" TripalTerm contains a synonym that should ahve been removed.');
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
    $is_a->setIsRelationshipType(TRUE);
    $this->assertTrue($is_a->isRelationshipType(), 'The "is_a" TripalTerm failed to indicate it is a relationship term.');

    // Next create a child term and set its parent.
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
        [$comment, $child_comment],
      ],
      'parents' => [
        [$parent, $is_a],
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

    // Make sure the isValid works.
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

    //
    // Inserting (Saving) Terms to Chado.
    //
    // We need to save the comment term first as this is used
    // for a property in our new child term below.
    $rdfs_id->saveTerm($comment);
    $go->saveTerm($parent);
    $go->saveTerm($is_a);

    $cvterm = $this->getChadoCvtermRecord('rdfs', 'comment');
    $this->assertTrue(!empty($cvterm) and $cvterm->name == 'comment', 'The term did not save a proper cvterm record  (Test #1).');
    $comment2 = $rdfs_id->getTerm('comment');
    $this->assertFalse($comment2->isRelationshipType(), 'The getTerm function did not return a term with the is_relationshiptype value loaded properly.');

    // Create a new term for saving.
    $new_child_def = 'The internally coordinated responses (actions or inactions) of animals (individuals or groups) to internal or external stimuli, via a mechanism that involves nervous system activity. Source: PMID:20160973, GOC:ems, GOC:jl, ISBN:0395448956';
    $new_child_comment = 'Note that this term is in the subset of terms that should not be used for direct gene product annotation. Instead, select a child term or, if no appropriate child term exists, please request a new term. Direct annotations to this term may be amended during annotation reviews. 2. While a broader definition of behavior encompassing plants and single cell organisms would be justified on the basis of some usage (see PMID:20160973 for discussion), GO uses a tight definition that limits behavior to animals and to responses involving the nervous system, excluding plant responses that GO classifies under development, and responses of unicellular organisms that has general classifications for covering the responses of cells in multicellular organisms (e.g. cell chemotaxis).';
    $new_child = new TripalTerm([
      'name' => 'behavior',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'accession' => '0007610',
      'definition' => $new_child_def,
      'altIDs' => [
        ['GO', '0044709'],
        ['GO', '0023032'],
        ['GO', '0044708'],
      ],
      'synonyms' => [
        'behavioral response to stimulus',
        'behaviour',
        'behavioural response to stimulus',
        'single-organism behavior',
      ],
      'properties' => [
        [$comment, $new_child_comment],
      ],
      'parents' => [
        [$parent, $is_a],
      ],
    ]);
    $go->saveTerm($new_child);
    $cvterm = $this->getChadoCvtermRecord('biological_process', 'behavior');
    $this->assertTrue(!empty($cvterm) and $cvterm->name == 'behavior', 'The term did not save a proper cvterm record (Test #2).');

    // Now that the term is saved, load it and see if all of the attributes
    // are properly set.
    $new_child2 = $go->getTerm('0007610');
    $this->assertTrue($new_child2->getName() == 'behavior', 'The getTerm function did not return a term with the name loaded properly.');
    $this->assertTrue($new_child2->getDefinition() == $new_child_def, 'The getTerm function did not return a term with the definition loaded properly.');
    $this->assertFalse($new_child2->isObsolete(), 'The getTerm function did not return a term with the is_obsolete value loaded properly.');
    $this->assertFalse($new_child2->isRelationshipType(), 'The getTerm function did not return a term with the is_obsolete value loaded properly.');
    $props = $new_child2->getProperties();
    $this->assertTrue(array_keys($props)[0] == 'rdfs:comment', 'The getTerm->getProperties function did not return properties in the correct format (keys).');
    $this->assertTrue(count($props['rdfs:comment'][0]) == 2, 'The getTerm->getProperties function did not return properties in the correct format (tuples).');
    $this->assertTrue($props['rdfs:comment'][0][0]->getName() == 'comment', 'The getTerm->getProperties function did not return properties in the correct format (type).');
    $this->assertTrue($props['rdfs:comment'][0][1] == $new_child_comment, 'The getTerm->getProperties function did not return properties in the correct format (value).');
    $altIds = $new_child2->getAltIds();
    $this->assertTrue(in_array('GO:0044709', $altIds), 'The getTerm->getAltIds function did not return all of the term IDs (Test #1).');
    $this->assertTrue(in_array('GO:0023032', $altIds), 'The getTerm->getAltIds function did not return all of the term IDs (Test #2).');
    $this->assertTrue(in_array('GO:0044708', $altIds), 'The getTerm->getAltIds function did not return all of the term IDs (Test #3).');
    $synonyms = $new_child2->getSynonyms();
    $this->assertTrue(in_array('behavioral response to stimulus', array_keys($synonyms)), 'The getTerm->getSynonysm function did not return all of the synonyms (Test #1).');
    $this->assertTrue(in_array('behaviour', array_keys($synonyms)), 'The getTerm->getSynonysm function did not return all of the synonyms (Test #2).');
    $this->assertTrue(in_array('behavioural response to stimulus', array_keys($synonyms)), 'The getTerm->getSynonysm function did not return all of the synonyms (Test #3).');
    $this->assertTrue(in_array('single-organism behavior', array_keys($synonyms)), 'The getTerm->getSynonysm function did not return all of the synonyms (Test #4).');
    $parents = $new_child2->getParents();
    $this->assertTrue(array_keys($parents)[0] == 'GO:0008150', 'The getTerm->getParents function did not return parents in the correct format (keys).');
    $this->assertTrue($parents['GO:0008150'][0]->getName() == 'biological_process', 'The getTerm->getParents function did not return parents in the correct format (term).');
    $this->assertTrue($parents['GO:0008150'][1]->getName() == 'is_a', 'The getTerm->getParents function did not return parents in the correct format (type).');

    //
    // Updating (Saving) Terms in Chado.
    //
    // Remove all optional attributes and save.
    $new_child2->removeAltId('GO', '0044709');
    $new_child2->removeAltId('GO', '0023032');
    $new_child2->removeAltId('GO', '0044708');
    $new_child2->removeSynonym('behavioral response to stimulus');
    $new_child2->removeSynonym('behavioural response to stimulus');
    $new_child2->removeSynonym('behaviour');
    $new_child2->removeSynonym('single-organism behavior');
    $new_child2->removeParent('GO', '0008150');
    $new_child2->removeProperty('rdfs', 'comment', 0);
    $go->saveTerm($new_child2);
    $new_child3 = $go->getTerm('0007610');
    $this->assertTrue(count(array_keys($new_child3->getProperties())) == 0, 'Updates to a term are not removing properties correctly');
    $this->assertTrue(count($new_child3->getAltIds()) == 0, 'Updates to a term are not removing alt IDs correctly');
    $this->assertTrue(count(array_keys($new_child3->getSynonyms())) == 0, 'Updates to a term are not removing synonyms correctly');
    $this->assertTrue(count(array_keys($new_child3->getParents())) == 0, 'Updates to a term are not removing parents correctly');

    // Add back in at least 1 attribute and save.
    $new_child3->addSynonym('behaviour');
    $new_child3->addAltId('GO', '0044708');
    $new_child3->addParent($parent, $is_a);
    $new_child3->addProperty($comment, $new_child_comment);
    $go->saveTerm($new_child3);
    $new_child4 = $go->getTerm('0007610');
    $this->assertTrue(count(array_keys($new_child4->getProperties())) == 1, 'Updates to a term are not adding properties correctly');
    $this->assertTrue(count($new_child4->getAltIds()) == 1, 'Updates to a term are not adding alt IDs correctly');
    $this->assertTrue(count(array_keys($new_child4->getSynonyms())) == 1, 'Updates to a term are not adding synonyms correctly');
    $this->assertTrue(count(array_keys($new_child4->getParents())) == 1, 'Updates to a term are not adding parents correctly');

    // Test updating of the boolean values.
    $new_child4->setIsObsolete(TRUE);
    $new_child4->setIsRelationshipType(TRUE);
    $go->saveTerm($new_child4);
    $new_child5 = $go->getTerm('0007610');
    $this->assertTrue($new_child5->isRelationshipType(), 'Updates to the relationship type did not get set when updating a term.');
    $this->assertTrue($new_child5->isObsolete(), 'Updates to the obsolete value did not get set when updating a term.');
    $new_child5->setIsObsolete(FALSE);
    $new_child5->setIsRelationshipType(FALSE);
    $go->saveTerm($new_child5);
    $new_child6 = $go->getTerm('0007610');
    $this->assertFalse($new_child6->isRelationshipType(), 'Updates to the relationship type did not get unset when updating a term.');
    $this->assertFalse($new_child6->isObsolete(), 'Updates to the obsolete value did not get unset when updating a term.');

    //
    // Finding Terms
    // .
    // Restore the parent to its full state.
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

    $go->saveTerm($parent);

    // Restore the child to its full state.
    $new_child = new TripalTerm([
      'name' => 'behavior',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'accession' => '0007610',
      'definition' => $new_child_def,
      'altIDs' => [
        ['GO', '0044709'],
        ['GO', '0023032'],
        ['GO', '0044708'],
      ],
      'synonyms' => [
        'behavioral response to stimulus',
        'behaviour',
        'behavioural response to stimulus',
        'single-organism behavior',
      ],
      'properties' => [
        [$comment, $new_child_comment],
      ],
      'parents' => [
        [$parent, $is_a],
      ],
    ]);
    $go->saveTerm($new_child);

    $terms = $go->getTerms('behav');
    $this->assertTrue(count(array_keys($terms)) == 4, 'Searching for a non exact term did not yield the correct number of matches.');
    $this->assertTrue(in_array('behavior', array_keys($terms)), 'Searching for a term did not return the matched name of a term.');
    $this->assertTrue(in_array('behavioral response to stimulus', array_keys($terms)), 'Searching for a term did not return the matched synonym of a term (Test #1).');
    $this->assertTrue(in_array('behavioural response to stimulus', array_keys($terms)), 'Searching for a term did not return the matched synonym of a term  (Test #2).');
    $this->assertTrue(in_array('behaviour', array_keys($terms)), 'Searching for a term did not return the matched synonym of a term  (Test #3).');
    $terms = $go->getTerms('behav', ['exact' => TRUE]);
    $this->assertTrue(count(array_keys($terms)) == 0, 'Searching for an exact term that does not match anything did not return 0 matches.');
    $terms = $go->getTerms('behavioral response to stimulus', ['exact' => TRUE]);
    $this->assertTrue(count(array_keys($terms)) == 1, 'Searching for an exact term using the synonym did not return a match.');
    $terms = $go->getTerms('biological');
    $this->assertTrue(count(array_keys($terms)) == 2, 'Searching for a non exact term that should match two terms did not.');

    //
    // Get Children
    // .
    // Restore the child to its full state.
    $child = new TripalTerm([
      'name' => 'biological phase',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'definition' => 'A distinct period or stage in a biological process or cycle.',
      'accession' => '0044848',
      'properties' => [
        [$comment, $child_comment],
      ],
      'parents' => [
        [$parent, $is_a],
      ],
    ]);
    $go->saveTerm($child);

    $children = $go->getChildren($parent);
    $this->assertTrue(count($children) == 2, 'The number of children returned for the parent is incorrect.');
    $child_names = [$children[0][0]->getName(), $children[1][0]->getName()];
    $this->assertTrue(in_array('behavior', $child_names), 'The list of children for the parent does not have a correct child name (Test #1).');
    $this->assertTrue(in_array('biological phase', $child_names), 'The list of children for the parent does not have a correct child name (Test #2).');
    $rel_types = [$children[0][1]->getName(), $children[1][1]->getName()];
    $this->assertTrue(in_array('is_a', $rel_types), 'The list of children relationship types for the parent does not have the correct type.');

    //
    // Testing synonym types.
    //
    // Create a type for the synonyms.
    $syn_vocab = $this->vmanager->createCollection("synonym_type", "chado_vocabulary");
    $this->assertIsObject($syn_vocab, 'Synonym vocabulary not created');
    $syn_id = $this->idsmanager->createCollection('synonym_type', "chado_id_space");
    $this->assertIsObject($syn_id, 'Synonym ID space not created');
    $exact = new TripalTerm();
    $exact->setName('exact');
    $exact->setIdSpace('synonym_type');
    $exact->setVocabulary('synonym_type');
    $exact->setAccession('exact');
    $syn_id->saveTerm($exact);

    $new_child = new TripalTerm([
      'name' => 'behavior',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'accession' => '0007610',
      'definition' => $new_child_def,
      'altIDs' => [
        ['GO', '0044709'],
        ['GO', '0023032'],
        ['GO', '0044708'],
      ],
      'synonyms' => [
        ['behavioral response to stimulus', $exact],
        ['behaviour', $exact],
        ['behavioural response to stimulus', $exact],
        ['single-organism behavior', $exact],
      ],
      'properties' => [
        [$comment, $new_child_comment],
      ],
      'parents' => [
        [$parent, $is_a],
      ],
    ]);
    $go->saveTerm($new_child);
    $new_child = $go->getTerm('0007610');
    $synonyms = $new_child->getSynonyms();
    $this->assertTrue(count(array_keys($synonyms)) == 4, 'The number of synonyms returned is not correct.');
    $this->assertTrue(in_array('behavioral response to stimulus', array_keys($synonyms)), 'The synonyms is missing (Test #1).');
    $this->assertTrue(in_array('behaviour', array_keys($synonyms)), 'The synonyms is missing (Test #2).');
    $this->assertTrue(in_array('behavioural response to stimulus', array_keys($synonyms)), 'The synonyms is missing (Test #3).');
    $this->assertTrue(in_array('single-organism behavior', array_keys($synonyms)), 'The synonyms is missing (Test #4).');
    $this->assertTrue($synonyms['behavioral response to stimulus']->getName() == 'exact', 'The synonym type is incorrect (Test #1).');
    $this->assertTrue($synonyms['behaviour']->getName() == 'exact', 'The synonym type is incorrect (Test #1).');
    $this->assertTrue($synonyms['behavioural response to stimulus']->getName() == 'exact', 'The synonym type is incorrect (Test #1).');
    $this->assertTrue($synonyms['single-organism behavior']->getName() == 'exact', 'The synonym type is incorrect (Test #1).');

    // Repeat the test,but adding the synonyms using setters.
    $new_child = new TripalTerm([
      'name' => 'behavior',
      'idSpace' => 'GO',
      'vocabulary' => 'biological_process',
      'accession' => '0007610',
      'definition' => $new_child_def,
      'altIDs' => [
        ['GO', '0044709'],
        ['GO', '0023032'],
        ['GO', '0044708'],
      ],
      'properties' => [
        [$comment, $new_child_comment],
      ],
      'parents' => [
        [$parent, $is_a],
      ],
    ]);
    $go->saveTerm($new_child);
    $new_child = $go->getTerm('0007610');
    $synonyms = $new_child->getSynonyms();
    $this->assertTrue(count($synonyms) == 0, 'There should be no synonyms but some were returned.');
    $new_child->addSynonym('behavioral response to stimulus', $exact);
    $new_child->addSynonym('behaviour', $exact);
    $go->saveTerm($new_child);
    $new_child = $go->getTerm('0007610');
    $synonyms = $new_child->getSynonyms();
    $this->assertTrue(in_array('behavioral response to stimulus', array_keys($synonyms)), 'The synonyms is missing after using the addSynonyms function (Test #3).');
    $this->assertTrue(in_array('behaviour', array_keys($synonyms)), 'The synonyms is missing after using the addSynonyms function (Test #2).');
    $this->assertTrue($synonyms['behavioral response to stimulus']->getName() == 'exact', 'The synonym type is incorrect after using the addSynonyms function (Test #1).');
    $this->assertTrue($synonyms['behaviour']->getName() == 'exact', 'The synonym type is incorrect after using the addSynonyms function (Test #1).');

    //
    // Saving an invalid term
    // .
    $dummy = new TripalTerm();
    // -- Test 1.
    $expected_message = 'ChadoIdSpace::saveTerm(). The term, ":" is not valid and cannot be saved.';
    $dummy->setName('dummy');
    ob_start();
    $this->assertFalse($go->saveTerm($dummy), 'An invalid term did not return False when saving (Test #1)');
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'An invalid term did not log expected message when saving (Test #1)'
    );

    // -- Test 2.
    $expected_message = 'ChadoIdSpace::saveTerm(). The term, ":" is not valid and cannot be saved.';
    $dummy->setDefinition('dummy');
    ob_start();
    $this->assertFalse($go->saveTerm($dummy), 'An invalid term did not return False when saving (Test #2)');
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'An invalid term did not log expected message when saving (Test #2)'
    );

    // -- Test 3.
    $expected_message = 'ChadoIdSpace::saveTerm(). The term, ":dummy" is not valid and cannot be saved.';
    $dummy->setAccession('dummy');
    ob_start();
    $this->assertFalse($go->saveTerm($dummy), 'An invalid term did not return False when saving (Test #3)');
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'An invalid term did not log expected message when saving (Test #3)'
    );

    // -- Test 4.
    $expected_message = 'ChadoIdSpace::saveTerm(). The term, "GO:dummy" is not valid and cannot be saved.';
    $dummy->setIdSpace('GO');
    ob_start();
    $this->assertFalse($go->saveTerm($dummy), 'An invalid term did not return False when saving (Test #4)');
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'An invalid term did not log expected message when saving (Test #4)'
    );

    $dummy->setVocabulary('biological_process');
    $this->assertTrue($go->saveTerm($dummy), 'A valid term did not return True when saving');

    // Try to save a term that doesn't belong to the idSpace.
    $expected_message = 'ChadoIdSpace::saveTerm(). The term, "GO:dummy", does not have the same ID space as this one.';
    ob_start();
    $success = $rdfs_id->saveTerm($dummy);
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'Should not be able to save a term that does not belong to the id space.'
    );
    $this->assertFalse($success, 'A term that did not belong to an idSpace should not have been saved.');
  }

  /**
   * Testing idSpace and Vocabulary collection with new db or cv.
   *
   * Importers may create new ID spaces and vocabularies.
   * Test that these work (issue #2032).
   */
  public function testIssue2032() {

    $chado = $this->getTestSchema();
    $db_name = 'imported_db';
    $cv_name = 'imported_cv';

    $query = $chado->insert('1:db');
    $query->fields(['name' => $db_name]);
    $db_id = $query->execute();
    $this->assertTrue(is_numeric($db_id), "Unable to insert new DB $db_name");

    $query = $chado->insert('1:cv');
    $query->fields(['name' => $cv_name]);
    $cv_id = $query->execute();
    $this->assertTrue(is_numeric($cv_id), "Unable to insert new vocabulary $cv_name");

    $imported_term = new TripalTerm();
    $this->assertIsObject($imported_term, "Failure to create an empty TripalTerm");
    $imported_term->setName('imported_term');
    $expected_message = "TripalTerm::setIdSpace(). The specified ID space, 'imported_db', does not exist";
    ob_start();
    $imported_term->setIdSpace($db_name);
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'Should not be able to set the IDspace because the plugin is not yet specified.'
    );
    $expected_message = "TripalTerm::setVocabulary(). The specified vocabulary, 'imported_cv' does not exist.";
    ob_start();
    $imported_term->setVocabulary($cv_name);
    $printed_output = ob_get_clean();
    $this->assertStringContainsString(
      $expected_message,
      $printed_output,
      'Should not be able to set the vocab because the plugin is not yet specified.'
    );
    $imported_term->setAccession('imported_term');
    // Expect not valid because plugins are not yet specified.
    $is_valid = $imported_term->isValid();
    $this->assertFalse($is_valid, "Term is valid but should not be valid");

    // Now specify the plugins and try again.
    $imported_term->setIdSpacePlugin('chado_id_space');
    $imported_term->setVocabularyPlugin('chado_vocabulary');
    $imported_term->setIdSpace($db_name);
    $imported_term->setVocabulary($cv_name);
    // Expect things to work now.
    $id_space = $imported_term->getIdSpace();
    $this->assertEquals($db_name, $id_space, "ID space does not match DB name");
    $vocabulary = $imported_term->getVocabulary();
    $this->assertEquals($cv_name, $vocabulary, "Vocabulary does not match CV name");
    $is_valid = $imported_term->isValid();
    $this->assertTrue($is_valid, "Term is not valid but should be");

  }

  /**
   * Testing idSpace and Vocabulary collection with missing db or cv.
   *
   * This can happen when a TripalTerm is created and then the underlying
   * cv/db is deleted.
   *
   * Tests that loading a collection whose underlying record is gone recreates
   * the record in the underlying backend (Issue #1354; PR #2185).
   */
  public function testIssue1354() {

    $cv_name = 'randomCV';
    $cv_txt = 'Describing my random VOCAB with mostly meaningless chatter';
    $db_name = 'randomDB';
    $db_txt = 'Describing my random ID SPACE with mostly meaningless chatter';

    // Create the backend records.
    $query = $this->chado_connection->insert('1:db');
    $query->fields(['name' => $db_name, 'description' => $db_txt]);
    $db_id = $query->execute();
    $this->assertTrue(is_numeric($db_id), "Unable to insert new DB $db_name");
    $query = $this->chado_connection->insert('1:cv');
    $query->fields(['name' => $cv_name, 'definition' => $cv_txt]);
    $cv_id = $query->execute();
    $this->assertTrue(is_numeric($cv_id), "Unable to insert new vocabulary $cv_name");

    // Now create the vocab and ID Space collections.
    $idspace = $this->idsmanager->createCollection($db_name, "chado_id_space");
    $this->assertIdSpaceEquals(
      ['name' => $db_name, 'description' => $db_txt, 'chado_record' => TRUE],
      $idspace,
      "ID Space created by createCollection(): "
    );
    $vocab = $this->vmanager->createCollection($cv_name, "chado_vocabulary");
    $this->assertVocabEquals(
      ['name' => $cv_name, 'definition' => $cv_txt, 'chado_record' => TRUE],
      $vocab,
      "Vocab created by createCollection(): "
    );

    // Delete the underlying cv/db from chado.
    // Confirm the idspace/vocab are unchanged but the chado record is gone.
    $this->cleanRecord('db', $db_name);
    $this->assertIdSpaceEquals(
      ['name' => $db_name, 'description' => '', 'chado_record' => FALSE],
      $idspace,
      "Chado DB record deleted: "
    );
    $this->cleanRecord('cv', $cv_name);
    $this->assertVocabEquals(
      ['name' => $cv_name, 'definition' => '', 'chado_record' => FALSE],
      $vocab,
      "Chado CV record deleted: "
    );

    // Now try to load the vocab and ID space.
    // Confirm the chado record was recreated but the description/definition
    // is no longer set because we didn't have it on load.
    $ret_idspace = $this->idsmanager->loadCollection($db_name, "chado_id_space");
    $this->assertIdSpaceEquals(
      ['name' => $db_name, 'description' => '', 'chado_record' => TRUE],
      $ret_idspace,
      "ID Space loaded with missing chado.db record: "
    );
    $ret_vocab = $this->vmanager->loadCollection($cv_name, "chado_vocabulary");
    $this->assertVocabEquals(
      ['name' => $cv_name, 'definition' => '', 'chado_record' => TRUE],
      $ret_vocab,
      "Vocab loaded with missing chado.cv record: "
    );

  }

  /**
   * Testing caching of terms in different ID spaces but same accession.
   *
   * Cache key should be unique for terms with the same accession
   * Issue #2328 PR #2330.
   */
  public function testIssue2328() {

    $db_name_1 = 'some_db_1';
    $db_name_2 = 'some_db_2';
    $cv_name_1 = 'some_cv_1';
    $cv_name_2 = 'some_cv_2';
    $common_accession = '0000042';

    $idspace_1 = $this->idsmanager->createCollection($db_name_1, 'chado_id_space');
    $this->assertIsObject($idspace_1, 'Failure to create an id space');
    $idspace_2 = $this->idsmanager->createCollection($db_name_2, 'chado_id_space');
    $this->assertIsObject($idspace_2, 'Failure to create an id space');
    $vocab_1 = $this->vmanager->createCollection($cv_name_1, 'chado_vocabulary');
    $this->assertIsObject($vocab_1, 'Failure to create a vocabulary');
    $vocab_2 = $this->vmanager->createCollection($cv_name_2, 'chado_vocabulary');
    $this->assertIsObject($vocab_2, 'Failure to create a vocabulary');

    $term_1 = new TripalTerm();
    $this->assertIsObject($term_1, 'Failure to create an empty TripalTerm');
    $term_1->setName('Name 1');
    $term_1->setIdSpacePlugin('chado_id_space');
    $term_1->setVocabularyPlugin('chado_vocabulary');
    $term_1->setIdSpace($db_name_1);
    $term_1->setVocabulary($cv_name_1);
    $term_1->setAccession($common_accession);
    $idspace_1->saveTerm($term_1);

    $term_2 = new TripalTerm();
    $this->assertIsObject($term_2, 'Failure to create an empty TripalTerm');
    $term_2->setName('Name 2');
    $term_2->setIdSpacePlugin('chado_id_space');
    $term_2->setVocabularyPlugin('chado_vocabulary');
    $term_2->setIdSpace($db_name_2);
    $term_2->setVocabulary($cv_name_2);
    $term_2->setAccession($common_accession);
    $idspace_2->saveTerm($term_2);

    // Tests caching of terms in different vocab but same accession.
    $term_1 = $idspace_1->getTerm($common_accession, []);
    $id_1 = $term_1->getTermId();
    $this->assertEquals('some_db_1:0000042', $id_1, 'Did not retrieve expected term ID');
    $term_2 = $idspace_2->getTerm($common_accession, []);
    $id_2 = $term_2->getTermId();
    $this->assertEquals('some_db_2:0000042', $id_2, 'Did not retrieve expected term ID');
  }

}
