<?php

namespace Drupal\Tests\tripal_chado\Kernel\Task;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Task\ChadoRenamer;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\CoversMethod;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests for renamer task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoRenamer
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Renamer
 *
 * @covers ::setParameters
 * @covers ::performTask
 */
#[CoversClass(ChadoRenamer::class)]
#[CoversMethod(ChadoRenamer::class, 'setParameters')]
#[CoversMethod(ChadoRenamer::class, 'performTask')]
#[Group('biodb-task')]
#[group('task-renamer')]
#[RunTestsInSeparateProcesses]
class ChadoRenamerTest extends ChadoTestKernelBase {

  /**
   * Tests task.
   */
  public function testPerformTaskRenamer() {
    // Create a temporary schema.
    $tripaldbx_db1 = $this->getTestSchema(ChadoTestKernelBase::CREATE_SCHEMA);
    // Get another temporary schema name.
    $tripaldbx_db2 = $this->getTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);

    // Test renamer.
    $renamer = \Drupal::service('tripal_chado.renamer');
    $renamer->setParameters([
      'output_schemas' => [$tripaldbx_db1->getSchemaName(), $tripaldbx_db2->getSchemaName()],
    ]);
    $success = $renamer->performTask();
    $this->assertTrue($success, 'Task performed.');

    $exists = $tripaldbx_db1->schema()->schemaExists();
    $this->assertFalse($exists, 'Orignal schema name not in use.');

    $exists = $tripaldbx_db2->schema()->schemaExists();
    $this->assertTrue($exists, 'New schema name used.');

    // Let know the "garbage schema collector" the schema are unused.
    $this->freeTestSchema($tripaldbx_db2);
    $this->freeTestSchema($tripaldbx_db1);
  }

}
