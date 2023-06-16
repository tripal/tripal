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
  protected $chado_connection;

  /**
   * A dummy Tripal Term for use whereever chado storage needs one.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected $mock_term;

  /**
   * A ChadoStorage object to run your tests on.
   *
   * @var \Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage
   */
  protected $chadoStorage;

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
  protected $propertyTypes = [];
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
          'label' => $field_name,
          'settings' => [
            'storage_plugin_id' => 'chado_storage',
            'storage_plugin_settings' => [
              'base_table' => $field_details['base_table'],
            ],
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
   * The key of the field defined in the $fields array for which you would like
   * to create propertyTypes for.
   */
  public function createPropertyTypes($field_name) {

    $content_type = 'entity_test';
    $test_term_string = 'rdfs:type';

    $field_details = $this->fields[$field_name];

    foreach ($field_details['properties'] as $property_key => $property_options) {
      $propertyTypeClass = $property_options['propertyType class'];
      $type = new $propertyTypeClass(
        $content_type,
        $field_name,
        $property_key,
        $test_term_string,
        $property_options
      );
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

}
