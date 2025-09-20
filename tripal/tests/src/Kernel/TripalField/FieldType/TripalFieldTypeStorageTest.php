<?php

namespace Drupal\Tests\tripal\Kernel\TripalField;

use Drupal\tripal\Entity\TripalEntity;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal\Traits\TripalEntityFieldTestTrait;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests the TripalFieldItemBase class local caching of TripalStorage.
 */
#[Group('tripal-entity')]
#[Group('tripal-field')]
class TripalFieldTypeStorageTest extends TripalTestKernelBase {

  use TripalEntityFieldTestTrait;

  /**
   * The theme to use when testing.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * The modules to install when testing.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'field', 'datetime', 'tripal'];


  /**
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/TripalFieldTypeStorageTest-TestInfo.yml';

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - bundle: an array defining the tripal entity type to create.
   *   - fields: a list of fields to be attached the above bundle.
   */
  protected array $system_under_test;

  /**
   * The TripalEntityType id of the bundle being used in this test.
   *
   * @var string
   */
  protected string $bundle_name;

  /**
   * Describes the scenarios to test.
   *
   * This will be used in combination with the data provider. It can't be
   * accessed directly in the dataProvider due to the way that PHPUnit is
   * setup.
   *
   * @var array
   *  A list of scenarios where each one has the following keys:
   *  - label: A human-readable label for the scenario to be used in assert
   *    messages.
   *  - description: A description of the scenario and what you are wanting to
   *    test. This will not be used in the test but is rather there to help
   *    people reading the YAML file and to make it easier to maintain.
   *  - create: An array of the values to be provided when creating a
   *    TripalEntity. There should be a key matching the name of each field in
   *    the system-under-test and it's value should be an array containing all
   *    the property types for that field mapped to a value.
   */
  protected array $scenarios;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // First retrieve info from the YAML file for this particular test.
    [$this->system_under_test, $this->scenarios] = $this->getTestInfoFromYaml($this->yaml_info_file);
    $this->bundle_name = $this->system_under_test['bundle']['id'];

