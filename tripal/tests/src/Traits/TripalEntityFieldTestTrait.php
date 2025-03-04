<?php
namespace Drupal\Tests\tripal\Traits;

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
 * Provides functions related to testing Tripal Entities + their Fields.
 */
trait TripalEntityFieldTestTrait {

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
  public function setupEntityFieldTestEnvironment(array $system_under_test = []): array {

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->setUpCurrentUser(['uid' => 1]);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm', 'TripalEntity', 'TripalField']);
    // -- we need the date_format entity to render the entity form.
    $this->installEntitySchema('date_format');
    $this->installConfig('datetime');

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
      return $this->setupEntityFieldSystemUnderTest($system_under_test);
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
  public function setupEntityFieldSystemUnderTest(array $system_under_test): array {

    // 1. Create the bundle.
    $bundle = $this->createTripalContentType($system_under_test['bundle']);
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
    if (!array_key_exists('settings', $values)) {
      $values['settings'] = [];
    }
    // @todo allow this to be set via variables.
    $term = $this->createTripalTerm($term_values, 'tripal_default_id_space', 'tripal_default_vocabulary');

    // Now for the field storage.
    $fieldStorage = FieldStorageConfig::create([
      'field_name' => $values['field_name'],
      'entity_type' => $entity_type,
      'type' => $values['field_type'],
      'cardinality' => $values['cardinality'] ?? 1,
      'settings' => [
        'termIdSpace' => $term->getIdSpace(),
        'termAccession' => $term->getAccession(),
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
   *    - widget_id (string)
   *    - fieldStorage (FieldStorageConfig)
   * @return FieldConfig
   *   The field object that was just created.
   */
  public function createFieldInstance(string $entity_type, array $values = []) {

    // Defaults
    $random = $this->getRandomGenerator();
    $values['formatter_id'] = $values['formatter_id'] ?? 'default_tripal_string_type_formatter';
    $values['widget_id'] = $values['widget_id'] ?? 'default_tripal_string_type_widget';
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
    } else {
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
}
