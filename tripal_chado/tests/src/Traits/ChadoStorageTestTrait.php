<?php
namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

use Symfony\Component\Yaml\Yaml;

/**
 * Provides functions and member variables to be used when testing Chado Storage.
 * This allows for less duplication of setup and more focus on the particular
 * use cases within the test classes themselves.
 *
 * How to write kernel tests for a given field:
 * 1. Create a test class where the name of the class is the same as the field
 *    but with "Test" appended to the end. This class should extend
 *    ChadoTestKernelBase.
 * 2. The first line in your class should be to use this trait
 *      use ChadoStorageTestTrait;
 * 3. Define the fields and properties for the field you are testing using the
 *    $fields protected variable. See the format for this variable below.
 */
trait ChadoStorageTestTrait {

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected object $chado_connection;

  /**
   * A dummy Tripal Term for use whereever chado storage needs one.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected object $mock_term;

  /**
   * A ChadoStorage object to run your tests on.
   *
   * @var \Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage
   */
  protected object $chadoStorage;

  /**
   * A nested array describing the fields and properties to be tested.
   * This will be used by the trait methods during setup and also
   * makes tests easier to read.
   *
   * @var array With a structure as follows:
   *  (field name) => [
   *    'field_name' => (field name),
   *    'base_table' => (base chado table for this field),
   *  ]
   */
  // protected array $fields;
  // DEFINED IN TEST CLASSES. protected array $fields = [];

  /**
   * An array of FieldConfig mock objects
   * for use with ChadoStorage::*Values() methods.
   *
   * @var array where the key is the field name
   *  and the value is a mock FieldConfig object.
   */
  protected array $fieldConfig_mock = [];

  /**
   * An array of propertyType objects initialized based on the $fields
   * properties array.
   *
   * @var array
   *   This is an array of property types 3 levels deep:
   *     The 1st level is the bundle name (e.g. bio_data_1).
   *     The 2st level is the field name (e.g. ChadoOrganismDefault).
   *     The 3rd level is the property key => PropertyType object
   */
  protected array $propertyTypes = [];

  /**
   * An array of propertyValue objects initialized based on the $fields
   * properties array.
   *
   * @var array
   */
  protected array $propertyValues = [];

  /**
   * An array for testing ChadoStorage::*Values methods for the current fields.
   * This is an associative array 5-levels deep.
   *    The 1st level is the field name (e.g. ChadoOrganismDefault).
   *    The 2nd level is the delta value (e.g. 0).
   *    The 3rd level is a field key name (i.e. record_id + value).
   *    The 4th level must contain the following three keys/value pairs
   *      - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *      - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *      - "definition": a \Drupal\Field\Entity\FieldConfig object
   *
   * @var array
   */
  protected array $dataStoreValues;

  /**
   * The machine name of the content type we are pretending to have
   * attached these fields to.
   *
   * Note: entity_test is defined by core Drupal for use in testing
   *
   * @var string
   */
  protected string $content_type = 'entity_test';

  /**
   * The term string (ID Space + Accession) to use with the mock term
   * for tests where the TripalTerm does not need to be unique.
   *
   * @var string
   */
  protected string $term_string = 'rdfs:type';

  /**
   * The ID of our test entity.
   * NOTE: Right now this is just made up. We'll see if that matters during testing.
   *
   * @var integer
   */
  protected int $content_entity_id = 1;

  /**
   * Setup mocks and services used when testing chado storage.
   *
   * NOTE: This is meant to be called in the setUp() method of your tests
   * and is restricted to use with KERNEL TESTS.
   */
  protected function setUpChadoStorageTestEnviro() :void {

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Grab the container.
    $container = \Drupal::getContainer();

    // Create a new test schema for us to use.
    $this->chado_connection = $this->createTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // ChadoStorage has terms associated with each property of a field.
    // These terms will be used by web services, etc but are not directly
    // used by ChadoStorage. As such, we can create a very basic mock term
    // for use everywhere a term is needed. Specifically, we are using a
    // Dummy class made by the mock builder which means if any methods are
    // called on it, they will return NULL.
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

    $this->cleanChadoStorageValues();
  }

