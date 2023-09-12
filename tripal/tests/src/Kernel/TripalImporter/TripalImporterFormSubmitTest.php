<?php

namespace Drupal\Tests\tripal\Kernel\TripalImporter;

use Drupal\KernelTests\KernelTestBase;
use \Drupal\Tests\user\Traits\UserCreationTrait;

/**
 * Tests the base functionality for importers.
 *
 * Cannot test actually implemented importers as those
 * require database specific implementations.
 */
class TripalImporterFormSubmitTest extends KernelTestBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal'];

  use UserCreationTrait;

  /**
   * A mocked TripalImporter object
   * @var \Drupal\tripal\TripalImporter\PluginManagers\TripalImporterBase
   */
  protected $mock_plugin;

  /**
   * A pretend listing of annotations associated with the mock_plugin.
   *
   * Define what we want to be detected in the annotation of our
   * fake importer class. While we do NOT have an actual class where
   * this annotation is defined... TripalImporter access the annotation
   * through the getDefinitions method in the plugin manager.
   *
   * These are the default with all base importer fields turned off.
   * Specific tests will alter these before building the form to
   * test specific cases.
   * @var Array
   */
  protected $definitions = [
    'fakeImporterName' => [
      'id' => 'fakeImporterName',
      'label' => 'Gemstone Loader',
      'description' => 'Imports details on the incredible diversity of gemstones created by our earth into Chado.',
      'file_types' => ["gem", "txt"],
      'upload_description' => "Please provide a plain text, tab-delimited file of gemstone descriptions making sure to include the details which make them most unique and beautiful.",
      'upload_title' => 'Gemstone Descriptions',
      'use_analysis' => FALSE,
      'require_analysis' => FALSE,
      'button_text' => 'Import file',
      'file_upload' => TRUE,
      'file_load' => TRUE,
      'file_remote' => TRUE,
      'file_required' => TRUE,
      'cardinality' => 1,
    ],
  ];

  /**
   * A selection of form elements to be provided by our fake importer.
   * @var Array
   */
  protected $form = [
    'gemstone_composition' => [
      '#title' => 'Chemical Composition',
      '#type' => 'select',
      '#description' => 'Choose the chemical composition that all gems in your input file fall into.',
      '#required' => FALSE,
      '#options' => [
        'borate' => 'Borate (e.g. Howlite)',
        'carbon' => 'Carbon (e.g. Diamond)',
        'carbonate' => 'Carbonate (e.g. Azurite, Calcite, Malachite)',
        'halide' => 'Halide (e.g. Fluorite)',
        'igneous' => 'Igneous Rock (e.g. obsidian, lava stone)',
        'organic' => 'Organic (e.g. Amber, Pearl)',
        'silicate' => 'Silicate (e.g. Amazonite, Danburite, Lepidolite)'
      ],
      '#empty_option' => '- Select -',
    ],
  ];

  /**
   * An analysis form element to be provided by our fake importer.
   * @var Array
   */
  protected $analysis_form = [
    'analysis_method' => [
      '#title' => 'Gemstone Validation',
      '#type' => 'select',
      '#description' => 'Choose the validation methodology that proves these stones are authentic.',
      '#required' => FALSE,
      '#options' => [
        'raman' => 'Raman spectroscopy',
        'luminescence' => 'Luminescence',
        'acid' => 'Acid Testing',
      ],
      '#empty_option' => '- Select -',
    ],
  ];

  protected $test_file;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Ensure we can access file_managed related functionality from Drupal.
    // ... users need access to system.action config?
    $this->installConfig('system');
    // ... managed files are associated with a user.
    $this->installEntitySchema('user');
    // ... Finally the file module + tables itself.
    $this->installEntitySchema('file');
    $this->installSchema('file', ['file_usage']);

    // Create and log-in a user.
    $this->setUpCurrentUser();

    // Create a managed file to use as needed.
    $filepath = 'temporary://Файл для тестирования ' . $this->randomMachineName();
    $contents = "file_put_contents() doesn't seem to appreciate empty strings so let's put in some data.";
    file_put_contents($filepath, $contents);
    $file = \Drupal\file\Entity\File::create([
      'uri' => $filepath,
      'uid' => 1,
    ]);
    $file->save();
    $this->assertFileExists($filepath);
    $this->test_file = $file;
  }

  /**
   * Tests the validation of the file elements added by the Base Tripal Importer.
   *
   * Specifically,
   *  -
   */
  public function testTripalImporterFormValidateFile() {

    $manager = $this->setMockManager($this->definitions);
    $container = \Drupal::getContainer();
    $container->set('tripal.importer', $manager);

    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions[$plugin_id];
    $test_file_path = 'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt';

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id
    );
    $this->assertIsArray($form,
      'We expect the form builder to return a form but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Now setup the form_state.
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setValue('file_remote', $test_file_path);

    // Now try validation!
    // This is expected to pass as that is a valid URL
    \Drupal::formBuilder()->submitForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $form_state
    );

    $this->assertTrue($form_state->isValidationComplete(),
      "We expect the form state to have been updated to indicate that validation is complete.");
  }

  /**
   * HELPER: Creates a mock plugin + plugin manager.
   */
  protected function setMockManager($annotation) {

    // Mock Tripal Importer Plugin.
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $plugin_definition = [];
    $this->mock_plugin = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_definition]
    );
    $this->mock_plugin->method('form')
      ->willReturn($this->form);
    $this->mock_plugin->method('addAnalysis')
      ->willReturn($this->analysis_form);

    // Mock Plugin Manager.
    $manager = $this->createMock(\Drupal\tripal\TripalImporter\PluginManagers\TripalImporterManager::class);
    $manager->method('createInstance')
      ->willReturn($this->mock_plugin);
    $manager->method('getDefinitions')
      ->willReturn($annotation);

    return $manager;
  }
}
