<?php

namespace Drupal\Tests\tripal\Kernel\TripalField;

use Drupal\tripal\Plugin\Field\FieldType\TripalStringTypeItem;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal\Traits\TripalFieldTestTrait;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\field\Entity\FieldConfig;
use Drupal\Core\Form\FormState;

/**
 * Tests the TripalFieldItemBase class indirectly.
 *
 * @group TripalField
 */
class TripalFieldTypeSettingsTest extends TripalTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'field', 'tripal'];

  use TripalFieldTestTrait;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $this->setupFieldTestEnvironment();
  }

  public function provideFieldsToTest() {
    $senarios =  [];

    // BOOLEAN
    $senarios['boolean'] = [
      'field_info' => [
        'field_type_id' => 'tripal_boolean_type',
        'field_type_class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalBooleanTypeItem',
        'widget_id' => 'default_tripal_boolean_type_widget',
        'formatter_id' => 'default_tripal_boolean_type_formatter',
      ],
      'expectations' => [
        'form_elements' => [],
      ],
    ];

    // INTEGER
    $senarios['integer'] = [
      'field_info' => [
        'field_type_id' => 'tripal_integer_type',
        'field_type_class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalIntegerTypeItem',
        'widget_id' => 'default_tripal_integer_type_widget',
        'formatter_id' => 'default_tripal_integer_type_formatter',
      ],
      'expectations' => [
        'form_elements' => [],
      ],
    ];

    // STRING
    $senarios['string'] = [
      'field_info' => [
        'field_type_id' => 'tripal_string_type',
        'field_type_class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalStringTypeItem',
        'widget_id' => 'default_tripal_string_type_widget',
        'formatter_id' => 'default_tripal_string_type_formatter',
      ],
      'expectations' => [
        'form_elements' => [
          'max_length' => [
            '#type' => 'number',
            '#default_value' => 255,
            '#required' => TRUE,
            '#min' => 1,
          ],
        ],
      ],
    ];

    // TEXT
    $senarios['text'] = [
      'field_info' =>[
        'field_type_id' => 'tripal_text_type',
        'field_type_class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalTextTypeItem',
        'widget_id' => 'default_tripal_text_type_widget',
        'formatter_id' => 'default_tripal_text_type_formatter',
      ],
      'expectations' => [
        'form_elements' => [],
      ],
    ];

    return $senarios;
  }

  /**
   * This method tests that we can build the storage settings subform.
   *
   * @dataProvider provideFieldsToTest
   */
  public function testStorageSettingsFormBuild($field_info, $expectations) {

    // Setup the field to be tested based on the data provider values.
    $field_name = $this->randomMachineName();
    $fieldConfig = $this->createFieldInstance(
      'tripal_entity',
      [
        'field_name' => $field_name,
        'field_type' => $field_info['field_type_id'],
        'formatter_id' => $field_info['formatter_id'],
      ]
    );

    // Build the form using the Drupal form builder.
    $formBuilder = \Drupal\field_ui\Form\FieldStorageConfigEditForm::create($this->container);
    $formBuilder->setEntity($this->fieldStorage);
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->set('field_config', $fieldConfig);
    $form_state->set('entity_type_id', 'tripal_entity');
    $form_state->set('bundle', $this->TripalEntityType->getID());
    $form = $formBuilder->form([], $form_state);
    $this->assertIsArray(
      $form,
      'We were not able to build the field storage settings form.'
    );
    // All TripalField Storage Settings forms should have:
    // - Tripal Storage Plugin ID
    $this->assertArrayHasKey('storage_plugin_id', $form['settings'],
      "All Tripal field storage settings forms should have a element for the Tripal Storage Plugin ID");
    // - Storage Settings Summary
    $this->assertArrayHasKey(
      'settings_fs',
      $form['settings'],
      "All Tripal field storage settings forms should have a element summarizing the Field Storage Settings"
    );

    // For Tripal Fields, the storage plugin id should be Drupal Storage.
    $this->assertEquals(
      'drupal_sql_storage',
      $form['settings']['storage_plugin_id']['#default_value'],
      "Tripal fields should have their Tripal Storage Plugin set to Drupal Storage."
    );
    $this->assertTrue(
      $form['settings']['storage_plugin_id']['#required'],
      "The storage plugin id should always be required for Tripal Fields."
    );
    $this->assertTrue(
      $form['settings']['storage_plugin_id']['#disabled'],
      "The storage plugin id should always be disabled for Tripal fields."
    );

    // Now lets check for the form elements specific to this field.
    foreach ($expectations['form_elements'] as $element_key => $element_details) {
      $this->assertArrayHasKey(
        $element_key,
        $form['settings'],
        "The " . $field_info['field_type_class'] . "::storageSettingsForm() expected form element is not present."
      );
      foreach ($element_details as $key => $expected_value) {
        $element_identifier = $element_key . '[' . $key . ']';
        $this->assertArrayHasKey(
          $key,
          $form['settings'][$element_key],
          "The " . $field_info['field_type_class'] . "::storageSettingsForm() form element $element_identifier element is not present."
        );
        $this->assertEquals($expected_value, $form['settings'][$element_key][$key],
          "The " . $field_info['field_type_class'] . "::storageSettingsForm() form element $element_identifier does not have the expected value.");
      }
    }
  }

  /**
   * Provides a collection of settings to test across all our fields.
   *
   * NOTE: Uses the provideFieldsToTest data provider as the source of the
   * fields to be tested and then combines them with settings sets.
   *
   * @return array
   *   An array of field by settings combinations to ensure we fully test the
   *   settings forms.
   */
  public function provideFieldsWithSettings() {

    $fields = $this->provideFieldsToTest();
    $senarios = [];

    // @todo loop through all fields with general settings.

    // FIELD SPECIFIC SETTINGS
    // -- String Field
    $field = $fields['string']['field_info'];

    $senarios['max_length_empty'] = [
      'field_info' => $field,
      'senario' => [
        'storage_settings' => [
          'submitted_values' => [
            'max_length' => 0,
          ],
          'expectations' => [

          ],
        ],
      ],
    ];

    return $senarios;
  }

  /**
   * This method tests that we can build the storage settings subform.
   *
   * @dataProvider provideFieldsWithSettings
   */
  public function testStorageSettingsFormValidate($field_info, $senario) {

    // We only care about the storage settings form
    // so let's limit the senario to that.
    $senario = $senario['storage_settings'];

    // Setup the field to be tested based on the data provider values.
    $field_name = $this->randomMachineName();
    $fieldConfig = $this->createFieldInstance(
      'tripal_entity',
      [
        'field_name' => $field_name,
        'field_type' => $field_info['field_type_id'],
        'formatter_id' => $field_info['formatter_id'],
      ]
    );

    // Build the form using the Drupal form builder.
    $formBuilder = \Drupal\field_ui\Form\FieldStorageConfigEditForm::create($this->container);
    $formBuilder->setEntity($this->fieldStorage);
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->set('field_config', $fieldConfig);
    $form_state->set('entity_type_id', 'tripal_entity');
    $form_state->set('bundle', $this->TripalEntityType->getID());
    $form = $formBuilder->form([], $form_state);
    $this->assertIsArray(
      $form,
      'We were not able to build the field storage settings form.'
    );

    // Now set our settings in the form state as though the admin had.
    foreach($senario['submitted_values'] as $element_key => $value) {
      $form_state->set($element_key, $value);
    }
    // And then submit for validation.
    $formBuilder->submitForm($form, $form_state);
    /*
    $this->assertTrue(
      $form_state->isValidationComplete(),
      "We expect the form state to have been updated to indicate that validation is complete."
    );
    */

    // Looking for form validation errors
    $form_validation_messages = $form_state->getErrors();
    print_r($form_validation_messages);

    //   Looking for drupal message errors.
    $messages = \Drupal::messenger()->all();
    print_r($messages);

  }
}
