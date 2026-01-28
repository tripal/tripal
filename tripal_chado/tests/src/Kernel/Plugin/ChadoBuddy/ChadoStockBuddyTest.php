<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\DataProvider;
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
    $instance = $type->createInstance('chado_stock_buddy', []);

    // TEST: If there is no stock record, then it should return an empty
    // array when we try to get it.
    $chado_buddy_records = $instance->getStock([
      'stock.name' => 'notastockname',
      'organism.species' => 'notanorganismspecies',
      'cvterm.name' => 'notacvtermname',
    ]);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a Stock record that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a Stock record that does not exist');
  }

}