  /**
   * Tests the insertValues including creating property types and values.
   *
   * All fields and properties referenced in the values parameter
   * must be defined in the $fields array.
   *
   * Currently tests ChadoStorage addTypes(), getTypes(), insertValues()
   * methods for the fields defined and the values provided.
   *
   * Assertions test that:
   * - For each newly created property type described in $fields the result is
   *   an object that is an instance of the StoragePropertyTypeBase class
   *   (within createPropertyTypes).
   * - At the end of createPropertyTypes() the number of properties created
   *   matches the number of properties expected based on the fields array.
   * - ChadoStorage getTypes() returns an array with the same number of entries
   *   as the array we passed into addTypes() (within addPropertyTypes2ChadoStorage).
   * - We were able to create mock field config objects for use with the
   *   loadValues() (within createDataStoreValues)
   * - For each newly created property value based on the values array,
   *   we were able to create an object of type StoragePropertyValue
   *   with no default value set (within createDataStoreValues).
   * - That for each field, we had the expected number of values in our data
   *   store values array after creating the property values above
   *   (within createDataStoreValues).
   * - That at the end of createDataStoreValues we have the expected number of
   *   fields in our data store values array (within createDataStoreValues).
   * - That we were able to use getValue() on each property value object with a
   *   default value described in the expectations array to retrieve the same
   *   value we set using setValue() (within setExpectedValues).
   * - That we were able to call insertValues() with our prepare data store
   *   values array without it returning an error.
   *
   * @param array $values
   *    A nested array with the following format:
   *   [
   *     <field name> => [
   *       <delta> => [
   *         <propertykey> => <value>,
   *         ...
   *       ],
   *     ],
   *   ]
   */
  protected function chadoStorageTestInsertValues(array $values) {

    // Get the list of fields we are testing from the values array.
    $field_names = array_keys($values);

    // Count total number of properties expected for the fields in the
    // values array we are testing.
    $expected_property_counts = [
      'total' => 0,
    ];
    foreach ($field_names as $field_name) {
      $this->assertArrayHasKey($field_name, $this->fields,
        "The \$fields array is malformed. '$field_name' must exist as a key in the top level of the array.");
      $this->assertArrayHasKey('properties', $this->fields[$field_name],
        "The \$fields array is malformed. '$field_name' array must have a 'properties' key which defines all the properties for this field.");

      $expected_property_counts['total'] += count($this->fields[$field_name]['properties']);
      $expected_property_counts[$field_name] = count($this->fields[$field_name]['properties']);
    }

    // Create the property types based on our fields array.
    $this->createPropertyTypes($field_names, $expected_property_counts);

    // Add the types to chado storage.
    $this->addPropertyTypes2ChadoStorage($field_names, $expected_property_counts);

    // Create the property values + format them for testing with *Values methods.
    $this->createDataStoreValues($field_names, $values);

    // Set the values in the propertyValue objects.
    $this->setExpectedValues($field_names, $values);

    // $this->debugChadoStorageTestTraitArrays();

    $success = $this->chadoStorage->insertValues($this->dataStoreValues);
    $this->assertTrue($success, 'We were not able to insert the data.');
  }

