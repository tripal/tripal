<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\views\ViewEntityInterface;
use Drupal\views\ViewsConfigUpdater;
use Drupal\views\ViewExecutable;
use Drupal\views\Plugin\views\query\QueryPluginBase;

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
   * Alters the Tripal Content Type Listing view to filter content types
   * based on the current user's permissions.
   */
  #[Hook('views_query_alter')]
  public function viewsQueryAlter(ViewExecutable $view, QueryPluginBase $query) {
    // Only alter the Tripal Content Type Listing view.
    if ($view->id() !== 'tripal_content_type_listing') {
      return;
    }

    // Load all Tripal entity types.
    $entity_types = \Drupal::service('entity_type.manager')
      ->getStorage('tripal_entity_type')
      ->loadByProperties([]);

    // Get the current user account.
    $account = \Drupal::currentUser();

    // Determine which entity types the user has permission to view.
    $allowed_types = [];
    foreach ($entity_types as $entity_type) {
      $entity_id = $entity_type->id();
      $permission = "view all $entity_id content";
      if ($account->hasPermission($permission)) {
        $allowed_types[] = $entity_id;
      }
    }

    if (empty($allowed_types)) {
      // If the user doesn't have permission to view any entity type,
      // return no rows.
      $query->addWhereExpression(0, 'null');
      return;
    }

    $query->addWhere('AND', "tripal_entity.type", $allowed_types, 'IN');
  }

}
