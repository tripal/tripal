<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;


/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class PropertyValueClassTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * A dummy Tripal Term for use where ever tripal storage needs one.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected object $mock_term;

  /**
   * A dummy Tripal ID Space for use where ever tripal storage needs one.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase
   */
  protected object $mock_idspace;

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
    $this->mock_idspace = $this->createMock(\Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface::class);
    $this->mock_idspace->method('getTerm')
      ->willReturnCallback(function($accession) {
        if ($accession == 'term') {
          return $this->mock_term;
        }
        else {
          return NULL;
        }
      });
    // Create a mock Tripal ID Space service to return our mock idspace when asked.
    $mock_idspace_service = $this->createMock(\Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager::class);
    $mock_idspace_service->method('loadCollection')
      ->willReturnCallback(function($id_space) {
        if ($id_space == 'mock') {
          return $this->mock_idspace;
        }
        else {
          return NULL;
        }
      });
    $container->set('tripal.collection_plugin_manager.idspace', $mock_idspace_service);
  }

  public function testPropertyValueClass() {

    // Valid Parameters.
    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakePropertyTypeKey';
    $term_id = 'mock:term';
    $entityId = 5;

    // Create with default value.
    $instance = '\Drupal\tripal\TripalStorage\StoragePropertyValue';
    $propertyValue = new \Drupal\tripal\TripalStorage\StoragePropertyValue($entityType, $fieldType, $key, $term_id, $entityId);
    $this->assertIsObject($propertyValue, "We were not able to create an object for PropertyValue.");
    $this->assertInstanceOf($instance, $propertyValue,
      "We created an object but it was not the type we expected.");

    // Try getting the value when it wasn't set during creation.
    $value = $propertyValue->getValue();
    $this->assertNull($value, "The value should not be set as we didn't set it on creation.");

    // We can get the Entity ID, right?
    $retrieved = $propertyValue->getEntityId();
    $this->assertEquals($entityId, $retrieved, "We were not able to retrieve the entity id.");

    // Create with a set value.
    $instance = '\Drupal\tripal\TripalStorage\StoragePropertyValue';
    $propertyValue = new \Drupal\tripal\TripalStorage\StoragePropertyValue($entityType, $fieldType, $key, $term_id, $entityId, 333);
    $this->assertIsObject($propertyValue, "We were not able to create an object for PropertyValue.");
    $this->assertInstanceOf($instance, $propertyValue,
      "We created an object but it was not the type we expected.");

    // Try getting the value when it wasn't set during creation.
    $value = $propertyValue->getValue();
    $this->assertEquals(333, $value, "The value should have been set to 333 on creation.");

    // Now lets set it to something else and check it changed.
    $propertyValue->setValue(999);
    $value = $propertyValue->getValue();
    $this->assertEquals(999, $value, "The value should have been set to 999 just now.");
  }

}
