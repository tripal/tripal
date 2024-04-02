<?php

namespace Drupal\Tests\tripal_chado\Kernel\Task;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Task\ChadoInstaller;


/**
 * Tests for installer task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoInstaller
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Installer
 */
class ChadoInstallerTest extends ChadoTestKernelBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskInstaller() {
    // Get a temporary schema name.
    $tripaldbx_db = $this->getTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);

    // Test installer.
    $installer = \Drupal::service('tripal_chado.installer');
    $installer->setParameters([
      'output_schemas'  => [$tripaldbx_db->getSchemaName()],
    ]);
    $success = $installer->performTask();
    $this->assertTrue($success, 'Task performed.');
    $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Schema created.');
    $this->assertTrue($tripaldbx_db->schema()->tableExists('stock'), 'Table created.');
    $this->assertTrue($tripaldbx_db->schema()->fieldExists('stock', 'uniquename'), 'Field created.');
    // @todo: test more... (types, functions, views, indexes, frange schema)
    $this->freeTestSchema($tripaldbx_db);

    // Test that the status can be retrieved.
    $status = $installer->getStatus();
    $this->assertEquals('Installation done.', $status,
      "We expect the status to let us know the installation is complete.");
  }

  /**
   * Data Provider: Test invalid parameters.
   */
  public function provideInvalidParameters() {
    $test_set = [];

    $test_set[] = [
      'test_name' => 'Version not a string',
      'parameters' => [
        'output_schemas' => [ 'chado' . uniqid() ],
        'version' => ['fred' => 'sarah'],
      ],
      'messages' => 'version must be a string; whereas, you passed an array or object'
    ];

    $test_set[] = [
      'test_name' => 'Version not valid',
      'parameters' => [
        'output_schemas' => [ 'chado' . uniqid() ],
        'version' => 5.9,
      ],
      'messages' => 'version .*is not supported by this installer'
    ];

    $test_set[] = [
      'test_name' => 'Schema already exists.',
      'parameters' => [
        'output_schemas' => [ 'testchadoschemaexists' ],
        'version' => 1.3,
      ],
      'messages' => 'Target schema ".*" already exists.',
    ];

    $test_set[] = [
      'test_name' => 'Input schema not supported.',
      'parameters' => [
        'input_schemas' => [ 'fred' ],
        'output_schemas' => [ 'chado' . uniqid() ],
        'version' => 1.3,
      ],
      'messages' => 'Chado installer does not take input schemas',
    ];

    $test_set[] = [
      'test_name' => 'Too many output schema.',
      'parameters' => [
        'output_schemas' => [ 'chado' . uniqid(), 'chado' . uniqid() ],
        'version' => 1.3,
      ],
      'messages' => 'Invalid number of output schemas',
    ];

    return $test_set;
  }

  /**
   * Tests task.
   *
   * @dataProvider provideInvalidParameters
   *
   * @cover ::setParameters
   */
  public function testPerformTaskInstallerParameters($test_name, $paramset, $expected_message) {

    $this->tripal_dbx->createSchema('testchadoschemaexists');

    $installer = \Drupal::service('tripal_chado.installer');
    $installer->setParameters($paramset);

    $this->expectException(\Drupal\tripal_biodb\Exception\ParameterException::class, "We expected an exception to be thrown for $test_name.");
    $this->expectExceptionMessageMatches("/$expected_message/",
      "The message thrown by validateParameters was not the one we expected for $test_name.");
    $installer->validateParameters();

  }

  /**
   * {@inheritdoc}
   */
  protected function tearDown(): void {
    $testschema = 'testchadoschemaexists';
    if ($this->tripal_dbx->schemaExists($testschema)) {
      $this->tripal_dbx->dropSchema($testschema);
    }
  }
}
