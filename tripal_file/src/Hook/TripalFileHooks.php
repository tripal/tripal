<?php

namespace Drupal\tripal_file\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\tripal_file\Services\TripalFileRebuildService;

/**
 * Hook implementations for the TripalFile module.
 */
class TripalFileHooks {

  use StringTranslationTrait;


  /**
   * The rebuild service for this module.
   *
   * @var \Drupal\tripal_file\Services\TripalFileRebuildService
   */
  protected TripalFileRebuildService $rebuildService;

  /**
   * Constructor.
   *
   * @param \Drupal\tripal_file\Service\RebuildService $rebuildService
   *   The rebuild service for this module.
   */
  public function __construct(TripalFileRebuildService $rebuildService) {
    $this->rebuildService = $rebuildService;
  }

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match) {
    switch ($route_name) {
      // Main module help for the tripal_file module.
      case 'help.page.tripal_file':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('A module for associating files with Tripal content and for accessing files via web services.') . '</p>';
        return $output;

      default:
    }
  }

  /**
   * Implements hook_rebuild().
   */
  #[Hook('rebuild')]
  public function rebuild(): string {
    $this->rebuildService->executeRebuild();
    // Return value of the module name is only used for phpunit tests.
    return 'tripal_file';
  }

  /**
   * Implements hook_uninstall().
   *
   * The hook uninstall does not support attributes and must remain procedural.
   */
  public function tripalFileUninstall() {
    // Delete the custom tables created by this module.
    $this->rebuildService->dropCustomChadoTables();
  }

}
