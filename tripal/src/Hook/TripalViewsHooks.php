<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\views\ViewEntityInterface;
use Drupal\views\ViewsConfigUpdater;

/**
 * Hook implementations for the Tripal module.
 *
 * This class contains hook implementations related to Drupal views.
 */
class TripalViewsHooks {

  /**
   * Implements hook_ENTITY_TYPE_presave().
   *
   * A new views 'class' setting was introduced in Drupal 11.2, but it has
   * not been added to our yaml views configurations so that we maintain
   * backward compatibility. We do the update here before Drupal does it
   * in order to avoid a deprecation notice in unit tests.
   * This hook will do nothing for Drupal <11.2.
   */
  #[Hook('view_presave')]
  public function viewPresave(ViewEntityInterface $view): void {
    /** @var \Drupal\views\ViewsConfigUpdater $config_updater */
    $config_updater = \Drupal::classResolver(ViewsConfigUpdater::class);
    if (method_exists($config_updater, 'processTableCssClassUpdate')) {
      $config_updater->setDeprecationsEnabled(FALSE);
      $config_updater->processTableCssClassUpdate($view);
    }
  }

}
