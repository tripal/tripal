<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;

// Needed for TripalTerm related tests.
use Drupal\tripal\TripalVocabTerms\TripalTerm;

/**
 * Tests the basic functions of the Bulk PostgreSQL Schema Installer.
 *
 * @group Tripal
 * @group Tripal Database
 */
class TripalVocabTermPluginTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;
  protected $defaultTheme = 'stable';

  protected static $modules = ['tripal'];

	/**
   * Basic tests for Tripal Vocabulary Plugin.
   *
   * @group TripalVocabTerms
   */
  public function testTripalVocabPlugin() {

		// Test the Vocabulary Plugin Manager.
		// --Ensure we can instantiate the plugin manager.
		$type = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
		// Note: If the plugin manager is not found you will get a ServiceNotFoundException.
		$this->assertIsObject($type, 'A vocabulary plugin service object was not returned.');

		// --Use the plugin manager to get a list of available implementations.
		$plugin_definitions = $type->getDefinitions();
		$this->assertIsArray(
			$plugin_definitions,
			'Implementations of the vocabulary plugin should be returned in an array.'
		);

    // Note: We can't test this further
    // unless we make a default Tripal (non-Chado) implementation.
    // In your own implementation modules, here are some things you should test:
    //  -- Create a pre-configured instance of your plugin.
    //     $vocab = $type->createInstance('plugin_id', ['a' => 'b']);
    //  -- Get/Set properties of the vocabulary (i.e. name, description, url).
    //  -- Add/Remove ID Spaces from a specific vocabulary.
    //  -- Get the ID Spaces in a specific vocabulary.
    //  -- Retrieve terms in a specific vocabulary.

	}

	/**
   * Basic tests for Tripal Id Space Plugin.
   *
   * @group TripalVocabTerms
   */
  public function testTripalIdSpacePlugin() {

		// Test the Id Space Plugin Manager.
		// --Ensure we can instantiate the plugin manager.
		$type = \Drupal::service('tripal.collection_plugin_manager.idspace');
		// Note: If the plugin manager is not found you will get a ServiceNotFoundException.
		$this->assertIsObject($type, 'An id space plugin service object was not returned.');

		// --Use the plugin manager to get a list of available implementations.
		$plugin_definitions = $type->getDefinitions();
		$this->assertIsArray(
			$plugin_definitions,
			'Implementations of the id space plugin should be returned in an array.'
		);

    // Note: We can't test this further
    // unless we make a default Tripal (non-Chado) implementation.
    // In your own implementation modules, here are some things you should test:
    //  -- Create a pre-configured instance of your plugin.
    //     $idspace = $type->createInstance('plugin_id', ['a' => 'b']);
    //  -- Get/Set properties of the ID space (i.e. name, url prefix).
    //  -- Get/Set default vocabulary.
    //  -- Save/Remove terms from a specific id space.
    //  -- Get a list of terms within the id space with an exact or partial name.
    //  -- Get a specific term based on its unique accession.
    //  -- Get the parents or children of a given term.
	}

	/**
	 * A helper function to retrieve a term record.
	 *
	 * @param string $cvname
	 * @param string $cvterm_name
	 */
	protected function getCVterm($cvname, $cvterm_name) {

	  $conn = \Drupal::service('database');
	  $query = $conn->select('tripal_terms', 'tt');
	  $query = $query->fields('tt');
	  $query = $query->condition('vocabulary', $cvname, '=');
	  $query = $query->condition('name', $cvterm_name, '=');
	  $result = $query->execute();
	  if (!$result) {
	    return [];
	  }
	  return $result->fetchAssoc();
	}


	/**
   * Basic tests for Tripal Term.
   *
   * @group TripalVocabTerms
   */
  public function testTripalTerm() {

    // Create Collection managers.
    /** @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idsmanager */
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    /** @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager $vmanager */
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

    /** @var \Drupal\tripal\Plugin\TripalIdSpace\TripalDefaultIdSpace $GO */
    $GO = $idsmanager->createCollection($GO_idspace, "tripal_default_id_space");
    $GO->setDescription($GO_description);
    $GO->setURLPrefix($GO_urlprefix);

    /** @var \Drupal\tripal\Plugin\TripalVocabulary\TripalDefaultVocabulary $cc */
    $cc = $vmanager->createCollection($GO_cc_namespace, "tripal_default_vocabulary");
    $cc->setLabel($GO_cc_label);
    $cc->setURL($GO_url);

    /** @var \Drupal\tripal\Plugin\TripalVocabulary\TripalDefaultVocabulary $bp */
    $bp = $vmanager->createCollection($GO_bp_namespace, "tripal_default_vocabulary");
    $bp->setLabel($GO_bp_label);
    $bp->setURL($GO_url);

    /** @var \Drupal\tripal\Plugin\TripalVocabulary\TripalDefaultVocabulary $bb */
    $mf = $vmanager->createCollection($GO_mf_namespace, "tripal_default_vocabulary");
    $mf->setLabel($GO_mf_label);
    $mf->setURL($GO_url);

    $GO->setDefaultVocabulary($GO_cc_namespace);

    // First create a term for the comment property.
    $rdfs_vocab = $vmanager->createCollection("rdfs", "tripal_default_vocabulary");
    $rdfs_vocab->setLabel('Resource Description Framework Schema');
    $rdfs_vocab->setURL('https://www.w3.org/TR/rdf-schema/');
    $rdfs_id = $idsmanager->createCollection('rdfs', "tripal_default_id_space");
    $rdfs_id->setDescription('Resource Description Framework Schema	');
    $rdfs_id->setURLPrefix('http://www.w3.org/2000/01/rdf-schema#{accession}');
    $rdfs_id->setDefaultVocabulary('rdfs', 'tripal_default_vocabulary');
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

    // Re-run the same tests above on this recreated term.// Run a suite of tests on the term.
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
    $is_a->setIsRelationshipType(True);
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

    // --Test the static suggestTerms() functionality.
    // @todo

  }
}
