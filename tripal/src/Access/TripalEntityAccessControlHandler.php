<?php

namespace Drupal\tripal\Access;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Tripal Content entity.
 *
 * @see \Drupal\tripal\Entity\TripalEntity.
 */
class TripalEntityAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {

    // Ensure that the Tripal Content Admin permission bypasses the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    switch ($operation) {
      case 'view':
        return AccessResult::allowedIfHasPermission($account, 'view tripal content entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit tripal content entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete tripal content entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {

    // Ensure that the Tripal Content Admin permission bypasses the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    return AccessResult::allowedIfHasPermission($account, 'add tripal content entities');
  }

}
