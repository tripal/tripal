<?php

namespace Drupal\tripal;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Controlled Vocabulary Term entity.
 *
 * @see \Drupal\tripal\Entity\TripalTerm.
 */
class TripalTermAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\tripal\Entity\TripalTermInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished controlled vocabulary term entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published controlled vocabulary term entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit controlled vocabulary term entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete controlled vocabulary term entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add controlled vocabulary term entities');
  }

}
