<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
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
    $this->assertIsObject($type, 'An chado buddy plugin service object was not returned.');

    // --Use the plugin manager to get a list of available implementations.
    $plugin_definitions = $type->getDefinitions();
    $this->assertIsArray(
      $plugin_definitions,
      'Implementations of the chado buddy plugin should be returned in an array.'
    );

    // --Use the plugin manager to create an instance.
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject($instance,
      "We did not have an object created when trying to create an ChadoBuddy instance.");
    $this->assertIsObject($instance->connection,
      "The chado connection should have been set by the plugin manager but the value is NOT AN OBJECT.");
    $this->assertInstanceOf(ChadoConnection::class, $instance->connection,
      "The chado connection should have been set by the plugin manager but the value is NOT A CHADOCONNECTION OBJECT.");
  }

  /**
   * Tests focused on basic getter/setters.
   */
  public function testChadoBuddyGetterSetters() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $this->assertIsObject($type, 'A chado buddy plugin service object was not returned.');
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject($instance,
      "We did not have an object created when trying to create an ChadoBuddy instance.");

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
    // Make methods accessible.
    $reflection = new \ReflectionClass($instance);
    $makeAlias = $reflection->getMethod('makeAlias');
    $makeAlias->setAccessible(true);
    $unmakeAlias = $reflection->getMethod('unmakeAlias');
    $unmakeAlias->setAccessible(true);

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
    // Make methods accessible.
    $removeTablePrefix = $reflection->getMethod('removeTablePrefix');
    $removeTablePrefix->setAccessible(true);

    // Test a typical use case.
    $referenced_values = ['cvterm.name' => 'sarah', 'cvterm.dbxref_id' => 3, 'cvterm.cv_id' => 9];
    $expected_values = ['name' => 'sarah', 'dbxref_id' => 3, 'cv_id' => 9];
    $dereferenced_values = $removeTablePrefix->invoke($instance, $referenced_values);
    $this->assertEquals($expected_values, $dereferenced_values, "We did not get the dereferenced values we expected when calling removeTablePrefix on " . print_r($referenced_values, TRUE));

    // Test when more then one table of values is passed in and an ambiguous column
    // name would result (e.g. cv.name and cvterm.name). Expect exception.
    $referenced_values = ['cv.name' => 'aldous', 'cvterm.name' => 'huxley'];
    $exception_caught = FALSE;
    try {
      $dereferenced_values = $removeTablePrefix->invoke($instance, $referenced_values);
    } catch (\Exception $e) {
      $exception_caught = TRUE;
    }
    $this->assertTrue($exception_caught, 'Did not catch exception that should have been thrown for removeTablePrefix()');

    // Test when a key does not have a dot or when it has multiple dots. Only trim to first dot.
    $referenced_values = ['name_no_dot' => 'newton', 'cvterm.name.fictional.indeed' => 'dumbeldore'];
    $expected_values = ['name_no_dot' => 'newton', 'name.fictional.indeed' => 'dumbeldore'];
    $dereferenced_values = $removeTablePrefix->invoke($instance, $referenced_values);
    $this->assertEquals($expected_values, $dereferenced_values, 'Unexpected dereferenced values from removeTablePrefix()');
  }
}
