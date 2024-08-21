<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;
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

    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
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
    $start_column = 'Abc3.dE_f6.ghI9';
    $expected_alias = 'Abc3__dE_f6.ghI9';
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
    $this->assertStringContainsString('Ambiguous columns passed to removeTablePrefix', $exception_message, "We didn't get the exception message we expected for removeTablePrefix()");

    // Test when a key does not have a dot or when it has multiple dots. Only trim to first dot.
    $referenced_values = ['real_name_no_dot' => 'newton', 'cvterm.name.fictional.indeed' => 'dumbledore'];
    $expected_values = ['real_name_no_dot' => 'newton', 'name.fictional.indeed' => 'dumbledore'];
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
    $this->assertEqualsCanonicalizing($expected_conditions, $returned_conditions, "We did not get the conditions we expected for the analysis table when calling makeUpsertConditions()");

    // CASE: pass in columns that are not in the values. Not an error.
    $columns_to_keep[] = 'db.name';
    $returned_conditions = $makeUpsertConditions->invoke($instance, $values, $columns_to_keep);
    $this->assertEqualsCanonicalizing($expected_conditions, $returned_conditions, "We did not get the conditions we expected for the analysis table when calling makeUpsertConditions()");

    // CASE: pass in columns that are not part of the unique constraint.
    $columns_to_keep[] = 'analysis.sourceversion';
    $expected_conditions['analysis.sourceversion'] = 'E';
    $returned_conditions = $makeUpsertConditions->invoke($instance, $values, $columns_to_keep);
    $this->assertEqualsCanonicalizing($expected_conditions, $returned_conditions, "We did not get the conditions we expected for the analysis table when calling makeUpsertConditions()");
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
    $validateOutput = $reflection->getMethod('validateOutput');
    $validateOutput->setAccessible(true);

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

    // CASE: calling validateInput where there is an invalid column
    // in the user input. Expect exception.
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

    // CASE: calling validateInput when valid values includes a column that
    // is not in the user input. Not an error.
    $user_values = ['analysis.name' => 'A2'];
    $exception_caught = FALSE;
    try {
      $validateInput->invoke($instance, $user_values, $valid_columns);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We should get not get an exception when calling validateInput() with missing columns in the user input.");

    // CASE: valid values passed to subsetInput(). i.e. filter out one
    // table from multiple. Normal use case.
    $user_values = [
      'project.name' => 'The Tripal Project',
      'project.description' => 'Cool stuff',
      'contact.name' => 'Us',
      'contact.type_id' => '1',
    ];
    $valid_tables = ['contact'];
    $exception_caught = FALSE;
    try {
      $subsetInput->invoke($instance, $user_values, $valid_tables);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling subsetInput() with valid input.");

    // CASE: calling subsetInput() when there are no valid tables
    // in the user input (i.e. all tables are filtered out). Expect exception.
    $valid_tables = ['study', 'pub'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $subsetInput->invoke($instance, $user_values, $valid_tables);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling subsetInput() with no valid tables.");
    $this->assertStringContainsString('no valid values were specified for tables', $exception_message, "We did not get the exception message we expected when calling subsetInput() with no valid tables.");

    // CASE: calling subsetInput() when all tables in the user input are in the
    // valid tables. Not an error.
    $valid_tables = ['project', 'contact'];
    $exception_caught = FALSE;
    try {
      $subsetInput->invoke($instance, $user_values, $valid_tables);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We should not get an exception when calling subsetInput() with multiple valid tables.");

    // CASE: calling subsetInput() when tables are listed but user input is
    // empty. Expect exception.
    $user_values = [];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $subsetInput->invoke($instance, $user_values, $valid_tables);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling subsetInput() with no user values.");
    $this->assertStringContainsString('no valid values were specified for tables', $exception_message, "We did not get the exception message we expected when calling subsetInput() with no valid tables.");

    // CASE: calling dereferenceBuddyRecord() with a values array not containing
    // a buddy record. Normal use case.
    $values = [
      'contact.name' => 'Dr. Charles A. Forbin',
    ];
    $expected_values = $values;
    $exception_caught = FALSE;
    try {
      $updated_values = $dereferenceBuddyRecord->invoke($instance, $values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, 'We should not get an exception when calling dereferenceBuddyRecord with valid values not including a ChadoBuddyRecord.');
    $this->assertEqualsCanonicalizing($expected_values, $updated_values, 'We did not get back the expected dereferenced values from dereferenceBuddyRecord() not including a ChadoBuddyRecord.');

    // CASE: calling dereferenceBuddyRecord() with a values array including a
    // buddy record. Normal use case.
    $sub_values = [
      'project.name' => 'Colossus: The Forbin Project',
      'project_description' => 'The perfect defense system',
    ];
    $buddy_record = new ChadoBuddyRecord();
    $buddy_record->setValues($sub_values);
    $expected_values = array_merge($values, $sub_values);
    $values['buddy_record'] = $buddy_record;
    $exception_caught = FALSE;
    try {
      $updated_values = $dereferenceBuddyRecord->invoke($instance, $values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, 'We should not get an exception when calling dereferenceBuddyRecord with valid values including a ChadoBuddyRecord.');
    $this->assertEqualsCanonicalizing($expected_values, $updated_values, 'We did not get back the expected dereferenced values from dereferenceBuddyRecord() including a ChadoBuddyRecord.');

    // CASE: using this ChadoBuddyRecord, getting a non-existant value from it causes an exception.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $should_fail = $buddy_record->getValue('non.exist');
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when calling getValue with an invalid key.');
    $this->assertStringContainsString("the key 'non.exist' is not present in the values array", $exception_message, "We did not get the exception message we expected when calling getValue with an invalid key.");

    // CASE: calling dereferenceBuddyRecord() with a key=>value pair in both the
    // ChadoBuddyRecord and in the values array, but the values are identical. Not an error.
    $values['project.name'] = $sub_values['project.name'];
    $exception_caught = FALSE;
    try {
      $updated_values = $dereferenceBuddyRecord->invoke($instance, $values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, 'We should not get an exception when calling dereferenceBuddyRecord with duplicate keys with identical values.');
    
    // CASE: calling dereferenceBuddyRecord() with a key=>value pair in both the
    // ChadoBuddyRecord and in the values array, but the values are different. Expect exception.
    $values['project.name'] = 'Incorrect name';
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $updated_values = $dereferenceBuddyRecord->invoke($instance, $values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when calling dereferenceBuddyRecord with duplicate keys with different values.');
    $this->assertStringContainsString('declared twice with different values', $exception_message, "We did not get the exception message we expected when calling dereferenceBuddyRecord() with duplicate keys.");

    // CASE: calling dereferenceBuddyRecord() with a values['buddy_record'] => array
    // (i.e. value is not actually a buddy record). Expect exception.
    $values['buddy_record'] = ['a.b' => 'c'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $updated_values = $dereferenceBuddyRecord->invoke($instance, $values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when calling dereferenceBuddyRecord with buddy_record not being a ChadoBuddyRecord.');
    $this->assertStringContainsString('something other than a ChadoBuddyRecord', $exception_message, "We did not get the exception message we expected when calling dereferenceBuddyRecord() with duplicate keys.");

    // CASE: valid values passed to validateOutput().
    $sub_values = [
      'project.name' => 'Colossus: The Forbin Project',
      'project_description' => 'The perfect defense system',
    ];
    $buddy_record_1 = new ChadoBuddyRecord();
    $buddy_record_1->setValues($sub_values);
    $buddy_record_2 = new ChadoBuddyRecord();
    $buddy_record_2->setValues($sub_values);

    $output_values = [$buddy_record_1];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateOutput->invoke($instance, $output_values, $sub_values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling validateOutput() with an array with one ChadoBuddyRecord.");

    // CASE: calling validateOutput() with a string as output.
    $output_values = 'should be array not string';
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateOutput->invoke($instance, $output_values, $sub_values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling validateOutput() with anything other than an array");
    $this->assertStringContainsString('did not retrieve the expected record', $exception_message, "We did not get the exception message we expected when calling validateOutput() with a string.");

    // CASE: calling validateOutput() with an empty array as output.
    $output_values = [];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateOutput->invoke($instance, $output_values, $sub_values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling validateOutput() with an empty array");
    $this->assertStringContainsString('did not retrieve the expected record', $exception_message, "We did not get the exception message we expected when calling validateOutput() with an empty array.");

    // CASE: calling validateOutput() with an array of something not a ChadoBuddyRecord.
    $output_values = ['a' => 'b'];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateOutput->invoke($instance, $output_values, $sub_values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling validateOutput() with an array that does not contain ChadoBuddyRecords");
    $this->assertStringContainsString('does not contain a ChadoBuddyRecord', $exception_message, "We did not get the exception message we expected when calling validateOutput() with an array without ChadoBuddyRecords.");

    // CASE: calling validateOutput() with an array containing multiple records.
    $output_values = [$buddy_record_1, $buddy_record_2];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $validateOutput->invoke($instance, $output_values, $sub_values);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception when calling validateOutput() with anything other than an array");
    $this->assertStringContainsString('more than one record', $exception_message, "We did not get the exception message we expected when calling validateOutput() with multiple records.");
  }



   /**
    * Tests methods dealing with the query object: addConditions().
    */
  public function testChadoBuddyQueryMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $this->assertIsObject($type, 'A chado buddy plugin service object was not returned.');
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject(
      $instance,
      "We did not have an object created when trying to create an ChadoBuddy instance."
    );

    // Make protected methods accessible.
    $reflection = new \ReflectionClass($instance);
    $addConditions = $reflection->getMethod('addConditions');
    $addConditions->setAccessible(true);

    // CASE: valid values passed to addConditions().
    $query = $this->connection->select('1:cv', 'cv');
    $conditions = [
      'cv.name' => 'EDAM',
    ];
    $options = [];
    $exception_caught = FALSE;
    try {
      $addConditions->invokeArgs($instance, [&$query, $conditions, $options]);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling addConditions() with valid conditions.");

    // CASE: calling addConditions() with a string as though its a query object ;-p
    // We don't bother to test this because it is a TypeError.

    // CASE: calling addConditions() with an empty array of conditions.
    $query = $this->connection->select('1:cv', 'cv');
    $conditions = [];
    $options = [];
    $exception_caught = FALSE;
    try {
      $addConditions->invokeArgs($instance, [&$query, $conditions, $options]);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling addConditions() with no conditions.");

    // CASE: calling addConditions() with a single key string to be case insensitive.
    $query = $this->connection->select('1:dbxref', 'dbxref');
    $conditions = [
      'db.name' => 'Edam',
      'dbxref.accession' => 'Ab000001',
    ];
    $options = ['case_insensitive' => 'db.name'];
    $exception_caught = FALSE;
    try {
      $addConditions->invokeArgs($instance, [&$query, $conditions, $options]);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling addConditions() with valid conditions.");
    $sql = (string) $query;
    $this->assertStringContainsString('LOWER(db.name)', $sql, "We did not get a query with case insensitivity for 'db.name'.");

    // CASE: calling addConditions() with an array of keys to be case insensitive.
    $options = ['case_insensitive' => ['db.name', 'dbxref.accession']];
    $exception_caught = FALSE;
    try {
      $addConditions->invokeArgs($instance, [&$query, $conditions, $options]);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, "We shouldn't get an exception when calling addConditions() with valid conditions.");
    $sql = (string) $query;
    $this->assertStringContainsString('LOWER(db.name)', $sql, "We did not get a query with case insensitivity for both 'db.name' and 'dbxref.accession'.");
    $this->assertStringContainsString('LOWER(dbxref.accession)', $sql, "We did not get a query with case insensitivity for both 'db.name' and 'dbxref.accession'.");

  }
}
