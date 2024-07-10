<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Form\FormState;

/**
 * Tests the base functionality for importers.
 *
 * Cannot test actually implemented importers as those
 * require database specific implementations.
 *
 * @group TripalImporter
 */
class TripalImporterFormBuildTest extends TripalTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal'];

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
      'use_button' => TRUE,
      'button_text' => 'Import file',
      'file_upload' => FALSE,
      'file_load' => FALSE,
      'file_remote' => FALSE,
      'file_required' => FALSE,
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
      '#required' => TRUE,
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
      '#required' => TRUE,
      '#options' => [
        'raman' => 'Raman spectroscopy',
        'luminescence' => 'Luminescence',
        'acid' => 'Acid Testing',
      ],
      '#empty_option' => '- Select -',
    ],
  ];

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

  }

  /**
   * HELPER: Creates a mock plugin + plugin manager.
   */
  protected function setMockManager($annotation) {

    // Mock Tripal Importer Plugin.
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $plugin_definition = $annotation['fakeImporterName'];
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

  /**
   * Tests focusing on the Tripal importer form.
   */
  public function testTripalImporterForm() {

    $manager = $this->setMockManager($this->definitions);
    $container = \Drupal::getContainer();
    $container->set('tripal.importer', $manager);

    // -- Test form with no plugin_id supplied.
    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm'
    );

    // Ensure we are able to build the form.
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Since we didn't provide a Tripal Importer plugin id...
    // We shouldn't get the file and submit form elements.
    $this->assertArrayNotHasKey('file', $form,
      "The form should not have a file fieldset if we don't provide a specific importer.");
    $this->assertArrayNotHasKey('button', $form,
      "The form should not have a submit button if we don't provide a specific importer.");

    // -- Test form with plugin_id but no file or analysis.
    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions[$plugin_id];

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
    $this->assertEquals($expected['label'], $form['#title'],
      "The title should match the label annotated for our fake plugin.");
    // the plugin_id stored in a value form element.
    $this->assertArrayHasKey('importer_plugin_id', $form,
      "The form should have an element to save the plugin_id.");
    $this->assertEquals($plugin_id, $form['importer_plugin_id']['#value'],
      "The importer_plugin_id[#value] should be set to our fake plugin_id.");
    // a submit button.
    $this->assertArrayHasKey('button', $form,
      "The form should have a submit button since we indicated a specific importer.");

    // We should also have our importer specific form elements added to the form!
    $this->assertArrayHasKey('gemstone_composition', $form,
      "The form should include our plugin-specific form elements.");
    foreach ($this->form['gemstone_composition'] as $expected_key => $expected_element) {
      $this->assertArrayHasKey($expected_key, $form['gemstone_composition'],
        "The form includes our plugin-specific form element but it does not have all they keys we expect.");
    }

    // Our default annotation indicates no file or analysis elements
    // should be added so let's confirm they are not.
    $this->assertArrayNotHasKey('file', $form,
      "Our default annotation for our fake importer indicates there should not be a file element added.");
    $this->assertArrayNotHasKey('analysis_method', $form,
      "Our default annotation for our fake importer indicates there should not be an analysis element added.");
	}

  /**
   * Confirm that the file-related form elements are added to the form
   * as expected based on plugin annotation.
   */
  public function testTripalImporterFormFiles() {

    $container = \Drupal::getContainer();
    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions[$plugin_id];

    // -- Include all file elements.
    $expected['file_upload'] = TRUE;
    $expected['file_load'] = TRUE;
    $expected['file_local'] = TRUE;
    $expected['file_remote'] = TRUE;
    $expected['file_required'] = TRUE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Check the file fieldset and upload description are as expected.
    $this->assertArrayHasKey('file', $form,
      "The form should have a file key as our fake importer definition indicates we want one.");
    $this->assertEquals('fieldset', $form['file']['#type'],
      "The file element in the form is expected to be a fieldset.");
    $this->assertArrayHasKey('upload_description', $form['file'],
      "If any file element is included, there should be an upload description added to the file fieldset.");
    $this->assertStringContainsString($expected['upload_description'], $form['file']['upload_description']['#markup'],
      "The upload description should match the one provided in the plugin annotation.");

    // Check the Upload file element
    $this->assertArrayHasKey('file_upload', $form['file'],
      "The form should have a file upload form element based on our annotation.");
    $this->assertEquals('html5_file', $form['file']['file_upload']['#type'],
      "The file_upload element is not of the expected type.");
    $this->assertEquals('tripal_importer', $form['file']['file_upload']['#usage_type'],
      "The file_upload element should indicate it is associated with tripal_importer.");
    $this->assertEquals($expected['file_types'], $form['file']['file_upload']['#allowed_types'],
      "Only the allowed types indicated by the annotation should be indicated for the file_upload element.");
    $this->assertEquals($expected['cardinality'], $form['file']['file_upload']['#cardinality'],
      "The cardinality indicated in the annotation should be reflected in the file_upload element.");
    // There should not be any existing files associated with this user.
    // So check that form elements does not exist.
    $this->assertArrayNotHasKey('file_upload_existing', $form['file'],
      "The form should NOT have an element for existing files as we have not created a user or associated files.");

    // Check the local file element
    $this->assertArrayHasKey('file_local', $form['file'],
      "The form should have a local file form element based on our annotation.");
    $this->assertEquals('textfield', $form['file']['file_local']['#type'],
      "The file_local element is not of the expected type.");

    // Check the remote file element
    $this->assertArrayHasKey('file_remote', $form['file'],
      "The form should have a remote file form element based on our annotation.");
    $this->assertEquals('textfield', $form['file']['file_remote']['#type'],
      "The file_remote element is not of the expected type.");

    // -- Include file_upload only and ensure other elements are not included.
    $expected = $this->definitions[$plugin_id];
    $expected['file_upload'] = TRUE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Check the file fieldset and upload description are as expected.
    $this->assertArrayHasKey('file', $form,
      "The form should have a file key as our fake importer definition indicates we want one.");
    $this->assertEquals('fieldset', $form['file']['#type'],
      "The file element in the form is expected to be a fieldset.");
    $this->assertArrayHasKey('upload_description', $form['file'],
      "If any file element is included, there should be an upload description added to the file fieldset.");
    $this->assertStringContainsString($expected['upload_description'], $form['file']['upload_description']['#markup'],
      "The upload description should match the one provided in the plugin annotation.");

    // Check the Upload file element
    $this->assertArrayHasKey('file_upload', $form['file'],
      "The form should have a file upload form element based on our annotation.");
    // But NOT the other two.
    $this->assertArrayNotHasKey('file_local', $form['file'],
      "The form should NOT have a local file form element based on our annotation.");
    $this->assertArrayNotHasKey('file_remote', $form['file'],
      "The form should NOT have a remote file form element based on our annotation.");

    // -- Include file_local only and ensure other elements are not included.
    $expected = $this->definitions[$plugin_id];
    $expected['file_local'] = TRUE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Check the file fieldset and upload description are as expected.
    $this->assertArrayHasKey('file', $form,
      "The form should have a file key as our fake importer definition indicates we want one.");
    $this->assertEquals('fieldset', $form['file']['#type'],
      "The file element in the form is expected to be a fieldset.");
    $this->assertArrayHasKey('upload_description', $form['file'],
      "If any file element is included, there should be an upload description added to the file fieldset.");
    $this->assertStringContainsString($expected['upload_description'], $form['file']['upload_description']['#markup'],
      "The upload description should match the one provided in the plugin annotation.");

    // Check the file element we should have
    $this->assertArrayHasKey('file_local', $form['file'],
      "The form should  have a local file form element based on our annotation.");
    // But NOT the other two.
    $this->assertArrayNotHasKey('file_upload', $form['file'],
      "The form should NOT have a file upload form element based on our annotation.");
    $this->assertArrayNotHasKey('file_remote', $form['file'],
      "The form should NOT have a remote file form element based on our annotation.");

    // -- Include file_upload only and ensure other elements are not included.
    $expected = $this->definitions[$plugin_id];
    $expected['file_remote'] = TRUE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // Check the file fieldset and upload description are as expected.
    $this->assertArrayHasKey('file', $form,
      "The form should have a file key as our fake importer definition indicates we want one.");
    $this->assertEquals('fieldset', $form['file']['#type'],
      "The file element in the form is expected to be a fieldset.");
    $this->assertArrayHasKey('upload_description', $form['file'],
      "If any file element is included, there should be an upload description added to the file fieldset.");
    $this->assertStringContainsString($expected['upload_description'], $form['file']['upload_description']['#markup'],
      "The upload description should match the one provided in the plugin annotation.");

    // Check the file element we should have
    $this->assertArrayHasKey('file_remote', $form['file'],
      "The form should NOT have a remote file form element based on our annotation.");
    // But NOT the other two.
    $this->assertArrayNotHasKey('file_upload', $form['file'],
      "The form should NOT have a file upload form element based on our annotation.");
    $this->assertArrayNotHasKey('file_local', $form['file'],
      "The form should  have a local file form element based on our annotation.");
  }

    /**
   * Confirm that the file-related form elements are added to the form
   * as expected based on plugin annotation.
   */
  public function testTripalImporterFormAnalysis() {

    $container = \Drupal::getContainer();
    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions[$plugin_id];

    // -- Indicate to use an analysis elements.
    $expected['use_analysis'] = TRUE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our analysis element is in the form.
    $this->assertArrayHasKey('analysis_method', $form,
      "Our analysis form element should be included based on the annotation.");
    $this->assertEquals('Gemstone Validation', $form['analysis_method']['#title'],
      "The title for our analysis element did not match what we expected.");
    $this->assertCount(4, $form['analysis_method']['#options'],
      "There were not the expected number of options including the empty option that we expected for our analysis.");
  }

  /**
   * Confirm that importers whose annotation indicates they do not want a submit
   * button, do not get a submit button forced on them.
   */
  public function testTripalImporterFormNoButton() {

    $container = \Drupal::getContainer();
    $plugin_id = 'fakeImporterName';
    $expected = $this->definitions[$plugin_id];

    // -- Indicate to use an analysis elements.
    $expected['use_button'] = FALSE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our analysis element is in the form.
    $this->assertArrayNotHasKey('button', $form,
      "We should not have a submit button if our annotation sets use_button to FALSE but we do.");

  }

  /**
   * Confirm that importers can indicate they want the form submit button
   * disabled using a particular key in the form state.
   */
  public function testTripalImporterFormDisableButtonFormStateOnly() {

    $container = \Drupal::getContainer();
    $form_builder = \Drupal::formBuilder();
    $plugin_id = 'fakeImporterName';
    $form_id = 'tripal_admin_form_tripalimporter';
    $form_class = 'Drupal\tripal\Form\TripalImporterForm';

    // CASE 1: No Form State
    $expected = $this->definitions[$plugin_id];
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    // and confirm that when the form is built with no Form State that
    // the button is enabled.
    $form = $form_builder->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'The form builder should return a form array.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our button element is in the form.
    $this->assertArrayHasKey('button', $form,
      "We should have a submit button.");
    $this->assertArrayHasKey('#disabled', $form['button'],
      "The submit button should have the disabled key set.");
    $this->assertFalse($form['button']['#disabled'],
      "The submit button should BE ENABLED when the form is built without a specific form state.");

    // CASE 2: Form State[disable_submit] = TRUE when form rebuilt.
    $form_state = new FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setStorage(['disable_TripalImporter_submit' => TRUE]);
    $form = $form_builder->buildForm($form_class, $form_state);

    $this->assertIsArray($form,
      'The form builder should return a form array.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our button element is in the form.
    $this->assertArrayHasKey('button', $form,
      "We should have a submit button.");
    $this->assertArrayHasKey('#disabled', $form['button'],
      "The submit button should have the disabled key set.");
    $this->assertTrue($form['button']['#disabled'],
      "The submit button should BE DISABLED when the form is built with the form state disable_TripalImporter_submit set to TRUE.");

    // CASE 3: Form State[disable_submit] = FALSE when form rebuilt.
    $form_state = new FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setStorage(['disable_TripalImporter_submit' => FALSE]);
    $form = $form_builder->buildForm($form_class, $form_state);

    $this->assertIsArray($form,
      'The form builder should return a form array.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our button element is in the form.
    $this->assertArrayHasKey('button', $form,
      "We should have a submit button.");
    $this->assertArrayHasKey('#disabled', $form['button'],
      "The submit button should have the disabled key set.");
    $this->assertFalse($form['button']['#disabled'],
      "The submit button should BE ENABLED when the form is built with the form state disable_TripalImporter_submit set to FALSE.");
  }

  /**
   * Confirm that importers can indicate they want the form submit button
   * disabled via annotation and change it via form state.
   */
  public function testTripalImporterFormDisableButtonAnnotation() {

    $container = \Drupal::getContainer();
    $form_builder = \Drupal::formBuilder();
    $plugin_id = 'fakeImporterName';
    $form_id = 'tripal_admin_form_tripalimporter';
    $form_class = 'Drupal\tripal\Form\TripalImporterForm';

    // CASE 1: No Form State
    $expected = $this->definitions[$plugin_id];
    $expected['submit_disabled'] = TRUE;
    $manager = $this->setMockManager([$plugin_id => $expected]);
    $container->set('tripal.importer', $manager);

    // Build the form using the Drupal form builder.
    // and confirm that when the form is built with no Form State that
    // the button is enabled.
    $form = $form_builder->getForm(
      'Drupal\tripal\Form\TripalImporterForm',
      $plugin_id,
    );
    $this->assertIsArray($form,
      'The form builder should return a form array.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our button element is in the form.
    $this->assertArrayHasKey('button', $form,
      "We should have a submit button.");
    $this->assertArrayHasKey('#disabled', $form['button'],
      "The submit button should have the disabled key set.");
    $this->assertTrue($form['button']['#disabled'],
      "The submit button should BE DISABLED when the form is built without a specific form state but annotation says it should be.");

    // CASE 2: Form State[disable_submit] = FALSE when form rebuilt.
    $form_state = new FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setStorage(['disable_TripalImporter_submit' => FALSE]);
    $form = $form_builder->buildForm($form_class, $form_state);

    $this->assertIsArray($form,
      'The form builder should return a form array.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our button element is in the form.
    $this->assertArrayHasKey('button', $form,
      "We should have a submit button.");
    $this->assertArrayHasKey('#disabled', $form['button'],
      "The submit button should have the disabled key set.");
    $this->assertFalse($form['button']['#disabled'],
      "The submit button should BE ENABLED when the form is built with the form state disable_TripalImporter_submit set to FALSE.");

    // CASE 3: Form State[disable_submit] = TRUE when form rebuilt.
    $form_state = new FormState();
    $form_state->addBuildInfo('args', [$plugin_id]);
    $form_state->setStorage(['disable_TripalImporter_submit' => TRUE]);
    $form = $form_builder->buildForm($form_class, $form_state);

    $this->assertIsArray($form,
      'The form builder should return a form array.');
    $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
      'We did not get the form id we expected.');

    // check that our button element is in the form.
    $this->assertArrayHasKey('button', $form,
      "We should have a submit button.");
    $this->assertArrayHasKey('#disabled', $form['button'],
      "The submit button should have the disabled key set.");
    $this->assertTrue($form['button']['#disabled'],
      "The submit button should BE DISABLED when the form is built with the form state disable_TripalImporter_submit set to TRUE.");

  }
}