  /**
   * Tests the loadValues including creating property types and values.
   *
   * All fields and properties referenced in the values parameter
   * must be defined in the $fields array.
   *
   * Currently tests ChadoStorage addTypes(), getTypes(), loadValues()
   * methods for the fields defined and the values provided.
   *
   * Assertions test that:
   * - For each newly created property type described in $fields the result is
   *   an object that is an instance of the StoragePropertyTypeBase class
   *   (within createPropertyTypes).
   * - At the end of createPropertyTypes() the number of properties created
   *   matches the number of properties expected based on the fields array.
   * - ChadoStorage getTypes() returns an array with the same number of entries
   *   as the array we passed into addTypes() (within addPropertyTypes2ChadoStorage).
   * - We were able to create mock field config objects for use with the
   *   loadValues() (within createDataStoreValues)
   * - For each newly created property value based on the values array,
   *   we were able to create an object of type StoragePropertyValue
   *   with no default value set (within createDataStoreValues).
   * - That for each field, we had the expected number of values in our data
   *   store values array after creating the property values above
   *   (within createDataStoreValues).
   * - That at the end of createDataStoreValues we have the expected number of
   *   fields in our data store values array (within createDataStoreValues).
   * - That we were able to use getValue() on each property value object with a
   *   default value described in the expectations array to retrieve the same
   *   value we set using setValue() (within setExpectedValues).
   * - That we were able to call loadValues() with our prepare data store
   *   values array without it returning an error.
   *
   * @param array $values
   *    A nested array with the following format:
   *   [
   *     <field name> => [
   *       <delta> => [
   *         <propertykey> => <value>,
   *         ...
   *       ],
   *     ],
   *   ]
   *
   * @return array
   *   An associative array 5-levels deep.
   *    The 1st level is the field name (e.g. ChadoOrganismDefault).
   *    The 2nd level is the delta value (e.g. 0).
   *    The 3rd level is a field key name (e.g. record_id + value).
   *    The 4th level must contain the following three keys/value pairs
   *      - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *      - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *      - "definition": a \Drupal\Field\Entity\FieldConfig object
   */
  protected function chadoStorageTestLoadValues(array $values) {

    // Get the list of fields we are testing from the values array.
    $field_names = array_keys($values);

    // Count total number of properties expected for the fields in the
    // values array we are testing.
    $expected_property_counts = [
      'total' => 0,
    ];
    foreach ($field_names as $field_name) {
      $this->assertArrayHasKey($field_name, $this->fields,
        "The \$fields array is malformed. '$field_name' must exist as a key in the top level of the array.");
      $this->assertArrayHasKey('properties', $this->fields[$field_name],
        "The \$fields array is malformed. '$field_name' array must have a 'properties' key which defines all the properties for this field.");

      $expected_property_counts['total'] += count($this->fields[$field_name]['properties']);
      $expected_property_counts[$field_name] = count($this->fields[$field_name]['properties']);
    }

    // Create the property types based on our fields array.
    $this->createPropertyTypes($field_names, $expected_property_counts);
    // Add the types to chado storage.
    $this->addPropertyTypes2ChadoStorage($field_names, $expected_property_counts);

    // Create the property values + format them for testing with *Values methods.
    $this->createDataStoreValues($field_names, $values);
    // Set the values in the propertyValue objects.
    $this->setExpectedValues($field_names, $values);

    $success = $this->chadoStorage->loadValues($this->dataStoreValues);
    $this->assertTrue($success, 'We were not able to load the data.');

    return $this->dataStoreValues;
  }


  /**
   * Tests the updateValues including creating property types and values.
   *
   * All fields and properties referenced in the values parameter
   * must be defined in the $fields array.
   *
   * Currently tests ChadoStorage addTypes(), getTypes(), updateValues()
   * methods for the fields defined and the values provided.
   *
   * Assertions test that:
   * - For each newly created property type described in $fields the result is
   *   an object that is an instance of the StoragePropertyTypeBase class
   *   (within createPropertyTypes).
   * - At the end of createPropertyTypes() the number of properties created
   *   matches the number of properties expected based on the fields array.
   * - ChadoStorage getTypes() returns an array with the same number of entries
   *   as the array we passed into addTypes() (within addPropertyTypes2ChadoStorage).
   * - We were able to create mock field config objects for use with the
   *   loadValues() (within createDataStoreValues)
   * - For each newly created property value based on the values array,
   *   we were able to create an object of type StoragePropertyValue
   *   with no default value set (within createDataStoreValues).
   * - That for each field, we had the expected number of values in our data
   *   store values array after creating the property values above
   *   (within createDataStoreValues).
   * - That at the end of createDataStoreValues we have the expected number of
   *   fields in our data store values array (within createDataStoreValues).
   * - That we were able to use getValue() on each property value object with a
   *   default value described in the expectations array to retrieve the same
   *   value we set using setValue() (within setExpectedValues).
   * - That we were able to call updateValues() with our prepare data store
   *   values array without it returning an error.
   *
   * @param array $values
   *    A nested array with the following format:
   *   [
   *     <field name> => [
   *       <delta> => [
   *         <propertykey> => <value>,
   *         ...
   *       ],
   *     ],
   *   ]
   *
   * @return array
   *   An associative array 5-levels deep.
   *    The 1st level is the field name (e.g. ChadoOrganismDefault).
   *    The 2nd level is the delta value (e.g. 0).
   *    The 3rd level is a field key name (e.g. record_id + value).
   *    The 4th level must contain the following three keys/value pairs
   *      - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *      - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *      - "definition": a \Drupal\Field\Entity\FieldConfig object
   */
  protected function chadoStorageTestUpdateValues(array $values) {

    // Get the list of fields we are testing from the values array.
    $field_names = array_keys($values);

    // Count total number of properties expected for the fields in the
    // values array we are testing.
    $expected_property_counts = [
      'total' => 0,
    ];
    foreach ($field_names as $field_name) {
      $this->assertArrayHasKey($field_name, $this->fields,
        "The \$fields array is malformed. '$field_name' must exist as a key in the top level of the array.");
      $this->assertArrayHasKey('properties', $this->fields[$field_name],
        "The \$fields array is malformed. '$field_name' array must have a 'properties' key which defines all the properties for this field.");

      $expected_property_counts['total'] += count($this->fields[$field_name]['properties']);
      $expected_property_counts[$field_name] = count($this->fields[$field_name]['properties']);
    }

    // Create the property types based on our fields array.
    $this->createPropertyTypes($field_names, $expected_property_counts);
    // Add the types to chado storage.
    $this->addPropertyTypes2ChadoStorage($field_names, $expected_property_counts);


    // Create the property values + format them for testing with *Values methods.
    $this->createDataStoreValues($field_names, $values);
    // Set the values in the propertyValue objects.
    $this->setExpectedValues($field_names, $values);

    $success = $this->chadoStorage->updateValues($this->dataStoreValues);
    $this->assertTrue($success, 'We were not able to update the data.');

    return $this->dataStoreValues;
  }

