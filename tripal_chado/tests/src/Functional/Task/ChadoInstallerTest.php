<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestKernelBase;
use Drupal\tripal_chado\Task\ChadoInstaller;


/**
 * Tests for installer task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoInstaller
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Installer
 */
class ChadoInstallerFunctionalTest extends ChadoTestKernelBase {

  /**
   * Tests task.
   *
   * @cover ::setParameters
   * @cover ::performTask
   */
  public function testPerformTaskInstaller() {
    // Get a temporary schema name.
    $biodb = $this->getTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);

    // Test installer.
    $installer = \Drupal::service('tripal_chado.installer');
    $installer->setParameters([
      'output_schemas'  => [$biodb->getSchemaName()],
    ]);
    $success = $installer->performTask();
    $this->assertTrue($success, 'Task performed.');
    $this->assertTrue($biodb->schema()->schemaExists(), 'Schema created.');
    $this->assertTrue($biodb->schema()->tableExists('stock'), 'Table created.');
    $this->assertTrue($biodb->schema()->fieldExists('stock', 'uniquename'), 'Field created.');
    // @todo: test more... (types, functions, views, indexes, frange schema)
    $this->freeTestSchema($biodb);
  }
}
