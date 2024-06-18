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
  protected $type = 'gene';

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
      'label' => 'Gene',
      'termIdSpace' => 'SO',
      'termAccession' => '0000704',
      'category' => 'Genomic',
      'id' => $this->type,
      'help_text' => 'Use the gene page for a region (or regions) that includes all of the sequence elements necessary to encode a functional transcript. A gene may include regulatory regions, transcribed regions and/or other functional sequence regions.',
    ]);
    $content_type->setThirdPartySetting('tripal', 'chado_base_table', 'feature');
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
    $this->createTripalTerm([
      'vocab_name' => 'SO',
      'id_space_name' => 'SO',
      'term' => [
        'name' => 'sequence_feature',
        'definition' => 'Any extent of continuous biological sequence.',
        'accession' => '0000110',
        ]
      ],
      'chado_id_space',
      'chado_vocabulary'
    );
    $this->createTripalTerm([
      'vocab_name' => 'local',
      'id_space_name' => 'local',
      'term' => [
        'name' => 'is_analysis',
        'accession' => 'is_analysis',
        ]
      ],
      'chado_id_space',
      'chado_vocabulary'
    );
    $this->createTripalTerm([
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'name' => 'name',
        'accession' => 'name',
        ]
      ],
      'chado_id_space',
      'chado_vocabulary'
    );
    $this->createTripalTerm([
      'vocab_name' => 'edam',
      'id_space_name' => 'data',
      'term' => [
        'name' => 'Identifier',
        'accession' => '0842',
        ]
      ],
      'chado_id_space',
      'chado_vocabulary'
    );
  }

  /**
   * Data Provider: provides a list of the basic fields to test.
   */
  public function provideFieldsToTest() {
    $sets = [];

    $sets[] = [
      'chado_integer_type_default',
      ['feature_id','dbxref_id','organism_id','seqlen','type_id'],
    ];

    $sets[] = [
      'chado_boolean_type_default',
      ['is_analysis', 'is_obsolete'],
    ];

    $sets[] = [
      'chado_string_type_default',
      ['name'],
    ];

    $sets[] = [
      'chado_text_type_default',
      ['uniquename','residues'],
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
      // The category of field we want to create.
      // This is a collection of radio inputs in the form where all of them have the
      // name=new_storage_type and the value indicates the storage type you want to select.
      // To indicate a Chado Field we select the one where the category machine name=tripal_chado.
      'new_storage_type' => 'tripal_chado',
    ];
    $this->submitForm($input, 'Continue');

    // Confirm that after submission, there
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

    // Step  1c: Actually indicate the name/label of our new field.
    $input = [
      // Machine name is a hidden input where the input name=field_name
      'field_name' => $details['name'],
      // Field label is an input where the name=label
      'label' => $details['label'],
    ];
    $this->submitForm($input, 'Continue');

    // Step 2: Fill in the Storage settings
    // -- Confirm the field label is set from the previous step
    $this->assertSession()->fieldValueEquals('label', $details['label']);
    // -- Confirm the select list for base table is disabled and set properly.
    $base_table_select = 'field_storage[subform][settings][storage_plugin_settings][base_table]';
    $this->assertSession()->fieldDisabled($base_table_select);
    $this->assertSession()->fieldValueEquals($base_table_select, 'feature');
    // -- Confirm the select list base column exists + has the appropriate options.
    $base_col_select = 'field_storage[subform][settings][storage_plugin_settings][base_column]';
    $this->assertSession()->fieldEnabled($base_col_select);
    foreach ($valid_options as $option) {
      $this->assertSession()->optionExists($base_col_select, $option);
    }
    // Now fill out the remaining parts of this form.
    // Each key here indicates the form element to apply the value to.
    // It can be either the id, name, label, or value of the form element.
    $input = [
      $base_table_select => 'feature',
      $base_col_select => $valid_options[0],
      'description' => 'This is the help text for the field.',
      'settings[field_term_fs][vocabulary_term]' => 'comment (schema:comment)',
    ];
    $this->submitForm($input, 'Save settings');

    // Finally assert the field exists on the overview.
    $this->assertFieldExistsOnOverview($details['label']);
  }
}
