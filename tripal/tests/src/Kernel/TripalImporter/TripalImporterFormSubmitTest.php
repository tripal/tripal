<?php

namespace Drupal\Tests\tripal\Kernel\TripalImporter;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use \Drupal\Tests\user\Traits\UserCreationTrait;

/**
 * Tests the base functionality for importers.
 *
 * Cannot test actually implemented importers as those
 * require database specific implementations.
 *
 * @group TripalImporter
 */
class TripalImporterFormSubmitTest extends TripalTestKernelBase {
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
      'file_local' => TRUE,
      'file_remote' => TRUE,
      'file_required' => TRUE,
      'cardinality' => 1,
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

    // Ensure we have our tripal import tables.
    $this->installSchema('tripal', ['tripal_import', 'tripal_jobs']);

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
   * Tests the validation of the local file element added by the Base Tripal Importer.
   *
   * Specifically,
   *  - VALID: provide an existing local file, correct file format
   *  - ERROR: indicate a file which does not exist locally.
   */
  public function testTripalImporterFormValidateLocalFile() {
    global $DRUPAL_ROOT;

    $manager = $this->setMockManager($this->definitions);
    $container = \Drupal::getContainer();
    $container->set('tripal.importer', $manager);

    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions;

    // --- CASE VALID
    // --- Supply a valid remote path and ensure the form works!
    $test_file_path = 'modules/contrib/tripal/LICENSE.txt';

    // Now setup the form_state.
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setValue('file_local', $test_file_path);

    // Now try validation!
    \Drupal::formBuilder()->submitForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $form_state
    );
    // And do some basic checks to ensure there were no errors.
    $this->assertTrue($form_state->isValidationComplete(),
      "We expect the form state to have been updated to indicate that validation is complete.");
    //   Looking for form validation errors
    $form_validation_messages = $form_state->getErrors();
    $helpful_output = [];
    foreach ($form_validation_messages as $element => $markup) {
      $helpful_output[] = $element . " => " . (string) $markup;
    }
    $this->assertCount(0, $form_validation_messages,
      "We should not have any validation errors for '$test_file_path' but instead we have: " . implode(" AND ", $helpful_output));
    //   Looking for drupal message errors.
    $messages = \Drupal::messenger()->all();
    $this->assertIsArray($messages,
      "We expect to have status messages to the user on submission of the form.");
    $this->assertArrayNotHasKey('error', $messages,
      "There should not be any error messages from this form. Instead we recieved: " . print_r($messages, TRUE));
    //   Now delete drupal messages so we start the next test clean.
    \Drupal::messenger()->deleteAll();

