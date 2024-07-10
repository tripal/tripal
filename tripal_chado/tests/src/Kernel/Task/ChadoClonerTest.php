<?php

namespace Drupal\Tests\tripal_chado\Kernel\Task;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Task\ChadoCloner;


/**
 * Tests for tasks.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoCloner
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Cloner
 */
class ChadoClonerTest extends ChadoTestKernelBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskCloner() {
    // Create a temporary schema.
    $tripaldbx_db1 = $this->getTestSchema(ChadoTestKernelBase::INIT_DUMMY);
    // Get another temporary schema name.
    $tripaldbx_db2 = $this->getTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);

    // Test cloner.
    $cloner = \Drupal::service('tripal_chado.cloner');
    $cloner->setParameters([
      'input_schemas'  => [$tripaldbx_db1->getSchemaName()],
      'output_schemas' => [$tripaldbx_db2->getSchemaName()],
    ]);
    $success = $cloner->performTask();
    $this->assertTrue($success, 'Task performed.');

    $exists = $tripaldbx_db2->schema()->schemaExists();
    $this->assertTrue($exists, 'Clone schema created.');

    $size = $tripaldbx_db2->schema()->getSchemaSize();
    $this->assertGreaterThan(100, $size, 'Clone schema not empty.');

    $this->freeTestSchema($tripaldbx_db2);
    $this->freeTestSchema($tripaldbx_db1);
  }
}
