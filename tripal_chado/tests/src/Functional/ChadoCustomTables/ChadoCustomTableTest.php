<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;

/**
 * Tests the base functionality for chado custom tables.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Custom Tables
 */
class ChadoCustomTablesTest extends ChadoTestBrowserBase {

  /**
   * Tests focusing on the Tripal Importer plugin system and chado importers.
   *
   * @group service manager
   */
  public function testManager() {
    $manager = \Drupal::service('tripal_chado.custom_tables');
    $this->assertIsObject($manager, 'Able to retrieve the custom table service manager.');

    $chado = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);
    $chado_schema_name = $chado->getSchemaName();

    // Test manager get list of chado custom tables.
    $custom_tables = $manager->getTables($chado_schema_name);
    $this->assertIsArray($custom_tables, "The return value of Custom Table manager getTables is expected to be an array.");
    $this->assertEmpty($custom_tables, "We just created this test schema so the Custom Table manager should not be able to find any tables yet.");

    // Test manager create. This just creates the object.
    $table_name = $this->randomString(25);
    $custom_table_obj = $manager->create($table_name, $chado_schema_name);
    $this->assertIsObject($custom_table_obj, "Unable to create a custom table object using the service manager.");
    $this->assertInstanceOf(
      \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable::class,
      $custom_table_obj,
      "Ensure the object the custom table manager created is in fact a custom table object."
    );

    // @todo Test manager loadById.

    // @todo Test manager get list of chado custom tables again now that one has been added.

    // @todo Test manager find by name.

    // @todo Test manager load by name.

  }

  /**
   * Tests focusing on the ChadoCustomTable class.
   */
  public function testCoreFunctionality() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }
}
