<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalEntityTypeCollection;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;


/**
 * Focused on testing the create() and createContentType() methods.
 *
 * @group Tripal
 * @group Tripal Content
 * @group TripalEntityTypeCollection
 */
class TripalEntityTypeCollectionGetCollectionsTest extends TripalTestKernelBase {


  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * A made-up set of details for some collection types to be used in testing
   * getTypeCollections. These will be written to storage in the setUp().
   */
  protected array $config_array = [
    'tripal.tripalentitytype_collection.monsters' => [
      'id' => 'monsters',
      'label' => 'Monsters',
      'description' => 'Types of monsters including those who live in the water, on land, and under beds.'
    ],
    'tripal.tripalentitytype_collection.fairies' => [
      'id' => 'fairies',
      'label' => 'Fairies',
      'description' => 'Types of fairies including those from both the Seelie and Unseelie Courts.'
    ],
  ];


    /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Define some types to get when testing.
    $active_storage = \Drupal::service('config.storage');
    foreach ($this->config_array as $config_item => $config) {
      $active_storage->write($config_item, $config);
    }
  }

  /**
   * Tests the TripalEntityTypeCollection::getTypeCollections() method.
   */
  public function testTripalEntityTypeCollection_getTypeCollections() {

    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');

    // Try retrieving collections when the config has not been loaded into the test environment.
    $collections = $content_type_service->getTypeCollections();
    $this->assertIsArray($collections,
      "TripalEntityTypeCollection::getTypeCollections() should always return an array.");
    $this->assertCount(2, $collections,
      "We expect there two be two collections based on what we setup for the test.");

    // Test that each one matches what we expect.
    foreach ($this->config_array as $expected_config) {
      $expected_config_id = $expected_config['id'];

      $this->assertArrayHasKey($expected_config_id, $collections,
        "Each expected config should be in the returned collections.");
      $this->assertIsArray($collections[$expected_config_id],
        "The details returned for a collection should be an array.");
      $this->assertEquals($expected_config, $collections[$expected_config_id],
        "We expect the details returned to match those we saved.");
    }
  }
}
