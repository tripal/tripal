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
    //  -- Get a specific term based on it's unique accession.
    //  -- Get the parents or children of a given term.
	}

	/**
   * Basic tests for Tripal Term.
   *
   * @group TripalVocabTerms
   */
  public function testTripalTerm() {

    // Test the TripalTerm object.
    // --Test creation of a TripalTerm provided the expected input.
    //   NOTE: the use Drupal\tripal\TripalVocabTerms\TripalTerm
    //   at the top of this file allows us to use TripalTerm here.
    $term = new TripalTerm([
      'name' => 'gene',
      'definition' => 'A region (or regions) that includes all of the sequence elements necessary to encode a functional transcript. A gene may include regulatory regions, transcribed regions and/or other functional sequence regions.',
      'idSpace' => 'SO',
      'accession' => '0000704',
      'vocabulary' => 'sequence'
    ]);
    $this->assertIsObject(
      $term,
      'When trying to create a new TripalTerm, an object was not produced.'
    );
    $this->assertInstanceOf(
      TripalTerm::class,
      $term,
      'The term created was not of type TripalTerm.'
    );

    // --Test that we can pull out all the properties we set on creation.
    // @todo

    // --Test that we can set new properties and retrieve them.
    // @todo

    // --Test retrieving the default vocabulary object.
    // @todo

    // --Test retrieving the ID Space object.
    // @todo

    // --Test retrieving the attribution URL for a term.
    // @todo

    // --Test term equality.
    // @todo

    // --Test that you can save a term to permanent storage and then retrieve it.
    // @todo

    // --Test the static suggestTerms() functionalty.
    // @todo

	}
}
