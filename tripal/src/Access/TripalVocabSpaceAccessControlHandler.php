<?php

namespace Drupal\tripal\Access;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Tripal Vocabulary IDSpace entity.
 *
 * @see \Drupal\tripal\Entity\TripalVocabSpace.
 */
class TripalVocabSpaceAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\tripal\Entity\TripalVocabSpaceInterface $entity */

    switch ($operation) {

      case 'view':

        return AccessResult::allowedIfHasPermission($account, 'view tripal vocabulary idspace entities');

      case 'update':

        return AccessResult::allowedIfHasPermission($account, 'edit tripal vocabulary idspace entities');

      case 'delete':

        return AccessResult::allowedIfHasPermission($account, 'delete tripal vocabulary idspace entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add tripal vocabulary idspace entities');
  }


}
