<?php
namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

/**
 * Provides functions and member variables to be used
 * when testing Chado Storage. This allows for less
 * duplication of setup and more focus on the particular
 * use cases within the test classes themselves.
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
      $this->fieldConfig_mock[$field_name] = $fieldConfig_mock;
    }
  }

  /**
   * Create PropertyType objects based on defined fields.
   *
   * @param string $field_name
   *   The key of the field defined in the $fields array for which you would like
   *   to create propertyTypes for.
   */
  public function createPropertyTypes($field_name) {

    $content_type = 'entity_test';
    $test_term_string = 'rdfs:type';

    $field_details = $this->fields[$field_name];

    foreach ($field_details['properties'] as $property_key => $property_options) {
      $propertyTypeClass = $property_options['propertyType class'];
      if (str_ends_with($propertyTypeClass, 'ChadoVarCharStoragePropertyType')) {
        $type = new $propertyTypeClass(
          $content_type,
          $field_name,
          $property_key,
          $test_term_string,
          255,
          $property_options
        );
      }
      else {
        $type = new $propertyTypeClass(
          $content_type,
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
      $this->propertyTypes[$property_key] = $type;
    }
  }

  /**
   * Create PropertyValue objects and return in the array structure needed
   * to test ChadoStorage::*Values functions.
   *
   * NOTE: You must have called createPropertyTypes() for this field first.
   *
   * @param string $field_name
   *   The key of the field defined in the $fields array for which you would like
   *   to create propertyValues for.
   * @param integer $num_values
   *   The total number of values you expect to set for this field. For example,
   *   for a property field with 3 properties then this would be 3.
   */
  public function createDataStoreValues(string $field_name, int $num_values = 1) {
    $this->dataStoreValues[$field_name] = [];

    $this->assertArrayHasKey($field_name, $this->fieldConfig_mock,
      "We expected there to already be a mock field config for $field_name but there was not.");

    for ($delta = 0; $delta < $num_values; $delta++) {
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

        $this->assertArrayHasKey($property_key, $this->propertyTypes,
          "We expected there to already be a property type for $field_name.$property_key but there was not.");

        // Add the property Value + Type + Field Config to the values array.
        $this->dataStoreValues[$field_name][$delta][$property_key] = [
          'value' => $this->propertyValues[$property_key],
          'type' => $this->propertyTypes[$property_key],
          'definition' => $this->fieldConfig_mock[$field_name],
        ];
      }
      $this->assertCount(sizeof($this->fields[$field_name]['properties']), $this->dataStoreValues[$field_name][$delta],
        "There was a different number of property arrays for $field_name[$delta] than expected in our dataStoreValues.");
    }

    $this->assertCount($num_values, $this->dataStoreValues[$field_name],
      "There was a different number of delta for $field_name field than expected in our dataStoreValues.");
  }
}
