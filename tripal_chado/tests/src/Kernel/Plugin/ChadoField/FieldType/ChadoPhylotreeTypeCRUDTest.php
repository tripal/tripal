<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField\FieldType;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;

/**
 * Tests the ChadoPhylotreeVisTypeDefault Field Type.
 *
 * Specifically focused on create + update actions performed on the entity
 * directly. Both TripalEntity, ChadoStorage and the field will be covered.
 *
 * @group TripalField
 * @group ChadoField
 */
class ChadoPhylotreeTypeCRUDTest extends ChadoTestKernelBase {

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
  protected string $yaml_info_file = __DIR__ . '/ChadoPhylotreeType-TestInfo.yml';

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
   *  - description: A description of the scenario and what you are wanting to
   *    test. This will not be used in the test but is rather there to help
   *    people reading the YAML file and to make it easier to maintain.
   *  - create: An array of the values to be provided when creating a
   *    TripalEntity. There should be a key matching the name of each field in
   *    the system-under-test and its value should be an array containing all
   *    the property types for that field mapped to a value.
   *  - edit: An array of the values to be provided when updating an existing
   *    TripalEntity. There should be a key matching the name of each field in
   *    the system-under-test and its value should be an array containing all
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
      "Typical Scenario",
    ];

    return $scenarios;
  }

  /**
   * Retrieves the current scenario based on the data provider.
   *
   * NOTE: Also ensures the type_ids match what is currently in the database.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @return array
   *   The scenario to be tested as defined in the YAML.
   */
  public function retrieveCurrentScenario(int $current_scenario_key, string $current_scenario_label) {

    // Retrieve the correct scenario.
    $current_scenario = $this->scenarios[$current_scenario_key];
    $this->assertEquals($current_scenario_label, $current_scenario['label'], "We may not have retrieved the expected scenario as the labels did not match.");

#    // Set the project type just in case.
#    $type_id = $this->getCvtermID('NCIT', 'C47885');
#    $current_scenario['create']['user_input']['project_type'][0]['type_id'] = $type_id;
#    $current_scenario['create']['expected']['project_type'][0]['type_id'] = $type_id;
#    $current_scenario['edit']['expected']['project_type'][0]['type_id'] = $type_id;
#    // Set the property field types just in case.
#    $comment_type_id = $this->getCvtermID('rdfs', 'comment');
#    $location_type_id = $this->getCvtermID('NCIT', 'C25341');
#    foreach (['create', 'edit'] as $process_key) {
#      foreach (['user_input', 'expected'] as $input_type) {
#        if (array_key_exists('project_prop1', $current_scenario[$process_key][$input_type])) {
#          foreach ($current_scenario[$process_key][$input_type]['project_prop1'] as $delta => $values) {
#            if ($values['type_id'] === 181) {
#              $current_scenario[$process_key][$input_type]['project_prop1'][$delta]['type_id'] = $comment_type_id;
#            }
#          }
#        }
#        if (array_key_exists('project_prop2', $current_scenario[$process_key][$input_type])) {
#          foreach ($current_scenario[$process_key][$input_type]['project_prop2'] as $delta => $values) {
#            if ($values['type_id'] === 159) {
#              $current_scenario[$process_key][$input_type]['project_prop2'][$delta]['type_id'] = $location_type_id;
#            }
#          }
#        }
#      }
#    }

    return $current_scenario;
  }

  /**
   * Tests the ChadoPhylotreeVisType field through TripalEntity->save().
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  public function testChadoPhylotreeVisTypeEntityCrud(int $current_scenario_key, string $current_scenario_label) {
    $current_scenario = $this->retrieveCurrentScenario($current_scenario_key, $current_scenario_label);

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
    // We initially created a tree but it had no nodes attached. Make some.
    $this->createTestNodes(1);
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
   * Create test phylonodes attached to the specified phylotree.
   *
   * This creates a very simple tree with just three nodes:
   * root, internal, and leaf nodes.
   *
   * @param int $phylotree_id
   *   The pkey of the tree to attach nodes to.
   *
   * @return void
   *   No return values.
   */
  protected function createTestNodes(int $phylotree_id): void {

    // These terms are hardcoded in the test chado so no need to look them up.
    $type_root = 109;
    $type_internal = 110;
    $type_leaf = 108;
    $type_gene = 185;
    $type_accession = 3;

    // To test the code involved in linking features and organisms
    // to nodes, create some organisms and features.
    $organism_1_id = $this->chado_connection->insert('1:organism')
      ->fields([
        'genus' => 'Tripalus',
        'species' => 'databasica',
        'common_name' => 'Common tripal',
      ])->execute();
    $organism_2_id = $this->chado_connection->insert('1:organism')
      ->fields([
        'genus' => 'Tripalus',
        'species' => 'bogusii',
        'common_name' => 'Common false tripal',
      ])->execute();
    $organism_3_id = $this->chado_connection->insert('1:organism')
      ->fields([
        'genus' => 'Tripalus',
        'species' => 'fakus',
        'common_name' => 'Rare false tripal',
      ])->execute();
    $feature_1_id = $this->chado_connection->insert('1:feature')
      ->fields([
        'organism_id' => $organism_1_id,
        'uniquename' => 'feature 1',
        'type_id' => $type_gene,
      ])->execute();
    $feature_3_id = $this->chado_connection->insert('1:feature')
      ->fields([
        'organism_id' => $organism_3_id,
        'uniquename' => 'feature 3',
        'type_id' => $type_gene,
      ])->execute();
    $stock_3_id = $this->chado_connection->insert('1:stock')
      ->fields([
        'organism_id' => $organism_3_id,
        'uniquename' => 'stock 3',
        'type_id' => $type_accession,
      ])->execute();

    $root_node_id = $this->chado_connection->insert('1:phylonode')
      ->fields([
        'phylotree_id' => $phylotree_id,
        'parent_phylonode_id' => NULL,
        'left_idx' => 1,
        'right_idx' => 2,
        'type_id' => $type_root,
        'label' => 'Root Node',
        'distance' => NULL,
      ])->execute();
    $internal_node_id = $this->chado_connection->insert('1:phylonode')
      ->fields([
        'phylotree_id' => $phylotree_id,
        'parent_phylonode_id' => $root_node_id,
        'left_idx' => 2,
        'right_idx' => 3,
        'type_id' => $type_internal,
        'label' => 'Internal Node',
        'distance' => 0.001,
      ])->execute();
    $leaf_node_1_id = $this->chado_connection->insert('1:phylonode')
      ->fields([
        'phylotree_id' => $phylotree_id,
        'parent_phylonode_id' => $internal_node_id,
        'left_idx' => 3,
        'right_idx' => 4,
        'type_id' => $type_leaf,
        'label' => 'Leaf Node Tripalus databasica',
        'distance' => 0.002,
        'feature_id' => $feature_1_id,
      ])->execute();
    $leaf_node_2_id = $this->chado_connection->insert('1:phylonode')
      ->fields([
        'phylotree_id' => $phylotree_id,
        'parent_phylonode_id' => $internal_node_id,
        'left_idx' => 4,
        'right_idx' => 5,
        'type_id' => $type_leaf,
        'label' => 'Leaf Node Tripalus bogusii',
        'distance' => 0.002,
      ])->execute();
    $leaf_node_3_id = $this->chado_connection->insert('1:phylonode')
      ->fields([
        'phylotree_id' => $phylotree_id,
        'parent_phylonode_id' => $internal_node_id,
        'left_idx' => 5,
        'right_idx' => 6,
        'type_id' => $type_leaf,
        'label' => 'Leaf Node Tripalus fakus',
        'distance' => 0.002,
        'feature_id' => $feature_3_id,
      ])->execute();
    // The first leaf node had organism linked via feature.
    // Link the second leaf node to the second organism through
    // the linker table.
    $this->chado_connection->insert('1:phylonode_organism')
      ->fields([
        'organism_id' => $organism_2_id,
        'phylonode_id' => $leaf_node_2_id,
      ])->execute();
    // The third leaf node has a linked feature and thus organism,
    // but extend one level and link a stock. This won't affect
    // the linked organism, only the linked entity, but in this
    // test environment there are no entities to link.
    $this->chado_connection->insert('1:stock_feature')
      ->fields([
        'feature_id' => $feature_3_id,
        'stock_id' => $stock_3_id,
        'type_id' => $type_accession,
      ])->execute();
  }

}
