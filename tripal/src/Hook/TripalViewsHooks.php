<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\views\ViewEntityInterface;
use Drupal\views\ViewsConfigUpdater;
use Drupal\views\ViewExecutable;
use Drupal\views\Plugin\views\query\QueryPluginBase;
use Drupal\tripal\Access\TripalEntityAccessControlHandler;

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

  /**
   * Implements hook_views_query_alter().
   *
   * Adds a where clause to any view showing Tripal Content which
   * restricts the results to Tripal Content Types that the current
   * user has permission to view.
   */
  #[Hook('views_query_alter')]
  public function viewsQueryAlter(ViewExecutable $view, QueryPluginBase $query) {
    // Only alter the views with base table 'tripal_entity'.
    if ($view->storage->get('base_table') !== 'tripal_entity') {
      return;
    }

    // Get the current user account.
    $account = \Drupal::currentUser();

    // Ensure that the Tripal Content Admin permission bypasses
    // the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return;
    }

    $permissions = TripalEntityAccessControlHandler::getTripalContentPermissionsList('view');

    // Determine which entity types the user has permission to view.
    $allowed_types = [];
    foreach ($permissions as $permission) {
      if (preg_match('/^view\s+(?:own|all)\s+(.+)\s+content$/i', $permission, $m)) {
        $entity_id = $m[1];
      }
      if ($account->hasPermission($permission)) {
        $allowed_types[] = $entity_id;
      }
    }

    $query->addWhere('AND', "tripal_entity.type", $allowed_types, 'IN');
  }

}
