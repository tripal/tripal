<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\KernelTests\KernelTestBase;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests schema alterations for TripalEntity storage configuration.
 */
#[RunTestsInSeparateProcesses]
class TripalEntityConfigAlterTest extends KernelTestBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = [
    'system',
    'field',
    'options',
    'tripal',
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // IMPORTANT: reset cached schema definitions.
    $this->container
      ->get('config.typed')
      ->clearCachedDefinitions();

  }

  /**
   * Tests that the config schema is properly altered.
   *
   * @see TripalEntityHooks::configSchemaInfoAlter()
   */
  public function testSchemaIsAltered() {

    // Fetch the altered schema definition.
    $definition = $this->container->get('config.typed')->getDefinition('field.storage.tripal_entity.test');
    $this->assertNotEmpty($definition, 'Tripal entity storage schema exists.');

    // Navigate to the altered mapping.
    $this->assertArrayHasKey('mapping', $definition);
    $this->assertArrayHasKey('settings', $definition['mapping']);
    $this->assertArrayHasKey('mapping', $definition['mapping']['settings']);
    $settings_mapping = $definition['mapping']['settings']['mapping'] ?? [];

    // Check that a setting from the Drupal string field is available now.
    $this->assertArrayHasKey(
      'case_sensitive',
      $settings_mapping,
      'Setting from Drupal field storage was copied to tripal entity.'
    );

    // Check that a setting from Tripal fields is still available.
    $this->assertArrayHasKey(
      'storage_plugin_id',
      $settings_mapping,
      'Setting from Tripal field storage is still available.'
    );

    // Check that a setting shared between Drupal and Tripal fields
    // is still correctly defined.
    $expected_maxlength_definition = [
      'type' => 'integer',
      'label' => 'Maximum length',
    ];
    $this->assertArrayHasKey(
      'max_length',
      $settings_mapping,
      'Setting from Tripal field storage is still available.'
    );
    $this->assertEquals($expected_maxlength_definition, $settings_mapping['max_length'], 'Shared setting is still correctly defined.');

    // Check that allowed_values is correctly defined with float keys.
    $this->assertArrayHasKey(
      'allowed_values',
      $settings_mapping,
      'Setting with conflicting definitions is still available.'
    );
    $allowed_values_definition = $settings_mapping['allowed_values'];
    $this->assertArrayHasKey('sequence', $allowed_values_definition);
    $this->assertArrayHasKey('mapping', $allowed_values_definition['sequence']);
    $this->assertArrayHasKey('value', $allowed_values_definition['sequence']['mapping']);
    $this->assertArrayHasKey('type', $allowed_values_definition['sequence']['mapping']['value']);
    $this->assertEquals('float', $allowed_values_definition['sequence']['mapping']['value']['type'], 'allowed_values is correctly defined with more permissive float keys.');

  }

}
