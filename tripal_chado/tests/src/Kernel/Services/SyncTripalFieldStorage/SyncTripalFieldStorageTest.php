<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\pgsql\Driver\Database\pgsql\Connection;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests the SyncTripalFieldStorage service.
 *
 * @group SyncTripalFieldStorage
 */
#[Group('SyncTripalFieldStorage')]
class SyncTripalFieldStorageTest extends ChadoTestKernelBase {

  /**
   * The theme under which testing should be done.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * The modules that this test relies on.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal', 'tripal_chado', 'views', 'field', 'field_ui'];

  /**
   * Connection to the test chado instance.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection;
   */
  protected ChadoConnection $chado_connection;

  /**
   * Connection to the test drupal instance.
   *
   * @var Drupal\pgsql\Driver\Database\pgsql\Connection
   */
  protected Connection $drupal_connection;

  /**
   * Provides information about specific versions of Chado to test for
   * discrepancies against field installed on Chado 1.3.
   *
   * @return array
   *   Each entry in this array is a specific scenario to be tested.
   *   Each scenario has the following elements:
   *   - chado_verison_under_test
   *   The version of chado we want to test for differences from Chado 1.3.
   *   - bundles_to_create
   *   An array of bundles to create for the test. The bundles should be a
   *   string of the format CATEGORY.BUNDLENAME. Each bundle will be created
   *   when the test schema is at 1.3 using createContentTypeFromConfig().
   *   - bundle_under_test
   *   The bundle name to pass into our test function.
   *   - expectations
   *   An array of expectations. Specifically,
   *     - num_fields: the number of fields with detected differences.
   *     - differences: the keys are field names with differences and the value
   *       for each is a list of property names with differences for that field.
   */
  public static function provideChadoVersionsToTest() {
    $scenarios = [];

    $scenarios[] = [
      '1.3.3.013',
      ['general_chado.project'],
      'project',
      [
        'num_fields' => 4,
        'differences' => [
          'project_analysis' => ['linker_type_id'],
          'project_contact' => ['linker_type_id', 'linker_rank'],
          'project_pub' => ['linker_type_id', 'linker_rank'],
          'project_dbxref' => ['linker_type_id'],
        ],
      ],
    ];

    $scenarios[] = [
      '1.3.3.013',
      ['general_chado.project'],
      NULL,
      [
        'num_fields' => 4,
        'differences' => [
          'project_analysis' => ['linker_type_id'],
          'project_contact' => ['linker_type_id', 'linker_rank'],
          'project_pub' => ['linker_type_id', 'linker_rank'],
          'project_dbxref' => ['linker_type_id'],
        ],
      ],
    ];

    return $scenarios;
  }

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm', 'TripalEntity']);
    // -- additionally we need tripal_chado config to access the yaml files.
    $this->installConfig('tripal_chado');

