<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal\Services\TripalLogger;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests for the PubSearchQueryImporter class.
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group PubImporter
 * @group PubSearchQueryImporter
 */
#[Group('tripal-importer')]
#[Group('chado-importer')]
#[group('importer-pub')]
#[Group('bio-pub')]
#[RunTestsInSeparateProcesses]
class PubSearchQueryImporterTest extends ChadoTestBrowserBase {

  /**
   * @var string
   *   The most recent error message from the mocked tripal logger
   */
  protected string $mock_error = '';

  /**
   *
   */
  protected function setUp() :void {
    parent::setUp();

    // Grab the container.
    $container = \Drupal::getContainer();

    // Create a mocked logger so we can access error messages from the Tripal logger.
    $mock_logger = $this->getMockBuilder(TripalLogger::class)
      ->onlyMethods(['error'])
      ->getMock();
    $mock_logger->method('error')
      ->willReturnCallback(function ($message, $context, $options) {
          $this->mock_error .= str_replace(array_keys($context), $context, $message);
          return NULL;
      });
    $container->set('tripal.logger', $mock_logger);
  }

  /**
   * Confirm basic Publications importer functionality.
   */
  public function testPubSearchQueryImporterSimpleTest() {
    // Public schema connection.
    $public = \Drupal::database();

    // Installs up the chado with the test chado data.
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it.
    $schema_name = $chado->getSchemaName();

    // We need to add a publication query to the database.
    $sql = "INSERT INTO {tripal_pub_library_query} (name,criteria) VALUES (:name,:criteria);";
    $args = [
      ':name' => 'Populus-PHPUNIT-TEST',
      ':criteria' => 'a:9:{s:9:"remote_db";s:4:"PMID";s:12:"num_criteria";s:1:"1";s:11:"loader_name";s:7:"Populus";s:8:"disabled";i:0;s:10:"do_contact";i:0;s:13:"pub_import_id";N;s:8:"criteria";a:1:{i:1;a:4:{s:12:"search_terms";s:7:"Populus";s:5:"scope";s:5:"title";s:9:"is_phrase";i:0;s:9:"operation";s:0:"";}}s:21:"form_state_user_input";a:12:{s:9:"plugin_id";s:23:"tripal_pub_library_PMID";s:11:"button_next";s:4:"Next";s:11:"loader_name";s:7:"Populus";s:12:"ncbi_api_key";s:0:"";s:4:"days";s:0:"";s:12:"num_criteria";s:1:"1";s:5:"table";a:1:{i:1;a:4:{s:11:"operation-1";s:0:"";s:7:"scope-1";s:5:"title";s:14:"search_terms-1";s:7:"Populus";s:11:"is_phrase-1";N;}}s:13:"form_build_id";s:48:"form-UpjBwJmfHyqAeLFwZqHbVhpvtgcBvgEez31-4KJ9jUA";s:10:"form_token";s:43:"FIxhzP6k7V1ruQoEoDzVCKVOt97wfbvGypPBPGFx13M";s:7:"form_id";s:31:"chado_new_pub_search_query_form";s:8:"disabled";N;s:10:"do_contact";N;}s:4:"days";s:0:"";}',
    ];
    $public->query($sql, $args);

    $results = $public->query("SELECT * FROM {tripal_pub_library_query} WHERE name = 'Populus-PHPUNIT-TEST';");
    $query_id = NULL;
    foreach ($results as $row) {
      $query_id = intval($row->pub_library_query_id);
    }
    $this->assertEquals($query_id, 1, 'This should have returned a query ID equal to 1 but did not');

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_record = $pub_library_manager->getSearchQuery(intval($query_id));

    $criteria = unserialize($pub_record->criteria);
    $this->assertEquals($criteria['form_state_user_input']['plugin_id'], 'tripal_pub_library_PMID', 'This should have returned the plugin id as tripal_pub_library_PMID but did not');
    $plugin_id = $criteria['form_state_user_input']['plugin_id'];

    $plugin = $pub_library_manager->createInstance($plugin_id, []);

    $this->mock_error = '';
    $results = $plugin->retrieve($criteria, 1, 0);
    // We will have an error message in the logger if there was an intermittent download problem.
    if ($this->mock_error) {
      $this->markTestSkipped('Test skipped due to network error: ' . $this->mock_error);
    }
    else {
      // This should return a single pub since we used the limit 1 in the retrieve function.
      $pub_count = count($results['pubs']);
      $this->assertEquals($pub_count, 1, 'One publication should have been retrieved but was not');
    }

    // Specific PMID.
    $criteria_serialized = 'a:9:{s:9:"remote_db";s:4:"PMID";s:12:"num_criteria";s:1:"1";s:11:"loader_name";s:13:"PMID:39125884";s:8:"disabled";i:0;s:10:"do_contact";i:0;s:13:"pub_import_id";N;s:8:"criteria";a:1:{i:1;a:4:{s:12:"search_terms";s:13:"PMID:39125884";s:5:"scope";s:2:"id";s:9:"is_phrase";i:0;s:9:"operation";s:0:"";}}s:21:"form_state_user_input";a:13:{s:9:"plugin_id";s:23:"tripal_pub_library_PMID";s:11:"button_next";s:4:"Next";s:11:"loader_name";s:13:"PMID:39125884";s:12:"ncbi_api_key";s:0:"";s:4:"days";s:0:"";s:12:"num_criteria";s:1:"1";s:5:"table";a:1:{i:1;a:4:{s:11:"operation-1";s:0:"";s:7:"scope-1";s:2:"id";s:14:"search_terms-1";s:13:"PMID:39125884";s:11:"is_phrase-1";N;}}s:13:"form_build_id";s:48:"form-aL6YIsiQvl_GAXbQwYymTZaMm4PZrWeHpNcNdSBW_84";s:10:"form_token";s:43:"_PQ4ccPhMHXx3llqAKiOvclk7BJmv0RrMvJkZAx50ws";s:7:"form_id";s:31:"chado_new_pub_search_query_form";s:8:"disabled";N;s:10:"do_contact";N;s:18:"test_results_table";N;}s:4:"days";s:0:"";}';
    // We need to add a publication query for this specific query to the database.
    $sql = "INSERT INTO {tripal_pub_library_query} (name,criteria) VALUES (:name,:criteria);";
    $args = [
      ':name' => 'PMID:39125884-PHPUNIT-TEST',
      ':criteria' => $criteria_serialized,
    ];
    $public->query($sql, $args);

    $results = $public->query("SELECT * FROM {tripal_pub_library_query} WHERE name = 'PMID:39125884-PHPUNIT-TEST';");
    $query_id = NULL;
    foreach ($results as $row) {
      $query_id = intval($row->pub_library_query_id);
    }
    $this->assertEquals($query_id, 2, 'This should have returned a query ID equal to 2 but did not');

    $pub_record = $pub_library_manager->getSearchQuery(intval($query_id));
    $criteria = unserialize($pub_record->criteria);
    // Perform a lookup for the PMID:39125884.
    $this->mock_error = '';
    $results = $plugin->retrieve($criteria, 1, 0);
    // We will have an error message in the logger if there was an intermittent download problem.
    if ($this->mock_error) {
      $this->markTestSkipped('Test skipped due to network error: ' . $this->mock_error);
    }
    else {
      // This should return a single pub since we used the limit 1 in the retrieve function.
      $pub_count = count($results['pubs']);
      $this->assertEquals(1, $pub_count, 'One publication should have been retrieved but was not');
      $this->assertEquals('39125884', $results['pubs'][0]['Publication Dbxref'], 'Publication Dbxref should have been 39125884 but it is not');
      $this->assertEquals('10.3390/ijms25158314', $results['pubs'][0]['DOI'], 'DOI should have been 10.3390/ijms25158314 but it is not - parsing issue?');
      $this->assertEquals('2024', $results['pubs'][0]['Year'], 'Year should have been 2024 but it is not - parsing issue?');
      $this->assertEquals('Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus.', $results['pubs'][0]['Title'], 'Title should have been Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus. but it is not - parsing issue?');
      $this->assertEquals('Yang X, Zhu P, Gui J. Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus. International journal of molecular sciences. 2024 Jul 30; 25(15).', $results['pubs'][0]['Citation'], 'Citation does not look correct, review test for details');
      $this->assertGreaterThan(2, count($results['pubs'][0]['Author List']), 'Author List should have more than 2 elements but does not');
    }
    // Perform an actual import with the importer (on our second query - the one above to see if props get imported in)
    $importer_manager = \Drupal::service('tripal.importer');
    $pub_search_query_loader_importer = $importer_manager->createInstance('pub_search_query_loader');
    $run_args = [
      'importer_plugin_id' => 'pub_search_query_loader',
      'schema_name' => $schema_name,
      'query_id' => 2,
    ];
    $pub_search_query_loader_importer->createImportJob($run_args);
    $this->mock_error = '';
    $able_to_run = $pub_search_query_loader_importer->run();
    if ($this->mock_error) {
      $this->markTestSkipped('Test skipped due to network error: ' . $this->mock_error);
    }
    $pub_records = $chado->query("SELECT * FROM {1:pub}", []);
    $pub_record = NULL;
    foreach ($pub_records as $row) {
      $pub_record = $row;
    }

    $this->assertNotEquals($pub_record, NULL, 'No publication record could be found in the chado pub table
    even though an import was executed');

    $this->assertEquals('Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus.', $pub_record->title, 'Publication title is different');
    $this->assertEquals('International journal of molecular sciences', $pub_record->series_name, 'Series name is different');
    $this->assertEquals('2024', $pub_record->pyear, 'Publication year is different');

    $pub_id = $pub_record->pub_id;
    $pub_props = $chado->query("SELECT count(*) as c1 FROM {1:pubprop} WHERE pub_id = :pub_id", [
      ':pub_id' => $pub_id,
    ]);
    $row_count = NULL;
    foreach ($pub_props as $row) {
      $row_count = $row->c1;
    }

    $this->assertGreaterThan(0, $row_count, 'No properties were found in pubprop, this is an error');

    $pub_props = $chado->query("SELECT count(*) as c1 FROM {1:pubprop} WHERE pub_id = :pub_id AND value = :value", [
      ':pub_id' => $pub_id,
      ':value' => '39125884',
    ]);
    $row_count = NULL;
    foreach ($pub_props as $row) {
      $row_count = $row->c1;
    }
    $this->assertGreaterThan(0, $row_count, 'Publication ID was not found in pubprop table');

    $pub_props = $chado->query("SELECT count(*) as c1 FROM {1:pubprop} WHERE pub_id = :pub_id AND value = :value", [
      ':pub_id' => $pub_id,
      ':value' => 'International journal of molecular sciences',
    ]);
    $row_count = NULL;
    foreach ($pub_props as $row) {
      $row_count = $row->c1;
    }
    $this->assertGreaterThan(0, $row_count, 'Journal name was not found in pubprop table');

    $pub_props = $chado->query("SELECT count(*) as c1 FROM {1:pubprop} WHERE pub_id = :pub_id AND value = :value", [
      ':pub_id' => $pub_id,
      ':value' => '10.3390/ijms25158314',
    ]);
    $row_count = NULL;
    foreach ($pub_props as $row) {
      $row_count = $row->c1;
    }
    $this->assertGreaterThan(0, $row_count, 'Publication DOI was not found in pubprop table');

    $pub_props = $chado->query("SELECT count(*) as c1 FROM {1:pubprop} WHERE pub_id = :pub_id AND value = :value", [
      ':pub_id' => $pub_id,
      ':value' => 'Yang X, Zhu P, Gui J',
    ]);
    $row_count = NULL;
    foreach ($pub_props as $row) {
      $row_count = $row->c1;
    }
    $this->assertGreaterThan(0, $row_count, 'Authors were not found in pubprop table');

    $pub_props = $chado->query("SELECT count(*) as c1 FROM {1:pubprop} WHERE pub_id = :pub_id AND value = :value", [
      ':pub_id' => $pub_id,
      ':value' => 'Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus.',
    ]);
    $row_count = NULL;
    foreach ($pub_props as $row) {
      $row_count = $row->c1;
    }
    $this->assertGreaterThan(0, $row_count, 'Title was not found in pubprop table');

    // Tests supplying a criteria array directly to the publication importer and
    // running it directly. This is the use case for a different importer wanting
    // to be able to import a publication using its pubmed ID number.
    $pmid = '91111';
    $title = 'Lysozyme-induced T-suppressor cells and antibodies have a predominant idiotype.';
    $arguments = [
      'run_args' => [
        'criteria' => [
          'criteria' => [
            1 => [
              'search_terms' => $pmid,
              'scope' => 'id',
              'is_phrase' => 0,
              'operation' => '',
            ],
          ],
          'days' => '',
          'disabled' => 0,
          'do_contact' => 0,
          'form_state_user_input' => [
            'plugin_id' => 'tripal_pub_library_PMID',
          ],
          'loader_name' => 'internal',
          'num_criteria' => 1,
          'remote_db' => 'PMID',
          'pub_import_id' => NULL,
        ],
        'schema_name' => $schema_name,
      ],
    ];
    // We need a clean instance for the test.
    $pub_search_query_loader_importer = $importer_manager->createInstance('pub_search_query_loader');
    $pub_search_query_loader_importer->setArguments($arguments);
    $this->mock_error = '';
    $result = $pub_search_query_loader_importer->run();
    if ($this->mock_error) {
      $this->markTestSkipped('Test skipped due to network error: ' . $this->mock_error);
    }
    $pub = $chado->select('1:pub', 'p')
      ->fields('p')
      ->condition('p.title', $title)
      ->execute()
      ->fetchObject();
    $this->assertIsObject($pub, "Publication for PMID $pmid was not imported as expected");
    $this->assertEquals('Nature', $pub->series_name, "Publication for PMID $pmid has an incorrect series_name");
  }

}
