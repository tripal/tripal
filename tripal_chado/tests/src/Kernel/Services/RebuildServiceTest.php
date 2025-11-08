<?php

namespace Drupal\Tests\tripal\Kernel\Services;

use Drupal\Core\Entity\EntityTypeManager;
use Drupal\Core\Extension\ModuleHandler;
use Symfony\Component\Yaml\Yaml;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the rebuild hooks and services.
 *
 * @group service-rebuild
 */
#[Group('service-rebuild')]
#[RunTestsInSeparateProcesses]
class RebuildServiceTest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'tripal', 'tripal_chado', 'views'];

  /**
   * Module handler service.
   *
   * @var Drupal\Core\Extension\ModuleHandler
   */
  protected ModuleHandler $module_handler;

  /**
   * Entity type manager service.
   *
   * @var Drupal\Core\Entity\EntityTypeManager
   */
  protected EntityTypeManager $entity_type_manager;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Initialize services.
    $this->module_handler = $this->container->get('module_handler');
    $this->entity_type_manager = $this->container->get('entity_type.manager');
  }

  /**
   * Tests the rebuild service.
   *
   * The rebuild service is called by hook_rebuild in any modules
   * that implement it, so we call it that way.
   */
  public function testTripalChadoRebuild() {

    $config = Yaml::parseFile(__DIR__ . '/RebuildServiceTest.yml');

    // Check that rebuild creates views.
    // Before rebuild they will not exist.
    $view_storage = $this->entity_type_manager->getStorage('view');
    foreach ($config['expected_views'] as $expect) {
      $view = $view_storage->load($expect['name']);
      $this->assertNull($view, 'The "' . $expect['name'] . '" view from the "' . $expect['module'] . '" module should not yet exist');
    }

    // Tests that rebuild creates configuration entities.
    // Before rebuild they will not exist.
    foreach ($config['expected_config_entities'] as $expect) {
      $exists = $this->checkConfigurationEntity($expect['type'], $expect['name']);
      $this->assertFalse($exists, 'The configuration for "' . $expect['type'] . ':' . $expect['name'] . '" from the "' . $expect['module'] . '" module should not exist');
    }

    // Execute hook_rebuild.
    // Find and execute all hook_rebuild instances in any installed modules.
    // Tripal rebuild hooks return the module name as a return value.
    $results = $this->module_handler->invokeAll('rebuild');
    foreach ($config['expected_modules'] as $module) {
      $this->assertContains($module, $results, "The rebuild hook for module \"$module\" was not invoked by invokeAll");
    }

    // Check that rebuild created views.
    foreach ($config['expected_views'] as $expect) {
      $view = $view_storage->load($expect['name']);
      $this->assertNotNull($view, 'The "' . $expect['name'] . '" view from the "' . $expect['module'] . '" module should have been created');
      $this->assertInstanceOf($expect['class'], $view, 'Returned view "' . $expect['name'] . '" from the "' . $expect['module'] . '" module is not of the correct class');
    }

    // Tests that rebuild created configuration entities.
    foreach ($config['expected_config_entities'] as $expect) {
      $exists = $this->checkConfigurationEntity($expect['type'], $expect['name']);
      $this->assertTrue($exists, 'The configuration for "' . $expect['type'] . ':' . $expect['name'] . '" from the "' . $expect['module'] . '" module should have been created');
    }
  }

  /**
   * Helper function to check existence of a configuration entity.
   *
   * @param string $config_type
   *   The type of configuration entity, e.g. "tripalfield_collection".
   * @param string $config_name
   *   The specific configuration, e.g. "general_chado".
   *
   * @return bool
   *   TRUE if it exists, FALSE if not.
   */
  protected function checkConfigurationEntity(string $config_type, string $config_name): bool {
    $exists = FALSE;
    $storage = $this->entity_type_manager->getStorage($config_type);
    $entity = $storage->load($config_name);
    if ($entity) {
      $exists = TRUE;
    }
    return $exists;
  }

}
