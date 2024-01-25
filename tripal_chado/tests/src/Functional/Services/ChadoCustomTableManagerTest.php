<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Database\Database;
use Drupal\Core\Test\FunctionalTestSetupTrait;

/**
 * Tests the functions in the ChadoCustomTableManager services class.
 * 
 * @group Tripal
 * @group Tripal Chado
 */
class ChadoCustomTableManagerTest {

/**
 *  Tests that we can create, list, and get custom table objects
 * 
 * @group chado
 */
public function testCustomTableManager() {
  // Use the existing testchado schema
  $chado_schema = 'testchado';

  // Get an instance of the manager.
  $ct_service = \Drupal::service('tripal_chado.custom_tables');

  // Test creating a table.
  $ct_service->create('test_custom_table', $chado_schema);

  $this->assertInstanceOf(ChadoCustomTable::class, $ct_service, 'The table we just tried to create should be in the database.');
}
}