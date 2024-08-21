<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the base functionality for Chado Buddies.
 *
 * Specifically, it tests the plugin manager and the base class.
 *
 * @group ChadoBuddy
 */
class ChadoBuddyBaseTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected ChadoConnection $connection;

  /**
   * Annotations associated with the mock_plugin.
   * @var Array
   */
  protected $cvtermbuddy_plugin_definition = [
    'id' => "chado_cvterm_buddy",
    'label' => "Chado Controlled Vocabulary Term Buddy",
    'description' => "Provides helper methods for managing chado cvs and cvterms.",
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installConfig('system');

    $connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests focusing on the ChadoBuddy Plugin Manager.
   */
  public function testChadoBuddyManager() {

    // Test the ChadoBuddy Plugin Manager.
    // --Ensure we can instantiate the plugin manager.
    $type = \Drupal::service('tripal_chado.chado_buddy');
    // Note: If the plugin manager is not found you will get a ServiceNotFoundException.
    $this->assertIsObject(
      $type,
      'A chado buddy plugin service object was not returned.'
    );

    // --Use the plugin manager to get a list of available implementations.
    $plugin_definitions = $type->getDefinitions();
    $this->assertIsArray(
      $plugin_definitions,
      'Implementations of the chado buddy plugin should be returned in an array.'
    );

    // --Use the plugin manager to create an instance.
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject(
      $instance,
      "We did not have an object created when trying to create an ChadoBuddy instance.");
    $this->assertIsObject(
      $instance->connection,
      "The chado connection should have been set by the plugin manager but the value is NOT AN OBJECT."
    );
    $this->assertInstanceOf(
      ChadoConnection::class, $instance->connection,
      "The chado connection should have been set by the plugin manager but the value is NOT A CHADOCONNECTION OBJECT."
    );
  }

  /**
   * Tests focused on basic getter/setters.
   *
   * Specifically, label(), description(), makeAlias(), unmakeAlias(),
   * removeTablePrefix().
   */
  public function testChadoBuddyGetterSetters() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $this->assertIsObject($type, 'A chado buddy plugin service object was not returned.');
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject(
      $instance,
      "We did not have an object created when trying to create an ChadoBuddy instance."
    );

      // Make protected methods accessible.
    $reflection = new \ReflectionClass($instance);
    $makeAlias = $reflection->getMethod('makeAlias');
    $makeAlias->setAccessible(true);
    $unmakeAlias = $reflection->getMethod('unmakeAlias');
    $unmakeAlias->setAccessible(true);
    $removeTablePrefix = $reflection->getMethod('removeTablePrefix');
    $removeTablePrefix->setAccessible(true);

    // Label
    $label = $instance->label();
    $this->assertIsString($label, "The label is expected to be a string.");
    $this->assertEquals($label, $this->cvtermbuddy_plugin_definition['label'],
      "The label returned did not match what we expected for the Chado Cvterm Buddy.");

    // Description
    $description = $instance->description();
    $this->assertIsString($description, "The description is expected to be a string.");
    $this->assertEquals($description, $this->cvtermbuddy_plugin_definition['description'],
      "The description returned did not match what we expected for the Chado Cvterm Buddy.");

    // Column Alias (protected)
    // Test a typical use case.
    $expected_alias = 'fred__sarah';
    $retrieved_alias = $makeAlias->invoke($instance, 'fred.sarah');
    $this->assertEquals($expected_alias, $retrieved_alias, "We did not retrieve the alias we expected.");
    $expected_column = 'sally.jacob';
    $retrieved_column = $unmakeAlias->invoke($instance, 'sally__jacob');
    $this->assertEquals($expected_column, $retrieved_column, "We did not retrieve the column we expected when unmaking the alias.");
    $start_column = 'me.you';
    $retrieved_alias = $makeAlias->invoke($instance, $start_column);
    $retrieved_column = $unmakeAlias->invoke($instance, $retrieved_alias);
    $this->assertEquals($start_column, $retrieved_column, "We were unable to recover the same column when passed to makeAlias() and then unmakeAlias().");

    // Test when a column with no dot is passed in. Expect no change.
    $start_column = 'abc3_def6_ghi9';
    $expected_alias = $start_column;
    $retrieved_alias = $makeAlias->invoke($instance, $start_column);
    $this->assertEquals($expected_alias, $retrieved_alias, "We did not retrieve the alias we expected.");
    $retrieved_column = $unmakeAlias->invoke($instance, $retrieved_alias);
    $this->assertEquals($start_column, $retrieved_column, "We were unable to recover the same column when passed to makeAlias() and then unmakeAlias().");

    // Test when a column with multiple dots, or alias with multiple double
    // underscores is passed in. Expect only the first instance to be changed.
    $start_column = 'Abc3.dEf6.ghI9';
    $expected_alias = 'Abc3__dEf6.ghI9';
    $retrieved_alias = $makeAlias->invoke($instance, $start_column);
    $this->assertEquals($expected_alias, $retrieved_alias, "We did not retrieve the alias we expected.");
    $start_alias = 'A_bc3__dEf6__ghI_9';
    $expected_column = 'A_bc3.dEf6__ghI_9';
    $retrieved_column = $unmakeAlias->invoke($instance, $start_alias);
    $this->assertEquals($expected_column, $retrieved_column, "We were unable to recover the expected column from its alias.");

    // Remove Table Prefix (protected)
    // Test a typical use case.
    $referenced_values = ['cvterm.name' => 'sarah', 'cvterm.dbxref_id' => 3, 'cvterm.cv_id' => 9];
    $expected_values = ['name' => 'sarah', 'dbxref_id' => 3, 'cv_id' => 9];
    $dereferenced_values = $removeTablePrefix->invoke($instance, $referenced_values);
    $this->assertEquals($expected_values, $dereferenced_values, "We did not get the dereferenced values we expected when calling removeTablePrefix on " . print_r($referenced_values, TRUE));

    // Test when more then one table of values is passed in and an ambiguous column
    // name would result (e.g. cv.name and cvterm.name). Expect exception.
    $referenced_values = ['cv.name' => 'aldous', 'cvterm.name' => 'huxley'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $dereferenced_values = $removeTablePrefix->invoke($instance, $referenced_values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'Did not catch exception that should have been thrown for removeTablePrefix()');
    $this->assertStringContainsString('Ambiguous columns passed to removeTablePrefix(), this function can only handle columns in a single table.', $exception_message, "We didn't get the exception message we expected for removeTablePrefix()");

    // Test when a key does not have a dot or when it has multiple dots. Only trim to first dot.
    $referenced_values = ['name_no_dot' => 'newton', 'cvterm.name.fictional.indeed' => 'dumbeldore'];
    $expected_values = ['name_no_dot' => 'newton', 'name.fictional.indeed' => 'dumbeldore'];
    $dereferenced_values = $removeTablePrefix->invoke($instance, $referenced_values);
    $this->assertEquals($expected_values, $dereferenced_values, 'Unexpected dereferenced values from removeTablePrefix()');
  }

  /**
   * Tests methods dealing with table columns: getTableColumns(),
   * addTableToCache(), makeUpsertConditions().
   */
  public function testChadoBuddyColumnMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $this->assertIsObject($type, 'A chado buddy plugin service object was not returned.');
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject(
      $instance,
      "We did not have an object created when trying to create an ChadoBuddy instance."
    );

    // Make protected methods accessible.
    $reflection = new \ReflectionClass($instance);
    $getTableColumns = $reflection->getMethod('getTableColumns');
    $getTableColumns->setAccessible(true);
    $addTableToCache = $reflection->getMethod('addTableToCache');
    $addTableToCache->setAccessible(true);
    $getTableCache = $reflection->getMethod('getTableCache');
    $getTableCache->setAccessible(true);
    $makeUpsertConditions = $reflection->getMethod('makeUpsertConditions');
    $makeUpsertConditions->setAccessible(true);

    // CASE: getTableColumns() with no tables.
    $returned_columns = $getTableColumns->invoke($instance, []);
    $this->assertCount(0, $returned_columns, "We should not have had any columns returned when calling getTableColumns() with an empty tables parameter.");

    // CASE: getTableColumns() with a single table, no filter, nothing cached.
    $expected_columns = [
      'db.db_id',
      'db.name',
      'db.description',
      'db.urlprefix',
      'db.url'
    ];
    $returned_columns = $getTableColumns->invoke($instance, ['db']);
    $retrieved_cache = $getTableCache->invoke($instance);
    $this->assertEqualsCanonicalizing($expected_columns, $returned_columns, 'We did not get the expected columns when calling getTableColumns() with "db" as the only table parameter.');
    $this->assertCount(1, $retrieved_cache, "There should only be a single table (db) in the cache.");
    $this->assertArrayHasKey('db', $retrieved_cache, "The db table should be in the cache.");

    // CASE: getTableColumns() with two tables, no filter, one table cached + the other not.
    $expected_columns = [
      'db.db_id',
      'db.name',
      'db.description',
      'db.urlprefix',
      'db.url',
      'dbxref.accession',
      'dbxref.dbxref_id',
      'dbxref.db_id',
      'dbxref.version',
      'dbxref.description'
    ];
    $returned_columns = $getTableColumns->invoke($instance, ['db', 'dbxref']);
    $retrieved_cache = $getTableCache->invoke($instance);
    $this->assertEqualsCanonicalizing($expected_columns, $returned_columns, 'We did not get the expected columns when calling getTableColumns() with "db" and "dbxref" as table parameters.');
    $this->assertCount(2, $retrieved_cache, "There should now contain two tables (db + dbxref) in the cache.");
    $this->assertArrayHasKey('db', $retrieved_cache, "The db table should be in the cache.");
    $this->assertArrayHasKey('dbxref', $retrieved_cache, "The db table should be in the cache.");

    // CASE: getTableColumns() with two tables, no filter, both tables cached.
    $returned_columns = $getTableColumns->invoke($instance, ['db', 'dbxref']);
    $retrieved_cache = $getTableCache->invoke($instance);
    $this->assertEqualsCanonicalizing($expected_columns, $returned_columns, 'We did not get the expected columns when calling getTableColumns() with "db" and "dbxref" as table parameters and both are cached.');
    $this->assertCount(2, $retrieved_cache, "There should now contain two tables (db + dbxref) in the cache.");
    $this->assertArrayHasKey('db', $retrieved_cache, "The db table should be in the cache.");
    $this->assertArrayHasKey('dbxref', $retrieved_cache, "The db table should be in the cache.");

    // CASE: getTableColumns() with required filter.
    $expected_columns = [
      'analysis.program',
      'analysis.programversion',
    ];
    $returned_columns = $getTableColumns->invoke($instance, ['analysis'], 'required');
    $this->assertEqualsCanonicalizing($expected_columns, $returned_columns, 'We did not get the expected required columns when calling getTableColumns(["analysis"], "required").');

    // CASE: getTableColumns() with unique filter.
    $expected_columns = [
      'analysis.program',
      'analysis.programversion',
      'analysis.sourcename',
    ];
    $returned_columns = $getTableColumns->invoke($instance, ['analysis'], 'unique');
    $this->assertEqualsCanonicalizing($expected_columns, $returned_columns, 'We did not get the expected unique columns when calling getTableColumns(["analysis"], "unique").');

    // CASE: addTableToCache() with a non-existent chado table.
    $expected_cache = $getTableCache->invoke($instance);
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $arguements = ['sarah', &$expected_cache];
      $addTableToCache->invokeArgs($instance, $arguements);
    }
    catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We expected an exception when calling addTableToCache('sarah') when sarah doesn't exist in chado but we didn't get one.");
    $this->assertStringContainsString('invalid table "sarah" passed to getTableColumns()', $exception_message, "We didn't get the exception message we expected when calling addTableToCache('sarah').");

    // CASE: Basic use of makeUpsertConditions().
    $values = [
      'analysis.name' => 'A',
      'analysis.program' => 'B',
      'analysis.programversion' => 'C',
      'analysis.sourcename' => 'D',
      'analysis.sourceversion' => 'E',
    ];
    $columns_to_keep = ['analysis.program', 'analysis.programversion', 'analysis.sourcename'];
    $expected_conditions = [
      'analysis.program' => 'B',
      'analysis.programversion' => 'C',
      'analysis.sourcename' => 'D',
    ];
    $returned_conditions = $makeUpsertConditions->invoke($instance, $values, $columns_to_keep);
    $this->assertEqualsCanonicalizing($expected_conditions, $returned_conditions, "We did not get the conditions we expected for the analsis table when calling makeUpsertConditions()");

    // @todo pass in columns that are not in the values.
    // @todo pass in columns that are not part of the unique constraint.

  }

  /**
   * Tests methods dealing with input: validateInput(), subsetInput(),
   * dereferenceBuddyRecord(), validateOutput().
   */
  public function testChadoBuddyInputOutputMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $this->assertIsObject($type, 'A chado buddy plugin service object was not returned.');
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject(
      $instance,
      "We did not have an object created when trying to create an ChadoBuddy instance."
    );

    // Make protected methods accessible.
    $reflection = new \ReflectionClass($instance);
    $validateInput = $reflection->getMethod('validateInput');
    $validateInput->setAccessible(true);
    $subsetInput = $reflection->getMethod('subsetInput');
    $subsetInput->setAccessible(true);
    $dereferenceBuddyRecord = $reflection->getMethod('dereferenceBuddyRecord');
    $dereferenceBuddyRecord->setAccessible(true);

    // CASE: valid values passed to validateInput().
    $user_values = [
      'analysis.name' => 'A',
      'analysis.program' => 'B',
      'analysis.programversion' => 'C',
      'analysis.sourcename' => 'D',
      'analysis.sourceversion' => 'E',
    ];
    $valid_columns = ['analysis.name', 'analysis.program', 'analysis.programversion', 'analysis.sourcename', 'analysis.sourceversion'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateInput->invoke($instance, $user_values, $valid_columns);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling validateInput() with valid input.");

    // CASE: calling validateInput with no user values
    $valid_columns = ['analysis.name', 'analysis.program', 'analysis.programversion', 'analysis.sourcename', 'analysis.sourceversion'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateInput->invoke($instance, [], $valid_columns);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling validateInput() with no user input.");
    $this->assertStringContainsString('no values were specified', $exception_message, "We did not get the exception message we expected when calling validateInput() with no user values.");

    // CASE: calling validateInput where there is an invalid column in the user input.
    $user_values = [
      'analysis.name' => 'A',
      'analysis.program' => 'B',
      'analysis.programversion' => 'C',
      'analysis.sourcename' => 'D',
      'analysis.sourceversion' => 'E',
      'me.you' => 'BEEEEEP',
    ];
    $valid_columns = ['analysis.name', 'analysis.program', 'analysis.programversion', 'analysis.sourcename', 'analysis.sourceversion'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateInput->invoke($instance, $user_values, $valid_columns);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling validateInput() with an invalid column in the user input.");
    $this->assertStringContainsString('key "me.you" is not valid', $exception_message, "We did not get the exception message we expected when calling validateInput() with an invalid column in the user input.");

    // @todo test when valid values includes a column that is not in the user input.

    // @todo call subsetInput with the expected input
    // @todo call subsetInput when the valid tables table is not in the user input
    // @todo call subsetInput when all tables in the user input are in the valid tables.
    // @todo call subsetInput when all tables are filtered out.
    // @todo call subsetInput when user input is empty.

    // @todo call dereferenceBuddyRecord with expected input
    // @todo call dereferenceBuddyRecord with a values array not containing a buddy record
    // @todo call dereferenceBuddyRecord with a values that are already in the values array with different values.
    // @todo call dereferenceBuddyRecord with a values['buddy_record'] => array (i.e. value is not actually a buddy record).

    // @todo call validateOutput with valid output
    // @todo call validateOutput with a string as output
    // @todo call validateOutput with an empty array as output
    // @todo call validateOutput with an array containing multiple records.
  }

   /**
    * Tests methods dealing with the query object: addConditions().
    */
  public function testChadoBuddyQueryMethods() {

    // @todo pass in the expected parameters.
    // @todo pass in a string as though its a query object ;-p
    // @todo pass in an empty array of conditions.
    // @todo pass in a single key string to be case insensitive
    // @todo pass in an array of keys to be case insensitive.

  }
}
