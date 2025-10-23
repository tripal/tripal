<?php

namespace Drupal\tripal\Access;

use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Cache\CacheableMetadata;
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
    $cacheability = new CacheableMetadata();

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
        if ($account->hasPermission("view all $entity_bundle content")) {
          return AccessResult::allowedIfHasPermission($account, "view all $entity_bundle content");
        }
        elseif ($account->hasPermission("view own $entity_bundle content")) {
          $cacheability->addCacheContexts(['user.permissions']);
          if ($account->id() != $entity->getOwnerId()) {
            return AccessResult::neutral();
          }
          return AccessResult::allowed()->addCacheableDependency($cacheability);
        }

      case 'update':

        $reject_anonymous = self::denyAnonymousUserAccess(
          $account,
          "The anonymous user should never be allowed to edit Tripal content.",
        );
        if ($reject_anonymous) {
          return $reject_anonymous;
        }

        if ($account->hasPermission("edit any $entity_bundle content")) {
          return AccessResult::allowedIfHasPermission($account, "edit any $entity_bundle content");
        }
        elseif ($account->hasPermission("edit own $entity_bundle content")) {
          $cacheability->addCacheContexts(['user.permissions']);
          if ($account->id() != $entity->getOwnerId()) {
            return AccessResult::neutral();
          }
          return AccessResult::allowed()->addCacheableDependency($cacheability);
        }

      case 'delete':

        $reject_anonymous = self::denyAnonymousUserAccess(
          $account,
          "The anonymous user should never be allowed to delete Tripal content.",
        );
        if ($reject_anonymous) {
          return $reject_anonymous;
        }

        if ($account->hasPermission("delete any $entity_bundle content")) {
          return AccessResult::allowedIfHasPermission($account, "delete any $entity_bundle content");
        }
        elseif ($account->hasPermission("delete own $entity_bundle content")) {
          $cacheability->addCacheContexts(['user.permissions']);
          if ($account->id() != $entity->getOwnerId()) {
            return AccessResult::neutral();
          }
          return AccessResult::allowed()->addCacheableDependency($cacheability);
        }

      case 'unpublish':

        $reject_anonymous = self::denyAnonymousUserAccess(
          $account,
          "The anonymous user should never be allowed to unpublish Tripal content.",
        );
        if ($reject_anonymous) {
          return $reject_anonymous;
        }

        if ($account->hasPermission("unpublish any $entity_bundle content")) {
          return AccessResult::allowedIfHasPermission($account, "unpublish any $entity_bundle content");
        }
        elseif ($account->hasPermission("unpublish own $entity_bundle content")) {
          $cacheability->addCacheContexts(['user.permissions']);
          if ($account->id() != $entity->getOwnerId()) {
            return AccessResult::neutral();
          }
          return AccessResult::allowed()->addCacheableDependency($cacheability);
        }
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

    $reject_anonymous = self::denyAnonymousUserAccess(
      $account,
      "The anonymous user should never be allowed to create Tripal content.",
    );
    if ($reject_anonymous) {
      return $reject_anonymous;
    }

    // Allow administrator permission to bypass per content type permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    // Check 'create TYPE content' permission.
    // Always return allow if has permission in order to allow other
    // implementations to override this functionality.
    if (!$account->hasPermission("create $entity_bundle content")) {
      return AccessResult::forbidden("You do not have permission to create this Tripal Content. Have your administrator give you the 'Create new content' permission for this type of Tripal Content specifically.");
    }
    else {
      return AccessResult::allowed();
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
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The result of this permission check.
   *
   * @see entity.tripal_entity.add_page
   */
  public static function checkHasAnyCreateAccess(AccountInterface $account): AccessResultInterface {

    $reject_anonymous = self::denyAnonymousUserAccess(
      $account,
      "The anonymous user should never be allowed to create Tripal content.",
    );
    if ($reject_anonymous) {
      return $reject_anonymous;
    }

    // Ensure that the Tripal Content Admin permission bypasses
    // the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    // Get the permissions provided by Tripal with 'create TYPE content'.
    $create_tripal_permissions = self::getTripalContentPermissionsList('create');

    // If the current user has any of the Tripal 'create TYPE content'.
    // permissions then they should be allowed access.
    foreach ($create_tripal_permissions as $permission_name) {
      if ($account->hasPermission($permission_name)) {
        return AccessResult::allowed();
      }
    }

    // If we get here, then the account does not have any 'create TYPE content'
    // permnissions. We will remain neutral in case someone else wants to hook
    // into this permission flow.
    return AccessResult::neutral();
  }

  /**
   * Grant access if 'publish tripal content' or 'administer tripal content'.
   *
   * Note: This is meant to be referenced directly in routes.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account of the user to check access for.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The result of this permission check.
   *
   * @see tripal.content_bio_data_publish_form
   * @see ::checkWithAdminOverrideAccess()
   */
  public static function checkHasPublishOrAdminAccess(AccountInterface $account): AccessResultInterface {

    $reject_anonymous = self::denyAnonymousUserAccess(
      $account,
      "The anonymous user should never be allowed to publish Tripal content.",
    );
    if ($reject_anonymous) {
      return $reject_anonymous;
    }

    return self::checkWithAdminOverrideAccess($account, 'publish tripal content');
  }

  /**
   * Grant access if the user has $permission or 'administer tripal content'.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account of the user to check access for.
   * @param string $permission
   *   An existing permission string to check as defined in permissions.yml.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The result of this permission check.
   *
   * @see entity.tripal_entity.add_page
   */
  public static function checkWithAdminOverrideAccess(AccountInterface $account, string $permission): AccessResultInterface {

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

  /**
   * Grant access for a single operation based on multiple Tripal content types.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account of the user to check access for.
   * @param array $content_types
   *   An array of content type ids the permissions should be restricted to.
   *   Note: passing 'all' as one element in this array will generate the full
   *   content type list for you.
   * @param string $operation
   *   One of 'view', 'create', 'edit', 'unpublish', or 'delete' depending on
   *   which permission you want to use for each content type.
   * @param string $mode
   *   One of 'any' or 'all' depending on whether the user only needs a single
   *   content type permission or all of them.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The result of this permission check.
   */
  public static function checkManyPermissions(AccountInterface $account, array $content_types, string $operation, string $mode): AccessResultInterface {

    // Ensure that the Tripal Content Admin permission bypasses
    // the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    // Now check the per content type permissions.
    // Keeping count of the content types for which access would be granted.
    $access_granted = 0;
    // If the 'all' special option is in the content_types array, then
    // get the full list of permissions.
    if (array_key_exists('all', $content_types)) {
      foreach (self::getTripalContentPermissionsList($operation) as $permission) {
        if ($account->hasPermission($permission)) {
          $access_granted++;
        }
      }
    }
    else {
      // If we have a specific list of content types, then generate the
      // permissions string for each one.
      foreach ($this->options['content_types'] as $content_type) {
        $permission = $this->options['operation'] . ' ' . $content_type . ' content';
        if ($account->hasPermission($permission)) {
          $access_granted++;
        }
      }
    }

    // Finally grant access depending on the mode configured.
    switch ($this->options['mode']) {
      case 'any':
        if ($access_granted > 0) {
          return AccessResult::allowed();
        }

      case 'all':
        if (count($content_type) === $access_granted) {
          return AccessResult::allowed();
        }
    }

    // If the mode is not recognized or did not return allowed, deny access.
    return AccessResult::forbidden("The user did not have $mode of the $operation permissions for the content types: " . implode(', ', $content_types));
  }

  /**
   * Get all TripalContent permissions for a specific operation (e.g. view).
   *
   * @param string $operation
   *   One of 'view', 'create', 'edit', 'unpublish', or 'delete' depending on
   *   which permission you want to use for each content type.
   *
   * @return array
   *   List of the permissions for all content types using that operation.
   */
  public static function getTripalContentPermissionsList(string $operation): array {

    // Get the full Drupal permissions list.
    $permission_list = \Drupal::service('user.permissions')->getPermissions();

    // Now filter this list based on the operation passed in.
    $filtered_permissions = array_filter($permission_list, function ($perm, $name) {
      if ($perm['provider'] === 'tripal') {
        if (preg_match('/^' . $operation . ' .* content$/', $name, $matches)) {
          return TRUE;
        }
      }
      return FALSE;
    }, ARRAY_FILTER_USE_BOTH);

    // Finally, return just the permission names.
    return array_keys($filtered_permissions);
  }

  /**
   * Ensures that anonymous users are denied access.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The current user account.
   * @param string $reason
   *   The reason to provide for denying access.
   *
   * @return \Drupal\Core\Access\AccessResultInterface|bool
   *   The result of this permission check.
   */
  public static function denyAnonymousUserAccess(AccountInterface $account, string $reason): AccessResultInterface|bool {

    if (!$account->isAuthenticated()) {
      return AccessResult::forbidden($reason);
    }

    return FALSE;
  }

}
