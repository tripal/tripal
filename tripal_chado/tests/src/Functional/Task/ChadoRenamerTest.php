<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBase;
use Drupal\tripal_chado\Task\ChadoRenamer;


/**
 * Tests for renamer task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoRenamer
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Renamer
 */
class ChadoRenamerFunctionalTest extends ChadoTestBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskRenamer() {
    // Create a temporary schema.
    $biodb = $this->getTestSchema(ChadoTestBase::CREATE_SCHEMA);
    // Get another temporary schema name.
    $biodb2 = $this->getTestSchema(ChadoTestBase::SCHEMA_NAME_ONLY);

    // Test renamer.
    $renamer = \Drupal::service('tripal_chado.renamer');
    $renamer->setParameters([
      'output_schemas' => [$biodb->getSchemaName(), $biodb2->getSchemaName()],
    ]);
    $success = $renamer->performTask();
    $this->assertTrue($success, 'Task performed.');
    
    $exists = $biodb->schema()->schemaExists();
    $this->assertFalse($exists, 'Orignal schema name not in use.');
    
    $exists = $biodb2->schema()->schemaExists();
    $this->assertTrue($exists, 'New schema name used.');

    // Let know the "garbage schema collector" the schema are unused.
    $this->freeTestSchema($biodb2);
    $this->freeTestSchema($biodb);
  }
}
