<?php

namespace Drupal\Tests\tripal\Kernel\Controller\PubSearchQueryNameAutocompleteController;


use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\Controller\PubSearchQueryNameAutocompleteController;
use Symfony\Component\HttpFoundation\Request;


/**
 * Tests the Publication Search Query Name Autocomplete.
 *
 * @group Tripal
 * @group PubImporter
 * @group Autocomplete
 */
class PubSearchQueryNameAutocompleteControllerTest extends TripalTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'tripal', 'file'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installSchema('tripal', ['tripal_pub_library_query']);
  }

  /**
   * Tests the Publication Search Query Name Autocomplete.
   */
  public function testPubSearchQueryNameAutocompleteController() {

    // Create some records in the public.tripal_pub_library_query
    // table using two different plugin_id values
    $public = \Drupal::database();
    $plugin_ids = ['tripal_pub_library_PMID', 'another_plugin'];
    $names = ['abcdef', 'abcxyz', 'a12345', 'lmnopq'];
    $ids = [];
    foreach ($plugin_ids as $plugin_id) {
      foreach ($names as $name) {
        // Fake criteria only needs the values that the autocomplete looks at
        $criteria = serialize(['loader_name' => $name, 'plugin_id' => $plugin_id]);
        $query = $public->insert('tripal_pub_library_query');
        $query->fields([
          'name' => $name,
          'criteria' => $criteria,
          'disabled' => 0,
          'do_contact' => 0,
        ]);
        $ids[] = $query->execute();
      }
    }

    $autocomplete = new PubSearchQueryNameAutocompleteController();
    $this->assertIsObject($autocomplete, 'Failed to create the PubSearchQueryNameAutocompleteController');

    // Test for any plugin
    $request = Request::create(
      'admin/tripal/autocomplete/pubsearchqueryname',
      'GET',
      ['db' => '*', 'q' => 'a']
    );
    $results = $autocomplete->handleAutocomplete($request, 10, 0)->getContent();
    $this->assertCount(6, json_decode($results), 'Expected exactly six results from "a" autocomplete for all plugins');

    // Test for a single plugin
    $request = Request::create(
      'admin/tripal/autocomplete/pubsearchqueryname',
      'GET',
      ['db' => 'tripal_pub_library_PMID', 'q' => 'a']
    );
    $results = $autocomplete->handleAutocomplete($request, 10, 0)->getContent();
    $this->assertCount(3, json_decode($results), 'Expected exactly three results from "a" autocomplete for pubmed plugin');

    // Test internal string
    $request = Request::create(
      'admin/tripal/autocomplete/pubsearchqueryname',
      'GET',
      ['db' => 'tripal_pub_library_PMID', 'q' => 'mnop']
    );
    $results = $autocomplete->handleAutocomplete($request, 10, 0)->getContent();
    $this->assertCount(1, json_decode($results), 'Expected exactly one result from "mnop" autocomplete for pubmed plugin');

    // Test nonmatching string
    $request = Request::create(
      'admin/tripal/autocomplete/pubsearchqueryname',
      'GET',
      ['db' => 'tripal_pub_library_PMID', 'q' => 'r']
    );
    $results = $autocomplete->handleAutocomplete($request, 10, 0)->getContent();
    $this->assertCount(0, json_decode($results), 'Expected no results from "r" autocomplete for pubmed plugin');

    // Test empty string
    $request = Request::create(
      'admin/tripal/autocomplete/pubsearchqueryname',
      'GET',
      ['db' => 'tripal_pub_library_PMID', 'q' => 'r']
    );
    $results = $autocomplete->handleAutocomplete($request, 10, 0)->getContent();
    $this->assertCount(0, json_decode($results), 'Expected no results from empty string autocomplete for pubmed plugin');

  }
}
