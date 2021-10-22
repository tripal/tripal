<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBase;
use Drupal\tripal_chado\Task\ChadoUpgrader;

/**
 * Tests for upgreader task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoUpgrader
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Upgrader
 */
class ChadoUpgraderFunctionalTest extends ChadoTestBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskUpgrader() {
    // Create a temporary schema.
    $biodb = $this->getTestSchema(ChadoTestBase::INIT_DUMMY);
    // Test upgrader.
    $upgrader = \Drupal::service('tripal_chado.upgrader');
    $upgrader->setParameters([
      'output_schemas'  => [$biodb->getSchemaName()],
      'cleanup'  => TRUE,
      // 'filename'  => '/tmp/upgrade_test.sql',
    ]);
    $this->markTestIncomplete(
      'This test has not been fully implemented yet.'
    );
    // There are issues with the given incomplete dummy schemas as objects are
    // missing during the upgrade process.
    $success = $upgrader->performTask();
    $this->assertTrue($success, 'Task performed.');
    $this->assertTrue($biodb->schema()->fieldExists('feature', 'md5checksum'), 'Missing column added.');
    $this->assertFalse($biodb->schema()->fieldExists('feature', 'testsum'), 'Extra column removed.');
    $this->assertTrue($biodb->schema()->tableExists('analysis'), 'Missing table added.');
    // @todo: test column types int --> bigint
    // @todo: test indexes
    $this->freeTestSchema($biodb);
  }
}
