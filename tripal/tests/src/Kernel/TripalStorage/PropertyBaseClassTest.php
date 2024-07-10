<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class PropertyBaseClassTest extends TripalTestKernelBase {

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

  /**
   * Tests the creation of properties focusing on invalid parameters.
   *
   * Note: Valid parameters are checked when testing property types + values.
   */
  public function testPropertyCreation() {

    // Valid Parameters.
    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakepropertyKey';
    $term_id = 'mock:term';

    // Test passing in a badly formatted term.
    $exception_message = NULL;
    $bad_term = 'NoColonDelimiter';
    try {
      $property = new \Drupal\tripal\TripalStorage\StoragePropertyBase($entityType, $fieldType, $key, $bad_term);
    }
    catch (\Exception $e) {
      $exception_message = $e->getMessage();
    }
    $this->assertStringContainsString('properly formatted term', $exception_message,
      "We did not get the exception message we expected for passing in a badly formatted term.");

    // Test passing in a term whose ID Space doesn't exist in our mock.
    $exception_message = NULL;
    $bad_term = 'MissingIdSpace:term';
    try {
      $property = new \Drupal\tripal\TripalStorage\StoragePropertyBase($entityType, $fieldType, $key, $bad_term);
    }
    catch (\Exception $e) {
      $exception_message = $e->getMessage();
    }
    $this->assertStringContainsString('IdSpace for the property term is not recognized', $exception_message,
      "We did not get the exception message we expected for passing in a term whose id space didn't exist.");

    // Test passing in a term whose ID Space doesn't exist in our mock.
    $exception_message = NULL;
    $bad_term = 'mock:MissingTerm';
    try {
      $property = new \Drupal\tripal\TripalStorage\StoragePropertyBase($entityType, $fieldType, $key, $bad_term);
    }
    catch (\Exception $e) {
      $exception_message = $e->getMessage();
    }
    $this->assertStringContainsString('accession for the property term is not recognized', $exception_message,
      "We did not get the exception message we expected for passing in a term whose accession didn't exist.");

    // Test passing in empty Entity type.
    $exception_message = NULL;
    try {
      $property = new \Drupal\tripal\TripalStorage\StoragePropertyBase('', $fieldType, $key, $term_id);
    }
    catch (\Exception $e) {
      $exception_message = $e->getMessage();
    }
    $this->assertStringContainsString('without specifying the entity type', $exception_message,
      "We did not get the exception message we expected for passing in an empty string for entity type.");

    // Test passing in empty field type.
    $exception_message = NULL;
    try {
      $property = new \Drupal\tripal\TripalStorage\StoragePropertyBase($entityType, '', $key, $term_id);
    }
    catch (\Exception $e) {
      $exception_message = $e->getMessage();
    }
    $this->assertStringContainsString('without specifying the field', $exception_message,
      "We did not get the exception message we expected for passing in an empty string for entity type.");

    // Test passing in empty property key.
    $exception_message = NULL;
    try {
      $property = new \Drupal\tripal\TripalStorage\StoragePropertyBase($entityType, $fieldType, '', $term_id);
    }
    catch (\Exception $e) {
      $exception_message = $e->getMessage();
    }
    $this->assertStringContainsString('without a key', $exception_message,
      "We did not get the exception message we expected for passing in an empty string for entity type.");

  }
}
