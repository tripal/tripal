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

    // TEST: Insert a stock record with name, uniquename, type_id, organism_id.
    // First create an organism using the ChadoOrganismBuddy.
    $simple_organism_values = [
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.common_name' => 'Tripal',
    ];
    $organism_buddy_record = $organism_instance->insertOrganism($simple_organism_values);
    $organism_id = $organism_buddy_record->getValue('organism.organism_id');
    $this->assertEquals(1, $organism_id, 'The organism id does not match what was expected to be returned using ChadoOrganismBuddy->insertOrganism().');
    $stock1_values = [
      'stock.name' => 'Stock1',
      'stock.uniquename' => 'stock1',
      // Cvterm ID for 'accession'.
      'stock.type_id' => '3',
      'stock.organism_id' => $organism_id,
    ];
    $test_records = [];
    $test_records['set'] = $stock_instance->insertStock($stock1_values);
    $test_records['get'] = $stock_instance->getStock($stock1_values);
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

    // TEST: Insert a stock record with name, uniquename, type_id, and the
    // previously inserted organism values.
    $stock2_values = [
      'stock.name' => 'Stock2',
      'stock.uniquename' => 'stock2',
      // Cvterm ID for 'accession'.
      'stock.type_id' => '3',
    ];
    $stock_organism_values = $stock2_values + $simple_organism_values;
    $test_records = [];
    $test_records['set'] = $stock_instance->insertStock($stock_organism_values);
    $test_records['get'] = $stock_instance->getStock($stock_organism_values);
    $values = $this->multiAssert(
      'insertStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock "Stock2"',
      36
    );
    $stock_id = $values['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($stock_id), 'We did not retrieve an integer stock_id for the new stock "Stock2"');

  }

  /**
   * Data Provider: Provide scenarios to test the insertStock() method.
   *
   * @return array
   *   An array of test scenarios, each containing:
   *   - values: An array of stock values to insert.
   *   - options: An array of options to pass to insertStock().
   *   - num_expected_records: The expected number of stock records to be
   *     created.
   */
  public static function provideInsertStockScenarios() {
    $scenarios = [];

    // #0: Insert a stock with name, uniquename, type_id, organism_id, dbxref_id
    // and validate foreign keys.
    $scenarios[] = [
      [
        'stock.name' => 'Stock1',
        'stock.uniquename' => 'stock1',
        // Cvterm ID for 'accession'.
        'stock.type_id' => 3,
        'stock.organism_id' => 1,
        'stock.dbxref_id' => 3,
      ],
      [],
      1,
    ];

    // #1: Provide a valid organism genus + species and validate foreign keys.
    $scenarios[] = [
      [
        'stock.name' => 'Stock2',
        'stock.uniquename' => 'stock2',
        // Cvterm ID for 'accession'.
        'stock.type_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [],
      1,
    ];

    // #2: Provide a valid stock type cvterm and cv, and validate foreign keys.
    $scenarios[] = [
      [
        'stock.name' => 'Stock3',
        'stock.uniquename' => 'stock3',
        // Cvterm ID for 'accession'.
        'cvterm.name' => 'accession',
        'cv.name' => 'germplasm_ontology',
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [],
      1,
    ];

    // #3: Provide a valid dbxref and db, and validate foreign keys.
    $scenarios[] = [
      [
        'stock.name' => 'Stock4',
        'stock.uniquename' => 'stock4',
        'stock.type_id' => 3,
        'db.name' => 'CO_010',
        'dbxref.accession' => '0000044',
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [],
      1,
    ];

    // #4: Provide a valid dbxref.dbxref_id and skip validating foreign keys
    // for dbxref_id only.
    $scenarios[] = [
      [
        'stock.name' => 'Stock5',
        'stock.uniquename' => 'stock5',
        'stock.type_id' => 3,
        'dbxref.dbxref_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [
        'validate_foreign_keys' => [
          'dbxref_id' => FALSE,
        ],
      ],
      1,
    ];

    // #5: Provide a valid organism_id and skip validating foreign keys for
    // organism_id only.
    $scenarios[] = [
      [
        'stock.name' => 'Stock6',
        'stock.uniquename' => 'stock6',
        'stock.type_id' => 3,
        'organism.organism_id' => 1,
      ],
      [
        'validate_foreign_keys' => [
          'organism_id' => FALSE,
        ],
      ],
      1,
    ];

    // #6: Provide a valid cvterm.cvterm_id for stock type and skip validating
    // foreign keys for cvterm_id only.
    $scenarios[] = [
      [
        'stock.name' => 'Stock7',
        'stock.uniquename' => 'stock7',
        'cvterm.cvterm_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [
        'validate_foreign_keys' => [
          'cvterm_id' => FALSE,
        ],
      ],
      1,
    ];

    // #7: Provide info to create a dbxref record for the stock.
    $scenarios[] = [
      [
        'stock.name' => 'Stock8',
        'stock.uniquename' => 'stock8',
        'stock.type_id' => 3,
        'dbxref.accession' => 'newAccession',
        'db.name' => 'local',
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [
        'create_dbxref' => TRUE,
      ],
      1,
    ];

    return $scenarios;

  }

  /**
  * Test method for insertStock().
  *
  * @param array $values
  *   An array of stock values to insert.
  * @param array $options
  *   An array of options to pass to insertStock().
  * @param int $num_expected_records
  *   The expected number of stock records to be created.
  *
  * @dataProvider provideInsertStockScenarios
  */
  #[DataProvider('provideInsertStockScenarios')]
  public function testInsertStock(array $values, array $options, int $num_expected_records) {

    // Insert our organisms needed by our test scenarios.
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $organism_instance = $type->createInstance('chado_organism_buddy', []);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.abbreviation' => 'Trp',
    ]);

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $stock_instance = $type->createInstance('chado_stock_buddy', []);

    // Insert the stock record.
    $test_records['set'] = $stock_instance->insertStock($values, $options);

    // Now try retrieving the stock records we just inserted.
    $test_records['get'] = $stock_instance->getStock($values);

    // Verify we retrieved the expected number of records.
    $this->assertCount(
      $num_expected_records,
      $test_records['get'],
      "Did not retrieve the expected number of stock records after inserting.",
    );
    // Verify the inserted and retrieved records match.
    $results = $this->multiAssert(
      'insertStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock inserted via insertStock() method',
      36
    );
    $stock_id = $results['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($stock_id), 'We did not retrieve a numeric stock_id for the new stock inserted via insertStock() method');
  }

  /**
   * Data Provider: Provide scenarios to test the updateStock() method.
   *
   * @return array
   *   An array of test scenarios, each containing:
   *   - values: An array of stock values to update.
   *   - conditions: An array of conditions to find the stock record to update.
   *   - options: An array of options to pass to updateStock().
   *   - num_expected_records: The expected number of stock records to be
   *     created.
   */
  public static function provideUpdateStockScenarios() {
    $scenarios = [];

    // #0: Update an existing stock's name.
    $scenarios[] = [
      [
        'stock.name' => 'Better Stock Name',
      ],
      [
        'stock.uniquename' => 'stock1',
        // Cvterm ID for 'accession'.
        'stock.type_id' => 3,
        'stock.organism_id' => 1,
      ],
      [],
      1,
    ];

    // #1: Update an existing stock to a different organism and validate foreign
    // keys.
    $scenarios[] = [
      [
        'organism.genus' => 'Postgres',
        'organism.species' => 'chadoii',
      ],
      [
        'stock.name' => 'Stock1',
        'stock.uniquename' => 'stock1',
        // Cvterm ID for 'accession'.
        'stock.type_id' => 3,
      ],
      [],
      1,
    ];

    // #2: Update an existing stock to a different organism and skip validating
    // foreign keys.
    $scenarios[] = [
      [
        'organism.organism_id' => 2,
      ],
      [
        'stock.name' => 'Stock1',
        'stock.uniquename' => 'stock1',
        // Cvterm ID for 'accession'.
        'stock.type_id' => 3,
      ],
      [
        'validate_foreign_keys' => [
          'organism_id' => FALSE,
        ],
      ],
      1,
    ];

    // #3: Update an existing stock with a valid dbxref and db.
    $scenarios[] = [
      [
        'db.name' => 'CO_010',
        'dbxref.accession' => '0000044',
      ],
      [
        'stock.name' => 'Stock1',
        'stock.uniquename' => 'stock1',
        'stock.type_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [],
      1,
    ];

    // #4: Update an existing stock with a valid dbxref.dbxref_id and skip
    // validating foreign keys for dbxref_id only.
    $scenarios[] = [
      [
        'dbxref.dbxref_id' => 3,
      ],
      [
        'stock.name' => 'Stock1',
        'stock.uniquename' => 'stock1',
        'stock.type_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [
        'validate_foreign_keys' => [
          'dbxref_id' => FALSE,
        ],
      ],
      1,
    ];

    // #5: Provide info to create a dbxref record for an existing stock.
    $scenarios[] = [
      [
        'dbxref.accession' => 'newAccession',
        'db.name' => 'local',
      ],
      [
        'stock.name' => 'Stock1',
        'stock.uniquename' => 'stock1',
        'stock.type_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
      ],
      [
        'create_dbxref' => TRUE,
      ],
      1,
    ];

    return $scenarios;

  }

  /**
  * Test method for updateStock().
  *
  * @param array $values
  *   An array of stock values to update.
  * @param array $conditions
  *   An array of conditions to find the stock record to update.
  * @param array $options
  *   An array of options to pass to updateStock().
  * @param int $num_expected_records
  *   The expected number of stock records to be created.
  *
  * @dataProvider provideUpdateStockScenarios
  */
  #[DataProvider('provideUpdateStockScenarios')]
  public function testUpdateStock(array $values, array $conditions, array $options, int $num_expected_records) {

    // Insert our organisms needed by our test scenarios.
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $organism_instance = $type->createInstance('chado_organism_buddy', []);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.abbreviation' => 'Trp',
    ]);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Postgres',
      'organism.species' => 'chadoii',
      'organism.abbreviation' => 'Pst',
    ]);

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $stock_instance = $type->createInstance('chado_stock_buddy', []);

    // Insert a test stock record.
    $stock_instance->insertStock([
      'stock.name' => 'Stock1',
      'stock.uniquename' => 'stock1',
      // Cvterm ID for 'accession'.
      'stock.type_id' => 3,
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
    ]);

    // Update the stock record.
    $test_records['set'] = $stock_instance->updateStock($values, $conditions, $options);

    // Now try retrieving the stock record we just updated.
    $test_records['get'] = $stock_instance->getStock(['stock.stock_id' => 1]);

    // Verify we retrieved the expected number of records.
    $this->assertCount(
      $num_expected_records,
      $test_records['get'],
      "Did not retrieve the expected number of stock records after updating.",
    );
    // Verify the updated and retrieved records match.
    $results = $this->multiAssert(
      'updateStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock updated via updateStock() method',
      36
    );
    $stock_id = $results['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($stock_id), 'We did not retrieve a numeric stock_id for the new stock updated via updateStock() method');
  }

  /**
   * Data Provider: Trigger exceptions in ChadoStockBuddy methods.
   *
   * @return array
   *   An array of test scenarios, each containing:
   *   - method_name: The ChadoStockBuddy method to call.
   *   - method_input: The input array to give to the desired method. Depending
   *     on the method, this can include an array of values for insert, array of
   *     conditions for lookup, or both. It can also include an array of
   *     options, though not required.
   *   - expected_exception_message: The expected exception message.
   */
  public static function provideStockBuddyExceptionScenarios() {
    $scenarios = [];

    /*
     * NOTE: Does not currently trigger the following cases:
     * - ChadoBuddy getStock database error
     */

    // #0: insertStock() when a record with these values already exists.
    $scenarios[] = [
      'insertStock',
      [
        [
          'stock.name' => 'ExistingStock',
          'stock.uniquename' => 'existingstock',
          'stock.type_id' => 3,
          'dbxref.dbxref_id' => 3,
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
        ],
      ],
      "ChadoBuddy insertStock error, a stock record already exists that matches the specified values:",
    ];

    // #1: insertStock() with insufficient values.
    $scenarios[] = [
      'insertStock',
      [
        [
          'stock.name' => 'LonesomeStock',
        ],
      ],
      "ChadoBuddy insertStock database error ",
    ];

    // #2: insertStock() where stock.dbxref_id and dbxref.dbxref_id don't match.
    $scenarios[] = [
      'insertStock',
      [
        [
          'stock.name' => 'StockWithMismatchDbxref',
          'stock.uniquename' => 'stockwithmismatchdbxref',
          'stock.type_id' => 3,
          'stock.dbxref_id' => 1,
          'dbxref.dbxref_id' => 2,
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
        ],
      ],
      "ChadoBuddy validateStockDbxref error, stock.dbxref_id and dbxref.dbxref_id values were both provided but do not match:",
    ];

    // #3: insertStock() with dbxref values that match more than one dbxref_id.
    $scenarios[] = [
      'insertStock',
      [
        [
          'stock.name' => 'StockWithTwoDbxref',
          'stock.uniquename' => 'stockwithtwodbxref',
          'stock.type_id' => 3,
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
          'dbxref.db_id' => 4,
        ],
      ],
      "ChadoBuddy validateStockDbxref error, more than one record matched the values specified:",
    ];

    // #4: insertStock() where dbxref values are provided but could not find or
    // create a matching dbxref record.
    $scenarios[] = [
      'insertStock',
      [
        [
          'stock.name' => 'StockWithNoDbxref',
          'stock.uniquename' => 'stockwithnodbxref',
          'stock.type_id' => 3,
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
          'dbxref.accession' => 'noSuchAccession',
        ],
      ],
      "ChadoBuddy validateStockDbxref error, could not find or create a dbxref, but dbxref values were provided:",
    ];
    // Trigger exceptions in updateStock() and validateStockOrganism() using
    // updateStock().
    // Trigger exceptions in upsertStock() and validateStockType() using
    // upsertStock().
    return $scenarios;
  }

  /**
   * Test method to trigger exceptions in ChadoStockBuddy class.
   *
   * @param string $method_name
   *   The ChadoStockBuddy method to call.
   * @param array $method_input
   *   The input array to give to the desired method. Depending on the method,
   *   this can include an array of values for insert, array of conditions for
   *   lookup, or both. It can also include an array of options, though not
   *   required.
   * @param string $expected_exception_message
   *   The expected exception message.
   *
   * @dataProvider provideStockBuddyExceptionScenarios
   */
  #[DataProvider('provideStockBuddyExceptionScenarios')]
  public function testStockBuddyExceptions(string $method_name, array $method_input, string $expected_exception_message) {
    // Insert an organism needed by our test scenarios.
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $organism_instance = $type->createInstance('chado_organism_buddy', []);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.abbreviation' => 'Trp',
    ]);

    // Insert a stock record so that it already exists.
    $stock_instance = $type->createInstance('chado_stock_buddy', []);
    $stock_instance->insertStock([
      'stock.name' => 'ExistingStock',
      'stock.uniquename' => 'existingstock',
      'stock.type_id' => 3,
      'dbxref.dbxref_id' => 3,
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
    ]);

    // Now call the method for this scenario.
    $exception_caught = FALSE;
    try {
      $stock_instance->$method_name(...$method_input);
    }
    catch (\Exception $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "Did not catch an expected exception when calling $method_name()");
    $this->assertStringContainsString(
      $expected_exception_message,
      $exception_message,
      "Did not receive expected exception message when calling $method_name()",
    );
  }

}
