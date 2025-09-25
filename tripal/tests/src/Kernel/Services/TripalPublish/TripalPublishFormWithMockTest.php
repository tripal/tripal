<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Core\Form\FormState;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager;
use Drupal\tripal\TripalStorage\TripalStorageBase;
use PHPUnit\Framework\Attributes\Group;


/**
 * Tests the publish form with a mock datastore.
 *
 * @group TripalPublish
 */
#[Group('TripalPublish')]
class TripalPublishFormWithMockTest extends TripalTestKernelBase {

  use UserCreationTrait;
  use StringTranslationTrait;

  /**
   * The name of the default theme.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * List of modules used for testing.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'file', 'tripal'];

  /**
   * The name of the plugin.
   *
   * @var string
   */
  protected string $plugin_id;

  /**
   * An annotation specification.
   *
   * @var array
   */
  protected array $annotation;

  /**
   * The expected render array for the form.
   *
   * @var array
   */
  protected array $expected_form;

  /**
   * The name of the bundle being tested.
   *
   * @var string
   */
  protected string $bundle_name;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installConfig('system');

    // Needed as the publish form lists tripal entity types.
    $this->installEntitySchema('tripal_entity_type');

    // Setup for working with tripal jobs being submitted in test.
    $this->installEntitySchema('user');
    $this->installSchema('file', ['file_usage']);
    $this->installSchema('tripal', ['tripal_import', 'tripal_jobs']);
    $this->setUpCurrentUser();

    $container = \Drupal::getContainer();

    $this->plugin_id = 'mock_datastore_' . uniqid();
    $this->annotation = [
      'id' => $this->plugin_id,
      'label' => $this->t('Mock Datastore'),
      'description' => $this->t('Just a mock datastore for testing the form'),
    ];
    $this->expected_form = [
      'random_element' => [
        '#type' => 'textfield',
        '#title' => 'Random Requirement',
        '#required' => TRUE,
      ],
    ];

    // Create a mock datastore.
    $mock_plugin = $this->createMock(TripalStorageBase::class);
    $mock_plugin->method('publishForm')
      ->willReturn($this->expected_form);

    // Create a mock version of the plugin manager to return our mock plugin.
    $manager = $this->createMock(TripalStorageManager::class);
    $manager->method('getInstance')
      ->willReturn($mock_plugin);
    $manager->method('getDefinitions')
      ->willReturn([$this->plugin_id => $this->annotation]);
    $manager->method('datastoreExists')
      ->willReturn(TRUE);

    $container->set('tripal.storage', $manager);

    // We also need a bundle with this storage type...
    $this->bundle_name = 'fake_bundle_' . uniqid();
    $entityType = TripalEntityType::create([
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
    $form_state = new FormState();
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

    // Check that our form has the basic details even with no storage
    // backends available.
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
    $form_state = new FormState();
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
    // Looking for form validation errors.
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
    // Looking for form validation errors.
    $form_validation_messages = $form_state->getErrors();
    $helpful_output = [];
    foreach ($form_validation_messages as $element => $markup) {
      $helpful_output[] = $element . " => " . (string) $markup;
    }
    $this->assertCount(0, $form_validation_messages,
      "We should not have any validation errors but instead we have: " . implode(" AND ", $helpful_output));
  }

}
