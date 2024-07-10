<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Task\ChadoRemover;


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
class ChadoRemoverFunctionalTest extends ChadoTestBrowserBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
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
