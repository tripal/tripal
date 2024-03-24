<?php

namespace Drupal\Tests\tripal_chado\Functional\Plugin\Fields\CreateEditFieldForms;

use Drupal\Tests\field_ui\Traits\FieldUiTestTrait;
use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;

/**
 * Tests the configuration for where a basic chado field is added to an existing
 * content type. Specific field types tested are below in the $field_types
 * property --these are fields that act on individual columns in the base table
 * and require their column to be set in the field storage settings.
 *
 * @group ChadoField
 */
class BasicDataTypeChadoFieldTest extends ChadoTestBrowserBase {
  use FieldUiTestTrait;

	protected $defaultTheme = 'stark';

	protected static $modules = ['system', 'user', 'field_ui', 'tripal', 'tripal_chado'];

  /**
   * Provides a list of the field types we are testing with this class.
   */
  protected static array $field_types = [
    'chado_integer_type_default',
    'chado_boolean_type_default',
    'chado_string_type_default',
    'chado_text_type_default',
  ];

  /**
   * The machine name of the Tripal Content Type used by tests in this class.
   */
  protected $type = 'organism';

  protected $connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    // Installs up the chado with all the items added via the prepare.
    // NOTE: This done not prepare Drupal so none of the TripalTerms we need are available.
    $this->connection = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Create the Organism Content Type
    $content_type = $this->createTripalContentType([
      'label' => 'Organism',
      'termIdSpace' => 'OBI',
      'termAccession' => '0100026',
      'category' => 'General',
      'id' => $this->type,
      'help_text' => 'A material entity that is an individual living system, ' .
        'such as animal, plant, bacteria or virus, that is capable of replicating ' .
        'or reproducing, growth and maintenance in the right environment. An ' .
        'organism may be unicellular or made up, like humans, of many billions ' .
        'of cells divided into specialized tissues and organs.',
    ]);
    $content_type->setThirdPartySetting('tripal', 'chado_base_table', 'organism');
    $content_type->save();

    // Create test user.
    $admin_user = $this->drupalCreateUser([
      'access content',
      'administer tripal',
      'manage tripal content types',
      'administer tripal_entity fields',
    ]);
    $this->drupalLogin($admin_user);

    // Create the Tripal Terms we need.
    // -- SIO:000729
    $this->createTripalTerm([
      'vocab_name' => 'SIO',
      'id_space_name' => 'SIO',
      'term' => [
        'name' => 'record identifier',
        'definition' => 'A record identifier is an identifier for a database entry.',
        'accession' =>'000729',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );

  }

  /**
   * Data Provider: proivides a list of the basic fields to test.
   */
  public function provideFieldsToTest() {
    $sets = [];

    $sets[] = [
      'chado_integer_type_default',
      ['organism_id', 'type_id'],
    ];

    /** Can't test on organism.
    $sets[] = [
      'chado_boolean_type_default',
      [],
    ];
    */

    $sets[] = [
      'chado_string_type_default',
      ['abbreviation','genus','species','common_name','infraspecific_name'],
    ];

    $sets[] = [
      'chado_text_type_default',
      ['comment']
    ];

    return $sets;
  }

  /**
   * Tests adding a basic field type via the combined add field form.
   * @dataProvider provideFieldsToTest
   *
   * This specifically refers to the form used in Drupal 10.2+ and thus
   * will only be run on systems of the right Drupal version.
   *
   * NOTE: Debugging pages use the following
   * - to get the full HTML: print $this->getSession()->getPage()->getContent();
   * - to get the plain text without markup: print $this->getTextContent();
   */
  public function testCreateViaCombinedAddFieldForm($field_type_name, $valid_options) {

    // ONLY do this test if we are in Drupal 10.2+ since we are assuming
    // that the field storage + settings form are combined into a single page.
    if (version_compare(\Drupal::VERSION, 10.2) < 0) {
      $this->markTestSkipped('Test only applies to Drupal 10.2+');
    }

    // Pages to access.
    $manage_fields_path = 'admin/structure/bio_data/manage/' . $this->type . '/fields';
    $add_field_path = '/admin/structure/bio_data/manage/' . $this->type . '/fields/add-field';

    // Details of the field to create.
    $unique_suffix = uniqid();
    $details = [
      'name' => 'basic_field_' . $unique_suffix,
      'label' => 'Test Basic Field ' . $unique_suffix,
    ];

    // Go to the manage fields admin page for the organism content type.
    $html = $this->drupalGet($manage_fields_path);
    $this->assertSession()->pageTextContains('Manage fields');
    // Confirm there are no fields yet.
    $this->assertSession()->pageTextContains('No fields are present yet.');

    // Go to the page to add a new field to this content type.
    $html = $this->drupalGet($add_field_path);
    $this->assertSession()->pageTextContains('Add field');

    // Step 1a: Field Name + Category
    // Submit the form with the following input.
    // We have no AJAX in these tests so we have to submit the form after selecting
    // the category in order to see the list of field types in that category.

    // Each key here indicates the form element to apply the value to.
    // It can be either the id, name, label, or value of the form element.
    $input = [
      // Machine name is a hidden input where the input name=field_name
      'field_name' => $details['name'],
      // Field label is an input where the name=label
      'label' => $details['label'],
      // The category of field we want to create.
      // This is a collection of radio inputs in the form where all of them have the
      // name=new_storage_type and the value indicates the storage type you want to select.
      // To indicate a Chado Field we select the one where the category machine name=tripal_chado.
      'new_storage_type' => 'tripal_chado',
    ];
    $this->submitForm($input, 'Continue');

    // Confirm that after submission, there
    // -- there is an error message telling us to select a field type...
    //    even though we couldn't before because this is the test environment.
    $this->assertSession()->statusMessageContains('select a field type');
    // -- is a form element with out field type.
    //    the id of the input element is the machine name of the field.
    $form_element = $this->getSession()->getPage()->findById($field_type_name);
    $this->assertNotNull($form_element,
      "We were not able to find a form element with an id of $field_type_name after choosing the category.");

    // Step 1b: Field Type
    // Submit the form indicating the actual field type we want to create.
    // This is another collection of radio inputs where the field machine name
    // is the id of the input element.
    $input = [
      $field_type_name => $field_type_name,
    ];
    $this->submitForm($input, 'Continue');

    // Step 2: Fill in the Storage settings
    // -- Confirm the field label is set from the previous step
    $this->assertSession()->fieldValueEquals('label', $details['label']);
    // -- Confirm the select list for base table is disabled and set properly.
    $base_table_select = 'field_storage[subform][settings][storage_plugin_settings][base_table]';
    $this->assertSession()->fieldDisabled($base_table_select);
    $this->assertSession()->fieldValueEquals($base_table_select, 'organism');
    // -- Confirm the select list base column exists + has the appropriate options.
    $base_col_select = 'field_storage[subform][settings][storage_plugin_settings][base_column]';
    $this->assertSession()->fieldEnabled($base_col_select);
    foreach ($valid_options as $option) {
      $this->assertSession()->optionExists($base_col_select, $option);
    }

    // @todo check the chado storage setting fields + fill out
    // @todo set the cvterm.
    // @todo submit the form to create the field
    // @todo use $this->assertFieldExistsOnOverview() to confirm the field exists now.
    // @debug print $this->getSession()->getPage()->getContent();
  }
}
