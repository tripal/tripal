<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Task\ChadoRemover;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\CoversMethod;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests for remover task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoRemover
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Remover
 *
 * @covers ::setParameters
 * @covers ::performTask
 */
#[CoversClass(ChadoRemover::class)]
#[CoversMethod(ChadoRemover::class, 'setParameters')]
#[CoversMethod(ChadoRemover::class, 'performTask')]
#[Group('biodb-task')]
#[group('task-remover')]
#[RunTestsInSeparateProcesses]
class ChadoRemoverTest extends ChadoTestBrowserBase {

  /**
   * Tests task.
   */
  public function testPerformTaskRemover() {
    // Create a temporary schema.
    $tripaldbx_db = $this->getTestSchema(ChadoTestBrowserBase::CREATE_SCHEMA);

    // Test remover.
    $remover = \Drupal::service('tripal_chado.remover');
    $remover->setParameters([
      'output_schemas'  => [$tripaldbx_db->getSchemaName()],
    ]);
    $success = $remover->performTask();
    $this->assertTrue($success, 'Task performed.');
    $this->assertFalse($tripaldbx_db->schema()->schemaExists(), 'Schema removed.');
    // Already dropped but we need to let know the "garbage schema collector".
    $this->freeTestSchema($tripaldbx_db);
  }

}
