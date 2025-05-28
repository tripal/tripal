<?php

namespace Drupal\Tests\tripal\Traits;

use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Core\Datetime\Entity\DateFormat;
use Drupal\Core\Entity\Entity\EntityFormDisplay;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\Core\Field\FieldItemList;
use Drupal\Core\Form\FormState;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\field\Entity\FieldConfig;
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
   * Confirms that the retrieved values match the expected ones.
   *
   * More specifically, it checks the following for each expected field:
   *  1. The expected field exists in the provided entity.
   *  2. We were able to retrieve the field values.
   *  3. There is a record in the Drupal field table for each delta.
   *  4. For each delta[property key]:
   *       - the value in the entity matches what we expected.
   *       - the value in the drupal field table matches what we expected.
   *       - the delta[property key][value] is a StoragePropertyValue instance.
   *       - the delta[property key][value]->value matches the expected value.
   *  5. There are the expected number of items in the entity.
   *  6. There are the expected number of items in the drupal field table.
   *
   * @param array $expected_values
   *   A nested array of expected values following the format:
   *    - field name (i.e. project_name):
   *      - delta (e.g. 0):
   *        - property key => expected value.
   * @param \Drupal\tripal\Entity\TripalEntity $entity
   *   An entity whose field values we want to check against those expected.
   * @param string $message_prefix
   *   A short string that all assert messages will be prefixed with.
   */
  public function assertFieldValuesMatch(array $expected_values, TripalEntity $entity, string $message_prefix = '') {

    // For each expected field...
    foreach ($expected_values as $expected_field_name => $expected_field_delta) {

      // Check that each expected field exists in the provided entity.
      $this->assertTrue($entity->hasField($expected_field_name), $message_prefix . "Field '$expected_field_name' was not found in the provided entity.");

      // Check that we were able to retrieve the field values.
      $field_item_list = $entity->get($expected_field_name);
      $this->assertInstanceOf(FieldItemList::class, $field_item_list, $message_prefix . "We could not retrieve the values of field '$expected_field_name' in the provided entity.");

      // Retrieve the Drupal field table values for this entity to check
      // against later.
      $drupal_field_table = 'tripal_entity__' . $expected_field_name;
      $query = $this->drupal_connection->select($drupal_field_table, 'drupal')
        ->fields('drupal')
        ->condition('entity_id', $entity->id(), "=")
        ->execute();
      $drupal_field_records = $query->fetchAllAssoc('delta');

      // For each delta in this field...
      foreach ($expected_field_delta as $expected_delta => $expected_delta_values) {

        // Check that we were able to retrieve a specific field value.
        $field_item = $field_item_list->get($expected_delta);
        $this->assertInstanceOf(TripalFieldItemInterface::class, $field_item, $message_prefix . "$expected_field_name [$expected_delta] could not be retrieved.");

        // Check record is in the Drupal field table for this delta.
        $this->assertArrayHasKey($expected_delta, $drupal_field_records, $message_prefix . "$expected_field_name [$expected_delta] should have a record in the Drupal field table '$drupal_field_table'.");
        $drupal_field_record = $drupal_field_records[$expected_delta];

        // For each property of the field...
        foreach ($expected_delta_values as $expected_property_type => $expected_value) {

          // Check that the property value matched what we expected.
          $property_value = $field_item->get($expected_property_type)->getValue();
          $this->assertEquals($expected_value, $property_value, $message_prefix . ": $expected_field_name [$expected_delta] [$expected_property_type] value did not match what we expected.");

          // Check that the Drupal field table matches what we expected.
          $drupal_column_name = $expected_field_name . '_' . $expected_property_type;
          $this->assertEquals($expected_value, $drupal_field_record->{$drupal_column_name}, $message_prefix . ": $expected_field_name [$expected_delta] [$expected_property_type] did not match what we expected in the drupal field table ($drupal_field_table).");

        }
      }

      // Check that there were the right number of field values.
      // This ensures there were not more then expected. It's checked after the
      // property values/delta are checked to ensure we get more tailored
      // feedback if there are less than expected.
      $this->assertCount(count($expected_field_delta), $field_item_list, $message_prefix . ": field '$expected_field_name' did not have the expected number of values.");

      // Check the right number of records are in the Drupal field table.
      $this->assertCount(
        count($expected_field_delta),
        $drupal_field_records,
        $message_prefix . ": field '$expected_field_name' did not have the expected number of records in the drupal field table ($drupal_field_table)."
      );
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

    // Update entity settings to match module defaults.
    // @see tripal/config/install/tripal.settings.yml.
    $allowed_title_tags = 'em i strong u';
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.allowed_title_tags', $allowed_title_tags)
      ->save();

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
   *   These values are passed directly to the create() method.
   *   Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - termIdSpace (string)
   *    - termAccession (string)
   * @param array $options
   *   Options to customize how the field type is created. Supported key are:
   *    - idspace_plugin_id: the TripalIdSpace plugin to use.
   *    - vocab_plugin_id: the TripalVocab plugin to use.
   *
   * @return \Drupal\field\Entity\FieldStorageConfig
   *   The field storage object that was just created.
   */
  public function createFieldType(string $entity_type, array $values = [], array $options = []): FieldStorageConfig {

    // Defaults.
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
    if (!array_key_exists('idspace_plugin_id', $options)) {
      $options['idspace_plugin_id'] = 'tripal_default_id_space';
    }
    if (!array_key_exists('vocab_plugin_id', $options)) {
      $options['vocab_plugin_id'] = 'tripal_default_vocabulary';
    }

    $term = $this->createTripalTerm($term_values, $options['idspace_plugin_id'], $options['vocab_plugin_id']);

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
   *   These values are passed directly to the create() method.
   *   Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - term_id_space (string)
   *    - term_accession (string)
   *    - bundle_name (string)
   *    - formatter_id (string)
   *    - widget_id (string)
   *    - fieldStorage (FieldStorageConfig)
   * @param array $options
   *   Options to customize how the field type is created. Supported key are:
   *    - idspace_plugin_id: the TripalIdSpace plugin to use.
   *    - vocab_plugin_id: the TripalVocab plugin to use.
   *
   * @return \Drupal\field\Entity\FieldConfig
   *   The field object that was just created.
   */
  public function createFieldInstance(string $entity_type, array $values = [], array $options = []): FieldConfig {

    // Defaults.
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
    }
    else {
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
   * Sets the 'fields' by specifying the top level key of a YAML file.
   *
   * @param string $yaml_file
   *   The full path to a yaml file which follows the format descripbed above.
   *
   * @return array
   *   The first array returned describes the state of the test environment to
   *   be setup and the second describes the scenarios to test. For a
   *   description of the structure of these arrays, see the YAML file directly.
   */
  public function getTestInfoFromYaml(string $yaml_file): array {

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
   * Sets up the TripalEntity Form to create an entity.
   *
   * This is typically used when testing fields.
   *
   * When a Drupal form is built it goes through two main stages:
   * 1. The retrieve and preparation stage: the form is built using the form's
   * build function and all the alter form hooks are called. The output of this
   * stage is a Form API array, an array with all the elements the form should
   * have, but not the final render array.
   * 2. Recursive building stage: a recursive process traversing the $form array
   * calling auxiliary functions like #process and #after_build callbacks. The
   * goal is to obtain a Drupal render array with the final shape of the form,
   * with all the required elements.
   *
   * This method ensures both are done in order to accurately reflect the
   * final form.
   *
   * @param string $bundle_name
   *   The id of the Tripal Entity Type this form is for.
   *
   * @return array
   *   The parts of an entity form; specifically,
   *   - ContentEntityFormInterface $form_object: an object that can be used
   *     with the form state to validate and submit the TripalEntity form.
   *   - array $form: the complete form including all attached field widget
   *     form elements.
   *   - FormState $form_state: the state of this form including relationships
   *     to both the form object and form array.
   *
   * @see Drupal\Core\Form\FormBuilder
   */
  public function setupTripalEntityAddForm(string $bundle_name): array {
    $entity_type_manager = $this->container->get('entity_type.manager');
    $form_builder = \Drupal::formBuilder();

    // Step 0: Prepare Form object and form state.
    // -- Get the form object from the entity type manager.
    $form_object = $entity_type_manager->getFormObject('tripal_entity', 'add');
    // -- Create an empty entity object for the form.
    // This is not saved yet but is needed as an empty husk for the form
    // to determine the fields, widgets, etc to build the form array.
    $entity = TripalEntity::create([
      'type' => $this->bundle_name,
    ]);
    $form_object->setEntity($entity);
    // -- Now get the form id (needs the entity first).
    $form_id = $form_object->getFormId();

    // -- Create the Form State and set it up to be used by an entity form.
    $form_state = new FormState();
    $form_state->setFormObject($form_object);

    // -- Set the base user input for the form.
    $input = [
      'uid' => 1,
    ];
    $form_state->setUserInput($input);

    // Step 1: Form Retrieve + Preparation Phase.
    $form = $form_builder->retrieveForm($form_id, $form_state);
    $form_builder->prepareForm($form_id, $form, $form_state);
    // -- The form state likes to have the full array.
    $form_state->setCompleteForm($form);

    // Step 2: Recursive Form Building stage.
    // This triggers the #after_build and $process callbacks of our widgets.
    $form_builder->processForm($form_id, $form, $form_state);

    return [$form_object, $form, $form_state];
  }

  /**
   * Populate TripalEntity form with the values passed in.
   *
   * @param Drupal\Core\Entity\ContentEntityFormInterface $form_object
   *   The form object like that returned by setupTripalEntityAddForm().
   *   This is needed in addition to the form state since it contains a
   *   form display that needs to be set with the form state.
   * @param Drupal\Core\Form\FormState $form_state
   *   The current state of the form. This is where we will set the values
   *   passed in.
   * @param array $values
   *   An array of the values to set. This should be keyed by each field name
   *   and then it's values should map directly to the form elements in that
   *   fields widget.
   */
  public function populateTripalEntityForm(&$form_object, &$form_state, $values) {

    // Retrieve the Tripal Entity Type id
    // for use when generating the form display.
    $bundle_name = $form_object->getEntity()->getType();

    // Get the form display object
    // or load one if it hasn't been set in the form object yet.
    $form_display = $form_object->getFormDisplay($form_state);
    if (!$form_display) {
      $form_display = $entity_type_manager->getStorage('entity_form_display')->load('tripal_entity.' . $bundle_name . '.default');
    }

    // Now, populate the form state with the user input.
    foreach ($values as $key => $value) {
      $form_state->setValue($key, $value);
    }

    // Set the user to the one created in the test unless overriden in values.
    if (!array_key_exists('uid', $values)) {
      $form_state->setValue('uid', [['target_id' => 1]]);
    }

    // And finally populate the form display based on the form state.
    $form_object->setFormDisplay($form_display, $form_state);
  }

}
