<?php

namespace Drupal\Tests\tripal_layout\Traits;

trait TripalLayoutTestTrait {

  /**
   * Provides information about the different tripal layout config entities.
   *
   * @var array
   */
  protected array $tripal_layout_entity_details = [
    'tripal_layout_default_view' => [
      'class' => '\Drupal\tripal_layout\Entity\TripalLayoutDefaultView',
      'id' => 'tripal_layout_default_view',
    ],
    'tripal_layout_default_form' => [
      'class' => '\Drupal\tripal_layout\Entity\TripalLayoutDefaultForm',
      'id' => 'tripal_layout_default_form',
    ],
  ];

  /**
   * Undocumented function
   *
   * @param string $config_entity_type
   *   The type of layout entity to create.
   *   One of those defined in $tripal_layout_entity_details above.
   * @param string $yaml_file
   *   The full path to a YAML file providing the definition for this type of
   *   layout config entity. The YAML must be valid.
   * @return void
   */
  public function createLayoutEntityFromConfig(string $config_entity_type, string $yaml_file) {

    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_storage **/
    $config_storage = \Drupal::entityTypeManager()->getStorage($config_entity_type);
    // -- Get the TEST YAML file.
    $yaml = \Symfony\Component\Yaml\Yaml::parseFile($yaml_file);
    $this->assertIsArray($yaml, "Unable to pull down the test YAML file ($yaml_file).");
    // -- Create a config entity of the specified type from the YAML.
    $config_entity = $config_storage->createFromStorageRecord($yaml);
    $config_entity->save();
    $this->assertIsObject($config_entity, "Unable to create a $config_entity_type config entity from the test file yaml ($yaml_file).");
    $this->assertInstanceOf(
      $this->tripal_layout_entity_details[$config_entity_type]['class'],
      $config_entity,
      "The created test entity is not of the correct type."
    );

    return $config_entity;
  }
}
