<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\TripalImporter;

use Drupal\Core\Url;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

/**
 * Tests the functionality of the FASTA Importer Importer.
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group GFF3Importer
 */
class GFF3ImporterFormTest extends ChadoTestKernelBase {

	protected $defaultTheme = 'stark';

	protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected $connection;

	/**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

		// Open connection to Chado
		$this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Ensure we can access file_managed related functionality from Drupal.
    // ... users need access to system.action config?
    $this->installConfig('system');
    // ... managed files are associated with a user.
    $this->installEntitySchema('user');
    // ... Finally the file module + tables itself.
    $this->installEntitySchema('file');
    $this->installSchema('file', ['file_usage']);

  }

	/**
   * Tests focusing on the importer form.
   */
  public function testImporterForm() {

		$plugin_id = 'chado_gff3_loader';
    $importer_label = 'Chado GFF3 File Loader';

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id
    );
    // Ensure we are able to build the form.
    $this->assertIsArray($form,
      'We expect the form builder to return a form but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Now that we have provided a plugin_id, we expect it to have...
    // title matching our importer label.
    $this->assertArrayHasKey('#title', $form,
      "The form should have a title set.");
    $this->assertEquals($importer_label, $form['#title'],
      "The title should match the label annotated for our plugin.");
    // the plugin_id stored in a value form element.
    $this->assertArrayHasKey('importer_plugin_id', $form,
      "The form should have an element to save the plugin_id.");
    $this->assertEquals($plugin_id, $form['importer_plugin_id']['#value'],
      "The importer_plugin_id[#value] should be set to our plugin_id.");
    // a submit button.
    $this->assertArrayHasKey('button', $form,
      "The form should have a submit button since we indicated a specific importer.");
    // This importer requires an analysis.
    $this->assertArrayHasKey('analysis_id', $form,
      "The from should not include analysis element, yet one exists.");

    // We should also have our importer-specific form elements added to the form!
    //$this->assertArrayHasKey('instructions', $form,
    //  "The form should include an instructions form element.");

	}

}
