<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the TripalEntity Class.
 *
 * @group TripalEntity
 * @group TripalTokenParser
 */
#[Group('tripal-content')]
#[Group('service-token-parser')]
#[RunTestsInSeparateProcesses]
class TripalEntityChadoFieldMethodTest extends ChadoTestKernelBase {

  use ChadoFieldTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'field', 'datetime', 'tripal'];

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
  protected string $yaml_info_file = __DIR__ . '/TripalEntityChadoFieldMethodTest-TestInfo.yml';

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
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO
    );

    // Adds contact which will be referred to by linker field in some scenarios.
    $values = [
      'name' => 'Zhanna Beissekova',
      'description' => 'Enjoys grading and is fascinated by how each stone is different, even within the same species',
    ];
    $this->chado_connection->insert('1:contact')
      ->fields($values)
      ->execute();

    // Setup the environment.
    $this->setupChadoEntityFieldTestEnvironment($this->system_under_test);
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
      "Simple Entity",
    ];

    return $scenarios;
  }

  /**
   * Data Provider: works with the YAML to provide scenarios for testing.
   *
   * @return array
   *   List of scenarios to test where each one matches a key and label in the
   *   associated YAML scenarios.
   */
  public static function provideExceptionScenarios() {
    $scenarios = [];

    $scenarios[] = [
      1,
      'No Storage Exception expected',
    ];

    return $scenarios;
  }

  /**
   * Tests a variety of basic get/set methods on the TripalEntity class.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testTripalEntityFieldMethods(int $current_scenario_key, string $current_scenario_label) {
    $current_scenario = $this->getYamlScenario($current_scenario_key, $current_scenario_label);

    // SETUP:Create the entity with that value set.
    $submitted_title = $this->randomString();
    $entity = TripalEntity::create([
      'title' => $submitted_title,
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");
    $status = $entity->save();
    $this->assertEquals(SAVED_NEW, $status, "We expected to have saved a new entity for our " . $current_scenario['label'] . " scenario.");
    // Load the entity we just created so we can check the values.
    $created_entity = TripalEntity::load($entity->id());
    $this->assertFieldValuesMatch($current_scenario['create']['expected_field_values'], $created_entity, '"' . $current_scenario['label'] . '" being created. ');

    // TEST: that invalid fields are not registered as tripal fields.
    // -- check registering fields performs as expected.
    foreach ($current_scenario['expectations']['registerField'] as $details) {
      $field_name = $details['field_name'];
      $expected_result = $details['expected_result'];
      $result = $created_entity->registerTripalField($field_name);
      if ($expected_result) {
        $this->assertTrue($result, "We expected the '$field_name' field to be registered as a tripal field.");
      }
      else {
        $this->assertFalse($result, "We expected the '$field_name' field to not be registered as a tripal field.");
      }
      $field_name = $this->randomMachineName() . '_noDefinition';
    }

    // TEST: tripal fields per storage are detected properly.
    // -- all fields for chado_storage should be detected.
    $basic_tripal_fields = $created_entity->getTripalStorageFields('chado_storage');
    $this->assertEqualsCanonicalizing($current_scenario['expectations']['getStorageFields']['all_fields'], $basic_tripal_fields, "We expected to have detected all of the tripal fields for our " . $current_scenario['label'] . " scenario.");

    // -- required fields for chado_storage should be detected.
    $required_tripal_fields = $created_entity->getTripalStorageFields(
      'chado_storage',
      ['is_required' => TRUE]
    );
    $this->assertEqualsCanonicalizing($current_scenario['expectations']['getStorageFields']['required_fields'], $required_tripal_fields, "We expected to have detected the required tripal fields for our " . $current_scenario['label'] . " scenario.");

    // -- optional fields for chado_storage should be detected.
    $optional_tripal_fields = $created_entity->getTripalStorageFields(
      'chado_storage',
      ['is_required' => FALSE]
    );
    $this->assertEqualsCanonicalizing($current_scenario['expectations']['getStorageFields']['optional_fields'], $optional_tripal_fields, "We expected to have detected the optional tripal fields for our " . $current_scenario['label'] . " scenario.");

    // -- no fields should be detected for a non-existent storage.
    $no_tripal_fields = $created_entity->getTripalStorageFields('not_a_real_storage');
    $this->assertEmpty($no_tripal_fields, "We expected to have detected no tripal fields for a non-existent storage for our " . $current_scenario['label'] . " scenario.");

    // TEST: the correct information is being stored about tripal fields.
    foreach ($current_scenario['expectations']['getTripalFieldInfo'] as $field_name => $expected_info) {
      foreach ($expected_info as $request_key => $expected_value) {
        $field_info = $created_entity->getTripalFieldInfo($field_name, $request_key);
        $this->assertEquals($expected_value, $field_info, "We expected to have retrieved the correct $request_key info for the '$field_name' field for our " . $current_scenario['label'] . " scenario.");
      }
    }

    // TEST: getFieldItemBackendStorage() returns what we expect.
    foreach ($current_scenario['expectations']['getFieldItemBackendStorage'] as $field_name => $expectations) {
      $item = $created_entity->get($field_name)->first();
      $storage = $created_entity->getFieldItemBackendStorage($field_name, $item);
      $this->assertEquals($expectations['expected_result'], $storage, "We expected getFieldItemBackendStorage() to return either FALSE or the storage plugin ID for the '$field_name' field for our " . $current_scenario['label'] . " scenario.");
    }
  }

  /**
   * Tests exceptions not thrown during create and easy to trigger.
   */
  public function testSimpleExceptions() {
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
      'project_name' => [
        'record_id' => 0,
        'value' => $this->randomString(),
      ],
    ]);
    $entity->save();

    // Test an invalid field name for getTripalFieldInfo().
    $exception_thrown = FALSE;
    $exception_message = 'NOT THROWN';
    try {
      $entity->getTripalFieldInfo('NOT_A_VALID_FIELD', 'field_type');
    }
    catch (\Exception $e) {
      $exception_thrown = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_thrown, "We expected an exception to be thrown for an invalid field info request.");
    $this->assertStringContainsString(
      "You requested information for a field (i.e. 'NOT_A_VALID_FIELD') that is either not attached to this entity or not a valid TripalField.",
      $exception_message,
      "We expected a specific exception message for an invalid field info request."
    );

    // Test an invalid request key for getTripalFieldInfo().
    $exception_thrown = FALSE;
    $exception_message = 'NOT THROWN';
    try {
      $entity->getTripalFieldInfo('project_name', 'NOT_A_VALID_KEY');
    }
    catch (\Exception $e) {
      $exception_thrown = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_thrown, "We expected an exception to be thrown for an invalid field info request.");
    $this->assertStringContainsString(
      "The Request key 'NOT_A_VALID_KEY' is not supported by TripalEntity::getTripalFieldInfo()",
      $exception_message,
      "We expected a specific exception message for an invalid field info request."
    );
  }

  /**
   * Specifically tests for exceptions thrown during create.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideExceptionScenarios
   */
  #[DataProvider('provideExceptionScenarios')]
  public function testExceptionThrownInCreate(int $current_scenario_key, string $current_scenario_label) {
    $current_scenario = $this->getYamlScenario($current_scenario_key, $current_scenario_label);

    $exception_thrown = FALSE;
    $exception_message = 'NOT THROWN';
    try {
      // Create the entity with that value set.
      $entity = TripalEntity::create([
        'title' => $this->randomString(),
        'type' => $this->bundle_name,
      ] + $current_scenario['user_input']);
      $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");
      $entity->save();
    }
    catch (\Exception $e) {
      $exception_thrown = TRUE;
      $exception_message = $e->getMessage();
    }

    $this->assertTrue($exception_thrown, "We expected an exception to be thrown for our " . $current_scenario['label'] . " scenario.");
    $this->assertStringContainsString(
      $current_scenario['exception_message'],
      $exception_message,
      "We expected a specific exception message for our " . $current_scenario['label'] . " scenario."
    );
  }

}
