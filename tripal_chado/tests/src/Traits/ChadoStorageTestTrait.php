<?php
namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

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
   * Note: Includes assertions to be sure property types were created as
   * expected.
   *
   * @param string $field_names
   *   An array of field name keys as defined in the $fields array for which you would like
   *   to create propertyTypes for.
   * @param int $expected_total_properties
   *   The total number of property types you expect to be created.
   */
  public function createPropertyTypes($field_names, $expected_total_properties) {

    // @todo currently these are hard coded but we should likely handle that differently.
    $content_type = 'entity_test';
    $test_term_string = 'rdfs:type';

    // For each of the fields we were asked to create properties...
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

        // Set it in the protected propertyTypes array for use in tests.
        $this->propertyTypes[$property_key] = $type;
      }
    }

    $this->assertCount($expected_total_properties, $this->propertyTypes,
      "We did not have the expected number of property types created on our behalf.");
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

  /**
   * Lookup Static variables provided by data providers.
   *
   * Data providers are a great way to test multiple combinations for a given field.
   * However, you cannot access services within a data provider. As such, you should
   * define any values needing lookup as static variables and then call this
   * function at the top of your test to have them looked up.
   *
   * You can expect this method to lookup a value based on its type definition
   * and then add it to the values array for that field as follows:
   *   $expectations[<name of field>]['values][<delta>][<property key] = <value looked up>
   *
   * Note: If a field has more then one delta then the same static values will
   * be set for each one.
   *
   * @param array $field_names
   *   A simple array of all the field names in the expectations array. This is
   *   required because there are some non-field name keys in the expectations
   *   array.
   *
   * @param $expectations
   *  This is expected to be provided to a test via a dataProvider. It should
   *  have the following format:
   *    [
   *      <name of field> => [
   *        ...
   *        'static values' => [
   *          <first static value definition>,
   *          <second static value definition>,
   *          ...
   *        ]
   *      ],
   *    ]
   *  Where a static value defininition depends on the type. The following
   *  types are supported:
   *    - Lookup a CVterm ID based on the ID Space and accession:
   *        [
   *          'type' => 'cvterm lookup',
   *          'idspace' => <ID Space>,
   *          'accession' => <accession>,
   *          'property_key' => <Key of the property to add to the field values>,
   *        ]
   *    - Provide the organism ID set in the setUp() method. For this one
   *      there must be a $organism_id protected variable.
   *        [
   *          'type' => 'organism',
   *          'property_key' => <Key of the property to add to the field values>,
   *        ]
   */
  public function lookupStaticValuesFromDataProvider(array $field_names, array &$expectations) {

    foreach ($field_names as $field_name) {
      foreach($expectations[$field_name]['static values'] as $args) {
        switch ($args['type']) {
          case 'organism':
            foreach(array_keys($expectations[$field_name]['values']) as $delta) {
              $expectations[$field_name]['values'][$delta][ $args['property_key'] ] = $this->organism_id;
            }
            break;
          case 'cvterm lookup':
            $cvterm_id = $this->getCvtermID($args['idspace'], $args['accession']);
            foreach(array_keys($expectations[$field_name]['values']) as $delta) {
              $expectations[$field_name]['values'][$delta][ $args['property_key'] ] = $cvterm_id;
            }
            break;
        }
      }
    }
  }
}
