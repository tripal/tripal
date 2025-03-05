<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\field\Entity\FieldStorageConfig;
use Drupal\field\Entity\FieldConfig;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal\Traits\TripalEntityFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Tests the TripalEntity Class with Chado Fields attached.
 *
 * @group TripalEntity
 * @group ChadoFields
 * @group TripalTokenParser
 */
class TripalEntityChadoFieldTest extends ChadoTestKernelBase {

  use TripalEntityFieldTestTrait;

  /**
   * {@inheritdoc}
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
  protected string $yaml_info_file = __DIR__ . '/TripalEntityChadoFieldTest-TestInfo.yml';

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
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO
    );

    // Adds contact which will be referred to by linker field in some scenarios.
    $values = [
      'name' => 'Zhanna Beissekova',
      'description' => 'Enjoys grading and is fascinated by how each stone is different, even within the same species'
    ];
    $this->chado_connection->insert('1:contact')
      ->fields($values)
      ->execute();

    // Setup the environment.
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

    $scenarios[] = [
      0,
      "Use format for title + URL",
    ];

    $scenarios[] = [
      1,
      'Use format with read_value for title + URL + override url on edit',
    ];

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

    // 1. Create the entity with that value set.
    $submitted_title = $this->randomString();
    $entity = TripalEntity::create([
      'title' => $submitted_title,
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $current_scenario['label'] . " scenario.");
    $status = $entity->save();
    $this->assertEquals(SAVED_NEW, $status, "We expected to have saved a new entity for our " . $current_scenario['label'] . " scenario.");

    // 2. Load the entity we just created so we can check the values.
    $created_entity = TripalEntity::load($entity->id());
    $this->assertFieldValuesMatch($current_scenario['create']['expected'], $created_entity, '"' . $current_scenario['label'] . '" being created. ');
    // -- Title.
    $this->assertNotEquals($submitted_title, $created_entity->getTitle(), "The submitted title should never be used but it was when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertEquals($current_scenario['create']['title'], $created_entity->getTitle(), "We did not get the title we expected when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    // -- URL.
    $retrieved_alias = $created_entity->getAlias();
    $this->assertIsArray($retrieved_alias, "The retrieved path should be an array when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertArrayHasKey('alias', $retrieved_alias, "The retrieved path should have an alias property when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertEquals($current_scenario['create']['url'], $retrieved_alias['alias'], "We did not get the url alias we expected when CREATING the entity for the '" . $current_scenario['label'] . "' scenario.");

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
    $this->assertFieldValuesMatch($current_scenario['edit']['expected'], $updated_entity, '"' . $current_scenario['label'] . '" being updated. ');
    // -- Title.
    $this->assertNotEquals($submitted_title, $updated_entity->getTitle(), "The submitted title should never be used but it was when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertEquals($current_scenario['edit']['title'], $updated_entity->getTitle(), "We did not get the title we expected when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    // -- URL.
    $retrieved_alias = $created_entity->getAlias();
    $this->assertIsArray($retrieved_alias, "The retrieved path should be an array when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertArrayHasKey('alias', $retrieved_alias, "The retrieved path should have an alias property when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
    $this->assertEquals($current_scenario['edit']['url'], $retrieved_alias['alias'], "We did not get the url alias we expected when UPDATING the entity for the '" . $current_scenario['label'] . "' scenario.");
  }
}
