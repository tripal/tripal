<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the static methods of Chado Fields.
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[RunTestsInSeparateProcesses]
class FieldStaticMethodTest extends ChadoTestKernelBase {

  use ChadoFieldTestTrait;

  /**
   * The theme to use when rendering content in the test environment.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * The modules that this test depends on.
   *
   * NOTE: since this is a kernel test, these modules are not being installed
   * but are available to be installed.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'field', 'datetime', 'tripal', 'tripal_chado'];

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected object $chado_connection;

  /**
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/FieldStaticMethodTest-TestInfo.yml';

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - chado_version: the version of chado to test under.
   */
  protected array $system_under_test;

  /**
   * Describes the scenarios to test.
   *
   * This will be used in combination with the data provider. It can't be
   * accessed directly in the dataProvider due to the way that PHPUnit is
   * setup.
   *
   * @var array
   *   A list of scenarios where each one has the following keys:
   *   - id: the machine name of the field to be tested.
   *   - class: the class defining the field to be tested.
   */
  protected array $scenarios;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // First retrieve info from the YAML file for this particular test.
    [$this->system_under_test, $this->scenarios] = $this->getTestInfoFromYaml($this->yaml_info_file);

    // Get Chado in place.
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO
    );

    // We need to setup the test environment for creating fields.
    // We don't pass in anything so no bundles/fields will be created.
    $this->setupChadoEntityFieldTestEnvironment();

    // Now create the fields.
    // createFieldInstance() will save all fields created to the fieldConfig.
    // property which is keyed by field name. The field names will be
    // generated and saved in each scenario as will the other values.
    $options = [
      'vocab_plugin_id' => 'chado_vocabulary',
      'idspace_plugin_id' => 'chado_id_space',
    ];
    foreach (array_keys($this->scenarios) as $scenario_key) {
      $this->createFieldInstance('feature', $this->scenarios[$scenario_key], $options);
    }

  }

  /**
   * Tests the static methods of the chado field types.
   */
  public function testStaticMethods() {

    foreach ($this->scenarios as $scenario) {
      $field_class = $scenario['field_class'];
      $field_id = $scenario['field_type'];
      $field_name = $scenario['field_name'];
      $field_defn = $this->fieldConfig[$field_name];

      $mainPropertyNameMSG = 'indicates the property required for this field not to be considered empty';
      $mainDisplayPropertyNameMSG = 'indicates the property used as the value for field-specific Tripal Tokens';

      // PROPERTY NAMES: Check that the static methods have been defined.
      $this->assertTrue(
        method_exists($field_class, 'mainPropertyName'),
        "$field_id should have ::mainPropertyName() method which $mainPropertyNameMSG."
      );
      $this->assertTrue(
        method_exists($field_class, 'mainDisplayPropertyName'),
        "$field_id should have ::mainDisplayPropertyName() method which $mainDisplayPropertyNameMSG."
      );

      // PROPERTY NAMES: Check that these methods do not return NULL.
      $main_property_name = $field_class::mainPropertyName();
      $this->assertNotNull($main_property_name, "$field_class::mainPropertyName() should $mainPropertyNameMSG.");
      $main_display_property_name = $field_class::mainDisplayPropertyName();
      $this->assertNotNull($main_display_property_name, "$field_class::mainDisplayPropertyName() should $mainDisplayPropertyNameMSG.");

      $storageSettingsMSG = 'defines the default settings for where this field stores its data in chado';
      $fieldSettingsMSG = 'defines the default settings for the field itself such as the term';

      // DEFAULT SETTINGS: Check that the static methods have been defined.
      $this->assertTrue(
        method_exists($field_class, 'defaultStorageSettings'),
        "$field_id should have ::defaultStorageSettings() method which $storageSettingsMSG."
      );
      $this->assertTrue(
        method_exists($field_class, 'defaultFieldSettings'),
        "$field_id should have ::defaultFieldSettings() method which $fieldSettingsMSG."
      );

      // DEFAULT SETTINGS: Check that these methods do not return NULL.
      // Storage Settings.
      $storage_settings = $field_class::defaultStorageSettings();
      $this->assertIsArray($storage_settings, "$field_class::defaultStorageSettings() should $storageSettingsMSG.");
      $this->assertArrayHasKey('storage_plugin_id', $storage_settings, "$field_class::defaultStorageSettings() should define the storage_plugin_id.");
      $this->assertEquals('chado_storage', $storage_settings['storage_plugin_id'], "$field_id doesn't have the expected storage backend despite being in the Chado Fields category.");
      $this->assertArrayHasKey('storage_plugin_settings', $storage_settings, "$field_class::defaultStorageSettings() should define the storage_plugin_settings.");
      $this->assertNotEmpty($storage_settings['storage_plugin_settings'], "$field_id did not define any storage_plugin_settings in $field_class::defaultStorageSettings.");
      // Field Settings.
      $field_settings = $field_class::defaultFieldSettings();
      $this->assertIsArray($field_settings, "$field_class::defaultFieldSettings() should $fieldSettingsMSG.");
      $this->assertArrayHasKey('termIdSpace', $field_settings, "$field_class::defaultFieldSettings() should define the termIdSpace.");
      $this->assertArrayHasKey('termAccession', $field_settings, "$field_class::defaultFieldSettings() should define the termAccession.");

      // Generate Sample Value.
      // Check that we can generate a sample value.
      $generated_value = $field_class::generateSampleValue($field_defn);
      $this->assertIsArray($generated_value, "We expected $field_name::generateSampleValue() to generate an array.");
      $this->assertArrayHasKey('record_id', $generated_value[0], "We expected the $field_name generated value to have a record_id.");
    }
  }

}