  /**
   * Create PropertyType objects based on defined fields.
   *
   * Note: Includes assertions to be sure property types were created as
   * expected.
   *
   * @param array $field_names
   *   An array of field name keys as defined in the $fields array for which you would like
   *   to create propertyTypes for.
   * @param array $expected_property_counts
   *   An array summarizing the number of properties we expect to create.
   *   It consists of:
   *    'total' => the total number of properties across all fields/bundles)
   *    [field name] => the number of properties expected for this field
   *        ...
   */
  public function createPropertyTypes($field_names, $expected_property_counts) {

    // @todo find a better way than hard-coding this
    $test_term_string = 'rdfs:type';

    // Count total number of properties created.
    $expected_total_properties = $expected_property_counts['total'];
    $total_properties_created = 0;

    // For each of the fields we were asked to create properties...
    $total_num_properties = 0;
    foreach ($field_names as $field_name) {

      // Grab the defaults from the fields array.
      $field_details = $this->fields[$field_name];

      // Then for each defined property, we will create it and check it.
      foreach ($field_details['properties'] as $property_key => $property_options) {

        // Get the property type class we should be using to create our property.
        $propertyTypeClass = $property_options['propertyType class'];

        // Note: varchar properties have an extra parameter so must be handled
        // separately. We use ends with since the property class should include
        // the namespace.
        if (str_ends_with($propertyTypeClass, 'CharStoragePropertyType')) {
          $type = new $propertyTypeClass(
            $this->content_type,
            $field_name,
            $property_key,
            $test_term_string,
            255,
            $property_options
          );
        }
        else {
          $type = new $propertyTypeClass(
            $this->content_type,
            $field_name,
            $property_key,
            $test_term_string,
            $property_options
          );
        }
        $this->assertIsObject(
          $type,
          "Unable to create $property_key property type: not an object."
        );
        $this->assertInstanceOf(
          StoragePropertyTypeBase::class,
          $type,
          "Unable to create $property_key property type: does not inherit from StoragePropertyTypeBase."
        );

        // Set it in the protected propertyTypes array for use in tests.
        $this->propertyTypes[$field_name][$property_key] = $type;
        $total_num_properties++;
      }

      $this->assertCount($expected_property_counts[$field_name], $this->propertyTypes[$field_name],
        "We did not have the expected number of property types created for $field_name.");
    }

    $this->assertEquals($expected_property_counts['total'], $total_num_properties,
      "We did not have the expected number of property types created on our behalf.");
  }

