<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;

/**
 * Tests for configuration in Yaml files.
 * These tests do not currently cover any code.
 *
 * @group Tripal
 * @group Tripal Config
 */
class configTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_biodb', 'tripal_chado'];

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
   * When we update the version of Tripal, we need to remember to
   * do it in all three yaml files. This function confirms that
   * version and also core_version_requirements are consistent in
   * each of the sub-modules' *.info.yml files.
   *
   */
  public function testConfigYaml() {
    $previous_module = NULL;
    $previous_version = NULL;
    $previous_core_version_requirement = NULL;
    foreach (self::$modules as $module) {
      $moduleObj = \Drupal::service('module_handler')->getModule($module);
      $moduleInfo = \Drupal::service('info_parser')->parse($moduleObj->getPathname());
      $version = $moduleInfo['version'] ?? NULL;
      $core_version_requirement = $moduleInfo['core_version_requirement'] ?? NULL;
      // Verify that values are present.
      $this->assertNotNull($version, 'No "version" was specified in ' . $module . '.info.yml');
      $this->assertNotNull($core_version_requirement, 'No "core_version_requirement" was specified in ' . $module . '.info.yml');
      // On second and later modules, verify that returned values match those
      // from the previous module (the consistency check).
      if ($previous_module) {
        $this->assertEquals($version, $previous_version,
                           'version for module "'
                           . $previous_module . '" is different than for module "' . $module . '"');
        $this->assertEquals($core_version_requirement, $previous_core_version_requirement,
                           'core_version_requirement for module "'
                           . $previous_module . '" is different than for module "' . $module . '"');
      }
      $previous_module = $module;
      $previous_version = $version;
      $previous_core_version_requirement = $core_version_requirement;
    }
  }

}
