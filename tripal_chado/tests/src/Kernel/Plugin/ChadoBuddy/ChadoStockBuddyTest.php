<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the Chado Stock Buddy.
 *
 * @group bio-stock
 * @group plugin-chado-buddy
 */
#[Group('bio-stock')]
#[Group('plugin-chado-buddy')]
#[RunTestsInSeparateProcesses]
class ChadoStockBuddyTest extends ChadoTestBuddyBase {

  /**
   * A Database query interface for querying Chado using Tripal DBX.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  public ChadoConnection $chado_connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to a test Chado.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests the various methods of the ChadoStockBuddy.
   */
  public function testStockMethods() {
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $organism_instance = $type->createInstance('chado_organism_buddy', []);
    $stock_instance = $type->createInstance('chado_stock_buddy', []);

    // TEST: If there is no stock record, then it should return an empty
    // array when we try to get it.
    $stock_buddy_records = $stock_instance->getStock([
      'stock.name' => 'notastockname',
      'organism.species' => 'notanorganismspecies',
      'cvterm.name' => 'notacvtermname',
    ]);
    $this->assertIsArray($stock_buddy_records, 'We did not retrieve an array for a Stock record that does not exist');
    $this->assertEquals(0, count($stock_buddy_records), 'We did not retrieve an empty array for a Stock record that does not exist');

    // TEST: Insert a stock record with name, uniquename, type and organism.
    // First create an organism us the ChadoOrganismBuddy
    // TEST: Insert an organism record with genus, species, and common name.
    $simple_organism_values = [
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.common_name' => 'Tripal',
    ];
    $organism_buddy_records = $organism_instance->insertOrganism($simple_organism_values);
    // $this->assertEquals(1, count($organism_buddy_records), 'We did not successfully insert one organism using the ChadoOrganismBuddy.');
    $simple_stock_values = [
      'stock.name' => 'Stock1',
      'stock.uniquename' => 'stock1',
      // Cvterm ID for 'accession'.
      'stock.type_id' => '3',
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
    ];
    $test_records = [];
    $test_records['set'] = $stock_instance->insertStock($simple_stock_values);
    $test_records['get'] = $stock_instance->getStock($simple_stock_values);
    $values = $this->multiAssert(
      'insertStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock "Stock1"',
      36
    );
    $stock_id = $values['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($stock_id), 'We did not retrieve an integer stock_id for the new stock "Stock1"');
  }

}