    // Create chado 1.3 test instance which will later be upgraded.
    $this->chado_connection = $this->createTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO, '1.3');
    $this->drupal_connection = \Drupal::service('database');

    // Add terms needed.
    // @todo update the createContentType method to add terms needed!
    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach (['local', 'SIO', 'schema', 'data', 'NCIT', 'operation', 'OBCS', 'SWO', 'IAO', 'TPUB', 'SBO', 'sep', 'ERO', 'EFO'] as $termIdSpace) {
      $idsmanager->createCollection($termIdSpace, "chado_id_space");
    }
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    foreach (['local', 'SIO', 'schema', 'EDAM', 'ncit', 'OBCS', 'swo', 'IAO', 'tripal_pub', 'sbo', 'sep', 'ero', 'efo'] as $termVocab) {
      $vmanager->createCollection($termVocab, "chado_vocabulary");
    }

  }

  /**
   * Tests detectDifferences() + resolveDetectedDifferences().
   *
   * @dataProvider provideChadoVersionsToTest
   *
   * @param string $chado_verison_under_test
   *   The version of chado we want to test for differences from Chado 1.3.
   * @param array $bundles_to_create
   *   An array of bundles to create for the test. The bundles should be a
   *   string of the format CATEGORY.BUNDLENAME. Each bundle will be created
   *   when the test schema is at 1.3 using createContentTypeFromConfig().
   * @param string|null $bundle_under_test
   *   The bundle name to pass into our test function.
   * @param array $expectations
   *   An array of expectations. Specifically,
   *    - num_fields: the number of fields with detected differences.
   *    - differences: the keys are field names with differences and the value
   *      for each is a list of property names with differences for that field.
   */
  public function testDetectDifferences(string $chado_verison_under_test, array $bundles_to_create, string|null $bundle_under_test, array $expectations) {

    // Create an instance of the specified bundle(s) with all associated fields.
    foreach ($bundles_to_create as $bundle_details) {
      [$bundle_category, $bundle_name] = explode('.', $bundle_details);
      $this->createContentTypeFromConfig($bundle_category, $bundle_name, TRUE);
    }

    // Upgrade the test environment to the specified chado version.
    $this->upgradeTestSchema($this->chado_connection, '1.3', $chado_verison_under_test);
    $this->assertEquals(
      $chado_verison_under_test,
      $this->chado_connection->getVersion(),
      "We were unable to upgrade our test schema to the version we intended to."
    );

    // Now actually call the method!
    $syncTripalFieldStorageService = \Drupal::service('tripal.sync_tripal_field_storage');
    $differences = $syncTripalFieldStorageService->detectDifferences($bundle_under_test);
    $this->assertNotEmpty($differences, "We expected some differences based on the version under test.");

    $this->assertCount($expectations['num_fields'], $differences, "We expected this many fields to have differences detected.");

    // Now check all the expected differences are present.
    $schema = $this->drupal_connection->schema();
    foreach ($expectations['differences'] as $expected_field => $expected_properties) {
      $this->assertArrayHasKey($expected_field, $differences, "We expected this field to have a difference detected but it did not.");
      foreach ($expected_properties as $expected_property) {
        $this->assertArrayHasKey($expected_property, $differences[$expected_field], "We expected this property of $expected_field to have a difference but it did not.");

        // Confirm that this field was NOT added to the drupal table
        // since we asked to check for differences but not to resolve them.
        $property_difference = $differences[$expected_field][$expected_property];
        $column_exists = $schema->fieldExists(
          $property_difference['drupal_table'],
          $property_difference['column_name']
        );
        $column = $property_difference['drupal_table'] . '.' . $property_difference['column_name'];
        $this->assertFalse($column_exists, "The $column column should still not exist because we checked for differences but did not choose to resolve them.");
      }
    }

    // Now try to fix the already checked differences.
    $syncTripalFieldStorageService->resolveDetectedDifferences($differences);

    // Now check that the differences were actually fixed.
    foreach ($expectations['differences'] as $expected_field => $expected_properties) {
      foreach ($expected_properties as $expected_property) {

        $property_difference = $differences[$expected_field][$expected_property];
        $column_exists = $schema->fieldExists(
          $property_difference['drupal_table'],
          $property_difference['column_name']
        );
        $column = $property_difference['drupal_table'] . '.' . $property_difference['column_name'];
        $this->assertTrue($column_exists, "The $column column should now exist because we asked to resolve the differences.");
      }
    }
  }

  /**
   * Tests detectDifferences() + resolveDetectedDifferences().
   *
   * @dataProvider provideChadoVersionsToTest
   *
   * @param string $chado_verison_under_test
   *   The version of chado we want to test for differences from Chado 1.3.
   * @param array $bundles_to_create
   *   An array of bundles to create for the test. The bundles should be a
   *   string of the format CATEGORY.BUNDLENAME. Each bundle will be created
   *   when the test schema is at 1.3 using createContentTypeFromConfig().
   * @param string|null $bundle_under_test
   *   The bundle name to pass into our test function.
   * @param array $expectations
   *   An array of expectations. Specifically,
   *    - num_fields: the number of fields with detected differences.
   *    - differences: the keys are field names with differences and the value
   *      for each is a list of property names with differences for that field.
   */
  public function testResolveDifferences(string $chado_verison_under_test, array $bundles_to_create, string|null $bundle_under_test, array $expectations) {

    // Create an instance of the specified bundle(s) with all associated fields.
    foreach ($bundles_to_create as $bundle_details) {
      [$bundle_category, $bundle_name] = explode('.', $bundle_details);
      $this->createContentTypeFromConfig($bundle_category, $bundle_name, TRUE);
    }

    // Upgrade the test environment to the specified chado version.
    $this->upgradeTestSchema($this->chado_connection, '1.3', $chado_verison_under_test);
    $this->assertEquals(
      $chado_verison_under_test,
      $this->chado_connection->getVersion(),
      "We were unable to upgrade our test schema to the version we intended to."
    );

    // Now actually call the method!
    $syncTripalFieldStorageService = \Drupal::service('tripal.sync_tripal_field_storage');
    $differences = $syncTripalFieldStorageService->resolveDifferences($bundle_under_test);
    $this->assertNotEmpty($differences, "We expected some differences based on the version under test.");

    $this->assertCount($expectations['num_fields'], $differences, "We expected this many fields to have differences detected.");

    // Now check all the expected differences are present.
    $schema = $this->drupal_connection->schema();
    foreach ($expectations['differences'] as $expected_field => $expected_properties) {
      $this->assertArrayHasKey($expected_field, $differences, "We expected this field to have a difference detected but it did not.");
      foreach ($expected_properties as $expected_property) {
        $this->assertArrayHasKey($expected_property, $differences[$expected_field], "We expected this property of $expected_field to have a difference but it did not.");

        // Confirm that this field WAS added to the drupal table
        // since we asked to resolve the differences directly.
        $property_difference = $differences[$expected_field][$expected_property];
        $column_exists = $schema->fieldExists(
          $property_difference['drupal_table'],
          $property_difference['column_name']
        );
        $column = $property_difference['drupal_table'] . '.' . $property_difference['column_name'];
        $this->assertTrue($column_exists, "The $column column SHOULD exist because we asked to resolve the differences as we were checking.");
      }
    }
  }
}
