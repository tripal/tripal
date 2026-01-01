<?php

namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Core\Render\Element;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal\Traits\TripalEntityFieldTestTrait;
use Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage;

/**
 * Provides functions related to testing Chado Fields.
 */
trait ChadoFieldTestTrait {

  use UserCreationTrait;
  use TripalEntityFieldTestTrait;

  /**
   * An array of FieldStorageConfig objects keyed by the field name.
   *
   * @var \Drupal\field\Entity\FieldStorageConfig[]
   */
  protected array $fieldStorage = [];

  /**
   * An array of FieldConfig objects keyed by the field name.
   *
   * @var \Drupal\field\Entity\FieldConfig[]
   */
  protected array $fieldConfig = [];

  /**
   * An array of TripalEntityType objects keyed by the bundle name.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType[]
   */
  protected array $tripalEntityType = [];

  /**
   * An array of display objects keyed by Tripal Content Type bundle name.
   *
   * @var \Drupal\Core\Entity\Entity\EntityViewDisplay[]
   */
  protected array $entityViewDisplay = [];

  /**
   * An array of display objects keyed by Tripal Content Type bundle name.
   *
   * @var \Drupal\Core\Entity\Entity\EntityFormDisplay[]
   */
  protected array $entityFormDisplay = [];

  /**
   * A ChadoStorage object to run your tests on.
   *
   * @var \Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage
   */
  protected ChadoStorage $chadoStorage;

  /**
   * PropertyType objects initialized based on the $fields properties array.
   *
   * @var array
   *   This is an array of property types 3 levels deep:
   *     The 1st level is the bundle name (e.g. bio_data_1).
   *     The 2st level is the field name (e.g. ChadoOrganismDefault).
   *     The 3rd level is the property key => PropertyType object
   */
  protected array $propertyTypes = [];

  /**
   * PropertyValue objects initialized based on the $fields properties array.
   *
   * @var array
   */
  protected array $propertyValues = [];

  /**
   * An array for testing ChadoStorage::*Values methods for the current fields.
   *
   * This is an associative array 5-levels deep.
   *    The 1st level is the field name (e.g. ChadoOrganismDefault).
   *    The 2nd level is the delta value (e.g. 0).
   *    The 3rd level is a field key name (i.e. record_id + value).
   *    The 4th level must contain the following three keys/value pairs
   *      - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object.
   *      - "type": a \Drupal\tripal\TripalStorage\StoragePropertyType object.
   *      - "definition": a \Drupal\Field\Entity\FieldConfig object.
   *
   * @var array
   */
  protected array $dataStoreValues;

  /**
   * Confirms the widget element is as expected in the TripalEntity form.
   *
   * @param mixed $expected_field_defaults
   *   The default value we expect to be set in the widget form for each field
   *   defined in the fields_expected param.
   * @param array $fields_expected
   *   The system_under_test information for the fields whose widget elements we
   *   want to check.
   * @param array $form
   *   The full TripalEntity form array we want to check for the field widget
   *   elements in.
   * @param string $message_prefix
   *   A short string that all assert messages will be prefixed with.
   */
  public function assertFieldWidgetsMatch(mixed $expected_field_defaults, array $fields_expected, array $form, string $message_prefix = '') {

    // Test the widget form for each expected field.
    foreach ($fields_expected as $field_details) {
      $field_name = $field_details['name'];
      // Tester doesn't have to define all fields.
      if (array_key_exists($field_name, $expected_field_defaults)) {
        $expected_defaults = $expected_field_defaults[$field_name];

        // Check the form has an element for this field.
        $this->assertArrayHasKey($field_name, $form,
        $message_prefix . ": We expect the TripalEntity form to have an element $field_name.");
        $this->assertArrayHasKey('widget', $form[$field_name],
          $message_prefix . ": We expect the widget element for $field_name to have a widget key containing the widget form elements.");

        $widget_form_element = $form[$field_name]['widget'];

        // Check that the expected form element keys match.
        foreach (Element::children($widget_form_element) as $element_key) {
          $element = $widget_form_element[$element_key];
          // The tested does not have to check every element.
          if (array_key_exists($element_key, $expected_defaults)) {
            $expected_values = $expected_defaults[$element_key];

            // Check that there is an element for each expected value.
            foreach ($expected_values as $expected_key => $expected_value) {
              $this->assertArrayHasKey($expected_key, $element, $message_prefix . ": $field_name [$element_key] widget element should contain an element with this name.");
              $this->assertArrayContainsElements($expected_value, $element[$expected_key], $message_prefix . ": $field_name [$element_key] [$expected_key] did not contain the value(s) we expected.");
            }
          }
        }
      }
    }
  }

