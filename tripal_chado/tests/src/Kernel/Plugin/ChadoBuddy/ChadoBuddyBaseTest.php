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
  protected $plugin_definition = [
    'id' => 'fake_chado_buddy',
    'label' => 'Gemstone Querier',
    'description' => 'Queries details on the incredible diversity of gemstones created by our earth into Chado.',
  ];

  protected $test_file;

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

	}
}
