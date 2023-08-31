<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\KernelTests\KernelTestBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class TripalStorageTest extends KernelTestBase {

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
  }

  /**
   * Tests the add/get field definition functionality.
   */
  public function testTripalStorageBaseFieldDefn() {

    // To create a tripal storage object we will need the parameters required
    // for the constructor.
    $configuration = [];
    $plugin_id = 'fakePluginName';
    $plugin_definition = [];
    $logger = \Drupal::service('tripal.logger');
    // Tripal Storage Base is an abstract class.
    // Therefore, in order to test it we need to mock the abstract methods.
    $tripalStorage = $this->getMockForAbstractClass(
      'Drupal\tripal\TripalStorage\TripalStorageBase',
      [$configuration, $plugin_id, $plugin_definition, $logger]
    );
    $this->assertIsObject($tripalStorage, "Unable to create tripal storage mock object.");

    // This will be our set of fields to test.
    // We're checking there are no special assumptions about field names here.
    $fields = [
      'name_all_underscores' => NULL,
      'NameSnakeCase' => NULL,
      'Name with Spaces' => NULL,
      'name.with-slightly.special-chars' => NULL,
      'name!with+symbols' => NULL,
    ];

    // We also need a FieldConfig object for each field
    foreach ($fields as $field_name => $placeholder) {
      $fields[$field_name] = $this->createMock(\Drupal\field\Entity\FieldConfig::class);
      $fields[$field_name]->method('getLabel')
        ->willReturn($field_name);

      // Now add it to the storage
      $success = $tripalStorage->addFieldDefinition($field_name, $fields[$field_name]);
      $this->assertTrue($success, "add Field Definition did not return true for $field_name");
    }

    // Now that we've added all fields we want to show that we can
    // retrieve each field definition back out from storage as needed.
    foreach ($fields as $field_name => $expected_defn) {
      $retrieved_defn = $tripalStorage->getFieldDefinition($field_name);
      $this->assertIsObject($retrieved_defn, "Unable to retrieve an object when given $field_name.");
      $this->assertEquals($expected_defn, $retrieved_defn,
        "The retrieved definition did not match the original one we mocked for $field_name.");
    }

    // Check that if we alter a field definition
    // and reset it that we get the most recent one.
    $altered_mock = $fields['NameSnakeCase'];
    $altered_mock->method('getLabel')
        ->willReturn('NEW LABEL');
    $success = $tripalStorage->addFieldDefinition('NameSnakeCase', $altered_mock);
    $this->assertTrue($success, "add Field Definition did not return true for NameSnakeCase (second time)");
    $retrieved_defn = $tripalStorage->getFieldDefinition('NameSnakeCase');
    $this->assertIsObject($retrieved_defn, "Unable to retrieve an object when given NameSnakeCase (second time).");
    $this->assertEquals($altered_mock, $retrieved_defn,
      "The retrieved definition did not match the one we altered for NameSnakeCase (second time).");
  }

  /**
   * Tests the add/get property type functionality.
   */
  public function testTripalStorageBasePropTypes() {

    // To create a tripal storage object we will need the parameters required
    // for the constructor.
    $configuration = [];
    $plugin_id = 'fakePluginName';
    $plugin_definition = [];
    $logger = \Drupal::service('tripal.logger');
    // Tripal Storage Base is an abstract class.
    // Therefore, in order to test it we need to mock the abstract methods.
    $tripalStorage = $this->getMockForAbstractClass(
      'Drupal\tripal\TripalStorage\TripalStorageBase',
      [$configuration, $plugin_id, $plugin_definition, $logger]
    );
    $this->assertIsObject($tripalStorage, "Unable to create tripal storage mock object.");

    // This will be our set of fields to test.
    // We're checking there are no special assumptions about field names here.
    $fields = [
      'name_all_underscores',
      'NameSnakeCase',
      'Name with Spaces',
      'name.with-slightly.special-chars',
      'name!with+symbols',
    ];

    // We want to use the same set of property keys for each field to confirm that
    // they will not be overridden.
    $property_keys = [
      'record_id',
      'value',
      'pkey',
      'a completely nonsense name',
      'one.with!some-symbols+special|chars',
    ];

    $propertyTyleClass_namespace = 'Drupal\tripal\TripalStorage\\';
    $propertyTypeClasses = ['BoolStoragePropertyType', 'DateTimeStoragePropertyType',
      'IntStoragePropertyType', 'RealStoragePropertyType', 'TextStoragePropertyType'];

    $expected_types = [];
    foreach ($fields as $field_name) {

      $propertyTypeClass = $propertyTyleClass_namespace . array_pop($propertyTypeClasses);

      foreach ($property_keys as $key) {

        $type = new $propertyTypeClass(
          'entity_test',
          $field_name,
          $key,
          'rdfs:type',
          [],
        );
        $this->assertIsObject(
          $type,
          "Unable to create $field_name.$key property type: not an object."
        );
        $this->assertInstanceOf(
          StoragePropertyTypeBase::class,
          $type,
          "Unable to create $field_name.$key property type: does not inherit from StoragePropertyTypeBase."
        );

        $expected_types[$field_name][$key] = $type;
      }

      // Types are added on a per field basis,
      // so lets do that now.
      $tripalStorage->addTypes($field_name, $expected_types[$field_name]);
    }
  }
}
