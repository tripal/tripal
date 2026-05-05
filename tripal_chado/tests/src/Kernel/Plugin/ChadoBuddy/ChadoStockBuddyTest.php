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
   *
   * These tests are designed for coverage of the following methods:
   * - getStock()
   * - upsertStock()
   * - additionally, testing methods with buddy records as values.
   * Other methods are tested more exhaustively in their own test methods below.
   * - getStock() is used frequently within the other test methods to verify the
   *   results of insert and update.
   * - upsertStock() does not need its own test method since it calls
   *   insertStock() and updateStock().
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

    // TEST: Insert a stock record with name, uniquename, type_id, and an
    // organism buddy record.
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
      'buddy_record' => $organism_buddy_record,
    ];
    $test_records = [];
    $test_records['set'] = $stock_instance->upsertStock($stock1_values);
    $test_records['get'] = $stock_instance->getStock($stock1_values);
    $values = $this->multiAssert(
      'upsertStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock "Stock1"',
      36
    );
    $stock_id = $values['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($stock_id), 'We did not retrieve an integer stock_id for stock "Stock1" which should have been inserted by upsertStock().');

    // TEST: Update an existing stock record to add a ChadoDbxrefBuddy record.
    $simple_dbxref_values = [
      'db.name' => 'local',
      'dbxref.accession' => 'newAccession',
    ];
    $dbxref_buddy_record = $type->createInstance('chado_dbxref_buddy', [])->insertDbxref($simple_dbxref_values);
    $dbxref_id = $dbxref_buddy_record->getValue('dbxref.dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the dbxref record we inserted.');
    $update_stock_dbxref_values = [
      'stock.uniquename' => 'stock1',
      'stock.type_id' => '3',
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'buddy_record' => $dbxref_buddy_record,
    ];
    $options = [
      'create_dbxref' => TRUE,
    ];
    $test_records = [];
    $test_records['set'] = $stock_instance->upsertStock($update_stock_dbxref_values, $options);
    $test_records['get'] = $stock_instance->getStock($update_stock_dbxref_values);
    $values = $this->multiAssert(
      'upsertStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock "Stock1" updated with new dbxref',
      36
    );
    $updated_stock_id = $values['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($updated_stock_id), 'We did not retrieve an integer stock_id for stock "Stock1" which should have been updated by upsertStock().');
    $this->assertEquals($stock_id, $updated_stock_id, 'The stock_id for stock "Stock1" changed when we updated the stock record with a dbxref buddy record using upsertStock().');

    $retrieved_dbxref_id = $values['get']['stock.dbxref_id'];
    $this->assertEquals($dbxref_id, $retrieved_dbxref_id, 'The dbxref_id associated with the stock record did not match the dbxref_id of the dbxref buddy record we updated the stock with.');

    // TEST: Update our stock's type using a ChadoCvtermBuddy record. Ensure
    // that our stock.dbxref_id value is not changed by this update.
    $simple_cvterm_values = [
      'cvterm.name' => 'generated germplasm',
      'cv.name' => 'germplasm_ontology',
      'db.name' => 'CO_010',
      'dbxref.accession' => '0000255',
    ];
    $cvterm_buddy_record = $type->createInstance('chado_cvterm_buddy', [])->getCvterm($simple_cvterm_values);
    $this->assertEquals(1, count($cvterm_buddy_record), 'Did not retrieve exactly one cvterm buddy record for the cvterm values we provided to update the stock type.');
    $cvterm_id = $cvterm_buddy_record[0]->getValue('cvterm.cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the cvterm record we inserted.');

    $test_records = [];
    $test_records['set'] = $stock_instance->updateStock(
      ['buddy_record' => $cvterm_buddy_record[0]],
      $update_stock_dbxref_values,
      $options
    );
    // Unset the stock.type_id since this should now be updated.
    unset($update_stock_dbxref_values['stock.type_id']);
    $test_records['get'] = $stock_instance->getStock($update_stock_dbxref_values);
    $values = $this->multiAssert(
      'updateStock',
      $test_records,
      'stock',
      'stock.stock_id',
      'Stock "Stock1" updated with new cvterm type',
      36
    );
    $updated_stock_id = $values['get']['stock.stock_id'];
    $this->assertTrue(is_numeric($updated_stock_id), 'We did not retrieve an integer stock_id for stock "Stock1" which should have been updated by updateStock().');
    $this->assertEquals($stock_id, $updated_stock_id, 'The stock_id for stock "Stock1" changed when we updated the stock record with a cvterm buddy record using updateStock().');

    $retrieved_cvterm_id = $values['get']['stock.type_id'];
    $this->assertEquals($cvterm_id, $retrieved_cvterm_id, 'The cvterm_id associated with the stock record did not match the cvterm_id of the cvterm buddy record we updated the stock with.');

    // Double check the dbxref_id value did not change with this update either.
    $retrieved_dbxref_id = $values['get']['stock.dbxref_id'];
    $this->assertEquals($dbxref_id, $retrieved_dbxref_id, 'The dbxref_id associated with the stock record was updated when updateStock() updated the stock.type_id using a cvterm buddy record.');
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

    // #8: Insert a stock with an empty description
    $scenarios[] = [
      [
        'stock.name' => 'Stock9',
        'stock.uniquename' => 'stock9',
        'stock.type_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
        'stock.description' => '',
      ],
      [],
      1,
    ];

    // #9: Insert a stock with a null description
    $scenarios[] = [
      [
        'stock.name' => 'Stock10',
        'stock.uniquename' => 'stock10',
        'stock.type_id' => 3,
        'organism.genus' => 'Tripalus',
        'organism.species' => 'databasica',
        'stock.description' => NULL,
      ],
      [],
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

    // Verify the stock names match.
    $this->assertEquals(
      $values['stock.name'],
      $results['get']['stock.name'],
      'The stock name we retrieved did not match the stock name we inserted.',
    );
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
      [
        'validate_foreign_keys' => TRUE,
      ],
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

    // #6: Attempt to update a non-existing stock.
    $scenarios[] = [
      [
        'stock.name' => 'NonExistingStock',
      ],
      [
        'stock.uniquename' => 'nonexistingstock',
      ],
      [],
      0,
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
    $test_records['get'] = $stock_instance->getStock($values + $conditions);

    // Verify we retrieved the expected number of records.
    $this->assertCount(
      $num_expected_records,
      $test_records['get'],
      "Did not retrieve the expected number of stock records after updating.",
    );

    if ($num_expected_records > 0) {
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

      foreach ($values as $field => $expected_value) {
        $this->assertEquals(
          $expected_value,
          $results['get'][$field],
          "The stock field '$field' we retrieved did not match the stock field value we updated.",
        );
      }
    }
  }

  /**
   * Data Provider: Provides test scenarios for testAssociateStock()
   *
   * @return array
   *   An array of test scenarios, each containing:
   *   - base_table: A string of the base table for which the stock should be
   *     associated.
   *   - base_table_values: An array of values to insert into the base table.
   *     If foreign keys need to be created first, add those to the
   *     foreign_table_values parameter and the test method will add the foreign
   *     key values to this array before inserting into the base table.
   *   - foreign_table_values: If values need to be inserted into foreign tables
   *     in order to insert into the base table, this array is structured such
   *     that the keys are the table names and the values are the array of
   *     values to insert into those tables.
   *   - linking_table: A string of the linking table that is used to create the
   *     relationship between the stock record and base table.
   *   - options: An array of options to pass to associateStock().
   */
  public static function provideAssociateStockScenarios() {
    $scenarios = [];

    // #0: Associate with the project table.
    $scenarios[] = [
      'project',
       [
         'name' => 'Test Project',
       ],
       [],
      'project_stock',
      [],
    ];

    // #1: Associate with stockcollection table.
    $scenarios[] = [
      'stockcollection',
      [
        // Cvterm ID for 'germplasm'.
        'type_id' => 41,
        'uniquename' => 'Test Stock Collection',
      ],
      [],
      'stockcollection_stock',
      [],
    ];

    // #2: Associate with nd_experiment table.
    $scenarios[] = [
      'nd_experiment',
      [
        'type_id' => 1,
      ],
      [
        'nd_geolocation' => [
          'latitude' => 1.23,
          'longitude' => 4.56,
          'altitude' => 789,
        ],
      ],
      'nd_experiment_stock',
      [
        'type_id' => 1,
      ],
    ];

    // #3: Associate with the cvterm table.
    $scenarios[] = [
      'cvterm',
      [
        'name' => 'Test Cvterm',
        'cv_id' => 1,
      ],
      [
        'dbxref' => [
          'accession' => 'test_accession_for_cvterm',
          'db_id' => 1,
        ],
      ],
      'stock_cvterm',
      [
        'pub_id' => 1,
        'is_not' => TRUE,
        'rank' => 1,
      ],
    ];

    // #4: Associate with the dbxref table.
    $scenarios[] = [
      'dbxref',
      [
        'accession' => 'Test Accession',
        'db_id' => 1,
        'version' => '1.0',
      ],
      [],
      'stock_dbxref',
      [],
    ];

    // #5: Associate with the feature table.
    $scenarios[] = [
      'feature',
      [
        'uniquename' => 'Test Feature',
        // The ID of the organism created within our test.
        'organism_id' => 1,
        'type_id' => 1,
      ],
      [],
      'stock_feature',
      [
        'type_id' => 1,
      ],
    ];

    // #6: Associate with the featuremap table.
    $scenarios[] = [
      'featuremap',
      [
        'name' => 'Test Feature Map',
      ],
      [],
      'stock_featuremap',
      [
        'type_id' => 1,
      ],
    ];

    // #7: Associate with the genotype table.
    $scenarios[] = [
      'genotype',
      [
        'uniquename' => 'Test Genotype',
        'type_id' => 1,
      ],
      [],
      'stock_genotype',
      [],
    ];

    // #8: Associate with the library table.
    $scenarios[] = [
      'library',
      [
        'uniquename' => 'Test Library',
        // The ID of the organism created within our test.
        'organism_id' => 1,
        'type_id' => 1,
      ],
      [],
      'stock_library',
      [],
    ];

    // #9: Associate with the pub table.
    $scenarios[] = [
      'pub',
      [
        'uniquename' => 'Test Pub',
        'type_id' => 1,
      ],
      [],
      'stock_pub',
      [],
    ];

    return $scenarios;
  }

  /**
   * Test method for associateStock()
   *
   * @param string $base_table
   *   A string indicating the base table which the stock should be associated.
   * @param array $base_table_values
   *   An array of values to insert into the base table. If foreign keys need to
   *   be created first, add those to the foreign_table_values parameter and the
   *   test method will add the foreign key values to this array before
   *   inserting into the base table.
   * @param array $foreign_table_values
   *   If values need to be inserted into foreign tables in order to insert into
   *   the base table, this array is structured such that the keys are the table
   *   names and the values are the array of values to insert into those tables.
   * @param string $linking_table
   *   A string indicating the linking table that is used to create the
   *   relationship between the stock record and base table.
   * @param array $options
   *   An array of options to pass to associateStock().
   *
   * @dataProvider provideAssociateStockScenarios
  */
  #[DataProvider('provideAssociateStockScenarios')]
  public function testAssociateStock(string $base_table, array $base_table_values, array $foreign_table_values, string $linking_table, array $options) {

    // Insert an organism needed for our chado stock buddy record.
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $organism_instance = $type->createInstance('chado_organism_buddy', []);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.abbreviation' => 'Trp',
    ]);

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $stock_instance = $type->createInstance('chado_stock_buddy', []);

    // Insert a test stock record.
    $test_chado_stock_record = $stock_instance->insertStock([
      'stock.name' => 'Stock1',
      'stock.uniquename' => 'stock1',
      // Cvterm ID for 'accession'.
      'stock.type_id' => 3,
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
    ]);

    // If our base table requires foreign table records to be inserted first,
    // then insert those.
    foreach ($foreign_table_values as $foreign_table => $values) {
      $foreign_table_pkey = NULL;
      $foreign_table_pkey = $this->chado_connection->insert('1:' . $foreign_table)
        ->fields($values)
        ->execute();
      // Verify the foreign table record was inserted.
      $this->assertTrue(is_numeric($foreign_table_pkey), "We did not retrieve a numeric primary key when inserting into the foreign table \"$foreign_table\" for our base table \"$base_table\" in preparation for testing associateStock().");
      // Add our new foreign table primary key to our base table values.
      $base_table_values[$foreign_table . '_id'] = $foreign_table_pkey;
    }

    // Insert a record into our base table for testing.
    $base_table_pkey = $this->chado_connection->insert('1:' . $base_table)
      ->fields($base_table_values)
      ->execute();
    // Verify the base table record was inserted.
    $this->assertTrue(is_numeric($base_table_pkey), "We did not retrieve a numeric primary key when inserting into the base table \"$base_table\" in preparation for testing associateStock().");

    // Associate the stock record with the base table record for the first time.
    $expected_status = 1;
    $status = $stock_instance->associateStock($base_table, $base_table_pkey, $test_chado_stock_record, $options);
    $this->assertIsInt($status, "We did not retrieve an integer when associating a stock with the base table \"$base_table\"");
    $this->assertEquals($expected_status, $status, "We did not retrieve the expected status when associating a stock with the base table \"$base_table\"");

    // Lookup the associated record in its linking table.
    $linking_table_query = $this->chado_connection->select('1:' . $linking_table, 'lt')
      ->fields('lt', ['stock_id'])
      ->fields('lt', [$base_table . '_id'])
      ->execute();
    $results = $linking_table_query->fetchAll();
    $this->assertIsArray($results, "We should have been able to select from the \"$linking_table\" table");
    $this->assertCount(1, $results, "There should only be a single \"$linking_table\" record inserted");
    $expected_stock_id = $test_chado_stock_record->getValue('stock.stock_id');
    $retrieved_stock_id = $results[0]->stock_id;
    $this->assertEquals($expected_stock_id, $retrieved_stock_id,
      "We did not get the stock_id from \"$linking_table\" that should have been set by associateStock()");
    $retrieved_base_table_id = $results[0]->{$base_table . '_id'};
    $this->assertEquals($base_table_pkey, $retrieved_base_table_id,
      "We did not get the correct base table primary key from \"$linking_table\" that should have been set by associateStock()");

    // Repeat the same association, it should not create a new one.
    $expected_status = 2;
    $status = $stock_instance->associateStock($base_table, $base_table_pkey, $test_chado_stock_record, $options);
    $this->assertIsInt($status, "We did not retrieve an integer when associating a stock with the base table \"$base_table\"");
    $this->assertEquals($expected_status, $status, "We did not retrieve the expected status when associating a stock with the base table \"$base_table\"");
    $linking_table_query = $this->chado_connection->select('1:' . $linking_table, 'lt')
      ->fields('lt', ['stock_id'])
      ->execute();
    $results = $linking_table_query->fetchAll();
    $this->assertIsArray($results, "We should have been able to select from the \"$linking_table\" table");
    $this->assertCount(1, $results, "There should only be a single \"$linking_table\" record.");
    $expected_stock_id = $test_chado_stock_record->getValue('stock.stock_id');
    $retrieved_stock_id = $results[0]->stock_id;
    $this->assertEquals($expected_stock_id, $retrieved_stock_id,
      "We did not get the stock_id from \"$linking_table\" that was previously associated using associateStock()");

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
   *     NOTE: For testing associateStock(), the method_input array should
   *     EXCLUDE the ChadoBuddyRecord parameter. The test will use the stock
   *     record that it creates and provide it to associateStock() in the
   *     correct order. Thus, the input should include only:
   *     - base_table (string)
   *     - record_id (int)
   *     - options (array)
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

    // #5: updateStock() with conditions that match more than one stock record.
    $scenarios[] = [
      'updateStock',
      [
        [
          'stock.name' => 'UpdatedStockName',
        ],
        [
          'stock.type_id' => 3,
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
        ],
      ],
      "ChadoBuddy updateStock error, more than one stock record matches the specified conditions:",
    ];

    // #6: updateStock() with values to update an existing stock's stock_id.
    $scenarios[] = [
      'updateStock',
      [
        [
          'stock.stock_id' => 999,
        ],
        [
          'stock.uniquename' => 'existingstock',
          // Cvterm ID for 'accession'.
          'stock.type_id' => 3,
          'stock.organism_id' => 1,
        ],
      ],
      "ChadoBuddy updateStock error, no valid values were specified for tables: stock",
    ];

    // #7: updateStock() with a dbxref_id that does not exist and set options
    // to skip validation of foreign keys.
    $scenarios[] = [
      'updateStock',
      [
        [
          'stock.dbxref_id' => 999999,
        ],
        [
          'stock.uniquename' => 'existingstock',
          // Cvterm ID for 'accession'.
          'stock.type_id' => 3,
          'stock.organism_id' => 1,
        ],
        [
          'validate_foreign_keys' => [
            'dbxref_id' => FALSE,
          ],
        ],
      ],
      "ChadoBuddy updateStock database error",
    ];

    // #8: updateStock() where stock.organism_id and organism.organism_id don't
    // match.
    $scenarios[] = [
      'updateStock',
      [
        [
          'stock.organism_id' => 1,
          'organism.organism_id' => 2,
        ],
        [
          'stock.uniquename' => 'existingstock',
          'stock.type_id' => 3,
        ],
      ],
      "ChadoBuddy validateStockOrganism error, stock.organism_id and organism.organism_id values were both provided but do not match:",
    ];

    // #9: updateStock() with organism values that match more than one
    // organism_id.
    $scenarios[] = [
      'updateStock',
      [
        [
          'organism.genus' => 'Tripalus',
          'organism.abbreviation' => 'Trp',
        ],
        [
          'stock.uniquename' => 'existingstock',
          'stock.type_id' => 3,
        ],
      ],
      "ChadoBuddy validateStockOrganism error, more than one record matched the values specified:",
    ];

    // #10: updateStock() where organism values are provided but could not find
    // or create a matching organism record.
    $scenarios[] = [
      'updateStock',
      [
        [
          'organism.genus' => 'NoSuchGenus',
          'organism.species' => 'NoSuchSpecies',
        ],
        [
          'stock.uniquename' => 'existingstock',
          'stock.type_id' => 3,
        ],
      ],
      "ChadoBuddy validateStockOrganism error, could not find an organism, but organism values were provided:",
    ];

    // #11: upsertStock() with conditions that match more than one stock record.
    $scenarios[] = [
      'upsertStock',
      [
        [
          'stock.type_id' => 3,
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
        ],
      ],
      "ChadoBuddy upsertStock error, more than one stock record matches the specified values:",
    ];

    // #12: upsertStock() where stock.type_id and cvterm.cvterm_id don't match.
    $scenarios[] = [
      'upsertStock',
      [
        [
          'stock.type_id' => 3,
          'cvterm.cvterm_id' => 4,
          'stock.uniquename' => 'existingstock',
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
        ],
      ],
      "ChadoBuddy validateStockType error, stock.type_id and cvterm.cvterm_id values were both provided but do not match:",
    ];

    // #13: upsertStock() with cvterm values that match more than one cvterm_id.
    $scenarios[] = [
      'upsertStock',
      [
        [
          'stock.uniquename' => 'existingstock',
          'cv.name' => 'germplasm_ontology',
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
        ],
      ],
      "ChadoBuddy validateStockType error, more than one record matched the values specified:",
    ];

    // #14: upsertStock() where cvterm values are provided but could not find
    // or create a matching cvterm record.
    $scenarios[] = [
      'upsertStock',
      [
        [
          'stock.uniquename' => 'existingstock',
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
          'cvterm.name' => 'notacvterm',
          'cv.name' => 'germplasm_ontology',
        ],
      ],
      "ChadoBuddy validateStockType error, could not find a cvterm, but cvterm values were provided:",
    ];

    // #15: Trigger a parseValidateForeignKeysOption error by providing a non-
    // boolean value.
    $scenarios[] = [
      'upsertStock',
      [
        [
          'stock.uniquename' => 'existingstock',
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
          'stock.type_id' => 3,
        ],
        [
          'validate_foreign_keys' => [
            'cvterm_id' => 'notaboolean',
          ],
        ],
      ],
      "ChadoBuddy parseValidateForeignKeysOption error, validate_foreign_keys option for key cvterm_id must be a boolean value:",
    ];

    // #16: Trigger a parseValidateForeignKeysOption error by providing a string
    // value for validate_foreign_keys instead of an array or boolean.
    $scenarios[] = [
      'upsertStock',
      [
        [
          'stock.uniquename' => 'existingstock',
          'organism.genus' => 'Tripalus',
          'organism.species' => 'databasica',
          'stock.type_id' => 3,
        ],
        [
          'validate_foreign_keys' => 'notabooleanorarray',
        ],
      ],
      "ChadoBuddy parseValidateForeignKeysOption error, validate_foreign_keys option must be a boolean value or an array:",
    ];

    // #17: Provide an invalid base table to associateStock().
    $scenarios[] = [
      'associateStock',
      [
        'madeupbasetable',
        1000,
        [],
      ],
      "ChadoBuddy associateStock error, invalid base_table provided: madeupbasetable. Valid options are:",
    ];

    // #18: Provide an invalid record_id to associateStock().
    $scenarios[] = [
      'associateStock',
      [
        'project',
        999999,
        [],
      ],
      "ChadoBuddy associateStock database error",
    ];

    // #19: Try to associate a stock with a base table that requires a cvterm_id
    // in the linking table but do not provide a cvterm_id in the options and
    // turn off looking it up.
    $scenarios[] = [
      'associateStock',
      [
        'feature',
        1000,
        [
          'lookup_columns' => FALSE,
        ],
      ],
      "ChadoBuddy associateStock database error",
    ];

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
   *   NOTE: For testing associateStock(), the method_input array should
   *   EXCLUDE the ChadoBuddyRecord parameter. The test will use the stock
   *   record that it creates and provide it to associateStock() in the
   *   correct order. Thus, the input should include only:
   *   - base_table (string)
   *   - record_id (int)
   *   - options (array)
   * @param string $expected_exception_message
   *   The expected exception message.
   *
   * @dataProvider provideStockBuddyExceptionScenarios
   */
  #[DataProvider('provideStockBuddyExceptionScenarios')]
  public function testStockBuddyExceptions(string $method_name, array $method_input, string $expected_exception_message) {
    // Insert organisms needed by our test scenarios.
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $organism_instance = $type->createInstance('chado_organism_buddy', []);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.abbreviation' => 'Trp',
    ]);
    $organism_instance->insertOrganism([
      'organism.genus' => 'Tripalus',
      'organism.species' => 'chadoii',
      'organism.abbreviation' => 'Trp',
    ]);

    // Insert stock records so that they already exist.
    $stock_instance = $type->createInstance('chado_stock_buddy', []);
    $existing_stock = $stock_instance->insertStock([
      'stock.name' => 'ExistingStock',
      'stock.uniquename' => 'existingstock',
      'stock.type_id' => 3,
      'dbxref.dbxref_id' => 3,
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
    ]);
    $stock_instance->insertStock([
      'stock.name' => 'BonusStock',
      'stock.uniquename' => 'bonusstock',
      'stock.type_id' => 3,
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
    ]);

    // For associateStock() scenarios, we need a ChadoBuddyRecord object. Use
    // the existing stock record we just created.
    if ($method_name == 'associateStock') {
      // Move the $options array to the end of the input array to make room
      // for the ChadoBuddyRecord parameter.
      $method_input[3] = $method_input[2];
      $method_input[2] = $existing_stock;
    }

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
