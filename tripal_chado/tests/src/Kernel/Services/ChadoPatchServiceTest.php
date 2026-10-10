<?php

namespace Drupal\Tests\tripal\Kernel\Services;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoPatchService;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Symfony\Component\Yaml\Yaml;

/**
 * Tests the chado patch service.
 *
 * @group service-patch
 */
#[Group('service-patch')]
#[RunTestsInSeparateProcesses]
class ChadoPatchServiceTest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'tripal', 'tripal_chado'];

  /**
   * The test chado connection.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * The chado patch service that we are testing.
   *
   * @var Drupal\tripal_chado\Services\ChadoPatchService
   */
  protected ChadoPatchService $chado_patch_service;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Initialize chado service.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
    $this->chado_patch_service = $this->container->get('tripal_chado.patch_service');
  }

  /**
   * Tests the chado patch service.
   */
  public function testChadoPatchService() {
    $test_yaml = __DIR__ . '/ChadoPatchServiceTest.yml';
    $this->assertFileExists($test_yaml, 'Test constructed incorrectly, this file should exist');

    // CASE: Should return error message if nonexistant yaml file is supplied.
    $result = $this->chado_patch_service->patchChado('');
    $this->assertIsString($result, 'Expect string returned for missing yaml file');
    $this->assertStringContainsString('Error, file "" does not exist', $result, 'Expect error for empty file name');

    // CASE: A non-existing schema should return an error message.
    $result = $this->chado_patch_service->patchChado($test_yaml, 'invalid_schema_name');
    $this->assertIsString($result, 'Expect string returned for invalid schema');
    $this->assertStringContainsString('relation "invalid_schema_name.db" does not exist', $result, 'Expect error for nonexistant schema');

    // CASE: A valid yaml should succeed.
    $result = $this->chado_patch_service->patchChado($test_yaml);
    $this->assertIsInt($result, 'Expect integer for valid yaml');

    // Verify records in the database match the expectations defined
    // in our test yaml file.
    $file_contents = file_get_contents($test_yaml);
    $this->assertNotEmpty($file_contents, "No information read from scenarios yaml file \"$test_yaml\"");
    $yaml_data = Yaml::parse($file_contents);
    foreach ($yaml_data as $record) {
      $this->checkPatchedRecord($record);
    }
  }

  /**
   * Helper function to check that the chado record was updated in the db.
   *
   * @param array $record
   *   One record specification from the yaml file.
   *
   * @return void
   *   No return value, only performs assertions.
   */
  protected function checkPatchedRecord(array $record): void {
    $query = $this->chado_connection->select('1:' . $record['table'], 'T');
    foreach ($record['conditions'] as $condition) {
      $query->condition('T.' . $condition['column'], $condition['value'], '=');
    }
    $query->fields('T', [$record['update_column']]);
    $results = $query->execute()->fetchAll();
    $this->assertEquals($record['expect_count'], count($results), 'Expect record count ' . $record['expect_count'] . ' for record ' . print_r($record, TRUE));
    if ($results) {
      $db_value = $results[0]->{$record['update_column']};
      $this->assertEquals($record['expect_value'], $db_value, 'Value in db not as expected for record ' . print_r($record, TRUE));
    }
  }

}