  /**
   * Recursive check that an array contains a set of elements.
   *
   * NOTE: Only the keys passed in will be checked. Additional keys will be
   * ignored. This is the difference between this method and assertEquals().
   *
   * @param array $expected_elements
   *   An array of expected keys mapped to their expected values.
   * @param iterable $haystack
   *   An array to check for the expectations.
   * @param string $message
   *   A message to provide if failure is encountered.
   */
  public function assertArrayContainsElements(array $expected_elements, iterable $haystack, string $message = ''): void {
    foreach ($expected_elements as $element_key => $element_value) {
      $this->assertArrayHasKey($element_key, $haystack, $message);
      if (is_array($element_value)) {
        $this->assertArrayContainsElements($element_value, $haystack[$element_key], $message);
      }
      else {
        $this->assertEquals($element_value, $haystack[$element_key], $message);
      }
    }
  }

  /**
   * Called in kernel test setUp() to ensure needed resources are available.
   *
   * @param array $system_under_test
   *   An array defining the environment to setup with the following keys:
   *   - chado_version: the version of chado to test under.
   *   - bundle: an array defining the tripal entity type to create.
   *   - fields: a list of fields to be attached the above bundle.
   *
   * @return array
   *   A list containing first the TripalEntityType object created and then an
   *   array of FieldConfig objects keyed by the associated field name. If the
   *   system-under-test was not provided then this will be an empty array.
   */
  public function setupChadoEntityFieldTestEnvironment(array $system_under_test = []): array {

    // First setup everything for a vanilla Tripal entity/field test. Don't
    // provide the system under test as we want to set that part up ourselves.
    $this->setupEntityFieldTestEnvironment();

    // Add in what is needed for chado fields specifically.
    $this->prepareEnvironment(['ChadoField']);
    // -- we need the chado term mapping for our properties.
    $this->installEntitySchema('chado_term_mapping');
    // -- we need access to the core term mappings.
    $rebuild_service = \Drupal::service('tripal_chado.rebuild_service');
    $rebuild_service->rebuildChadoTermMappings();

    // If information about the environment to be setup was provided, then we
    // will set it up for them :-).
    if (!empty($system_under_test)) {
      return $this->setupChadoEntityFieldSystemUnderTest($system_under_test);
    }

    return [];
  }

  /**
   * Setup the test environment according to the details provided.
   *
   * @param array $system_under_test
   *   An array defining the environment to setup with the following keys:
   *   - chado_version: the version of chado to test under.
   *   - bundle: an array defining the tripal entity type to create.
   *   - fields: a list of fields to be attached the above bundle.
   *
   * @return array
   *   A list containing first the TripalEntityType object created and then an
   *   array of FieldConfig objects keyed by the associated field name.
   */
  public function setupChadoEntityFieldSystemUnderTest(array $system_under_test): array {

    // 1. Create any needed collections. Collections for the bundle get
    // created automatically, but not those for fields.
    if (array_key_exists('termIdSpace', $system_under_test)) {
      $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      foreach ($system_under_test['termIdSpace'] as $termIdSpace) {
        $idsmanager->createCollection($termIdSpace, "chado_id_space");
      }
    }
    if (array_key_exists('termVocab', $system_under_test)) {
      $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
      foreach ($system_under_test['termVocab'] as $termVocab) {
        $vmanager->createCollection($termVocab, "chado_vocabulary");
      }
    }

    // 2. Create the bundle.
    $bundle = $this->createTripalContentType($system_under_test['bundle']);
    $bundle->setThirdPartySetting('tripal', 'chado_base_table', $system_under_test['bundle']['settings']['chado_base_table']);
    $bundle->save();
    $bundle_name = $bundle->id();

    // 3. Create the fields.
    $fields = [];
    foreach ($system_under_test['fields'] as $field_details) {

      // Create both the FieldConfig and FieldStorageConfig.
      $fields[$field_details['name']] = $this->createFieldInstance(
        'tripal_entity',
        [
          'field_name' => $field_details['name'],
          'bundle_name' => $bundle_name,
          'field_type' => $field_details['type'],
          'widget_id' => $field_details['widget'],
          'formatter_id' => $field_details['formatter'],
          'cardinality' => $field_details['cardinality'] ?? 1,
          'settings' => $field_details['settings'],
        ],
        [
          'idspace_plugin_id' => 'chado_id_space',
          'vocab_plugin_id' => 'chado_vocab',
        ]
      );
    }

    return [$bundle, $fields];
  }

  /**
   * Prepares the environment for testing ChadoStorage directly.
   *
   * This uses the bundle to setup the testing environment similarily to the
   * original ChadoStorage tests. In this manner we don't need YAML files
   * describing the fields nor do we need to create them because that has
   * already been done by setupChadoEntityFieldSystemUnderTest().
   */
  public function prepareTestingChadoStorage() {
    $this->propertyTypes = [];
    $this->propertyValues = [];
    $this->dataStoreValues = [];

    // Get plugin managers we need for our testing.
    $storage_manager = \Drupal::service('tripal.storage');
    $this->chadoStorage = $storage_manager->createInstance('chado_storage');

    // We need to add each field to the ChadoStorage object.
    foreach ($this->fieldConfig as $field_name => $fieldConfig) {
      $this->chadoStorage->addFieldDefinition($field_name, $fieldConfig);
    }
  }

}
