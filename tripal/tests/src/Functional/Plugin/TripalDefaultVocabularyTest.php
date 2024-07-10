<?php

namespace Drupal\Tests\tripal\Functional\Plugin;

use Drupal\Core\Database\Database;
use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Test\FunctionalTestSetupTrait;
use Drupal\tripal\TripalVocabTerms\TripalTerm;


/**
 * Tests for the TripalDefaultVocabulary classes
 *
 * @group Tripal
 * @group TripalDefaultVocabulary
 */
class TripalDefaultVocabularyTest extends TripalTestBrowserBase {

  /**
   * A helper function to retrieve a vocabulary record.
   *
   * @param string $vocabulary
   *   The name of the id_space to lookup.
   *
   * @return object
   *   A database query result.
   */
  protected function getVocabulary($vocabulary) {

    $conn = \Drupal::service('database');
    $query = $conn->select('tripal_terms_vocabs', 'vocab');
    $query = $query->condition('name', $vocabulary, '=');
    $query = $query->fields('vocab');
    $result = $query->execute();
    if (!$result) {
      return [];
    }
    return $result->fetchAssoc();
  }


  /**
   * A helper function to retrieve an id_space record.
   *
   * @param string $id_space
   *   The name of the id_space to lookup.
   *
   * @return object
   *   A database query result.
   */
  protected function getIdSpace($id_space) {

    $conn = \Drupal::service('database');
    $query = $conn->select('tripal_terms_idspaces', 'idspace');
    $query = $query->fields('idspace');
    $query = $query->condition('name', $id_space, '=');
    $result = $query->execute();
    if (!$result) {
      return [];
    }
    return $result->fetchAssoc();
  }


  /**
   * Tests the TripalDefaultIdSpace and TripalDefaultVocabulary Classes
   *
   */
  public function testTripalDefaultVocabulary() {

    // Create Collection managers.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');

    // These are the values we'll use for the ID space and vocablary.
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
    // Testing TripalVocabulary Functionality
    //

    // Make sure the Vocabulary does not yet exist.
    /** @var \Drupal\tripal\Plugin\TripalVocabulary\TripalDefaultVocabulary $cc */
    $cc = $vmanager->createCollection($GO_cc_namespace, "tripal_default_vocabulary");
    $cv = $this->getVocabulary($GO_cc_namespace);
    $this->assertTrue($cv['name'] == $GO_cc_namespace, 'The name was not set correctly by the TripalVocabulary object.');
    $this->assertEmpty($cv['label'], 'The label should not be set by the TripalVocabulary object just yet.');

    // Set the definition to make sure it gets set in the database.
    $cc->setLabel($GO_cc_label);
    $cv = $this->getVocabulary($GO_cc_namespace);
    $this->assertTrue($cv['name'] == $GO_cc_namespace, 'The name was not set correctly by the TripalVocabulary object.');
    $this->assertTrue($cv['label'] == $GO_cc_label, 'The label was not set correctly by the TripalVocabulary object.');

    // Make sure the getter works.
    $this->assertTrue($cc->getLabel() == $GO_cc_label, "The TripalVocabulary object did not return a correct label.");

    // Associate the IDSpace with the vocabulary,
    /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $GO */
    $GO = $idsmanager->createCollection($GO_idspace, "tripal_default_id_space");
    $id_spaces = $cc->getIdSpaceNames();
    $this->assertFalse(in_array($GO_idspace, $id_spaces), 'ID spaces should not be set yet in the TripalVocabulary');
    $cc->addIdSpace($GO_idspace);
    $id_spaces = $cc->getIdSpaceNames();
    $this->assertTrue(in_array($GO_idspace, $id_spaces), 'The ID space is missing from the TripalVocabulary');

    // Add a URL to the vocabulary, it should show up in the
    // database table for the ID space.
    $cv = $this->getVocabulary($GO_cc_namespace);
    $this->assertEmpty($cv['url'], 'The URL should not be set by the TripalVocabulary object just yet.');
    $cc->setURL($GO_url);
    $cv = $this->getVocabulary($GO_cc_namespace);
    $this->assertTrue($cv['url'] == $GO_url, 'The URL was not set correctly by the TripalVocabulary object.');
    $this->assertTrue($cc->getURL() == $GO_url, 'The URL was not retrieved by the TripalVocabulary object.');

    // Test adding a URL without an ID space.
    /** @var \Drupal\tripal\Plugin\TripalVocabulary\TripalDefaultVocabulary $bp */
    $bp = $vmanager->createCollection($GO_bp_namespace, "tripal_default_vocabulary");
    $bp->setLabel($GO_bp_label);
    $bp->setURL($GO_url);
    $this->assertFalse($bp->getURL() == $GO_url, 'The URL should not be set without an ID Space');

    // Test adding a default vocabulary to an ID space.  This should call the
    // addIdSpace() function on the vocabulary as well.
    $GO->setDefaultVocabulary($GO_bp_namespace);
    $this->assertTrue($GO->getDefaultVocabulary() == $GO_bp_namespace, 'The default vocabulary was not set correctly by the ChadoIdSpace object.');
    $bp->setURL($GO_url);
    $this->assertTrue($bp->getURL() == $GO_url, 'The URL was not set correctly by the TripalVocabulary after setting the default vocabulary.');

    //
    // Testing multiple ID spaces per Vocabulary
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

    /** @var \Drupal\tripal\Plugin\TripalVocabulary\TripalDefaultVocabulary $edam */
    $edam = $vmanager->createCollection($EDAM_namespace, "tripal_default_vocabulary");
    /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $edam_data */
    $edam_data = $idsmanager->createCollection($EDAM_data_idspace, "tripal_default_id_space");
    /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $edam_format */
    $edam_format = $idsmanager->createCollection($EDAM_format_idspace, "tripal_default_id_space");
    /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $edam_operation */
    $edam_operation = $idsmanager->createCollection($EDAM_operation_idspace, "tripal_default_id_space");
    /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $edam_topic */
    $edam_topic = $idsmanager->createCollection($EDAM_topic_idspace, "tripal_default_id_space");

    $edam_data->setDefaultVocabulary($EDAM_namespace);
    $edam_format->setDefaultVocabulary($EDAM_namespace);
    $edam_operation->setDefaultVocabulary($EDAM_namespace);
    $edam_topic->setDefaultVocabulary($EDAM_namespace);
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
  }
}
