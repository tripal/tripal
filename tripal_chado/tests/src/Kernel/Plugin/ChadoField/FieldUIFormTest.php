<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField;

use Drupal\Core\Form\FormState;
use Drupal\field_ui\Form\FieldStorageAddForm;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests adding and editing field instances on a given content type.
 *
 * More specifically, tests in this class are focused on the user interface
 * form process described here. The administrator,
 * 1. Navigates to the "Manage Fields" page for a given TripalEntityType:
 *    Route: entity.tripal_entity.field_ui_fields
 *    Controller: \Drupal\field_ui\Controller\FieldConfigListController::listing
 * 2. Clicks "Create a new field" which loads a page listing the field
 *    type categories:
 *    Route: field_ui.field_storage_config_add_tripal_entity
 *    Controller: Drupal\field_ui\Controller\FieldStorageAddController::getFieldSelectionLinks
 * 3. Chooses the category "Chado Fields" which triggers AJAX loading the page:
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
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[RunTestsInSeparateProcesses]
class FieldUIFormTest extends ChadoTestKernelBase {

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
  protected string $yaml_info_file = __DIR__ . '/FieldStaticMethodTest-TestInfo.yml';

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

    // First create the bundles.
    // We do it this way rather then letting createFieldInstance() do it for us
    // so that we can set the chado base table.
    foreach ($this->system_under_test['bundles'] as $chado_table) {
      $this->tripalEntityType[$chado_table] = $this->createTripalContentType([
        'id' => $chado_table,
      ]);
      $this->tripalEntityType[$chado_table]->setThirdPartySetting('tripal', 'chado_base_table', $chado_table);
      $this->tripalEntityType[$chado_table]->save();
    }

    // Now create the fields.
    // createFieldInstance() will save all fields created to the fieldConfig.
    // property which is keyed by field name. The field names will be
    // generated and saved in each scenario as will the other values.
    $options = [
      'vocab_plugin_id' => 'chado_vocabulary',
      'idspace_plugin_id' => 'chado_id_space',
    ];
    foreach (array_keys($this->scenarios) as $scenario_key) {
      $scenario_bundle_name = $this->scenarios[$scenario_key]['bundle_name'];
      $this->scenarios[$scenario_key]['bundle'] = $this->tripalEntityType[$scenario_bundle_name];
      $this->createFieldInstance($scenario_bundle_name, $this->scenarios[$scenario_key], $options);
    }

  }

  /**
   * Tests the static methods of the chado field types.
   */
  public function testForm() {

    $scenario = $this->scenarios[0];

    // Start with the FieldStorageAddForm which collects just enough information
    // to create an unconfigured field instance.
    // Step 3 described in the test docblock.
    $form_builder = \Drupal::formBuilder();
    $form_state = new FormState();
    $form_state->set('entity_type_id', 'tripal_entity');
    $form_state->set('bundle', $scenario['bundle_name']);
    // This is where the form object is created and set in the form state.
    $form = $form_builder->buildForm(FieldStorageAddForm::class, $form_state);
    $form_object = $form_state->getFormObject();
    // Step 4 described in the test docblock.
    $form_state->setValueForElement($form['field_name'], $scenario['field_name']);
    $form_state->setValueForElement($form['label'], $scenario['label']);
    $form_state->set('field_type', $scenario['field_type']);
    // This is where the tempstore is set.
    $form_object->validateForm($form, $form_state);
    // Step 5a described in the test docblock.
    $temp_store = $this->container->get('tempstore.private')->get('field_ui');
    $stored_values = $temp_store->get('tripal_entity:' . $scenario['field_name']);
    $entity_type_manager = $this->container->get('entity_type.manager');
    $field_config = $entity_type_manager
      ->getStorage('field_config')
      ->create(
        ['field_storage' => $stored_values['field_storage']] + $stored_values['field_config_values'],
      );
    // Step 5b described in the test block.
    $entity_form_builder = $this->container->get('entity.form_builder');
    $fieldconfig_form = $entity_form_builder->getForm(
      $field_config,
      'default',
      ['default_options' => $stored_values['default_options']],
    );
    // @debug print_r(array_keys($fieldconfig_form));
    $this->assertArrayHasKey('field_storage', $fieldconfig_form, "We expect the field config form to have a field storage subform.");

  }

}
