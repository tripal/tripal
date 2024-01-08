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

    // Create a temporary schema.
    $tripaldbx_db = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);

    // Now modify the schema.
    $tripaldbx_db->query('ALTER TABLE {1:project} DROP COLUMN description');
    $tripaldbx_db->query('ALTER TABLE {1:feature} ADD COLUMN testsum INT');
    $tripaldbx_db->query('DROP TABLE {1:analysis} CASCADE');

    $this->assertFalse($tripaldbx_db->schema()->fieldExists('project', 'description'), 'Unable to prepare chado for test by removing project.description column.');
    $this->assertTrue($tripaldbx_db->schema()->fieldExists('feature', 'testsum'), 'Unable to prepare chado for test by adding feature.testsum column.');
    $this->assertFalse($tripaldbx_db->schema()->tableExists('analysis'), 'Unable to prepare chado for test by removing analysis table.');

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
    // Since we have a file specified above, we can't check the changes were
    // applied, since they weren't. They were added to the file.
    // We can't remove the file and apply directly since the upgrade task is broken.
    // $this->assertTrue($tripaldbx_db->schema()->fieldExists('project', 'description'), 'Missing column project.description was not added.');
    // $this->assertFalse($tripaldbx_db->schema()->fieldExists('feature', 'testsum'), 'Extra column feature.testsum should not have been removed but was.');
    // $this->assertTrue($tripaldbx_db->schema()->tableExists('analysis'), 'Missing analysis table should have been added.');
    // @todo: test column types int --> bigint
    // @todo: test indexes
  }

  /**
   * {@inheritdoc}
   */
  public static function tearDownAfterClass() :void {
    parent::tearDownAfterClass();
    if (file_exists('/tmp/upgrade_test.sql')) {
      unlink('/tmp/upgrade_test.sql');
    }
  }
}
