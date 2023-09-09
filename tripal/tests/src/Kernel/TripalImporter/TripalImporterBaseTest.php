<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\KernelTests\KernelTestBase;
use \Drupal\Tests\user\Traits\UserCreationTrait;

/**
 * Tests the base functionality for importers.
 *
 * Cannot test actually implemented importers as those
 * require database specific implementations.
 */
class TripalImporterBaseTest extends KernelTestBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal'];

  use UserCreationTrait;

  /**
   * Annotations associated with the mock_plugin.
   * @var Array
   */
  protected $plugin_definition = [
    'id' => 'fakeImporterName',
    'label' => 'Gemstone Loader',
    'description' => 'Imports details on the incredible diversity of gemstones created by our earth into Chado.',
    'file_types' => ["gem", "txt"],
    'upload_description' => "Please provide a plain text, tab-delimited file of gemstone descriptions making sure to include the details which make them most unique and beautiful.",
    'upload_title' => 'Gemstone Descriptions',
    'use_analysis' => FALSE,
    'require_analysis' => FALSE,
    'button_text' => 'Import file',
    'file_upload' => FALSE,
    'file_load' => FALSE,
    'file_remote' => FALSE,
    'file_required' => FALSE,
    'cardinality' => 1,
  ];

  protected $test_file;

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
    $this->installSchema('tripal', ['tripal_import']);

    // Create and log-in a user.
    $this->setUpCurrentUser();

    // Create a managed file to use as needed.
    $filepath = 'temporary://Файл для тестирования ' . $this->randomMachineName();
    $contents = "file_put_contents() doesn't seem to appreciate empty strings so let's put in some data.";
    file_put_contents($filepath, $contents);
    $file = \Drupal\file\Entity\File::create([
      'uri' => $filepath,
      'uid' => 1,
    ]);
    $file->save();
    $this->assertFileExists($filepath);
    $this->test_file = $file;

  }

	/**
   * Tests focusing on the Tripal Importer plugin system.
   *
   * @group tripal_importer
   */
  public function testTripalImporterManager() {

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

	}

  /**
   * Tests focusing on the Tripal importer base class.
   * Specifically, create(), load(), and getArguments() methods.
   *
   * @group tripal_importer
   */
  public function testTripalImporterBase() {

    // CASE --- Valid
    // -- Empty run args + no file.
    $expected_args = ['run_args' => [], 'files' => []];
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    // Execute create, file not required so expected to succeed.
    $run_args = [];
    $file_details = [];
    $import_id = $importer->create($run_args, $file_details);

    // Now check that a record was added to the tripal_import table.
    $public = \Drupal::database();
    $query = $public->select('tripal_import', 'ti');
    $query->fields('ti', ['uid', 'class', 'fid', 'arguments']);
    $query->condition('import_id', $import_id, '=');
    $records = $query->execute()
      ->fetchAll();
    $this->assertCount(1, $records,
      "We should have a single record in the tripal_import table.");
    $this->assertEquals($plugin_id, $records[0]->class,
      "The class should match our fake plugin name.");
    $selected_args = unserialize(base64_decode($records[0]->arguments));
    $this->assertIsArray($selected_args,
      "Unable to retrieve arguements after creating tripal importer record.");
    $this->assertEquals($expected_args, $selected_args,
      "We did not retreive the arguements we expected.");

    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );
    $importerTestLoad->load($import_id);
    $retrieved_args = $importerTestLoad->getArguments();
    $this->assertIsArray($retrieved_args,
      "Unable to retrieve arguements after loading tripal importer.");
    $this->assertEquals($expected_args, $retrieved_args,
      "We did not retreive the arguements we expected after loading.");

    // CASE --- Exception Expected
    // -- Empty run args, no file when file required.
    $plugin_defn = $this->plugin_definition;
    $plugin_defn['file_required'] = TRUE;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    // Execute create, file required so expected to FAIL.
    $exception_msg = NULL;
    try {
      $run_args = [];
      $file_details = [];
      $import_id = $importer->create($run_args, $file_details);
    }
    catch(\Exception $e) {
      $exception_msg = $e->getMessage();
    }
    $this->assertNotNull($exception_msg,
      "We did not recieve an exception when trying to create with no file + file_required is TRUE.");
    $this->assertStringContainsString('Must provide a proper file', $exception_msg,
      "We did not get the exception we expected when trying to create with no file + file_required is TRUE.");

    // CASE --- Valid
    // -- run args + local file.
    $test_file_path = $this->test_file->getFileUri();
    $expected_args = [
      'run_args' => ['test' => 'single run arg'],
      'files' => [['file_local' => $test_file_path, 'file_path' => $test_file_path]]
    ];
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    // Execute create, file not required so expected to succeed.
    $run_args = ['test' => 'single run arg'];
    $file_details = ['file_local' => $test_file_path];
    $import_id = $importer->create($run_args, $file_details);

    // Now check that a record was added to the tripal_import table.
    $public = \Drupal::database();
    $query = $public->select('tripal_import', 'ti');
    $query->fields('ti', ['uid', 'class', 'fid', 'arguments']);
    $query->condition('import_id', $import_id, '=');
    $records = $query->execute()
      ->fetchAll();
    $this->assertCount(1, $records,
      "We should have a single record in the tripal_import table.");
    $this->assertEquals($plugin_id, $records[0]->class,
      "The class should match our fake plugin name.");
    $selected_args = unserialize(base64_decode($records[0]->arguments));
    $this->assertIsArray($selected_args,
      "Unable to retrieve arguements after creating tripal importer record.");
    $this->assertEquals($expected_args, $selected_args,
      "We did not retreive the arguements we expected.");

    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );
    $importerTestLoad->load($import_id);
    $retrieved_args = $importerTestLoad->getArguments();
    $this->assertIsArray($retrieved_args,
      "Unable to retrieve arguements after loading tripal importer.");
    $this->assertEquals($expected_args, $retrieved_args,
      "We did not retreive the arguements we expected after loading.");

    // CASE --- Valid
    // -- run args + remote file.
    $test_file_path = 'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt';
    $expected_args = [
      'run_args' => ['test' => 'single run arg'],
      'files' => [['file_remote' => $test_file_path]]
    ];
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    // Execute create, file not required so expected to succeed.
    $run_args = ['test' => 'single run arg'];
    $file_details = ['file_remote' => $test_file_path];
    $import_id = $importer->create($run_args, $file_details);

    // Now check that a record was added to the tripal_import table.
    $public = \Drupal::database();
    $query = $public->select('tripal_import', 'ti');
    $query->fields('ti', ['uid', 'class', 'fid', 'arguments']);
    $query->condition('import_id', $import_id, '=');
    $records = $query->execute()
      ->fetchAll();
    $this->assertCount(1, $records,
      "We should have a single record in the tripal_import table.");
    $this->assertEquals($plugin_id, $records[0]->class,
      "The class should match our fake plugin name.");
    $selected_args = unserialize(base64_decode($records[0]->arguments));
    $this->assertIsArray($selected_args,
      "Unable to retrieve arguements after creating tripal importer record.");
    $this->assertEquals($expected_args, $selected_args,
      "We did not retreive the arguements we expected.");

    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );
    $importerTestLoad->load($import_id);
    $retrieved_args = $importerTestLoad->getArguments();
    $this->assertIsArray($retrieved_args,
      "Unable to retrieve arguements after loading tripal importer.");
    $this->assertEquals($expected_args, $retrieved_args,
      "We did not retreive the arguements we expected after loading.");

    // CASE --- Valid
    // -- run args + file upload (single file).
    $test_file_path = $this->test_file->getFileUri();
    $test_file_path = \Drupal::service('file_system')->realpath($test_file_path);
    $test_fid = $this->test_file->Id();
    $expected_args = [
      'run_args' => ['test' => 'single run arg'],
      'files' => [['fid' => $test_fid, 'file_path' => $test_file_path]],
      'file' => ['fid' => $test_fid, 'file_path' => $test_file_path],
    ];
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    // Execute create, file not required so expected to succeed.
    $run_args = ['test' => 'single run arg'];
    $file_details = ['fid' => $test_fid];
    $import_id = $importer->create($run_args, $file_details);

    // Now check that a record was added to the tripal_import table.
    $public = \Drupal::database();
    $query = $public->select('tripal_import', 'ti');
    $query->fields('ti', ['uid', 'class', 'fid', 'arguments']);
    $query->condition('import_id', $import_id, '=');
    $records = $query->execute()
      ->fetchAll();
    $this->assertCount(1, $records,
      "We should have a single record in the tripal_import table.");
    $this->assertEquals($plugin_id, $records[0]->class,
      "The class should match our fake plugin name.");
    $selected_args = unserialize(base64_decode($records[0]->arguments));
    $this->assertIsArray($selected_args,
      "Unable to retrieve arguements after creating tripal importer record.");
    $this->assertEquals($expected_args, $selected_args,
      "We did not retreive the arguements we expected.");

    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );
    $importerTestLoad->load($import_id);
    $retrieved_args = $importerTestLoad->getArguments();
    $this->assertIsArray($retrieved_args,
      "Unable to retrieve arguements after loading tripal importer.");
    $this->assertEquals($expected_args, $retrieved_args,
      "We did not retreive the arguements we expected after loading.");

    // CASE --- Valid
    // -- run args + file upload (multiple files).
    $test_file_path = $this->test_file->getFileUri();
    $test_file_path = \Drupal::service('file_system')->realpath($test_file_path);
    $test_fid = $this->test_file->Id();
    $expected_args = [
      'run_args' => ['test' => 'single run arg'],
      'files' => [
        ['fid' => $test_fid, 'file_path' => $test_file_path],
        ['fid' => $test_fid, 'file_path' => $test_file_path],
        ['fid' => $test_fid, 'file_path' => $test_file_path]
      ],
    ];
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    // Execute create, file not required so expected to succeed.
    $run_args = ['test' => 'single run arg'];
    $file_details = ['fid' => "$test_fid|$test_fid|$test_fid"];
    $import_id = $importer->create($run_args, $file_details);

    // Now check that a record was added to the tripal_import table.
    $public = \Drupal::database();
    $query = $public->select('tripal_import', 'ti');
    $query->fields('ti', ['uid', 'class', 'fid', 'arguments']);
    $query->condition('import_id', $import_id, '=');
    $records = $query->execute()
      ->fetchAll();
    $this->assertCount(1, $records,
      "We should have a single record in the tripal_import table.");
    $this->assertEquals($plugin_id, $records[0]->class,
      "The class should match our fake plugin name.");
    $selected_args = unserialize(base64_decode($records[0]->arguments));
    $this->assertIsArray($selected_args,
      "Unable to retrieve arguements after creating tripal importer record.");
    $this->assertEquals($expected_args, $selected_args,
      "We did not retreive the arguements we expected.");

    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );
    $importerTestLoad->load($import_id);
    $retrieved_args = $importerTestLoad->getArguments();
    $this->assertIsArray($retrieved_args,
      "Unable to retrieve arguements after loading tripal importer.");
    $this->assertEquals($expected_args, $retrieved_args,
      "We did not retreive the arguements we expected after loading.");

    // CASE --- Exception Expected
    // -- Load non-existant importer.
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    $exception_msg = NULL;
    try {
      $run_args = [];
      $file_details = [];
      $import_id = $importerTestLoad->load(999);
    }
    catch(\Exception $e) {
      $exception_msg = $e->getMessage();
    }
    $this->assertNotNull($exception_msg,
      "We did not recieve an exception when trying to load a non-existant importer.");
    $this->assertStringContainsString('Cannot find an importer', $exception_msg,
      "We did not get the exception we expected when trying to load a non-existant.");

    // CASE --- Exception Expected
    // -- Load an importer where the class doesn't match.
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importerTestLoad = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $plugin_defn]
    );

    $mismatched_import_id = $public->insert('tripal_import')
	    ->fields([
        'uid' => 2,
        'class' => 'SomeCompletelyDifferentImporterClass',
        'submit_date' => time(),
      ])
	    ->execute();

    $exception_msg = NULL;
    try {
      $run_args = [];
      $file_details = [];
      $import_id = $importerTestLoad->load($mismatched_import_id);
    }
    catch(\Exception $e) {
      $exception_msg = $e->getMessage();
    }
    $this->assertNotNull($exception_msg,
      "We did not recieve an exception when trying to load a non-existant importer.");
    $this->assertStringContainsString('does not match this importer class', $exception_msg,
      "We did not get the exception we expected when trying to load a non-existant.");

  }

    /**
   * Tests focusing on the Tripal importer base class.
   * Specifically, submitJob() and setJob() methods.
   *
   * @group tripal_importer
   */
  public function testTripalImporterBaseJobs() {

    // Mock Tripal Importer Plugin.
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $this->plugin_definition]
    );
  }

  /**
   * Tests focusing on the Tripal importer base class.
   * Specifically, prepareFiles() and cleanFile() methods.
   *
   * @group tripal_importer
   */
  public function testTripalImporterBaseFiles() {

    // Mock Tripal Importer Plugin.
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $this->plugin_definition]
    );
  }

  /**
   * Tests focusing on the Tripal importer base class.
   * Specifically, setTotalItems(), addItemsHandled(), setItemsHandled(), setInterval().
   *
   * @group tripal_importer
   */
  public function testTripalImporterBaseProgress() {

    // Mock Tripal Importer Plugin.
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $importer = $this->getMockForAbstractClass(
      '\Drupal\tripal\TripalImporter\TripalImporterBase',
      [$configuration, $plugin_id, $this->plugin_definition]
    );
  }
}
