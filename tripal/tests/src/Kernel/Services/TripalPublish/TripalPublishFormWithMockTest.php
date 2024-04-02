<?php

namespace Drupal\Tests\tripal\Kernel;

use \Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Form\FormState;

/**
 * Tests the publish form with a mock datastore.
 *
 * @group TripalPublish
 */
class TripalPublishFormWithMockTest extends TripalTestKernelBase {
  protected $defaultTheme = 'stark';

  use UserCreationTrait;

  protected static $modules = ['system', 'user', 'file', 'tripal'];

  protected string $plugin_id;
  protected array $annotation;
  protected array $expected_form;
  protected string $bundle_name;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installConfig('system');

    // Needed as the publish form lists tripal entitiy types.
    $this->installEntitySchema('tripal_entity_type');

    // Setup for working with tripal jobs being submitted in test.
    $this->installEntitySchema('user');
    $this->installSchema('tripal', ['tripal_import', 'tripal_jobs']);
    $this->setUpCurrentUser();

    $container = \Drupal::getContainer();
    $logger = $container->get('tripal.logger');

    $this->plugin_id = 'mock_datastore_' . uniqid();
    $configuration = [];
    $this->annotation = [
      'id' => $this->plugin_id,
      'label' => t('Mock Datastore'),
      'description' => t('Just a mock datastore for testing the form'),
    ];
    $this->expected_form = [
      'random_element' => [
        '#type' => 'textfield',
        '#title' => 'Random Requirement',
        '#required' => TRUE,
      ],
    ];

    // Create a mock datastore.
    $mock_plugin = $this->createMock(\Drupal\tripal\TripalStorage\TripalStorageBase::class);
    $mock_plugin->method('publishForm')
      ->willReturn($this->expected_form);

    // Create a mock version of the plugin manager to return our mock plugin.
    $manager = $this->createMock(\Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager::class);
    $manager->method('getInstance')
      ->willReturn($mock_plugin);
    $manager->method('getDefinitions')
      ->willReturn([$this->plugin_id => $this->annotation]);
    $manager->method('datastoreExists')
      ->willReturn(TRUE);

    $container->set('tripal.storage', $manager);

    // We also need a bundle with this storage type...
    $this->bundle_name = 'fake_bundle_' . uniqid();
    $entityType = \Drupal\tripal\Entity\TripalEntityType::create([
      'id' => $this->bundle_name,
      'label' => 'FAKE Bundle For Testing',
      'termIdSpace' => 'FAKE',
      'termAccession' => 'Term',
      'help_text' => '',
      'category' => '',
      'title_format' => '',
      'url_format' => '',
      'hide_empty_field' => '',
      'ajax_field' => '',
    ]);
    $this->assertIsObject($entityType,
      "We were unable to create our Tripal Entity type during test setup.");
    $entityType->save();

  }

  /**
   * Basic test for the default publish form with no storage backend chosen.
   */
  public function testTripalPublishFormBuild() {

    // Setup the form_state.
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->setValue('datastore', $this->plugin_id);

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->buildForm(
      'Drupal\tripal\Form\TripalEntityPublishForm',
      $form_state
    );

    // Ensure we are able to build the form.
    $this->assertIsArray($form,
      'We expect the form builder to return a form but it did not.');
    $this->assertEquals('content_bio_data_publish_form', $form['#form_id'],
      'We did not get the form id we expected.');

    // Check that our form has the basic details even with no storage backends available.
    $this->assertArrayHasKey('random_element', $form['storage-options'],
      "The form should have the random element added by our mock datastore.");
    $this->assertEquals('textfield', $form['storage-options']['random_element']['#type'],
      "The random element added by our mock datastore should be a textfield.");
  }

  /**
   * Basic Submit without anything selected.
   */
  public function testTripalPublishFormSubmit() {

    // Setup the form_state.
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->setValue('datastore', $this->plugin_id);
    $form_state->setValue('bundle', $this->bundle_name);

    // Now try validation!
    \Drupal::formBuilder()->submitForm(
      'Drupal\tripal\Form\TripalEntityPublishForm',
      $form_state
    );
    // And do some basic checks to check for errors.
    $this->assertTrue($form_state->isValidationComplete(),
      "We expect the form state to have been updated to indicate that validation is complete.");
    //   Looking for form validation errors
    $form_validation_messages = $form_state->getErrors();
    $helpful_output = [];
    foreach ($form_validation_messages as $element => $markup) {
      $helpful_output[] = $element . " => " . (string) $markup;
    }
    $this->assertCount(1, $form_validation_messages,
      "We should have a single validation error but instead we have: " . implode(" AND ", $helpful_output));
    $this->assertArrayHasKey('random_element', $form_validation_messages,
      "There should be an error on the random element added by our mock datastore publish form.");

    // Now set the required value and submit it again.
    $form_state->setValue('random_element', 'Required so we need to set it to something!');

    \Drupal::formBuilder()->submitForm(
      'Drupal\tripal\Form\TripalEntityPublishForm',
      $form_state
    );
    // And do some basic checks to check for errors.
    $this->assertTrue($form_state->isValidationComplete(),
      "We expect the form state to have been updated to indicate that validation is complete.");
    //   Looking for form validation errors
    $form_validation_messages = $form_state->getErrors();
    $helpful_output = [];
    foreach ($form_validation_messages as $element => $markup) {
      $helpful_output[] = $element . " => " . (string) $markup;
    }
    $this->assertCount(0, $form_validation_messages,
      "We should not have any validation errors but instead we have: " . implode(" AND ", $helpful_output));
  }
}
