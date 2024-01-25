<?php

namespace Drupal\Tests\tripal_chado\Functional;

class TripalPubLibraryTest extends ChadoTestBrowserBase {
  /**
   * Confirm basic Taxonomy importer functionality.
   *
   * @group taxonomy
   */
  public function testTripalPubLibraryTestSimpleTest() {

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    $pub_library_manager = \Drupal::service('tripal.pub_library');

    $pub_library_defs = $pub_library_manager->getDefinitions();
    $plugins = [];
    foreach ($pub_library_defs as $plugin_id => $def) {
      $plugin_key = $def['id'];
      $plugin_value = $def['label']->render();
      $plugins[$plugin_key] = $plugin_value;
    }
    asort($plugins);
    $this->assertEquals($plugins['tripal_pub_library_pubmed'], 'NIH PubMed database');
    

    $plugin_id = 'tripal_pub_library_pubmed';
    $plugin = $pub_library_manager->createInstance($plugin_id, []);


    $search_array = [
      'remote_db' => 'pubmed',
      'num_criteria' => 1,
      'loader_name' => 'ok',
      'disabled' => 0,
      'do_contact' => 0,
      'pub_import_id' => 25,
      'criteria' => [
        1 =>   [
          'search_terms' => 'Populus trichocarpa',
          'scope' => 'abstract',
          'is_phrase' => 0,
          'operation' => '', 
        ]
      ],
    ];

    $results = $plugin->remoteSearchPMID($search_array, 1, 1);
    $this->assertNotEquals($results, NULL, 'This should have returned one pubmed record');
    
    $this->assertGreaterThan(0, $results['total_records'], 'There should be more than 0 records found for this query');

    $pubs_count = count($results['pubs']);
    $this->assertEquals($pubs_count, 1);

    $this->assertNotEquals($results['pubs'][0]['Title'], NULL, 'There should be a title but a title was not found');

  }
}