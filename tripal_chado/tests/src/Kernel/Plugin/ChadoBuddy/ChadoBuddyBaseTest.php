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
    $this->assertIsObject($type, 'An chado buddy plugin service object was not returned.');
    $instance = $type->createInstance('chado_cvterm_buddy', []);
    $this->assertIsObject($instance,
      "We did not have an object created when trying to create an ChadoBuddy instance.");

    $label = $instance->label();
    $this->assertIsString($label, "The label is expected to be a string.");
    $this->assertEquals($label, $this->cvtermbuddy_plugin_definition['label'],
      "The label returned did not match what we expected for the Chado Cvterm Buddy.");

    $description = $instance->description();
    $this->assertIsString($description, "The description is expected to be a string.");
    $this->assertEquals($description, $this->cvtermbuddy_plugin_definition['description'],
      "The description returned did not match what we expected for the Chado Cvterm Buddy.");
  }
}
