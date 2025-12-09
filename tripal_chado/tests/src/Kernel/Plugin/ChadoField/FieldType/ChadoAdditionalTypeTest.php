<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoField\FieldType;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the Field Types in the associated Test Info YAML.
 *
 * Specifically focused on create + update actions performed on the entity
 * directly. Both TripalEntity, ChadoStorage and the field will be covered.
 *
 * @group TripalField
 * @group ChadoField
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[RunTestsInSeparateProcesses]
class ChadoAdditionalTypeTest extends ChadoTestKernelBase {

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
  protected string $yaml_info_file = __DIR__ . '/ChadoAdditionalType-organism-TestInfo.yml';

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - chado_version: the version of chado to test under.
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
   *  - descrition: A description of the scenario and what you are wanting to
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

    // Create the test Chado installation we will be using.
    if (!array_key_exists('chado_version', $this->system_under_test)) {
      $this->system_under_test['chado_version'] = '1.3';
    }
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO,
      $this->system_under_test['chado_version']
    );

    // Next setup the environment according to the system under test.
    $this->setupChadoEntityFieldTestEnvironment($this->system_under_test);
    $this->installSchema('tripal_chado', ['tripal_custom_tables', 'tripal_mviews']);
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
      "Default Values Only",
    ];

    return $scenarios;
  }

  /**
   * Tests the field through TripalEntity->save().
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testFieldTypeCrud(int $current_scenario_key, string $current_scenario_label) {

    // Retrieve the correct scenario.
    $current_scenario = $this->getYamlScenario($current_scenario_key, $current_scenario_label);

    // 1. Create the entity with that value set.
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");
    $status = $entity->save();
    $this->assertEquals(SAVED_NEW, $status, "We expected to have saved a new entity for our " . $current_scenario['label'] . " scenario.");

    // @debug print_r($entity->toArray());
    // 2. Load the entity we just created so we can check the values.
    $created_entity = TripalEntity::load($entity->id());
    $this->assertFieldValuesMatch($current_scenario['create']['expected'], $created_entity, $current_scenario['label'] . ' CREATE ');

    // 3. Make changes and then save again.
    foreach ($current_scenario['edit']['user_input'] as $field_name => $new_values) {
      $created_entity->set($field_name, $new_values);
    }
    // @debug print_r($created_entity->toArray());
    $status = $created_entity->save();
    $this->assertEquals(SAVED_UPDATED, $status, "We expected to have updated the existing entity for our " . $current_scenario['label'] . " scenario.");

    // 4. Load the entity we just updated so we can check the values.
    $updated_entity = TripalEntity::load($created_entity->id());
    // @debug print_r($updated_entity->toArray());
    $this->assertFieldValuesMatch($current_scenario['edit']['expected'], $updated_entity, $current_scenario['label'] . ' EDIT ');
  }

  /**
   * Tests non-CRUD methods for the field types.
   *
   * Specifically,
   *  - generateSampleValue()
   *  - isCompatible()
   *  - getConstraints()
   */
  public function testFieldTypeHelperMethods() {
    // Retrieve the first scenario.
    $current_scenario = $this->getYamlScenario(0, 'Default Values Only');

    // Create the entity with that value set.
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");
    $status = $entity->save();
    $this->assertEquals(SAVED_NEW, $status, "We expected to have saved a new entity for our " . $current_scenario['label'] . " scenario.");

    // Create another content type with a chado_table not compatible with
    // any fields.
    $sad_bundle = $this->createTripalContentType();
    $this->chado_connection->query('CREATE TABLE IF NOT EXISTS {1:emptytable} (amount real)');
    $sad_bundle->setThirdPartySetting('tripal', 'chado_base_table', 'emptytable');
    $sad_bundle->save();

    // For each of the fields in the system under test...
    foreach ($this->system_under_test['fields'] as $field_info) {
      $field_name = $field_info['name'];
      $field_class = $field_info['type_class'];
      $field_defn = $this->fieldConfig[$field_name];
      $field_item = $entity->get($field_name)->first();

      // Check that we can generate a sample value.
      $generated_value = $field_class::generateSampleValue($field_defn);
      $this->assertIsArray($generated_value, "We expected $field_name::generateSampleValue() to generate an array.");
      $this->assertArrayHasKey('record_id', $generated_value[0], "We expected the $field_name generated value to have a record_id.");

      // Check that we can confirm it is compatible with the current bundle.
      $compatible = $field_item->isCompatible($entity->getBundle());
      $this->assertTrue($compatible, "We expect the $field_name field to be compatible with the current bundle (i.e. " . $this->bundle_name . ").");

      // Check that it is not compatible with a bundle that has no compatible
      // columns for any field.
      $compatible = $field_item->isCompatible($sad_bundle);
      $this->assertFalse($compatible, "We don't expect the $field_name field to be compatible with a bundle that has no chado table set.");

      // Check that we can retrieve constraints.
      $constraints = $field_item->getConstraints();
      $this->assertIsArray($constraints, "We expected to at least be given an empty array when trying to retrieve the constraints for $field_name.");
    }
  }

}
