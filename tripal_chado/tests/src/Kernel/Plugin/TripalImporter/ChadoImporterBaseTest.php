<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\TripalImporter;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Component\FileCache\FileCacheFactory;

/**
 * Tests the base functionality for chado importers.
 *
 * @group TripalImporter
 * @group ChadoImporter
 */
class ChadoImporterBaseTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  use UserCreationTrait;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Ensure we can access file_managed related functionality from Drupal.
    // ... users need access to system.action config?
    $this->installConfig('system');
    // ... managed files are associated with a user.
    $this->installEntitySchema('user');
    // ... Finally the file module + tables itself.
    $this->installEntitySchema('file');
    $this->installSchema('file', ['file_usage']);

    // Ensure we have our tripal import tables.
    $this->installSchema('tripal', ['tripal_import', 'tripal_jobs']);

    // Create and log-in a user.
    $this->setUpCurrentUser();

    // Ensure the file cache is disabled.
    FileCacheFactory::setConfiguration([
      FileCacheFactory::DISABLE_CACHE => TRUE,
    ]);

    // Ensure that FileCacheFactory has a prefix.
    FileCacheFactory::setPrefix('prefix');
  }

  /**
   * Tests focusing on the Tripal Importer plugin system.
   *
   * @group tripal_importer
   */
  public function testTripalImporterManagerForChadoImporters() {

    // These are the importers we expect to have.
    $expected_importers = ['chado_obo_loader', 'chado_taxonomy_loader', 'chado_tree_generator', 'chado_newick_tree_loader', 'chado_fasta_loader', 'chado_gff3_loader'];
    $expected_count = count($expected_importers);
    $expected_annotation = ['id', 'label', 'description', 'file_types', 'use_analysis', 'require_analysis', 'use_button', 'submit_disabled', 'button_text', 'file_upload', 'file_local', 'file_remote', 'file_required'];

    // Test the Tripal Importer Plugin Manager.
    // --Ensure we can instantiate the plugin manager.
    $type = \Drupal::service('tripal.importer');
    // Note: If the plugin manager is not found you will get a ServiceNotFoundException.
    $this->assertIsObject($type, 'An importer plugin service object was not returned.');

    // --Use the plugin manager to get a list of available implementations.
    $plugin_definitions = $type->getDefinitions();
    $this->assertIsArray(
      $plugin_definitions,
      'Implementations of the tripal importer plugin should be returned in an array.'
    );
    $this->assertGreaterThanOrEqual($expected_count, count($plugin_definitions),
      "We expected to at least have the core chado importers listed.");

    // Check Specific Importers.
    foreach ($expected_importers as $expected_importer_name) {
      // Ensure this specific importer is included in those discovered.
      $this->assertArrayHasKey($expected_importer_name, $plugin_definitions,
        "We expected this core importer to be available via plugin discovery but it was not.");

      // Ensure that this specific importer has the annotation we expect.
      $importer_details = $plugin_definitions[$expected_importer_name];
      $this->assertIsArray($importer_details,
        "We expect the importer details returned by getDefinitions for $expected_importer_name to be an array.");
      // Now check that all the expected annotation keys are present.
      // Plugin discovery should add defaults for any not defined by a specific importer.
      foreach ($expected_annotation as $annotation_key) {
        $this->assertArrayHasKey($annotation_key, $importer_details,
          "We expected $expected_importer_name annotation to include this key but it did not.");
      }
    }
  }
}
