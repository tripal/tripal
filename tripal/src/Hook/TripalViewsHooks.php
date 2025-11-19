<?php

namespace Drupal\tripal\Hook;

use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\views\ViewEntityInterface;
use Drupal\views\ViewExecutable;
use Drupal\views\ViewsConfigUpdater;

/**
 * Hook implementations for the Tripal module.
 *
 * This class contains hook implementations related to Drupal views.
 */
class TripalViewsHooks {

  /**
   * Implements hook_views_pre_view().
   */
  #[Hook('views_pre_view')]
  public function viewsPreView(ViewExecutable $view, $display_id, array &$args) {
    // Override permission of Tripal Content Type listing
    // (admin/content/bio_data) in order to support OR when evaluating
    // permissions since views UI does not support this.
    // WARNING: requires you to have NO Permissions on this view through the UI!
    if ($view->id() == 'tripal_content_type_listing') {
      // The user must have 1+ of the following permissions to access the
      // Tripal Content Type listing (admin/content/bio_data).
      $perm = [
        'access tripal content overview',
        'administer tripal content',
      ];

      // Check if current user has either permissions when attempting to
      // view listing.
      $user = \Drupal::currentUser();

      $has_permission = FALSE;
      foreach ($perm as $permission) {
        if ($user->hasPermission($permission)) {
          $has_permission = TRUE;
          break;
        }
      }

      // Deny access if user does not have necessary permission.
      if (!$has_permission) {
        throw new AccessDeniedHttpException();
      }
    }
  }

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
