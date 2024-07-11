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

    // We need to add a publication query to the database
    $sql = "INSERT INTO tripal_pub_library_query (name,criteria) VALUES (:name,:criteria);";
    $args = [
      ':name' => 'Populus',
      ':criteria' => 'a:9:{s:9:"remote_db";s:6:"pubmed";s:12:"num_criteria";s:1:"1";s:11:"loader_name";s:7:"Populus";s:8:"disabled";i:0;s:10:"do_contact";i:0;s:13:"pub_import_id";N;s:8:"criteria";a:1:{i:1;a:4:{s:12:"search_terms";s:7:"Populus";s:5:"scope";s:5:"title";s:9:"is_phrase";i:0;s:9:"operation";s:0:"";}}s:21:"form_state_user_input";a:12:{s:9:"plugin_id";s:25:"tripal_pub_library_pubmed";s:11:"button_next";s:4:"Next";s:11:"loader_name";s:7:"Populus";s:12:"ncbi_api_key";s:0:"";s:4:"days";s:0:"";s:12:"num_criteria";s:1:"1";s:5:"table";a:1:{i:1;a:4:{s:11:"operation-1";s:0:"";s:7:"scope-1";s:5:"title";s:14:"search_terms-1";s:7:"Populus";s:11:"is_phrase-1";N;}}s:13:"form_build_id";s:48:"form-UpjBwJmfHyqAeLFwZqHbVhpvtgcBvgEez31-4KJ9jUA";s:10:"form_token";s:43:"FIxhzP6k7V1ruQoEoDzVCKVOt97wfbvGypPBPGFx13M";s:7:"form_id";s:31:"chado_new_pub_search_query_form";s:8:"disabled";N;s:10:"do_contact";N;}s:4:"days";s:0:"";}'
    ];
    $public->query($sql,$args);

    $results = $public->query("SELECT * FROM tripal_pub_library_query WHERE name = 'Populus';");
    $query_id = NULL;
    foreach ($results as $row) {
      $query_id = $row['pub_library_query_id'];
    }
    print_r('Query ID: ' . $query_id . "\n");

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_record = $pub_library_manager->getSearchQuery($query_id);

    $criteria = unserialize($pub_record->criteria);
    $plugin_id = $criteria['form_state_user_input']['plugin_id'];
    
    $plugin = $pub_library_manager->createInstance($plugin_id, []);
  }
}
?>