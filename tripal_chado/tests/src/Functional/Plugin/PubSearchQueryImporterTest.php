<?php

namespace Drupal\Tests\tripal_chado\Functional;

/**
 * Tests for the PubSearchQueryImporter class
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group PubSearchQueryImporter
 */
class PubSearchQueryImporterTest extends ChadoTestBrowserBase
{

  /**
   * Confirm basic Publications importer functionality.
   *
   * @group pub
   */
  public function testPubSearchQueryImporterSimpleTest()
  {
    $this->assertNotEquals(1, 0);
    // Public schema connection
    $public = \Drupal::database();

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it
    $schema_name = $chado->getSchemaName();

    // $tripal_pub_library_query_schema = [
    //   'fields' => [
    //     'pub_library_query_id' => [
    //       'type' => 'serial',
    //       'not null' => TRUE
    //     ],
    //     'name' => [
    //       'type' => 'varchar',
    //       'length' => 255,
    //       'not null' => TRUE
    //     ],
    //     'criteria' => [
    //       'type' => 'text',
    //       'size' => 'normal',
    //       'not null' => TRUE,
    //       'description' => 'Contains a serialized PHP array containing the search criteria'
    //     ],
    //     'disabled'  => [
    //       'type' => 'int',
    //       'unsigned' => TRUE,
    //       'not NULL' => TRUE,
    //       'default' => 0
    //     ],
    //     'do_contact'  => [
    //       'type' => 'int',
    //       'unsigned' => TRUE,
    //       'not NULL' => TRUE,
    //       'default' => 0
    //     ],
    //   ],
    //   'primary key' => ['pub_library_query_id'],
    //   'indexes' => [
    //     'name' => ['name']
    //   ],
    // ];

    // Create the tripal_pub_library_query in the test public schema
    // $public->schema()->createTable('tripal_pub_library_query', $tripal_pub_library_query_schema);

    // We need to add a publication query to the database
    $sql = "INSERT INTO {tripal_pub_library_query} (name,criteria) VALUES (:name,:criteria);";
    $args = [
      ':name' => 'Populus-PHPUNIT-TEST',
      ':criteria' => 'a:9:{s:9:"remote_db";s:6:"pubmed";s:12:"num_criteria";s:1:"1";s:11:"loader_name";s:7:"Populus";s:8:"disabled";i:0;s:10:"do_contact";i:0;s:13:"pub_import_id";N;s:8:"criteria";a:1:{i:1;a:4:{s:12:"search_terms";s:7:"Populus";s:5:"scope";s:5:"title";s:9:"is_phrase";i:0;s:9:"operation";s:0:"";}}s:21:"form_state_user_input";a:12:{s:9:"plugin_id";s:25:"tripal_pub_library_pubmed";s:11:"button_next";s:4:"Next";s:11:"loader_name";s:7:"Populus";s:12:"ncbi_api_key";s:0:"";s:4:"days";s:0:"";s:12:"num_criteria";s:1:"1";s:5:"table";a:1:{i:1;a:4:{s:11:"operation-1";s:0:"";s:7:"scope-1";s:5:"title";s:14:"search_terms-1";s:7:"Populus";s:11:"is_phrase-1";N;}}s:13:"form_build_id";s:48:"form-UpjBwJmfHyqAeLFwZqHbVhpvtgcBvgEez31-4KJ9jUA";s:10:"form_token";s:43:"FIxhzP6k7V1ruQoEoDzVCKVOt97wfbvGypPBPGFx13M";s:7:"form_id";s:31:"chado_new_pub_search_query_form";s:8:"disabled";N;s:10:"do_contact";N;}s:4:"days";s:0:"";}'
    ];
    $public->query($sql,$args);

    $results = $public->query("SELECT * FROM {tripal_pub_library_query} WHERE name = 'Populus-PHPUNIT-TEST';");
    $query_id = NULL;
    foreach ($results as $row) {
      $query_id = intval($row->pub_library_query_id);
    }
    $this->assertEquals($query_id, 1, 'This should have returned a query ID equal to 1 but did not');
    // print_r('Query ID: ' . $query_id . "\n");

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_record = $pub_library_manager->getSearchQuery(intval($query_id));
    // print_r("Pub Record:\n");
    // print_r($pub_record);

    $criteria = unserialize($pub_record->criteria);
    $this->assertEquals($criteria['form_state_user_input']['plugin_id'], 'tripal_pub_library_pubmed', 'This should have returned the plugin id as tripal_pub_library_pubmed but did not');
    // print_r($criteria);
    $plugin_id = $criteria['form_state_user_input']['plugin_id'];
    
    $plugin = $pub_library_manager->createInstance($plugin_id, []);
    $results = $plugin->retrieve($criteria, 1, 0);
    // This should return a single pub since we used the limit 1 in the retrieve function
    $pub_count = count($results['pubs']);
    $this->assertEquals($pub_count, 1, 'One publication should have been retrieved but was not');
    


    // Specific PMID
    $criteria_serialized = 'a:9:{s:9:"remote_db";s:6:"pubmed";s:12:"num_criteria";s:1:"1";s:11:"loader_name";s:13:"PMID:39125884";s:8:"disabled";i:0;s:10:"do_contact";i:0;s:13:"pub_import_id";N;s:8:"criteria";a:1:{i:1;a:4:{s:12:"search_terms";s:13:"PMID:39125884";s:5:"scope";s:2:"id";s:9:"is_phrase";i:0;s:9:"operation";s:0:"";}}s:21:"form_state_user_input";a:13:{s:9:"plugin_id";s:25:"tripal_pub_library_pubmed";s:11:"button_next";s:4:"Next";s:11:"loader_name";s:13:"PMID:39125884";s:12:"ncbi_api_key";s:0:"";s:4:"days";s:0:"";s:12:"num_criteria";s:1:"1";s:5:"table";a:1:{i:1;a:4:{s:11:"operation-1";s:0:"";s:7:"scope-1";s:2:"id";s:14:"search_terms-1";s:13:"PMID:39125884";s:11:"is_phrase-1";N;}}s:13:"form_build_id";s:48:"form-aL6YIsiQvl_GAXbQwYymTZaMm4PZrWeHpNcNdSBW_84";s:10:"form_token";s:43:"_PQ4ccPhMHXx3llqAKiOvclk7BJmv0RrMvJkZAx50ws";s:7:"form_id";s:31:"chado_new_pub_search_query_form";s:8:"disabled";N;s:10:"do_contact";N;s:18:"test_results_table";N;}s:4:"days";s:0:"";}';
    // We need to add a publication query for this specific query to the database
    $sql = "INSERT INTO {tripal_pub_library_query} (name,criteria) VALUES (:name,:criteria);";
    $args = [
      ':name' => 'PMID:39125884-PHPUNIT-TEST',
      ':criteria' => $criteria_serialized
    ];
    $public->query($sql,$args);

    $results = $public->query("SELECT * FROM {tripal_pub_library_query} WHERE name = 'PMID:39125884-PHPUNIT-TEST';");
    $query_id = NULL;
    foreach ($results as $row) {
      $query_id = intval($row->pub_library_query_id);
    }
    $this->assertEquals($query_id, 2, 'This should have returned a query ID equal to 2 but did not');
    // print_r('Query ID: ' . $query_id . "\n");

  
    $pub_record = $pub_library_manager->getSearchQuery(intval($query_id));
    $criteria = unserialize($pub_record->criteria);
    // Perform a lookup for the PMID:39125884
    $results = $plugin->retrieve($criteria, 1, 0);

    // This should return a single pub since we used the limit 1 in the retrieve function
    $pub_count = count($results['pubs']);
    $this->assertEquals(1, $pub_count, 'One publication should have been retrieved but was not');
    $this->assertEquals('PMID:39125884', $results['pubs'][0]['Publication Dbxref'], 'Publication Dbxref should have been PMID:39125884 but it is not');
    $this->assertEquals('10.3390/ijms25158314', $results['pubs'][0]['DOI'], 'DOI should have been 10.3390/ijms25158314 but it is not - parsing issue?');
    $this->assertEquals('2024', $results['pubs'][0]['Year'], 'Year should have been 2024 but it is not - parsing issue?');
    $this->assertEquals('Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus.', $results['pubs'][0]['Title'], 'Title should have been Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus. but it is not - parsing issue?');
    $this->assertEquals('Yang X, Zhu P, Gui J. Advancements of CRISPR-Mediated Base Editing in Crops and Potential Applications in Populus.. International journal of molecular sciences. 2024 Jul 30; 25(15).', $results['pubs'][0]['Citation'], 'Citation does not look correct, review test for details');
    $this->assertGreaterThan(2, count($results['pubs'][0]['Author List']), 'Author List should have more than 2 elements but does not');
    // print_r($results);

    // TODO: Perform an actual import with the importer
    $importer_manager = \Drupal::service('tripal.importer');
    $pub_search_query_loader_importer = $importer_manager->createInstance('pub_search_query_loader');
    // $run_args = [
    //   'importer_plugin_id' => 'pub_search_query_loader',
    //   'schema_name' => 'chado',
    //   'query_id' => 1,
    //   'database' => 'tripal_pub_library_pubmed',
    //   'search_query_name' => 'Populus (1)',
    //   'button_view_query_details' => 'Preview query details',
    //   'pub_query_details' => ''
    // ];
  }
}
?>