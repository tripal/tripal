<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField\FieldType;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\DataProvider;

/**
 * Tests the ChadoPropertyTypeDefault Field Type.
 *
 * Specifically focused on create + update actions performed on the entity
 * directly. Both TripalEntity, ChadoStorage and the field will be covered.
 *
 * @group TripalField
 * @group ChadoField
 */
#[Group('TripalField')]
#[Group('ChadoField')]
class ChadoPropertyWidgetFormTest extends ChadoTestKernelBase {

  use ChadoFieldTestTrait;

  /**
   * The theme to use when rendering content in the test environment.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

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
    'tripal',
    'tripal_chado',
  ];

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected object $chado_connection;

  /**
   * The test drupal connection. It is also set in the container.
   *
   * @var object
   */
  protected object $drupal_connection;

  /**
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/ChadoPropertyWidgetForm-TestInfo.yml';

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

    // First retrieve info from the YAML file for this particular test.
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

    // Next setup the environment according to the system under test.
    $this->setupChadoEntityFieldTestEnvironment($this->system_under_test);
  }

  /**
   * Data Provider: works with the YAML to provide scenarios for testing.
   *
   * @return array
   *   List of scenarios to test where each one matches a key and label in the
   *   associated YAML scenarios.
   */
  public static function provideScenarios() {
    $scenarios = [];

    $scenarios[] = [
      0,
      "Base Fields Only",
    ];

    $scenarios[] = [
      1,
      "Properties Added on Edit",
    ];

    $scenarios[] = [
      2,
      "Property Reorder",
    ];

    return $scenarios;
  }

  /**
   * Retrieves the current scenario based on the data provider.
   *
   * NOTE: Also ensures the type_ids match what is currently in the database.
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

    // Set the property field types just in case.
    $comment_type_id = $this->getCvtermID('rdfs', 'comment');
    $location_type_id = $this->getCvtermID('NCIT', 'C25341');
    foreach (['create', 'edit'] as $process_key) {
      foreach (['user_input'] as $input_type) {
        if (array_key_exists('project_prop1', $current_scenario[$process_key][$input_type])) {
          foreach ($current_scenario[$process_key][$input_type]['project_prop1'] as $delta => $values) {
            if ($values['type_id'] === 181) {
              $current_scenario[$process_key][$input_type]['project_prop1'][$delta]['type_id'] = $comment_type_id;
            }
          }
        }
        if (array_key_exists('project_prop2', $current_scenario[$process_key][$input_type])) {
          foreach ($current_scenario[$process_key][$input_type]['project_prop2'] as $delta => $values) {
            if ($values['type_id'] === 159) {
              $current_scenario[$process_key][$input_type]['project_prop2'][$delta]['type_id'] = $location_type_id;
            }
          }
        }
      }
    }

    return $current_scenario;
  }

  /**
   * Tests the ChadoPropertyType field through entity form + field widget.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testChadoPropertyWidgetUpdate(int $current_scenario_key, string $current_scenario_label) {

    // Retrieve the full details of the current scenario.
    $current_scenario = $this->retrieveCurrentScenario($current_scenario_key, $current_scenario_label);

    // 1. Test the create form is generated properly.
    // Setup an empty Tripal entity form to interact with (test defaults).
    [$form_object, $form, $form_state] = $this->setupTripalEntityAddForm($this->bundle_name);
    // Confirm all the keys we expect are set to the values we expect.
    $this->assertFieldWidgetsMatch($current_scenario['create']['expected_form_keys'], $this->system_under_test['fields'], $form, $current_scenario['label'] . '[CREATE]');

    // 2. Test the form submit / widget submit to create the entity.
    $this->populateTripalEntityFormState($form_object, $form_state, $current_scenario['create']['user_input']);
    // -- now validate it with the data provided.
    $form_object->validateForm($form, $form_state);
    $errors = $form_state->getErrors();
    $this->assertCount(0, $errors, "We got errors when we submitted the CREATE form with the supplied values. Errors: " . print_r($errors, TRUE));
    // -- if there are no errors then submit the form.
    if (count($errors) === 0) {
      $form_object->submitForm($form, $form_state);
      $form_object->save($form, $form_state);
      // -- confirm the entity matches our expectations.
      $entity = $form_object->getEntity();
      $this->assertFieldValuesMatch($current_scenario['create']['expected_field_values'], $entity, "Field values didn't match our expectations after saving the create form");
    }

    // 3. Test the edit form is generated properly.
    $form = $form_object = $form_state = NULL;
    // Setup an edit Tripal entity form to interact with (test defaults).
    [$form_object, $form, $form_state] = $this->setupTripalEntityEditForm($this->bundle_name, $entity);
    // Confirm all the keys we expect are set to the values we expect.
    $this->assertFieldWidgetsMatch($current_scenario['edit']['expected_form_keys'], $this->system_under_test['fields'], $form, $current_scenario['label'] . ' [EDIT]');

    // 4. Test editing the entity through the form.
    $this->populateTripalEntityFormState($form_object, $form_state, $current_scenario['edit']['user_input']);
    // -- now validate it with the data provided.
    $form_object->validateForm($form, $form_state);
    $errors = $form_state->getErrors();
    $this->assertCount(0, $errors, "We got errors when we submitted the EDIT form with the supplied values. Errors: " . print_r($errors, TRUE));
    // -- if there are no errors then submit the form.
    if (count($errors) === 0) {
      $form_object->submitForm($form, $form_state);
      $form_object->save($form, $form_state);
      // -- confirm the entity matches our expectations.
      $entity = $form_object->getEntity();
      $this->assertFieldValuesMatch($current_scenario['edit']['expected_field_values'], $entity, "Field values didn't match our expectations after saving the edit form");
    }
  }

}
