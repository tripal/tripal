<?php

namespace Drupal\Tests\tripal_chado\Functional\Plugin;

use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Confirm basic publication importer functionality.
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group PubImporter
 */
#[Group('tripal-importer')]
#[Group('chado-importer')]
#[group('importer-pub')]
#[Group('bio-pub')]
#[RunTestsInSeparateProcesses]
class TripalPubLibraryTest extends ChadoTestBrowserBase {

  /**
   * Confirm basic publication importer functionality.
   */
  public function testTripalPubLibraryTestSimpleTest() {

    // Installs up the chado with the test chado data.
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
    $public = \Drupal::service('database');

    // Keep track of the schema name in case we need it.
    $schema_name = $chado->getSchemaName();

    $pub_library_manager = \Drupal::service('tripal.pub_library');

    $pub_library_defs = $pub_library_manager->getDefinitions();
    $plugins = [];
    foreach ($pub_library_defs as $plugin_id => $def) {
      $plugin_key = $def['id'];
      $plugin_value = $def['label']->render();
      $plugins[$plugin_key] = $plugin_value;
    }
    asort($plugins);
    $this->assertEquals('NIH PubMed database', $plugins['tripal_pub_library_PMID']);

    $pub_library_options = $pub_library_manager->getLibraryOptions();
    $this->assertIsArray($pub_library_options, 'getLibraryOptions did not return an array');
    $this->assertGreaterThan(0, count($pub_library_options), 'getLibraryOptions did not return any options');
    $this->assertArrayHasKey('tripal_pub_library_PMID', $pub_library_options, 'getLibraryOptions results missing expected key');

    $plugin_id = 'tripal_pub_library_PMID';
    $plugin = $pub_library_manager->createInstance($plugin_id, []);

    $search_array = [
      'remote_db' => 'pubmed',
      'num_criteria' => 1,
      'loader_name' => 'ok',
      'disabled' => 0,
      'do_contact' => 0,
      // 'pub_import_id' => 25,
      'criteria' => [
        1 => [
          'search_terms' => 'Populus trichocarpa',
          'scope' => 'abstract',
          'is_phrase' => 0,
          'operation' => '',
        ],
      ],
    ];

    $results = $plugin->retrieve($search_array, 1, 0);
    if ($results === NULL) {
      $this->markTestSkipped('Skipping PubMed test due to being unable to access service.');
    }

    $this->assertGreaterThan(0, $results['total_records'], 'There should be more than 0 records found for this query');

    $pubs_count = count($results['pubs']);
    $this->assertEquals($pubs_count, 1);

    $this->assertNotNull($results['pubs'][0]['Title'], 'There should be a title but a title was not found');

    // Test for a BOOK type.
    $search_array = [
      'remote_db' => 'pubmed',
      'num_criteria' => 1,
      'loader_name' => 'ok2',
      'disabled' => 0,
      'do_contact' => 0,
      // 'pub_import_id' => 25,
      'criteria' => [
        1 => [
          'search_terms' => '30000852',
          'scope' => 'id',
          'is_phrase' => 0,
          'operation' => '',
        ],
      ],
    ];

    $results = $plugin->retrieve($search_array, 1, 0);
    if ($results === NULL) {
      $this->markTestSkipped('Skipping PubMed test due to being unable to access service.');
    }
    $this->assertEquals('30000852', $results['pubs'][0]['Publication Dbxref'], 'This should have returned the PMID');
    $this->assertEquals('National Institute of Child Health and Human Development', $results['pubs'][0]['Publisher'], 'This should have returned the Title');

    $search_array = [
      'remote_db' => 'pubmed',
      'num_criteria' => 1,
      'loader_name' => 'ok',
      'disabled' => 0,
      'do_contact' => 0,
      // 'pub_import_id' => 25,
      'criteria' => [
        1 => [
          'search_terms' => 'Populus trichocarpa',
          'scope' => 'abstract',
          'is_phrase' => 0,
          'operation' => '',
        ],
      ],
    ];
    $db_fields = [
      'name' => 'test-query',
      'criteria' => serialize($search_array),
      'disabled' => 0,
      'do_contact' => 0,
    ];
    // Add search query.
    $pub_library_manager->addSearchQuery($db_fields);
    $query = $public->select('tripal_pub_library_query', 'tplq');
    $query = $query->condition('name', 'test-query', '=');
    $query = $query->fields('tplq');
    $results = $query->execute();
    $this->assertNotNull($results, 'Tripal Pub Library Query tables contains no query by test-query, this is an error');
    $row = $results->fetchAssoc();
    $this->assertEquals('test-query', $row['name'], 'The Tripal Pub Library Query name is not test-query, this is an error');

    $query_id = $row['pub_library_query_id'];

    // --- Get search query test
    // Returns object.
    $row = $pub_library_manager->getSearchQuery($query_id);
    $this->assertEquals('test-query', $row->name,
      'The Tripal Pub Library Query name is not test-query, this is an error - getSearchQuery test error');

    // Get all search queries test.
    // Returns results.
    $results = $pub_library_manager->getSearchQueries();
    $this->assertNotNull($results,
      'Tripal Pub Library Query tables contains no query by test-query, this is an error - issue with getSearchQueries');
    $row = $results[0];
    $this->assertEquals('test-query', $row->name,
      'The Tripal Pub Library Query name is not test-query, this is an error - issue with getSearchQueries');

    // --- Update search query test
    $search_array = [
      'remote_db' => 'pubmed',
      'num_criteria' => 1,
      'loader_name' => 'ok',
      'disabled' => 0,
      'do_contact' => 0,
      // 'pub_import_id' => 25,
      'criteria' => [
        1 => [
          'search_terms' => 'Populus trichocarpa',
          'scope' => 'abstract',
          'is_phrase' => 0,
          'operation' => '',
        ],
      ],
    ];
    $db_fields = [
      'name' => 'test-query-updated',
      'criteria' => serialize($search_array),
      'disabled' => 0,
      'do_contact' => 0,
    ];

    // This should update the search query.
    $pub_library_manager->updateSearchQuery($query_id, $db_fields);
    // Returns object.
    $row = $pub_library_manager->getSearchQuery($query_id);
    $this->assertEquals('test-query-updated', $row->name,
       'The Tripal Pub Library Query name is not test-query-updated, this is an error - updateSearchQuery test error');

    // // --- Delete search query test
    $pub_library_manager->deleteSearchQuery($query_id);
    $query = $public->select('tripal_pub_library_query', 'tplq');
    $query = $query->condition('name', 'test-query-updated', '=');
    $query = $query->fields('tplq');
    $results = $query->execute();
    $row_count = 0;
    foreach ($results as $row) {
      $row_count++;
    }
    $this->assertEquals(0, $row_count, 'Tripal Pub Library Query tables contains test-query-updated, deleteSearchQuery test error');

  }

}
