<?php
namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Core\Datetime\Entity\DateFormat;
use Drupal\Core\Entity\Entity\EntityFormDisplay;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\Core\Field\FieldItemList;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\field\Entity\FieldConfig;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;
use Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage;
use Symfony\Component\Yaml\Yaml;

/**
 * Provides functions related to testing Chado Fields.
 */
trait ChadoFieldTestTrait {

  use UserCreationTrait;

  /**
   * An array of FieldStorageConfig objects keyed by the field name.
   *
   * @var FieldStorageConfig[]
   */
  protected array $fieldStorage = [];

  /**
   * An array of FieldConfig objects keyed by the field name.
   *
   * @var FieldConfig[]
   */
  protected array $fieldConfig = [];

  /**
   * An array of TripalEntityType objects keyed by the bundle name.
   *
   * @var TripalEntityType[]
   */
  protected array $tripalEntityType = [];

  /**
   * An array of display objects keyed by Tripal Content Type bundle name.
   *
   * @var EntityViewDisplay[]
   */
  protected array $entityViewDisplay = [];

  /**
   * An array of display objects keyed by Tripal Content Type bundle name.
   *
   * @var EntityFormDisplay[]
   */
  protected array $entityFormDisplay = [];

  /**
   * A ChadoStorage object to run your tests on.
   *
   * @var \Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage
   */
  protected ChadoStorage $chadoStorage;

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
   * Confirms that the retrieved values match the expected ones.
   *
   * More specifically, it checks
   *  1. the retrieved values matches the expected format
   *  2. Each expected property type exists in the retrieved values for
   *     each delta by checking the property key is at values[delta][property key]
   *  3. For each delta[property key]
   *       - the value is an array
   *       - the array has a 'value' key
   *       - the delta[property key][value] is a StoragePropertyValue instance
   *       - the delta[property key][value]->value matches the expected value
   *
   * @param array $expected_values
   *  A nested array of expected values following the format:
   *    - delta (e.g. 0):
   *      - property key => expected value
   * @param TripalEntity $entity
   *   An entity whose field values we want to check against those expected.
   * @param string $message_prefix
   *   A short string that all assert messages will be prefixed with.
   */
  public function assertFieldValuesMatch(array $expected_values, TripalEntity $entity, string $message_prefix = '') {

    foreach ($expected_values as $expected_field_name => $expected_field_delta) {

      // Check that each expected field exists in the provided entity.
      $this->assertTrue($entity->hasField($expected_field_name), $message_prefix . ": field '$expected_field_name' was not found in the provided entity.");

      // Check that we were able to retrieve the field values.
      $field_item_list = $entity->get($expected_field_name);
      $this->assertInstanceOf(FieldItemList::class,$field_item_list, $message_prefix . ": we could not retrieve the values of field '$expected_field_name' in the provided entity.");

      // Retrieve the Drupal field table values for this entity to check
      // against later.
      $drupal_field_table = 'tripal_entity__' . $expected_field_name;
      $query = $this->drupal_connection->select($drupal_field_table, 'drupal')
        ->fields('drupal')
        ->condition('entity_id', $entity->id(), "=")
        ->execute();
      $drupal_field_records = $query->fetchAllAssoc('delta');

      foreach ($expected_field_delta as $expected_delta => $expected_delta_values) {

        // Check that we were able to retrieve a specific field value.
        $field_item = $field_item_list->get($expected_delta);
        $this->assertInstanceOf(TripalFieldItemInterface::class, $field_item, $message_prefix . ": $expected_field_name [$expected_delta] could not be retrieved.");

        // Check that there is a record in the Drupal field table for this delta.
        $this->assertArrayHasKey($expected_delta, $drupal_field_records, $message_prefix . ": $expected_field_name [$expected_delta] should have a record in the Drupal field table '$drupal_field_table'.");
        $drupal_field_record = $drupal_field_records[$expected_delta];

        foreach ($expected_delta_values as $expected_property_type => $expected_value) {

          // Check that the property value matched what we expected.
          $property_value = $field_item->get($expected_property_type)->getValue();
          $this->assertEquals($expected_value, $property_value, $message_prefix . ": $expected_field_name [$expected_delta] [$expected_property_type] value did not match what we expected.");

          // Check that the Drupal field table matches what we expected.
          $drupal_column_name = $expected_field_name . '_' . $expected_property_type;
          $this->assertEquals($expected_value, $drupal_field_record->{$drupal_column_name}, $message_prefix . ": $expected_field_name [$expected_delta] [$expected_property_type] did not match what we expected in the drupal field table ($drupal_field_table).");

          // If the current property type is a rank, then check the rank and
          // delta match.
          // @todo waiting to confirm the above change worked first.
        }
      }

      // Check that there were the right number of field values.
      // This ensures there were not more then expected. It's checked after the
      // property values/delta are checked to ensure we get more tailored
      // feedback if there are less than expected.
      $this->assertCount(sizeof($expected_field_delta), $field_item_list, $message_prefix . ": field '$expected_field_name' did not have the expected number of values.");

      // Check that there are the right number of records in the Drupal field table.
      $this->assertCount(sizeof($expected_field_delta), $drupal_field_records,
      $message_prefix . ": field '$expected_field_name' did not have the expected number of records in the drupal field table ($drupal_field_table).");
    }
  }

