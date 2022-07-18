<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the base functionality for importers.
 *
 * Cannot test actually implemented importers as those
 * require database specific implementations.
 */
class TripalImporterTest extends BrowserTestBase {
  protected $defaultTheme = 'stable';

  protected static $modules = ['tripal'];

	/**
   * Tests focusing on the Tripal Importer plugin system.
   *
   * @group tripal_importer
   */
  public function testTripalImporterManager() {

    // Test the Tripal Importer Plugin Manager.
		// --Ensure we can instantiate the plugin manager.
		$type = \Drupal::service('tripal.importer');
		// Note: If the plugin manager is not found you will get a ServiceNotFoundException.
		$this->assertIsObject($type, 'An importer plugin service object was not returned.');

		// --Use the plugin manager to get a list of available implementations.
		$plugin_definitions = $type->getDefinitions();
		$this->assertIsArray(
			$plugin_definitions,
			'Implementations of the tripal importer plugin should be returned in an array.'
		);

	}

	/**
   * Tests focusing on the Tripal Importer plugin system.
   *
   * Functions to test:
   * - tripal_run_importer
   * - tripal_run_importer_run
   * - tripal_run_importer_post_run
   *
   * @group tripal_importer
   */
  public function testTripalImporterFunctions() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
	}

  /**
   * Tests focusing on the Tripal importer form.
   *
   * @group tripal_importer
   */
  public function testTripalImporterForm() {

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm('Drupal\tripal\Form\TripalImporterForm');

    // Ensure we are able to build the form.
    $this->assertIsArray(
      $form,
      'The form array was not returned by the form builder.'
    );

    // Check the form_id
    $this->assertEquals(
      'tripal_admin_form_tripalimporter',
      $form['#form_id'],
      'We did not get the form id we expected.'
    );

    // Since we didn't provide a Tripal Importer plugin id...
    // We shouldn't get the file and submit form elements.
    $this->assertArrayNotHasKey(
      'file',
      $form,
      "The form should not have a file fieldset if we don't provide a specific importer."
    );
    $this->assertArrayNotHasKey(
      'button',
      $form,
      "The form should not have a submit button if we don't provide a specific importer."
    );
	}

  /**
   * Tests focusing on the Tripal importer base class.
   *
   * @group tripal_importer
   */
  public function testTripalImporterBase() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
	}
}
