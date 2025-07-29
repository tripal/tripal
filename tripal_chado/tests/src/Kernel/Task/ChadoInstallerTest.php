<?php

namespace Drupal\Tests\tripal_chado\Kernel\Task;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\CoversDefaultClass;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\DataProvider;


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
#[CoversDefaultClass('\Drupal\tripal_chado\Task\ChadoInstaller')]
#[Group('Tripal')]
#[Group('Tripal Chado')]
#[Group('Tripal Chado Task')]
#[Group('Tripal Chado Installer')]
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
  public static function provideInvalidParameters() {
    $test_set = [];

    $test_set[] = [
      'test_name' => 'Version not a string',
      'paramset' => [
        'output_schemas' => [ 'chado' . uniqid() ],
        'version' => ['fred' => 'sarah'],
      ],
      'expected_message' => 'version must be a string; whereas, you passed an array or object'
    ];

    $test_set[] = [
      'test_name' => 'Version not valid',
      'paramset' => [
        'output_schemas' => [ 'chado' . uniqid() ],
        'version' => 5.9,
      ],
      'expected_message' => 'version .*is not supported by this installer'
    ];

    $test_set[] = [
      'test_name' => 'Schema already exists.',
      'paramset' => [
        'output_schemas' => [ 'testchadoschemaexists' ],
        'version' => 1.3,
      ],
      'expected_message' => 'Target schema ".*" already exists.',
    ];

    $test_set[] = [
      'test_name' => 'Input schema not supported.',
      'paramset' => [
        'input_schemas' => [ 'fred' ],
        'output_schemas' => [ 'chado' . uniqid() ],
        'version' => 1.3,
      ],
      'expected_message' => 'Chado installer does not take input schemas',
    ];

    $test_set[] = [
      'test_name' => 'Too many output schema.',
      'paramset' => [
        'output_schemas' => [ 'chado' . uniqid(), 'chado' . uniqid() ],
        'version' => 1.3,
      ],
      'expected_message' => 'Invalid number of output schemas',
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
  #[DataProvider('provideInvalidParameters')]
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
