<?php

namespace Drupal\Tests\tripal\Kernel\Api\TripalApi;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;

/**
 * Tests for procedural API functions.
 *
 * @group Tripal
 * @group Tripal Api
 */
class apiTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();
  }

  /**
   * {@inheritdoc}
   */
  public static function tearDownAfterClass() :void {
  }

  /**
   * Tests the API function tripal_version().
   *
   * @cover ::tripal_version
   */
  public function testTripalVersion() {
    // Get the version of the Tripal module as stored in tripal.info.yml
    $moduleObj = \Drupal::service('module_handler')->getModule(self::$modules[0]);
    $info = \Drupal::service('info_parser')->parse($moduleObj->getPathname());
    $check_version = $info['version'] ?? NULL;

    // Get the version using the API function to be tested.
    $tripal_version = tripal_version();

    $this->assertEquals($check_version, $tripal_version, 'tripal_version() returned the wrong value');
  }

}
