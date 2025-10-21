<?php

namespace Drupal\tripal\Access;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Tripal Content entity (i.e. TripalEntity).
 *
 * @see \Drupal\tripal\Entity\TripalEntity.
 */
class TripalEntityAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   *
   * Note: we do not want to call the parent here as we are doing our own thing
   * and do not want to introduce any unexpected behaviour.
   *
   * If an extension module wants to override what we are doing here then they
   * should implement one of the following hooks which take priority:
   * hook_entity_access() or hook_ENTITY_TYPE_access().
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {

    // Ensure that the Tripal Content Admin permission bypasses
    // the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    // For view, update, delete we want to use bundle-specific permissions
    // so lets get the bundle now.
    $entity_bundle = $entity->getType();

    // Now, lets check the permission based on the bundle and operation.
    // Always return allow if has permission in order to allow other
    // implementations to override this functionality.
    switch ($operation) {
      case 'view':
        // @todo add logic here to check 1) "all" then 2) "own".
        return AccessResult::allowedIfHasPermission($account, "view all $entity_bundle content");

      case 'update':
        // @todo add logic here to check 1) "any" then 2) "own".
        return AccessResult::allowedIfHasPermission($account, "edit any $entity_bundle content");

      case 'delete':
        // @todo add logic here to check 1) "any" then 2) "own".
        return AccessResult::allowedIfHasPermission($account, "delete any $entity_bundle content");

      // @todo figure out how to catch unpublish :thinking: is it available as
      // an operation?
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   *
   * Note: we do not want to call the parent here as we are doing our own thing
   * and do not want to introduce any unexpected behaviour.
   *
   * If an extension module wants to override what we are doing here then they
   * should implement one of the following hooks which take priority:
   * hook_entity_create_access() or hook_ENTITY_TYPE_create_access().
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {

    // Allow administrator permission to bypass per content type permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed()->cachePerPermissions();
    }

    // Check 'create TYPE content' permission.
    // Always return allow if has permission in order to allow other
    // implementations to override this functionality.
    if (!$account->hasPermission("create $entity_bundle content")) {
      return AccessResult::forbidden("You do not have permission to create this Tripal Content. Have your administrator give you the 'Create new content' permission for this type of Tripal Content specifically.")->cachePerPermissions();
    }
  }

  /**
   * Grant access if the user has 'create TYPE content' for any TYPE.
   *
   * Note: This is meant to be referenced directly in routes.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account of the user to check access for.
   *
   * @return \Drupal\Core\Access\AccessResult
   *   The access result.
   *
   * @see entity.tripal_entity.add_page
   */
  public static function checkHasAnyCreateAccess(AccountInterface $account): AccessResult {

    // @todo this needs to be implemented.
    return AccessResult::allowed();
  }

  /**
   * Grant access if 'publish tripal content' or 'administer tripal content'.
   *
   * Note: This is meant to be referenced directly in routes.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account of the user to check access for.
   *
   * @return \Drupal\Core\Access\AccessResult
   *   The access result.
   *
   * @see tripal.content_bio_data_publish_form
   */
  public static function checkHasPublishOrAdminAccess(AccountInterface $account): AccessResult {
    return self::checkWithAdminOverrideAccess($account, 'publish tripal content');
  }

  /**
   * Grant access if the user has $permission or 'administer tripal content'.
   *
   * Note: This is meant to be referenced directly in routes.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account of the user to check access for.
   * @param string $permission
   *   An existing permission string to check as defined in permissions.yml.
   *
   * @return \Drupal\Core\Access\AccessResult
   *   The access result.
   *
   * @see entity.tripal_entity.add_page
   */
  public static function checkWithAdminOverrideAccess(AccountInterface $account, string $permission): AccessResult {

    // Allow administrator permission to bypass indicated permission.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed()->cachePerPermissions();
    }

    // Check permission indicated.
    if ($account->hasPermission($permission)) {
      return AccessResult::allowed()->cachePerPermissions();
    }

    // If neither then we opt out of deciding.
    // @todo we may need to return forbidden here.
    return AccessResult::neutral();
  }

}
