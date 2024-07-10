<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Database\Database;
use Drupal\Core\Test\FunctionalTestSetupTrait;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;
use Drupal\tripal_chado\Services\ChadoCustomTableManager;

/**
 * Tests the functions in the ChadoCustomTableManager services class.
 * 
 * @group Tripal
 * @group Tripal Chado
 */
class ChadoCustomTableManagerTest extends ChadoTestBrowserBase {

/**
 *  Tests that we can create, list, and get custom table objects
 * 
 * @group chado
 */
  public function testCustomTableManager() {
    // Create and then get the existing test chado schema name.
    $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);
    $chado = \Drupal::service('tripal_chado.database');
    $default_chado_schema = $chado->schema()->getDefault();

    // Get an instance of the manager.
    $ct_service = \Drupal::service('tripal_chado.custom_tables');

    // Test that the manager was created successfully.
    $this->assertInstanceOf(ChadoCustomTableManager::class, $ct_service, 'The Chado Custom Table Manager could not be created.');

    // Test creating a table. The create() function returns a ChadoCustomTable object
    $custom_table = $ct_service->create('test_custom_table', $default_chado_schema);

    $this->assertInstanceOf(ChadoCustomTable::class, $custom_table, 'The test_custom_table could not be created');

    // We created a table, let's load it by name.
    $test_table_by_name = $ct_service->loadByName('test_custom_table', $default_chado_schema);
    
    $this->assertInstanceOf(ChadoCustomTable::class, $test_table_by_name, 'The test_custom_table could not be created');

    // Test loadById() with the id from the $test_table we just loaded. Make sure the returned tables are the same table object.
    $test_table_by_id = $ct_service->loadById($test_table_by_name->getTableId());

    $this->assertEquals($test_table_by_name, $test_table_by_id, 'Could not load the table by ID');

    // Test the findByName() function. The returned int should match the ID found above by the table's getTableID() function.
    $test_table_find = $ct_service->findByName('test_custom_table', $default_chado_schema);
    $this->assertEquals($test_table_find, $test_table_by_name->getTableId(), 'Could not find the correct table');

    // Test the getTables() function.
    $expected_tables = [
      '1' => 'test_custom_table',
    ];
    $tables = $ct_service->getTables($default_chado_schema);
    $this->assertEquals($tables, $expected_tables, 'Could not get a list of tables from the ' . $default_chado_schema . ' schema.');
    // Add another table
    $custom_table = $ct_service->create('test_custom_table2', $default_chado_schema);
    $expected_tables = [
      '1' => 'test_custom_table',
      '2' => 'test_custom_table2',
    ];
    $tables = $ct_service->getTables($default_chado_schema);
    $this->assertEquals($tables, $expected_tables, 'Could not get a list of tables from the ' . $default_chado_schema . ' schema.');

  }
}