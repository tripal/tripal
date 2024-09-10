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
class TripalFieldTypeCRUDTest extends TripalTestKernelBase {
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
    $senarios[] = [
      'type' => [
        'id' => 'tripal_boolean_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalBooleanTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_boolean_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalBooleanTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_boolean_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalBooleanTypeFormatter',
      ],
      'expectations' => [],
    ];

    // INTEGER
    $senarios[] = [
      'type' => [
        'id' => 'tripal_integer_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalIntegerTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_integer_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalIntegerTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_integer_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalIntegerTypeFormatter',
      ],
      'expectations' => [],
    ];

    // STRING
    $senarios[] = [
      'type' => [
        'id' => 'tripal_string_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalStringTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_string_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalStringTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_string_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalStringTypeFormatter',
      ],
      'expectations' => [],
    ];

    // TEXT
    $senarios[] = [
      'type' =>[
        'id' => 'tripal_text_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalTextTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_text_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalTextTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_text_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalTextTypeFormatter',
      ],
      'expectations' => [],
    ];

    return $senarios;
  }

  /**
   * This method tests that we can create an entity with this field.
   *
   * @dataProvider provideFieldsToTest
   */
  public function testCreateEntityWithField($field_type, $field_widget, $field_formatter, $expectations) {

    // Setup the field to be tested based on the data provider values.
    $field_name = $this->randomMachineName();
    $fieldConfig = $this->createFieldInstance(
      'tripal_entity',
      [
        'field_name' => $field_name,
        'field_type' => $field_type['id'],
        'formatter_id' => $field_formatter['id'],
      ]
    );

    // Create an entity with a specific value for this field
    // -- use the sample value generating to get a value for this field.
    $field_value = $field_type['class']::generateSampleValue($fieldConfig);
    $this->assertIsArray($field_value,
      "The ".$field_type['class']."::generateSampleValue() method for this field type did not return a valid value.");
    // -- create the entity with that value set
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->TripalEntityType->getID(),
      $field_name => $field_value,
    ]);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $field_type['id'] . " field.");
    // -- confirm the values in the created entity match those we set.
    foreach ($field_value as $property_key => $expected_property_value) {
      $this->assertEquals($expected_property_value, $entity->{$field_name}->{$property_key},
        "The value of the property $property_key was not what we expected for this field.");
    }
  }
}
