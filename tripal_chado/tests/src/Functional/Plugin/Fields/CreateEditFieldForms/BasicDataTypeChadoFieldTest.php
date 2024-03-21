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
  protected static $field_types = [
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

    // Installs up the chado with the test chado data
    $connection = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Create the Organism Content Type
    $this->createTripalContentType([
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

    // Create test user.
    $admin_user = $this->drupalCreateUser([
      'access content',
      'administer tripal',
      'manage tripal content types',
      'administer tripal_entity fields',
    ]);
    $this->drupalLogin($admin_user);

  }

  /**
   * Tests adding a basic field type via the combined add field form.
   *
   * This specifically refers to the form used in Drupal 10.2+ and thus
   * will only be run on systems of the right Drupal version.
   */
  public function testCreateViaCombinedAddFieldForm() {
    $manage_fields_path = 'admin/structure/bio_data/manage/' . $this->type . '/fields';
    $add_field_path = '/admin/structure/bio_data/manage/' . $this->type . '/fields/add-field';


    // @todo set this via a phpunit dataprovider and the $field_types
    $field_type_name = 'chado_integer_type_default';

    // Go to the manage fields admin page for the organism content type.
    $html = $this->drupalGet($manage_fields_path);
    $this->assertSession()->pageTextContains('Manage fields');
    // Confirm there are no fields yet.
    $this->assertSession()->pageTextContains('No fields are present yet.');

    // Go to the page to add a new field to this content type.
    $html = $this->drupalGet($add_field_path);
    $this->assertSession()->pageTextContains('Add field');

    // Submit the form with the following input.
    // Each key here indicates the form element to apply the value to.
    // It can be either the id, name, label, or value of the form element.
    $unique_suffix = uniqid();
    $input = [
      // Machine name is a hidden input where the input name=field_name
      'field_name' => 'test_basic_field_' . $unique_suffix,
      // Field label is an input where the name=label
      'label' => 'Test Basic Field ' . $unique_suffix,
      // The category of field we want to create.
      // This is a collection of radio inputs in the form where all of them have the
      // name=new_storage_type and the value indicates the storage type you want to select.
      // To indicate a Chado Field we select the one where the category machine name=tripal_chado.
      'new_storage_type' => 'tripal_chado',
      // The actual field type we want to create.
      // This is another collection of radio inputs where the field machine name
      // is the id of the input element... BUT IT ONLY APPEARS VIA AJAX
      // $field_type_name => TRUE,
    ];
    $this->submitForm($input, 'Continue');
  }
}
