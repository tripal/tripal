<?php

namespace Drupal\Tests\tripal_chado\Kernel\Task;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
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
class ChadoUpgraderTest extends ChadoTestKernelBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskUpgrader() {

    // $this->markTestIncomplete(
    //   'This test has not been fully implemented yet.'
    // );

    // Create a temporary schema.
    $tripaldbx_db = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);

    // Test upgrader.
    $upgrader = \Drupal::service('tripal_chado.upgrader');
    $upgrader->setParameters([
      'output_schemas'  => [$tripaldbx_db->getSchemaName()],
      'cleanup'  => TRUE,
      'filename'  => '/tmp/upgrade_test.sql',
    ]);

    $success = $upgrader->performTask();
    $this->assertTrue($success, 'Task performed.');

    // Check some of the upgraded changes are present.
    $this->assertTrue($tripaldbx_db->schema()->fieldExists('feature', 'md5checksum'), 'Missing column added.');
    $this->assertFalse($tripaldbx_db->schema()->fieldExists('feature', 'testsum'), 'Extra column removed.');
    $this->assertTrue($tripaldbx_db->schema()->tableExists('analysis'), 'Missing table added.');
    // @todo: test column types int --> bigint
    // @todo: test indexes
  }
}
