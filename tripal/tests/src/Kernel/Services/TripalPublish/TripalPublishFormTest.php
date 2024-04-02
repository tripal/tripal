<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Form\FormState;

/**
 * Tests the publish form.
 *
 * @group TripalPublish
 */
class TripalPublishFormTest extends TripalTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal'];


  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installConfig('system');

  }

  /**
   * Basic test for the default publish form with no storage backend chosen.
   */
  public function testTripalPublishFormBuild() {

    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm(
      'Drupal\tripal\Form\TripalEntityPublishForm',
    );

    // Ensure we are able to build the form.
    $this->assertIsArray($form,
      'We expect the form builder to return a form but it did not.');
    $this->assertEquals('content_bio_data_publish_form', $form['#form_id'],
      'We did not get the form id we expected.');

    // Check that our form has the basic details even with no storage backends available.
    $this->assertArrayHasKey('datastore', $form,
      "The form should have a storage backend element.");
    $this->assertEquals('select', $form['datastore']['#type'],
      "The storage backend should be a select element.");
    $this->assertArrayHasKey('bundle', $form,
      "The form should have a bundle element.");
    $this->assertEquals('select', $form['bundle']['#type'],
      "The bundle should be a select element.");
    $this->assertArrayHasKey('submit_button', $form,
      "The form should have a submit button.");
  }

  /**
   * Basic Submit without anything selected.
   */
  public function testTripalPublishFormSubmit() {

    // Setup the form_state.
    $form_state = new \Drupal\Core\Form\FormState();
    // $form_state->setValue('datastore', 'drupal_sql_storage');

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
    $this->assertCount(2, $form_validation_messages,
      "We should have two validation error but instead we have: " . implode(" AND ", $helpful_output));
    $this->assertArrayHasKey('datastore', $form_validation_messages,
      "There should be an error on the datastore element.");
    $this->assertStringContainsString('required', $form_validation_messages['datastore'],
      "The error message should indicate that the datastore element is required.");
    $this->assertArrayHasKey('bundle', $form_validation_messages,
      "There should be an error on the bundle element.");
    $this->assertStringContainsString('required', $form_validation_messages['bundle'],
      "The error message should indicate that the bundle element is required.");
    //   Looking for drupal message errors.
    $messages = \Drupal::messenger()->all();
    $this->assertIsArray($messages,
      "We expect to have status messages to the user on submission of the form.");
    $this->assertArrayHasKey('error', $messages,
      "There should be error messages from this form for the form validation.");
    $this->assertCount(2, $messages['error'],
      "Specifically there should be two error messages since there were two validations errors.");
  }
}