    $this->setupEntityFieldTestEnvironment($this->system_under_test);
  }

  /**
   * This method tests that we can setup a field for use with TripalStorage.
   */
  public function testTripalFieldLocalCaching() {

    $field_value_scenario = $this->scenarios[0];

    // 1. Create the entity with the values set.
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
    ] + $field_value_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content.");

    // 2. Check that we can get the property types + values that we expect.
    foreach ($this->system_under_test['fields'] as $fieldyml) {
      $field_name = $fieldyml['name'];
      $field_items = $entity->get($field_name);

      $field_value_expectations = $field_value_scenario['create']['expected'][$field_name];
      $expected_num_values = count($field_value_expectations);
      $this->assertCount($expected_num_values, $field_items, "We did not get the expected number of values for $field_name.");

      foreach ($field_items as $delta => $field_item) {
        $expected_values = $field_value_expectations[$delta];
        // All types.
        $prop_types = $field_item->getTripalStoragePropertyTypes();
        $this->assertCount(1, $prop_types, "We did not get the expected number of property TYPES when trying to retrieve them ALL.");

        // Value type.
        $value_prop_type = $field_item->getTripalStoragePropertyType('value');
        $this->assertEquals('value', $value_prop_type->getKey(), "We were not able to get the property TYPE object for the $field_name value property specifically.");

        // All Values.
        $prop_values = $field_item->getTripalStoragePropertyValues();
        $this->assertCount(1, $prop_values, "We did not get the expected number of property VALUES when trying to retrieve them ALL.");

        // Value value.
        $value_prop_value = $field_item->getTripalStoragePropertyValue('value');
        $this->assertEquals('value', $value_prop_value->getKey(), "We were not able to get the property VALUE object for the $field_name value property specifically.");
        $this->assertEquals($expected_values['value'], $value_prop_value->getValue(), "We did not get the value we expected from the $field_name 'value' property value object.");
      }
    }

    // 3. Try to get property type + value objects for keys which do not exist.
    $field_item = $entity->get('gemstone_name')[0];
    // Property Type.
    $exception_caught = FALSE;
    $exception_msg = uniqid();
    try {
      $field_item->getTripalStoragePropertyType('NON-EXISTING-PROPERTY');
    }
    catch (\Exception $e) {
      $exception_caught = TRUE;
      $exception_msg = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should have gotten an exception when trying to access a property type that doesn't exist.");
    $this->assertEquals(
    "Cannot access the 'NON-EXISTING-PROPERTY' property type for 'gemstone_name' field as it is not defined by its Drupal\\tripal\Plugin\Field\FieldType\TripalStringTypeItem::tripalTypes method.",
    $exception_msg,
    "We did not get the exception message we expected.",
    );
    // Property Value.
    $exception_caught = FALSE;
    $exception_msg = uniqid();
    try {
      $field_item->getTripalStoragePropertyValue('NON-EXISTING-PROPERTY');
    }
    catch (\Exception $e) {
      $exception_caught = TRUE;
      $exception_msg = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should have gotten an exception when trying to access a property value that doesn't exist.");
    $this->assertEquals(
    "Cannot access the 'NON-EXISTING-PROPERTY' property value for 'gemstone_name' field.",
    $exception_msg,
    "We did not get the exception message we expected.",
    );
  }

  /**
   * This method tests that we can sync field values with property values.
   */
  public function testTripalFieldPropertyFieldSync() {

    $field_value_scenario = $this->scenarios[0];
    $fields_under_test = array_keys($field_value_scenario['create']['user_input']);

    // 1. Create the entity with the values set.
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
    ] + $field_value_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content.");
    // - Ensure the property values are initialized.
    foreach ($fields_under_test as $field_name) {
      foreach ($entity->get($field_name) as $delta => $field_item) {
        $field_item->getTripalStoragePropertyValues();
      }
    }

    // 2. Test that property values are correctly updated to match field values.
    // - Change some field values to no longer match the property values.
    $new_field_values = $field_value_scenario['syncValues']['user_input'];
    $original_values = $field_value_scenario['create']['expected'];
    $expected_values = $field_value_scenario['syncValues']['expected'];
    foreach ($new_field_values as $field_name => $new_vals) {
      $entity->set($field_name, $new_vals);
    }

    foreach ($fields_under_test as $field_name) {
      foreach ($entity->get($field_name) as $delta => $field_item) {

        // Confirm field value matches what we just set it to.
        $this->assertEquals(
          $new_field_values[$field_name][$delta],
          $field_item->getValue(),
          "The field values for $field_name did not match what we just set them to."
        );

        // Confirm the property value still matches its original value.
        $this->assertEquals(
          $original_values[$field_name][$delta],
          $field_item->exportTripalStoragePropertyValues(),
          "We expected the property values for $field_name not to have changed when we set the field values."
        );
        $this->assertNotEquals(
          $new_field_values[$field_name][$delta],
          $field_item->exportTripalStoragePropertyValues(),
          "We expect the new values to not match the original values for the purpose of this test."
        );

        // - Now sync the property values and confirm they have been updated.
        $field_item->syncTripalStoragePropertyValues();
        $this->assertEquals(
          $expected_values[$field_name][$delta],
          $field_item->exportTripalStoragePropertyValues(),
          "The updated property values after $field_name syncTripalStoragePropertyValues() did not match what we expected."
        );
      }
    }

    // 3. Test that field values are correctly updated to match property values.
    // - Refresh the created entity back to the defaults.
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
    ] + $field_value_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to refresh the entity back to a clean slate.");
    // - Change some property values to no longer match the field values.
    $new_property_values = $field_value_scenario['syncValues']['user_input'];
    $original_values = $field_value_scenario['create']['expected'];
    $expected_values = $field_value_scenario['syncValues']['expected'];
    foreach ($new_property_values as $field_name => $new_vals) {
      foreach ($new_vals as $delta => $new_prop_vals) {
        $field_item = $entity->get($field_name)->get($delta);
        $field_item->updateTripalStoragePropertyValues($new_prop_vals);
      }
    }

    foreach ($fields_under_test as $field_name) {
      foreach ($entity->get($field_name) as $delta => $field_item) {

        // Confirm field value still matches its original value.
        $this->assertEquals(
          $original_values[$field_name][$delta],
          $field_item->getValue(),
          "We expected the field values for $field_name not to have changed when we set the property values."
        );
        // Confirm the property value have been updated.
        $this->assertEquals(
          $new_property_values[$field_name][$delta],
          $field_item->exportTripalStoragePropertyValues(),
          "The property values for $field_name were not updated as we expected."
        );

        // - Now sync the field values.
        $field_item->syncFieldValuesWithTripalStorage();
        $this->assertEquals(
          $expected_values[$field_name][$delta],
          $field_item->getValue(),
          "The updated field values after $field_name syncFieldValuesWithTripalStorage() did not match what we expected."
        );
      }
    }

    // 4. Test that field values are reset to defaults of the correct type.
    // Note: We will only do this for the first field item for each field.
    $expected_values = $field_value_scenario['clearValues']['expected'];
    foreach ($fields_under_test as $field_name) {
      $field_item = $entity->get($field_name)->first();

      $field_item->clearFieldValuesForTripalStorage();
      $field_values = $field_item->getValue();
      foreach ($field_values as $property_key => $field_val) {
        $expected_prop_value = $expected_values[$field_name][0][$property_key];
        $expected_propval_type = gettype($expected_prop_value);

        $this->assertEquals(
          $expected_prop_value,
          $field_val,
          "The field value for $field_name [0] [$property_key] was not what we expected (i.e. was not the correct default value for the property value)."
        );

        $this->assertEquals(
          $expected_propval_type,
          gettype($field_val),
          "The primitive type of the cleared field value for $field_name [0] [$property_key] was not what we expected."
        );
      }
    }
  }

}
