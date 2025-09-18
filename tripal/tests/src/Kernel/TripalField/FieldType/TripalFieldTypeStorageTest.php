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
  }

  // 3. Try to get property type and value objects for keys which do not exist.
  // @todo implement this check.

  // 4. Confirm that this new method is equivalent to getValuesArray().
  // @todo implement this.

}
