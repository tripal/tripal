<?php

namespace Drupal\tripal_image\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\tripal_image\Services\TripalImageRebuildService;

/**
 * Hook implementations for the TripalImage module.
 */
class TripalImageHooks {

  use StringTranslationTrait;


  /**
   * The rebuild service for this module.
   *
   * @var \Drupal\tripal_image\Services\TripalImageRebuildService
   */
  protected TripalImageRebuildService $rebuildService;

  /**
   * Constructor.
   *
   * @param \Drupal\tripal_image\Service\RebuildService $rebuildService
   *   The rebuild service for this module.
   */
  public function __construct(TripalImageRebuildService $rebuildService) {
    $this->rebuildService = $rebuildService;
  }

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match) {
    switch ($route_name) {
      // Main module help for the tripal_image module.
      case 'help.page.tripal_image':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('A module for associating images with Tripal content.') . '</p>';
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
    return 'tripal_image';
  }

  /**
   * Implements hook_uninstall().
   *
   * The hook uninstall does not support attributes and must remain procedural.
   */
  public function tripalImageUninstall() {
    // Delete the custom tables created by this module.
    $this->rebuildService->dropCustomChadoTables();
  }

}
