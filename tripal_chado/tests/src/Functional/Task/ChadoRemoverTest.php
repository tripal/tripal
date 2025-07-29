<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use PHPUnit\Framework\Attributes\CoversDefaultClass;
use PHPUnit\Framework\Attributes\Group;


/**
 * Tests for remover task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoRemover
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Remover
 */
#[CoversDefaultClass('\Drupal\tripal_chado\Task\ChadoRemover')]
#[Group('Tripal')]
#[Group('Tripal Chado')]
#[Group('Tripal Chado Task')]
#[Group('Tripal Chado Remover')]
class ChadoRemoverTest extends ChadoTestBrowserBase {

  /**
   * Tests task.
   *
   * @covers ::setParameters
   * @covers ::performTask
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
