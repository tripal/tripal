<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Task\ChadoPreparer;


/**
 * Tests for Chado preparer task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoPreparer
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Preparer
 */
class ChadoPreparerFunctionalTest extends ChadoTestBrowserBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTask() {

    // Test preparer.
    $preparer = \Drupal::service('tripal_chado.preparer');
    $preparer->setParameters([
      'output_schemas' => [$this->chado->getSchemaName()],
    ]);
    $success = $preparer->performTask();
    $this->assertTrue($success, 'Task performed.');

  }
}