  /**
   * Create PropertyValue objects and return in the array structure needed
   * to test ChadoStorage::*Values functions.
   *
   * NOTE: You must have called createPropertyTypes() for this field first.
   *
   * All fields and properties referenced in the values parameter
   * must be defined in the $fields array.
   *
   * @param array $field_names
   *   An array of field name keys as defined in the $fields array for which you would like
   *   to create propertyTypes for.
   * @param array $values
   *   A nested array with the following format:
   *   [
   *     <field name> => [
   *       <delta> => [
   *         <propertykey> => <value>,
   *         ...
   *       ],
   *     ],
   *   ]
   */
  public function createDataStoreValues(array $field_names, array $values) {

    $num_fields = count($field_names);

    foreach ($field_names as $field_name) {
      $this->dataStoreValues[$field_name] = [];

      $this->assertArrayHasKey($field_name, $this->fieldConfig_mock,
        "We expected there to already be a mock field config for $field_name but there was not.");

      $num_values = count($values[$field_name]);

      foreach ($values[$field_name] as $delta => $current_values) {
        $this->dataStoreValues[$field_name][$delta] = [];
        foreach ($this->fields[$field_name]['properties'] as $property_key => $property_options) {

          // Create the propertyValue.
          $this->propertyValues[$property_key] = new StoragePropertyValue(
            $this->content_type,
            $field_name,
            $property_key,
            $this->term_string,
            $this->content_entity_id
          );
          $this->assertArrayHasKey($property_key, $this->propertyValues,
            "We were unable to create/set the property value for $field_name.$property_key.");
          $this->assertIsObject($this->propertyValues[$property_key],
            "Unable to create $property_key property type: not an object.");
          $this->assertInstanceOf(StoragePropertyValue::class, $this->propertyValues[$property_key],
            "Unable to create $property_key property type: does not inherit from StoragePropertyValue.");
          $this->assertTrue( empty($this->propertyValues[$property_key]->getValue()),
            "The $field_name $property_key property should not have a value.");

          $this->assertArrayHasKey($property_key, $this->propertyTypes[$field_name],
            "We expected there to already be a property type for $field_name.$property_key but there was not.");

          // Add the property Value + Type + Field Config to the values array.
          $this->dataStoreValues[$field_name][$delta][$property_key] = [
            'value' => $this->propertyValues[$property_key],
          ];
        }
        $this->assertCount(sizeof($this->fields[$field_name]['properties']), $this->dataStoreValues[$field_name][$delta],
          "There was a different number of property arrays for $field_name\[$delta\] than expected in our dataStoreValues.");
      }

      $this->assertCount($num_values, $this->dataStoreValues[$field_name],
        "There was a different number of delta for $field_name field than expected in our dataStoreValues.");
    }

    $this->assertCount($num_fields, $this->dataStoreValues,
      "There was a different number of fields in our dataStoreValues then we expected.");
  }

  /**
   * Add Types to ChadoStorage + check.
   *
   * Note: Includes assertions to be sure property types were added as
   * expected.
   *
   * @param string $field_names
   *   An array of field name keys as defined in the $fields array for which
   *   propertyTypes fohave already been created for.
   * @param array $expected_property_counts
   *   An array summarizing the number of properties we expect to create.
   *   It consists of:
   *    'total' => the total number of properties across all fields/bundles)
   *    [field name] => the number of properties expected for this field
   *        ...
   */
  public function addPropertyTypes2ChadoStorage($field_names, $expected_property_counts) {

    foreach ($this->propertyTypes as $field_name => $properties) {
      $this->chadoStorage->addTypes($field_name, $properties);
    }
    $retrieved_types = $this->chadoStorage->getTypes();

    $field_name_string = implode(' + ', $field_names);
    $this->assertIsArray($retrieved_types,
      "Unable to retrieve the PropertyTypes after adding $field_name_string.");
    $total_num_properties = 0;
    foreach ($retrieved_types as $field_name => $retrieved_properties) {
      $this->assertCount($expected_property_counts[$field_name], $retrieved_properties,
        "Did not revieve the expected number of PropertyTypes for $field_name after adding $field_name_string.");
      $total_num_properties += count($retrieved_properties);
    }
    $this->assertEquals($expected_property_counts['total'], $total_num_properties,
      "Did not retrieve the expected number of PropertyTypes after adding $field_name_string.");
  }

  /**
   * Set expected values from data provider in the dataStoreValues propertyValue objects.
   *
   * @param array $field_names
   *   A simple array of all the field names in the expectations array. This is
   *   required because there are some non-field name keys in the expectations
   *   array.
   *
   * All fields and properties referenced in the values parameter
   * must be defined in the $fields array.
   *
   * @param array $values
   *   A nested array with the following format:
   *   [
   *     <field name> => [
   *       <delta> => [
   *         <propertykey> => <value>,
   *         ...
   *       ],
   *     ],
   *   ]
   */
  public function setExpectedValues($field_names, $values) {

    foreach ($field_names as $field_name) {
      foreach($values[$field_name] as $delta => $current_values) {
        foreach($current_values as $property_key => $val) {

          $this->assertArrayHasKey($property_key, $this->dataStoreValues[$field_name][$delta],
            "The key $property_key does not exist in the data store values for ".$field_name."[".$delta."], it may be missing from your \$fields definition");

          $this->dataStoreValues[$field_name][$delta][$property_key]['value']->setValue($val);

          // @debug print "SETTING $field_name [ $delta ] $property_key: $val\n";

          $retrieved_val = $this->dataStoreValues[$field_name][$delta][$property_key]['value']->getValue();
          $this->assertEquals($val, $retrieved_val,
            "We were unable to retrieve the value for $property_key right after we set it.");
        }
      }
    }
  }

