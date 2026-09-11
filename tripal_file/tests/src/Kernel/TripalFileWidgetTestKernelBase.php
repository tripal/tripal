<?php

namespace Drupal\Tests\tripal_file\Kernel;

use Drupal\pgsql\Driver\Database\pgsql\Connection;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * A parent class for the Tripal File widget tests.
 *
 * This contains the code common to testing the tripal file widgets.
 */
#[RunTestsInSeparateProcesses]
abstract class TripalFileWidgetTestKernelBase extends ChadoTestKernelBase {

  use ChadoFieldTestTrait;

  /**
   * The modules that this test depends on.
   *
   * NOTE: since this is a kernel test, these modules are not being installed
   * but are available to be installed.
   *
   * @var array
   */
  protected static $modules = [
    'system',
    'user',
    'path',
    'path_alias',
    'field',
    'filter',
    'datetime',
    'text',
    'file',
    'tripal',
    'tripal_chado',
    'tripal_file',
  ];

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * The test drupal connection. It is also set in the container.
   *
   * @var Drupal\pgsql\Driver\Database\pgsql\Connection
   */
  protected Connection $drupal_connection;

  /**
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = '';

  /**
   * Record IDs created for testing.
   *
   * @var array
   */
  protected array $record_ids;

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - chado_version: the version of chado to test under.
   *   - bundle: an array defining the tripal entity type to create.
   *   - fields: a list of fields to be attached the above bundle.
   */
  protected array $system_under_test;

  /**
   * The TripalEntityType id of the bundle being used in this test.
   *
   * @var string
   */
  protected string $bundle_name;

  /**
   * Describes the scenarios to test.
   *
   * This will be used in combination with the data provider. It can't be
   * accessed directly in the dataProvider due to the way that PHPUnit is
   * setup.
   *
   * @var array
   *  A list of scenarios where each one has the following keys:
   *  - label: A human-readable label for the scenario to be used in assert
   *    messages.
   *  - descrition: A description of the scenario and what you are wanting to
   *    test. This will not be used in the test but is rather there to help
   *    people reading the YAML file and to make it easier to maintain.
   *  - create: An array of the values to be provided when creating a
   *    TripalEntity. There should be a key matching the name of each field in
   *    the system-under-test and it's value should be an array containing all
   *    the property types for that field mapped to a value.
   *  - edit: An array of the values to be provided when updating an existing
   *    TripalEntity. There should be a key matching the name of each field in
   *    the system-under-test and it's value should be an array containing all
   *    the property types for that field mapped to a value.
   */
  protected array $scenarios;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // The Drupal connection will be created in the parent. This is used
    // when checking the Drupal field tables.
    $this->drupal_connection = $this->container->get('database');

    // Retrieve info from the YAML file for this particular test.
    [$this->system_under_test, $this->scenarios] = $this->getTestInfoFromYaml($this->yaml_info_file);
    $this->bundle_name = $this->system_under_test['bundle']['id'];

