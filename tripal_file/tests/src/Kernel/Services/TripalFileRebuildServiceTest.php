<?php

namespace Drupal\Tests\tripal\Kernel\Services;

use Drupal\Core\Extension\ModuleHandler;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use Symfony\Component\Yaml\Yaml;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the rebuild hooks and services.
 *
 * @group service-rebuild
 * @group tripal-file
 */
#[Group('service-rebuild')]
#[Group('tripal-file')]
#[RunTestsInSeparateProcesses]
class TripalFileRebuildServiceTest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'tripal', 'tripal_chado', 'tripal_file', 'views'];

  /**
   * Module handler service.
   *
   * @var Drupal\Core\Extension\ModuleHandler
   */
  protected ModuleHandler $module_handler;

  /**
   * The test chado connection.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Initialize services.
    $this->module_handler = $this->container->get('module_handler');
    $this->chado_connection = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY);
    $this->installSchema('tripal_chado', ['tripal_custom_tables']);

    // To test tripal 3 table migration, create one table as it
    // would exist there, without type_id and rank columns, and create
    // the fileloc table as it would exist if the site never ran
    // tripal 3 update hooks 7101 and 7102.
    $ts = $this->chado_connection->getSchemaName();
    $statements = [];
    $statements[] = "CREATE TABLE $ts.file_contact (file_contact_id integer NOT NULL, file_id integer NOT NULL, contact_id integer)";
    $statements[] = "CREATE SEQUENCE $ts.file_contact_file_contact_id_seq AS integer START WITH 1 INCREMENT BY 1 NO MINVALUE NO MAXVALUE CACHE 1;";
    $statements[] = "ALTER SEQUENCE $ts.file_contact_file_contact_id_seq OWNED BY $ts.file_contact.file_contact_id";
    $statements[] = "CREATE TABLE $ts.fileloc (fileloc_id integer NOT NULL, file_id integer NOT NULL, uri text NOT NULL, rank integer DEFAULT 0 NOT NULL, md5checksum character(32), size character varying(1024))";
    $statements[] = "CREATE SEQUENCE $ts.fileloc_fileloc_id_seq AS integer START WITH 1 INCREMENT BY 1 NO MINVALUE NO MAXVALUE CACHE 1";
    $statements[] = "ALTER SEQUENCE $ts.fileloc_fileloc_id_seq OWNED BY $ts.fileloc.fileloc_id";
    foreach ($statements as $sql) {
      $this->chado_connection->query($sql, []);
    }
  }

  /**
   * Tests the rebuild service.
   *
   * The rebuild service is called by hook_rebuild in any modules
   * that implement it, so we call it that way.
   */
  public function testTripalChadoRebuild() {

    $config = Yaml::parseFile(__DIR__ . '/TripalFileRebuildServiceTest.yml');

    // Check that rebuild creates tables.
    // Before rebuild they will not exist.
    foreach ($config['expected_tables'] as $expect) {
      $table_exists = $this->chado_connection->schema()->tableExists($expect['name']);
      $this->assertEquals($expect['before']['exists'], $table_exists, 'The existence of the ' . $expect['name'] . ' table is not as expected');
      if ($table_exists) {
        $this->checkStatus($expect['name'], $expect['before']);
      }
    }

    // Execute hook_rebuild.
    // Find and execute all hook_rebuild instances in any installed modules.
    // Tripal rebuild hooks return the module name as a return value.
    $results = $this->module_handler->invokeAll('rebuild');
    foreach ($config['expected_modules'] as $module) {
      $this->assertContains($module, $results, "The rebuild hook for module \"$module\" was not invoked by invokeAll");
    }

    // Check that rebuild created or updated tables.
    foreach ($config['expected_tables'] as $expect) {
      $table_exists = $this->chado_connection->schema()->tableExists($expect['name']);
      $this->assertEquals($expect['after']['exists'], $table_exists, 'The ' . $expect['name'] . ' table was not created on rebuild');
      if ($table_exists) {
        $this->checkStatus($expect['name'], $expect['after']);
      }
    }
  }

  /**
   * Checks type_id and rank column status in a table.
   *
   * @param string $table_name
   *   The name of the table.
   * @param array $expect
   *   Expected state.
   *
   * @return void
   *   No return value.
   */
  protected function checkStatus(string $table_name, array $expect): void {
    $args = [
      'format' => 'drupal',
      'source' => 'database',
      'clear' => TRUE,
    ];
    $table_schema = $this->chado_connection->schema()->getTableDef($table_name, $args);
    $this->assertEquals($expect['type_id'], array_key_exists('type_id', $table_schema['fields']),
      'Presence of column type_id in table ' . $table_name . ' does not match expectation.');
    $this->assertEquals($expect['rank'], array_key_exists('rank', $table_schema['fields']),
      'Presence of column rank in table ' . $table_name . ' does not match expectation.');
    $this->assertEquals($expect['filename'] ?? FALSE, array_key_exists('filename', $table_schema['fields']),
      'Presence of column filename in table ' . $table_name . ' does not match expectation.');
  }

}
