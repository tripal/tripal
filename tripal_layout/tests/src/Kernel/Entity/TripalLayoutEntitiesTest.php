<?php

namespace Drupal\Tests\tripal_layout\Kernel\Entity;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal_layout\Traits\TripalLayoutTestTrait;

/**
 * Tests the TripalLayoutDefaultView and TripalLayoutDefaultForm entities.
 *
 * @group TripalLayoutDisplay
 */
class TripalLayoutEntitiesTest extends TripalTestKernelBase {

  use TripalLayoutTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'tripal', 'tripal_layout'];


  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_layout_default_form');
    $this->installEntitySchema('tripal_layout_default_view');

  }

  /**
   * Provides senarios to test the the TripalLayoutDefaultView and
   * TripalLayoutDefaultForm entities.
   *
   * @return array
   *   An array of senarios to test.
   */
  public function provideLayoutDisplayEntitySenarios() {
    $senarios = [];

    $entity_defns = [
      'view' => [
        'class' => '\Drupal\tripal_layout\Entity\TripalLayoutDefaultView',
        'id' => 'tripal_layout_default_view'
      ],
      'form' => [
        'class' => '\Drupal\tripal_layout\Entity\TripalLayoutDefaultForm',
        'id' => 'tripal_layout_default_form'
      ],
    ];

    $bundle_defns = [
      'organism' => [
        'id' => 'organism',
      ],
    ];

    $senarios['organism_view'] = [
      'display_context' => 'view',
      'entity_defn' => $entity_defns['view'],
      'bundle_defn' => $bundle_defns['organism'],
      'expectations' => [
        'num_layouts' => 2,
        'layouts' => [
          'organism',
          'analysis',
        ],
      ],
    ];
    $senarios['organism_view']['entity_defn']['yaml_file'] = __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_view.test_view.yml';

    $senarios['organism_form'] = [
      'display_context' => 'form',
      'entity_defn' => $entity_defns['form'],
      'bundle_defn' => $bundle_defns['organism'],
      'expectations' => [
        'num_layouts' => 2,
        'layouts' => [
          'organism',
          'analysis',
        ],
      ],
    ];
    $senarios['organism_form']['entity_defn']['yaml_file'] = __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_form.test_form.yml';
    return $senarios;
  }

  /**
   * Tests getters for a test TripalLayoutEntity.
   *
   * @dataProvider provideLayoutDisplayEntitySenarios
   *
   * @param string $display_context
   *   The type of display entity we are testing. One of 'view' or 'form'.
   * @param array $entity_defn
   *   Details about the TripalLayoutEntity we are testing.
   *   Expected keys include: class, id, yaml_file.
   * @param array $bundle_defn
   *   Details about the TripalEntityType whose display we want to test.
   *   Expected keys include: id
   * @param array $expectations
   *   An array of expectations for this test. Keys include:
   *    - num_layouts: the number of layouts in the file.
   *    - layouts: an list of the tripal_entity_type the layouts are for.
   * @return void
   */
  public function testTripalLayoutEntityGetters(string $display_context, array $entity_defn, array $bundle_defn, array $expectations) {

    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_storage **/
    $config_storage = \Drupal::entityTypeManager()->getStorage($entity_defn['id']);

    // Create entity from valid YAML
    $config_entity = $this->createLayoutEntityFromConfig(
      $entity_defn['id'],
      $entity_defn['yaml_file']
    );

    // Also get the TEST YAML file for validation.
    $yaml_file = $entity_defn['yaml_file'];
    $yaml = \Symfony\Component\Yaml\Yaml::parseFile($yaml_file);
    $this->assertIsArray($yaml, "Unable to pull down the test YAML file ($yaml_file).");

    $ret_id = $config_entity->id();
    $this->assertIsString($ret_id, "Unable to retrieve the id.");
    $this->assertEquals($yaml['id'], $ret_id, "The id of the config entity did not match what we expected.");

    $ret_label = $config_entity->label();
    $this->assertIsString($ret_label, "Unable to retrieve the label.");
    $this->assertEquals($yaml['label'], $ret_label, "The label of the config entity did not match what we expected.");

    $ret_description = $config_entity->description();
    $this->assertIsString($ret_description, "Unable to retrieve the description.");
    $this->assertEquals($yaml['description'], $ret_description, "The description of the config entity did not match what we expected.");

    $ret_layouts = $config_entity->getLayouts();
    $this->assertIsArray($ret_layouts, "Unable to retrieve the layouts for this config entity.");
    $this->assertCount($expectations['num_layouts'], $ret_layouts, "There were not the expected number of layouts defined for this config entity.");

    // Check we can get specific bundle layouts that do exist.
    foreach ($expectations['layouts'] as $expected_bundle) {
      // Checks we can detect if this config entity has the expected bundle
      // when the bundle layouts have NOT been cached.
      $ret_has_layout = $config_entity->hasLayout($expected_bundle);
      $this->assertTrue($ret_has_layout, "This config entity doesn't have the expected $expected_bundle bundle layout according to hasLayout().");

      // Checks that we can get the layout once the bundle layout cache HAS BEEN built.
      $ret_bundle_layout = $config_entity->getLayout($expected_bundle);
      $this->assertNotNull($ret_bundle_layout, "The config entity was unable to retrieve the expected $expected_bundle bundle layout.");
      $this->assertIsArray($ret_bundle_layout, "The retrieved bundle layout for $expected_bundle did not match the expected format when layouts cached.");

      // Checks that we can get the layout
      // when the bundle layouts have NOT been cached.
      $config_entity->clearLayoutCache();
      $ret_bundle_layout = $config_entity->getLayout($expected_bundle);
      $this->assertNotNull($ret_bundle_layout, "The config entity was unable to retrieve the expected $expected_bundle bundle layout.");
      $this->assertIsArray($ret_bundle_layout, "The retrieved bundle layout for $expected_bundle did not match the expected format.");

      // Checks we can detect if this config entity has the expected bundle
      // once the cache HAS BEEN built.
      $ret_has_layout = $config_entity->hasLayout($expected_bundle);
      $this->assertTrue($ret_has_layout, "This config entity doesn't have the expected $expected_bundle bundle layout according to hasLayout() when layouts cached.");
    }

    // Check that we can't get layouts that do not exist.
    $nonexistant_bundle = uniqid();
    $ret_has_layout = $config_entity->hasLayout($nonexistant_bundle);
    $this->assertFalse($ret_has_layout, "This config entity should not indicate it has a bunel that does not exist.");

    $ret_bundle_layout = $config_entity->getLayout($nonexistant_bundle);
    $this->assertNull($ret_bundle_layout, "The config entity should not be able to retrieve the layout for a bundle that doesn't exist.");
  }
}
