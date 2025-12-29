<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\Core\Form\FormState;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the tripal entity settings form.
 *
 * @group Tripal
 * @group TripalEntity
 */
#[Group('tripal-content')]
#[RunTestsInSeparateProcesses]
class TripalEntitySettingsFormTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'file', 'tripal'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installConfig('system');
    $this->installSchema('file', ['file_usage']);
  }

  /**
   * Tests the Tripal Entity Settings form.
   *
   * @return void
   *   No return value.
   */
  public function testTripalEntitySettingsForm() {

    $form_route = 'Drupal\tripal\Form\TripalEntitySettingsForm';
    // Build the form using the Drupal form builder.
    $form = \Drupal::formBuilder()->getForm($form_route);

    // Ensure we are able to build the form.
    $this->assertIsArray($form,
      'We expect the form builder to return a form but it did not.');
    $this->assertEquals('tripal_entity_settings_form', $form['#form_id'],
      'We did not get the form id we expected.');

    // Confirm we have a "checkbox" element for "Cache Backend Storage field
    // values in Drupal".
    $settings = \Drupal::config('tripal.settings');
    $element_type = $form['default_cache_backend_field_values']['#type'] ?? NULL;
    $this->assertEquals('checkbox', $element_type,
      'Missing a checkbox element in the form');

    // Confirm we have a "textfield" element for "HTML tags allowed in page
    // titles".
    $element_type = $form['allowed_title_tags']['#type'] ?? NULL;
    $this->assertEquals('textfield', $element_type,
      'Missing a textfield element in the form');

    // Confirm we have a "number" element for "Maximum records for a select".
    $element_type = $form['widget_global_select_limit']['#type'] ?? NULL;
    $this->assertEquals('number', $element_type,
      'Missing a number element in the form');
    $element_value = $form['widget_global_select_limit']['#default_value'] ?? NULL;
    $expected_value = $settings->get('tripal_entity_type.widget_global_select_limit') ?? 50;
    $this->assertEquals($expected_value, $element_value,
      'The default value for widget_global_select_limit is not the expected value');

    // Confirm we have a "number" and "checkbox" element for "Publishing
    // Options".
    $element_type = $form['publish']['publish_global_max_delta']['#type'] ?? NULL;
    $this->assertEquals('number', $element_type,
      'Missing a number element in the form');
    $element_value = $form['publish']['publish_global_max_delta']['#default_value'] ?? NULL;
    $expected_value = $settings->get('tripal_entity_type.publish_global_max_delta') ?? 100;
    $this->assertEquals($expected_value, $element_value,
      'The default value for publish_global_max_delta is not the expected value');
    $element_type = $form['publish']['publish_global_max_delta_inhibit']['#type'] ?? NULL;
    $this->assertEquals('checkbox', $element_type,
      'Missing a checkbox element in the form');

    // Confirm we have a submit button.
    $element_type = $form['submit']['#type'] ?? NULL;
    $this->assertEquals('submit', $element_type,
      'Missing a submit element in the form');

    // Try submitting the form.
    $form_state = new FormState();
    \Drupal::formBuilder()->submitForm($form_route, $form_state);
    $this->assertTrue($form_state->isValidationComplete(),
      'We expected the form state to have been updated to indicate that validation is complete.');

    // Expect no form validation errors.
    $form_validation_messages = $form_state->getErrors();
    $this->assertCount(0, $form_validation_messages,
      'We did not expect validation errors with no input.');

    // Change values and submit again.
    $form_state->setValue('allowed_title_tags', 'em i u');
    $form_state->setValue('default_cache_backend_field_values', 1);
    $form_state->setValue('widget_global_select_limit', 42);
    $form_state->setValue('publish_global_max_delta', 150);
    $form_state->setValue('publish_global_max_delta_inhibit', 1);
    \Drupal::formBuilder()->submitForm($form_route, $form_state);
    $this->assertTrue($form_state->isValidationComplete(),
      'We expected the form state to have been updated to indicate that validation is complete.');

    // Expect no form validation errors.
    $form_validation_messages = $form_state->getErrors();
    $this->assertCount(0, $form_validation_messages,
      'We did not expect validation errors with no input.');

    // Validate saved settings.
    $settings = \Drupal::config('tripal.settings');
    $this->assertEquals('em i u', $settings->get('tripal_entity_type.allowed_title_tags'),
      'Did not update allowed title tags setting');
    $this->assertEquals(1, $settings->get('tripal_entity_type.default_cache_backend_field_values'),
      'Did not update cache backend setting');
    $this->assertEquals(42, $settings->get('tripal_entity_type.widget_global_select_limit'),
      'Did not update widget global select limit setting');
    $this->assertEquals(150, $settings->get('tripal_entity_type.publish_global_max_delta'),
      'Did not update publish global max delta setting');
    $this->assertEquals(1, $settings->get('tripal_entity_type.publish_global_max_delta_inhibit'),
      'Did not update publish global max delta inhibit setting');
  }

}
