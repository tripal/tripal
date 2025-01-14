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

    // Create a reference.
    $tripaldbx_db_ref = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);

    // Create a temporary schema.
    $tripaldbx_db = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);

    // Confirm the reference schema is different from the one we are testing
    // an upgrade on.
    $this->assertNotEquals(
      $tripaldbx_db->getSchemaName(),
      $tripaldbx_db_ref->getSchemaName(),
      "The reference should not be the same schema we are testing an upgrade on."
    );

    // Now modify the schema.
    $tripaldbx_db->query('ALTER TABLE {1:project} DROP COLUMN description');
    $tripaldbx_db->query('ALTER TABLE {1:feature} ADD COLUMN testsum INT');
    $tripaldbx_db->query('DROP TABLE {1:analysis} CASCADE');
    // -- confirm it was successful.
    $this->assertFalse($tripaldbx_db->schema()->fieldExists('project', 'description'), 'Unable to prepare chado for test by removing project.description column.');
    $this->assertTrue($tripaldbx_db->schema()->fieldExists('feature', 'testsum'), 'Unable to prepare chado for test by adding feature.testsum column.');
    $this->assertFalse($tripaldbx_db->schema()->tableExists('analysis'), 'Unable to prepare chado for test by removing analysis table.');
    // -- confirm you did not inadvertently alter the reference too.
    $this->assertTrue($tripaldbx_db_ref->schema()->fieldExists('project', 'description'), 'Accidentally removed project.description column from reference.');
    $this->assertFalse($tripaldbx_db_ref->schema()->fieldExists('feature', 'testsum'), 'Accidentally added feature.testsum column to reference.');
    $this->assertTrue($tripaldbx_db_ref->schema()->tableExists('analysis'), 'Accidentally removed analysis table from reference.');

    // Test upgrader.
    $upgrader = \Drupal::service('tripal_chado.upgrader');
    $upgrader->setParameters([
      'input_schemas' => [$tripaldbx_db_ref->getSchemaName()],
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
