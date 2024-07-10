<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class TripalStorageTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * A dummy Tripal Term for use whereever tripal storage needs one.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected object $mock_term;

  /**
   * A dummy tripal logger. This exists to ensure that nothing is written to the
   * PHP error_log as that causes a PHPUnit exception. Instead this mock will
   * always print the message to the screen.
   */
  protected object $mock_logger;

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

    // Some of our tests will check logged messages that would normally go to
    // php error_log. PHPUnit will throw an exception if anything is added to
    // error_log so we want to mock TripalLogger to ensure all errors are printed
    // to the screen.
    // We only need to mock the error method. Other methods will not be mocked.
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['error'])
      ->getMock();
    $mock_logger->method('error')
      ->willReturnCallback(function($message, $context, $options) {
          print 'ERROR: ' . str_replace(array_keys($context), $context, $message);
          return NULL;
        });
    $container->set('tripal.logger', $mock_logger);

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
    $logger = \Drupal::service('tripal.logger');;
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

    // We also want to test trying to get a field definition that doesn't exist.
    // We expect this to return FALSE and not to throw an exception.
    $retrieved_defn = $tripalStorage->getFieldDefinition('aFieldWhichDoesntExist');
    $this->assertFalse($retrieved_defn, "We should not be able to retrieve a field definition for a field which does not exist.");

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
    $expected_class_type = [];
    foreach ($fields as $field_name) {

      $propertyTypeClass = $propertyTyleClass_namespace . array_pop($propertyTypeClasses);
      $expected_class_type[$field_name] = $propertyTypeClass;

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

        // We expect the key to have been sanitized so lets do that quickly.
        // Otherwise we won't be able to check retrieved properties
        // against those we passed in.
        $sanitized_key = preg_replace('/[^\w]/', '_', $key);
        $expected_types[$field_name][$sanitized_key] = $type;
      }

      // Types are added on a per field basis, so lets do that now.
      $tripalStorage->addTypes($field_name, $expected_types[$field_name]);
    }

    // Also test that if we try to add a type that is not an object
    $bad_test_properties = [
      'NotClass' => 'fred really wanted to be a property type but alas he was a string',
    ];
    ob_start();
    $tripalStorage->addTypes('fieldWithBadPropertyTypes', $bad_test_properties);
    $printed_output = ob_get_clean();
    $this->assertMatchesRegularExpression('/ERROR.*must be an object.*NotClass/', $printed_output,
      "We expected an error to be printed saying that 'NotClass' was not an object.");
    $this->assertDoesNotMatchRegularExpression('/ERROR.*ERROR/', $printed_output,
      "We only expected a single error and yet we may have found multiple?");

    // or is not a propertyType object that we get an error.
    // We use Drupal\tripal\TripalStorage\StoragePropertyValue to check that it
    // is really specific.
    $propertyValueNotType = new \Drupal\tripal\TripalStorage\StoragePropertyValue(
      'tripal_entity',
      'name_all_underscores',
      'NotAPropertyType',
      'mock:term',
      'entity_test'
    );
    $bad_test_properties = [
      'NotAPropertyType' => $propertyValueNotType
    ];
    ob_start();
    $tripalStorage->addTypes('fieldWithBadPropertyTypes', $bad_test_properties);
    $printed_output = ob_get_clean();
    $this->assertMatchesRegularExpression('/ERROR.*StoragePropertyType.*NotAPropertyType/', $printed_output,
      "We expected an error to be printed saying that 'NotAPropertyType' was not a property type object.");
    $this->assertDoesNotMatchRegularExpression('/ERROR.*ERROR/', $printed_output,
      "We only expected a single error and yet we may have found multiple?");

    // Now we use the generic getTypes method to test that we can retrieve what we added.
    // Retrieved types should be of the form:
    // field_name -> property key -> property type object.
    $retrieved_types = $tripalStorage->getTypes();
    $expected_number_of_fields = count($fields);
    $this->assertCount($expected_number_of_fields, $retrieved_types,
      "We did not have the number of fields we expected assuming that the first level of keys in the array returned by getTypes() are fields.");
    foreach ($retrieved_types as $retrieved_field => $retrieved_field_proptypes) {
      $this->assertArrayHasKey($retrieved_field, $expected_types,
        "The field we retrieved ($retrieved_field) was not in the fields we expected.");
      $expected_count = count($expected_types[$retrieved_field]);
      $this->assertCount($expected_count, $retrieved_field_proptypes,
        "We did not have the expected number of entries for $retrieved_field.");

      foreach ($retrieved_field_proptypes as $retrieved_key => $retrieved_proptype) {
        $this->assertArrayHasKey($retrieved_key, $expected_types[$retrieved_field],
          "We expected the property '$retrieved_key' (sanitized from original) to be associated with the field ($retrieved_field)");

        $expected_object = $expected_types[$retrieved_field][$retrieved_key];
        $this->assertEquals($expected_object, $retrieved_proptype,
          "The object associated with $retrieved_field.$retrieved_key did not match the one we expected.");

        // We specifically chose to create a different type of propertyType for
        // each field so that we could check that properties with the same
        // key were being stored properly and not overridden.
        $expected_type = $expected_class_type[$retrieved_field];
        $this->assertInstanceOf($expected_type, $retrieved_proptype,
          "The type of propertyType we retrieved did not match what we expected.");
      }
    }

    // Now we want to check retrieving a specific property type.
    foreach ($expected_types as $field_name => $properties) {
      foreach ($properties as $property_key => $expected_property_object) {
        $retrieved_property_object = $tripalStorage->getPropertyType($field_name, $property_key);
        $this->assertIsObject($retrieved_property_object,
          "We should have had a property type object returned but did not.");
        $expected_type = $expected_class_type[$field_name];
        $this->assertInstanceOf($expected_type, $retrieved_property_object,
          "The retrieved property type object was not the type we expected.");
      }
    }

    // Also check that if we ask for a non-existant property type that we don't get one.
    $retrieved_property_object = $tripalStorage->getPropertyType('A field that definitely doesnt exist', 'also not a property that exists');
    $this->assertIsNotObject($retrieved_property_object,
      "We should not have had an object returned as the field/property type combo should not exist.");

    // Finally check that we can accurately remove property types.
    // We will remove a subset of types for a single field.
    $field_name = 'NameSnakeCase';
    $properties2remove = [
      'value' => $expected_types[$field_name]['value'],
      'pkey' => $expected_types[$field_name]['pkey'],
    ];
    $tripalStorage->removeTypes($field_name, $properties2remove);
    // Try to retrieve a property which should have been removed.
    $retrieved_property_object = $tripalStorage->getPropertyType($field_name, 'value');
    $this->assertIsNotObject($retrieved_property_object,
      "We should not have had an object returned as the $field_name.value should have been removed.");
    $retrieved_property_object = $tripalStorage->getPropertyType($field_name, 'pkey');
    $this->assertIsNotObject($retrieved_property_object,
      "We should not have had an object returned as the $field_name.pkey should have been removed.");
    // Also check that the remaining properties for that field do exist.
    $retrieved_types = $tripalStorage->getTypes();
    $this->assertArrayHasKey($field_name, $retrieved_types,
      "We should have had some properties remaining for $field_name after removing 2 but none were returned by getTypes().");
    $expected_count = count($expected_types[$field_name]) - 2;
    $this->assertCount($expected_count, $retrieved_types[$field_name],
      "We did not have the expected number of properties remaining after removing some.");
  }
}
