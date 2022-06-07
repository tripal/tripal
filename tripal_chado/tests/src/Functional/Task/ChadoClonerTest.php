<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestKernelBase;
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
class ChadoClonerFunctionalTest extends ChadoTestKernelBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskCloner() {
    // Create a temporary schema.
    $biodb = $this->getTestSchema(ChadoTestKernelBase::INIT_DUMMY);
    // Get another temporary schema name.
    $biodb2 = $this->getTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);

    // Test cloner.
    $cloner = \Drupal::service('tripal_chado.cloner');
    $cloner->setParameters([
      'input_schemas'  => [$biodb->getSchemaName()],
      'output_schemas' => [$biodb2->getSchemaName()],
    ]);
    $success = $cloner->performTask();
    $this->assertTrue($success, 'Task performed.');
    
    $exists = $biodb2->schema()->schemaExists();
    $this->assertTrue($exists, 'Clone schema created.');

    $size = $biodb2->schema()->getSchemaSize();
    $this->assertGreaterThan(100, $size, 'Clone schema not empty.');
    
    $this->freeTestSchema($biodb2);
    $this->freeTestSchema($biodb);
  }
}