    // Create the test Chado installation we will be using.
    if (!array_key_exists('chado_version', $this->system_under_test)) {
      $this->system_under_test['chado_version'] = '1.3';
    }
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO,
      $this->system_under_test['chado_version']
    );

    // Next setup the environment, but skip the system under test for now,
    // we need to setup the tripal_file module before that can be done.
    $this->setupChadoEntityFieldTestEnvironment([]);

    // We will need the drupal 'file' module also.
    $this->installEntitySchema('file');

    // Install schema for custom chado tables, needed for tripal_file module.
    $this->installSchema('tripal_chado', ['tripal_custom_tables']);

    // This schema is needed to create a test file.
    $this->installSchema('file', ['file_usage']);

    // Set up the tripal file module.
    $this->installConfig(['tripal_file']);

    // Install the module's custom chado tables in the test schema.
    $test_schema_name = $this->chado_connection->getSchemaName();
    tripal_file_create_custom_chado_tables($test_schema_name);

    // Install terms for the tripal_file module.
    $terms_init_service = \Drupal::service('tripal_chado.terms_init');
    $terms_init_service->installTerms('tripal_file_content_terms');

    // Now that tripal_file is set up, we can setup the environment
    // according to the system under test.
    $this->setupChadoEntityFieldSystemUnderTest($this->system_under_test);
  }

  /**
   * Retrieves the current scenario based on the data provider.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @return array
   *   The scenario to be tested as defined in the YAML.
   */
  public function retrieveCurrentScenario(int $current_scenario_key, string $current_scenario_label) {

    // Retrieve the correct scenario.
    $current_scenario = $this->scenarios[$current_scenario_key];
    $this->assertEquals($current_scenario_label, $current_scenario['label'], "We may not have retrieved the expected scenario as the labels did not match.");

    return $current_scenario;
  }

  /**
   * Tests the widget.
   *
   * In the actual tests, this will use the DataProvider provideScenarios.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   */
  public function checkTripalFileWidget(int $current_scenario_key, string $current_scenario_label) {
    // Retrieve the full details of the current scenario.
    $current_scenario = $this->retrieveCurrentScenario($current_scenario_key, $current_scenario_label);

    // 1. Test that the create form is generated properly.
    // Setup an empty Tripal entity form to interact with (test defaults),
    // and confirm all the keys we expect are set to the values we expect.
    [$form_object, $form, $form_state] = $this->setupTripalEntityAddForm($this->bundle_name);
    $this->assertFieldWidgetsMatch($current_scenario['create']['expected_form_keys'], $this->system_under_test['fields'], $form, $current_scenario['label'] . '[CREATE]');

    // 2. Test the form submit / widget submit to create the entity,
    // and validate it with the data provided.
    $this->populateTripalEntityFormState($form_object, $form_state, $current_scenario['create']['user_input']);
    $form_object->validateForm($form, $form_state);
    $errors = $form_state->getErrors();
    $this->assertCount(0, $errors, "We got errors when we submitted the CREATE form with the supplied values. Errors: " . print_r($errors, TRUE));

    // Submit the create form and confirm the entity matches our expectations,
    // and that values saved in chado match our expectations.
    $form_object->submitForm($form, $form_state);
    $form_object->save($form, $form_state);
    $entity = $form_object->getEntity();
    $this->assertFieldValuesMatch($current_scenario['create']['expected_field_values'], $entity, "Field values didn't match our expectations after saving the create form");
    if (isset($current_scenario['create']['expected_chado_values'])) {
      $this->assertChadoValuesMatch($current_scenario['create']['expected_chado_values'], "Chado values didn't match our expectations after saving the create form");
    }

    // 3. Test that the edit form is generated properly.
    // Setup an edit Tripal entity form to interact with (test defaults),
    // and confirm all the keys we expect are set to the values we expect.
    $form = $form_object = $form_state = NULL;
    [$form_object, $form, $form_state] = $this->setupTripalEntityEditForm($this->bundle_name, $entity);
    $this->assertFieldWidgetsMatch($current_scenario['edit']['expected_form_keys'], $this->system_under_test['fields'], $form, $current_scenario['label'] . ' [EDIT]');

    // 4. Test editing the entity through the form,
    // and validate it with the data provided.
    $this->populateTripalEntityFormState($form_object, $form_state, $current_scenario['edit']['user_input']);
    $form_object->validateForm($form, $form_state);
    $errors = $form_state->getErrors();
    $this->assertCount(0, $errors, "We got errors when we submitted the EDIT form with the supplied values. Errors: " . print_r($errors, TRUE));

    // Submit the edit form and confirm the entity matches our expectations,
    // and that values saved in chado match our expectations.
    $form_object->submitForm($form, $form_state);
    $form_object->save($form, $form_state);
    $entity = $form_object->getEntity();
    $this->assertFieldValuesMatch($current_scenario['edit']['expected_field_values'], $entity, "Field values didn't match our expectations after saving the edit form");
    if (isset($current_scenario['edit']['expected_chado_values'])) {
      $this->assertChadoValuesMatch($current_scenario['edit']['expected_chado_values'], "Chado values didn't match our expectations after saving the edit form");
    }
  }

}
