<?php

namespace Drupal\tripal\Access;

use Drupal\Core\Session\AccountInterface;
use Drupal\User\UserInterface;
use Drupal\Core\Access\AccessResult;
use Drupal\Core\Routing\Access\AccessInterface;

/**
 * Checks that users have access to only their own content
 * given a user account and a permission to check if it's their content.
 * Permission is only granted if it's the users content and they have
 * the passed in permission.
 *
 * Note: This can only be used on paths with the {user} slug
 */
class TripalAccessOwnContentCheck implements AccessInterface {

  /**
   * The actual access check.
   *
   * @param Drupal\User\UserInterface $user
   *   The user loaded from the slug in the route. This is the user who owns
   *   the content.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   Run access checks for this account. This is the user requesting access.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The access result.
   */
  public function access(UserInterface $user, AccountInterface $account) {


    // First check that the account requesting access matches the user in the path.
    $slug_uid = $user->id();
    $requesting_uid = $account->id();
    if ($slug_uid === $requesting_uid) {
      return AccessResult::allowed();
    }

    return AccessResult::forbidden();
    // return ($account->hasPermission('administer tripal') && $this->someOtherCustomCondition()) ? AccessResult::allowed() : AccessResult::forbidden();

  }
}
