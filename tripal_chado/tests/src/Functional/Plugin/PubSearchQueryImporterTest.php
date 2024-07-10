<?php

namespace Drupal\Tests\tripal_chado\Functional;
/**
 * Tests for the GFF3Importer class
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group GFF3Importer
 */
class PubSearchQueryImporterTest extends ChadoTestBrowserBase
{

  /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testPubSearchQueryImporterSimpleTest()
  {

    // Public schema connection
    $public = \Drupal::database();

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it
    $schema_name = $chado->getSchemaName();

    // We need to add a publication query to the database
    

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $plugin = $pub_library_manager->createInstance($plugin_id, []);
  }
}
?>