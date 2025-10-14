<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\tripal\TripalStorage\RealStoragePropertyType;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\DateTimeStoragePropertyType;
use Drupal\tripal\TripalStorage\BoolStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
#[Group('storage-property')]
#[Group('tripal-storage')]
class PropertyTypeClassTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * A dummy Tripal Term for use where ever tripal storage needs one.
   *
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected object $mock_term;

  /**
   * A dummy Tripal ID Space for use where ever tripal storage needs one.
   *
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
    // here which will be pulled from the container any time a term is needed.
    $this->mock_term = $this->createMock(TripalTerm::class);
    // Create a mock ID space to return our mock term when asked.
    $this->mock_idspace = $this->createMock(TripalIdSpaceInterface::class);
    $this->mock_idspace->method('getTerm')
      ->willReturnCallback(function ($accession) {
        if ($accession == 'term') {
          return $this->mock_term;
        }
        else {
          return NULL;
        }
      });
    // Create a mock Tripal ID Space service to return our mock idspace.
    $mock_idspace_service = $this->createMock(TripalIdSpaceManager::class);
    $mock_idspace_service->method('loadCollection')
      ->willReturnCallback(function ($id_space) {
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
    $propertyType = new StoragePropertyTypeBase($entityType, $fieldType, $key, $term_id, $id, $storage_settings);

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

    // Now expand our tests to other methods that do not just access exactly
    // what we supplied.
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

    // --default value.
    $exception_caught = FALSE;
    $exception_message = 'NO EXCEPTION THROWN';
    try {
      $propertyType->getDefaultValue();
    }
    catch (\Exception $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, "We should get an exception thrown when trying to get the default value of a generic storage property type.");
    $this->assertStringContainsStringIgnoringCase('getDefaultValue() was not implemented', $exception_message, "We expect to be told this method was not implemented but should be.");
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
   *  - VarCharStoragePropertyType.
   */
  public function testPropertyTypes() {

    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakePropertyTypeKey';
    $term_id = 'mock:term';

    // BoolStoragePropertyType.
    $type = 'BoolStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new BoolStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
    $default = $propertyType->getDefaultValue();
    $this->assertFalse($default, "We expect the default value to be FALSE for the boolean property type.");

    // DateTimeStoragePropertyType.
    $type = 'DateTimeStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new DateTimeStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
    $default = $propertyType->getDefaultValue();
    $this->assertEquals('', $default, "We expect the default value to be an empty string for the datetime property type.");

    // IntStoragePropertyType.
    $type = 'IntStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new IntStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
    $default = $propertyType->getDefaultValue();
    $this->assertEquals(0, $default, "We expect the default value to be 0 for the int property type.");

    // RealStoragePropertyType.
    $type = 'RealStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new RealStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
    $default = $propertyType->getDefaultValue();
    $this->assertEquals(0, $default, "We expect the default value to be 0 for the real/float property type.");

    // TextStoragePropertyType.
    $type = 'TextStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new TextStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
    $default = $propertyType->getDefaultValue();
    $this->assertEquals('', $default, "We expect the default value to be an empty string for the text property type.");

    // VarCharStoragePropertyType.
    $type = 'VarCharStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new VarCharStoragePropertyType($entityType, $fieldType, $key, $term_id);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");
    $default = $propertyType->getDefaultValue();
    $this->assertEquals('', $default, "We expect the default value to be an empty string for the varchar property type.");
  }

  /**
   * Tests extra functionality associated with varchar property types.
   */
  public function testVarCharStoragePropertyType() {

    $entityType = 'tripal_entity';
    $fieldType = 'AFakeFieldType';
    $key = 'AFakePropertyTypeKey';
    $term_id = 'mock:term';

    // Check the default max char size.
    $type = 'VarCharStoragePropertyType';
    $instance = '\Drupal\tripal\TripalStorage\\' . $type;
    $propertyType = new VarCharStoragePropertyType($entityType, $fieldType, $key, $term_id);
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
    $propertyType = new VarCharStoragePropertyType($entityType, $fieldType, $key, $term_id, 333);
    $this->assertIsObject($propertyType, "We were not able to create an object for $type.");
    $this->assertInstanceOf($instance, $propertyType,
      "We created an object but it was not the type we expected.");

    $retrieved = $propertyType->getMaxCharacterSize();
    $this->assertIsInt($retrieved, "We did not get an integer when trying to access the default max char size.");
    $this->assertEquals(333, $retrieved,
      "We did not retrieve the expected max char size based on what we passed in during creation.");

  }

}
