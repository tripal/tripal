<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField;

use Drupal\Core\Form\FormState;
use Drupal\field_ui\Form\FieldStorageAddForm;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests adding and editing field instances on a given content type.
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[RunTestsInSeparateProcesses]
class FieldUiFormTest extends ChadoTestKernelBase {

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
    'field_ui',
    'datetime',
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
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/FieldUiFormTest-TestInfo.yml';

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - chado_version: the version of chado to test under.
   */
  protected array $system_under_test;

  /**
   * Describes the scenarios to test.
   *
   * This will be used in combination with the data provider. It can't be
   * accessed directly in the dataProvider due to the way that PHPUnit is
   * setup.
   *
   * @var array
   *   A list of scenarios where each one has the following keys:
   *   - id: the machine name of the field to be tested.
   *   - class: the class defining the field to be tested.
   */
  protected array $scenarios;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // First retrieve info from the YAML file for this particular test.
    [$this->system_under_test, $this->scenarios] = $this->getTestInfoFromYaml($this->yaml_info_file);

    // Get Chado in place.
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO
    );

    // We need to setup the test environment for creating fields.
    // We don't pass in anything so no bundles/fields will be created.
    $this->setupChadoEntityFieldTestEnvironment();
    $this->installSchema('tripal_chado', ['tripal_custom_tables', 'tripal_mviews']);

    // Create the bundles that our test fields will be attached to.
    foreach ($this->system_under_test['bundles'] as $chado_table) {
      $this->tripalEntityType[$chado_table] = $this->createTripalContentType([
        'id' => $chado_table,
      ]);
      $this->tripalEntityType[$chado_table]->setThirdPartySetting('tripal', 'chado_base_table', $chado_table);
      $this->tripalEntityType[$chado_table]->save();
    }

    // Make sure the placeholder term exists.
    // This is used in step 5a of the test process described in the class
    // docblock since we do not yet have the chado base table set causing
    // tripalTypes() to return an empty array.
    $values = [
      'id_space_name' => 'SIO',
      'term' => [
        'accession' => '000729',
      ],
    ];
    $this->createTripalTerm(
      $values,
      'tripal_default_id_space',
      'tripal_default_vocabulary'
    );

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

    $scenarios[] = [0, 'Analysis through linker table'];
    $scenarios[] = [1, 'Analysis in base table'];
    $scenarios[] = [2, 'Array Design in base table'];
    $scenarios[] = [3, 'Assay through linker table'];
    $scenarios[] = [4, 'Assay in base table'];
    $scenarios[] = [5, 'Biomaterial through linking table'];
    $scenarios[] = [6, 'Biomaterial in base table'];
    $scenarios[] = [7, 'Boolean in base table'];
    $scenarios[] = [8, 'Contact through linker table'];
    $scenarios[] = [9, 'Contact in base table'];
    // $scenarios[] = [10, 'Contact BY TYPE through linker table'];
    $scenarios[] = [11, 'DBXREF in base table'];
    $scenarios[] = [12, 'DBXREF through linking table'];
    $scenarios[] = [13, 'Data Source on only supported base table'];
    $scenarios[] = [14, 'Feature through linker table'];
    $scenarios[] = [15, 'Feature linked in base table'];
    $scenarios[] = [16, 'Feature MAP through linking table'];
    $scenarios[] = [17, 'Sequence Checksum'];
    $scenarios[] = [18, 'Sequence Length'];
    $scenarios[] = [19, 'Integer in base table'];
    $scenarios[] = [20, 'Organism in Base Table'];
    $scenarios[] = [21, 'Organism through linking table'];
    $scenarios[] = [22, 'Project through linker table'];
    $scenarios[] = [23, 'Properties'];
    $scenarios[] = [24, 'Protocol in base table'];
    $scenarios[] = [25, 'Publication in base table'];
    $scenarios[] = [26, 'Publication through linker table'];
    $scenarios[] = [27, 'Relationship typical'];
    $scenarios[] = [28, 'Sequence Coordinates'];
    $scenarios[] = [29, 'Sequence'];
    $scenarios[] = [30, 'Stock through linking table'];
    $scenarios[] = [31, 'String in base table'];
    $scenarios[] = [32, 'Study through linker table'];
    $scenarios[] = [33, 'Synonym through linker table'];
    $scenarios[] = [34, 'Text in base table'];
    $scenarios[] = [35, 'Type in base table'];
    $scenarios[] = [36, 'Type through property table'];
    $scenarios[] = [37, 'Featuremap Unit'];

    return $scenarios;
  }

  /**
   * Tests the static methods of the chado field types.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testForm(int $current_scenario_key, string $current_scenario_label) {
    $scenario = $this->getYamlScenario($current_scenario_key, $current_scenario_label);

    $bundle_name = $scenario['field_details']['bundle_name'];
    $scenario['field_details']['field_label'] ??= ucwords(str_replace('_', ' ', $scenario['field_details']['field_name']));
    [$form_object, $form, $form_state] = $this->setupFieldConfigAddForm($bundle_name, $scenario['field_details']);
    $this->assertIsArray($form, "We were unable to setup the form for adding a field to bundle_name.");
  }

  /**
   * For testing the form that adds fields to a content type.
   *
   * NOTE: this form keeps changing in Drupal core. As such you will see
   * version specific flows when setting it up.
   *
   * @param string $bundle_name
   *   The name of the bundle this form is to add a field to.
   * @param array $field_details
   *   An array providing the details for the field to be created through this
   *   form. Required keys include:
   *   - field_name: the name of the new field instance.
   *   - field_type: the machine name of the field type we are testing.
   *   - field_label: the label of the new field instance.
   *
   * @return array
   *   The parts of the form; specifically,
   *   - $form_object: an object that can be used with the form state to
   *     validate and submit the field add form.
   *   - array $form: the complete form including any subforms, etc.
   *   - FormState $form_state: the state of this form including relationships
   *     to both the form object and form array.
   */
  public function setupFieldConfigAddForm(string $bundle_name, array $field_details): array {

    // FieldStorageAddForm is split into FieldStorageAddController
    // and FieldStorageAddForm.
    // Change introduced in Drupal 11.2.0.
    // @see https://www.drupal.org/node/3503549
    if (TRUE) {
      return $this->setupFieldConfigAddForm3503549($bundle_name, $field_details);
    }

    return [];
  }

  /**
   * Returns the field add form described in Drupal Issue #3503549.
   *
   * Tl;dr FieldStorageAddController collects defaults for FieldStorageAddForm.
   *
   * More specifically, this is the process used when a field is being added.
   * The administrator,
   * 1. Navigates to the "Manage Fields" page for a given TripalEntityType.
   * 2. Clicks "Create a new field" which loads a page listing the field
   *    type categories:
   *    Route: field_ui.field_storage_config_add_tripal_entity
   *    Controller: Drupal\field_ui\Controller\FieldStorageAddController::getFieldSelectionLinks
   * 3. Chooses category "Chado Fields" which triggers AJAX loading the page:
   *    Route: field_ui.field_storage_config_add_sub_tripal_entity
   *    Form: \Drupal\field_ui\Form\FieldStorageAddForm
   * 4. Fills in the label, machine name, and field type (e.g. "Chado Organism")
   *    for the field to be created and clicks "continue".
   * 5. Then the following page is loaded:
   *    Route: field_ui.field_add_tripal_entity
   *    Controller: Drupal\field_ui\Controller\FieldConfigAddController::fieldConfigAddConfigureForm
   * 5a. The controller creates a FieldConfig object based on the details saved
   *    in the "tempStore" by \Drupal\field_ui\Form\FieldStorageAddForm
   *    ::setTempStore() on submission triggered by step 4.
   * 5b. Then it returns the FieldConfig entity form. This page contains the
   *    field settings and field storage settings forms.
   *
   * @param string $bundle_name
   *   The name of the bundle this form is to add a field to.
   * @param array $field_details
   *   An array providing the details for the field to be created through this
   *   form.
   *   @see self::setupFieldConfigAddForm()
   *
   * @return array
   *   The parts of the form; specifically,
   *   - FieldStorageAddForm $form_object: an object that can be used with the
   *     form state to validate and submit the field add form.
   *   - array $form: the complete form including any subforms, etc.
   *   - FormState $form_state: the state of this form including relationships
   *     to both the form object and form array.
   */
  private function setupFieldConfigAddForm3503549(string $bundle_name, array $field_details): array {
    $form_builder = \Drupal::formBuilder();

    // Start with the FieldStorageAddController which collects just enough
    // information to create an unconfigured field instance.
    // Step 3 described in the docblock.
    $form_state = new FormState();
    $form_state->set('entity_type_id', 'tripal_entity');
    $form_state->set('bundle', $bundle_name);
    // This is where the form object is created and set in the form state.
    $form = $form_builder->buildForm(FieldStorageAddForm::class, $form_state);
    $form_object = $form_state->getFormObject();
    // Step 4 described in the docblock.
    $form_state->setValueForElement($form['field_name'], $field_details['field_name']);
    $form_state->setValueForElement($form['label'], $field_details['field_label']);
    $form_state->set('field_type', $field_details['field_type']);
    // This is where the tempstore is set.
    $form_object->validateForm($form, $form_state);
    // Step 5a described in the docblock.
    $temp_store = $this->container->get('tempstore.private')->get('field_ui');
    $stored_values = $temp_store->get('tripal_entity:' . $field_details['field_name']);
    $entity_type_manager = $this->container->get('entity_type.manager');
    $field_config = $entity_type_manager
      ->getStorage('field_config')
      ->create(
        ['field_storage' => $stored_values['field_storage']] + $stored_values['field_config_values'],
      );
    // Step 5b described in the docblock.
    $entity_form_builder = $this->container->get('entity.form_builder');
    $fieldconfig_form = $entity_form_builder->getForm(
      $field_config,
      'default',
      ['default_options' => $stored_values['default_options']],
    );
    // @debug print_r(array_keys($fieldconfig_form));
    $this->assertArrayHasKey('field_storage', $fieldconfig_form, "We expect the field config form to have a field storage subform.");

    return [$form_object, $fieldconfig_form, $form_state];
  }

}
