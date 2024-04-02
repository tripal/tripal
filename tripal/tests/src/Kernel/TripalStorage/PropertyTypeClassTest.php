<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class PropertyTypeClassTest extends TripalTestKernelBase {

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
   * Tests the Base Classes for property types focusing on the methods.
   */
  public function testPropertyTypeBaseClass() {

    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakePropertyTypeKey';
    $term_id = 'mock:term';
    $id = 'FAKEStoragePropertyType';
    $storage_settings = ['put something in here' => 'so that we know its been retrieved'];
    $propertyType = new \Drupal\tripal\TripalStorage\StoragePropertyTypeBase($entityType, $fieldType, $key, $term_id, $id, $storage_settings);

    $retrieved = $propertyType->getEntityType();
    $this->assertEquals($entityType, $retrieved,
      "We were not able to retrieve the entity type that we set when creating the property type.");

    $retrieved = $propertyType->getFieldType();
    $this->assertEquals($fieldType, $retrieved,
      "We were not able to retrieve the field type that we set when creating the property type.");

    $retrieved = $propertyType->getKey();
    $this->assertEquals($key, $retrieved,
      "We were not able to retrieve the key that we set when creating the property type.");

    $retrieved = $propertyType->getTerm();
    $this->assertEquals($this->mock_term, $retrieved,
      "We were not able to retrieve the term that we set when creating the property type.");
    $retrieved = $propertyType->getTermIdSpace();
    $this->assertEquals('mock', $retrieved,
      "We were not able to retrieve the idspace of the term we set when creating the property type.");
    $retrieved = $propertyType->getTermAccession();
    $this->assertEquals('term', $retrieved,
      "We were not able to retrieve the accession of the term we set when creating the property type.");

    $retrieved = $propertyType->getId();
    $this->assertEquals($id, $retrieved,
      "We were not able to retrieve the id that we set when creating the property type.");

    $retrieved = $propertyType->getStorageSettings();
    $this->assertEquals($storage_settings, $retrieved,
      "We were not able to retrieve the storage settings that we set when creating the property type.");
    $new_settings = ['these are just' => 'random different words from before'];
    $propertyType->setStorageSettings($new_settings);
    $retrieved = $propertyType->getStorageSettings();
    $this->assertEquals($new_settings, $retrieved,
      "We were not able to retrieve the storage settings that we just set.");

    // Now expand our tests to other methods that do not just access exactly what we supplied.
    // -- Cardinality.
    $retrieved = $propertyType->getCardinality();
    $this->assertEquals(1, $retrieved, "We were not able to retrieve the default cardinality.");
    $propertyType->setCardinality(5);
    $retrieved = $propertyType->getCardinality();
    $this->assertEquals(5, $retrieved, "We were not able to retrieve the cardinality we just set.");
    $propertyType->setCardinality(0);
    $retrieved = $propertyType->getCardinality();
    $this->assertEquals(0, $retrieved, "We were not able to retrieve the cardinality when we try to set it to unlimited.");
    // -- Searchability
    $retrieved = $propertyType->getSearchability();
    $this->assertEquals(TRUE, $retrieved, "We were not able to retrieve the default Searchability.");
    $propertyType->setSearchability(FALSE);
    $retrieved = $propertyType->getSearchability();
    $this->assertFalse($retrieved, "We were not able to retrieve the Searchability we just set.");
    // -- Operations.
    $retrieved = $propertyType->getOperations();
    $this->assertIsArray($retrieved, "We were not able to retrieve the default operations.");
    $this->assertContains('=', $retrieved,
      "We expected '=' to be included in the default operations but it was not.");
    $propertyType->setOperations(['A', 'B', 'C']);
    $retrieved = $propertyType->getOperations();
    $this->assertIsArray($retrieved, "We were not able to retrieve the operations we just set.");
    $this->assertCount(3, $retrieved,
      "We set only 3 operations so that is what we expect to be able to retrieve.");
    // -- sortable
    $retrieved = $propertyType->getSortable();
    $this->assertEquals(TRUE, $retrieved, "We were not able to retrieve the default sortability.");
    $propertyType->setSortable(FALSE);
    $retrieved = $propertyType->getSortable();
    $this->assertFalse($retrieved, "We were not able to retrieve the sortability we just set.");
    // -- read only
    $retrieved = $propertyType->getReadOnly();
    $this->assertEquals(FALSE, $retrieved, "We were not able to retrieve the default read only value.");
    $propertyType->setReadOnly(TRUE);
    $retrieved = $propertyType->getReadOnly();
    $this->assertTrue($retrieved, "We were not able to retrieve the read only we just set.");
    // -- required
    $retrieved = $propertyType->getRequired();
    $this->assertEquals(FALSE, $retrieved, "We were not able to retrieve the default required setting.");
    $propertyType->setRequired(TRUE);
    $retrieved = $propertyType->getRequired();
    $this->assertTrue($retrieved, "We were not able to retrieve the required setting we just set.");
  }

  /**
   * Tests the default implementation of Tripal PropertyTypes.
   *
   * Specifically:
   *  - BoolStoragePropertyType
   *  - DateTimeStoragePropertyType
   *  - IntStoragePropertyType
   *  - RealStoragePropertyType
   *  - TextStoragePropertyType
   *  - VarCharStoragePropertyType
   */
  public function testPropertyTypes() {

    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakePropertyTypeKey';
    $term_id = 'mock:term';
    $storage_settings = ['put something in here' => 'so that we know its been retrieved'];

    // BoolStoragePropertyType
    $type = 'BoolStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\BoolStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    // DateTimeStoragePropertyType
    $type = 'DateTimeStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\DateTimeStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    // IntStoragePropertyType
    $type = 'IntStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\IntStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    // RealStoragePropertyType
    $type = 'RealStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\RealStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    // TextStoragePropertyType
    $type = 'TextStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\TextStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

      // VarCharStoragePropertyType
    $type = 'VarCharStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\VarCharStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
  }

  /**
   * Tests extra functionality associated with varchar property types.
   */
  public function testVarCharStoragePropertyType() {

    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakePropertyTypeKey';
    $term_id = 'mock:term';
    $id = 'FAKEStoragePropertyType';

    // Check the default max char size.
    $type = 'VarCharStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\VarCharStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    $retrieved = $propertyType->getMaxCharacterSize();
    $this->assertIsInt($retrieved, "We did not get an integer when trying to access the default max char size.");
    $this->assertEquals(255, $retrieved,
      "We did not retrieve the expected default max char size.");

    // Check a non-default max char size.
    $type = 'VarCharStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new \Drupal\tripal\TripalStorage\VarCharStoragePropertyType($entityType, $fieldType, $key, $term_id, 333);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    $retrieved = $propertyType->getMaxCharacterSize();
    $this->assertIsInt($retrieved, "We did not get an integer when trying to access the default max char size.");
    $this->assertEquals(333, $retrieved,
      "We did not retrieve the expected max char size based on what we passed in during creation.");

  }
}
