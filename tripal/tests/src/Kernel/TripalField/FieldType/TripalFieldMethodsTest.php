<?php

namespace Drupal\Tests\tripal\Kernel\TripalField\FieldType;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal\Traits\TripalEntityFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the methods of the TripalFieldBase class.
 */
#[Group('tripal-content')]
#[Group('tripal-field')]
#[RunTestsInSeparateProcesses]
class TripalFieldMethodsTest extends TripalTestKernelBase {

  use TripalEntityFieldTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'field', 'datetime', 'tripal'];

  /**
   * The test drupal connection. It is also set in the container.
   *
   * @var object
   */
  protected object $drupal_connection;

  /**
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/TripalFieldMethodsTest-TestInfo.yml';

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - bundle: an array defining the tripal entity type to create.
   *   - fields: a list of fields to be attached the above bundle.
   *   - field_index: a mapping where the key is the field name and the value
   *     is it's position in the fields list. This is populated when setting
   *     up the system under test.
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
   *  - edit: An array of the values to be provided when updating an existing
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

    // The Drupal connection will be created in the parent. This is used
    // when checking the Drupal field tables.
    $this->drupal_connection = $this->container->get('database');

    // First retrieve info from the YAML file for this particular test.
    [$this->system_under_test, $this->scenarios] = $this->getTestInfoFromYaml($this->yaml_info_file);
    $this->bundle_name = $this->system_under_test['bundle']['id'];

    $this->setupEntityFieldTestEnvironment($this->system_under_test);
    // And populate the indices.
    foreach ($this->system_under_test['fields'] as $field_index => $deets) {
      $field_name = $deets['name'];
      $this->system_under_test['field_index'][$field_name] = $field_index;
    }
  }

  /**
   * Data Provider: works with the YAML to provide scenarios for testing.
   *
   * @return array
   *   List of scenarios to test where each one matches a key and label in the
   *   associated YAML scenarios.
   */
  public static function provideScenarios() {
    $scenarios = [];

    $scenarios[] = [
      0,
      'Value for all fields',
    ];

    return $scenarios;
  }

  /**
   * Tests various TripalFieldBase methods.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testTripalFieldBaseMethods(int $current_scenario_key, string $current_scenario_label) {
    $current_scenario = $this->scenarios[$current_scenario_key];
    $this->assertEquals($current_scenario_label, $current_scenario['label'], "We may not have retrieved the expected scenario as the labels did not match.");

    // 1. Create the entity with that value set.
    $submitted_title = $this->randomString();
    $entity = TripalEntity::create([
      'title' => $submitted_title,
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");
    $status = $entity->save();
    $this->assertEquals(SAVED_NEW, $status, "We expected to have saved a new entity for our " . $current_scenario['label'] . " scenario.");

    // 2. Check a number of TripalField methods for their expected values.
    foreach ($entity->getTripalFieldItems() as $field_name => $items) {
      $field_index = $this->system_under_test['field_index'][$field_name];
      $field = $items->first();
      $field_defn = $field->getFieldDefinition();
      $expected = $current_scenario['create']['expected'][$field_name];

      // Test mainDisplayPropertyName().
      $main_property = $field->mainDisplayPropertyName();
      $this->assertEquals($expected['main_property'], $main_property, "The main display property for field '$field_name' did not match what we expected.");

      // Test defaultFieldSettings().
      $default_settings = $field->defaultFieldSettings();
      $this->assertIsArray($default_settings, "The defaultFieldSettings() method for field '$field_name' did not return an array as expected.");
      foreach (['termIdSpace', 'termAccession', 'debug'] as $setting_key) {
        $this->assertArrayHasKey($setting_key, $default_settings, "The defaultFieldSettings() method for field '$field_name' did not return the expected '$setting_key' setting.");
      }

      // Test getSettings().
      $settings = $field_defn->getSettings();
      $this->assertIsArray($settings, "The getSettings() method for field '$field_name' did not return an array as expected.");
      foreach (['termIdSpace', 'termAccession'] as $setting_key) {
        $this->assertArrayHasKey($setting_key, $settings, "The getSettings() method for field '$field_name' did not return the expected '$setting_key' setting.");
        $this->assertEquals($this->system_under_test['fields'][$field_index][$setting_key], $settings[$setting_key], "The getSetting('$setting_key') method for field '$field_name' did not return the expected value.");
      }

      // Test defaultStorageSettings().
      $default_settings = $field->defaultStorageSettings();
      $this->assertIsArray($default_settings, "The defaultStorageSettings() method for field '$field_name' did not return an array as expected.");
      foreach (['termIdSpace', 'termAccession', 'storage_plugin_id', 'storage_plugin_settings'] as $setting_key) {
        $this->assertArrayHasKey($setting_key, $default_settings, "The defaultStorageSettings() method for field '$field_name' did not return the expected '$setting_key' setting.");
      }

    }

  }

}
