<?php

namespace Drupal\Tests\tripal\Kernel\TripalConfig;

use Drupal\KernelTests\KernelTestBase;

/**
 * Tests for configuration in Yaml files.
 *
 * @group Tripal
 * @group Tripal Config
 */
class configTest extends KernelTestBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado', 'tripal_biodb'];

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
   * Tests version and core_version_requirements for consistency
   * across submodules.
   *
   * @cover ::__tripal_version
   */
  public function testConfigYaml() {
    $previous_module = NULL;
    $previous_version = NULL;
    $previous_core_version_requirement = NULL;
    foreach (self::$modules as $module) {
      $moduleObj = \Drupal::service('module_handler')->getModule($module);
      $info = \Drupal::service('info_parser')->parse($moduleObj->getPathname());
      $version = $info['version'] ?? NULL;
      $core_version_requirement = $info['core_version_requirement'] ?? NULL;
      $this->assertNotNull($version, 'No "version" was specified in ' . $module . '.info.yml');
      $this->assertNotNull($core_version_requirement, 'No "core_version_requirement" was specified in ' . $module . '.info.yml');
      // On second and later modules, verify that returned values match those from the previous module.
      if ($previous_module) {
        $this->assertEqual($version, $previous_version, 'version for module "'
                           . $module . '" "' . $version
                           . '" is different than for module "'
                           . $previous_module . '" "' . $previous_version . '"');
        $this->assertEqual($core_version_requirement, $previous_core_version_requirement, 'core_version_requirement for module "'
                           . $module . '" "' . $core_version_requirement
                           . '" is different than for module "'
                           . $previous_module . '" "' . $previous_core_version_requirement . '"');
      }
      $previous_module = $module;
      $previous_version = $version;
      $previous_core_version_requirement = $core_version_requirement;
    }
  }

}
