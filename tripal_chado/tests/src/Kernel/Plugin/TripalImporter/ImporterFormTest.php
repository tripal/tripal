<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\TripalImporter;

use Drupal\Core\Url;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

/**
 * Tests the functionality of various importer forms.
 *
 * @group TripalImporter
 * @group ChadoImporter
 */
class ImporterFormTest extends ChadoTestKernelBase {

  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected $connection;

  /**
   * This specifies which inporter forms to test, and special settings to confirm.
   */
  protected $forms_to_test = [
    ['plugin_id' => 'chado_fasta_loader',
     'importer_label' => 'Chado FASTA File Loader',
     'requires_analysis' => TRUE,
    ],
    ['plugin_id' => 'chado_gff3_loader',
     'importer_label' => 'Chado GFF3 File Loader',
     'requires_analysis' => TRUE,
    ],
    ['plugin_id' => 'chado_obo_loader',
     'importer_label' => 'OBO Vocabulary Loader',
     'requires_instructions' => TRUE,
    ],
    ['plugin_id' => 'chado_taxonomy_loader',
     'importer_label' => 'NCBI Taxonomy Loader',
     'requires_instructions' => TRUE,
    ],
    ['plugin_id' => 'chado_tree_generator',
     'importer_label' => 'Taxonomy Tree Generator',
     'requires_instructions' => TRUE,
    ],
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Open connection to Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Ensure we can access file_managed related functionality from Drupal.
    // ... users need access to system.action config?
    $this->installConfig('system');
    // ... managed files are associated with a user.
    $this->installEntitySchema('user');
    // ... Finally the file module + tables itself.
    $this->installEntitySchema('file');
    $this->installSchema('file', ['file_usage']);
    $this->installSchema('tripal_chado', ['tripal_cv_obo']);

  }

  /**
   * Tests focusing on the importer form.
   */
  public function testImporterForm() {

    foreach ($this->forms_to_test as $form_to_test) {
      $plugin_id = $form_to_test['plugin_id'];
      $importer_label = $form_to_test['importer_label'];
      $requires_analysis = $form_to_test['requires_analysis'] ?? FALSE;
      $requires_instructions = $form_to_test['requires_instructions'] ?? FALSE;

      // Build the form using the Drupal form builder.
      $form = \Drupal::formBuilder()->getForm(
        'Drupal\tripal\Form\TripalImporterForm',
        $plugin_id
      );
      // Ensure we are able to build the form.
      $this->assertIsArray($form,
        "For \"$plugin_id\" we expect the form builder to return a form but it did not.");
      $this->assertEquals('tripal_admin_form_tripalimporter', $form['#form_id'],
        "For \"$plugin_id\" we did not get the form id we expected.");

      // Now that we have provided a plugin_id, we expect it to have...
      // title matching our importer label.
      $this->assertArrayHasKey('#title', $form,
        "The \"$plugin_id\" form should have a title set.");
      $this->assertEquals($importer_label, $form['#title'],
        "The \"$plugin_id\" form title should match the label annotated for our plugin.");
      // the plugin_id stored in a value form element.
      $this->assertArrayHasKey('importer_plugin_id', $form,
        "The \"$plugin_id\" form should have an element to save the plugin_id.");
      $this->assertEquals($plugin_id, $form['importer_plugin_id']['#value'],
        "The \"$plugin_id\" form importer_plugin_id[#value] should be set to our plugin_id.");
      // a submit button.
      $this->assertArrayHasKey('button', $form,
        "The \"$plugin_id\" form should have a submit button since we indicated a specific importer.");

      // Check if this importer does or does not require an analysis.
      if ($requires_analysis) {
        $this->assertArrayHasKey('analysis_id', $form,
        "The \"$plugin_id\" form should include an analysis element, yet one does not exist.");
      }
      else {
        $this->assertArrayNotHasKey('analysis_id', $form,
        "The \"$plugin_id\" form should not include an analysis element, yet one exists.");
      }

      // We should also have our importer-specific form elements added to the form!
      if ($requires_instructions) {
        $this->assertArrayHasKey('instructions', $form,
          "The \"$plugin_id\" form should include an instructions form element.");
      }
    }
  }

}