    // --- CASE ERROR
    // --- Supply a file which does not exist locally
    $bad_remote_uri = [
      '/var/di/dump/doo',
      'relative/but/not/here',
      'singleWord.no',
    ];
    foreach ($bad_remote_uri as $test_file_path) {
      // Now setup the form_state.
      $form_state = new \Drupal\Core\Form\FormState();
      $form_state->addBuildInfo('args', [$plugin_id]);
      $form_state->setValue('file_local', $test_file_path);

      // Now try validation!
      \Drupal::formBuilder()->submitForm(
        'Drupal\tripal\Form\TripalImporterForm',
        $form_state
      );
      // And do some basic checks to ensure there were no errors.
      $this->assertTrue($form_state->isValidationComplete(),
        "We expect the form state to have been updated to indicate that validation is complete.");
      //   Looking for form validation errors
      $form_validation_messages = $form_state->getErrors();
      $this->assertCount(1, $form_validation_messages,
        "We expect validation errors for '$test_file_path' but did not recieve them.");
      $this->assertArrayHasKey('file_local', $form_validation_messages,
        "There should be an entry for file_local in the validation errors for '$test_file_path'.");
      //   Looking for drupal message errors.
      $messages = \Drupal::messenger()->all();
      $this->assertIsArray($messages,
        "We expect to have status messages to the user on submission of the form.");
      $this->assertArrayHasKey('error', $messages,
        "There should be an error message from this form but we didn't recieve any.");
      $this->assertCount(1, $messages['error'],
        "There should be only one error message.");
      $this->assertStringContainsString('Cannot find the file', (string) $messages['error'][0],
        "The error did not match the one we expected for an file which doesn't exist for file_local.");
      $this->assertArrayNotHasKey('status', $messages,
        "There should not be any success/status messages from this form. Instead we recieved: " . print_r($messages, TRUE));
      //   Now delete drupal messages so we start the next test clean.
      \Drupal::messenger()->deleteAll();
    }
  }

  /**
   * Tests the validation of the remote file element added by the Base Tripal Importer.
   *
   * Specifically,
   *  - VALID: provide a valid remote URL
   *  - ERROR: provide a badly formatted URI
   *  - ERROR: provide a correctly formatted but non-existent URI
   */
  public function testTripalImporterFormValidateRemoteFile() {

    $manager = $this->setMockManager($this->definitions);
    $container = \Drupal::getContainer();
    $container->set('tripal.importer', $manager);

    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions;

    // --- CASE VALID
    // --- Supply a valid remote path and ensure the form works!
    $test_file_path = 'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt';

    // Now setup the form_state.
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setValue('file_remote', $test_file_path);

    // Now try validation!
    \Drupal::formBuilder()->submitForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $form_state
    );
    // And do some basic checks to ensure there were no errors.
    $this->assertTrue($form_state->isValidationComplete(),
      "We expect the form state to have been updated to indicate that validation is complete.");
    //   Looking for form validation errors
    $form_validation_messages = $form_state->getErrors();
    $this->assertCount(0, $form_validation_messages,
      "We should not have any validation errors for '$test_file_path'.");
    //   Looking for drupal message errors.
    $messages = \Drupal::messenger()->all();
    $this->assertIsArray($messages,
      "We expect to have status messages to the user on submission of the form.");
    $this->assertArrayNotHasKey('error', $messages,
      "There should not be any error messages from this form. Instead we recieved: " . print_r($messages, TRUE));
    //   Now delete drupal messages so we start the next test clean.
    \Drupal::messenger()->deleteAll();

    // --- CASE ERROR
    // --- Supply a text string that is clearly not a URL for file_remote and ensure it fails.
    $bad_remote_uri = [
      'lalalalalalala',
      'http://.com',
      'http://...',
      'http://',
      'http://£$"%$&*.com',
    ];
    foreach ($bad_remote_uri as $test_file_path) {
      // Now setup the form_state.
      $form_state = new \Drupal\Core\Form\FormState();
      $form_state->addBuildInfo('args', [$plugin_id]);
      $form_state->setValue('file_remote', $test_file_path);

      // Now try validation!
      \Drupal::formBuilder()->submitForm(
        'Drupal\tripal\Form\TripalImporterForm',
        $form_state
      );
      // And do some basic checks to ensure there were no errors.
      $this->assertTrue($form_state->isValidationComplete(),
        "We expect the form state to have been updated to indicate that validation is complete.");
      //   Looking for form validation errors
      $form_validation_messages = $form_state->getErrors();
      $this->assertCount(1, $form_validation_messages,
        "We expect validation errors for '$test_file_path' but did not recieve them.");
      $this->assertArrayHasKey('file_remote', $form_validation_messages,
        "There should be an entry for file_remote in the validation errors for '$test_file_path'.");
      //   Looking for drupal message errors.
      $messages = \Drupal::messenger()->all();
      $this->assertIsArray($messages,
        "We expect to have status messages to the user on submission of the form.");
      $this->assertArrayHasKey('error', $messages,
        "There should be an error message from this form but we didn't recieve any.");
      $this->assertCount(1, $messages['error'],
        "There should be only one error message.");
      $this->assertStringContainsString('not a valid URI', (string) $messages['error'][0],
        "The error did not match the one we expected for an invalid URL passed to file_remote.");
      $this->assertArrayNotHasKey('status', $messages,
        "There should not be any success/status messages from this form. Instead we recieved: " . print_r($messages, TRUE));
      //   Now delete drupal messages so we start the next test clean.
      \Drupal::messenger()->deleteAll();
    }

    // --- CASE ERROR
    // --- Supply a URI that is valid but does not resolve to a web page.
    $bad_remote_uri = [
      'http://notreallyasite.com/',
      'https://notreallyatripalsite.github.com',
    ];
    foreach ($bad_remote_uri as $test_file_path) {
      // Now setup the form_state.
      $form_state = new \Drupal\Core\Form\FormState();
      $form_state->addBuildInfo('args', [$plugin_id]);
      $form_state->setValue('file_remote', $test_file_path);

      // Now try validation!
      \Drupal::formBuilder()->submitForm(
        'Drupal\tripal\Form\TripalImporterForm',
        $form_state
      );
      // And do some basic checks to ensure there were no errors.
      $this->assertTrue($form_state->isValidationComplete(),
        "We expect the form state to have been updated to indicate that validation is complete.");
      //   Looking for form validation errors
      $form_validation_messages = $form_state->getErrors();
      $this->assertCount(1, $form_validation_messages,
        "We expect validation errors for '$test_file_path' but did not recieve them.");
      $this->assertArrayHasKey('file_remote', $form_validation_messages,
        "There should be an entry for file_remote in the validation errors for '$test_file_path'.");
      //   Looking for drupal message errors.
      $messages = \Drupal::messenger()->all();
      $this->assertIsArray($messages,
        "We expect to have status messages to the user on submission of the form.");
      $this->assertArrayHasKey('error', $messages,
        "There should be an error message from this form but we didn't recieve any.");
      $this->assertCount(1, $messages['error'],
        "There should be only one error message.");
      $this->assertStringContainsString('cannot be accessed', (string) $messages['error'][0],
        "The error did not match the one we expected for an invalid URL passed to file_remote.");
      $this->assertArrayNotHasKey('status', $messages,
        "There should not be any success/status messages from this form. Instead we recieved: " . print_r($messages, TRUE));
      //   Now delete drupal messages so we start the next test clean.
      \Drupal::messenger()->deleteAll();
    }
  }

  /**
   * HELPER: Creates a mock plugin + plugin manager.
   */
  protected function setMockManager($annotation) {

    // Mock Tripal Importer Plugin.
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $this->mock_plugin = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $annotation]
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
      ->willReturn([$plugin_id => $annotation]);

    return $manager;
  }
}
