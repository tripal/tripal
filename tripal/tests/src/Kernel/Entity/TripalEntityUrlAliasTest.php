<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\field\Entity\FieldStorageConfig;
use Drupal\field\Entity\FieldConfig;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal\Traits\TripalEntityFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Tests the TripalEntity URL alias system specifically.
 *
 * @group TripalEntity
 * @group TripalTokenParser
 */
class TripalEntityUrlAliasTest extends TripalTestKernelBase {

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
  protected string $yaml_info_file = __DIR__ . '/TripalEntityUrlAliasTest-TestInfo.yml';

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

    $this->setupEntityFieldTestEnvironment($this->system_under_test);
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

    $labels = [
      "Use format for URL alias (multi-word)",
      "Use format for URL alias (optional value)",
      "Use format for URL (duplicate already exists)",
      "Use form element value on create + edit for URL alias",
      "Use form element value on update for URL alias",
      "Use form element value for URL alias (duplicate already exists)",
    ];
    foreach ($labels as $key => $label) {
      $scenarios[] = [$key, $label];
    }

    return $scenarios;
  }

  /**
   * Tests that TripalEntity::save() handles URL alias' with substitutions.
   *
   * @dataProvider provideScenarios
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   */
  public function testTripalEntitySaveUrlAlias(int $current_scenario_key, string $current_scenario_label) {
    $current_scenario = $this->scenarios[$current_scenario_key];
    $this->assertEquals($current_scenario_label, $current_scenario['label'], "We may not have retrieved the expected scenario as the labels did not match.");

    // 0. Create any pre-existing entities before the test if some are specified.
    if (array_key_exists('pre_create', $current_scenario)) {
      $entity = TripalEntity::create([
        'type' => $this->bundle_name,
      ] + $current_scenario['pre_create']);
      $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to SET UP FOR our " . $current_scenario['label'] . " scenario.");
      $status = $entity->save();
    }

    // 1. Create the entity with that value set.
    $entity = TripalEntity::create([
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");

    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $status = $entity->save();
    }
    catch (\Exception $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }

    $this->assertEquals(SAVED_NEW, $status, "We expected to have saved a new entity for our " . $current_scenario['label'] . " scenario.");
    $this->assertEquals(
      $current_scenario['create']['expected']['exception'],
      $exception_caught,
      "Regarding an exception being thrown on create, we did not get what we expected."
    );
    $this->assertEquals(
      $current_scenario['create']['expected']['exception_message'],
      $exception_message,
      "The message of the exception thrown on create was not what we expected."
    );

    // We cannot test update if create failed.
    if ($exception_caught) {
      return;
    }

    // 2. Load the entity we just created so we can check the values.
    $created_entity = TripalEntity::load($entity->id());
    $this->assertFieldValuesMatch($current_scenario['create']['expected_values'], $created_entity, '"' . $current_scenario['label'] . '" being created. ');
    // -- Title.
    $this->assertEquals($current_scenario['create']['expected']['title'], $created_entity->getTitle(), "We did not get the title we expected when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    // -- URL.
    $retrieved_alias = $created_entity->getAlias();
    $this->assertIsArray($retrieved_alias, "The retrieved path should be an array when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertArrayHasKey('alias', $retrieved_alias, "The retrieved path should have an alias property when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertEquals($current_scenario['create']['expected']['url_alias'], $retrieved_alias['alias'], "We did not get the url alias we expected when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");

    // 3. Make changes and then save again.
    foreach ($current_scenario['edit']['user_input'] as $field_name => $new_values) {
      $created_entity->set($field_name, $new_values);
    }

    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $status = $created_entity->save();
    } catch (\Exception $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }

    $this->assertEquals(SAVED_UPDATED, $status, "We expected to have updated the existing entity for our " . $current_scenario['label'] . " scenario.");
    $this->assertEquals(
      $current_scenario['edit']['expected']['exception'],
      $exception_caught,
      "Regarding an exception being thrown on update, we did not get what we expected. The message thrown was '$exception_message'."
    );
    $this->assertEquals(
      $current_scenario['edit']['expected']['exception_message'],
      $exception_message,
      "The message of the exception thrown on update was not what we expected."
    );

    // 4. Load the entity we just updated so we can check the values.
    $updated_entity = TripalEntity::load($created_entity->id());
    $this->assertFieldValuesMatch($current_scenario['edit']['expected_values'], $updated_entity, '"' . $current_scenario['label'] . '" being updated. ');
    // -- Title.
    $this->assertEquals($current_scenario['edit']['expected']['title'], $updated_entity->getTitle(), "We did not get the title we expected when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    // -- URL.
    $retrieved_alias = $created_entity->getAlias();
    $this->assertIsArray($retrieved_alias, "The retrieved path should be an array when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertArrayHasKey('alias', $retrieved_alias, "The retrieved path should have an alias property when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertEquals($current_scenario['edit']['expected']['url_alias'], $retrieved_alias['alias'], "We did not get the url alias we expected when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
  }
}
