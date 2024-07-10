<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;
use Drupal\tripal\Plugin\TripalStorage\DrupalSqlStorage;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class DrupalSqlStorageTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * A dummy Tripal Term for use whereever chado storage needs one.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected object $mock_term;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Grab the container.
    $container = \Drupal::getContainer();

    // We need a term for property types so we will create a generic mocked one
    // here which will be pulled from the container any time a term is requested.
    $this->mock_term = $this->createMock(\Drupal\tripal\TripalVocabTerms\TripalTerm::class);
    // Create a mock ID space to return our mock term when asked.
    $mock_idspace = $this->createMock(\Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface::class);
    $mock_idspace->method('getTerm')
      ->willReturn($this->mock_term);
    // Create a mock Tripal ID Space service to return our mock idspace when asked.
    $mock_idspace_service = $this->createMock(\Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager::class);
    $mock_idspace_service->method('loadCollection')
      ->willReturn($mock_idspace);
    $container->set('tripal.collection_plugin_manager.idspace', $mock_idspace_service);


    // Check that the static create method is returning the above mocked objects
    // as we expect it to. Note: Testing in this way is a big arbitrary as the
    // create() method is used for dependency injection but we like to be thorough.
    $returned_static_class = DrupalSqlStorage::create(
      $container,
      [],
      'test_plugin_id',
      []
    );
    $this->assertEquals([], $returned_static_class->getPluginDefinition(),
      "We expect to be able to retrieve the plugin definition that we passed in.");
    $this->assertEquals('test_plugin_id', $returned_static_class->getPluginId(),
      "We expect to be able to retrieve the plugin ID we set.");
    // We also want to test the logger but it is a protected variable.
    // Use closures to test.
    $that = $this;
    $assertClosure = function ()  use ($that){
      $that->assertIsObject($this->logger,
        "The message logging object in our plugin was not set properly.");
      $this->assertInstanceOf(\Drupal\tripal\Services\TripalLogger::class, $this->logger,
      "We expect the logger to have been set and be a Tripal Logger.");
    };
     $doAssertLogger = $assertClosure->bindTo($returned_static_class, get_class($returned_static_class));
  }

  /**
   * Tests the add/get field definition functionality.
   */
  public function testDrupalSqlStorageCRUD() {

    // To create a tripal storage object we will need the parameters required
    // for the constructor.
    $configuration = [];
    $plugin_id = 'fakePluginName';
    $plugin_definition = [];
    $logger = \Drupal::service('tripal.logger');
    // Create a new instance of Drupal SQL Storage for testing purposes.
    $manager = \Drupal::service('tripal.storage');
    $drupalSqlStorage = $manager->getInstance(['plugin_id' => 'drupal_sql_storage']);
    $this->assertIsObject($drupalSqlStorage, "Unable to create Drupal SQL Storage object.");

    // All crud should be handled outside of TripalStorage...
    // So we are just checking that these methods are implemented.
    $values = [];
    // Should return TRUE.
    $return_value = $drupalSqlStorage->updateValues($values);
    $this->assertTrue($return_value, "drupalSqlStorage::updateValues() did not return the expected TRUE boolean.");
    $return_value = $drupalSqlStorage->deleteValues($values);
    $this->assertTrue($return_value, "drupalSqlStorage::deleteValues() did not return the expected TRUE boolean.");
    $return_value = $drupalSqlStorage->insertValues($values);
    $this->assertTrue($return_value, "drupalSqlStorage::insertValues() did not return the expected TRUE boolean.");
    $return_value = $drupalSqlStorage->loadValues($values);
    $this->assertTrue($return_value, "drupalSqlStorage::loadValues() did not return the expected TRUE boolean.");
    // Should return empty array.
    $return_value = $drupalSqlStorage->validateValues($values);
    $this->assertIsArray($return_value, "drupalSqlStorage::validateValues() did not return the expected empty array.");
    $this->assertCount(0, $return_value, "The returned array should be empty but is not.");
    $return_value = $drupalSqlStorage->findValues($values);
    $this->assertIsArray($return_value, "drupalSqlStorage::findValues() did not return the expected empty array.");
    $this->assertCount(0, $return_value, "The returned array should be empty but is not.");

  }
}