  /**
   * Confirms the widget element is as expected in the TripalEntity form.
   *
   * @param mixed $expected_default
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
      $expected_defaults = $expected_field_defaults[$field_name];

      // Check the form has an element for this field.
      $this->assertArrayHasKey($field_name, $form,
        $message_prefix . ": We expect the TripalEntity form to have an element $field_name.");
      $this->assertArrayHasKey('widget', $form[$field_name],
        $message_prefix . ": We expect the widget element for $field_name to have a widget key containing the widget form elements.");

      $widget_form_element = $form[$field_name]['widget'];

      // Check that the default is set properly after create.
      /** @todo finish this...
      foreach (\Drupal\Core\Render\Element::children($widget_form_element) as $element_key) {
        $element = $widget_form_element[$element_key];
        $expected_values = $expected_defaults[$element_key];

        // Check that there is an element for each expected value.
        foreach ($expected_values as $key => $value) {
          $this->assertArrayHasKey($key, $element, $message_prefix . ": $field_name [$element_key] widget element should contain an element with this name.");
          /** @todo Figure out how to check the value.
          if (array_key_exists('#value', $element[$key])) {
            $this->assertEquals($value, $element[$key]['#value'], $message_prefix . ": $field_name [$element_key] [$key][#value] doesn't match what we expect. Element:" . print_r($element[$key], TRUE));
          }
          elseif (array_key_exists('#default_value', $element[$key])) {
            $this->assertEquals($value, $element[$key]['#default_value'], $message_prefix . ": $field_name [$element_key] [$key][#default_value] doesn't match what we expect. Element:" . print_r($element[$key], TRUE));
          }
          else {
            print "\n$field_name [$element_key] [$key]: " . print_r(array_keys($element[$key]), TRUE);
          }
        }
      }
        */
    }
  }

  /**
   * Allows you to set the 'fields' by specifying the top level key of a YAML file.
   *
   * @param string $yaml_file
   *   The full path to a yaml file which follows the format descripbed above.
   *
   * @return array
   *   The first array returned describes the state of the test environment to
   *   be setup and the second describes the scenarios to test. For a
   *   description of the structure of these arrays, see the YAML file directly.
   */
  public function getTestInfoFromYaml($yaml_file) {

    if (!file_exists($yaml_file)) {
      throw new \Exception("Cannot open YAML file $yaml_file.");
    }

    $file_contents = file_get_contents($yaml_file);
    if (empty($file_contents)) {
      throw new \Exception("Unable to retrieve contents for YAML file $yaml_file.");
    }

    $yaml_data = Yaml::parse($file_contents);
    if (empty($yaml_data)) {
      throw new \Exception("Unable to parse YAML file $yaml_file.");
    }

    if (!array_key_exists('system-under-test', $yaml_data)) {
      throw new \Exception("The 'system-under-test' key is missing from the $yaml_file.");
    }
    if (!array_key_exists('scenarios', $yaml_data)) {
      throw new \Exception("The 'scenarios' key is missing from the $yaml_file.");
    }

    return [$yaml_data['system-under-test'], $yaml_data['scenarios']];
  }

  /**
   * Called in the test setUp() for kernel tests to ensure all the needed
   * resources are available.
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
  public function setupFieldTestEnvironment(array $system_under_test = []): array {

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->setUpCurrentUser(['uid' => 1]);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm', 'TripalEntity', 'ChadoField']);
    // -- we need the chado term mapping for our properties.
    $this->installEntitySchema('chado_term_mapping');
    // -- we need the date_format entity to render the entity form.
    $this->installEntitySchema('date_format');
    $this->installConfig('datetime');
    // -- we need access to the core term mappings.
    tripal_chado_rebuild_chado_term_mappings();

    // We also need to be able to access date formats for created/updated.
    $formats = [
      ['id' => 'short', 'pattern' => 'j M Y - H:i'],
      ['id' => 'html_date', 'pattern' => 'Y-m-d'],
      ['id' => 'html_time', 'pattern' => 'H:i:s'],
    ];
    foreach ($formats as $values) {
      DateFormat::create($values)
        ->save();
    }

    // If information about the environment to be setup was provided, then we
    // will set it up for them :-).
    if (!empty($system_under_test)) {
      return $this->setupFieldSystemUnderTest($system_under_test);
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
  public function setupFieldSystemUnderTest(array $system_under_test): array {

    // 1. Create the bundle.
    $bundle = $this->createTripalContentType($system_under_test['bundle']);
    $bundle->setThirdPartySetting('tripal', 'chado_base_table', $system_under_test['bundle']['settings']['chado_base_table']);
    $bundle_name = $bundle->id();

    // 2. Create the fields.
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
        ]
      );
    }

    return [$bundle, $fields];
  }

  /**
   * Create a FieldStorage object for a given field type.
   *
   * @param string $entity_type
   *   The machine name of the entity to add the field to (e.g., organism)
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - termIdSpace (string)
   *    - termAccession (string)
   *    - settings (array) an array of additional settings for the field
   * @return FieldStorageConfig
   *   The field storage object that was just created.
   */
  public function createFieldType(string $entity_type, array $values = []) {

    // Defaults
    $random = $this->getRandomGenerator();
    $values['field_name'] = $values['field_name'] ?? $random->word(6) . '_' . $random->word(15);
    $values['field_type'] = $values['field_type'] ?? 'tripal_string_type';
    // -- Term
    $term_values = [];
    if (array_key_exists('termIdSpace', $values)) {
      $term_values['id_space_name'] = $values['termIdSpace'];
    }
    if (array_key_exists('termAccession', $values)) {
      $term_values['term'] = [];
      $term_values['term']['accession'] = $values['termAccession'];
    }
    $values['settings'] = $values['settings'] ?? [];

    // Now create the main term for the field.
    $term = $this->createTripalTerm($term_values, 'chado_id_space', 'chado_vocabulary');

    // Now for the field storage.
    $fieldStorage = FieldStorageConfig::create([
      'field_name' => $values['field_name'],
      'entity_type' => $entity_type,
      'type' => $values['field_type'],
      'cardinality' => $values['cardinality'] ?? 1,
      'settings' => [
        'termIdSpace' => $term_values['id_space_name'],
        'termAccession' => $term_values['term']['accession'],
      ] + $values['settings'],
    ]);
    $fieldStorage
      ->save();

    $this->fieldStorage[$values['field_name']] = $fieldStorage;
    return $fieldStorage;
  }

  /**
   * Create a FieldConfig object for a given field type on a given entity.
   *
   * @param string $entity_type
   *   The machine name of the entity to add the field to (e.g., organism)
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - term_id_space (string)
   *    - term_accession (string)
   *    - bundle_name (string)
   *    - formatter_id (string)
   *    - fieldStorage (FieldStorageConfig)
   * @return FieldConfig
   *   The field object that was just created.
   */
  public function createFieldInstance(string $entity_type, array $values = []) {

    // Defaults
    $random = $this->getRandomGenerator();
    $values['formatter_id'] = $values['formatter_id'] ?? 'default_tripal_string_type_formatter';
    $values['field_type'] = $values['field_type'] ?? 'tripal_string_type';
    // -- Bundle
    if (array_key_exists('bundle', $values)) {
      $bundle = $values['bundle'];
      $values['bundle_name'] = $bundle->getID();
    }
    elseif (array_key_exists('bundle_name', $values)) {
      $bundle = \Drupal::entityTypeManager()
        ->getStorage('tripal_entity_type')
        ->loadByProperties(['id' => $values['bundle_name']]);
      $bundle = array_pop($bundle);
    }
    else {
      $bundle = $this->createTripalContentType();
      $values['bundle_name'] = $bundle->getID();
    }

    // -- Field Storage Config
    if (!array_key_exists('fieldStorage', $values)) {
      $values['fieldStorage'] = $this->createFieldType(
        'tripal_entity',
        $values
      );
      $this->fieldStorage[$values['field_name']] = $values['fieldStorage'];
    }

    $fieldConfig = FieldConfig::create([
      'field_storage' => $values['fieldStorage'],
      'bundle' => $values['bundle_name'],
      'required' => TRUE,
    ]);
    $fieldConfig
      ->save();

    // WIDGET.
    $display_options = [
      'type' => $values['widget_id'],
      'region' => 'content',
      'settings' => [],
    ];
    if (array_key_exists($values['bundle_name'], $this->entityFormDisplay)) {
      $display = $this->entityFormDisplay[$values['bundle_name']];
    } else {
      $display = EntityFormDisplay::create([
        'targetEntityType' => $fieldConfig->getTargetEntityTypeId(),
        'bundle' => $values['bundle_name'],
        'mode' => 'default',
        'status' => TRUE,
      ]);
      $this->entityFormDisplay[$values['bundle_name']] = $display;
    }
    $display->setComponent($values['fieldStorage']->getName(), $display_options);
    $display->save();

    // FORMATTER.
    $display_options = [
      'type' => $values['formatter_id'],
      'label' => 'hidden',
      'settings' => [],
    ];
    if (array_key_exists($values['bundle_name'], $this->entityViewDisplay)) {
      $display = $this->entityViewDisplay[$values['bundle_name']];
    }
    else {
      $display = EntityViewDisplay::create([
        'targetEntityType' => $fieldConfig->getTargetEntityTypeId(),
        'bundle' => $values['bundle_name'],
        'mode' => 'default',
        'status' => TRUE,
      ]);
      $this->entityViewDisplay[$values['bundle_name']] = $display;
    }
    $display->setComponent($values['fieldStorage']->getName(), $display_options);
    $display->save();

    $this->fieldConfig[$values['field_name']] = $fieldConfig;
    $this->tripalEntityType[$values['bundle_name']] = $bundle;
    return $fieldConfig;
  }

  /**
   * Prepares the environment for testing ChadoStorage directly.
   *
   * This uses the bundle to setup the testing environment similarily to the
   * original ChadoStorage tests. In this manner we don't need YAML files
   * describing the fields nor do we need to create them because that has
   * already been done by setupFieldSystemUnderTest().
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