  /**
   * Empty out all property types, proeprty values and data store values.
   * This should always be called between two test cases in the same test
   * method to ensure you are starting fresh and seeing dependant interactions.
   */
  protected function cleanChadoStorageValues() {
    $this->propertyTypes = [];
    $this->propertyValues = [];
    $this->dataStoreValues = [];

    // Get plugin managers we need for our testing.
    $storage_manager = \Drupal::service('tripal.storage');
    $this->chadoStorage = $storage_manager->createInstance('chado_storage');

    // There are some values we need setup for each field.
    // Each test implmenting this trait should have a $fields variable defined
    // which we will use here to cater our mocks to each use case.
    foreach ($this->fields as $field_name => $field_details) {

      // We also need a FieldConfig object for *Values() methods.
      $fieldConfig_mock = $this->createMock(\Drupal\field\Entity\FieldConfig::class);
      $fieldConfig_mock->method('getSettings')
        ->willReturn([
            'storage_plugin_id' => 'chado_storage',
            'storage_plugin_settings' => [
              'base_table' => $field_details['base_table'],
            ],
        ]);
      $fieldConfig_mock->method('getLabel')
        ->willReturn($field_name);
      $this->chadoStorage->addFieldDefinition($field_name, $fieldConfig_mock);
      $this->fieldConfig_mock[$field_name] = $fieldConfig_mock;
    }
  }
  /**
   * DEBUGGING USE ONLY: Prints out a bunch of debugging information.
   */
  public function debugChadoStorageTestTraitArrays() {

    print "\n\nDEBUGGING:\n";

    print "\tData Store Values:\n";

    foreach ($this->dataStoreValues as $field_name => $level1) {
      print "\n\tFIELD: $field_name:\n";
      foreach ($level1 as $delta => $level2) {
        print "\t\tDelta: $delta\n";
        foreach ($level2 as $property_key => $level3) {
          print "\t\t\tProperty: $property_key\n";

          $val = $level3['value']->getValue();
          if ($val) {
            print "\t\t\t\tValue: '$val'.\n";
          }

        }
      }
    }

    print "\n\n";

  }

  /**
   * Allows you to set the 'fields' by specifying the top level key of a YAML file.
   *
   * @param string $yaml_file
   *   The full path to a yaml file which follows the format descripbed above.
   * @param string $top_level_key
   *   The top level key in the yaml file which contains the fields you would
   *   like to set.
   */
  public function setFieldsFromYaml($yaml_file, $top_level_key) {

    if (!file_exists($yaml_file)) {
      throw new \Exception("Cannot open YAML file $yaml_file in order to set the fields for testing chadostorage.");
    }

    $file_contents = file_get_contents($yaml_file);
    if (empty($file_contents)) {
      throw new \Exception("Unable to retrieve contents for YAML file $yaml_file in order to set the fields for testing chadostorage.");
    }

    $yaml_data = Yaml::parse($file_contents);
    if (empty($yaml_data)) {
      throw new \Exception("Unable to parse YAML file $yaml_file in order to set the fields for testing chadostorage.");
    }

    // Check if we have a single top level key or if there are more levels.
    $levels = explode('.', $top_level_key);
    $data2return = $yaml_data;
    $deepest_level = max(array_keys($levels));
    foreach($levels as $i => $key) {
      if (!array_key_exists($key, $data2return)) {
        throw new \Exception("The key $key (part of $top_level_key) that you provided does not exist in the parsed YAML file: $yaml_file.");
      }


      if ($i === $deepest_level AND $i !== 0) {
        // If this is the deepest level and not the only level then we want to
        // do something different to ensure we keep the structure of the fields array.
        $data2return = [ $key => $data2return[$key] ];
      }
      else {
        $data2return = $data2return[$key];
      }
    }

    $this->fields = $data2return;
  }
}
